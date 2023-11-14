/*
   BLAKE2 reference source code package - reference C implementations

   Written in 2012 by Samuel Neves <sneves@dei.uc.pt>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/
/* blake2-int.h
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




#ifndef WOLFCRYPT_BLAKE2_INT_H
#define WOLFCRYPT_BLAKE2_INT_H

#include <wolfssl/wolfcrypt/types.h>

#if defined(__cplusplus)
    extern "C" {
#endif

  enum blake2s_constant
  {
    BLAKE2S_BLOCKBYTES = 64,
    BLAKE2S_OUTBYTES   = 32,
    BLAKE2S_KEYBYTES   = 32,
    BLAKE2S_SALTBYTES  = 8,
    BLAKE2S_PERSONALBYTES = 8
  };

  enum blake2b_constant
  {
    BLAKE2B_BLOCKBYTES = 128,
    BLAKE2B_OUTBYTES   = 64,
    BLAKE2B_KEYBYTES   = 64,
    BLAKE2B_SALTBYTES  = 16,
    BLAKE2B_PERSONALBYTES = 16
  };

#pragma pack(push, 1)
  typedef struct __blake2s_param
  {
    byte  digest_length; /* 1 */
    byte  key_length;    /* 2 */
    byte  fanout;        /* 3 */
    byte  depth;         /* 4 */
    word32 leaf_length;   /* 8 */
    byte  node_offset[6];/* 14 */
    byte  node_depth;    /* 15 */
    byte  inner_length;  /* 16 */
    /* byte  reserved[0]; */
    byte  salt[BLAKE2S_SALTBYTES]; /* 24 */
    byte  personal[BLAKE2S_PERSONALBYTES];  /* 32 */
  } blake2s_param;

  typedef struct __blake2s_state
  {
    word32 h[8];
    word32 t[2];
    word32 f[2];
    byte  buf[2 * BLAKE2S_BLOCKBYTES];
    word32 buflen;
    byte  last_node;
  } blake2s_state ;

  typedef struct __blake2b_param
  {
    byte  digest_length; /* 1 */
    byte  key_length;    /* 2 */
    byte  fanout;        /* 3 */
    byte  depth;         /* 4 */
    word32 leaf_length;   /* 8 */
    word64 node_offset;   /* 16 */
    byte  node_depth;    /* 17 */
    byte  inner_length;  /* 18 */
    byte  reserved[14];  /* 32 */
    byte  salt[BLAKE2B_SALTBYTES]; /* 48 */
    byte  personal[BLAKE2B_PERSONALBYTES];  /* 64 */
  } blake2b_param;

  typedef struct __blake2b_state
  {
    word64 h[8];
    word64 t[2];
    word64 f[2];
    byte  buf[2 * BLAKE2B_BLOCKBYTES];
    word64 buflen;
    byte  last_node;
  } blake2b_state;

  typedef struct __blake2sp_state
  {
    blake2s_state S[8][1];
    blake2s_state R[1];
    byte buf[8 * BLAKE2S_BLOCKBYTES];
    word32 buflen;
  } blake2sp_state;

  typedef struct __blake2bp_state
  {
    blake2b_state S[4][1];
    blake2b_state R[1];
    byte buf[4 * BLAKE2B_BLOCKBYTES];
    word64 buflen;
  } blake2bp_state;
#pragma pack(pop)

  /* Streaming API */
  int blake2s_init( blake2s_state *S, byte outlen );
  int blake2s_init_key( blake2s_state *S, byte outlen, const void *key, byte keylen );
  int blake2s_init_param( blake2s_state *S, const blake2s_param *P );
  int blake2s_update( blake2s_state *S, const byte *in, word32 inlen );
  int blake2s_final( blake2s_state *S, byte *out, byte outlen );

  int blake2b_init( blake2b_state *S, byte outlen );
  int blake2b_init_key( blake2b_state *S, byte outlen, const void *key, byte keylen );
  int blake2b_init_param( blake2b_state *S, const blake2b_param *P );
  int blake2b_update( blake2b_state *S, const byte *in, word64 inlen );
  int blake2b_final( blake2b_state *S, byte *out, byte outlen );

  int blake2sp_init( blake2sp_state *S, byte outlen );
  int blake2sp_init_key( blake2sp_state *S, byte outlen, const void *key, byte keylen );
  int blake2sp_update( blake2sp_state *S, const byte *in, word32 inlen );
  int blake2sp_final( blake2sp_state *S, byte *out, byte outlen );

  int blake2bp_init( blake2bp_state *S, byte outlen );
  int blake2bp_init_key( blake2bp_state *S, byte outlen, const void *key, byte keylen );
  int blake2bp_update( blake2bp_state *S, const byte *in, word64 inlen );
  int blake2bp_final( blake2bp_state *S, byte *out, byte outlen );

  /* Simple API */
  int blake2s( byte *out, const void *in, const void *key, byte outlen, word32 inlen, byte keylen );
  int blake2b( byte *out, const void *in, const void *key, byte outlen, word64 inlen, byte keylen );

  int blake2sp( byte *out, const void *in, const void *key, byte outlen, word32 inlen, byte keylen );
  int blake2bp( byte *out, const void *in, const void *key, byte outlen, word64 inlen, byte keylen );

  static WC_INLINE int blake2( byte *out, const void *in, const void *key, byte outlen, word64 inlen, byte keylen )
  {
    return blake2b( out, in, key, outlen, inlen, keylen );
  }



#if defined(__cplusplus)
    }
#endif

#endif  /* WOLFCRYPT_BLAKE2_INT_H */

