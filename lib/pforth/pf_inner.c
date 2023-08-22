/* @(#) pf_inner.c 98/03/16 1.7 */
/***************************************************************
** Inner Interpreter for Forth based on 'C'
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
****************************************************************
**
** 940502 PLB Creation.
** 940505 PLB More macros.
** 940509 PLB Moved all stack stuff into pfCatch.
** 941014 PLB Converted to flat secondary strusture.
** 941027 rdg added casts to ID_SP_FETCH, ID_RP_FETCH,
**             and ID_HERE for armcc
** 941130 PLB Made w@ unsigned
**
***************************************************************/

#include "pf_all.h"

#if defined(WIN32) && !defined(__MINGW32__)
#include <crtdbg.h>
#endif

#define SYSTEM_LOAD_FILE "system.fth"

/***************************************************************
** Macros for data stack access.
** TOS is cached in a register in pfCatch.
***************************************************************/

#define STKPTR   (DataStackPtr)
#define M_POP    (*(STKPTR++))
#define M_PUSH(n) {*(--(STKPTR)) = (cell_t) (n);}
#define M_STACK(n) (STKPTR[n])

#define TOS      (TopOfStack)
#define PUSH_TOS M_PUSH(TOS)
#define M_DUP    PUSH_TOS;
#define M_DROP   { TOS = M_POP; }

#define ASCII_EOT   (0x04)

/***************************************************************
** Macros for Floating Point stack access.
***************************************************************/
#ifdef PF_SUPPORT_FP
#define FP_STKPTR   (FloatStackPtr)
#define M_FP_SPZERO (gCurrentTask->td_FloatStackBase)
#define M_FP_POP    (*(FP_STKPTR++))
#define M_FP_PUSH(n) {*(--(FP_STKPTR)) = (PF_FLOAT) (n);}
#define M_FP_STACK(n) (FP_STKPTR[n])

#define FP_TOS      (fpTopOfStack)
#define PUSH_FP_TOS M_FP_PUSH(FP_TOS)
#define M_FP_DUP    PUSH_FP_TOS;
#define M_FP_DROP   { FP_TOS = M_FP_POP; }
#endif

/***************************************************************
** Macros for return stack access.
***************************************************************/

#define TORPTR (ReturnStackPtr)
#define M_R_DROP {TORPTR++;}
#define M_R_POP (*(TORPTR++))
#define M_R_PICK(n) (TORPTR[n])
#define M_R_PUSH(n) {*(--(TORPTR)) = (cell_t) (n);}

/***************************************************************
** Misc Forth macros
***************************************************************/

#define M_BRANCH   { InsPtr = (cell_t *) (((uint8_t *) InsPtr) + READ_CELL_DIC(InsPtr)); }

/* Cache top of data stack like in JForth. */
#ifdef PF_SUPPORT_FP
#define LOAD_REGISTERS \
    { \
        STKPTR = gCurrentTask->td_StackPtr; \
        TOS = M_POP; \
        FP_STKPTR = gCurrentTask->td_FloatStackPtr; \
        FP_TOS = M_FP_POP; \
        TORPTR = gCurrentTask->td_ReturnPtr; \
     }

#define SAVE_REGISTERS \
    { \
        gCurrentTask->td_ReturnPtr = TORPTR; \
        M_PUSH( TOS ); \
        gCurrentTask->td_StackPtr = STKPTR; \
        M_FP_PUSH( FP_TOS ); \
        gCurrentTask->td_FloatStackPtr = FP_STKPTR; \
     }

#else
/* Cache top of data stack like in JForth. */
#define LOAD_REGISTERS \
    { \
        STKPTR = gCurrentTask->td_StackPtr; \
        TOS = M_POP; \
        TORPTR = gCurrentTask->td_ReturnPtr; \
     }

#define SAVE_REGISTERS \
    { \
        gCurrentTask->td_ReturnPtr = TORPTR; \
        M_PUSH( TOS ); \
        gCurrentTask->td_StackPtr = STKPTR; \
     }
#endif

#define M_DOTS \
    SAVE_REGISTERS; \
    ffDotS( ); \
    LOAD_REGISTERS;

#define DO_VAR(varname) { PUSH_TOS; TOS = (cell_t) &varname; }

#ifdef PF_SUPPORT_FP
#define M_THROW(err) \
    { \
        ExceptionReturnCode = (ThrowCode)(err); \
        TORPTR = InitialReturnStack; /* Will cause return to 'C' */ \
        STKPTR = InitialDataStack; \
        FP_STKPTR = InitialFloatStack; \
    }
#else
#define M_THROW(err) \
    { \
        ExceptionReturnCode = (err); \
        TORPTR = InitialReturnStack; /* Will cause return to 'C' */ \
        STKPTR = InitialDataStack; \
    }
#endif

/***************************************************************
** Other macros
***************************************************************/

#define BINARY_OP( op ) { TOS = M_POP op TOS; }
#define endcase break

#if defined(PF_NO_SHELL) || !defined(PF_SUPPORT_TRACE)
    #define TRACENAMES /* no names */
#else
/* Display name of executing routine. */
static void TraceNames( ExecToken Token, cell_t Level )
{
    char *DebugName;
    cell_t i;

    if( ffTokenToName( Token, &DebugName ) )
    {
        cell_t NumSpaces;
        if( gCurrentTask->td_OUT > 0 ) EMIT_CR;
        EMIT( '>' );
        for( i=0; i<Level; i++ )
        {
            MSG( "  " );
        }
        TypeName( DebugName );
/* Space out to column N then .S */
        NumSpaces = 30 - gCurrentTask->td_OUT;
        for( i=0; i < NumSpaces; i++ )
        {
            EMIT( ' ' );
        }
        ffDotS();
/* No longer needed?        gCurrentTask->td_OUT = 0; */ /* !!! Hack for ffDotS() */

    }
    else
    {
        MSG_NUM_H("Couldn't find Name for ", Token);
    }
}

#define TRACENAMES \
    if( (gVarTraceLevel > Level) ) \
    { SAVE_REGISTERS; TraceNames( Token, Level ); LOAD_REGISTERS; }
#endif /* PF_NO_SHELL */

/* Use local copy of CODE_BASE for speed. */
#define LOCAL_CODEREL_TO_ABS( a ) ((cell_t *) (((cell_t) a) + CodeBase))

/* Truncate the unsigned double cell integer LO/HI to an uint64_t. */
static uint64_t UdToUint64( ucell_t Lo, ucell_t Hi )
{
    return (( 2 * sizeof(ucell_t) == sizeof(uint64_t) )
        ? (((uint64_t)Lo) | (((uint64_t)Hi) >> (sizeof(ucell_t) * 8)))
        : Lo);
}

/* Return TRUE if the unsigned double cell integer LO/HI is not greater
 * then the greatest uint64_t.
 */
static int UdIsUint64( ucell_t Lo, ucell_t Hi )
{
    return (( 2 * sizeof(ucell_t) == sizeof(uint64_t) )
        ? TRUE
        : Hi == 0);
}

static const char *pfSelectFileModeCreate( cell_t fam );
static const char *pfSelectFileModeOpen( cell_t fam );

/**************************************************************/
static const char *pfSelectFileModeCreate( cell_t fam )
{
    const char *famText = NULL;
    switch( fam )
    {
    case (PF_FAM_WRITE_ONLY + PF_FAM_BINARY_FLAG):
        famText = PF_FAM_BIN_CREATE_WO;
        break;
    case (PF_FAM_READ_WRITE + PF_FAM_BINARY_FLAG):
        famText = PF_FAM_BIN_CREATE_RW;
        break;
    case PF_FAM_WRITE_ONLY:
        famText = PF_FAM_CREATE_WO;
        break;
    case PF_FAM_READ_WRITE:
        famText = PF_FAM_CREATE_RW;
        break;
    default:
        famText = "illegal";
        break;
    }
    return famText;
}

