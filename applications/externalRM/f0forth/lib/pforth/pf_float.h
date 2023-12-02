/* @(#) pf_float.h 98/01/28 1.1 */
#ifndef _pf_float_h
#define _pf_float_h

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

typedef double PF_FLOAT;

/* Define pForth specific math functions. */

#define fp_acos   acos
#define fp_asin   asin
#define fp_atan   atan
#define fp_atan2  atan2
#define fp_cos    cos
#define fp_cosh   cosh
#define fp_fabs   fabs
#define fp_floor  floor
#define fp_log    log
#define fp_log10  log10
#define fp_pow    pow
#define fp_sin    sin
#define fp_sinh   sinh
#define fp_sqrt   sqrt
#define fp_tan    tan
#define fp_tanh   tanh
#define fp_round  round

#endif
