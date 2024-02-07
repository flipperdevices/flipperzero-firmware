/* @(#) pf_guts.h 98/01/28 1.4 */
#ifndef _pf_guts_h
#define _pf_guts_h

/***************************************************************
** Include file for PForth, a Forth based on 'C'
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
***************************************************************/

/*
** PFORTH_VERSION changes when PForth is modified and released.
** See README file for version info.
*/
#define PFORTH_VERSION_CODE 30
#define PFORTH_VERSION_NAME "2.0.1"

/*
** PFORTH_FILE_VERSION changes when incompatible changes are made
** in the ".dic" file format.
**
** FV3 - 950225 - Use ABS_TO_CODEREL for CodePtr. See file "pf_save.c".
** FV4 - 950309 - Added NameSize and CodeSize to pfSaveForth().
** FV5 - 950316 - Added Floats and reserved words.
** FV6 - 961213 - Added ID_LOCAL_PLUSSTORE, ID_COLON_P, etc.
** FV7 - 971203 - Added ID_FILL, (1LOCAL@),  etc., ran out of reserved, resorted.
** FV8 - 980818 - Added Endian flag.
** FV9 - 20100503 - Added support for 64-bit CELL.
** FV10 - 20170103 - Added ID_FILE_FLUSH ID_FILE_RENAME ID_FILE_RESIZE
*/
#define PF_FILE_VERSION (10)   /* Bump this whenever primitives added. */
#define PF_EARLIEST_FILE_VERSION (9)  /* earliest one still compatible */

/***************************************************************
** Sizes and other constants
***************************************************************/

#define TIB_SIZE (256)

#ifndef FALSE
    #define FALSE (0)
#endif
#ifndef TRUE
    #define TRUE (1)
#endif

#define FFALSE (0)
#define FTRUE (-1)
#define BLANK (' ')

#define FLAG_PRECEDENCE (0x80)
#define FLAG_IMMEDIATE  (0x40)
#define FLAG_SMUDGE     (0x20)
#define MASK_NAME_SIZE  (0x1F)

/* Debug TRACE flags */
#define TRACE_INNER     (0x0002)
#define TRACE_COMPILE   (0x0004)
#define TRACE_SPECIAL   (0x0008)

/* Numeric types returned by NUMBER? */
#define NUM_TYPE_BAD    (0)
#define NUM_TYPE_SINGLE (1)
#define NUM_TYPE_DOUBLE (2)
#define NUM_TYPE_FLOAT  (3)

#define CREATE_BODY_OFFSET  (3*sizeof(cell_t))