/**************************************************************/
static const char *pfSelectFileModeOpen( cell_t fam )
{
    const char *famText = NULL;
    switch( fam )
    {
    case (PF_FAM_READ_ONLY + PF_FAM_BINARY_FLAG):
        famText = PF_FAM_BIN_OPEN_RO;
        break;
    case (PF_FAM_WRITE_ONLY + PF_FAM_BINARY_FLAG):
        famText = PF_FAM_BIN_CREATE_WO;
        break;
    case (PF_FAM_READ_WRITE + PF_FAM_BINARY_FLAG):
        famText = PF_FAM_BIN_OPEN_RW;
        break;
    case PF_FAM_READ_ONLY:
        famText = PF_FAM_OPEN_RO;
        break;
    case PF_FAM_WRITE_ONLY:
        famText = PF_FAM_CREATE_WO;
        break;
    case PF_FAM_READ_WRITE:
    default:
        famText = PF_FAM_OPEN_RW;
        break;
    }
    return famText;
}

/**************************************************************/
ThrowCode pfCatch( ExecToken XT )
{
    register cell_t  TopOfStack;    /* Cache for faster execution. */
    register cell_t *DataStackPtr;
    register cell_t *ReturnStackPtr;
    register cell_t *InsPtr = NULL;
    register cell_t  Token;
    cell_t           Scratch;

#ifdef PF_SUPPORT_FP
    PF_FLOAT       fpTopOfStack;
    PF_FLOAT      *FloatStackPtr;
    PF_FLOAT       fpScratch;
    PF_FLOAT       fpTemp;
    PF_FLOAT      *InitialFloatStack;
#endif
#ifdef PF_SUPPORT_TRACE
    cell_t Level = 0;
#endif
    cell_t        *LocalsPtr = NULL;
    cell_t         Temp;
    cell_t        *InitialReturnStack;
    cell_t        *InitialDataStack;
    cell_t         FakeSecondary[2];
    char          *CharPtr;
    cell_t        *CellPtr;
    FileStream    *FileID;
    uint8_t       *CodeBase = (uint8_t *) CODE_BASE;
    ThrowCode      ExceptionReturnCode = 0;

/* FIXME
    gExecutionDepth += 1;
    PRT(("pfCatch( 0x%x ), depth = %d\n", XT, gExecutionDepth ));
*/

/*
** Initialize FakeSecondary this way to avoid having stuff in the data section,
** which is not supported for some embedded system loaders.
*/
    FakeSecondary[0] = 0;
    FakeSecondary[1] = ID_EXIT; /* For EXECUTE */

/* Move data from task structure to registers for speed. */
    LOAD_REGISTERS;

/* Save initial stack depths for THROW */
    InitialReturnStack = TORPTR;
    InitialDataStack   = STKPTR ;
#ifdef PF_SUPPORT_FP
    InitialFloatStack  = FP_STKPTR;
#endif

    Token = XT;

    do
    {
DBUG(("pfCatch: Token = 0x%x\n", Token ));

/* --------------------------------------------------------------- */
/* If secondary, thread down code tree until we hit a primitive. */
        while( !IsTokenPrimitive( Token ) )
        {
#ifdef PF_SUPPORT_TRACE
            if((gVarTraceFlags & TRACE_INNER) )
            {
                MSG("pfCatch: Secondary Token = 0x");
                ffDotHex(Token);
                MSG_NUM_H(", InsPtr = 0x", InsPtr);
            }
            TRACENAMES;
#endif

/* Save IP on return stack like a JSR. */
            M_R_PUSH( InsPtr );

/* Convert execution token to absolute address. */
            InsPtr = (cell_t *) ( LOCAL_CODEREL_TO_ABS(Token) );

/* Fetch token at IP. */
            Token = READ_CELL_DIC(InsPtr++);

#ifdef PF_SUPPORT_TRACE
/* Bump level for trace display */
            Level++;
#endif
        }


#ifdef PF_SUPPORT_TRACE
        TRACENAMES;
#endif

/* Execute primitive Token. */
        switch( Token )
        {

    /* Pop up a level in Forth inner interpreter.
    ** Used to implement semicolon.
    ** Put first in switch because ID_EXIT==0 */
        case ID_EXIT:
            InsPtr = ( cell_t *) M_R_POP;
#ifdef PF_SUPPORT_TRACE
            Level--;
#endif
            endcase;

        case ID_1MINUS:  TOS--; endcase;

        case ID_1PLUS:   TOS++; endcase;

#ifndef PF_NO_SHELL
        case ID_2LITERAL:
            ff2Literal( TOS, M_POP );
            M_DROP;
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_2LITERAL_P:
/* hi part stored first, put on top of stack */
            PUSH_TOS;
            TOS = READ_CELL_DIC(InsPtr++);
            M_PUSH(READ_CELL_DIC(InsPtr++));
            endcase;

        case ID_2MINUS:  TOS -= 2; endcase;

        case ID_2PLUS:   TOS += 2; endcase;


        case ID_2OVER:  /* ( a b c d -- a b c d a b ) */
            PUSH_TOS;
            Scratch = M_STACK(3);
            M_PUSH(Scratch);
            TOS = M_STACK(3);
            endcase;

        case ID_2SWAP:  /* ( a b c d -- c d a b ) */
            Scratch = M_STACK(0);    /* c */
            M_STACK(0) = M_STACK(2); /* a */
            M_STACK(2) = Scratch;    /* c */
            Scratch = TOS;           /* d */
            TOS = M_STACK(1);        /* b */
            M_STACK(1) = Scratch;    /* d */
            endcase;

        case ID_2DUP:   /* ( a b -- a b a b ) */
            PUSH_TOS;
            Scratch = M_STACK(1);
            M_PUSH(Scratch);
            endcase;

        case ID_2_R_FETCH:
            PUSH_TOS;
            M_PUSH( (*(TORPTR+1)) );
            TOS = (*(TORPTR));
            endcase;

        case ID_2_R_FROM:
            PUSH_TOS;
            TOS = M_R_POP;
            M_PUSH( M_R_POP );
            endcase;

        case ID_2_TO_R:
            M_R_PUSH( M_POP );
            M_R_PUSH( TOS );
            M_DROP;
            endcase;

        case ID_ACCEPT_P: /* ( c-addr +n1 -- +n2 ) */
            CharPtr = (char *) M_POP;
            TOS = ioAccept( CharPtr, TOS );
            endcase;

#ifndef PF_NO_SHELL
        case ID_ALITERAL:
            ffALiteral( ABS_TO_CODEREL(TOS) );
            M_DROP;
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_ALITERAL_P:
            PUSH_TOS;
            TOS = (cell_t) LOCAL_CODEREL_TO_ABS( READ_CELL_DIC(InsPtr++) );
            endcase;

/* Allocate some extra and put validation identifier at base */
#define PF_MEMORY_VALIDATOR  (0xA81B4D69)
        case ID_ALLOCATE:
            /* Allocate at least one cell's worth because we clobber first cell. */
            if ( TOS < sizeof(cell_t) )
            {
                Temp = sizeof(cell_t);
            }
            else
            {
                Temp = TOS;
            }
            /* Allocate extra cells worth because we store validation info. */
            CellPtr = (cell_t *) pfAllocMem( Temp + sizeof(cell_t) );
            if( CellPtr )
            {
/* This was broken into two steps because different compilers incremented
** CellPtr before or after the XOR step. */
                Temp = (cell_t)CellPtr ^ PF_MEMORY_VALIDATOR;
                *CellPtr++ = Temp;
                M_PUSH( (cell_t) CellPtr );
                TOS = 0;
            }
            else
            {
                M_PUSH( 0 );
                TOS = -1;  /* FIXME Fix error code. */
            }
            endcase;

        case ID_AND:     BINARY_OP( & ); endcase;

        case ID_ARSHIFT:     BINARY_OP( >> ); endcase;  /* Arithmetic right shift */

        case ID_BODY_OFFSET:
            PUSH_TOS;
            TOS = CREATE_BODY_OFFSET;
            endcase;

/* Branch is followed by an offset relative to address of offset. */
        case ID_BRANCH:
DBUGX(("Before Branch: IP = 0x%x\n", InsPtr ));
            M_BRANCH;
DBUGX(("After Branch: IP = 0x%x\n", InsPtr ));
            endcase;

        case ID_BYE:
            EMIT_CR;
            M_THROW( THROW_BYE );
            endcase;

        case ID_BAIL:
            MSG("Emergency exit.\n");
            EXIT(1);
            endcase;

        case ID_CATCH:
            Scratch = TOS;
            TOS = M_POP;
            SAVE_REGISTERS;
            Scratch = pfCatch( Scratch );
            LOAD_REGISTERS;
            M_PUSH( TOS );
            TOS = Scratch;
            endcase;

        case ID_CALL_C:
            SAVE_REGISTERS;
            Scratch = READ_CELL_DIC(InsPtr++);
            CallUserFunction( Scratch & 0xFFFF,
                (Scratch >> 31) & 1,
                (Scratch >> 24) & 0x7F );
            LOAD_REGISTERS;
            endcase;

        /* Support 32/64 bit operation. */
        case ID_CELL:
                M_PUSH( TOS );
                TOS = sizeof(cell_t);
                endcase;

        case ID_CELLS:
                TOS = TOS * sizeof(cell_t);
                endcase;

        case ID_CFETCH:   TOS = *((uint8_t *) TOS); endcase;

        case ID_CMOVE: /* ( src dst n -- ) */
            {
                register char *DstPtr = (char *) M_POP; /* dst */
                CharPtr = (char *) M_POP;    /* src */
                for( Scratch=0; (ucell_t) Scratch < (ucell_t) TOS ; Scratch++ )
                {
                    *DstPtr++ = *CharPtr++;
                }
                M_DROP;
            }
            endcase;

        case ID_CMOVE_UP: /* ( src dst n -- ) */
            {
                register char *DstPtr = ((char *) M_POP) + TOS; /* dst */
                CharPtr = ((char *) M_POP) + TOS;;    /* src */
                for( Scratch=0; (ucell_t) Scratch < (ucell_t) TOS ; Scratch++ )
                {
                    *(--DstPtr) = *(--CharPtr);
                }
                M_DROP;
            }
            endcase;

#ifndef PF_NO_SHELL
        case ID_COLON:
            SAVE_REGISTERS;
            ffColon( );
            LOAD_REGISTERS;
            endcase;
        case ID_COLON_P:  /* ( $name xt -- ) */
            CreateDicEntry( TOS, (char *) M_POP, 0 );
            M_DROP;
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_COMPARE:
            {
                const char *s1, *s2;
                cell_t len1;
                s2 = (const char *) M_POP;
                len1 = M_POP;
                s1 = (const char *) M_POP;
                TOS = ffCompare( s1, len1, s2, TOS );
            }
            endcase;

/* ( a b -- flag , Comparisons ) */
        case ID_COMP_EQUAL:
            TOS = ( TOS == M_POP ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_NOT_EQUAL:
            TOS = ( TOS != M_POP ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_GREATERTHAN:
            TOS = ( M_POP > TOS ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_LESSTHAN:
            TOS = (  M_POP < TOS ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_U_GREATERTHAN:
            TOS = ( ((ucell_t)M_POP) > ((ucell_t)TOS) ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_U_LESSTHAN:
            TOS = ( ((ucell_t)M_POP) < ((ucell_t)TOS) ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_ZERO_EQUAL:
            TOS = ( TOS == 0 ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_ZERO_NOT_EQUAL:
            TOS = ( TOS != 0 ) ? FTRUE : FALSE ;
            endcase;
        case ID_COMP_ZERO_GREATERTHAN:
            TOS = ( TOS > 0 ) ? FTRUE : FFALSE ;
            endcase;
        case ID_COMP_ZERO_LESSTHAN:
            TOS = ( TOS < 0 ) ? FTRUE : FFALSE ;
            endcase;

        case ID_CR:
            EMIT_CR;
            endcase;

#ifndef PF_NO_SHELL
        case ID_CREATE:
            SAVE_REGISTERS;
            ffCreate();
            LOAD_REGISTERS;
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_CREATE_P:
            PUSH_TOS;
/* Put address of body on stack.  Insptr points after code start. */
            TOS = (cell_t) ((char *)InsPtr - sizeof(cell_t) + CREATE_BODY_OFFSET );
            endcase;

        case ID_CSTORE: /* ( c caddr -- ) */
            *((uint8_t *) TOS) = (uint8_t) M_POP;
            M_DROP;
            endcase;

/* Double precision add. */
        case ID_D_PLUS:  /* D+ ( al ah bl bh -- sl sh ) */
            {
                register ucell_t ah,al,bl,sh,sl;
#define bh TOS
                bl = M_POP;
                ah = M_POP;
                al = M_POP;
                sh = 0;
                sl = al + bl;
                if( sl < bl ) sh = 1; /* Carry */
                sh += ah + bh;
                M_PUSH( sl );
                TOS = sh;
#undef bh
            }
            endcase;

/* Double precision subtract. */
        case ID_D_MINUS:  /* D- ( al ah bl bh -- sl sh ) */
            {
                register ucell_t ah,al,bl,sh,sl;
#define bh TOS
                bl = M_POP;
                ah = M_POP;
                al = M_POP;
                sh = 0;
                sl = al - bl;
                if( al < bl ) sh = 1; /* Borrow */
                sh = ah - bh - sh;
                M_PUSH( sl );
                TOS = sh;
#undef bh
            }
            endcase;

/* Assume 8-bit char and calculate cell width. */
#define NBITS ((sizeof(ucell_t)) * 8)
/* Define half the number of bits in a cell. */
#define HNBITS (NBITS / 2)
/* Assume two-complement arithmetic to calculate lower half. */
#define LOWER_HALF(n) ((n) & (((ucell_t)1 << HNBITS) - 1))
#define HIGH_BIT ((ucell_t)1 << (NBITS - 1))

/* Perform cell*cell bit multiply for a 2 cell result, by factoring into half cell quantities.
 * Using an improved algorithm suggested by Steve Green.
 * Converted to 64-bit by Aleksej Saushev.
 */
        case ID_D_UMTIMES:  /* UM* ( a b -- lo hi ) */
            {
                ucell_t ahi, alo, bhi, blo; /* input parts */
                ucell_t lo, hi, temp;
/* Get values from stack. */
                ahi = M_POP;
                bhi = TOS;
/* Break into hi and lo 16 bit parts. */
                alo = LOWER_HALF(ahi);
                ahi = ahi >> HNBITS;
                blo = LOWER_HALF(bhi);
                bhi = bhi >> HNBITS;

                lo = 0;
                hi = 0;
/* higher part: ahi * bhi */
                hi += ahi * bhi;
/* middle (overlapping) part: ahi * blo */
                temp = ahi * blo;
                lo += LOWER_HALF(temp);
                hi += temp >> HNBITS;
/* middle (overlapping) part: alo * bhi  */
                temp = alo * bhi;
                lo += LOWER_HALF(temp);
                hi += temp >> HNBITS;
/* lower part: alo * blo */
                temp = alo * blo;
/* its higher half overlaps with middle's lower half: */
                lo += temp >> HNBITS;
/* process carry: */
                hi += lo >> HNBITS;
                lo = LOWER_HALF(lo);
/* combine lower part of result: */
                lo = (lo << HNBITS) + LOWER_HALF(temp);

                M_PUSH( lo );
                TOS = hi;
            }
            endcase;

/* Perform cell*cell bit multiply for 2 cell result, using shift and add. */
        case ID_D_MTIMES:  /* M* ( a b -- pl ph ) */
            {
                ucell_t ahi, alo, bhi, blo; /* input parts */
                ucell_t lo, hi, temp;
                int sg;
/* Get values from stack. */
                ahi = M_POP;
                bhi = TOS;

/* Calculate product sign: */
                sg = ((cell_t)(ahi ^ bhi) < 0);
/* Take absolute values and reduce to um* */
                if ((cell_t)ahi < 0) ahi = (ucell_t)(-(cell_t)ahi);
                if ((cell_t)bhi < 0) bhi = (ucell_t)(-(cell_t)bhi);

/* Break into hi and lo 16 bit parts. */
                alo = LOWER_HALF(ahi);
                ahi = ahi >> HNBITS;
                blo = LOWER_HALF(bhi);
                bhi = bhi >> HNBITS;

                lo = 0;
                hi = 0;
/* higher part: ahi * bhi */
                hi += ahi * bhi;
/* middle (overlapping) part: ahi * blo */
                temp = ahi * blo;
                lo += LOWER_HALF(temp);
                hi += temp >> HNBITS;
/* middle (overlapping) part: alo * bhi  */
                temp = alo * bhi;
                lo += LOWER_HALF(temp);
                hi += temp >> HNBITS;
/* lower part: alo * blo */
                temp = alo * blo;
/* its higher half overlaps with middle's lower half: */
                lo += temp >> HNBITS;
/* process carry: */
                hi += lo >> HNBITS;
                lo = LOWER_HALF(lo);
/* combine lower part of result: */
                lo = (lo << HNBITS) + LOWER_HALF(temp);

/* Negate product if one operand negative. */
                if(sg)
                {
                    /* lo = (ucell_t)(- lo); */
                    lo = ~lo + 1;
                    hi = ~hi + ((lo == 0) ? 1 : 0);
                }

                M_PUSH( lo );
                TOS = hi;
            }
            endcase;

#define DULT(du1l,du1h,du2l,du2h) ( (du2h<du1h) ? FALSE : ( (du2h==du1h) ? (du1l<du2l) : TRUE) )
/* Perform 2 cell by 1 cell divide for 1 cell result and remainder, using shift and subtract. */
        case ID_D_UMSMOD:  /* UM/MOD ( al ah bdiv -- rem q ) */
            {
                ucell_t ah,al, q,di, bl,bh, sl,sh;
                ah = M_POP;
                al = M_POP;
                bh = TOS;
                bl = 0;
                q = 0;
                for( di=0; di<NBITS; di++ )
                {
                    if( !DULT(al,ah,bl,bh) )
                    {
                        sh = 0;
                        sl = al - bl;
                        if( al < bl ) sh = 1; /* Borrow */
                        sh = ah - bh - sh;
                        ah = sh;
                        al = sl;
                        q |= 1;
                    }
                    q = q << 1;
                    bl = (bl >> 1) | (bh << (NBITS-1));
                    bh = bh >> 1;
                }
                if( !DULT(al,ah,bl,bh) )
                {

                    al = al - bl;
                    q |= 1;
                }
                M_PUSH( al );  /* rem */
                TOS = q;
            }
            endcase;

/* Perform 2 cell by 1 cell divide for 2 cell result and remainder, using shift and subtract. */
        case ID_D_MUSMOD:  /* MU/MOD ( al am bdiv -- rem ql qh ) */
            {
                register ucell_t ah,am,al,ql,qh,di;
#define bdiv ((ucell_t)TOS)
                ah = 0;
                am = M_POP;
                al = M_POP;
                qh = ql = 0;
                for( di=0; di<2*NBITS; di++ )
                {
                    if( bdiv <= ah )
                    {
                        ah = ah - bdiv;
                        ql |= 1;
                    }
                    qh = (qh << 1) | (ql >> (NBITS-1));
                    ql = ql << 1;
                    ah = (ah << 1) | (am >> (NBITS-1));
                    am = (am << 1) | (al >> (NBITS-1));
                    al = al << 1;
DBUG(("XX ah,m,l = 0x%8x,%8x,%8x - qh,l = 0x%8x,%8x\n", ah,am,al, qh,ql ));
                }
                if( bdiv <= ah )
                {
                    ah = ah - bdiv;
                    ql |= 1;
                }
                M_PUSH( ah ); /* rem */
                M_PUSH( ql );
                TOS = qh;
#undef bdiv
            }
            endcase;

#ifndef PF_NO_SHELL
        case ID_DEFER:
            ffDefer( );
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_DEFER_P:
            endcase;

        case ID_DEPTH:
            PUSH_TOS;
            TOS = gCurrentTask->td_StackBase - STKPTR;
            endcase;

        case ID_DIVIDE:     BINARY_OP( / ); endcase;

        case ID_DOT:
            ffDot( TOS );
            M_DROP;
            endcase;

        case ID_DOTS:
            M_DOTS;
            endcase;

        case ID_DROP:  M_DROP; endcase;

        case ID_DUMP:
            Scratch = M_POP;
            DumpMemory( (char *) Scratch, TOS );
            M_DROP;
            endcase;

        case ID_DUP:   M_DUP; endcase;

        case ID_DO_P: /* ( limit start -- ) ( R: -- start limit ) */
            M_R_PUSH( TOS );
            M_R_PUSH( M_POP );
            M_DROP;
            endcase;

        case ID_EOL:    /* ( -- end_of_line_char ) */
            PUSH_TOS;
            TOS = (cell_t) '\n';
            endcase;

        case ID_ERRORQ_P:  /* ( flag num -- , quit if flag true ) */
            Scratch = TOS;
            M_DROP;
            if(TOS)
            {
                M_THROW(Scratch);
            }
            else
            {
                M_DROP;
            }
            endcase;

        case ID_EMIT_P:
            EMIT( (char) TOS );
            M_DROP;
            endcase;

        case ID_EXECUTE:
/* Save IP on return stack like a JSR. */
            M_R_PUSH( InsPtr );
#ifdef PF_SUPPORT_TRACE
/* Bump level for trace. */
            Level++;
#endif
            if( IsTokenPrimitive( TOS ) )
            {
                WRITE_CELL_DIC( (cell_t *) &FakeSecondary[0], TOS);   /* Build a fake secondary and execute it. */
                InsPtr = &FakeSecondary[0];
            }
            else
            {
                InsPtr = (cell_t *) LOCAL_CODEREL_TO_ABS(TOS);
            }
            M_DROP;
            endcase;

        case ID_FETCH:
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
            if( IN_DICS( TOS ) )
            {
                TOS = (cell_t) READ_CELL_DIC((cell_t *)TOS);
            }
            else
            {
                TOS = *((cell_t *)TOS);
            }
#else
            TOS = *((cell_t *)TOS);
#endif
            endcase;

        case ID_FILE_CREATE: /* ( c-addr u fam -- fid ior ) */
/* Build NUL terminated name string. */
            Scratch = M_POP; /* u */
            Temp = M_POP;    /* caddr */
            if( Scratch < TIB_SIZE-2 )
            {
                const char *famText = pfSelectFileModeCreate( TOS );
                pfCopyMemory( gScratch, (char *) Temp, (ucell_t) Scratch );
                gScratch[Scratch] = '\0';
                DBUG(("Create file = %s with famTxt %s\n", gScratch, famText ));
                FileID = sdOpenFile( gScratch, famText );
                TOS = ( FileID == NULL ) ? -1 : 0 ;
                M_PUSH( (cell_t) FileID );
            }
            else
            {
                ERR("Filename too large for name buffer.\n");
                M_PUSH( 0 );
                TOS = -2;
            }
            endcase;

        case ID_FILE_DELETE: /* ( c-addr u -- ior ) */
/* Build NUL terminated name string. */
            Temp = M_POP;    /* caddr */
            if( TOS < TIB_SIZE-2 )
            {
                pfCopyMemory( gScratch, (char *) Temp, (ucell_t) TOS );
                gScratch[TOS] = '\0';
                DBUG(("Delete file = %s\n", gScratch ));
                TOS = sdDeleteFile( gScratch );
            }
            else
            {
                ERR("Filename too large for name buffer.\n");
                TOS = -2;
            }
            endcase;

        case ID_FILE_OPEN: /* ( c-addr u fam -- fid ior ) */
/* Build NUL terminated name string. */
            Scratch = M_POP; /* u */
            Temp = M_POP;    /* caddr */
            if( Scratch < TIB_SIZE-2 )
            {
                const char *famText = pfSelectFileModeOpen( TOS );
                pfCopyMemory( gScratch, (char *) Temp, (ucell_t) Scratch );
                gScratch[Scratch] = '\0';
                DBUG(("Open file = %s\n", gScratch ));
                FileID = sdOpenFile( gScratch, famText );

                TOS = ( FileID == NULL ) ? -1 : 0 ;
                M_PUSH( (cell_t) FileID );
            }
            else
            {
                ERR("Filename too large for name buffer.\n");
                M_PUSH( 0 );
                TOS = -2;
            }
            endcase;

        case ID_FILE_CLOSE: /* ( fid -- ior ) */
            TOS = sdCloseFile( (FileStream *) TOS );
            endcase;

        case ID_FILE_READ: /* ( addr len fid -- u2 ior ) */
            FileID = (FileStream *) TOS;
            Scratch = M_POP;
            CharPtr = (char *) M_POP;
            Temp = sdReadFile( CharPtr, 1, Scratch, FileID );
            /* TODO check feof() or ferror() */
            M_PUSH(Temp);
            TOS = 0;
            endcase;

        /* TODO Why does this crash when passed an illegal FID? */
        case ID_FILE_SIZE: /* ( fid -- ud ior ) */
/* Determine file size by seeking to end and returning position. */
            FileID = (FileStream *) TOS;
            {
                file_offset_t endposition = -1;
                file_offset_t original = sdTellFile( FileID );
                if (original >= 0)
                {
                    sdSeekFile( FileID, 0, PF_SEEK_END );
                    endposition = sdTellFile( FileID );
                    /* Restore original position. */
                    sdSeekFile( FileID, original, PF_SEEK_SET );
                }
                if (endposition < 0)
                {
                    M_PUSH(0); /* low */
                    M_PUSH(0); /* high */
                    TOS = -4;  /* TODO proper error number */
                }
                else
                {
                    M_PUSH(endposition); /* low */
                    /* We do not support double precision file offsets.*/
                    M_PUSH(0); /* high */
                    TOS = 0;   /* OK */
                }
            }
            endcase;

        case ID_FILE_WRITE: /* ( addr len fid -- ior ) */
            FileID = (FileStream *) TOS;
            Scratch = M_POP;
            CharPtr = (char *) M_POP;
            Temp = sdWriteFile( CharPtr, 1, Scratch, FileID );
            TOS = (Temp != Scratch) ? -3 : 0;
            endcase;

        case ID_FILE_REPOSITION: /* ( ud fid -- ior ) */
            {
                file_offset_t offset;
                cell_t offsetHigh;
                cell_t offsetLow;
                FileID = (FileStream *) TOS;
                offsetHigh = M_POP;
                offsetLow = M_POP;
                /* We do not support double precision file offsets in pForth.
                 * So check to make sure the high bits are not used.
                 */
                if (offsetHigh != 0)
                {
                    TOS = -3; /* TODO err num? */
                    break;
                }
                offset = (file_offset_t)offsetLow;
                TOS = sdSeekFile( FileID, offset, PF_SEEK_SET );
            }
            endcase;

        case ID_FILE_POSITION: /* ( fid -- ud ior ) */
            {
                file_offset_t position;
                FileID = (FileStream *) TOS;
                position = sdTellFile( FileID );
                if (position < 0)
                {
                    M_PUSH(0); /* low */
                    M_PUSH(0); /* high */
                    TOS = -4;  /* TODO proper error number */
                }
                else
                {
                    M_PUSH(position); /* low */
                    /* We do not support double precision file offsets.*/
                    M_PUSH(0); /* high */
                    TOS = 0; /* OK */
                }
            }
            endcase;

        case ID_FILE_RO: /* (  -- fam ) */
            PUSH_TOS;
            TOS = PF_FAM_READ_ONLY;
            endcase;

        case ID_FILE_RW: /* ( -- fam ) */
            PUSH_TOS;
            TOS = PF_FAM_READ_WRITE;
            endcase;

        case ID_FILE_WO: /* ( -- fam ) */
            PUSH_TOS;
            TOS = PF_FAM_WRITE_ONLY;
            endcase;

        case ID_FILE_BIN: /* ( fam1 -- fam2 ) */
            TOS = TOS | PF_FAM_BINARY_FLAG;
            endcase;

	case ID_FILE_FLUSH: /* ( fileid -- ior ) */
	    {
		FileStream *Stream = (FileStream *) TOS;
		TOS = (sdFlushFile( Stream ) == 0) ? 0 : THROW_FLUSH_FILE;
	    }
	    endcase;

	case ID_FILE_RENAME: /* ( oldName newName -- ior ) */
	    {
		char *New = (char *) TOS;
		char *Old = (char *) M_POP;
		TOS = sdRenameFile( Old, New );
	    }
	    endcase;

	case ID_FILE_RESIZE: /* ( ud fileid -- ior ) */
	    {
		FileStream *File = (FileStream *) TOS;
		ucell_t SizeHi = (ucell_t) M_POP;
		ucell_t SizeLo = (ucell_t) M_POP;
		TOS = ( UdIsUint64( SizeLo, SizeHi )
			? sdResizeFile( File, UdToUint64( SizeLo, SizeHi ))
			: THROW_RESIZE_FILE );
	    }
	    endcase;

        case ID_FILL: /* ( caddr num charval -- ) */
            {
                register char *DstPtr;
                Temp = M_POP;    /* num */
                DstPtr = (char *) M_POP; /* dst */
                for( Scratch=0; (ucell_t) Scratch < (ucell_t) Temp ; Scratch++ )
                {
                    *DstPtr++ = (char) TOS;
                }
                M_DROP;
            }
            endcase;

#ifndef PF_NO_SHELL
        case ID_FIND:  /* ( $addr -- $addr 0 | xt +-1 ) */
            TOS = ffFind( (char *) TOS, (ExecToken *) &Temp );
            M_PUSH( Temp );
            endcase;

        case ID_FINDNFA:
            TOS = ffFindNFA( (const ForthString *) TOS, (const ForthString **) &Temp );
            M_PUSH( (cell_t) Temp );
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_FLUSHEMIT:
            sdTerminalFlush();
            endcase;

/* Validate memory before freeing. Clobber validator and first word. */
        case ID_FREE:   /* ( addr -- result ) */
            if( TOS == 0 )
            {
                ERR("FREE passed NULL!\n");
                TOS = -2; /* FIXME error code */
            }
            else
            {
                CellPtr = (cell_t *) TOS;
                CellPtr--;
                if( ((ucell_t)*CellPtr) != ((ucell_t)CellPtr ^ PF_MEMORY_VALIDATOR))
                {
                    TOS = -2; /* FIXME error code */
                }
                else
                {
                    CellPtr[0] = 0xDeadBeef;
                    pfFreeMem((char *)CellPtr);
                    TOS = 0;
                }
            }
            endcase;

#include "pfinnrfp.h"

        case ID_HERE:
            PUSH_TOS;
            TOS = (cell_t)CODE_HERE;
            endcase;

        case ID_NUMBERQ_P:   /* ( addr -- 0 | n 1 ) */
/* Convert using number converter in 'C'.
** Only supports single precision for bootstrap.
*/
            TOS = (cell_t) ffNumberQ( (char *) TOS, &Temp );
            if( TOS == NUM_TYPE_SINGLE)
            {
                M_PUSH( Temp );   /* Push single number */
            }
            endcase;

        case ID_I:  /* ( -- i , DO LOOP index ) */
            PUSH_TOS;
            TOS = M_R_PICK(1);
            endcase;

#ifndef PF_NO_SHELL
        case ID_INCLUDE_FILE:
            FileID = (FileStream *) TOS;
            M_DROP;    /* Drop now so that INCLUDE has a clean stack. */
            SAVE_REGISTERS;
            Scratch = ffIncludeFile( FileID );
            LOAD_REGISTERS;
            if( Scratch ) M_THROW(Scratch)
            endcase;
#endif  /* !PF_NO_SHELL */

#ifndef PF_NO_SHELL
        case ID_INTERPRET:
            SAVE_REGISTERS;
            Scratch = ffInterpret();
            LOAD_REGISTERS;
            if( Scratch ) M_THROW(Scratch)
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_J:  /* ( -- j , second DO LOOP index ) */
            PUSH_TOS;
            TOS = M_R_PICK(3);
            endcase;

        case ID_KEY:
            PUSH_TOS;
            TOS = ioKey();
            if (TOS == ASCII_EOT) {
                M_THROW(THROW_BYE);
            }
            endcase;

#ifndef PF_NO_SHELL
        case ID_LITERAL:
            ffLiteral( TOS );
            M_DROP;
            endcase;
#endif /* !PF_NO_SHELL */

        case ID_LITERAL_P:
            DBUG(("ID_LITERAL_P: InsPtr = 0x%x, *InsPtr = 0x%x\n", InsPtr, *InsPtr ));
            PUSH_TOS;
            TOS = READ_CELL_DIC(InsPtr++);
            endcase;

#ifndef PF_NO_SHELL
        case ID_LOCAL_COMPILER: DO_VAR(gLocalCompiler_XT); endcase;
#endif /* !PF_NO_SHELL */

        case ID_LOCAL_FETCH: /* ( i <local> -- n , fetch from local ) */
            TOS = *(LocalsPtr - TOS);
            endcase;

#define LOCAL_FETCH_N(num) \
        case ID_LOCAL_FETCH_##num: /* ( <local> -- n , fetch from local ) */ \
            PUSH_TOS; \
            TOS = *(LocalsPtr -(num)); \
            endcase;

        LOCAL_FETCH_N(1);
        LOCAL_FETCH_N(2);
        LOCAL_FETCH_N(3);
        LOCAL_FETCH_N(4);
        LOCAL_FETCH_N(5);
        LOCAL_FETCH_N(6);
        LOCAL_FETCH_N(7);
        LOCAL_FETCH_N(8);

        case ID_LOCAL_STORE:  /* ( n i <local> -- , store n in local ) */
            *(LocalsPtr - TOS) = M_POP;
            M_DROP;
            endcase;

#define LOCAL_STORE_N(num) \
        case ID_LOCAL_STORE_##num:  /* ( n <local> -- , store n in local ) */ \
            *(LocalsPtr - (num)) = TOS; \
            M_DROP; \
            endcase;

        LOCAL_STORE_N(1);
        LOCAL_STORE_N(2);
        LOCAL_STORE_N(3);
        LOCAL_STORE_N(4);
        LOCAL_STORE_N(5);
        LOCAL_STORE_N(6);
        LOCAL_STORE_N(7);
        LOCAL_STORE_N(8);

        case ID_LOCAL_PLUSSTORE:  /* ( n i <local> -- , add n to local ) */
            *(LocalsPtr - TOS) += M_POP;
            M_DROP;
            endcase;

        case ID_LOCAL_ENTRY: /* ( x0 x1 ... xn n -- ) */
        /* create local stack frame */
            {
                cell_t i = TOS;
                cell_t *lp;
                DBUG(("LocalEntry: n = %d\n", TOS));
                /* End of locals. Create stack frame */
                DBUG(("LocalEntry: before RP@ = 0x%x, LP = 0x%x\n",
                    TORPTR, LocalsPtr));
                M_R_PUSH(LocalsPtr);
                LocalsPtr = TORPTR;
                TORPTR -= TOS;
                DBUG(("LocalEntry: after RP@ = 0x%x, LP = 0x%x\n",
                    TORPTR, LocalsPtr));
                lp = TORPTR;
                while(i-- > 0)
                {
                    *lp++ = M_POP;    /* Load local vars from stack */
                }
                M_DROP;
            }
            endcase;

        case ID_LOCAL_EXIT: /* cleanup up local stack frame */
            DBUG(("LocalExit: before RP@ = 0x%x, LP = 0x%x\n",
                TORPTR, LocalsPtr));
            TORPTR = LocalsPtr;
            LocalsPtr = (cell_t *) M_R_POP;
            DBUG(("LocalExit: after RP@ = 0x%x, LP = 0x%x\n",
                TORPTR, LocalsPtr));
            endcase;

#ifndef PF_NO_SHELL
        case ID_LOADSYS:
            MSG("Load "); MSG(SYSTEM_LOAD_FILE); EMIT_CR;
            FileID = sdOpenFile(SYSTEM_LOAD_FILE, "r");
            if( FileID )
            {
                SAVE_REGISTERS;
                Scratch = ffIncludeFile( FileID ); /* Also closes the file. */
                LOAD_REGISTERS;
                if( Scratch ) M_THROW(Scratch);
            }
            else
            {
                 ERR(SYSTEM_LOAD_FILE); ERR(" could not be opened!\n");
            }
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_LEAVE_P: /* ( R: index limit --  ) */
            M_R_DROP;
            M_R_DROP;
            M_BRANCH;
            endcase;

        case ID_LOOP_P: /* ( R: index limit -- | index limit ) */
            Temp = M_R_POP; /* limit */
            Scratch = M_R_POP + 1; /* index */
            if( Scratch == Temp )
            {
                InsPtr++;   /* skip branch offset, exit loop */
            }
            else
            {
/* Push index and limit back to R */
                M_R_PUSH( Scratch );
                M_R_PUSH( Temp );
/* Branch back to just after (DO) */
                M_BRANCH;
            }
            endcase;

        case ID_LSHIFT:     BINARY_OP( << ); endcase;

        case ID_MAX:
            Scratch = M_POP;
            TOS = ( TOS > Scratch ) ? TOS : Scratch ;
            endcase;

        case ID_MIN:
            Scratch = M_POP;
            TOS = ( TOS < Scratch ) ? TOS : Scratch ;
            endcase;

        case ID_MINUS:     BINARY_OP( - ); endcase;

#ifndef PF_NO_SHELL
        case ID_NAME_TO_TOKEN:
            TOS = (cell_t) NameToToken((ForthString *)TOS);
            endcase;

        case ID_NAME_TO_PREVIOUS:
            TOS = (cell_t) NameToPrevious((ForthString *)TOS);
            endcase;
#endif

        case ID_NOOP:
            endcase;

        case ID_OR:     BINARY_OP( | ); endcase;

        case ID_OVER:
            PUSH_TOS;
            TOS = M_STACK(1);
            endcase;

        case ID_PICK: /* ( ... n -- sp(n) ) */
            TOS = M_STACK(TOS);
            endcase;

        case ID_PLUS:     BINARY_OP( + ); endcase;

        case ID_PLUS_STORE:   /* ( n addr -- , add n to *addr ) */
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
            if( IN_DICS( TOS ) )
            {
                Scratch = READ_CELL_DIC((cell_t *)TOS);
                Scratch += M_POP;
                WRITE_CELL_DIC((cell_t *)TOS,Scratch);
            }
            else
            {
                *((cell_t *)TOS) += M_POP;
            }
#else
            *((cell_t *)TOS) += M_POP;
#endif
            M_DROP;
            endcase;

        case ID_PLUSLOOP_P: /* ( delta -- ) ( R: index limit -- | index limit ) */
            {
		cell_t Limit = M_R_POP;
		cell_t OldIndex = M_R_POP;
		cell_t Delta = TOS; /* add TOS to index, not 1 */
		cell_t NewIndex = OldIndex + Delta;
		cell_t OldDiff = OldIndex - Limit;

		/* This exploits this idea (lifted from Gforth):
		   (x^y)<0 is equivalent to (x<0) != (y<0) */
                if( ((OldDiff ^ (OldDiff + Delta)) /* is the limit crossed? */
		     & (OldDiff ^ Delta))          /* is it a wrap-around? */
		    < 0 )
		{
                    InsPtr++;   /* skip branch offset, exit loop */
                }
                else
                {
/* Push index and limit back to R */
                    M_R_PUSH( NewIndex );
                    M_R_PUSH( Limit );
/* Branch back to just after (DO) */
                    M_BRANCH;
                }
                M_DROP;
            }
            endcase;

        case ID_QDO_P: /* (?DO) ( limit start -- ) ( R: -- start limit ) */
            Scratch = M_POP;  /* limit */
            if( Scratch == TOS )
            {
/* Branch to just after (LOOP) */
                M_BRANCH;
            }
            else
            {
                M_R_PUSH( TOS );
                M_R_PUSH( Scratch );
                InsPtr++;   /* skip branch offset, enter loop */
            }
            M_DROP;
            endcase;

        case ID_QDUP:     if( TOS ) M_DUP; endcase;

        case ID_QTERMINAL:  /* WARNING: Typically not fully implemented! */
            PUSH_TOS;
            TOS = sdQueryTerminal();
            endcase;

        case ID_QUIT_P: /* Stop inner interpreter, go back to user. */
#ifdef PF_SUPPORT_TRACE
            Level = 0;
#endif
            M_THROW(THROW_QUIT);
            endcase;

        case ID_R_DROP:
            M_R_DROP;
            endcase;

        case ID_R_FETCH:
            PUSH_TOS;
            TOS = (*(TORPTR));
            endcase;

        case ID_R_FROM:
            PUSH_TOS;
            TOS = M_R_POP;
            endcase;

        case ID_REFILL:
            PUSH_TOS;
            TOS = (ffRefill() > 0) ? FTRUE : FFALSE;
            endcase;

/* Resize memory allocated by ALLOCATE. */
        case ID_RESIZE:  /* ( addr1 u -- addr2 result ) */
            {
                cell_t *Addr1 = (cell_t *) M_POP;
                /* Point to validator below users address. */
                cell_t *FreePtr = Addr1 - 1;
                if( ((ucell_t)*FreePtr) != ((ucell_t)FreePtr ^ PF_MEMORY_VALIDATOR))
                {
                    /* 090218 - Fixed bug, was returning zero. */
                    M_PUSH( Addr1 );
                    TOS = -3;
                }
                else
                {
                    /* Try to allocate. */
                    CellPtr = (cell_t *) pfAllocMem( TOS + sizeof(cell_t) );
                    if( CellPtr )
                    {
                        /* Copy memory including validation. */
                        pfCopyMemory( (char *) CellPtr, (char *) FreePtr, TOS + sizeof(cell_t) );
                        *CellPtr = (cell_t)(((ucell_t)CellPtr) ^ (ucell_t)PF_MEMORY_VALIDATOR);
                        /* 090218 - Fixed bug that was incrementing the address twice. Thanks Reinhold Straub. */
                        /* Increment past validator to user address. */
                        M_PUSH( (cell_t) (CellPtr + 1) );
                        TOS = 0; /* Result code. */
                        /* Mark old cell as dead so we can't free it twice. */
                        FreePtr[0] = 0xDeadBeef;
                        pfFreeMem((char *) FreePtr);
                    }
                    else
                    {
                        /* 090218 - Fixed bug, was returning zero. */
                        M_PUSH( Addr1 );
                        TOS = -4;  /* FIXME Fix error code. */
                    }
                }
            }
            endcase;

/*
** RP@ and RP! are called secondaries so we must
** account for the return address pushed before calling.
*/
        case ID_RP_FETCH:    /* ( -- rp , address of top of return stack ) */
            PUSH_TOS;
            TOS = (cell_t)TORPTR;  /* value before calling RP@ */
            endcase;

        case ID_RP_STORE:    /* ( rp -- , address of top of return stack ) */
            TORPTR = (cell_t *) TOS;
            M_DROP;
            endcase;

        case ID_ROLL: /* ( xu xu-1 xu-1 ... x0 u -- xu-1 xu-1 ... x0 xu ) */
            {
                cell_t ri;
                cell_t *srcPtr, *dstPtr;
                Scratch = M_STACK(TOS);
                srcPtr = &M_STACK(TOS-1);
                dstPtr = &M_STACK(TOS);
                for( ri=0; ri<TOS; ri++ )
                {
                    *dstPtr-- = *srcPtr--;
                }
                TOS = Scratch;
                STKPTR++;
            }
            endcase;

        case ID_ROT:  /* ( a b c -- b c a ) */
            Scratch = M_POP;    /* b */
            Temp = M_POP;       /* a */
            M_PUSH( Scratch );  /* b */
            PUSH_TOS;           /* c */
            TOS = Temp;         /* a */
            endcase;

/* Logical right shift */
        case ID_RSHIFT:     { TOS = ((ucell_t)M_POP) >> TOS; } endcase;

#ifndef PF_NO_SHELL
        case ID_SAVE_FORTH_P:   /* ( $name Entry NameSize CodeSize -- err ) */
            {
                cell_t NameSize, CodeSize, EntryPoint;
                CodeSize = TOS;
                NameSize = M_POP;
                EntryPoint = M_POP;
                ForthStringToC( gScratch, (char *) M_POP, sizeof(gScratch) );
                TOS =  ffSaveForth( gScratch, EntryPoint, NameSize, CodeSize );
            }
            endcase;
#endif

        case ID_SLEEP_P:
            TOS = sdSleepMillis(TOS);
            endcase;

        case ID_SP_FETCH:    /* ( -- sp , address of top of stack, sorta ) */
            PUSH_TOS;
            TOS = (cell_t)STKPTR;
            endcase;

        case ID_SP_STORE:    /* ( sp -- , address of top of stack, sorta ) */
            STKPTR = (cell_t *) TOS;
            M_DROP;
            endcase;

        case ID_STORE: /* ( n addr -- , write n to addr ) */
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
            if( IN_DICS( TOS ) )
            {
                WRITE_CELL_DIC((cell_t *)TOS,M_POP);
            }
            else
            {
                *((cell_t *)TOS) = M_POP;
            }
#else
            *((cell_t *)TOS) = M_POP;
#endif
            M_DROP;
            endcase;

        case ID_SCAN: /* ( addr cnt char -- addr' cnt' ) */
            Scratch = M_POP; /* cnt */
            Temp = M_POP;    /* addr */
            TOS = ffScan( (char *) Temp, Scratch, (char) TOS, &CharPtr );
            M_PUSH((cell_t) CharPtr);
            endcase;

#ifndef PF_NO_SHELL
        case ID_SEMICOLON:
            SAVE_REGISTERS;
            Scratch = ffSemiColon();
            LOAD_REGISTERS;
            if( Scratch ) M_THROW( Scratch );
            endcase;
#endif /* !PF_NO_SHELL */

        case ID_SKIP: /* ( addr cnt char -- addr' cnt' ) */
            Scratch = M_POP; /* cnt */
            Temp = M_POP;    /* addr */
            TOS = ffSkip( (char *) Temp, Scratch, (char) TOS, &CharPtr );
            M_PUSH((cell_t) CharPtr);
            endcase;

        case ID_SOURCE:  /* ( -- c-addr num ) */
            PUSH_TOS;
            M_PUSH( (cell_t) gCurrentTask->td_SourcePtr );
            TOS = (cell_t) gCurrentTask->td_SourceNum;
            endcase;

        case ID_SOURCE_SET: /* ( c-addr num -- ) */
            gCurrentTask->td_SourcePtr = (char *) M_POP;
            gCurrentTask->td_SourceNum = TOS;
            M_DROP;
            endcase;

        case ID_SOURCE_ID:
            PUSH_TOS;
            TOS = ffConvertStreamToSourceID( gCurrentTask->td_InputStream ) ;
            endcase;

        case ID_SOURCE_ID_POP:
            PUSH_TOS;
            TOS = ffConvertStreamToSourceID( ffPopInputStream() ) ;
            endcase;

        case ID_SOURCE_ID_PUSH:  /* ( source-id -- ) */
            TOS = (cell_t)ffConvertSourceIDToStream( TOS );
            Scratch = ffPushInputStream((FileStream *) TOS );
            if( Scratch )
            {
                M_THROW(Scratch);
            }
            else M_DROP;
            endcase;

	case ID_SOURCE_LINE_NUMBER_FETCH: /* ( -- linenr ) */
	    PUSH_TOS;
	    TOS = gCurrentTask->td_LineNumber;
	    endcase;

	case ID_SOURCE_LINE_NUMBER_STORE: /* ( linenr -- ) */
	    gCurrentTask->td_LineNumber = TOS;
	    TOS = M_POP;
	    endcase;

        case ID_SWAP:
            Scratch = TOS;
            TOS = *STKPTR;
            *STKPTR = Scratch;
            endcase;

        case ID_TEST1:
            PUSH_TOS;
            M_PUSH( 0x11 );
            M_PUSH( 0x22 );
            TOS = 0x33;
            endcase;

        case ID_TEST2:
            endcase;

        case ID_THROW:  /* ( k*x err -- k*x | i*x err , jump to where CATCH was called ) */
            if(TOS)
            {
                M_THROW(TOS);
            }
            else M_DROP;
            endcase;

#ifndef PF_NO_SHELL
        case ID_TICK:
            PUSH_TOS;
            CharPtr = (char *) ffWord( (char) ' ' );
            TOS = ffFind( CharPtr, (ExecToken *) &Temp );
            if( TOS == 0 )
            {
                ERR("' could not find ");
                ioType( (char *) CharPtr+1, *CharPtr );
                EMIT_CR;
                M_THROW(-13);
            }
            else
            {
                TOS = Temp;
            }
            endcase;
#endif  /* !PF_NO_SHELL */

        case ID_TIMES: BINARY_OP( * ); endcase;

        case ID_TYPE:
            Scratch = M_POP; /* addr */
            ioType( (char *) Scratch, TOS );
            M_DROP;
            endcase;

        case ID_TO_R:
            M_R_PUSH( TOS );
            M_DROP;
            endcase;

        case ID_VAR_BASE: DO_VAR(gVarBase); endcase;
        case ID_VAR_BYE_CODE: DO_VAR(gVarByeCode); endcase;
        case ID_VAR_CODE_BASE: DO_VAR(gCurrentDictionary->dic_CodeBase); endcase;
        case ID_VAR_CODE_LIMIT: DO_VAR(gCurrentDictionary->dic_CodeLimit); endcase;
        case ID_VAR_CONTEXT: DO_VAR(gVarContext); endcase;
        case ID_VAR_DP: DO_VAR(gCurrentDictionary->dic_CodePtr.Cell); endcase;
        case ID_VAR_ECHO: DO_VAR(gVarEcho); endcase;
        case ID_VAR_HEADERS_BASE: DO_VAR(gCurrentDictionary->dic_HeaderBase); endcase;
        case ID_VAR_HEADERS_LIMIT: DO_VAR(gCurrentDictionary->dic_HeaderLimit); endcase;
        case ID_VAR_HEADERS_PTR: DO_VAR(gCurrentDictionary->dic_HeaderPtr); endcase;
        case ID_VAR_NUM_TIB: DO_VAR(gCurrentTask->td_SourceNum); endcase;
        case ID_VAR_OUT: DO_VAR(gCurrentTask->td_OUT); endcase;
        case ID_VAR_STATE: DO_VAR(gVarState); endcase;
        case ID_VAR_TO_IN: DO_VAR(gCurrentTask->td_IN); endcase;
        case ID_VAR_TRACE_FLAGS: DO_VAR(gVarTraceFlags); endcase;
        case ID_VAR_TRACE_LEVEL: DO_VAR(gVarTraceLevel); endcase;
        case ID_VAR_TRACE_STACK: DO_VAR(gVarTraceStack); endcase;
        case ID_VAR_RETURN_CODE: DO_VAR(gVarReturnCode); endcase;

        case ID_VERSION_CODE:
            M_PUSH( TOS );
            TOS = PFORTH_VERSION_CODE;
            endcase;

        case ID_WORD:
            TOS = (cell_t) ffWord( (char) TOS );
            endcase;

        case ID_WORD_FETCH: /* ( waddr -- w ) */
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
            if( IN_DICS( TOS ) )
            {
                TOS = (uint16_t) READ_SHORT_DIC((uint16_t *)TOS);
            }
            else
            {
                TOS = *((uint16_t *)TOS);
            }
#else
            TOS = *((uint16_t *)TOS);
#endif
            endcase;

        case ID_WORD_STORE: /* ( w waddr -- ) */

#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
            if( IN_DICS( TOS ) )
            {
                WRITE_SHORT_DIC((uint16_t *)TOS,(uint16_t)M_POP);
            }
            else
            {
                *((uint16_t *)TOS) = (uint16_t) M_POP;
            }
#else
            *((uint16_t *)TOS) = (uint16_t) M_POP;
#endif
            M_DROP;
            endcase;

        case ID_XOR: BINARY_OP( ^ ); endcase;


/* Branch is followed by an offset relative to address of offset. */
        case ID_ZERO_BRANCH:
DBUGX(("Before 0Branch: IP = 0x%x\n", InsPtr ));
            if( TOS == 0 )
            {
                M_BRANCH;
            }
            else
            {
                InsPtr++;      /* skip over offset */
            }
            M_DROP;
DBUGX(("After 0Branch: IP = 0x%x\n", InsPtr ));
            endcase;

        default:
            ERR("pfCatch: Unrecognised token = 0x");
            ffDotHex(Token);
            ERR(" at 0x");
            ffDotHex((cell_t) InsPtr);
            EMIT_CR;
            InsPtr = 0;
            endcase;
        }

        if(InsPtr) Token = READ_CELL_DIC(InsPtr++);   /* Traverse to next token in secondary. */

#ifdef PF_DEBUG
        M_DOTS;
#endif

#if 0
        if( _CrtCheckMemory() == 0 )
        {
            ERR("_CrtCheckMemory abort: InsPtr = 0x");
            ffDotHex((int)InsPtr);
            ERR("\n");
        }
#endif

    } while( (InitialReturnStack - TORPTR) > 0 );

    SAVE_REGISTERS;

    return ExceptionReturnCode;
}
