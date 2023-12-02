/* @(#) pf_all.h 98/01/26 1.2 */

#ifndef _pf_all_h
#define _pf_all_h

/***************************************************************
** Include all files needed for PForth
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
** 940521 PLB Creation.
**
***************************************************************/

#ifdef PF_EMBEDDED
    #define PF_NO_INIT
    #define PF_NO_STDIO
    #define PF_NO_MALLOC
    #define PF_NO_CLIB
    #define PF_NO_FILEIO
#endif

/* I don't see any way to pass compiler flags to the Mac Code Warrior compiler! */
#ifdef __MWERKS__
    #define PF_SUPPORT_FP    (1)
#endif

#ifdef WIN32
    #define PF_USER_INC2     "pf_win32.h"
#endif

#if defined(PF_USER_INC1)
    #include PF_USER_INC1
#else
    #include "pf_inc1.h"
#endif

#include "pforth.h"
#include "pf_types.h"
#include "pf_io.h"
#include "pf_guts.h"
#include "pf_text.h"
#include "pfcompil.h"
#include "pf_clib.h"
#include "pf_words.h"
#include "pf_save.h"
#include "pf_mem.h"
#include "pf_cglue.h"
#include "pf_core.h"

#ifdef PF_USER_INC2
/* This could be used to undef and redefine macros. */
    #include PF_USER_INC2
#endif

#endif /* _pf_all_h */