/***************************************************************
** Primitive Token IDS
** Do NOT change the order of these IDs or dictionary files will break!
***************************************************************/
enum cforth_primitive_ids
{
    ID_EXIT = 0,  /* ID_EXIT must always be zero. */
/* Do NOT change the order of these IDs or dictionary files will break! */
    ID_1MINUS,
    ID_1PLUS,
    ID_2DUP,
    ID_2LITERAL,
    ID_2LITERAL_P,
    ID_2MINUS,
    ID_2OVER,
    ID_2PLUS,
    ID_2SWAP,
    ID_2_R_FETCH,
    ID_2_R_FROM,
    ID_2_TO_R,
    ID_ACCEPT_P,
    ID_ALITERAL,
    ID_ALITERAL_P,
    ID_ALLOCATE,
    ID_AND,
    ID_ARSHIFT,
    ID_BAIL,
    ID_BODY_OFFSET,
    ID_BRANCH,
    ID_BYE,
    ID_CALL_C,
    ID_CFETCH,
    ID_CMOVE,
    ID_CMOVE_UP,
    ID_COLON,
    ID_COLON_P,
    ID_COMPARE,
    ID_COMP_EQUAL,
    ID_COMP_GREATERTHAN,
    ID_COMP_LESSTHAN,
    ID_COMP_NOT_EQUAL,
    ID_COMP_U_GREATERTHAN,
    ID_COMP_U_LESSTHAN,
    ID_COMP_ZERO_EQUAL,
    ID_COMP_ZERO_GREATERTHAN,
    ID_COMP_ZERO_LESSTHAN,
    ID_COMP_ZERO_NOT_EQUAL,
    ID_CR,
    ID_CREATE,
    ID_CREATE_P,
    ID_CSTORE,
    ID_DEFER,
    ID_DEFER_P,
    ID_DEPTH,
    ID_DIVIDE,
    ID_DOT,
    ID_DOTS,
    ID_DO_P,
    ID_DROP,
    ID_DUMP,
    ID_DUP,
    ID_D_MINUS,
    ID_D_MTIMES,
    ID_D_MUSMOD,
    ID_D_PLUS,
    ID_D_UMSMOD,
    ID_D_UMTIMES,
    ID_EMIT,
    ID_EMIT_P,
    ID_EOL,
    ID_ERRORQ_P,
    ID_EXECUTE,
    ID_FETCH,
    ID_FILE_CLOSE,
    ID_FILE_CREATE,
    ID_FILE_OPEN,
    ID_FILE_POSITION,
    ID_FILE_READ,
    ID_FILE_REPOSITION,
    ID_FILE_RO,
    ID_FILE_RW,
    ID_FILE_SIZE,
    ID_FILE_WRITE,
    ID_FILL,
    ID_FIND,
    ID_FINDNFA,
    ID_FLUSHEMIT,
    ID_FREE,
    ID_HERE,
    ID_NUMBERQ_P,
    ID_I,
    ID_INCLUDE_FILE,
    ID_J,
    ID_KEY,
    ID_LEAVE_P,
    ID_LITERAL,
    ID_LITERAL_P,
    ID_LOADSYS,
    ID_LOCAL_COMPILER,
    ID_LOCAL_ENTRY,
    ID_LOCAL_EXIT,
    ID_LOCAL_FETCH,
    ID_LOCAL_FETCH_1,
    ID_LOCAL_FETCH_2,
    ID_LOCAL_FETCH_3,
    ID_LOCAL_FETCH_4,
    ID_LOCAL_FETCH_5,
    ID_LOCAL_FETCH_6,
    ID_LOCAL_FETCH_7,
    ID_LOCAL_FETCH_8,
    ID_LOCAL_PLUSSTORE,
    ID_LOCAL_STORE,
    ID_LOCAL_STORE_1,
    ID_LOCAL_STORE_2,
    ID_LOCAL_STORE_3,
    ID_LOCAL_STORE_4,
    ID_LOCAL_STORE_5,
    ID_LOCAL_STORE_6,
    ID_LOCAL_STORE_7,
    ID_LOCAL_STORE_8,
    ID_LOOP_P,
    ID_LSHIFT,
    ID_MAX,
    ID_MIN,
    ID_MINUS,
    ID_NAME_TO_PREVIOUS,
    ID_NAME_TO_TOKEN,
    ID_NOOP,
    ID_NUMBERQ,
    ID_OR,
    ID_OVER,
    ID_PICK,
    ID_PLUS,
    ID_PLUSLOOP_P,
    ID_PLUS_STORE,
    ID_QDO_P,
    ID_QDUP,
    ID_QTERMINAL,
    ID_QUIT_P,
    ID_REFILL,
    ID_RESIZE,
    ID_SOURCE_LINE_NUMBER_FETCH, /* used to be ID_RESTORE_INPUT */
    ID_ROLL,
    ID_ROT,
    ID_RP_FETCH,
    ID_RP_STORE,
    ID_RSHIFT,
    ID_R_DROP,
    ID_R_FETCH,
    ID_R_FROM,
    ID_SAVE_FORTH_P,
    ID_SOURCE_LINE_NUMBER_STORE, /* used to be ID_SAVE_INPUT */
    ID_SCAN,
    ID_SEMICOLON,
    ID_SKIP,
    ID_SOURCE,
    ID_SOURCE_ID,
    ID_SOURCE_ID_POP,
    ID_SOURCE_ID_PUSH,
    ID_SOURCE_SET,
    ID_SP_FETCH,
    ID_SP_STORE,
    ID_STORE,
    ID_SWAP,
    ID_TEST1,
    ID_TEST2,
    ID_TEST3,
    ID_TICK,
    ID_TIMES,
    ID_TO_R,
    ID_TYPE,
    ID_TYPE_P,
    ID_VAR_BASE,
    ID_VAR_CODE_BASE,
    ID_VAR_CODE_LIMIT,
    ID_VAR_CONTEXT,
    ID_VAR_DP,
    ID_VAR_ECHO,
    ID_VAR_HEADERS_BASE,
    ID_VAR_HEADERS_LIMIT,
    ID_VAR_HEADERS_PTR,
    ID_VAR_NUM_TIB,
    ID_VAR_OUT,
    ID_VAR_RETURN_CODE,
    ID_VAR_SOURCE_ID,
    ID_VAR_STATE,
    ID_VAR_TO_IN,
    ID_VAR_TRACE_FLAGS,
    ID_VAR_TRACE_LEVEL,
    ID_VAR_TRACE_STACK,
    ID_VLIST,
    ID_WORD,
    ID_WORD_FETCH,
    ID_WORD_STORE,
    ID_XOR,
    ID_ZERO_BRANCH,
    ID_CATCH,
    ID_THROW,
    ID_INTERPRET,
    ID_FILE_WO,
    ID_FILE_BIN,
    /* Added to support 64 bit operation. */
    ID_CELL,
    ID_CELLS,
    /* DELETE-FILE */
    ID_FILE_DELETE,
    ID_FILE_FLUSH,     /* FLUSH-FILE */
    ID_FILE_RENAME,    /* (RENAME-FILE) */
    ID_FILE_RESIZE,    /* RESIZE-FILE */
    ID_SLEEP_P,        /* (SLEEP) V2.0.0 */
    ID_VAR_BYE_CODE,   /* BYE-CODE */
    ID_VERSION_CODE,
/* If you add a word here, take away one reserved word below. */
#ifdef PF_SUPPORT_FP
/* Only reserve space if we are adding FP so that we can detect
** unsupported primitives when loading dictionary.
*/
    ID_RESERVED03,
    ID_RESERVED04,
    ID_RESERVED05,
    ID_RESERVED06,
    ID_RESERVED07,
    ID_RESERVED08,
    ID_RESERVED09,
    ID_FP_D_TO_F,
    ID_FP_FSTORE,
    ID_FP_FTIMES,
    ID_FP_FPLUS,
    ID_FP_FMINUS,
    ID_FP_FSLASH,
    ID_FP_F_ZERO_LESS_THAN,
    ID_FP_F_ZERO_EQUALS,
    ID_FP_F_LESS_THAN,
    ID_FP_F_TO_D,
    ID_FP_FFETCH,
    ID_FP_FDEPTH,
    ID_FP_FDROP,
    ID_FP_FDUP,
    ID_FP_FLITERAL,
    ID_FP_FLITERAL_P,
    ID_FP_FLOAT_PLUS,
    ID_FP_FLOATS,
    ID_FP_FLOOR,
    ID_FP_FMAX,
    ID_FP_FMIN,
    ID_FP_FNEGATE,
    ID_FP_FOVER,
    ID_FP_FROT,
    ID_FP_FROUND,
    ID_FP_FSWAP,
    ID_FP_FSTAR_STAR,
    ID_FP_FABS,
    ID_FP_FACOS,
    ID_FP_FACOSH,
    ID_FP_FALOG,
    ID_FP_FASIN,
    ID_FP_FASINH,
    ID_FP_FATAN,
    ID_FP_FATAN2,
    ID_FP_FATANH,
    ID_FP_FCOS,
    ID_FP_FCOSH,
    ID_FP_FLN,
    ID_FP_FLNP1,
    ID_FP_FLOG,
    ID_FP_FSIN,
    ID_FP_FSINCOS,
    ID_FP_FSINH,
    ID_FP_FSQRT,
    ID_FP_FTAN,
    ID_FP_FTANH,
    ID_FP_FPICK,
#endif
/* Add new IDs by replacing reserved IDs or extending FP routines. */
/* Do NOT change the order of these IDs or dictionary files will break! */
    NUM_PRIMITIVES     /* This must always be LAST */
};



