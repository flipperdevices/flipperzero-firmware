/* Groestl hash from https://github.com/Groestlcoin/vanitygen
 * Trezor adaptation by Yura Pakhuchiy <pakhuchiy@gmail.com>. */
/*
 * Groestl implementation.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#include <stddef.h>
#include <string.h>

#include "groestl_internal.h"
#include "groestl.h"
#include "memzero.h"


#define C32e(x)     ((SPH_C32(x) >> 24) \
                    | ((SPH_C32(x) >>  8) & SPH_C32(0x0000FF00)) \
                    | ((SPH_C32(x) <<  8) & SPH_C32(0x00FF0000)) \
                    | ((SPH_C32(x) << 24) & SPH_C32(0xFF000000)))
#define dec32e_aligned   sph_dec32le_aligned
#define enc32e           sph_enc32le
#define B32_0(x)    ((x) & 0xFF)
#define B32_1(x)    (((x) >> 8) & 0xFF)
#define B32_2(x)    (((x) >> 16) & 0xFF)
#define B32_3(x)    ((x) >> 24)

#define R32u(u, d)   SPH_T32(((u) << 16) | ((d) >> 16))
#define R32d(u, d)   SPH_T32(((u) >> 16) | ((d) << 16))

#define PC32up(j, r)   ((sph_u32)((j) + (r)))
#define PC32dn(j, r)   0
#define QC32up(j, r)   SPH_C32(0xFFFFFFFF)
#define QC32dn(j, r)   (((sph_u32)(r) << 24) ^ SPH_T32(~((sph_u32)(j) << 24)))

#define C64e(x)     ((SPH_C64(x) >> 56) \
                    | ((SPH_C64(x) >> 40) & SPH_C64(0x000000000000FF00)) \
                    | ((SPH_C64(x) >> 24) & SPH_C64(0x0000000000FF0000)) \
                    | ((SPH_C64(x) >>  8) & SPH_C64(0x00000000FF000000)) \
                    | ((SPH_C64(x) <<  8) & SPH_C64(0x000000FF00000000)) \
                    | ((SPH_C64(x) << 24) & SPH_C64(0x0000FF0000000000)) \
                    | ((SPH_C64(x) << 40) & SPH_C64(0x00FF000000000000)) \
                    | ((SPH_C64(x) << 56) & SPH_C64(0xFF00000000000000)))
#define dec64e_aligned   sph_dec64le_aligned
#define enc64e           sph_enc64le
#define B64_0(x)    ((x) & 0xFF)
#define B64_1(x)    (((x) >> 8) & 0xFF)
#define B64_2(x)    (((x) >> 16) & 0xFF)
#define B64_3(x)    (((x) >> 24) & 0xFF)
#define B64_4(x)    (((x) >> 32) & 0xFF)
#define B64_5(x)    (((x) >> 40) & 0xFF)
#define B64_6(x)    (((x) >> 48) & 0xFF)
#define B64_7(x)    ((x) >> 56)
#define R64         SPH_ROTL64
#define PC64(j, r)  ((sph_u64)((j) + (r)))
#define QC64(j, r)  (((sph_u64)(r) << 56) ^ SPH_T64(~((sph_u64)(j) << 56)))


static const sph_u32 T0up[] = {
	C32e(0xc632f4a5), C32e(0xf86f9784), C32e(0xee5eb099), C32e(0xf67a8c8d),
	C32e(0xffe8170d), C32e(0xd60adcbd), C32e(0xde16c8b1), C32e(0x916dfc54),
	C32e(0x6090f050), C32e(0x02070503), C32e(0xce2ee0a9), C32e(0x56d1877d),
	C32e(0xe7cc2b19), C32e(0xb513a662), C32e(0x4d7c31e6), C32e(0xec59b59a),
	C32e(0x8f40cf45), C32e(0x1fa3bc9d), C32e(0x8949c040), C32e(0xfa689287),
	C32e(0xefd03f15), C32e(0xb29426eb), C32e(0x8ece40c9), C32e(0xfbe61d0b),
	C32e(0x416e2fec), C32e(0xb31aa967), C32e(0x5f431cfd), C32e(0x456025ea),
	C32e(0x23f9dabf), C32e(0x535102f7), C32e(0xe445a196), C32e(0x9b76ed5b),
	C32e(0x75285dc2), C32e(0xe1c5241c), C32e(0x3dd4e9ae), C32e(0x4cf2be6a),
	C32e(0x6c82ee5a), C32e(0x7ebdc341), C32e(0xf5f30602), C32e(0x8352d14f),
	C32e(0x688ce45c), C32e(0x515607f4), C32e(0xd18d5c34), C32e(0xf9e11808),
	C32e(0xe24cae93), C32e(0xab3e9573), C32e(0x6297f553), C32e(0x2a6b413f),
	C32e(0x081c140c), C32e(0x9563f652), C32e(0x46e9af65), C32e(0x9d7fe25e),
	C32e(0x30487828), C32e(0x37cff8a1), C32e(0x0a1b110f), C32e(0x2febc4b5),
	C32e(0x0e151b09), C32e(0x247e5a36), C32e(0x1badb69b), C32e(0xdf98473d),
	C32e(0xcda76a26), C32e(0x4ef5bb69), C32e(0x7f334ccd), C32e(0xea50ba9f),
	C32e(0x123f2d1b), C32e(0x1da4b99e), C32e(0x58c49c74), C32e(0x3446722e),
	C32e(0x3641772d), C32e(0xdc11cdb2), C32e(0xb49d29ee), C32e(0x5b4d16fb),
	C32e(0xa4a501f6), C32e(0x76a1d74d), C32e(0xb714a361), C32e(0x7d3449ce),
	C32e(0x52df8d7b), C32e(0xdd9f423e), C32e(0x5ecd9371), C32e(0x13b1a297),
	C32e(0xa6a204f5), C32e(0xb901b868), C32e(0x00000000), C32e(0xc1b5742c),
	C32e(0x40e0a060), C32e(0xe3c2211f), C32e(0x793a43c8), C32e(0xb69a2ced),
	C32e(0xd40dd9be), C32e(0x8d47ca46), C32e(0x671770d9), C32e(0x72afdd4b),
	C32e(0x94ed79de), C32e(0x98ff67d4), C32e(0xb09323e8), C32e(0x855bde4a),
	C32e(0xbb06bd6b), C32e(0xc5bb7e2a), C32e(0x4f7b34e5), C32e(0xedd73a16),
	C32e(0x86d254c5), C32e(0x9af862d7), C32e(0x6699ff55), C32e(0x11b6a794),
	C32e(0x8ac04acf), C32e(0xe9d93010), C32e(0x040e0a06), C32e(0xfe669881),
	C32e(0xa0ab0bf0), C32e(0x78b4cc44), C32e(0x25f0d5ba), C32e(0x4b753ee3),
	C32e(0xa2ac0ef3), C32e(0x5d4419fe), C32e(0x80db5bc0), C32e(0x0580858a),
	C32e(0x3fd3ecad), C32e(0x21fedfbc), C32e(0x70a8d848), C32e(0xf1fd0c04),
	C32e(0x63197adf), C32e(0x772f58c1), C32e(0xaf309f75), C32e(0x42e7a563),
	C32e(0x20705030), C32e(0xe5cb2e1a), C32e(0xfdef120e), C32e(0xbf08b76d),
	C32e(0x8155d44c), C32e(0x18243c14), C32e(0x26795f35), C32e(0xc3b2712f),
	C32e(0xbe8638e1), C32e(0x35c8fda2), C32e(0x88c74fcc), C32e(0x2e654b39),
	C32e(0x936af957), C32e(0x55580df2), C32e(0xfc619d82), C32e(0x7ab3c947),
	C32e(0xc827efac), C32e(0xba8832e7), C32e(0x324f7d2b), C32e(0xe642a495),
	C32e(0xc03bfba0), C32e(0x19aab398), C32e(0x9ef668d1), C32e(0xa322817f),
	C32e(0x44eeaa66), C32e(0x54d6827e), C32e(0x3bdde6ab), C32e(0x0b959e83),
	C32e(0x8cc945ca), C32e(0xc7bc7b29), C32e(0x6b056ed3), C32e(0x286c443c),
	C32e(0xa72c8b79), C32e(0xbc813de2), C32e(0x1631271d), C32e(0xad379a76),
	C32e(0xdb964d3b), C32e(0x649efa56), C32e(0x74a6d24e), C32e(0x1436221e),
	C32e(0x92e476db), C32e(0x0c121e0a), C32e(0x48fcb46c), C32e(0xb88f37e4),
	C32e(0x9f78e75d), C32e(0xbd0fb26e), C32e(0x43692aef), C32e(0xc435f1a6),
	C32e(0x39dae3a8), C32e(0x31c6f7a4), C32e(0xd38a5937), C32e(0xf274868b),
	C32e(0xd5835632), C32e(0x8b4ec543), C32e(0x6e85eb59), C32e(0xda18c2b7),
	C32e(0x018e8f8c), C32e(0xb11dac64), C32e(0x9cf16dd2), C32e(0x49723be0),
	C32e(0xd81fc7b4), C32e(0xacb915fa), C32e(0xf3fa0907), C32e(0xcfa06f25),
	C32e(0xca20eaaf), C32e(0xf47d898e), C32e(0x476720e9), C32e(0x10382818),
	C32e(0x6f0b64d5), C32e(0xf0738388), C32e(0x4afbb16f), C32e(0x5cca9672),
	C32e(0x38546c24), C32e(0x575f08f1), C32e(0x732152c7), C32e(0x9764f351),
	C32e(0xcbae6523), C32e(0xa125847c), C32e(0xe857bf9c), C32e(0x3e5d6321),
	C32e(0x96ea7cdd), C32e(0x611e7fdc), C32e(0x0d9c9186), C32e(0x0f9b9485),
	C32e(0xe04bab90), C32e(0x7cbac642), C32e(0x712657c4), C32e(0xcc29e5aa),
	C32e(0x90e373d8), C32e(0x06090f05), C32e(0xf7f40301), C32e(0x1c2a3612),
	C32e(0xc23cfea3), C32e(0x6a8be15f), C32e(0xaebe10f9), C32e(0x69026bd0),
	C32e(0x17bfa891), C32e(0x9971e858), C32e(0x3a536927), C32e(0x27f7d0b9),
	C32e(0xd9914838), C32e(0xebde3513), C32e(0x2be5ceb3), C32e(0x22775533),
	C32e(0xd204d6bb), C32e(0xa9399070), C32e(0x07878089), C32e(0x33c1f2a7),
	C32e(0x2decc1b6), C32e(0x3c5a6622), C32e(0x15b8ad92), C32e(0xc9a96020),
	C32e(0x875cdb49), C32e(0xaab01aff), C32e(0x50d88878), C32e(0xa52b8e7a),
	C32e(0x03898a8f), C32e(0x594a13f8), C32e(0x09929b80), C32e(0x1a233917),
	C32e(0x651075da), C32e(0xd7845331), C32e(0x84d551c6), C32e(0xd003d3b8),
	C32e(0x82dc5ec3), C32e(0x29e2cbb0), C32e(0x5ac39977), C32e(0x1e2d3311),
	C32e(0x7b3d46cb), C32e(0xa8b71ffc), C32e(0x6d0c61d6), C32e(0x2c624e3a)
};

static const sph_u32 T0dn[] = {
	C32e(0xf497a5c6), C32e(0x97eb84f8), C32e(0xb0c799ee), C32e(0x8cf78df6),
	C32e(0x17e50dff), C32e(0xdcb7bdd6), C32e(0xc8a7b1de), C32e(0xfc395491),
	C32e(0xf0c05060), C32e(0x05040302), C32e(0xe087a9ce), C32e(0x87ac7d56),
	C32e(0x2bd519e7), C32e(0xa67162b5), C32e(0x319ae64d), C32e(0xb5c39aec),
	C32e(0xcf05458f), C32e(0xbc3e9d1f), C32e(0xc0094089), C32e(0x92ef87fa),
	C32e(0x3fc515ef), C32e(0x267febb2), C32e(0x4007c98e), C32e(0x1ded0bfb),
	C32e(0x2f82ec41), C32e(0xa97d67b3), C32e(0x1cbefd5f), C32e(0x258aea45),
	C32e(0xda46bf23), C32e(0x02a6f753), C32e(0xa1d396e4), C32e(0xed2d5b9b),
	C32e(0x5deac275), C32e(0x24d91ce1), C32e(0xe97aae3d), C32e(0xbe986a4c),
	C32e(0xeed85a6c), C32e(0xc3fc417e), C32e(0x06f102f5), C32e(0xd11d4f83),
	C32e(0xe4d05c68), C32e(0x07a2f451), C32e(0x5cb934d1), C32e(0x18e908f9),
	C32e(0xaedf93e2), C32e(0x954d73ab), C32e(0xf5c45362), C32e(0x41543f2a),
	C32e(0x14100c08), C32e(0xf6315295), C32e(0xaf8c6546), C32e(0xe2215e9d),
	C32e(0x78602830), C32e(0xf86ea137), C32e(0x11140f0a), C32e(0xc45eb52f),
	C32e(0x1b1c090e), C32e(0x5a483624), C32e(0xb6369b1b), C32e(0x47a53ddf),
	C32e(0x6a8126cd), C32e(0xbb9c694e), C32e(0x4cfecd7f), C32e(0xbacf9fea),
	C32e(0x2d241b12), C32e(0xb93a9e1d), C32e(0x9cb07458), C32e(0x72682e34),
	C32e(0x776c2d36), C32e(0xcda3b2dc), C32e(0x2973eeb4), C32e(0x16b6fb5b),
	C32e(0x0153f6a4), C32e(0xd7ec4d76), C32e(0xa37561b7), C32e(0x49face7d),
	C32e(0x8da47b52), C32e(0x42a13edd), C32e(0x93bc715e), C32e(0xa2269713),
	C32e(0x0457f5a6), C32e(0xb86968b9), C32e(0x00000000), C32e(0x74992cc1),
	C32e(0xa0806040), C32e(0x21dd1fe3), C32e(0x43f2c879), C32e(0x2c77edb6),
	C32e(0xd9b3bed4), C32e(0xca01468d), C32e(0x70ced967), C32e(0xdde44b72),
	C32e(0x7933de94), C32e(0x672bd498), C32e(0x237be8b0), C32e(0xde114a85),
	C32e(0xbd6d6bbb), C32e(0x7e912ac5), C32e(0x349ee54f), C32e(0x3ac116ed),
	C32e(0x5417c586), C32e(0x622fd79a), C32e(0xffcc5566), C32e(0xa7229411),
	C32e(0x4a0fcf8a), C32e(0x30c910e9), C32e(0x0a080604), C32e(0x98e781fe),
	C32e(0x0b5bf0a0), C32e(0xccf04478), C32e(0xd54aba25), C32e(0x3e96e34b),
	C32e(0x0e5ff3a2), C32e(0x19bafe5d), C32e(0x5b1bc080), C32e(0x850a8a05),
	C32e(0xec7ead3f), C32e(0xdf42bc21), C32e(0xd8e04870), C32e(0x0cf904f1),
	C32e(0x7ac6df63), C32e(0x58eec177), C32e(0x9f4575af), C32e(0xa5846342),
	C32e(0x50403020), C32e(0x2ed11ae5), C32e(0x12e10efd), C32e(0xb7656dbf),
	C32e(0xd4194c81), C32e(0x3c301418), C32e(0x5f4c3526), C32e(0x719d2fc3),
	C32e(0x3867e1be), C32e(0xfd6aa235), C32e(0x4f0bcc88), C32e(0x4b5c392e),
	C32e(0xf93d5793), C32e(0x0daaf255), C32e(0x9de382fc), C32e(0xc9f4477a),
	C32e(0xef8bacc8), C32e(0x326fe7ba), C32e(0x7d642b32), C32e(0xa4d795e6),
	C32e(0xfb9ba0c0), C32e(0xb3329819), C32e(0x6827d19e), C32e(0x815d7fa3),
	C32e(0xaa886644), C32e(0x82a87e54), C32e(0xe676ab3b), C32e(0x9e16830b),
	C32e(0x4503ca8c), C32e(0x7b9529c7), C32e(0x6ed6d36b), C32e(0x44503c28),
	C32e(0x8b5579a7), C32e(0x3d63e2bc), C32e(0x272c1d16), C32e(0x9a4176ad),
	C32e(0x4dad3bdb), C32e(0xfac85664), C32e(0xd2e84e74), C32e(0x22281e14),
	C32e(0x763fdb92), C32e(0x1e180a0c), C32e(0xb4906c48), C32e(0x376be4b8),
	C32e(0xe7255d9f), C32e(0xb2616ebd), C32e(0x2a86ef43), C32e(0xf193a6c4),
	C32e(0xe372a839), C32e(0xf762a431), C32e(0x59bd37d3), C32e(0x86ff8bf2),
	C32e(0x56b132d5), C32e(0xc50d438b), C32e(0xebdc596e), C32e(0xc2afb7da),
	C32e(0x8f028c01), C32e(0xac7964b1), C32e(0x6d23d29c), C32e(0x3b92e049),
	C32e(0xc7abb4d8), C32e(0x1543faac), C32e(0x09fd07f3), C32e(0x6f8525cf),
	C32e(0xea8fafca), C32e(0x89f38ef4), C32e(0x208ee947), C32e(0x28201810),
	C32e(0x64ded56f), C32e(0x83fb88f0), C32e(0xb1946f4a), C32e(0x96b8725c),
	C32e(0x6c702438), C32e(0x08aef157), C32e(0x52e6c773), C32e(0xf3355197),
	C32e(0x658d23cb), C32e(0x84597ca1), C32e(0xbfcb9ce8), C32e(0x637c213e),
	C32e(0x7c37dd96), C32e(0x7fc2dc61), C32e(0x911a860d), C32e(0x941e850f),
	C32e(0xabdb90e0), C32e(0xc6f8427c), C32e(0x57e2c471), C32e(0xe583aacc),
	C32e(0x733bd890), C32e(0x0f0c0506), C32e(0x03f501f7), C32e(0x3638121c),
	C32e(0xfe9fa3c2), C32e(0xe1d45f6a), C32e(0x1047f9ae), C32e(0x6bd2d069),
	C32e(0xa82e9117), C32e(0xe8295899), C32e(0x6974273a), C32e(0xd04eb927),
	C32e(0x48a938d9), C32e(0x35cd13eb), C32e(0xce56b32b), C32e(0x55443322),
	C32e(0xd6bfbbd2), C32e(0x904970a9), C32e(0x800e8907), C32e(0xf266a733),
	C32e(0xc15ab62d), C32e(0x6678223c), C32e(0xad2a9215), C32e(0x608920c9),
	C32e(0xdb154987), C32e(0x1a4fffaa), C32e(0x88a07850), C32e(0x8e517aa5),
	C32e(0x8a068f03), C32e(0x13b2f859), C32e(0x9b128009), C32e(0x3934171a),
	C32e(0x75cada65), C32e(0x53b531d7), C32e(0x5113c684), C32e(0xd3bbb8d0),
	C32e(0x5e1fc382), C32e(0xcb52b029), C32e(0x99b4775a), C32e(0x333c111e),
	C32e(0x46f6cb7b), C32e(0x1f4bfca8), C32e(0x61dad66d), C32e(0x4e583a2c)
};

static const sph_u32 T1up[] = {
	C32e(0xc6c632f4), C32e(0xf8f86f97), C32e(0xeeee5eb0), C32e(0xf6f67a8c),
	C32e(0xffffe817), C32e(0xd6d60adc), C32e(0xdede16c8), C32e(0x91916dfc),
	C32e(0x606090f0), C32e(0x02020705), C32e(0xcece2ee0), C32e(0x5656d187),
	C32e(0xe7e7cc2b), C32e(0xb5b513a6), C32e(0x4d4d7c31), C32e(0xecec59b5),
	C32e(0x8f8f40cf), C32e(0x1f1fa3bc), C32e(0x898949c0), C32e(0xfafa6892),
	C32e(0xefefd03f), C32e(0xb2b29426), C32e(0x8e8ece40), C32e(0xfbfbe61d),
	C32e(0x41416e2f), C32e(0xb3b31aa9), C32e(0x5f5f431c), C32e(0x45456025),
	C32e(0x2323f9da), C32e(0x53535102), C32e(0xe4e445a1), C32e(0x9b9b76ed),
	C32e(0x7575285d), C32e(0xe1e1c524), C32e(0x3d3dd4e9), C32e(0x4c4cf2be),
	C32e(0x6c6c82ee), C32e(0x7e7ebdc3), C32e(0xf5f5f306), C32e(0x838352d1),
	C32e(0x68688ce4), C32e(0x51515607), C32e(0xd1d18d5c), C32e(0xf9f9e118),
	C32e(0xe2e24cae), C32e(0xabab3e95), C32e(0x626297f5), C32e(0x2a2a6b41),
	C32e(0x08081c14), C32e(0x959563f6), C32e(0x4646e9af), C32e(0x9d9d7fe2),
	C32e(0x30304878), C32e(0x3737cff8), C32e(0x0a0a1b11), C32e(0x2f2febc4),
	C32e(0x0e0e151b), C32e(0x24247e5a), C32e(0x1b1badb6), C32e(0xdfdf9847),
	C32e(0xcdcda76a), C32e(0x4e4ef5bb), C32e(0x7f7f334c), C32e(0xeaea50ba),
	C32e(0x12123f2d), C32e(0x1d1da4b9), C32e(0x5858c49c), C32e(0x34344672),
	C32e(0x36364177), C32e(0xdcdc11cd), C32e(0xb4b49d29), C32e(0x5b5b4d16),
	C32e(0xa4a4a501), C32e(0x7676a1d7), C32e(0xb7b714a3), C32e(0x7d7d3449),
	C32e(0x5252df8d), C32e(0xdddd9f42), C32e(0x5e5ecd93), C32e(0x1313b1a2),
	C32e(0xa6a6a204), C32e(0xb9b901b8), C32e(0x00000000), C32e(0xc1c1b574),
	C32e(0x4040e0a0), C32e(0xe3e3c221), C32e(0x79793a43), C32e(0xb6b69a2c),
	C32e(0xd4d40dd9), C32e(0x8d8d47ca), C32e(0x67671770), C32e(0x7272afdd),
	C32e(0x9494ed79), C32e(0x9898ff67), C32e(0xb0b09323), C32e(0x85855bde),
	C32e(0xbbbb06bd), C32e(0xc5c5bb7e), C32e(0x4f4f7b34), C32e(0xededd73a),
	C32e(0x8686d254), C32e(0x9a9af862), C32e(0x666699ff), C32e(0x1111b6a7),
	C32e(0x8a8ac04a), C32e(0xe9e9d930), C32e(0x04040e0a), C32e(0xfefe6698),
	C32e(0xa0a0ab0b), C32e(0x7878b4cc), C32e(0x2525f0d5), C32e(0x4b4b753e),
	C32e(0xa2a2ac0e), C32e(0x5d5d4419), C32e(0x8080db5b), C32e(0x05058085),
	C32e(0x3f3fd3ec), C32e(0x2121fedf), C32e(0x7070a8d8), C32e(0xf1f1fd0c),
	C32e(0x6363197a), C32e(0x77772f58), C32e(0xafaf309f), C32e(0x4242e7a5),
	C32e(0x20207050), C32e(0xe5e5cb2e), C32e(0xfdfdef12), C32e(0xbfbf08b7),
	C32e(0x818155d4), C32e(0x1818243c), C32e(0x2626795f), C32e(0xc3c3b271),
	C32e(0xbebe8638), C32e(0x3535c8fd), C32e(0x8888c74f), C32e(0x2e2e654b),
	C32e(0x93936af9), C32e(0x5555580d), C32e(0xfcfc619d), C32e(0x7a7ab3c9),
	C32e(0xc8c827ef), C32e(0xbaba8832), C32e(0x32324f7d), C32e(0xe6e642a4),
	C32e(0xc0c03bfb), C32e(0x1919aab3), C32e(0x9e9ef668), C32e(0xa3a32281),
	C32e(0x4444eeaa), C32e(0x5454d682), C32e(0x3b3bdde6), C32e(0x0b0b959e),
	C32e(0x8c8cc945), C32e(0xc7c7bc7b), C32e(0x6b6b056e), C32e(0x28286c44),
	C32e(0xa7a72c8b), C32e(0xbcbc813d), C32e(0x16163127), C32e(0xadad379a),
	C32e(0xdbdb964d), C32e(0x64649efa), C32e(0x7474a6d2), C32e(0x14143622),
	C32e(0x9292e476), C32e(0x0c0c121e), C32e(0x4848fcb4), C32e(0xb8b88f37),
	C32e(0x9f9f78e7), C32e(0xbdbd0fb2), C32e(0x4343692a), C32e(0xc4c435f1),
	C32e(0x3939dae3), C32e(0x3131c6f7), C32e(0xd3d38a59), C32e(0xf2f27486),
	C32e(0xd5d58356), C32e(0x8b8b4ec5), C32e(0x6e6e85eb), C32e(0xdada18c2),
	C32e(0x01018e8f), C32e(0xb1b11dac), C32e(0x9c9cf16d), C32e(0x4949723b),
	C32e(0xd8d81fc7), C32e(0xacacb915), C32e(0xf3f3fa09), C32e(0xcfcfa06f),
	C32e(0xcaca20ea), C32e(0xf4f47d89), C32e(0x47476720), C32e(0x10103828),
	C32e(0x6f6f0b64), C32e(0xf0f07383), C32e(0x4a4afbb1), C32e(0x5c5cca96),
	C32e(0x3838546c), C32e(0x57575f08), C32e(0x73732152), C32e(0x979764f3),
	C32e(0xcbcbae65), C32e(0xa1a12584), C32e(0xe8e857bf), C32e(0x3e3e5d63),
	C32e(0x9696ea7c), C32e(0x61611e7f), C32e(0x0d0d9c91), C32e(0x0f0f9b94),
	C32e(0xe0e04bab), C32e(0x7c7cbac6), C32e(0x71712657), C32e(0xcccc29e5),
	C32e(0x9090e373), C32e(0x0606090f), C32e(0xf7f7f403), C32e(0x1c1c2a36),
	C32e(0xc2c23cfe), C32e(0x6a6a8be1), C32e(0xaeaebe10), C32e(0x6969026b),
	C32e(0x1717bfa8), C32e(0x999971e8), C32e(0x3a3a5369), C32e(0x2727f7d0),
	C32e(0xd9d99148), C32e(0xebebde35), C32e(0x2b2be5ce), C32e(0x22227755),
	C32e(0xd2d204d6), C32e(0xa9a93990), C32e(0x07078780), C32e(0x3333c1f2),
	C32e(0x2d2decc1), C32e(0x3c3c5a66), C32e(0x1515b8ad), C32e(0xc9c9a960),
	C32e(0x87875cdb), C32e(0xaaaab01a), C32e(0x5050d888), C32e(0xa5a52b8e),
	C32e(0x0303898a), C32e(0x59594a13), C32e(0x0909929b), C32e(0x1a1a2339),
	C32e(0x65651075), C32e(0xd7d78453), C32e(0x8484d551), C32e(0xd0d003d3),
	C32e(0x8282dc5e), C32e(0x2929e2cb), C32e(0x5a5ac399), C32e(0x1e1e2d33),
	C32e(0x7b7b3d46), C32e(0xa8a8b71f), C32e(0x6d6d0c61), C32e(0x2c2c624e)
};

static const sph_u32 T1dn[] = {
	C32e(0xa5f497a5), C32e(0x8497eb84), C32e(0x99b0c799), C32e(0x8d8cf78d),
	C32e(0x0d17e50d), C32e(0xbddcb7bd), C32e(0xb1c8a7b1), C32e(0x54fc3954),
	C32e(0x50f0c050), C32e(0x03050403), C32e(0xa9e087a9), C32e(0x7d87ac7d),
	C32e(0x192bd519), C32e(0x62a67162), C32e(0xe6319ae6), C32e(0x9ab5c39a),
	C32e(0x45cf0545), C32e(0x9dbc3e9d), C32e(0x40c00940), C32e(0x8792ef87),
	C32e(0x153fc515), C32e(0xeb267feb), C32e(0xc94007c9), C32e(0x0b1ded0b),
	C32e(0xec2f82ec), C32e(0x67a97d67), C32e(0xfd1cbefd), C32e(0xea258aea),
	C32e(0xbfda46bf), C32e(0xf702a6f7), C32e(0x96a1d396), C32e(0x5bed2d5b),
	C32e(0xc25deac2), C32e(0x1c24d91c), C32e(0xaee97aae), C32e(0x6abe986a),
	C32e(0x5aeed85a), C32e(0x41c3fc41), C32e(0x0206f102), C32e(0x4fd11d4f),
	C32e(0x5ce4d05c), C32e(0xf407a2f4), C32e(0x345cb934), C32e(0x0818e908),
	C32e(0x93aedf93), C32e(0x73954d73), C32e(0x53f5c453), C32e(0x3f41543f),
	C32e(0x0c14100c), C32e(0x52f63152), C32e(0x65af8c65), C32e(0x5ee2215e),
	C32e(0x28786028), C32e(0xa1f86ea1), C32e(0x0f11140f), C32e(0xb5c45eb5),
	C32e(0x091b1c09), C32e(0x365a4836), C32e(0x9bb6369b), C32e(0x3d47a53d),
	C32e(0x266a8126), C32e(0x69bb9c69), C32e(0xcd4cfecd), C32e(0x9fbacf9f),
	C32e(0x1b2d241b), C32e(0x9eb93a9e), C32e(0x749cb074), C32e(0x2e72682e),
	C32e(0x2d776c2d), C32e(0xb2cda3b2), C32e(0xee2973ee), C32e(0xfb16b6fb),
	C32e(0xf60153f6), C32e(0x4dd7ec4d), C32e(0x61a37561), C32e(0xce49face),
	C32e(0x7b8da47b), C32e(0x3e42a13e), C32e(0x7193bc71), C32e(0x97a22697),
	C32e(0xf50457f5), C32e(0x68b86968), C32e(0x00000000), C32e(0x2c74992c),
	C32e(0x60a08060), C32e(0x1f21dd1f), C32e(0xc843f2c8), C32e(0xed2c77ed),
	C32e(0xbed9b3be), C32e(0x46ca0146), C32e(0xd970ced9), C32e(0x4bdde44b),
	C32e(0xde7933de), C32e(0xd4672bd4), C32e(0xe8237be8), C32e(0x4ade114a),
	C32e(0x6bbd6d6b), C32e(0x2a7e912a), C32e(0xe5349ee5), C32e(0x163ac116),
	C32e(0xc55417c5), C32e(0xd7622fd7), C32e(0x55ffcc55), C32e(0x94a72294),
	C32e(0xcf4a0fcf), C32e(0x1030c910), C32e(0x060a0806), C32e(0x8198e781),
	C32e(0xf00b5bf0), C32e(0x44ccf044), C32e(0xbad54aba), C32e(0xe33e96e3),
	C32e(0xf30e5ff3), C32e(0xfe19bafe), C32e(0xc05b1bc0), C32e(0x8a850a8a),
	C32e(0xadec7ead), C32e(0xbcdf42bc), C32e(0x48d8e048), C32e(0x040cf904),
	C32e(0xdf7ac6df), C32e(0xc158eec1), C32e(0x759f4575), C32e(0x63a58463),
	C32e(0x30504030), C32e(0x1a2ed11a), C32e(0x0e12e10e), C32e(0x6db7656d),
	C32e(0x4cd4194c), C32e(0x143c3014), C32e(0x355f4c35), C32e(0x2f719d2f),
	C32e(0xe13867e1), C32e(0xa2fd6aa2), C32e(0xcc4f0bcc), C32e(0x394b5c39),
	C32e(0x57f93d57), C32e(0xf20daaf2), C32e(0x829de382), C32e(0x47c9f447),
	C32e(0xacef8bac), C32e(0xe7326fe7), C32e(0x2b7d642b), C32e(0x95a4d795),
	C32e(0xa0fb9ba0), C32e(0x98b33298), C32e(0xd16827d1), C32e(0x7f815d7f),
	C32e(0x66aa8866), C32e(0x7e82a87e), C32e(0xabe676ab), C32e(0x839e1683),
	C32e(0xca4503ca), C32e(0x297b9529), C32e(0xd36ed6d3), C32e(0x3c44503c),
	C32e(0x798b5579), C32e(0xe23d63e2), C32e(0x1d272c1d), C32e(0x769a4176),
	C32e(0x3b4dad3b), C32e(0x56fac856), C32e(0x4ed2e84e), C32e(0x1e22281e),
	C32e(0xdb763fdb), C32e(0x0a1e180a), C32e(0x6cb4906c), C32e(0xe4376be4),
	C32e(0x5de7255d), C32e(0x6eb2616e), C32e(0xef2a86ef), C32e(0xa6f193a6),
	C32e(0xa8e372a8), C32e(0xa4f762a4), C32e(0x3759bd37), C32e(0x8b86ff8b),
	C32e(0x3256b132), C32e(0x43c50d43), C32e(0x59ebdc59), C32e(0xb7c2afb7),
	C32e(0x8c8f028c), C32e(0x64ac7964), C32e(0xd26d23d2), C32e(0xe03b92e0),
	C32e(0xb4c7abb4), C32e(0xfa1543fa), C32e(0x0709fd07), C32e(0x256f8525),
	C32e(0xafea8faf), C32e(0x8e89f38e), C32e(0xe9208ee9), C32e(0x18282018),
	C32e(0xd564ded5), C32e(0x8883fb88), C32e(0x6fb1946f), C32e(0x7296b872),
	C32e(0x246c7024), C32e(0xf108aef1), C32e(0xc752e6c7), C32e(0x51f33551),
	C32e(0x23658d23), C32e(0x7c84597c), C32e(0x9cbfcb9c), C32e(0x21637c21),
	C32e(0xdd7c37dd), C32e(0xdc7fc2dc), C32e(0x86911a86), C32e(0x85941e85),
	C32e(0x90abdb90), C32e(0x42c6f842), C32e(0xc457e2c4), C32e(0xaae583aa),
	C32e(0xd8733bd8), C32e(0x050f0c05), C32e(0x0103f501), C32e(0x12363812),
	C32e(0xa3fe9fa3), C32e(0x5fe1d45f), C32e(0xf91047f9), C32e(0xd06bd2d0),
	C32e(0x91a82e91), C32e(0x58e82958), C32e(0x27697427), C32e(0xb9d04eb9),
	C32e(0x3848a938), C32e(0x1335cd13), C32e(0xb3ce56b3), C32e(0x33554433),
	C32e(0xbbd6bfbb), C32e(0x70904970), C32e(0x89800e89), C32e(0xa7f266a7),
	C32e(0xb6c15ab6), C32e(0x22667822), C32e(0x92ad2a92), C32e(0x20608920),
	C32e(0x49db1549), C32e(0xff1a4fff), C32e(0x7888a078), C32e(0x7a8e517a),
	C32e(0x8f8a068f), C32e(0xf813b2f8), C32e(0x809b1280), C32e(0x17393417),
	C32e(0xda75cada), C32e(0x3153b531), C32e(0xc65113c6), C32e(0xb8d3bbb8),
	C32e(0xc35e1fc3), C32e(0xb0cb52b0), C32e(0x7799b477), C32e(0x11333c11),
	C32e(0xcb46f6cb), C32e(0xfc1f4bfc), C32e(0xd661dad6), C32e(0x3a4e583a)
};

#define DECL_STATE_SMALL \
	sph_u32 H[16] = {0};

#define READ_STATE_SMALL(sc)   do { \
		memcpy(H, (sc)->state.narrow, sizeof H); \
	} while (0)

#define WRITE_STATE_SMALL(sc)   do { \
		memcpy((sc)->state.narrow, H, sizeof H); \
	} while (0)

#define XCAT(x, y)    XCAT_(x, y)
#define XCAT_(x, y)   x ## y

#define RSTT(d0, d1, a, b0, b1, b2, b3, b4, b5, b6, b7)   do { \
		t[d0] = T0up[B32_0(a[b0])] \
			^ T1up[B32_1(a[b1])] \
			^ T2up[B32_2(a[b2])] \
			^ T3up[B32_3(a[b3])] \
			^ T0dn[B32_0(a[b4])] \
			^ T1dn[B32_1(a[b5])] \
			^ T2dn[B32_2(a[b6])] \
			^ T3dn[B32_3(a[b7])]; \
		t[d1] = T0dn[B32_0(a[b0])] \
			^ T1dn[B32_1(a[b1])] \
			^ T2dn[B32_2(a[b2])] \
			^ T3dn[B32_3(a[b3])] \
			^ T0up[B32_0(a[b4])] \
			^ T1up[B32_1(a[b5])] \
			^ T2up[B32_2(a[b6])] \
			^ T3up[B32_3(a[b7])]; \
	} while (0)

#define ROUND_SMALL_P(a, r)   do { \
		sph_u32 t[16]; \
		a[0x0] ^= PC32up(0x00, r); \
		a[0x1] ^= PC32dn(0x00, r); \
		a[0x2] ^= PC32up(0x10, r); \
		a[0x3] ^= PC32dn(0x10, r); \
		a[0x4] ^= PC32up(0x20, r); \
		a[0x5] ^= PC32dn(0x20, r); \
		a[0x6] ^= PC32up(0x30, r); \
		a[0x7] ^= PC32dn(0x30, r); \
		a[0x8] ^= PC32up(0x40, r); \
		a[0x9] ^= PC32dn(0x40, r); \
		a[0xA] ^= PC32up(0x50, r); \
		a[0xB] ^= PC32dn(0x50, r); \
		a[0xC] ^= PC32up(0x60, r); \
		a[0xD] ^= PC32dn(0x60, r); \
		a[0xE] ^= PC32up(0x70, r); \
		a[0xF] ^= PC32dn(0x70, r); \
		RSTT(0x0, 0x1, a, 0x0, 0x2, 0x4, 0x6, 0x9, 0xB, 0xD, 0xF); \
		RSTT(0x2, 0x3, a, 0x2, 0x4, 0x6, 0x8, 0xB, 0xD, 0xF, 0x1); \
		RSTT(0x4, 0x5, a, 0x4, 0x6, 0x8, 0xA, 0xD, 0xF, 0x1, 0x3); \
		RSTT(0x6, 0x7, a, 0x6, 0x8, 0xA, 0xC, 0xF, 0x1, 0x3, 0x5); \
		RSTT(0x8, 0x9, a, 0x8, 0xA, 0xC, 0xE, 0x1, 0x3, 0x5, 0x7); \
		RSTT(0xA, 0xB, a, 0xA, 0xC, 0xE, 0x0, 0x3, 0x5, 0x7, 0x9); \
		RSTT(0xC, 0xD, a, 0xC, 0xE, 0x0, 0x2, 0x5, 0x7, 0x9, 0xB); \
		RSTT(0xE, 0xF, a, 0xE, 0x0, 0x2, 0x4, 0x7, 0x9, 0xB, 0xD); \
		memcpy(a, t, sizeof t); \
	} while (0)

#define ROUND_SMALL_Q(a, r)   do { \
		sph_u32 t[16]; \
		a[0x0] ^= QC32up(0x00, r); \
		a[0x1] ^= QC32dn(0x00, r); \
		a[0x2] ^= QC32up(0x10, r); \
		a[0x3] ^= QC32dn(0x10, r); \
		a[0x4] ^= QC32up(0x20, r); \
		a[0x5] ^= QC32dn(0x20, r); \
		a[0x6] ^= QC32up(0x30, r); \
		a[0x7] ^= QC32dn(0x30, r); \
		a[0x8] ^= QC32up(0x40, r); \
		a[0x9] ^= QC32dn(0x40, r); \
		a[0xA] ^= QC32up(0x50, r); \
		a[0xB] ^= QC32dn(0x50, r); \
		a[0xC] ^= QC32up(0x60, r); \
		a[0xD] ^= QC32dn(0x60, r); \
		a[0xE] ^= QC32up(0x70, r); \
		a[0xF] ^= QC32dn(0x70, r); \
		RSTT(0x0, 0x1, a, 0x2, 0x6, 0xA, 0xE, 0x1, 0x5, 0x9, 0xD); \
		RSTT(0x2, 0x3, a, 0x4, 0x8, 0xC, 0x0, 0x3, 0x7, 0xB, 0xF); \
		RSTT(0x4, 0x5, a, 0x6, 0xA, 0xE, 0x2, 0x5, 0x9, 0xD, 0x1); \
		RSTT(0x6, 0x7, a, 0x8, 0xC, 0x0, 0x4, 0x7, 0xB, 0xF, 0x3); \
		RSTT(0x8, 0x9, a, 0xA, 0xE, 0x2, 0x6, 0x9, 0xD, 0x1, 0x5); \
		RSTT(0xA, 0xB, a, 0xC, 0x0, 0x4, 0x8, 0xB, 0xF, 0x3, 0x7); \
		RSTT(0xC, 0xD, a, 0xE, 0x2, 0x6, 0xA, 0xD, 0x1, 0x5, 0x9); \
		RSTT(0xE, 0xF, a, 0x0, 0x4, 0x8, 0xC, 0xF, 0x3, 0x7, 0xB); \
		memcpy(a, t, sizeof t); \
	} while (0)

#define PERM_SMALL_P(a)   do { \
		int r; \
		for (r = 0; r < 10; r ++) \
			ROUND_SMALL_P(a, r); \
	} while (0)

#define PERM_SMALL_Q(a)   do { \
		int r; \
		for (r = 0; r < 10; r ++) \
			ROUND_SMALL_Q(a, r); \
	} while (0)


#define COMPRESS_SMALL   do { \
		sph_u32 g[16], m[16]; \
		size_t u; \
		for (u = 0; u < 16; u ++) { \
			m[u] = dec32e_aligned(buf + (u << 2)); \
			g[u] = m[u] ^ H[u]; \
		} \
		PERM_SMALL_P(g); \
		PERM_SMALL_Q(m); \
		for (u = 0; u < 16; u ++) \
			H[u] ^= g[u] ^ m[u]; \
	} while (0)

#define FINAL_SMALL   do { \
		sph_u32 x[16]; \
		size_t u; \
		memcpy(x, H, sizeof x); \
		PERM_SMALL_P(x); \
		for (u = 0; u < 16; u ++) \
			H[u] ^= x[u]; \
	} while (0)

#define DECL_STATE_BIG \
	sph_u32 H[32] = {0};

#define READ_STATE_BIG(sc)   do { \
		memcpy(H, (sc)->state.narrow, sizeof H); \
	} while (0)

#define WRITE_STATE_BIG(sc)   do { \
		memcpy((sc)->state.narrow, H, sizeof H); \
	} while (0)


#define RBTT(d0, d1, a, b0, b1, b2, b3, b4, b5, b6, b7)   do { \
		sph_u32 fu2 = T0up[B32_2(a[b2])]; \
		sph_u32 fd2 = T0dn[B32_2(a[b2])]; \
		sph_u32 fu3 = T1up[B32_3(a[b3])]; \
		sph_u32 fd3 = T1dn[B32_3(a[b3])]; \
		sph_u32 fu6 = T0up[B32_2(a[b6])]; \
		sph_u32 fd6 = T0dn[B32_2(a[b6])]; \
		sph_u32 fu7 = T1up[B32_3(a[b7])]; \
		sph_u32 fd7 = T1dn[B32_3(a[b7])]; \
		t[d0] = T0up[B32_0(a[b0])] \
			^ T1up[B32_1(a[b1])] \
			^ R32u(fu2, fd2) \
			^ R32u(fu3, fd3) \
			^ T0dn[B32_0(a[b4])] \
			^ T1dn[B32_1(a[b5])] \
			^ R32d(fu6, fd6) \
			^ R32d(fu7, fd7); \
		t[d1] = T0dn[B32_0(a[b0])] \
			^ T1dn[B32_1(a[b1])] \
			^ R32d(fu2, fd2) \
			^ R32d(fu3, fd3) \
			^ T0up[B32_0(a[b4])] \
			^ T1up[B32_1(a[b5])] \
			^ R32u(fu6, fd6) \
			^ R32u(fu7, fd7); \
	} while (0)


#define ROUND_BIG_P(a, r)   do { \
		sph_u32 t[32]; \
		size_t u; \
		a[0x00] ^= PC32up(0x00, r); \
		a[0x01] ^= PC32dn(0x00, r); \
		a[0x02] ^= PC32up(0x10, r); \
		a[0x03] ^= PC32dn(0x10, r); \
		a[0x04] ^= PC32up(0x20, r); \
		a[0x05] ^= PC32dn(0x20, r); \
		a[0x06] ^= PC32up(0x30, r); \
		a[0x07] ^= PC32dn(0x30, r); \
		a[0x08] ^= PC32up(0x40, r); \
		a[0x09] ^= PC32dn(0x40, r); \
		a[0x0A] ^= PC32up(0x50, r); \
		a[0x0B] ^= PC32dn(0x50, r); \
		a[0x0C] ^= PC32up(0x60, r); \
		a[0x0D] ^= PC32dn(0x60, r); \
		a[0x0E] ^= PC32up(0x70, r); \
		a[0x0F] ^= PC32dn(0x70, r); \
		a[0x10] ^= PC32up(0x80, r); \
		a[0x11] ^= PC32dn(0x80, r); \
		a[0x12] ^= PC32up(0x90, r); \
		a[0x13] ^= PC32dn(0x90, r); \
		a[0x14] ^= PC32up(0xA0, r); \
		a[0x15] ^= PC32dn(0xA0, r); \
		a[0x16] ^= PC32up(0xB0, r); \
		a[0x17] ^= PC32dn(0xB0, r); \
		a[0x18] ^= PC32up(0xC0, r); \
		a[0x19] ^= PC32dn(0xC0, r); \
		a[0x1A] ^= PC32up(0xD0, r); \
		a[0x1B] ^= PC32dn(0xD0, r); \
		a[0x1C] ^= PC32up(0xE0, r); \
		a[0x1D] ^= PC32dn(0xE0, r); \
		a[0x1E] ^= PC32up(0xF0, r); \
		a[0x1F] ^= PC32dn(0xF0, r); \
		for (u = 0; u < 32; u += 8) { \
			RBTT(u + 0x00, (u + 0x01) & 0x1F, a, \
				u + 0x00, (u + 0x02) & 0x1F, \
				(u + 0x04) & 0x1F, (u + 0x06) & 0x1F, \
				(u + 0x09) & 0x1F, (u + 0x0B) & 0x1F, \
				(u + 0x0D) & 0x1F, (u + 0x17) & 0x1F); \
			RBTT(u + 0x02, (u + 0x03) & 0x1F, a, \
				u + 0x02, (u + 0x04) & 0x1F, \
				(u + 0x06) & 0x1F, (u + 0x08) & 0x1F, \
				(u + 0x0B) & 0x1F, (u + 0x0D) & 0x1F, \
				(u + 0x0F) & 0x1F, (u + 0x19) & 0x1F); \
			RBTT(u + 0x04, (u + 0x05) & 0x1F, a, \
				u + 0x04, (u + 0x06) & 0x1F, \
				(u + 0x08) & 0x1F, (u + 0x0A) & 0x1F, \
				(u + 0x0D) & 0x1F, (u + 0x0F) & 0x1F, \
				(u + 0x11) & 0x1F, (u + 0x1B) & 0x1F); \
			RBTT(u + 0x06, (u + 0x07) & 0x1F, a, \
				u + 0x06, (u + 0x08) & 0x1F, \
				(u + 0x0A) & 0x1F, (u + 0x0C) & 0x1F, \
				(u + 0x0F) & 0x1F, (u + 0x11) & 0x1F, \
				(u + 0x13) & 0x1F, (u + 0x1D) & 0x1F); \
		} \
		memcpy(a, t, sizeof t); \
	} while (0)

#define ROUND_BIG_Q(a, r)   do { \
		sph_u32 t[32]; \
		size_t u; \
		a[0x00] ^= QC32up(0x00, r); \
		a[0x01] ^= QC32dn(0x00, r); \
		a[0x02] ^= QC32up(0x10, r); \
		a[0x03] ^= QC32dn(0x10, r); \
		a[0x04] ^= QC32up(0x20, r); \
		a[0x05] ^= QC32dn(0x20, r); \
		a[0x06] ^= QC32up(0x30, r); \
		a[0x07] ^= QC32dn(0x30, r); \
		a[0x08] ^= QC32up(0x40, r); \
		a[0x09] ^= QC32dn(0x40, r); \
		a[0x0A] ^= QC32up(0x50, r); \
		a[0x0B] ^= QC32dn(0x50, r); \
		a[0x0C] ^= QC32up(0x60, r); \
		a[0x0D] ^= QC32dn(0x60, r); \
		a[0x0E] ^= QC32up(0x70, r); \
		a[0x0F] ^= QC32dn(0x70, r); \
		a[0x10] ^= QC32up(0x80, r); \
		a[0x11] ^= QC32dn(0x80, r); \
		a[0x12] ^= QC32up(0x90, r); \
		a[0x13] ^= QC32dn(0x90, r); \
		a[0x14] ^= QC32up(0xA0, r); \
		a[0x15] ^= QC32dn(0xA0, r); \
		a[0x16] ^= QC32up(0xB0, r); \
		a[0x17] ^= QC32dn(0xB0, r); \
		a[0x18] ^= QC32up(0xC0, r); \
		a[0x19] ^= QC32dn(0xC0, r); \
		a[0x1A] ^= QC32up(0xD0, r); \
		a[0x1B] ^= QC32dn(0xD0, r); \
		a[0x1C] ^= QC32up(0xE0, r); \
		a[0x1D] ^= QC32dn(0xE0, r); \
		a[0x1E] ^= QC32up(0xF0, r); \
		a[0x1F] ^= QC32dn(0xF0, r); \
		for (u = 0; u < 32; u += 8) { \
			RBTT(u + 0x00, (u + 0x01) & 0x1F, a, \
				(u + 0x02) & 0x1F, (u + 0x06) & 0x1F, \
				(u + 0x0A) & 0x1F, (u + 0x16) & 0x1F, \
				(u + 0x01) & 0x1F, (u + 0x05) & 0x1F, \
				(u + 0x09) & 0x1F, (u + 0x0D) & 0x1F); \
			RBTT(u + 0x02, (u + 0x03) & 0x1F, a, \
				(u + 0x04) & 0x1F, (u + 0x08) & 0x1F, \
				(u + 0x0C) & 0x1F, (u + 0x18) & 0x1F, \
				(u + 0x03) & 0x1F, (u + 0x07) & 0x1F, \
				(u + 0x0B) & 0x1F, (u + 0x0F) & 0x1F); \
			RBTT(u + 0x04, (u + 0x05) & 0x1F, a, \
				(u + 0x06) & 0x1F, (u + 0x0A) & 0x1F, \
				(u + 0x0E) & 0x1F, (u + 0x1A) & 0x1F, \
				(u + 0x05) & 0x1F, (u + 0x09) & 0x1F, \
				(u + 0x0D) & 0x1F, (u + 0x11) & 0x1F); \
			RBTT(u + 0x06, (u + 0x07) & 0x1F, a, \
				(u + 0x08) & 0x1F, (u + 0x0C) & 0x1F, \
				(u + 0x10) & 0x1F, (u + 0x1C) & 0x1F, \
				(u + 0x07) & 0x1F, (u + 0x0B) & 0x1F, \
				(u + 0x0F) & 0x1F, (u + 0x13) & 0x1F); \
		} \
		memcpy(a, t, sizeof t); \
	} while (0)


#define PERM_BIG_P(a)   do { \
		int r; \
		for (r = 0; r < 14; r ++) \
			ROUND_BIG_P(a, r); \
	} while (0)

#define PERM_BIG_Q(a)   do { \
		int r; \
		for (r = 0; r < 14; r ++) \
			ROUND_BIG_Q(a, r); \
	} while (0)


#define COMPRESS_BIG   do { \
		sph_u32 g[32], m[32]; \
		size_t uu; \
		for (uu = 0; uu < 32; uu ++) { \
			m[uu] = dec32e_aligned(buf + (uu << 2)); \
			g[uu] = m[uu] ^ H[uu]; \
		} \
		PERM_BIG_P(g); \
		PERM_BIG_Q(m); \
		for (uu = 0; uu < 32; uu ++) \
			H[uu] ^= g[uu] ^ m[uu]; \
	} while (0)

#define FINAL_BIG   do { \
		sph_u32 x[32]; \
		size_t uu; \
		memcpy(x, H, sizeof x); \
		PERM_BIG_P(x); \
		for (uu = 0; uu < 32; uu ++) \
			H[uu] ^= x[uu]; \
	} while (0)


static void
groestl_big_init(sph_groestl_big_context *sc, unsigned out_size)
{
	size_t u = 0;

	sc->ptr = 0;
	for (u = 0; u < 31; u ++)
		sc->state.narrow[u] = 0;
	sc->state.narrow[31] = ((sph_u32)(out_size & 0xFF) << 24)
		| ((sph_u32)(out_size & 0xFF00) << 8);
	sc->count = 0;
}

static void
groestl_big_core(sph_groestl_big_context *sc, const void *data, size_t len)
{
  if (len == 0) {
    return;
  }

	unsigned char *buf = NULL;
	size_t ptr = 0;
	DECL_STATE_BIG

	buf = sc->buf;
	ptr = sc->ptr;
	if (len < (sizeof sc->buf) - ptr) {
		memcpy(buf + ptr, data, len);
		ptr += len;
		sc->ptr = ptr;
		return;
	}

	READ_STATE_BIG(sc);
	while (len > 0) {
		size_t clen = 0;

		clen = (sizeof sc->buf) - ptr;
		if (clen > len)
			clen = len;
		memcpy(buf + ptr, data, clen);
		ptr += clen;
		data = (const unsigned char *)data + clen;
		len -= clen;
		if (ptr == sizeof sc->buf) {
			COMPRESS_BIG;
			sc->count ++;
			ptr = 0;
		}
	}
	WRITE_STATE_BIG(sc);
	sc->ptr = ptr;
}

static void
groestl_big_close(sph_groestl_big_context *sc,
	unsigned ub, unsigned n, void *dst, size_t out_len)
{
	unsigned char pad[136] = {0};
	size_t ptr = 0, pad_len = 0, u2 = 0;
	sph_u64 count = 0;
	unsigned z = 0;
	DECL_STATE_BIG

	ptr = sc->ptr;
	z = 0x80 >> n;
	pad[0] = ((ub & -z) | z) & 0xFF;
	if (ptr < 120) {
		pad_len = 128 - ptr;
		count = SPH_T64(sc->count + 1);
	} else {
		pad_len = 256 - ptr;
		count = SPH_T64(sc->count + 2);
	}
	memzero(pad + 1, pad_len - 9);
	sph_enc64be(pad + pad_len - 8, count);
	groestl_big_core(sc, pad, pad_len);
	READ_STATE_BIG(sc);
	FINAL_BIG;
	for (u2 = 0; u2 < 16; u2 ++)
		enc32e(pad + (u2 << 2), H[u2 + 16]);
	memcpy(dst, pad + 64 - out_len, out_len);
	groestl_big_init(sc, (unsigned)out_len << 3);
}

void
groestl512_Init(void *cc)
{
	groestl_big_init((sph_groestl_big_context *)cc, 512);
}

void
groestl512_Update(void *cc, const void *data, size_t len)
{
	groestl_big_core((sph_groestl_big_context *)cc, data, len);
}

void
groestl512_Final(void *cc, void *dst)
{
	groestl_big_close((sph_groestl_big_context *)cc, 0, 0, dst, 64);
}

void
groestl512_DoubleTrunc(void *cc, void *dst)
{
	char buf[64] = {0};

	groestl512_Final(cc, buf);
	groestl512_Update(cc, buf, sizeof(buf));
	groestl512_Final(cc, buf);
	memcpy(dst, buf, 32);
}
