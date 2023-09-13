/* @(#) pf_types.h 96/12/18 1.3 */
#ifndef _pf_types_h
#define _pf_types_h

/***************************************************************
** Type declarations for PForth, a Forth based on 'C'
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

/***************************************************************
** Type Declarations
***************************************************************/

#if !defined(AMIGA) && !defined(ATARI)
#include <sys/types.h>
#endif

/* file_offset_t is used in place of off_t */
typedef long file_offset_t;

#ifndef Err
    typedef long Err;
#endif

typedef char  ForthString;
typedef char *ForthStringPtr;

#endif /* _pf_types_h */
