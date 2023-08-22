/* @(#) pf_words.c 96/12/18 1.10 */
/***************************************************************
** Forth words for PForth based on 'C'
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
**
**  941031  rdg     fix ffScan() to look for CRs and LFs
**
***************************************************************/

#include "pf_all.h"


/***************************************************************
** Print number in current base to output stream.
** This version does not handle double precision.
*/
void ffDot( cell_t n )
{
    MSG( ConvertNumberToText( n, gVarBase, TRUE, 1 ) );
    EMIT(' ');
}

/***************************************************************
** Print number in current base to output stream.
** This version does not handle double precision.
*/
void ffDotHex( cell_t n )
{
    MSG( ConvertNumberToText( n, 16, FALSE, 1 ) );
    EMIT(' ');
}

/* ( ... --- ... , print stack ) */
void ffDotS( void )
{
    cell_t *sp;
    cell_t i, Depth;

    MSG("Stack<");
    MSG( ConvertNumberToText( gVarBase, 10, TRUE, 1 ) ); /* Print base in decimal. */
    MSG("> ");

    Depth = gCurrentTask->td_StackBase - gCurrentTask->td_StackPtr;
    sp = gCurrentTask->td_StackBase;

    if( Depth < 0 )
    {
        MSG("UNDERFLOW!");
    }
    else
    {
        for( i=0; i<Depth; i++ )
        {
/* Print as unsigned if not base 10. */
            MSG( ConvertNumberToText( *(--sp), gVarBase, (gVarBase == 10), 1 ) );
            EMIT(' ');
        }
    }
    MSG("\n");
}

/* ( addr cnt char -- addr' cnt' , skip leading characters ) */
cell_t ffSkip( char *AddrIn, cell_t Cnt, char c, char **AddrOut )
{
    char *s;

    s = AddrIn;

    if( c == BLANK )
    {
        while( ( Cnt > 0 ) &&
            (( *s == BLANK) || ( *s == '\t')) )
        {
DBUGX(("ffSkip BLANK: %c, %d\n", *s, Cnt ));
            s++;
            Cnt--;
        }
    }
    else
    {
        while(( Cnt > 0 ) && ( *s == c ))
        {
DBUGX(("ffSkip: %c=0x%x, %d\n", *s, Cnt ));
        s++;
        Cnt--;
        }
    }
    *AddrOut = s;
    return Cnt;
}

/* ( addr cnt char -- addr' cnt' , scan for char ) */
cell_t ffScan( char *AddrIn, cell_t Cnt, char c, char **AddrOut )
{
    char *s;

    s = AddrIn;

    if( c == BLANK )
    {
        while(( Cnt > 0 ) &&
            ( *s != BLANK) &&
            ( *s != '\r') &&
            ( *s != '\n') &&
            ( *s != '\t'))
        {
DBUGX(("ffScan BLANK: %c, %d\n", *s, Cnt ));
            s++;
            Cnt--;
        }
    }
    else
    {
        while(( Cnt > 0 ) && ( *s != c ))
        {
DBUGX(("ffScan: %c, %d\n", *s, Cnt ));
            s++;
            Cnt--;
        }
    }
    *AddrOut = s;
    return Cnt;
}

/***************************************************************
** Forth equivalent 'C' functions.
***************************************************************/

/* Convert a single digit to the corresponding hex number. */
static cell_t HexDigitToNumber( char c )
{
    if( (c >= '0') && (c <= '9') )
    {
        return( c - '0' );
    }
    else if ( (c >= 'A') && (c <= 'F') )
    {
        return( c - 'A' + 0x0A );
    }
    else
    {
        return -1;
    }
}

/* Convert a string to the corresponding number using BASE. */
cell_t ffNumberQ( const char *FWord, cell_t *Num )
{
    cell_t Len, i, Accum=0, n, Sign=1, Base=gVarBase;
    const char *s;

/* get count */
    Len = *FWord++;
    s = FWord;

    switch (*s) {
    case '#': Base = 10; s++; Len--; break;
    case '$': Base = 16; s++; Len--; break;
    case '%': Base =  2; s++; Len--; break;
    case '\'':
	if( Len == 3 && s[2] == '\'' )
	{
	    *Num = s[1];
	    return NUM_TYPE_SINGLE;
	}
    }

/* process initial minus sign */
    if( *s == '-' )
    {
        Sign = -1;
        s++;
        Len--;
    }

    for( i=0; i<Len; i++)
    {
        n = HexDigitToNumber( *s++ );
        if( (n < 0) || (n >= Base) )
        {
            return NUM_TYPE_BAD;
        }

        Accum = (Accum * Base) + n;
    }
    *Num = Accum * Sign;
    return NUM_TYPE_SINGLE;
}

/***************************************************************
** Compiler Support
***************************************************************/

/* Skip whitespace, then parse input delimited by C. If UPCASE is true
 * convert the word to upper case.  The result is stored in
 * gScratch.
 */
static char * Word ( char c, int Upcase )
{
    char *s1,*s2,*s3;
    cell_t n1, n2, n3;
    cell_t i, nc;

    s1 = gCurrentTask->td_SourcePtr + gCurrentTask->td_IN;
    n1 = gCurrentTask->td_SourceNum - gCurrentTask->td_IN;
    n2 = ffSkip( s1, n1, c, &s2 );
DBUGX(("Word: s2=%c, %d\n", *s2, n2 ));
    n3 = ffScan( s2, n2, c, &s3 );
DBUGX(("Word: s3=%c, %d\n", *s3, n3 ));
    nc = n2-n3;
    if (nc > 0)
    {
        gScratch[0] = (char) nc;
        for( i=0; i<nc; i++ )
        {
	    gScratch[i+1] = Upcase ? pfCharToUpper( s2[i] ) : s2[i] ;
        }
    }
    else
    {

        gScratch[0] = 0;
    }
    gCurrentTask->td_IN += (n1-n3) + 1;
    return &gScratch[0];
}

/* ( char -- c-addr , parse word ) */
char * ffWord( char c )
{
  return Word( c, TRUE );
}

/* ( char -- c-addr , parse word, preserving case ) */
char * ffLWord( char c )
{
  return Word( c, FALSE );
}
