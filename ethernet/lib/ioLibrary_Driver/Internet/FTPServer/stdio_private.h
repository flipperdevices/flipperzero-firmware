/* Copyright (c) 2002, Joerg Wunsch
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:

   * Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   * Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
   * Neither the name of the copyright holders nor the names of
     contributors may be used to endorse or promote products derived
     from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

/* $Id: stdio_private.h,v 1.6 2003/01/07 22:17:24 joerg_wunsch Exp $ */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

struct __file {
	char	*buf;		/* buffer pointer */
	unsigned char unget;	/* ungetc() buffer */
	uint8_t	flags;		/* flags, see below */
#define __SRD	0x0001		/* OK to read */
#define __SWR	0x0002		/* OK to write */
#define __SSTR	0x0004		/* this is an sprintf/snprintf string */
#define __SPGM	0x0008		/* fmt string is in progmem */
#define __SERR	0x0010		/* found error */
#define __SEOF	0x0020		/* found EOF */
#define __SUNGET 0x040		/* ungetc() happened */
#if 0
/* possible future extensions, will require uint16_t flags */
#define __SRW	0x0080		/* open for reading & writing */
#define __SLBF	0x0100		/* line buffered */
#define __SNBF	0x0200		/* unbuffered */
#define __SMBF	0x0400		/* buf is from malloc */
#endif
	int	size;		/* size of buffer */
	int	len;		/* characters read or written so far */
	int	(*put)(char);	/* function to write one char to device */
	int	(*get)(void);	/* function to read one char from device */
};

/* values for PRINTF_LEVEL */
#define PRINTF_MIN 1
#define PRINTF_STD 2
#define PRINTF_FLT 3

/* values for SCANF_LEVEL */
#define SCANF_MIN 1
#define SCANF_STD 2
#define SCANF_FLT 3

#ifdef __cplusplus
}
#endif
