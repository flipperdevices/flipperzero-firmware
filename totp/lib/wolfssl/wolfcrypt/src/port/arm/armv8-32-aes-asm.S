/* armv8-32-aes-asm
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

/* Generated using (from wolfssl):
 *   cd ../scripts
 *   ruby ./aes/aes.rb arm32 ../wolfssl/wolfcrypt/src/port/arm/armv8-32-aes-asm.S
 */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif /* HAVE_CONFIG_H */
#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_ARMASM
#ifndef __aarch64__
	.text
	.type	L_AES_ARM32_te, %object
	.size	L_AES_ARM32_te, 1024
	.align	4
L_AES_ARM32_te:
	.word	0xa5c66363
	.word	0x84f87c7c
	.word	0x99ee7777
	.word	0x8df67b7b
	.word	0xdfff2f2
	.word	0xbdd66b6b
	.word	0xb1de6f6f
	.word	0x5491c5c5
	.word	0x50603030
	.word	0x3020101
	.word	0xa9ce6767
	.word	0x7d562b2b
	.word	0x19e7fefe
	.word	0x62b5d7d7
	.word	0xe64dabab
	.word	0x9aec7676
	.word	0x458fcaca
	.word	0x9d1f8282
	.word	0x4089c9c9
	.word	0x87fa7d7d
	.word	0x15effafa
	.word	0xebb25959
	.word	0xc98e4747
	.word	0xbfbf0f0
	.word	0xec41adad
	.word	0x67b3d4d4
	.word	0xfd5fa2a2
	.word	0xea45afaf
	.word	0xbf239c9c
	.word	0xf753a4a4
	.word	0x96e47272
	.word	0x5b9bc0c0
	.word	0xc275b7b7
	.word	0x1ce1fdfd
	.word	0xae3d9393
	.word	0x6a4c2626
	.word	0x5a6c3636
	.word	0x417e3f3f
	.word	0x2f5f7f7
	.word	0x4f83cccc
	.word	0x5c683434
	.word	0xf451a5a5
	.word	0x34d1e5e5
	.word	0x8f9f1f1
	.word	0x93e27171
	.word	0x73abd8d8
	.word	0x53623131
	.word	0x3f2a1515
	.word	0xc080404
	.word	0x5295c7c7
	.word	0x65462323
	.word	0x5e9dc3c3
	.word	0x28301818
	.word	0xa1379696
	.word	0xf0a0505
	.word	0xb52f9a9a
	.word	0x90e0707
	.word	0x36241212
	.word	0x9b1b8080
	.word	0x3ddfe2e2
	.word	0x26cdebeb
	.word	0x694e2727
	.word	0xcd7fb2b2
	.word	0x9fea7575
	.word	0x1b120909
	.word	0x9e1d8383
	.word	0x74582c2c
	.word	0x2e341a1a
	.word	0x2d361b1b
	.word	0xb2dc6e6e
	.word	0xeeb45a5a
	.word	0xfb5ba0a0
	.word	0xf6a45252
	.word	0x4d763b3b
	.word	0x61b7d6d6
	.word	0xce7db3b3
	.word	0x7b522929
	.word	0x3edde3e3
	.word	0x715e2f2f
	.word	0x97138484
	.word	0xf5a65353
	.word	0x68b9d1d1
	.word	0x0
	.word	0x2cc1eded
	.word	0x60402020
	.word	0x1fe3fcfc
	.word	0xc879b1b1
	.word	0xedb65b5b
	.word	0xbed46a6a
	.word	0x468dcbcb
	.word	0xd967bebe
	.word	0x4b723939
	.word	0xde944a4a
	.word	0xd4984c4c
	.word	0xe8b05858
	.word	0x4a85cfcf
	.word	0x6bbbd0d0
	.word	0x2ac5efef
	.word	0xe54faaaa
	.word	0x16edfbfb
	.word	0xc5864343
	.word	0xd79a4d4d
	.word	0x55663333
	.word	0x94118585
	.word	0xcf8a4545
	.word	0x10e9f9f9
	.word	0x6040202
	.word	0x81fe7f7f
	.word	0xf0a05050
	.word	0x44783c3c
	.word	0xba259f9f
	.word	0xe34ba8a8
	.word	0xf3a25151
	.word	0xfe5da3a3
	.word	0xc0804040
	.word	0x8a058f8f
	.word	0xad3f9292
	.word	0xbc219d9d
	.word	0x48703838
	.word	0x4f1f5f5
	.word	0xdf63bcbc
	.word	0xc177b6b6
	.word	0x75afdada
	.word	0x63422121
	.word	0x30201010
	.word	0x1ae5ffff
	.word	0xefdf3f3
	.word	0x6dbfd2d2
	.word	0x4c81cdcd
	.word	0x14180c0c
	.word	0x35261313
	.word	0x2fc3ecec
	.word	0xe1be5f5f
	.word	0xa2359797
	.word	0xcc884444
	.word	0x392e1717
	.word	0x5793c4c4
	.word	0xf255a7a7
	.word	0x82fc7e7e
	.word	0x477a3d3d
	.word	0xacc86464
	.word	0xe7ba5d5d
	.word	0x2b321919
	.word	0x95e67373
	.word	0xa0c06060
	.word	0x98198181
	.word	0xd19e4f4f
	.word	0x7fa3dcdc
	.word	0x66442222
	.word	0x7e542a2a
	.word	0xab3b9090
	.word	0x830b8888
	.word	0xca8c4646
	.word	0x29c7eeee
	.word	0xd36bb8b8
	.word	0x3c281414
	.word	0x79a7dede
	.word	0xe2bc5e5e
	.word	0x1d160b0b
	.word	0x76addbdb
	.word	0x3bdbe0e0
	.word	0x56643232
	.word	0x4e743a3a
	.word	0x1e140a0a
	.word	0xdb924949
	.word	0xa0c0606
	.word	0x6c482424
	.word	0xe4b85c5c
	.word	0x5d9fc2c2
	.word	0x6ebdd3d3
	.word	0xef43acac
	.word	0xa6c46262
	.word	0xa8399191
	.word	0xa4319595
	.word	0x37d3e4e4
	.word	0x8bf27979
	.word	0x32d5e7e7
	.word	0x438bc8c8
	.word	0x596e3737
	.word	0xb7da6d6d
	.word	0x8c018d8d
	.word	0x64b1d5d5
	.word	0xd29c4e4e
	.word	0xe049a9a9
	.word	0xb4d86c6c
	.word	0xfaac5656
	.word	0x7f3f4f4
	.word	0x25cfeaea
	.word	0xafca6565
	.word	0x8ef47a7a
	.word	0xe947aeae
	.word	0x18100808
	.word	0xd56fbaba
	.word	0x88f07878
	.word	0x6f4a2525
	.word	0x725c2e2e
	.word	0x24381c1c
	.word	0xf157a6a6
	.word	0xc773b4b4
	.word	0x5197c6c6
	.word	0x23cbe8e8
	.word	0x7ca1dddd
	.word	0x9ce87474
	.word	0x213e1f1f
	.word	0xdd964b4b
	.word	0xdc61bdbd
	.word	0x860d8b8b
	.word	0x850f8a8a
	.word	0x90e07070
	.word	0x427c3e3e
	.word	0xc471b5b5
	.word	0xaacc6666
	.word	0xd8904848
	.word	0x5060303
	.word	0x1f7f6f6
	.word	0x121c0e0e
	.word	0xa3c26161
	.word	0x5f6a3535
	.word	0xf9ae5757
	.word	0xd069b9b9
	.word	0x91178686
	.word	0x5899c1c1
	.word	0x273a1d1d
	.word	0xb9279e9e
	.word	0x38d9e1e1
	.word	0x13ebf8f8
	.word	0xb32b9898
	.word	0x33221111
	.word	0xbbd26969
	.word	0x70a9d9d9
	.word	0x89078e8e
	.word	0xa7339494
	.word	0xb62d9b9b
	.word	0x223c1e1e
	.word	0x92158787
	.word	0x20c9e9e9
	.word	0x4987cece
	.word	0xffaa5555
	.word	0x78502828
	.word	0x7aa5dfdf
	.word	0x8f038c8c
	.word	0xf859a1a1
	.word	0x80098989
	.word	0x171a0d0d
	.word	0xda65bfbf
	.word	0x31d7e6e6
	.word	0xc6844242
	.word	0xb8d06868
	.word	0xc3824141
	.word	0xb0299999
	.word	0x775a2d2d
	.word	0x111e0f0f
	.word	0xcb7bb0b0
	.word	0xfca85454
	.word	0xd66dbbbb
	.word	0x3a2c1616
	.text
	.type	L_AES_ARM32_td, %object
	.size	L_AES_ARM32_td, 1024
	.align	4
