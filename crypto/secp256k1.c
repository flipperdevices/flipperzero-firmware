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

#include "secp256k1.h"

const ecdsa_curve secp256k1 = {
    /* .prime */ {/*.val =*/{0x1ffffc2f, 0x1ffffff7, 0x1fffffff, 0x1fffffff,
                             0x1fffffff, 0x1fffffff, 0x1fffffff, 0x1fffffff,
                             0xffffff}},

    /* G */
    {/*.x =*/{/*.val =*/{0x16f81798, 0x0f940ad8, 0x138a3656, 0x17f9b65b,
                         0x10b07029, 0x114ae743, 0x0eb15681, 0x0fdf3b97,
                         0x79be66}},
     /*.y =*/{/*.val =*/{0x1b10d4b8, 0x023e847f, 0x01550667, 0x0f68914d,
                         0x108a8fd1, 0x1dfe0708, 0x11957693, 0x0ee4d478,
                         0x483ada}}},

    /* order */
    {/*.val =*/{0x10364141, 0x1e92f466, 0x12280eef, 0x1db9cd5e, 0x1fffebaa,
                0x1fffffff, 0x1fffffff, 0x1fffffff, 0xffffff}},

    /* order_half */
    {/*.val =*/{0x081b20a0, 0x1f497a33, 0x09140777, 0x0edce6af, 0x1ffff5d5,
                0x1fffffff, 0x1fffffff, 0x1fffffff, 0x7fffff}},

    /* a */ 0,

    /* b */ {/*.val =*/{7}}

#if USE_PRECOMPUTED_CP
    ,
    /* cp */
    {
#include "secp256k1.table"
    }
#endif
};

const curve_info secp256k1_info = {
    .bip32_name = "Bitcoin seed",
    .params = &secp256k1,
    .hasher_base58 = HASHER_SHA2D,
    .hasher_sign = HASHER_SHA2D,
    .hasher_pubkey = HASHER_SHA2_RIPEMD,
    .hasher_script = HASHER_SHA2,
};

const curve_info secp256k1_decred_info = {
    .bip32_name = "Bitcoin seed",
    .params = &secp256k1,
    .hasher_base58 = HASHER_BLAKED,
    .hasher_sign = HASHER_BLAKE,
    .hasher_pubkey = HASHER_BLAKE_RIPEMD,
    .hasher_script = HASHER_BLAKE,
};

const curve_info secp256k1_groestl_info = {
    .bip32_name = "Bitcoin seed",
    .params = &secp256k1,
    .hasher_base58 = HASHER_GROESTLD_TRUNC,
    .hasher_sign = HASHER_SHA2,
    .hasher_pubkey = HASHER_SHA2_RIPEMD,
    .hasher_script = HASHER_SHA2,
};

const curve_info secp256k1_smart_info = {
    .bip32_name = "Bitcoin seed",
    .params = &secp256k1,
    .hasher_base58 = HASHER_SHA3K,
    .hasher_sign = HASHER_SHA2,
    .hasher_pubkey = HASHER_SHA2_RIPEMD,
    .hasher_script = HASHER_SHA2,
};