/***************************************************************
** THROW Codes
***************************************************************/
/* ANSI standard definitions needed by pForth */
#define THROW_ABORT            (-1)
#define THROW_ABORT_QUOTE      (-2)
#define THROW_STACK_OVERFLOW   (-3)
#define THROW_STACK_UNDERFLOW  (-4)
#define THROW_UNDEFINED_WORD  (-13)
#define THROW_EXECUTING       (-14)
#define THROW_PAIRS           (-22)
#define THROW_FLOAT_STACK_UNDERFLOW  ( -45)
#define THROW_QUIT            (-56)
#define THROW_FLUSH_FILE      (-68)
#define THROW_RESIZE_FILE     (-74)

/* THROW codes unique to pForth */
#define THROW_BYE            (-256) /* Exit program. */
#define THROW_SEMICOLON      (-257) /* Error detected at ; */
#define THROW_DEFERRED       (-258) /* Not a deferred word. Used in system.fth */

/***************************************************************
** Structures
***************************************************************/

typedef struct pfTaskData_s
{
    cell_t   *td_StackPtr;       /* Primary data stack */
    cell_t   *td_StackBase;
    cell_t   *td_StackLimit;
    cell_t   *td_ReturnPtr;      /* Return stack */
    cell_t   *td_ReturnBase;
    cell_t   *td_ReturnLimit;
#ifdef PF_SUPPORT_FP
    PF_FLOAT  *td_FloatStackPtr;
    PF_FLOAT  *td_FloatStackBase;
    PF_FLOAT  *td_FloatStackLimit;
#endif
    cell_t   *td_InsPtr;          /* Instruction pointer, "PC" */
    FileStream   *td_InputStream;
/* Terminal. */
    char    td_TIB[TIB_SIZE];   /* Buffer for terminal input. */
    cell_t    td_IN;              /* Index into Source */
    cell_t    td_SourceNum;       /* #TIB after REFILL */
    char   *td_SourcePtr;       /* Pointer to TIB or other source. */
    cell_t   td_LineNumber;      /* Incremented on every refill. */
    cell_t    td_OUT;             /* Current output column. */
    char    scratch[TIB_SIZE];
} pfTaskData_t;

