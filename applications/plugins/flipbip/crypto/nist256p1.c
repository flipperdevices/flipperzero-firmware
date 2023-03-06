/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "nist256p1.h"

const ecdsa_curve nist256p1 = {
    /* .prime */ {/*.val =*/{0x1fffffff, 0x1fffffff, 0x1fffffff, 0x000001ff,
                             0x00000000, 0x00000000, 0x00040000, 0x1fe00000,
                             0xffffff}},

    /* G */
    {/*.x =*/{/*.val =*/{0x1898c296, 0x0509ca2e, 0x1acce83d, 0x06fb025b,
                         0x040f2770, 0x1372b1d2, 0x091fe2f3, 0x1e5c2588,
                         0x6b17d1}},
     /*.y =*/{/*.val =*/{0x17bf51f5, 0x1db20341, 0x0c57b3b2, 0x1c66aed6,
                         0x19e162bc, 0x15a53e07, 0x1e6e3b9f, 0x1c5fc34f,
                         0x4fe342}}},

    /* order */
    {/*.val =*/{0x1c632551, 0x1dce5617, 0x05e7a13c, 0x0df55b4e, 0x1ffffbce,
                0x1fffffff, 0x0003ffff, 0x1fe00000, 0xffffff}},

    /* order_half */
    {/*.val =*/{0x1e3192a8, 0x0ee72b0b, 0x02f3d09e, 0x06faada7, 0x1ffffde7,
                0x1fffffff, 0x0001ffff, 0x1ff00000, 0x7fffff}},

    /* a */ -3,

    /* b */
    {/*.val =*/{0x07d2604b, 0x1e71e1f1, 0x14ec3d8e, 0x1a0d6198, 0x086bc651,
                0x1eaabb4c, 0x0f9ecfae, 0x1b154752, 0x005ac635}}

#if USE_PRECOMPUTED_CP
    ,
    /* cp */
    {
#include "nist256p1.table"
    }
#endif
};

const curve_info nist256p1_info = {
    .bip32_name = "Nist256p1 seed",
    .params = &nist256p1,
    .hasher_base58 = HASHER_SHA2D,
    .hasher_sign = HASHER_SHA2D,
    .hasher_pubkey = HASHER_SHA2_RIPEMD,
    .hasher_script = HASHER_SHA2,
};