L_AES_ARM32_td:
	.word	0x5051f4a7
	.word	0x537e4165
	.word	0xc31a17a4
	.word	0x963a275e
	.word	0xcb3bab6b
	.word	0xf11f9d45
	.word	0xabacfa58
	.word	0x934be303
	.word	0x552030fa
	.word	0xf6ad766d
	.word	0x9188cc76
	.word	0x25f5024c
	.word	0xfc4fe5d7
	.word	0xd7c52acb
	.word	0x80263544
	.word	0x8fb562a3
	.word	0x49deb15a
	.word	0x6725ba1b
	.word	0x9845ea0e
	.word	0xe15dfec0
	.word	0x2c32f75
	.word	0x12814cf0
	.word	0xa38d4697
	.word	0xc66bd3f9
	.word	0xe7038f5f
	.word	0x9515929c
	.word	0xebbf6d7a
	.word	0xda955259
	.word	0x2dd4be83
	.word	0xd3587421
	.word	0x2949e069
	.word	0x448ec9c8
	.word	0x6a75c289
	.word	0x78f48e79
	.word	0x6b99583e
	.word	0xdd27b971
	.word	0xb6bee14f
	.word	0x17f088ad
	.word	0x66c920ac
	.word	0xb47dce3a
	.word	0x1863df4a
	.word	0x82e51a31
	.word	0x60975133
	.word	0x4562537f
	.word	0xe0b16477
	.word	0x84bb6bae
	.word	0x1cfe81a0
	.word	0x94f9082b
	.word	0x58704868
	.word	0x198f45fd
	.word	0x8794de6c
	.word	0xb7527bf8
	.word	0x23ab73d3
	.word	0xe2724b02
	.word	0x57e31f8f
	.word	0x2a6655ab
	.word	0x7b2eb28
	.word	0x32fb5c2
	.word	0x9a86c57b
	.word	0xa5d33708
	.word	0xf2302887
	.word	0xb223bfa5
	.word	0xba02036a
	.word	0x5ced1682
	.word	0x2b8acf1c
	.word	0x92a779b4
	.word	0xf0f307f2
	.word	0xa14e69e2
	.word	0xcd65daf4
	.word	0xd50605be
	.word	0x1fd13462
	.word	0x8ac4a6fe
	.word	0x9d342e53
	.word	0xa0a2f355
	.word	0x32058ae1
	.word	0x75a4f6eb
	.word	0x390b83ec
	.word	0xaa4060ef
	.word	0x65e719f
	.word	0x51bd6e10
	.word	0xf93e218a
	.word	0x3d96dd06
	.word	0xaedd3e05
	.word	0x464de6bd
	.word	0xb591548d
	.word	0x571c45d
	.word	0x6f0406d4
	.word	0xff605015
	.word	0x241998fb
	.word	0x97d6bde9
	.word	0xcc894043
	.word	0x7767d99e
	.word	0xbdb0e842
	.word	0x8807898b
	.word	0x38e7195b
	.word	0xdb79c8ee
	.word	0x47a17c0a
	.word	0xe97c420f
	.word	0xc9f8841e
	.word	0x0
	.word	0x83098086
	.word	0x48322bed
	.word	0xac1e1170
	.word	0x4e6c5a72
	.word	0xfbfd0eff
	.word	0x560f8538
	.word	0x1e3daed5
	.word	0x27362d39
	.word	0x640a0fd9
	.word	0x21685ca6
	.word	0xd19b5b54
	.word	0x3a24362e
	.word	0xb10c0a67
	.word	0xf9357e7
	.word	0xd2b4ee96
	.word	0x9e1b9b91
	.word	0x4f80c0c5
	.word	0xa261dc20
	.word	0x695a774b
	.word	0x161c121a
	.word	0xae293ba
	.word	0xe5c0a02a
	.word	0x433c22e0
	.word	0x1d121b17
	.word	0xb0e090d
	.word	0xadf28bc7
	.word	0xb92db6a8
	.word	0xc8141ea9
	.word	0x8557f119
	.word	0x4caf7507
	.word	0xbbee99dd
	.word	0xfda37f60
	.word	0x9ff70126
	.word	0xbc5c72f5
	.word	0xc544663b
	.word	0x345bfb7e
	.word	0x768b4329
	.word	0xdccb23c6
	.word	0x68b6edfc
	.word	0x63b8e4f1
	.word	0xcad731dc
	.word	0x10426385
	.word	0x40139722
	.word	0x2084c611
	.word	0x7d854a24
	.word	0xf8d2bb3d
	.word	0x11aef932
	.word	0x6dc729a1
	.word	0x4b1d9e2f
	.word	0xf3dcb230
	.word	0xec0d8652
	.word	0xd077c1e3
	.word	0x6c2bb316
	.word	0x99a970b9
	.word	0xfa119448
	.word	0x2247e964
	.word	0xc4a8fc8c
	.word	0x1aa0f03f
	.word	0xd8567d2c
	.word	0xef223390
	.word	0xc787494e
	.word	0xc1d938d1
	.word	0xfe8ccaa2
	.word	0x3698d40b
	.word	0xcfa6f581
	.word	0x28a57ade
	.word	0x26dab78e
	.word	0xa43fadbf
	.word	0xe42c3a9d
	.word	0xd507892
	.word	0x9b6a5fcc
	.word	0x62547e46
	.word	0xc2f68d13
	.word	0xe890d8b8
	.word	0x5e2e39f7
	.word	0xf582c3af
	.word	0xbe9f5d80
	.word	0x7c69d093
	.word	0xa96fd52d
	.word	0xb3cf2512
	.word	0x3bc8ac99
	.word	0xa710187d
	.word	0x6ee89c63
	.word	0x7bdb3bbb
	.word	0x9cd2678
	.word	0xf46e5918
	.word	0x1ec9ab7
	.word	0xa8834f9a
	.word	0x65e6956e
	.word	0x7eaaffe6
	.word	0x821bccf
	.word	0xe6ef15e8
	.word	0xd9bae79b
	.word	0xce4a6f36
	.word	0xd4ea9f09
	.word	0xd629b07c
	.word	0xaf31a4b2
	.word	0x312a3f23
	.word	0x30c6a594
	.word	0xc035a266
	.word	0x37744ebc
	.word	0xa6fc82ca
	.word	0xb0e090d0
	.word	0x1533a7d8
	.word	0x4af10498
	.word	0xf741ecda
	.word	0xe7fcd50
	.word	0x2f1791f6
	.word	0x8d764dd6
	.word	0x4d43efb0
	.word	0x54ccaa4d
	.word	0xdfe49604
	.word	0xe39ed1b5
	.word	0x1b4c6a88
	.word	0xb8c12c1f
	.word	0x7f466551
	.word	0x49d5eea
	.word	0x5d018c35
	.word	0x73fa8774
	.word	0x2efb0b41
	.word	0x5ab3671d
	.word	0x5292dbd2
	.word	0x33e91056
	.word	0x136dd647
	.word	0x8c9ad761
	.word	0x7a37a10c
	.word	0x8e59f814
	.word	0x89eb133c
	.word	0xeecea927
	.word	0x35b761c9
	.word	0xede11ce5
	.word	0x3c7a47b1
	.word	0x599cd2df
	.word	0x3f55f273
	.word	0x791814ce
	.word	0xbf73c737
	.word	0xea53f7cd
	.word	0x5b5ffdaa
	.word	0x14df3d6f
	.word	0x867844db
	.word	0x81caaff3
	.word	0x3eb968c4
	.word	0x2c382434
	.word	0x5fc2a340
	.word	0x72161dc3
	.word	0xcbce225
	.word	0x8b283c49
	.word	0x41ff0d95
	.word	0x7139a801
	.word	0xde080cb3
	.word	0x9cd8b4e4
	.word	0x906456c1
	.word	0x617bcb84
	.word	0x70d532b6
	.word	0x74486c5c
	.word	0x42d0b857
#ifndef NO_AES
	.text
	.type	L_AES_SEK_ARM32_rcon, %object
	.size	L_AES_SEK_ARM32_rcon, 40
	.align	4
L_AES_SEK_ARM32_rcon:
	.word	0x1000000
	.word	0x2000000
	.word	0x4000000
	.word	0x8000000
	.word	0x10000000
	.word	0x20000000
	.word	0x40000000
	.word	0x80000000
	.word	0x1b000000
	.word	0x36000000
	.text
	.align	4
	.globl	AES_set_encrypt_key
	.type	AES_set_encrypt_key, %function