typedef struct pfNode
{
    struct pfNode *n_Next;
    struct pfNode *n_Prev;
} pfNode;

/* Structure of header entry in dictionary. These will be stored in dictionary specific endian format*/
typedef struct cfNameLinks
{
    cell_t       cfnl_PreviousName;   /* name relative address of previous */
    ExecToken  cfnl_ExecToken;      /* Execution token for word. */
/* Followed by variable length name field. */
} cfNameLinks;

#define PF_DICF_ALLOCATED_SEGMENTS  ( 0x0001)
#define PF_DICF_MOVED_SEGMENTS  ( 0x0002)
typedef struct pfDictionary_s
{
    pfNode  dic_Node;
    ucell_t  dic_Flags;
/* Headers contain pointers to names and dictionary. */

    ucell_t dic_HeaderBaseUnaligned;

    ucell_t dic_HeaderBase;
    ucell_t dic_HeaderPtr;
    ucell_t dic_HeaderLimit;
/* Code segment contains tokenized code and data. */
    ucell_t dic_CodeBaseUnaligned;
    ucell_t dic_CodeBase;
    union
    {
        cell_t  *Cell;
        uint8_t *Byte;
    } dic_CodePtr;
    ucell_t dic_CodeLimit;
} pfDictionary_t;

/* Save state of include when nesting files. */
typedef struct IncludeFrame
{
    FileStream   *inf_FileID;
    cell_t         inf_LineNumber;
    cell_t         inf_SourceNum;
    cell_t         inf_IN;
    char          inf_SaveTIB[TIB_SIZE];
} IncludeFrame;

