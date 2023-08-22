/* @(#) pf_text.c 98/01/26 1.3 */
/***************************************************************
** Text Strings for Error Messages
** Various Text tools.
**
** For PForth based on 'C'
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
** 19970702 PLB Fixed ConvertNumberToText for unsigned numbers.
** 19980522 PLB Expand PAD for ConvertNumberToText so "-1 binary .s" doesn't crash.
***************************************************************/

#include "pf_all.h"

#define PF_ENGLISH

/*
** Define array of error messages.
** These are defined in one place to make it easier to translate them.
*/
#ifdef PF_ENGLISH
/***************************************************************/
void pfReportError( const char *FunctionName, Err ErrCode )
{
    const char *s;

    MSG("Error in ");
    MSG(FunctionName);
    MSG(" - ");

    switch(ErrCode & 0xFF)
    {
    case PF_ERR_NO_MEM & 0xFF:
        s = "insufficient memory"; break;
    case PF_ERR_TOO_BIG & 0xFF:
        s = "data chunk too large"; break;
    case PF_ERR_NUM_PARAMS & 0xFF:
        s = "incorrect number of parameters"; break;
    case PF_ERR_OPEN_FILE & 0xFF:
        s = "could not open file"; break;
    case PF_ERR_WRONG_FILE & 0xFF:
        s = "wrong type of file format"; break;
    case PF_ERR_BAD_FILE & 0xFF:
        s = "badly formatted file"; break;
    case PF_ERR_READ_FILE & 0xFF:
        s = "file read failed"; break;
    case PF_ERR_WRITE_FILE & 0xFF:
        s = "file write failed"; break;
    case PF_ERR_CORRUPT_DIC & 0xFF:
        s = "corrupted dictionary"; break;
    case PF_ERR_NOT_SUPPORTED & 0xFF:
        s = "not supported in this version"; break;
    case PF_ERR_VERSION_FUTURE & 0xFF:
        s = "version from future"; break;
    case PF_ERR_VERSION_PAST & 0xFF:
        s = "version is obsolete. Rebuild new one."; break;
    case PF_ERR_COLON_STACK & 0xFF:
        s = "stack depth changed between : and ; . Probably unbalanced conditional"; break;
    case PF_ERR_HEADER_ROOM & 0xFF:
        s = "no room left in header space"; break;
    case PF_ERR_CODE_ROOM & 0xFF:
        s = "no room left in code space"; break;
    case PF_ERR_NO_SHELL & 0xFF:
        s = "attempt to use names in forth compiled with PF_NO_SHELL"; break;
    case PF_ERR_NO_NAMES & 0xFF:
        s = "dictionary has no names";  break;
    case PF_ERR_OUT_OF_RANGE & 0xFF:
        s = "parameter out of range";  break;
    case PF_ERR_ENDIAN_CONFLICT & 0xFF:
        s = "endian-ness of dictionary does not match code";  break;
    case PF_ERR_FLOAT_CONFLICT & 0xFF:
        s = "float support mismatch between .dic file and code";  break;
    case PF_ERR_CELL_SIZE_CONFLICT & 0xFF:
        s = "cell size mismatch between .dic file and code";  break;
    default:
        s = "unrecognized error code!"; break;
    }
    MSG(s);
    EMIT_CR;
}

void pfReportThrow( ThrowCode code )
{
    const char *s = NULL;
    switch(code)
    {
    case THROW_ABORT:
    case THROW_ABORT_QUOTE:
        s = "ABORT"; break;
    case THROW_STACK_OVERFLOW:
        s = "Stack overflow!"; break;
    case THROW_STACK_UNDERFLOW:
        s = "Stack underflow!"; break;
    case THROW_EXECUTING:
        s = "Executing a compile-only word!"; break;
    case THROW_FLOAT_STACK_UNDERFLOW:
        s = "Float Stack underflow!"; break;
    case THROW_UNDEFINED_WORD:
        s = "Undefined word!"; break;
    case THROW_PAIRS:
        s = "Conditional control structure mismatch!"; break;
    case THROW_BYE:
    case THROW_QUIT:
        break;
    case THROW_SEMICOLON:
        s = "Stack depth changed between : and ; . Probably unbalanced conditional!"; break;
    case THROW_DEFERRED:
        s = "Not a DEFERred word!"; break;
    default:
        s = "Unrecognized throw code!"; break;
    }

    if( s )
    {
        MSG_NUM_D("THROW code = ", code );
        MSG(s);
        EMIT_CR;
    }
}
#endif