AES_set_encrypt_key:
	push	{r4, r5, r6, r7, r8, lr}
	adr	r4, AES_set_encrypt_key
	mov	r8, #AES_set_encrypt_key-L_AES_ARM32_te
	sub	r8, r4, r8
	adr	lr, L_AES_SEK_ARM32_rcon
	cmp	r1, #0x80
	beq	L_AES_set_encrypt_key_start_128
	cmp	r1, #0xc0
	beq	L_AES_set_encrypt_key_start_192
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	stm	r2!, {r4, r5, r6, r7}
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0, #16]
	ldr	r5, [r0, #20]
#else
	ldrd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #24]
	ldr	r7, [r0, #28]
#else
	ldrd	r6, r7, [r0, #24]
#endif
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	stm	r2, {r4, r5, r6, r7}
	sub	r2, r2, #16
	mov	r12, #6
L_AES_set_encrypt_key_loop_256:
	mov	r3, r7
	mov	r7, #0xff
	and	r5, r7, r3, lsr #8
	and	r4, r7, r3
	and	r6, r7, r3, lsr #16
	lsr	r3, r3, #24
	ldrb	r4, [r8, r4, lsl #2]
	ldrb	r6, [r8, r6, lsl #2]
	ldrb	r5, [r8, r5, lsl #2]
	ldrb	r3, [r8, r3, lsl #2]
	eor	r3, r3, r4, lsl #8
	eor	r3, r3, r5, lsl #16
	eor	r3, r3, r6, lsl #24
	ldm	r2!, {r4, r5, r6, r7}
	eor	r4, r4, r3
	ldm	lr!, {r3}
	eor	r4, r4, r3
	eor	r5, r5, r4
	eor	r6, r6, r5
	eor	r7, r7, r6
	add	r2, r2, #16
	stm	r2, {r4, r5, r6, r7}
	sub	r2, r2, #16
	mov	r3, r7
	mov	r7, #0xff
	and	r4, r7, r3, lsr #8
	and	r5, r7, r3, lsr #16
	lsr	r6, r3, #24
	and	r3, r7, r3
	ldrb	r4, [r8, r4, lsl #2]
	ldrb	r6, [r8, r6, lsl #2]
	ldrb	r5, [r8, r5, lsl #2]
	ldrb	r3, [r8, r3, lsl #2]
	eor	r3, r3, r4, lsl #8
	eor	r3, r3, r5, lsl #16
	eor	r3, r3, r6, lsl #24
	ldm	r2!, {r4, r5, r6, r7}
	eor	r4, r4, r3
	eor	r5, r5, r4
	eor	r6, r6, r5
	eor	r7, r7, r6
	add	r2, r2, #16
	stm	r2, {r4, r5, r6, r7}
	sub	r2, r2, #16
	subs	r12, r12, #1
	bne	L_AES_set_encrypt_key_loop_256
	mov	r3, r7
	mov	r7, #0xff
	and	r5, r7, r3, lsr #8
	and	r4, r7, r3
	and	r6, r7, r3, lsr #16
	lsr	r3, r3, #24
	ldrb	r4, [r8, r4, lsl #2]
	ldrb	r6, [r8, r6, lsl #2]
	ldrb	r5, [r8, r5, lsl #2]
	ldrb	r3, [r8, r3, lsl #2]
	eor	r3, r3, r4, lsl #8
	eor	r3, r3, r5, lsl #16
	eor	r3, r3, r6, lsl #24
	ldm	r2!, {r4, r5, r6, r7}
	eor	r4, r4, r3
	ldm	lr!, {r3}
	eor	r4, r4, r3
	eor	r5, r5, r4
	eor	r6, r6, r5
	eor	r7, r7, r6
	add	r2, r2, #16
	stm	r2, {r4, r5, r6, r7}
	sub	r2, r2, #16
	b	L_AES_set_encrypt_key_end
L_AES_set_encrypt_key_start_192:
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r1, [r0, #20]
	ldr	r0, [r0, #16]
#else
	ldrd	r0, r1, [r0, #16]
#endif
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	rev	r0, r0
	rev	r1, r1
	stm	r2, {r4, r5, r6, r7}
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r0, [r2, #16]
	str	r1, [r2, #20]
#else
	strd	r0, r1, [r2, #16]
#endif
	mov	r7, r1
	mov	r12, #7
L_AES_set_encrypt_key_loop_192:
	mov	r3, r7
	mov	r5, #0xff
	and	r1, r5, r3, lsr #8
	and	r0, r5, r3
	and	r4, r5, r3, lsr #16
	lsr	r3, r3, #24
	ldrb	r0, [r8, r0, lsl #2]
	ldrb	r4, [r8, r4, lsl #2]
	ldrb	r1, [r8, r1, lsl #2]
	ldrb	r3, [r8, r3, lsl #2]
	eor	r3, r3, r0, lsl #8
	eor	r3, r3, r1, lsl #16
	eor	r3, r3, r4, lsl #24
	ldm	r2!, {r0, r1, r4, r5, r6, r7}
	eor	r0, r0, r3
	ldm	lr!, {r3}
	eor	r0, r0, r3
	eor	r1, r1, r0
	eor	r4, r4, r1
	eor	r5, r5, r4
	eor	r6, r6, r5
	eor	r7, r7, r6
	stm	r2, {r0, r1, r4, r5, r6, r7}
	subs	r12, r12, #1
	bne	L_AES_set_encrypt_key_loop_192
	mov	r3, r7
	mov	r5, #0xff
	and	r1, r5, r3, lsr #8
	and	r0, r5, r3
	and	r4, r5, r3, lsr #16
	lsr	r3, r3, #24
	ldrb	r0, [r8, r0, lsl #2]
	ldrb	r4, [r8, r4, lsl #2]
	ldrb	r1, [r8, r1, lsl #2]
	ldrb	r3, [r8, r3, lsl #2]
	eor	r3, r3, r0, lsl #8
	eor	r3, r3, r1, lsl #16
	eor	r3, r3, r4, lsl #24
	ldm	r2!, {r0, r1, r4, r5, r6, r7}
	eor	r0, r0, r3
	ldm	lr!, {r3}
	eor	r0, r0, r3
	eor	r1, r1, r0
	eor	r4, r4, r1
	eor	r5, r5, r4
	stm	r2, {r0, r1, r4, r5}
	b	L_AES_set_encrypt_key_end
L_AES_set_encrypt_key_start_128:
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
#else
	ldrd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
#else
	ldrd	r6, r7, [r0, #8]
#endif
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	stm	r2, {r4, r5, r6, r7}
	mov	r12, #10
L_AES_set_encrypt_key_loop_128:
	mov	r3, r7
	mov	r7, #0xff
	and	r5, r7, r3, lsr #8
	and	r4, r7, r3
	and	r6, r7, r3, lsr #16
	lsr	r3, r3, #24
	ldrb	r4, [r8, r4, lsl #2]
	ldrb	r6, [r8, r6, lsl #2]
	ldrb	r5, [r8, r5, lsl #2]
	ldrb	r3, [r8, r3, lsl #2]
	eor	r3, r3, r4, lsl #8
	eor	r3, r3, r5, lsl #16
	eor	r3, r3, r6, lsl #24
	ldm	r2!, {r4, r5, r6, r7}
	eor	r4, r4, r3
	ldm	lr!, {r3}
	eor	r4, r4, r3
	eor	r5, r5, r4
	eor	r6, r6, r5
	eor	r7, r7, r6
	stm	r2, {r4, r5, r6, r7}
	subs	r12, r12, #1
	bne	L_AES_set_encrypt_key_loop_128
L_AES_set_encrypt_key_end:
	pop	{r4, r5, r6, r7, r8, pc}
	.size	AES_set_encrypt_key,.-AES_set_encrypt_key
#ifdef HAVE_AES_DECRYPT
	.text
	.align	4
	.globl	AES_invert_key
	.type	AES_invert_key, %function
AES_invert_key:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	adr	r4, AES_invert_key
	mov	r9, #AES_invert_key-L_AES_ARM32_te
	mov	r10, #AES_invert_key-L_AES_ARM32_td
	sub	r9, r4, r9
	sub	r10, r4, r10
	add	r8, r0, r1, lsl #4
	mov	r11, r1
L_AES_invert_key_loop:
	ldm	r0, {r2, r3, r12, lr}
	ldm	r8, {r4, r5, r6, r7}
	stm	r8, {r2, r3, r12, lr}
	stm	r0!, {r4, r5, r6, r7}
	subs	r11, r11, #2
	sub	r8, r8, #16
	bne	L_AES_invert_key_loop
	sub	r0, r0, r1, lsl #3
	add	r0, r0, #16
	sub	r11, r1, #1
	mov	r1, #0xff
L_AES_invert_key_mix_loop:
	ldm	r0, {r2, r3, r12, lr}
	lsr	r4, r2, #24
	and	r7, r1, r2
	and	r8, r1, r2, lsr #16
	and	r6, r1, r2, lsr #8
	ldrb	r4, [r9, r4, lsl #2]
	ldrb	r7, [r9, r7, lsl #2]
	ldrb	r8, [r9, r8, lsl #2]
	ldrb	r6, [r9, r6, lsl #2]
	ldr	r4, [r10, r4, lsl #2]
	ldr	r7, [r10, r7, lsl #2]
	ldr	r8, [r10, r8, lsl #2]
	ldr	r6, [r10, r6, lsl #2]
	eor	r8, r8, r4, ror #24
	eor	r8, r8, r7, ror #16
	eor	r8, r8, r6, ror #8
	str	r8, [r0], #4
	lsr	r4, r3, #24
	and	r7, r1, r3
	and	r8, r1, r3, lsr #16
	and	r6, r1, r3, lsr #8
	ldrb	r4, [r9, r4, lsl #2]
	ldrb	r7, [r9, r7, lsl #2]
	ldrb	r8, [r9, r8, lsl #2]
	ldrb	r6, [r9, r6, lsl #2]
	ldr	r4, [r10, r4, lsl #2]
	ldr	r7, [r10, r7, lsl #2]
	ldr	r8, [r10, r8, lsl #2]
	ldr	r6, [r10, r6, lsl #2]
	eor	r8, r8, r4, ror #24
	eor	r8, r8, r7, ror #16
	eor	r8, r8, r6, ror #8
	str	r8, [r0], #4
	lsr	r4, r12, #24
	and	r7, r1, r12
	and	r8, r1, r12, lsr #16
	and	r6, r1, r12, lsr #8
	ldrb	r4, [r9, r4, lsl #2]
	ldrb	r7, [r9, r7, lsl #2]
	ldrb	r8, [r9, r8, lsl #2]
	ldrb	r6, [r9, r6, lsl #2]
	ldr	r4, [r10, r4, lsl #2]
	ldr	r7, [r10, r7, lsl #2]
	ldr	r8, [r10, r8, lsl #2]
	ldr	r6, [r10, r6, lsl #2]
	eor	r8, r8, r4, ror #24
	eor	r8, r8, r7, ror #16
	eor	r8, r8, r6, ror #8
	str	r8, [r0], #4
	lsr	r4, lr, #24
	and	r7, r1, lr
	and	r8, r1, lr, lsr #16
	and	r6, r1, lr, lsr #8
	ldrb	r4, [r9, r4, lsl #2]
	ldrb	r7, [r9, r7, lsl #2]
	ldrb	r8, [r9, r8, lsl #2]
	ldrb	r6, [r9, r6, lsl #2]
	ldr	r4, [r10, r4, lsl #2]
	ldr	r7, [r10, r7, lsl #2]
	ldr	r8, [r10, r8, lsl #2]
	ldr	r6, [r10, r6, lsl #2]
	eor	r8, r8, r4, ror #24
	eor	r8, r8, r7, ror #16
	eor	r8, r8, r6, ror #8
	str	r8, [r0], #4
	subs	r11, r11, #1
	bne	L_AES_invert_key_mix_loop
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	AES_invert_key,.-AES_invert_key
#endif /* HAVE_AES_DECRYPT */
#if defined(HAVE_AESCCM) || defined(HAVE_AESGCM) || defined(WOLFSSL_AES_DIRECT) || defined(WOLFSSL_AES_COUNTER)
	.text
	.align	4
	.globl	AES_encrypt_block
	.type	AES_encrypt_block, %function
AES_encrypt_block:
L_AES_encrypt_block_14:
	push	{r2, lr}
	ldr	lr, [sp, #12]
	mov	r2, #6
	b	L_AES_encrypt_block_nr
L_AES_encrypt_block_12:
	push	{r2, lr}
	ldr	lr, [sp, #12]
	mov	r2, #5
	b	L_AES_encrypt_block_nr
L_AES_encrypt_block_10:
	push	{r2, lr}
	ldr	lr, [sp, #12]
	mov	r2, #4
L_AES_encrypt_block_nr:
	lsr	r11, r4, #24
	and	r1, r12, r7
	and	r8, r12, r5, lsr #16
	and	r0, r12, r6, lsr #8
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	ldr	r8, [lr, r8, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	and	r9, r12, r6, lsr #16
	eor	r8, r8, r11, ror #24
	lsr	r11, r5, #24
	eor	r8, r8, r0, ror #8
	and	r0, r12, r7, lsr #8
	eor	r8, r8, r1, ror #16
	and	r1, r12, r4
	ldr	r9, [lr, r9, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r10, r12, r7, lsr #16
	eor	r9, r9, r11, ror #24
	lsr	r11, r6, #24
	eor	r9, r9, r0, ror #8
	and	r0, r12, r4, lsr #8
	eor	r9, r9, r1, ror #16
	and	r1, r12, r5
	ldr	r10, [lr, r10, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r6, r12, r6
	eor	r10, r10, r11, ror #24
	and	r11, r12, r4, lsr #16
	eor	r10, r10, r0, ror #8
	lsr	r0, r7, #24
	eor	r10, r10, r1, ror #16
	and	r1, r12, r5, lsr #8
	ldr	r6, [lr, r6, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	eor	r0, r0, r6, ror #24
	ldm	r3!, {r4, r5, r6, r7}
	eor	r11, r11, r0, ror #24
	eor	r11, r11, r1, ror #8
	#   XOR in Key Schedule
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r7, r8, #24
	and	r1, r12, r11
	and	r4, r12, r9, lsr #16
	and	r0, r12, r10, lsr #8
	ldr	r7, [lr, r7, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	ldr	r4, [lr, r4, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	and	r5, r12, r10, lsr #16
	eor	r4, r4, r7, ror #24
	lsr	r7, r9, #24
	eor	r4, r4, r0, ror #8
	and	r0, r12, r11, lsr #8
	eor	r4, r4, r1, ror #16
	and	r1, r12, r8
	ldr	r5, [lr, r5, lsl #2]
	ldr	r7, [lr, r7, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r6, r12, r11, lsr #16
	eor	r5, r5, r7, ror #24
	lsr	r7, r10, #24
	eor	r5, r5, r0, ror #8
	and	r0, r12, r8, lsr #8
	eor	r5, r5, r1, ror #16
	and	r1, r12, r9
	ldr	r6, [lr, r6, lsl #2]
	ldr	r7, [lr, r7, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r10, r12, r10
	eor	r6, r6, r7, ror #24
	and	r7, r12, r8, lsr #16
	eor	r6, r6, r0, ror #8
	lsr	r0, r11, #24
	eor	r6, r6, r1, ror #16
	and	r1, r12, r9, lsr #8
	ldr	r10, [lr, r10, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r7, [lr, r7, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	eor	r0, r0, r10, ror #24
	ldm	r3!, {r8, r9, r10, r11}
	eor	r7, r7, r0, ror #24
	eor	r7, r7, r1, ror #8
	#   XOR in Key Schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	subs	r2, r2, #1
	bne	L_AES_encrypt_block_nr
	lsr	r11, r4, #24
	and	r1, r12, r7
	and	r8, r12, r5, lsr #16
	and	r0, r12, r6, lsr #8
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	ldr	r8, [lr, r8, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	and	r9, r12, r6, lsr #16
	eor	r8, r8, r11, ror #24
	lsr	r11, r5, #24
	eor	r8, r8, r0, ror #8
	and	r0, r12, r7, lsr #8
	eor	r8, r8, r1, ror #16
	and	r1, r12, r4
	ldr	r9, [lr, r9, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r10, r12, r7, lsr #16
	eor	r9, r9, r11, ror #24
	lsr	r11, r6, #24
	eor	r9, r9, r0, ror #8
	and	r0, r12, r4, lsr #8
	eor	r9, r9, r1, ror #16
	and	r1, r12, r5
	ldr	r10, [lr, r10, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r6, r12, r6
	eor	r10, r10, r11, ror #24
	and	r11, r12, r4, lsr #16
	eor	r10, r10, r0, ror #8
	lsr	r0, r7, #24
	eor	r10, r10, r1, ror #16
	and	r1, r12, r5, lsr #8
	ldr	r6, [lr, r6, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	eor	r0, r0, r6, ror #24
	ldm	r3!, {r4, r5, r6, r7}
	eor	r11, r11, r0, ror #24
	eor	r11, r11, r1, ror #8
	#   XOR in Key Schedule
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	and	r7, r12, r10, lsr #8
	lsr	r1, r8, #24
	and	r4, r12, r11
	and	r0, r12, r9, lsr #16
	ldrb	r7, [lr, r7, lsl #2]
	ldrb	r1, [lr, r1, lsl #2]
	ldrb	r4, [lr, r4, lsl #2]
	ldrb	r0, [lr, r0, lsl #2]
	and	r5, r12, r8
	eor	r4, r4, r7, lsl #8
	and	r7, r12, r11, lsr #8
	eor	r4, r4, r0, lsl #16
	and	r0, r12, r10, lsr #16
	eor	r4, r4, r1, lsl #24
	lsr	r1, r9, #24
	ldrb	r7, [lr, r7, lsl #2]
	ldrb	r1, [lr, r1, lsl #2]
	ldrb	r5, [lr, r5, lsl #2]
	ldrb	r0, [lr, r0, lsl #2]
	and	r6, r12, r9
	eor	r5, r5, r7, lsl #8
	and	r7, r12, r8, lsr #8
	eor	r5, r5, r0, lsl #16
	and	r0, r12, r11, lsr #16
	eor	r5, r5, r1, lsl #24
	lsr	r1, r10, #24
	ldrb	r7, [lr, r7, lsl #2]
	ldrb	r1, [lr, r1, lsl #2]
	ldrb	r6, [lr, r6, lsl #2]
	ldrb	r0, [lr, r0, lsl #2]
	lsr	r11, r11, #24
	eor	r6, r6, r7, lsl #8
	and	r7, r12, r10
	eor	r6, r6, r0, lsl #16
	and	r0, r12, r9, lsr #8
	eor	r6, r6, r1, lsl #24
	and	r1, r12, r8, lsr #16
	ldrb	r11, [lr, r11, lsl #2]
	ldrb	r0, [lr, r0, lsl #2]
	ldrb	r7, [lr, r7, lsl #2]
	ldrb	r1, [lr, r1, lsl #2]
	eor	r0, r0, r11, lsl #16
	ldm	r3, {r8, r9, r10, r11}
	eor	r7, r7, r0, lsl #8
	eor	r7, r7, r1, lsl #16
	#   XOR in Key Schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	lr, [sp, #12]
	pop	{r2, lr}
	bx	lr
	bx	lr
	.size	AES_encrypt_block,.-AES_encrypt_block
#if defined(HAVE_AESCCM) || defined(HAVE_AESGCM) || defined(WOLFSSL_AES_DIRECT) || defined(WOLFSSL_AES_COUNTER)
	.text
	.align	4
	.globl	AES_ECB_encrypt
	.type	AES_ECB_encrypt, %function
AES_ECB_encrypt:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	adr	r4, AES_ECB_encrypt
	mov	lr, #AES_ECB_encrypt-L_AES_ARM32_te
	sub	lr, r4, lr
	ldr	r12, [sp, #36]
	cmp	r12, #10
	beq	L_AES_ECB_encrypt_start_block_128
	cmp	r12, #12
	beq	L_AES_ECB_encrypt_start_block_192
	mov	r12, #0xff
L_AES_ECB_encrypt_loop_block_256:
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	push	{r0, r1}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_14
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_ECB_encrypt_loop_block_256
	b	L_AES_ECB_encrypt_end
L_AES_ECB_encrypt_start_block_192:
	mov	r12, #0xff
L_AES_ECB_encrypt_loop_block_192:
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	push	{r0, r1}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_12
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_ECB_encrypt_loop_block_192
	b	L_AES_ECB_encrypt_end
L_AES_ECB_encrypt_start_block_128:
	mov	r12, #0xff
L_AES_ECB_encrypt_loop_block_128:
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	push	{r0, r1}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_10
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_ECB_encrypt_loop_block_128
L_AES_ECB_encrypt_end:
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	AES_ECB_encrypt,.-AES_ECB_encrypt
#endif /* HAVE_AESCCM || HAVE_AESGCM || WOLFSSL_AES_DIRECT || WOLFSSL_AES_COUNTER */
#ifdef HAVE_AES_CBC
	.text
	.align	4
	.globl	AES_CBC_encrypt
	.type	AES_CBC_encrypt, %function
AES_CBC_encrypt:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	ldr	r12, [sp, #36]
	ldr	lr, [sp, #40]
	ldm	lr, {r4, r5, r6, r7}
	push	{lr}
	adr	r8, AES_CBC_encrypt
	mov	lr, #AES_CBC_encrypt-L_AES_ARM32_te
	sub	lr, r8, lr
	cmp	r12, #10
	beq	L_AES_CBC_encrypt_start_block_128
	cmp	r12, #12
	beq	L_AES_CBC_encrypt_start_block_192
	mov	r12, #0xff
L_AES_CBC_encrypt_loop_block_256:
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	push	{r0, r1}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_14
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CBC_encrypt_loop_block_256
	b	L_AES_CBC_encrypt_end
L_AES_CBC_encrypt_start_block_192:
	mov	r12, #0xff
L_AES_CBC_encrypt_loop_block_192:
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	push	{r0, r1}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_12
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CBC_encrypt_loop_block_192
	b	L_AES_CBC_encrypt_end
L_AES_CBC_encrypt_start_block_128:
	mov	r12, #0xff
L_AES_CBC_encrypt_loop_block_128:
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	push	{r0, r1}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_10
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CBC_encrypt_loop_block_128
L_AES_CBC_encrypt_end:
	pop	{lr}
	stm	lr, {r4, r5, r6, r7}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	AES_CBC_encrypt,.-AES_CBC_encrypt
#endif /* HAVE_AES_CBC */
#ifdef WOLFSSL_AES_COUNTER
	.text
	.align	4
	.globl	AES_CTR_encrypt
	.type	AES_CTR_encrypt, %function
AES_CTR_encrypt:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	ldr	r12, [sp, #36]
	ldr	lr, [sp, #40]
	ldm	lr, {r4, r5, r6, r7}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	stm	lr, {r4, r5, r6, r7}
	push	{lr}
	adr	r8, AES_CTR_encrypt
	mov	lr, #AES_CTR_encrypt-L_AES_ARM32_te
	sub	lr, r8, lr
	cmp	r12, #10
	beq	L_AES_CTR_encrypt_start_block_128
	cmp	r12, #12
	beq	L_AES_CTR_encrypt_start_block_192
	mov	r12, #0xff
L_AES_CTR_encrypt_loop_block_256:
	push	{r0, r1}
	ldr	r0, [sp, #8]
	adds	r11, r7, #1
	adcs	r10, r6, #0
	adcs	r9, r5, #0
	adc	r8, r4, #0
	stm	r0, {r8, r9, r10, r11}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_14
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r8
	eor	r5, r9
	eor	r6, r10
	eor	r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	ldr	r8, [sp]
	ldm	r8, {r4, r5, r6, r7}
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CTR_encrypt_loop_block_256
	b	L_AES_CTR_encrypt_end
L_AES_CTR_encrypt_start_block_192:
	mov	r12, #0xff
L_AES_CTR_encrypt_loop_block_192:
	push	{r0, r1}
	ldr	r0, [sp, #8]
	adds	r11, r7, #1
	adcs	r10, r6, #0
	adcs	r9, r5, #0
	adc	r8, r4, #0
	stm	r0, {r8, r9, r10, r11}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_12
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r8
	eor	r5, r9
	eor	r6, r10
	eor	r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	ldr	r8, [sp]
	ldm	r8, {r4, r5, r6, r7}
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CTR_encrypt_loop_block_192
	b	L_AES_CTR_encrypt_end
L_AES_CTR_encrypt_start_block_128:
	mov	r12, #0xff
L_AES_CTR_encrypt_loop_block_128:
	push	{r0, r1}
	ldr	r0, [sp, #8]
	adds	r11, r7, #1
	adcs	r10, r6, #0
	adcs	r9, r5, #0
	adc	r8, r4, #0
	stm	r0, {r8, r9, r10, r11}
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_10
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r8
	eor	r5, r9
	eor	r6, r10
	eor	r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	ldr	r8, [sp]
	ldm	r8, {r4, r5, r6, r7}
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CTR_encrypt_loop_block_128
L_AES_CTR_encrypt_end:
	pop	{lr}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	stm	lr, {r4, r5, r6, r7}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	AES_CTR_encrypt,.-AES_CTR_encrypt
#endif /* WOLFSSL_AES_COUNTER */
#endif /* HAVE_AESCCM || HAVE_AESGCM || WOLFSSL_AES_DIRECT || WOLFSSL_AES_COUNTER */
#ifdef HAVE_AES_DECRYPT
#if defined(WOLFSSL_AES_DIRECT) || defined(WOLFSSL_AES_COUNTER) || defined(HAVE_AES_CBC)
	.text
	.type	L_AES_ARM32_td4, %object
	.size	L_AES_ARM32_td4, 256
	.align	4
L_AES_ARM32_td4:
	.byte	0x52
	.byte	0x9
	.byte	0x6a
	.byte	0xd5
	.byte	0x30
	.byte	0x36
	.byte	0xa5
	.byte	0x38
	.byte	0xbf
	.byte	0x40
	.byte	0xa3
	.byte	0x9e
	.byte	0x81
	.byte	0xf3
	.byte	0xd7
	.byte	0xfb
	.byte	0x7c
	.byte	0xe3
	.byte	0x39
	.byte	0x82
	.byte	0x9b
	.byte	0x2f
	.byte	0xff
	.byte	0x87
	.byte	0x34
	.byte	0x8e
	.byte	0x43
	.byte	0x44
	.byte	0xc4
	.byte	0xde
	.byte	0xe9
	.byte	0xcb
	.byte	0x54
	.byte	0x7b
	.byte	0x94
	.byte	0x32
	.byte	0xa6
	.byte	0xc2
	.byte	0x23
	.byte	0x3d
	.byte	0xee
	.byte	0x4c
	.byte	0x95
	.byte	0xb
	.byte	0x42
	.byte	0xfa
	.byte	0xc3
	.byte	0x4e
	.byte	0x8
	.byte	0x2e
	.byte	0xa1
	.byte	0x66
	.byte	0x28
	.byte	0xd9
	.byte	0x24
	.byte	0xb2
	.byte	0x76
	.byte	0x5b
	.byte	0xa2
	.byte	0x49
	.byte	0x6d
	.byte	0x8b
	.byte	0xd1
	.byte	0x25
	.byte	0x72
	.byte	0xf8
	.byte	0xf6
	.byte	0x64
	.byte	0x86
	.byte	0x68
	.byte	0x98
	.byte	0x16
	.byte	0xd4
	.byte	0xa4
	.byte	0x5c
	.byte	0xcc
	.byte	0x5d
	.byte	0x65
	.byte	0xb6
	.byte	0x92
	.byte	0x6c
	.byte	0x70
	.byte	0x48
	.byte	0x50
	.byte	0xfd
	.byte	0xed
	.byte	0xb9
	.byte	0xda
	.byte	0x5e
	.byte	0x15
	.byte	0x46
	.byte	0x57
	.byte	0xa7
	.byte	0x8d
	.byte	0x9d
	.byte	0x84
	.byte	0x90
	.byte	0xd8
	.byte	0xab
	.byte	0x0
	.byte	0x8c
	.byte	0xbc
	.byte	0xd3
	.byte	0xa
	.byte	0xf7
	.byte	0xe4
	.byte	0x58
	.byte	0x5
	.byte	0xb8
	.byte	0xb3
	.byte	0x45
	.byte	0x6
	.byte	0xd0
	.byte	0x2c
	.byte	0x1e
	.byte	0x8f
	.byte	0xca
	.byte	0x3f
	.byte	0xf
	.byte	0x2
	.byte	0xc1
	.byte	0xaf
	.byte	0xbd
	.byte	0x3
	.byte	0x1
	.byte	0x13
	.byte	0x8a
	.byte	0x6b
	.byte	0x3a
	.byte	0x91
	.byte	0x11
	.byte	0x41
	.byte	0x4f
	.byte	0x67
	.byte	0xdc
	.byte	0xea
	.byte	0x97
	.byte	0xf2
	.byte	0xcf
	.byte	0xce
	.byte	0xf0
	.byte	0xb4
	.byte	0xe6
	.byte	0x73
	.byte	0x96
	.byte	0xac
	.byte	0x74
	.byte	0x22
	.byte	0xe7
	.byte	0xad
	.byte	0x35
	.byte	0x85
	.byte	0xe2
	.byte	0xf9
	.byte	0x37
	.byte	0xe8
	.byte	0x1c
	.byte	0x75
	.byte	0xdf
	.byte	0x6e
	.byte	0x47
	.byte	0xf1
	.byte	0x1a
	.byte	0x71
	.byte	0x1d
	.byte	0x29
	.byte	0xc5
	.byte	0x89
	.byte	0x6f
	.byte	0xb7
	.byte	0x62
	.byte	0xe
	.byte	0xaa
	.byte	0x18
	.byte	0xbe
	.byte	0x1b
	.byte	0xfc
	.byte	0x56
	.byte	0x3e
	.byte	0x4b
	.byte	0xc6
	.byte	0xd2
	.byte	0x79
	.byte	0x20
	.byte	0x9a
	.byte	0xdb
	.byte	0xc0
	.byte	0xfe
	.byte	0x78
	.byte	0xcd
	.byte	0x5a
	.byte	0xf4
	.byte	0x1f
	.byte	0xdd
	.byte	0xa8
	.byte	0x33
	.byte	0x88
	.byte	0x7
	.byte	0xc7
	.byte	0x31
	.byte	0xb1
	.byte	0x12
	.byte	0x10
	.byte	0x59
	.byte	0x27
	.byte	0x80
	.byte	0xec
	.byte	0x5f
	.byte	0x60
	.byte	0x51
	.byte	0x7f
	.byte	0xa9
	.byte	0x19
	.byte	0xb5
	.byte	0x4a
	.byte	0xd
	.byte	0x2d
	.byte	0xe5
	.byte	0x7a
	.byte	0x9f
	.byte	0x93
	.byte	0xc9
	.byte	0x9c
	.byte	0xef
	.byte	0xa0
	.byte	0xe0
	.byte	0x3b
	.byte	0x4d
	.byte	0xae
	.byte	0x2a
	.byte	0xf5
	.byte	0xb0
	.byte	0xc8
	.byte	0xeb
	.byte	0xbb
	.byte	0x3c
	.byte	0x83
	.byte	0x53
	.byte	0x99
	.byte	0x61
	.byte	0x17
	.byte	0x2b
	.byte	0x4
	.byte	0x7e
	.byte	0xba
	.byte	0x77
	.byte	0xd6
	.byte	0x26
	.byte	0xe1
	.byte	0x69
	.byte	0x14
	.byte	0x63
	.byte	0x55
	.byte	0x21
	.byte	0xc
	.byte	0x7d
	.text
	.align	4
	.globl	AES_decrypt_block
	.type	AES_decrypt_block, %function
AES_decrypt_block:
L_AES_decrypt_block_14:
	push	{lr}
	ldr	lr, [sp, #8]
	mov	r12, #6
	b	L_AES_decrypt_block_nr
L_AES_decrypt_block_12:
	push	{lr}
	ldr	lr, [sp, #8]
	mov	r12, #5
	b	L_AES_decrypt_block_nr
L_AES_decrypt_block_10:
	push	{lr}
	ldr	lr, [sp, #8]
	mov	r12, #4
L_AES_decrypt_block_nr:
	lsr	r11, r4, #24
	and	r1, r2, r5
	and	r8, r2, r7, lsr #16
	and	r0, r2, r6, lsr #8
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	ldr	r8, [lr, r8, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	and	r9, r2, r4, lsr #16
	eor	r8, r8, r11, ror #24
	lsr	r11, r5, #24
	eor	r8, r8, r0, ror #8
	and	r0, r2, r7, lsr #8
	eor	r8, r8, r1, ror #16
	and	r1, r2, r6
	ldr	r9, [lr, r9, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r10, r2, r5, lsr #16
	eor	r9, r9, r11, ror #24
	lsr	r11, r6, #24
	eor	r9, r9, r0, ror #8
	and	r0, r2, r4, lsr #8
	eor	r9, r9, r1, ror #16
	and	r1, r2, r7
	ldr	r10, [lr, r10, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r4, r2, r4
	eor	r10, r10, r11, ror #24
	and	r11, r2, r6, lsr #16
	eor	r10, r10, r0, ror #8
	lsr	r0, r7, #24
	eor	r10, r10, r1, ror #16
	and	r1, r2, r5, lsr #8
	ldr	r4, [lr, r4, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	eor	r0, r0, r4, ror #24
	ldm	r3!, {r4, r5, r6, r7}
	eor	r11, r11, r1, ror #8
	eor	r11, r11, r0, ror #24
	#   XOR in Key Schedule
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r7, r8, #24
	and	r1, r2, r9
	and	r4, r2, r11, lsr #16
	and	r0, r2, r10, lsr #8
	ldr	r7, [lr, r7, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	ldr	r4, [lr, r4, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	and	r5, r2, r8, lsr #16
	eor	r4, r4, r7, ror #24
	lsr	r7, r9, #24
	eor	r4, r4, r0, ror #8
	and	r0, r2, r11, lsr #8
	eor	r4, r4, r1, ror #16
	and	r1, r2, r10
	ldr	r5, [lr, r5, lsl #2]
	ldr	r7, [lr, r7, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r6, r2, r9, lsr #16
	eor	r5, r5, r7, ror #24
	lsr	r7, r10, #24
	eor	r5, r5, r0, ror #8
	and	r0, r2, r8, lsr #8
	eor	r5, r5, r1, ror #16
	and	r1, r2, r11
	ldr	r6, [lr, r6, lsl #2]
	ldr	r7, [lr, r7, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r8, r2, r8
	eor	r6, r6, r7, ror #24
	and	r7, r2, r10, lsr #16
	eor	r6, r6, r0, ror #8
	lsr	r0, r11, #24
	eor	r6, r6, r1, ror #16
	and	r1, r2, r9, lsr #8
	ldr	r8, [lr, r8, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r7, [lr, r7, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	eor	r0, r0, r8, ror #24
	ldm	r3!, {r8, r9, r10, r11}
	eor	r7, r7, r1, ror #8
	eor	r7, r7, r0, ror #24
	#   XOR in Key Schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	subs	r12, r12, #1
	bne	L_AES_decrypt_block_nr
	lsr	r11, r4, #24
	and	r1, r2, r5
	and	r8, r2, r7, lsr #16
	and	r0, r2, r6, lsr #8
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	ldr	r8, [lr, r8, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	and	r9, r2, r4, lsr #16
	eor	r8, r8, r11, ror #24
	lsr	r11, r5, #24
	eor	r8, r8, r0, ror #8
	and	r0, r2, r7, lsr #8
	eor	r8, r8, r1, ror #16
	and	r1, r2, r6
	ldr	r9, [lr, r9, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r10, r2, r5, lsr #16
	eor	r9, r9, r11, ror #24
	lsr	r11, r6, #24
	eor	r9, r9, r0, ror #8
	and	r0, r2, r4, lsr #8
	eor	r9, r9, r1, ror #16
	and	r1, r2, r7
	ldr	r10, [lr, r10, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	and	r4, r2, r4
	eor	r10, r10, r11, ror #24
	and	r11, r2, r6, lsr #16
	eor	r10, r10, r0, ror #8
	lsr	r0, r7, #24
	eor	r10, r10, r1, ror #16
	and	r1, r2, r5, lsr #8
	ldr	r4, [lr, r4, lsl #2]
	ldr	r0, [lr, r0, lsl #2]
	ldr	r11, [lr, r11, lsl #2]
	ldr	r1, [lr, r1, lsl #2]
	eor	r0, r0, r4, ror #24
	ldm	r3!, {r4, r5, r6, r7}
	eor	r11, r11, r1, ror #8
	eor	r11, r11, r0, ror #24
	#   XOR in Key Schedule
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	adr	r12, L_AES_ARM32_td4
	and	r7, r2, r10, lsr #8
	lsr	r1, r8, #24
	and	r4, r2, r9
	and	r0, r2, r11, lsr #16
	ldrb	r7, [r12, r7]
	ldrb	r1, [r12, r1]
	ldrb	r4, [r12, r4]
	ldrb	r0, [r12, r0]
	and	r5, r2, r10
	eor	r4, r4, r7, lsl #8
	and	r7, r2, r11, lsr #8
	eor	r4, r4, r0, lsl #16
	and	r0, r2, r8, lsr #16
	eor	r4, r4, r1, lsl #24
	lsr	r1, r9, #24
	ldrb	r7, [r12, r7]
	ldrb	r1, [r12, r1]
	ldrb	r5, [r12, r5]
	ldrb	r0, [r12, r0]
	and	r6, r2, r11
	eor	r5, r5, r7, lsl #8
	and	r7, r2, r8, lsr #8
	eor	r5, r5, r0, lsl #16
	and	r0, r2, r9, lsr #16
	eor	r5, r5, r1, lsl #24
	lsr	r1, r10, #24
	ldrb	r7, [r12, r7]
	ldrb	r1, [r12, r1]
	ldrb	r6, [r12, r6]
	ldrb	r0, [r12, r0]
	lsr	r11, r11, #24
	eor	r6, r6, r7, lsl #8
	and	r7, r2, r8
	eor	r6, r6, r0, lsl #16
	and	r0, r2, r9, lsr #8
	eor	r6, r6, r1, lsl #24
	and	r1, r2, r10, lsr #16
	ldrb	r11, [r12, r11]
	ldrb	r0, [r12, r0]
	ldrb	r7, [r12, r7]
	ldrb	r1, [r12, r1]
	eor	r0, r0, r11, lsl #16
	ldm	r3, {r8, r9, r10, r11}
	eor	r7, r7, r0, lsl #8
	eor	r7, r7, r1, lsl #16
	#   XOR in Key Schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	lr, [sp, #8]
	pop	{lr}
	bx	lr
	bx	lr
	.size	AES_decrypt_block,.-AES_decrypt_block
#if defined(WOLFSSL_AES_DIRECT) || defined(WOLFSSL_AES_COUNTER)
	.text
	.align	4
	.globl	AES_ECB_decrypt
	.type	AES_ECB_decrypt, %function
AES_ECB_decrypt:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	ldr	r12, [sp, #36]
	mov	r8, r12
	adr	r4, AES_ECB_decrypt
	mov	lr, #AES_ECB_decrypt-L_AES_ARM32_td
	sub	lr, r4, lr
	cmp	r8, #10
	beq	L_AES_ECB_decrypt_start_block_128
	cmp	r8, #12
	beq	L_AES_ECB_decrypt_start_block_192
L_AES_ECB_decrypt_loop_block_256:
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	push	{r0, r1, r2}
	mov	r2, #0xff
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_14
	pop	{r3, lr}
	pop	{r0, r1, r2}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_ECB_decrypt_loop_block_256
	b	L_AES_ECB_decrypt_end
L_AES_ECB_decrypt_start_block_192:
L_AES_ECB_decrypt_loop_block_192:
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	push	{r0, r1, r2}
	mov	r2, #0xff
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_12
	pop	{r3, lr}
	pop	{r0, r1, r2}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_ECB_decrypt_loop_block_192
	b	L_AES_ECB_decrypt_end
L_AES_ECB_decrypt_start_block_128:
L_AES_ECB_decrypt_loop_block_128:
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	push	{r0, r1, r2}
	mov	r2, #0xff
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_10
	pop	{r3, lr}
	pop	{r0, r1, r2}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_ECB_decrypt_loop_block_128
L_AES_ECB_decrypt_end:
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	AES_ECB_decrypt,.-AES_ECB_decrypt
#endif /* WOLFSSL_AES_DIRECT || WOLFSSL_AES_COUNTER */
#ifdef HAVE_AES_CBC
	.text
	.align	4
	.globl	AES_CBC_decrypt
	.type	AES_CBC_decrypt, %function
AES_CBC_decrypt:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	sub	sp, sp, #24
	ldr	r12, [sp, #60]
	ldr	lr, [sp, #64]
	str	lr, [sp, #20]
	str	r3, [sp]
	adr	r8, AES_CBC_decrypt
	mov	lr, #AES_CBC_decrypt-L_AES_ARM32_td
	sub	lr, r8, lr
	mov	r8, r12
	str	lr, [sp, #4]
	cmp	r8, #10
	beq	L_AES_CBC_decrypt_loop_block_128
	cmp	r8, #12
	beq	L_AES_CBC_decrypt_loop_block_192
L_AES_CBC_decrypt_loop_block_256:
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r0, [sp, #8]
	str	r1, [sp, #12]
#else
	strd	r0, r1, [sp, #8]
#endif
	str	r2, [sp, #16]
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #24]
	str	r7, [r0, #28]
#else
	strd	r6, r7, [r0, #24]
#endif
	mov	r2, #0xff
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_14
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r0, [sp, #20]
	ldm	r0, {r8, r9, r10, r11}
	ldr	r3, [sp]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
#else
	ldrd	r0, r1, [sp, #8]
#endif
	ldr	r2, [sp, #16]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	beq	L_AES_CBC_decrypt_end_odd
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r0, [sp, #8]
	str	r1, [sp, #12]
#else
	strd	r0, r1, [sp, #8]
#endif
	str	r2, [sp, #16]
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
	mov	r2, #0xff
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_14
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r0, #16]
	ldr	r9, [r0, #20]
#else
	ldrd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r0, #24]
	ldr	r11, [r0, #28]
#else
	ldrd	r10, r11, [r0, #24]
#endif
	ldr	r3, [sp]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
#else
	ldrd	r0, r1, [sp, #8]
#endif
	ldr	r2, [sp, #16]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CBC_decrypt_loop_block_256
	b	L_AES_CBC_decrypt_end
L_AES_CBC_decrypt_loop_block_192:
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r0, [sp, #8]
	str	r1, [sp, #12]
#else
	strd	r0, r1, [sp, #8]
#endif
	str	r2, [sp, #16]
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #24]
	str	r7, [r0, #28]
#else
	strd	r6, r7, [r0, #24]
#endif
	mov	r2, #0xff
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_12
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r0, [sp, #20]
	ldm	r0, {r8, r9, r10, r11}
	ldr	r3, [sp]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
#else
	ldrd	r0, r1, [sp, #8]
#endif
	ldr	r2, [sp, #16]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	beq	L_AES_CBC_decrypt_end_odd
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r0, [sp, #8]
	str	r1, [sp, #12]
#else
	strd	r0, r1, [sp, #8]
#endif
	str	r2, [sp, #16]
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
	mov	r2, #0xff
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_12
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r0, #16]
	ldr	r9, [r0, #20]
#else
	ldrd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r0, #24]
	ldr	r11, [r0, #28]
#else
	ldrd	r10, r11, [r0, #24]
#endif
	ldr	r3, [sp]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
#else
	ldrd	r0, r1, [sp, #8]
#endif
	ldr	r2, [sp, #16]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CBC_decrypt_loop_block_192
	b	L_AES_CBC_decrypt_end
L_AES_CBC_decrypt_loop_block_128:
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r0, [sp, #8]
	str	r1, [sp, #12]
#else
	strd	r0, r1, [sp, #8]
#endif
	str	r2, [sp, #16]
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0, #16]
	str	r5, [r0, #20]
#else
	strd	r4, r5, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #24]
	str	r7, [r0, #28]
#else
	strd	r6, r7, [r0, #24]
#endif
	mov	r2, #0xff
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_10
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r0, [sp, #20]
	ldm	r0, {r8, r9, r10, r11}
	ldr	r3, [sp]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
#else
	ldrd	r0, r1, [sp, #8]
#endif
	ldr	r2, [sp, #16]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	beq	L_AES_CBC_decrypt_end_odd
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r0, [sp, #8]
	str	r1, [sp, #12]
#else
	strd	r0, r1, [sp, #8]
#endif
	str	r2, [sp, #16]
	ldr	r4, [r0]
	ldr	r5, [r0, #4]
	ldr	r6, [r0, #8]
	ldr	r7, [r0, #12]
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r4, [r0]
	str	r5, [r0, #4]
#else
	strd	r4, r5, [r0]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r6, [r0, #8]
	str	r7, [r0, #12]
#else
	strd	r6, r7, [r0, #8]
#endif
	mov	r2, #0xff
	ldm	r3!, {r8, r9, r10, r11}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_decrypt_block_10
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r0, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [r0, #16]
	ldr	r9, [r0, #20]
#else
	ldrd	r8, r9, [r0, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [r0, #24]
	ldr	r11, [r0, #28]
#else
	ldrd	r10, r11, [r0, #24]
#endif
	ldr	r3, [sp]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r0, [sp, #8]
	ldr	r1, [sp, #12]
#else
	ldrd	r0, r1, [sp, #8]
#endif
	ldr	r2, [sp, #16]
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_CBC_decrypt_loop_block_128
	b	L_AES_CBC_decrypt_end
L_AES_CBC_decrypt_end_odd:
	ldr	lr, [sp, #20]
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r8, [lr, #16]
	ldr	r9, [lr, #20]
#else
	ldrd	r8, r9, [lr, #16]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	ldr	r10, [lr, #24]
	ldr	r11, [lr, #28]
#else
	ldrd	r10, r11, [lr, #24]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r8, [lr]
	str	r9, [lr, #4]
#else
	strd	r8, r9, [lr]
#endif
#if defined(WOLFSSL_SP_ARM_ARCH) && (WOLFSSL_SP_ARM_ARCH < 7)
	str	r10, [lr, #8]
	str	r11, [lr, #12]
#else
	strd	r10, r11, [lr, #8]
#endif
L_AES_CBC_decrypt_end:
	ldr	lr, [sp, #4]
	add	sp, sp, #24
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	AES_CBC_decrypt,.-AES_CBC_decrypt
#endif /* HAVE_AES_CBC */
#endif /* WOLFSSL_AES_DIRECT || WOLFSSL_AES_COUNTER || HAVE_AES_CBC */
#endif /* HAVE_AES_DECRYPT */
#ifdef HAVE_AESGCM
	.text
	.type	L_GCM_gmult_len_r, %object
	.size	L_GCM_gmult_len_r, 64
	.align	4
L_GCM_gmult_len_r:
	.word	0x0
	.word	0x1c200000
	.word	0x38400000
	.word	0x24600000
	.word	0x70800000
	.word	0x6ca00000
	.word	0x48c00000
	.word	0x54e00000
	.word	0xe1000000
	.word	0xfd200000
	.word	0xd9400000
	.word	0xc5600000
	.word	0x91800000
	.word	0x8da00000
	.word	0xa9c00000
	.word	0xb5e00000
	.text
	.align	4
	.globl	GCM_gmult_len
	.type	GCM_gmult_len, %function
GCM_gmult_len:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	adr	lr, L_GCM_gmult_len_r
L_GCM_gmult_len_start_block:
	push	{r3}
	ldr	r12, [r0, #12]
	ldr	r3, [r2, #12]
	eor	r12, r12, r3
	lsr	r3, r12, #24
	and	r3, r3, #15
	add	r3, r1, r3, lsl #4
	ldm	r3, {r8, r9, r10, r11}
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #28
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #16
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #20
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #8
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #12
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	and	r4, r12, #15
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #4
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	ldr	r12, [r0, #8]
	ldr	r3, [r2, #8]
	eor	r12, r12, r3
	lsr	r3, r12, #24
	and	r3, r3, #15
	add	r3, r1, r3, lsl #4
	ldm	r3, {r4, r5, r6, r7}
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #28
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #16
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #20
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #8
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #12
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	and	r4, r12, #15
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #4
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	ldr	r12, [r0, #4]
	ldr	r3, [r2, #4]
	eor	r12, r12, r3
	lsr	r3, r12, #24
	and	r3, r3, #15
	add	r3, r1, r3, lsl #4
	ldm	r3, {r4, r5, r6, r7}
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #28
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #16
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #20
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #8
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #12
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	and	r4, r12, #15
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #4
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	ldr	r12, [r0]
	ldr	r3, [r2]
	eor	r12, r12, r3
	lsr	r3, r12, #24
	and	r3, r3, #15
	add	r3, r1, r3, lsl #4
	ldm	r3, {r4, r5, r6, r7}
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #28
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #16
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #20
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #8
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #12
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	and	r4, r12, #15
	eor	r11, r11, r10, lsl #28
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	lsr	r6, r10, #4
	and	r3, r11, #15
	lsr	r11, r11, #4
	lsr	r4, r12, #4
	eor	r11, r11, r10, lsl #28
	and	r4, r4, #15
	ldr	r3, [lr, r3, lsl #2]
	add	r4, r1, r4, lsl #4
	eor	r10, r6, r9, lsl #28
	lsr	r9, r9, #4
	ldm	r4, {r4, r5, r6, r7}
	eor	r9, r9, r8, lsl #28
	eor	r8, r3, r8, lsr #4
	eor	r8, r8, r4
	eor	r9, r9, r5
	eor	r10, r10, r6
	eor	r11, r11, r7
	rev	r8, r8
	rev	r9, r9
	rev	r10, r10
	rev	r11, r11
	stm	r0, {r8, r9, r10, r11}
	pop	{r3}
	subs	r3, r3, #16
	add	r2, r2, #16
	bne	L_GCM_gmult_len_start_block
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	GCM_gmult_len,.-GCM_gmult_len
	.text
	.align	4
	.globl	AES_GCM_encrypt
	.type	AES_GCM_encrypt, %function
AES_GCM_encrypt:
	push	{r4, r5, r6, r7, r8, r9, r10, r11, lr}
	ldr	r12, [sp, #36]
	ldr	lr, [sp, #40]
	ldm	lr, {r4, r5, r6, r7}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	stm	lr, {r4, r5, r6, r7}
	push	{lr}
	adr	r8, AES_GCM_encrypt
	mov	lr, #AES_GCM_encrypt-L_AES_ARM32_te
	sub	lr, r8, lr
	cmp	r12, #10
	beq	L_AES_GCM_encrypt_start_block_128
	cmp	r12, #12
	beq	L_AES_GCM_encrypt_start_block_192
	mov	r12, #0xff
L_AES_GCM_encrypt_loop_block_256:
	push	{r0, r1}
	ldr	r0, [sp, #8]
	add	r7, r7, #1
	str	r7, [r0, #12]
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_14
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r8
	eor	r5, r9
	eor	r6, r10
	eor	r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	ldr	r8, [sp]
	ldm	r8, {r4, r5, r6, r7}
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_GCM_encrypt_loop_block_256
	b	L_AES_GCM_encrypt_end
L_AES_GCM_encrypt_start_block_192:
	mov	r12, #0xff
L_AES_GCM_encrypt_loop_block_192:
	push	{r0, r1}
	ldr	r0, [sp, #8]
	add	r7, r7, #1
	str	r7, [r0, #12]
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_12
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r8
	eor	r5, r9
	eor	r6, r10
	eor	r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	ldr	r8, [sp]
	ldm	r8, {r4, r5, r6, r7}
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_GCM_encrypt_loop_block_192
	b	L_AES_GCM_encrypt_end
L_AES_GCM_encrypt_start_block_128:
	mov	r12, #0xff
L_AES_GCM_encrypt_loop_block_128:
	push	{r0, r1}
	ldr	r0, [sp, #8]
	add	r7, r7, #1
	str	r7, [r0, #12]
	push	{r3, lr}
	ldm	r3!, {r8, r9, r10, r11}
	# Round: 0 - XOR in key schedule
	eor	r4, r4, r8
	eor	r5, r5, r9
	eor	r6, r6, r10
	eor	r7, r7, r11
	bl	L_AES_encrypt_block_10
	pop	{r3, lr}
	pop	{r0, r1}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	ldr	r8, [r0]
	ldr	r9, [r0, #4]
	ldr	r10, [r0, #8]
	ldr	r11, [r0, #12]
	eor	r4, r8
	eor	r5, r9
	eor	r6, r10
	eor	r7, r11
	str	r4, [r1]
	str	r5, [r1, #4]
	str	r6, [r1, #8]
	str	r7, [r1, #12]
	ldr	r8, [sp]
	ldm	r8, {r4, r5, r6, r7}
	subs	r2, r2, #16
	add	r0, r0, #16
	add	r1, r1, #16
	bne	L_AES_GCM_encrypt_loop_block_128
L_AES_GCM_encrypt_end:
	pop	{lr}
	rev	r4, r4
	rev	r5, r5
	rev	r6, r6
	rev	r7, r7
	stm	lr, {r4, r5, r6, r7}
	pop	{r4, r5, r6, r7, r8, r9, r10, r11, pc}
	.size	AES_GCM_encrypt,.-AES_GCM_encrypt
#endif /* HAVE_AESGCM */
#endif /* !NO_AES */
#endif /* !__aarch64__ */
#endif /* WOLFSSL_ARMASM */

#if defined(__linux__) && defined(__ELF__)
.section	.note.GNU-stack,"",%progbits
#endif