#define MAX_INCLUDE_DEPTH (16)

/***************************************************************
** Prototypes
***************************************************************/

// #ifdef __cplusplus
// extern "C" {
// #endif

// ThrowCode pfCatch( ExecToken XT, pfTaskData_t *gCurrentTask );

// #ifdef __cplusplus
// }
// #endif

/***************************************************************
** External Globals
***************************************************************/
//extern pfTaskData_t *gCurrentTask;
extern pfDictionary_t *gCurrentDictionary;
//extern char          gScratch[TIB_SIZE];
#define gScratch (gCurrentTask->scratch)
extern cell_t         gNumPrimitives;

extern ExecToken     gLocalCompiler_XT;      /* CFA of (LOCAL) compiler. */
extern ExecToken     gNumberQ_XT;         /* XT of NUMBER? */
extern ExecToken     gQuitP_XT;           /* XT of (QUIT) */
extern ExecToken     gAcceptP_XT;         /* XT of ACCEPT */

#define DEPTH_AT_COLON_INVALID (-100)
extern cell_t         gDepthAtColon;

/* Global variables. */
extern cell_t        gVarContext;    /* Points to last name field. */
extern cell_t        gVarState;      /* 1 if compiling. */
extern cell_t        gVarBase;       /* Numeric Base. */
extern cell_t        gVarByeCode;    /* BYE-CODE returned on exit */
extern cell_t        gVarEcho;       /* Echo input from file. */
extern cell_t        gVarEchoAccept; /* Echo input from ACCEPT. */
extern cell_t        gVarTraceLevel;
extern cell_t        gVarTraceStack;
extern cell_t        gVarTraceFlags;
extern cell_t        gVarQuiet;      /* Suppress unnecessary messages, OK, etc. */
extern cell_t        gVarReturnCode; /* Returned to caller of Forth, eg. UNIX shell. */

extern IncludeFrame  gIncludeStack[MAX_INCLUDE_DEPTH];
extern cell_t         gIncludeIndex;
/***************************************************************
** Macros
***************************************************************/


/* Endian specific macros for creating target dictionaries for machines with

** different endian-ness.

*/

#if defined(PF_BIG_ENDIAN_DIC)

#define WRITE_FLOAT_DIC             WriteFloatBigEndian
#define WRITE_CELL_DIC(addr,data)   WriteCellBigEndian((uint8_t *)(addr),(ucell_t)(data))
#define WRITE_SHORT_DIC(addr,data)  Write16BigEndian((uint8_t *)(addr),(uint16_t)(data))
#define READ_FLOAT_DIC              ReadFloatBigEndian
#define READ_CELL_DIC(addr)         ReadCellBigEndian((const uint8_t *)(addr))
#define READ_SHORT_DIC(addr)        Read16BigEndian((const uint8_t *)(addr))

#elif defined(PF_LITTLE_ENDIAN_DIC)

#define WRITE_FLOAT_DIC             WriteFloatLittleEndian
#define WRITE_CELL_DIC(addr,data)   WriteCellLittleEndian((uint8_t *)(addr),(ucell_t)(data))
#define WRITE_SHORT_DIC(addr,data)  Write16LittleEndian((uint8_t *)(addr),(uint16_t)(data))
#define READ_FLOAT_DIC              ReadFloatLittleEndian
#define READ_CELL_DIC(addr)         ReadCellLittleEndian((const uint8_t *)(addr))
#define READ_SHORT_DIC(addr)        Read16LittleEndian((const uint8_t *)(addr))

#else

#define WRITE_FLOAT_DIC(addr,data)  { *((PF_FLOAT *)(addr)) = (PF_FLOAT)(data); }
#define WRITE_CELL_DIC(addr,data)   { *((cell_t *)(addr)) = (cell_t)(data); }
#define WRITE_SHORT_DIC(addr,data)  { *((int16_t *)(addr)) = (int16_t)(data); }
#define READ_FLOAT_DIC(addr)        ( *((PF_FLOAT *)(addr)) )
#define READ_CELL_DIC(addr)         ( *((const ucell_t *)(addr)) )
#define READ_SHORT_DIC(addr)        ( *((const uint16_t *)(addr)) )