/**************************************************************
** Copy a Forth String to a 'C' string.
*/

char *ForthStringToC( char *dst, const char *FString, cell_t dstSize )
{
    cell_t Len;

    Len = (cell_t) *FString;
    /* Make sure the text + NUL can fit. */
    if( Len >= dstSize )
    {
        Len = dstSize - 1;
    }
    pfCopyMemory( dst, FString+1, Len );
    dst[Len] = '\0';

    return dst;
}

/**************************************************************
** Copy a NUL terminated string to a Forth counted string.
*/
char *CStringToForth( char *dst, const char *CString, cell_t dstSize )
{
    cell_t i;

    /* Make sure the SIZE+text can fit. */
    for( i=1; i<dstSize; i++ )
    {
        if( *CString == 0 )
        {
            break;
        }
        dst[i] = *CString++;
    }
    *dst = (char ) i-1;
    return dst;
}

/**************************************************************
** Compare two test strings, case sensitive.
** Return TRUE if they match.
*/
cell_t ffCompareText( const char *s1, const char *s2, cell_t len )
{
    cell_t i, Result;

    Result = TRUE;
    for( i=0; i<len; i++ )
    {
DBUGX(("ffCompareText: *s1 = 0x%x, *s2 = 0x%x\n", *s1, *s2 ));
        if( *s1++ != *s2++ )
        {
            Result = FALSE;
            break;
        }
    }
DBUGX(("ffCompareText: return 0x%x\n", Result ));
    return Result;
}

/**************************************************************
** Compare two test strings, case INsensitive.
** Return TRUE if they match.
*/
cell_t ffCompareTextCaseN( const char *s1, const char *s2, cell_t len )
{
    cell_t i, Result;
    char  c1,c2;

    Result = TRUE;
    for( i=0; i<len; i++ )
    {
        c1 = pfCharToLower(*s1++);
        c2 = pfCharToLower(*s2++);
DBUGX(("ffCompareText: c1 = 0x%x, c2 = 0x%x\n", c1, c2 ));
        if( c1 != c2 )
        {
            Result = FALSE;
            break;
        }
    }
DBUGX(("ffCompareText: return 0x%x\n", Result ));
    return Result;
}

/**************************************************************
** Compare two strings, case sensitive.
** Return zero if they match, -1 if s1<s2, +1 is s1>s2;
*/
cell_t ffCompare( const char *s1, cell_t len1, const char *s2, cell_t len2 )
{
    cell_t i, result, n, diff;

    result = 0;
    n = MIN(len1,len2);
    for( i=0; i<n; i++ )
    {
        if( (diff = (*s2++ - *s1++)) != 0 )
        {
            result = (diff > 0) ? -1 : 1 ;
            break;
        }
    }
    if( result == 0 )  /* Match up to MIN(len1,len2) */
    {
        if( len1 < len2 )
        {
            result = -1;
        }
        else if ( len1 > len2 )
        {
            result = 1;
        }
    }
    return result;
}

/***************************************************************
** Convert number to text.
*/
#define CNTT_PAD_SIZE ((sizeof(cell_t)*8)+2)  /* PLB 19980522 - Expand PAD so "-1 binary .s" doesn't crash. */
static char cnttPad[CNTT_PAD_SIZE];

char *ConvertNumberToText( cell_t Num, cell_t Base, int32_t IfSigned, int32_t MinChars )
{
    cell_t IfNegative = 0;
    char *p,c;
    ucell_t NewNum, Rem, uNum;
    cell_t i = 0;

    uNum = Num;
    if( IfSigned )
    {
/* Convert to positive and keep sign. */
        if( Num < 0 )
        {
            IfNegative = TRUE;
            uNum = -Num;
        }
    }

/* Point past end of Pad */
    p = cnttPad + CNTT_PAD_SIZE;
    *(--p) = (char) 0; /* NUL terminate */

    while( (i++<MinChars) || (uNum != 0) )
    {
        NewNum = uNum / Base;
        Rem = uNum - (NewNum * Base);
        c = (char) (( Rem < 10 ) ? (Rem + '0') : (Rem - 10 + 'A'));
        *(--p) = c;
        uNum = NewNum;
    }

    if( IfSigned )
    {
        if( IfNegative ) *(--p) = '-';
    }
    return p;
}

