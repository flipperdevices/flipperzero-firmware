/*  @(#) pf_unix.h 98/01/28 1.4 */
#ifndef _pf_embedded_h
#define _pf_embedded_h

/***************************************************************
** Embedded System include file for PForth, a Forth based on 'C'
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

#ifndef PF_NO_CLIB
    #include <string.h>    /* Needed for strlen(), memcpy(), and memset(). */
    #include <stdlib.h>    /* Needed for exit(). */
#endif

#ifdef PF_NO_STDIO
    #define NULL  ((void *) 0)
    #define EOF   (-1)
#else
    #include <stdio.h>
#endif

#ifdef PF_SUPPORT_FP
    #include <math.h>

    #ifndef PF_USER_FP
        #include "pf_float.h"
    #else
        #include PF_USER_FP
    #endif
#endif

#endif /* _pf_embedded_h */