#endif


#define HEADER_HERE (gCurrentDictionary->dic_HeaderPtr.Cell)
#define CODE_HERE (gCurrentDictionary->dic_CodePtr.Cell)
#define CODE_COMMA( N ) WRITE_CELL_DIC(CODE_HERE++,(N))
#define NAME_BASE (gCurrentDictionary->dic_HeaderBase)
#define CODE_BASE (gCurrentDictionary->dic_CodeBase)
#define NAME_SIZE (gCurrentDictionary->dic_HeaderLimit - gCurrentDictionary->dic_HeaderBase)
#define CODE_SIZE (gCurrentDictionary->dic_CodeLimit - gCurrentDictionary->dic_CodeBase)

#define IN_CODE_DIC(addr) ( ( ((uint8_t *)(addr)) >= gCurrentDictionary->dic_CodeBase)   && ( ((uint8_t *)(addr)) < gCurrentDictionary->dic_CodeLimit) )

#define IN_NAME_DIC(addr) ( ( ((uint8_t *)(addr)) >= gCurrentDictionary->dic_HeaderBase) && ( ((uint8_t *)(addr)) < gCurrentDictionary->dic_HeaderLimit) )
#define IN_DICS(addr) (IN_CODE_DIC(addr) || IN_NAME_DIC(addr))

/* Address conversion */
#define ABS_TO_NAMEREL( a ) ((cell_t)  (((ucell_t) a) - NAME_BASE ))
#define ABS_TO_CODEREL( a ) ((cell_t)  (((ucell_t) a) - CODE_BASE ))
#define NAMEREL_TO_ABS( a ) ((ucell_t) (((cell_t) a) + NAME_BASE))
#define CODEREL_TO_ABS( a ) ((ucell_t) (((cell_t) a) + CODE_BASE))

/* The check for >0 is only needed for CLONE testing. !!! */
#define IsTokenPrimitive(xt) ((xt<gNumPrimitives) && (xt>=0))

#define FREE_VAR(v) { if (v) { pfFreeMem((void *)(v)); v = 0; } }

#define DATA_STACK_DEPTH (gCurrentTask->td_StackBase - gCurrentTask->td_StackPtr)
#define DROP_DATA_STACK (gCurrentTask->td_StackPtr++)
#define POP_DATA_STACK (*gCurrentTask->td_StackPtr++)
#define PUSH_DATA_STACK(x) {*(--(gCurrentTask->td_StackPtr)) = (cell_t) x; }

/* Force Quad alignment. */
#define QUADUP(x) (((x)+3)&~3)

#ifndef MIN
#define MIN(a,b)  ( ((a)<(b)) ? (a) : (b) )
#endif
#ifndef MAX
#define MAX(a,b)  ( ((a)>(b)) ? (a) : (b) )
#endif

#ifndef TOUCH
    #define TOUCH(argument) ((void)argument)
#endif

/***************************************************************
** I/O related macros
***************************************************************/

#define EMIT(c)  ioEmit(c)
#define EMIT_CR  EMIT('\n');

#define MSG(cs)   pfMessage(cs)
#define ERR(x)    MSG(x)

#define DBUG(x)  /* PRT(x) */
#define DBUGX(x) /* DBUG(x) */

#define MSG_NUM_D(msg,num) { MSG(msg); ffDot((cell_t) num); EMIT_CR; }
#define MSG_NUM_H(msg,num) { MSG(msg); ffDotHex((cell_t) num); EMIT_CR; }

#define DBUG_NUM_D(msg,num) { pfDebugMessage(msg); pfDebugPrintDecimalNumber((cell_t) num); pfDebugMessage("\n"); }

#endif  /* _pf_guts_h */