/***************************************************************
** Diagnostic routine that prints memory in table format.
*/
void DumpMemory( void *addr, cell_t cnt)
{
    cell_t ln, cn, nlines;
    unsigned char *ptr, *cptr, c;

    nlines = (cnt + 15) / 16;

    ptr = (unsigned char *) addr;

    EMIT_CR;

    for (ln=0; ln<nlines; ln++)
    {
        MSG( ConvertNumberToText( (cell_t) ptr, 16, FALSE, 8 ) );
        MSG(": ");
        cptr = ptr;
        for (cn=0; cn<16; cn++)
        {
            MSG( ConvertNumberToText( (cell_t) *cptr++, 16, FALSE, 2 ) );
            EMIT(' ');
        }
        EMIT(' ');
        for (cn=0; cn<16; cn++)
        {
            c = *ptr++;
            if ((c < ' ') || (c > '}')) c = '.';
            EMIT(c);
        }
        EMIT_CR;
    }
}


/* Print name, mask off any dictionary bits. */
void TypeName( const char *Name )
{
    const char *FirstChar;
    cell_t Len;

    FirstChar = Name+1;
    Len = *Name & 0x1F;

    ioType( FirstChar, Len );
}



#ifdef PF_UNIT_TEST
/* Unit test for string conversion routines. */
#define ASSERT_PAD_IS( index, value, msg ) \
    if( pad[index] != ((char)(value)) ) \
    { \
        ERR(( "ERROR text test failed: " msg "\n")); \
        numErrors += 1; \
    } \

cell_t pfUnitTestText( void )
{
    cell_t numErrors = 0;
    char pad[16];
    char fpad[8];

    /* test CStringToForth */
    pfSetMemory(pad,0xA5,sizeof(pad));
    CStringToForth( pad, "frog", 6 );
    ASSERT_PAD_IS( 0, 4, "CS len 6" );
    ASSERT_PAD_IS( 4, 'g', "CS end 6" );
    ASSERT_PAD_IS( 5, 0xA5, "CS past 6" );

    pfSetMemory(pad,0xA5,sizeof(pad));
    CStringToForth( pad, "frog", 5 );
    ASSERT_PAD_IS( 0, 4, "CS len 5" );
    ASSERT_PAD_IS( 4, 'g', "CS end 5" );
    ASSERT_PAD_IS( 5, 0xA5, "CS past 5" );

    pfSetMemory(pad,0xA5,sizeof(pad));
    CStringToForth( pad, "frog", 4 );
    ASSERT_PAD_IS( 0, 3, "CS len 4" );
    ASSERT_PAD_IS( 3, 'o', "CS end 4" );
    ASSERT_PAD_IS( 4, 0xA5, "CS past 4" );

    /* Make a Forth string for testing ForthStringToC. */
    CStringToForth( fpad, "frog", sizeof(fpad) );

    pfSetMemory(pad,0xA5,sizeof(pad));
    ForthStringToC( pad, fpad, 6 );
    ASSERT_PAD_IS( 0, 'f', "FS len 6" );
    ASSERT_PAD_IS( 3, 'g', "FS end 6" );
    ASSERT_PAD_IS( 4, 0, "FS nul 6" );
    ASSERT_PAD_IS( 5, 0xA5, "FS past 6" );

    pfSetMemory(pad,0xA5,sizeof(pad));
    ForthStringToC( pad, fpad, 5 );
    ASSERT_PAD_IS( 0, 'f', "FS len 5" );
    ASSERT_PAD_IS( 3, 'g', "FS end 5" );
    ASSERT_PAD_IS( 4, 0, "FS nul 5" );
    ASSERT_PAD_IS( 5, 0xA5, "FS past 5" );

    pfSetMemory(pad,0xA5,sizeof(pad));
    ForthStringToC( pad, fpad, 4 );
    ASSERT_PAD_IS( 0, 'f', "FS len 4" );
    ASSERT_PAD_IS( 2, 'o', "FS end 4" );
    ASSERT_PAD_IS( 3, 0, "FS nul 4" );
    ASSERT_PAD_IS( 4, 0xA5, "FS past 4" );

    return numErrors;
}
#endif
