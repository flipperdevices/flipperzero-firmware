/* @(#) pf_mem.h 98/01/26 1.3 */
#ifndef _pf_mem_h
#define _pf_mem_h

/***************************************************************
** Include file for PForth Fake Memory Allocator
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
***************************************************************/

#ifdef PF_NO_MALLOC

    #ifdef __cplusplus
    extern "C" {
    #endif

    void  pfInitMemoryAllocator( void );
    char *pfAllocMem( cell_t NumBytes );
    void  pfFreeMem( void *Mem );

    #ifdef __cplusplus
    }
    #endif

#else

    #ifdef PF_USER_MALLOC
/* Get user prototypes or macros from include file.
** API must match that defined above for the stubs.
*/
        #include PF_USER_MALLOC
    #else
        #define pfInitMemoryAllocator()
        #define pfAllocMem malloc
        #define pfFreeMem free
    #endif

#endif /* PF_NO_MALLOC */

#endif /* _pf_mem_h */
