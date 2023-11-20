/*
 * Copyright (c) 2003, 2004 X/IO Labs, xiolabs.com.
 * Copyright (c) 2003, 2004, 2005 Lev Walkin <vlm@lionet.info>.
 * 	All rights reserved.
 * Redistribution and modifications are permitted subject to BSD license.
 */
#include <asn_system.h>
#include <xer_support.h>

/* Parser states */
typedef enum {
	ST_TEXT,
	ST_TAG_START,
	ST_TAG_BODY,
	ST_TAG_QUOTE_WAIT,
	ST_TAG_QUOTED_STRING,
	ST_TAG_UNQUOTED_STRING,
	ST_COMMENT_WAIT_DASH1,	/* "<!--"[1] */
	ST_COMMENT_WAIT_DASH2,	/* "<!--"[2] */
	ST_COMMENT,
	ST_COMMENT_CLO_DASH2,	/* "-->"[0] */
	ST_COMMENT_CLO_RT	/* "-->"[1] */
} pstate_e;

static const int
_charclass[256] = {
	0,0,0,0,0,0,0,0, 0,1,1,0,1,1,0,0,
	0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	1,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	2,2,2,2,2,2,2,2, 2,2,0,0,0,0,0,0,	/* 01234567 89       */
	0,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,	/*  ABCDEFG HIJKLMNO */
	3,3,3,3,3,3,3,3, 3,3,3,0,0,0,0,0,	/* PQRSTUVW XYZ      */
	0,3,3,3,3,3,3,3, 3,3,3,3,3,3,3,3,	/*  abcdefg hijklmno */
	3,3,3,3,3,3,3,3, 3,3,3,0,0,0,0,0	/* pqrstuvw xyz      */
};
#define WHITESPACE(c)	(_charclass[(unsigned char)(c)] == 1)
#define ALNUM(c)	(_charclass[(unsigned char)(c)] >= 2)
#define ALPHA(c)	(_charclass[(unsigned char)(c)] == 3)

/* Aliases for characters, ASCII/UTF-8 */
#define	EXCLAM	0x21	/* '!' */
#define	CQUOTE	0x22	/* '"' */
#define	CDASH	0x2d	/* '-' */
#define	CSLASH	0x2f	/* '/' */
#define	LANGLE	0x3c	/* '<' */
#define	CEQUAL	0x3d	/* '=' */
#define	RANGLE	0x3e	/* '>' */
#define	CQUEST	0x3f	/* '?' */

/* Invoke token callback */
#define	TOKEN_CB_CALL(type, _ns, _current_too, _final) do {	\
		int _ret;					\
		pstate_e ns  = _ns;				\
		ssize_t _sz = (p - chunk_start) + _current_too;	\
		if (!_sz) {					\
			/* Shortcut */				\
			state = _ns;				\
			break;					\
		}						\
		_ret = cb(type, chunk_start, _sz, key);		\
		if(_ret < _sz) {				\
			if(_current_too && _ret == -1)		\
				state = ns;			\
			goto finish;				\
		}						\
		chunk_start = p + _current_too;			\
		state = ns;					\
	} while(0)

#define TOKEN_CB(_type, _ns, _current_too)			\
	TOKEN_CB_CALL(_type, _ns, _current_too, 0)

#define PXML_TAG_FINAL_CHUNK_TYPE      PXML_TAG_END
#define PXML_COMMENT_FINAL_CHUNK_TYPE  PXML_COMMENT_END

