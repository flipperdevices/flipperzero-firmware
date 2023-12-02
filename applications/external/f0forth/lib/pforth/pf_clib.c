/* @(#) pf_clib.c 96/12/18 1.12 */
/***************************************************************
** Duplicate functions from stdlib for PForth based on 'C'
**
** This code duplicates some of the code in the 'C' lib
** because it reduces the dependency on foreign libraries
** for monitor mode where no OS is available.
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
** 961124 PLB Advance pointers in pfCopyMemory() and pfSetMemory()
***************************************************************/

#include "pf_all.h"

#ifdef PF_NO_CLIB
/* Count chars until NUL.  Replace strlen() */
#define  NUL  ((char) 0)
cell_t pfCStringLength( const char *s )
{
    cell_t len = 0;
    while( *s++ != NUL ) len++;
    return len;
}

/*    void *memset (void *s, cell_t c, size_t n); */
void *pfSetMemory( void *s, cell_t c, cell_t n )
{
    uint8_t *p = s, byt = (uint8_t) c;
    while( (n--) > 0) *p++ = byt;
    return s;
}

/*  void *memccpy (void *s1, const void *s2, cell_t c, size_t n); */
void *pfCopyMemory( void *s1, const void *s2, cell_t n)
{
    uint8_t *p1 = s1;
    const uint8_t *p2 = s2;
    while( (n--) > 0) *p1++ = *p2++;
    return s1;
}

#endif  /* PF_NO_CLIB */

char pfCharToUpper( char c )
{
    return (char) ( ((c>='a') && (c<='z')) ? (c - ('a' - 'A')) : c );
}

char pfCharToLower( char c )
{
    return (char) ( ((c>='A') && (c<='Z')) ? (c + ('a' - 'A')) : c );
}
