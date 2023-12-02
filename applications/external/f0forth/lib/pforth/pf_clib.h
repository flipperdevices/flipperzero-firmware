/* @(#) pf_clib.h 96/12/18 1.10 */
#ifndef _pf_clib_h
#define _pf_clib_h

/***************************************************************
** Include file for PForth tools
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

#ifdef  PF_NO_CLIB

    #ifdef __cplusplus
    extern "C" {
    #endif

    cell_t pfCStringLength( const char *s );
    void *pfSetMemory( void *s, cell_t c, cell_t n );
    void *pfCopyMemory( void *s1, const void *s2, cell_t n);
    #define EXIT(n)  {while(1);}

    #ifdef __cplusplus
    }
    #endif

#else   /* PF_NO_CLIB */

    #ifdef PF_USER_CLIB
        #include PF_USER_CLIB
    #else
/* Use stdlib functions if available because they are probably faster. */
        #define pfCStringLength strlen
        #define pfSetMemory     memset
        #define pfCopyMemory    memcpy
        #define EXIT(n)  exit(n)
    #endif /* PF_USER_CLIB */

#endif  /* !PF_NO_CLIB */

#ifdef __cplusplus
extern "C" {
#endif

/* Always use my own functions to avoid macro expansion problems with tolower(*s++) */
char pfCharToUpper( char c );
char pfCharToLower( char c );

#ifdef __cplusplus
}
#endif

#endif /* _pf_clib_h */