#define TOKEN_CB_FINAL(_type, _ns, _current_too)		\
	TOKEN_CB_CALL( _type ## _FINAL_CHUNK_TYPE , _ns, _current_too, 1)

/*
 * Parser itself
 */
ssize_t pxml_parse(int *stateContext, const void *xmlbuf, size_t size, pxml_callback_f *cb, void *key) {
	pstate_e state = (pstate_e)*stateContext;
	const char *chunk_start = (const char *)xmlbuf;
	const char *p = chunk_start;
	const char *end = p + size;

	for(; p < end; p++) {
	  int C = *(const unsigned char *)p;
	  switch(state) {
	  case ST_TEXT:
		/*
		 * Initial state: we're in the middle of some text,
		 * or just have started.
		 */
		if (C == LANGLE) 
			/* We're now in the tag, probably */
			TOKEN_CB(PXML_TEXT, ST_TAG_START, 0);
		break;
	  case ST_TAG_START:
		if (ALPHA(C) || (C == CSLASH))
			state = ST_TAG_BODY;
		else if (C == EXCLAM)
			state = ST_COMMENT_WAIT_DASH1;
		else 
			/*
			 * Not characters and not whitespace.
			 * Must be something like "3 < 4".
			 */
			TOKEN_CB(PXML_TEXT, ST_TEXT, 1);/* Flush as data */
		break;
	  case ST_TAG_BODY:
		switch(C) {
		case RANGLE:
			/* End of the tag */
			TOKEN_CB_FINAL(PXML_TAG, ST_TEXT, 1);
			break;
		case LANGLE:
			/*
			 * The previous tag wasn't completed, but still
			 * recognized as valid. (Mozilla-compatible)
			 */
			TOKEN_CB_FINAL(PXML_TAG, ST_TAG_START, 0);	
			break;
		case CEQUAL:
			state = ST_TAG_QUOTE_WAIT;
			break;
		}
		break;
	  case ST_TAG_QUOTE_WAIT:
		/*
		 * State after the equal sign ("=") in the tag.
		 */
		switch(C) {
		case CQUOTE:
			state = ST_TAG_QUOTED_STRING;
			break;
		case RANGLE:
			/* End of the tag */
			TOKEN_CB_FINAL(PXML_TAG, ST_TEXT, 1);
			break;
		default:
			if(!WHITESPACE(C))
				/* Unquoted string value */
				state = ST_TAG_UNQUOTED_STRING;
		}
		break;
	  case ST_TAG_QUOTED_STRING:
		/*
		 * Tag attribute's string value in quotes.
		 */
		if(C == CQUOTE) {
			/* Return back to the tag state */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_TAG_UNQUOTED_STRING:
		if(C == RANGLE) {
			/* End of the tag */
			TOKEN_CB_FINAL(PXML_TAG, ST_TEXT, 1);
		} else if(WHITESPACE(C)) {
			/* Return back to the tag state */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_COMMENT_WAIT_DASH1:
		if(C == CDASH) {
			state = ST_COMMENT_WAIT_DASH2;
		} else {
			/* Some ordinary tag. */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_COMMENT_WAIT_DASH2:
		if(C == CDASH) {
			/* Seen "<--" */
			state = ST_COMMENT;
		} else {
			/* Some ordinary tag */
			state = ST_TAG_BODY;
		}
		break;
	  case ST_COMMENT:
		if(C == CDASH) {
			state = ST_COMMENT_CLO_DASH2;
		}
		break;
	  case ST_COMMENT_CLO_DASH2:
		if(C == CDASH) {
			state = ST_COMMENT_CLO_RT;
		} else {
			/* This is not an end of a comment */
			state = ST_COMMENT;
		}
		break;
	  case ST_COMMENT_CLO_RT:
		if(C == RANGLE) {
			TOKEN_CB_FINAL(PXML_COMMENT, ST_TEXT, 1);
		} else if(C == CDASH) {
			/* Maintain current state, still waiting for '>' */
		} else {
			state = ST_COMMENT;
		}
		break;
	  } /* switch(*ptr) */
	} /* for() */

	/*
	 * Flush the partially processed chunk, state permitting.
	 */
	if(p - chunk_start) {
		switch (state) {
		case ST_COMMENT:
			TOKEN_CB(PXML_COMMENT, state, 0);
			break;
		case ST_TEXT:
			TOKEN_CB(PXML_TEXT, state, 0);
			break;
		default: break;	/* a no-op */
		}
	}

finish:
	*stateContext = (int)state;
	return chunk_start - (const char *)xmlbuf;
}

