/* w64wrapper.c w64wrapper unit tests
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */
#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <tests/unit.h>

#ifdef WOLFSSL_W64_WRAPPER

#ifndef NO_INLINE
#define WOLFSSL_MISC_INCLUDED
#include <wolfcrypt/src/misc.c>
#else
#include <wolfssl/wolfcrypt/misc.h>
#endif

int w64wrapper_test(void)
{
    w64wrapper a, b, c;
    byte wrap, raw[8];

    a = w64From32(0x01020304, 0x05060708);
#if defined(WORD64_AVAILABLE) && !defined(WOLFSSL_W64_WRAPPER_TEST)
    if (a.n != 0x0102030405060708)
        return -1;
#else
    if (a.n[0] != 0x01020304 || a.n[1] != 0x05060708)
        return -1;
#endif /* WORD64_AVAILABLE && WOLFSSL_W64_WRAPPER_TEST */

    if (w64GetLow32(a) != 0x05060708)
        return -2;
    if (w64GetHigh32(a) != 0x01020304)
        return -3;
    w64SetLow32(&a, 0xabcdefff);
    if (w64GetLow32(a) != 0xabcdefff || w64GetHigh32(a) != 0x01020304)
        return -4;

    a = w64From32(0,0);
    w64Increment(&a);
    if (w64GetLow32(a) != 1 || w64GetHigh32(a) != 0)
        return -5;

    a = w64From32(0, 0xffffffff);
    w64Increment(&a);
    if (w64GetLow32(a) != 0 || w64GetHigh32(a) != 1)
        return -6;

    a = w64From32(0,1);
    w64Decrement(&a);
    if (w64GetLow32(a) != 0 || w64GetHigh32(a) != 0)
        return -7;

    a = w64From32(1,0);
    w64Decrement(&a);
    if (w64GetLow32(a) != 0xffffffff || w64GetHigh32(a) != 0)
        return -8;

    a = w64From32(0xabcdef, 0xdeed);
    b = w64From32(0xabcdef, 0xdeed);
    if (!w64Equal(a, b))
        return -9;

    a = w64From32(1, 1);
    b = w64From32(0, 1);
    if (w64Equal(a, b))
        return -10;

    wrap = 0;
    a = w64From32(0x0, 0x1);
    b = w64Add32(a, 0x1, &wrap);
    if (w64GetLow32(b) != 0x2 || w64GetHigh32(b) != 0x0 || wrap)
        return -11;

    wrap = 0;
    a = w64From32(0x0, 0xffffffff);
    b = w64Add32(a, 0x1, &wrap);
    if (w64GetLow32(b) != 0x0 || w64GetHigh32(b) != 0x01 || wrap)
        return -12;

    wrap = 0;
    a = w64From32(0xffffffff, 0xffffffff);
    b = w64Add32(a, 0x1, &wrap);
    if (w64GetLow32(b) != 0x0 || w64GetHigh32(b) != 0x00 || !wrap)
        return -13;

    wrap = 0;
    a = w64From32(0x0, 0x1);
    b = w64Sub32(a, 0x1, &wrap);
    if (w64GetLow32(b) != 0x0 || w64GetHigh32(b) != 0x00 || wrap)
        return -14;

    wrap = 0;
    a = w64From32(0xffffffff, 0x0);
    b = w64Sub32(a, 0x1, &wrap);
    if (w64GetLow32(b) != 0xffffffff ||
            w64GetHigh32(b) != 0xfffffffe || wrap)
        return -15;

    wrap = 0;
    a = w64From32(0x0, 0x0);
    b = w64Sub32(a, 0x1, &wrap);
    if (w64GetLow32(b) != 0xffffffff ||
        w64GetHigh32(b) != 0xffffffff || !wrap)
        return -16;

    a = w64From32(0x0, 0x0);
    b = w64From32(0x0, 0x0);
    if (w64GT(a,b) || w64GT(b,a) || !w64GTE(a,b) || w64LT(a,b) || w64LT(b,a))
        return -17;

    a = w64From32(0x0, 0x1);
    b = w64From32(0x0, 0x0);
    if (!w64GT(a, b) || w64GT(b, a) || !w64GTE(a, b) || w64GTE(b, a) ||
        w64LT(a, b) || !w64LT(b, a))
        return -18;

    a = w64From32(0x1, 0x0);
    b = w64From32(0x0, 0x0);
    if (!w64GT(a, b) || w64GT(b, a) || !w64GTE(a, b) || w64GTE(b, a) ||
        !w64LT(b, a) || w64LT(a, b))
        return -19;

    a = w64From32(0x1, 0x0);
    b = w64From32(0x1, 0x0);
    if (w64GT(a,b) || w64GT(b,a) || !w64GTE(a,b) || w64LT(a,b))
        return -20;

    a = w64From32(0x1, 0x1);
    b = w64From32(0x1, 0x0);
    if (!w64GT(a, b) || w64GT(b, a) || !w64GTE(a, b) || w64GTE(b, a) ||
        w64LT(a, b) || !w64LT(b, a))
        return -21;

    a = w64From32(0x2, 0x1);
    b = w64From32(0x1, 0x3);
    if (!w64GT(a, b) || w64GT(b, a) || !w64GTE(a, b) || w64GTE(b, a) ||
        w64LT(a, b) || !w64LT(b, a))
        return -22;

    a = w64From32(0x0, 0x0);
    if (!w64IsZero(a))
        return -23;

    a = w64From32(0x01020304, 0x05060708);
    c64toa(&a, raw);
    if (raw[0] != 0x01
        ||raw[1] != 0x02
        ||raw[2] != 0x03
        ||raw[3] != 0x04
        ||raw[4] != 0x05
        ||raw[5] != 0x06
        ||raw[6] != 0x07
        ||raw[7] != 0x08) {
        return -24;
    }

    b = w64From32(0x0,0x0);
    ato64(raw, &b);
    if (w64GetHigh32(b) != 0x01020304 || w64GetLow32(b) != 0x05060708)
        return -25;

    w64Zero(&b);
    if (w64GetHigh32(b) != 0x0 || w64GetLow32(b) != 0x0)
        return -26;

    b = w64From32(0x0, 0xffffffff);
    w64Increment(&b);
    if (w64GetHigh32(b) != 0x1 || w64GetLow32(b) != 0x0)
        return -27;
    b = w64From32(0xffffffff, 0xffffffff);
    w64Increment(&b);
    if (w64GetHigh32(b) != 0x0 || w64GetLow32(b) != 0x0)
        return -28;
    b = w64From32(0xffffffff, 0x0);
    w64Decrement(&b);
    if (w64GetHigh32(b) != 0xfffffffe || w64GetLow32(b) != 0xffffffff)
        return -29;

    a = w64From32(0x01, 0x20);
    b = w64From32(0x01, 0x10);
    c = w64Sub(a,b);
    if (w64GetHigh32(c) != 0x0 || w64GetLow32(c) != 0x10)
        return -30;
    c = w64Sub(b,a);
    if (w64GetHigh32(c) != 0xffffffff || w64GetLow32(c) != 0xfffffff0)
        return -31;

    a = w64From32(0x01, 0x10);
    b = w64From32(0x00, 0x20);
    c = w64Sub(a,b);
    if (w64GetHigh32(c) != 0x00000000 || w64GetLow32(c) != 0xfffffff0)
        return -32;

    return 0;
}

#endif /* WOLFSSL_W64_WRAPPER */
