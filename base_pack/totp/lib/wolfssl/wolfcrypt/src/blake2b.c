/*
   BLAKE2 reference source code package - reference C implementations

   Written in 2012 by Samuel Neves <sneves@dei.uc.pt>

   To the extent possible under law, the author(s) have dedicated all copyright
   and related and neighboring rights to this software to the public domain
   worldwide. This software is distributed without any warranty.

   You should have received a copy of the CC0 Public Domain Dedication along with
   this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
*/
/* blake2b.c
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

#ifdef HAVE_BLAKE2

#include <wolfssl/wolfcrypt/blake2.h>
#include <wolfssl/wolfcrypt/blake2-impl.h>
#include <wolfssl/wolfcrypt/error-crypt.h>


static const word64 blake2b_IV[8] =
{
  W64LIT(0x6a09e667f3bcc908), W64LIT(0xbb67ae8584caa73b),
  W64LIT(0x3c6ef372fe94f82b), W64LIT(0xa54ff53a5f1d36f1),
  W64LIT(0x510e527fade682d1), W64LIT(0x9b05688c2b3e6c1f),
  W64LIT(0x1f83d9abfb41bd6b), W64LIT(0x5be0cd19137e2179)
};

static const byte blake2b_sigma[12][16] =
{
  {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 } ,
  { 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 } ,
  { 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 } ,
  {  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 } ,
  {  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 } ,
  {  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 } ,
  { 12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11 } ,
  { 13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10 } ,
  {  6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5 } ,
  { 10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13 , 0 } ,
  {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 } ,
  { 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 }
};


static WC_INLINE int blake2b_set_lastnode( blake2b_state *S )
{
  S->f[1] = ~W64LIT(0);
  return 0;
}

/* Some helper functions, not necessarily useful */
static WC_INLINE int blake2b_set_lastblock( blake2b_state *S )
{
  if( S->last_node ) blake2b_set_lastnode( S );

  S->f[0] = ~W64LIT(0);
  return 0;
}

static WC_INLINE int blake2b_increment_counter( blake2b_state *S, const word64
                                             inc )
{
  S->t[0] += inc;
  S->t[1] += ( S->t[0] < inc );
  return 0;
}

static WC_INLINE int blake2b_init0( blake2b_state *S )
{
  int i;
  XMEMSET( S, 0, sizeof( blake2b_state ) );

  for( i = 0; i < 8; ++i ) S->h[i] = blake2b_IV[i];

  return 0;
}

/* init xors IV with input parameter block */
int blake2b_init_param( blake2b_state *S, const blake2b_param *P )
{
  word32 i;
  byte *p ;
  blake2b_init0( S );
  p =  ( byte * )( P );

  /* IV XOR ParamBlock */
  for( i = 0; i < 8; ++i )
    S->h[i] ^= load64( p + sizeof( S->h[i] ) * i );

  return 0;
}


int blake2b_init( blake2b_state *S, const byte outlen )
{
#ifdef WOLFSSL_BLAKE2B_INIT_EACH_FIELD
  blake2b_param P[1];
#else
  volatile blake2b_param P[1];
#endif

  if ( ( !outlen ) || ( outlen > BLAKE2B_OUTBYTES ) ) return BAD_FUNC_ARG;

#ifdef WOLFSSL_BLAKE2B_INIT_EACH_FIELD
  P->digest_length = outlen;
  P->key_length    = 0;
  P->fanout        = 1;
  P->depth         = 1;
  store32( &P->leaf_length, 0 );
  store64( &P->node_offset, 0 );
  P->node_depth    = 0;
  P->inner_length  = 0;
  XMEMSET( P->reserved, 0, sizeof( P->reserved ) );
  XMEMSET( P->salt,     0, sizeof( P->salt ) );
  XMEMSET( P->personal, 0, sizeof( P->personal ) );
#else
  XMEMSET( (blake2b_param *)P, 0, sizeof( *P ) );
  P->digest_length = outlen;
  P->fanout        = 1;
  P->depth         = 1;
#endif
  return blake2b_init_param( S, (blake2b_param *)P );
}


int blake2b_init_key( blake2b_state *S, const byte outlen, const void *key,
                      const byte keylen )
{
  int ret = 0;
#ifdef WOLFSSL_BLAKE2B_INIT_EACH_FIELD
  blake2b_param P[1];
#else
  volatile blake2b_param P[1];
#endif

  if ( ( !outlen ) || ( outlen > BLAKE2B_OUTBYTES ) ) return BAD_FUNC_ARG;

  if ( !key || !keylen || keylen > BLAKE2B_KEYBYTES ) return BAD_FUNC_ARG;

#ifdef WOLFSSL_BLAKE2B_INIT_EACH_FIELD
  P->digest_length = outlen;
  P->key_length    = keylen;
  P->fanout        = 1;
  P->depth         = 1;
  store32( &P->leaf_length, 0 );
  store64( &P->node_offset, 0 );
  P->node_depth    = 0;
  P->inner_length  = 0;
  XMEMSET( P->reserved, 0, sizeof( P->reserved ) );
  XMEMSET( P->salt,     0, sizeof( P->salt ) );
  XMEMSET( P->personal, 0, sizeof( P->personal ) );
#else
  XMEMSET( (blake2b_param *)P, 0, sizeof( *P ) );
  P->digest_length = outlen;
  P->key_length    = keylen;
  P->fanout        = 1;
  P->depth         = 1;
#endif

  ret = blake2b_init_param( S, (blake2b_param *)P );
  if ( ret < 0 ) return ret;

  {
#ifdef WOLFSSL_SMALL_STACK
    byte* block;

    block = (byte*)XMALLOC(BLAKE2B_BLOCKBYTES, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if ( block == NULL ) return MEMORY_E;
#else
    byte block[BLAKE2B_BLOCKBYTES];
#endif

    XMEMSET( block, 0, BLAKE2B_BLOCKBYTES );
    XMEMCPY( block, key, keylen );
    ret = blake2b_update( S, block, BLAKE2B_BLOCKBYTES );
    secure_zero_memory( block, BLAKE2B_BLOCKBYTES ); /* Burn the key from */
                                                     /* memory */

#ifdef WOLFSSL_SMALL_STACK
    XFREE(block, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
  }
  return ret;
}

static WC_INLINE int blake2b_compress(
    blake2b_state *S,
    const byte block[BLAKE2B_BLOCKBYTES],
    word64* m,
    word64* v)
{
  word64 i;

  for( i = 0; i < 16; ++i )
    m[i] = load64( block + i * sizeof( m[i] ) );

  for( i = 0; i < 8; ++i )
    v[i] = S->h[i];

  v[ 8] = blake2b_IV[0];
  v[ 9] = blake2b_IV[1];
  v[10] = blake2b_IV[2];
  v[11] = blake2b_IV[3];
  v[12] = S->t[0] ^ blake2b_IV[4];
  v[13] = S->t[1] ^ blake2b_IV[5];
  v[14] = S->f[0] ^ blake2b_IV[6];
  v[15] = S->f[1] ^ blake2b_IV[7];
#define G(r,i,a,b,c,d) \
  do { \
      (a) = (a) + (b) + m[blake2b_sigma[r][2*(i)+0]];   \
      (d) = rotr64((d) ^ (a), 32);                      \
      (c) = (c) + (d);                                  \
      (b) = rotr64((b) ^ (c), 24);                      \
      (a) = (a) + (b) + m[blake2b_sigma[r][2*(i)+1]];   \
      (d) = rotr64((d) ^ (a), 16);                      \
      (c) = (c) + (d);                                  \
      (b) = rotr64((b) ^ (c), 63);                      \
  } while(0)
#define ROUND(r)  \
  do { \
    G(r,0,v[ 0],v[ 4],v[ 8],v[12]); \
    G(r,1,v[ 1],v[ 5],v[ 9],v[13]); \
    G(r,2,v[ 2],v[ 6],v[10],v[14]); \
    G(r,3,v[ 3],v[ 7],v[11],v[15]); \
    G(r,4,v[ 0],v[ 5],v[10],v[15]); \
    G(r,5,v[ 1],v[ 6],v[11],v[12]); \
    G(r,6,v[ 2],v[ 7],v[ 8],v[13]); \
    G(r,7,v[ 3],v[ 4],v[ 9],v[14]); \
  } while(0)
  ROUND( 0 );
  ROUND( 1 );
  ROUND( 2 );
  ROUND( 3 );
  ROUND( 4 );
  ROUND( 5 );
  ROUND( 6 );
  ROUND( 7 );
  ROUND( 8 );
  ROUND( 9 );
  ROUND( 10 );
  ROUND( 11 );

  for( i = 0; i < 8; ++i )
    S->h[i] = S->h[i] ^ v[i] ^ v[i + 8];

#undef G
#undef ROUND

  return 0;
}

/* inlen now in bytes */
int blake2b_update( blake2b_state *S, const byte *in, word64 inlen )
{
  int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
  word64* m;
  word64* v;

  m = (word64*)XMALLOC(sizeof(word64) * 32, NULL, DYNAMIC_TYPE_TMP_BUFFER);

  if ( m == NULL ) return MEMORY_E;

  v = &m[16];
#else
  word64 m[16];
  word64 v[16];
#endif

  while( inlen > 0 )
  {
    word64 left = S->buflen;
    word64 fill = 2 * BLAKE2B_BLOCKBYTES - left;

    if( inlen > fill )
    {
      XMEMCPY( S->buf + left, in, (wolfssl_word)fill ); /* Fill buffer */
      S->buflen += fill;
      blake2b_increment_counter( S, BLAKE2B_BLOCKBYTES );

      {
          ret = blake2b_compress( S, S->buf, m, v );
          if (ret < 0) break;
      }

      XMEMCPY( S->buf, S->buf + BLAKE2B_BLOCKBYTES, BLAKE2B_BLOCKBYTES );
              /* Shift buffer left */
      S->buflen -= BLAKE2B_BLOCKBYTES;
      in += fill;
      inlen -= fill;
    }
    else /* inlen <= fill */
    {
      XMEMCPY( S->buf + left, in, (wolfssl_word)inlen );
      S->buflen += inlen; /* Be lazy, do not compress */
      inlen = 0;
    }
  }

#ifdef WOLFSSL_SMALL_STACK
  XFREE(m, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

  return ret;
}

/* Is this correct? */
int blake2b_final( blake2b_state *S, byte *out, byte outlen )
{
  int ret = 0;
  byte buffer[BLAKE2B_OUTBYTES];
  word64    i;
#ifdef WOLFSSL_SMALL_STACK
  word64* m;
  word64* v;

  m = (word64*)XMALLOC(sizeof(word64) * 32, NULL, DYNAMIC_TYPE_TMP_BUFFER);

  if ( m == NULL ) return MEMORY_E;

  v = &m[16];
#else
  word64 m[16];
  word64 v[16];
#endif

  if( S->buflen > BLAKE2B_BLOCKBYTES )
  {
    blake2b_increment_counter( S, BLAKE2B_BLOCKBYTES );

    {
      ret = blake2b_compress( S, S->buf, m, v );
      if (ret < 0) goto out;
    }

    S->buflen -= BLAKE2B_BLOCKBYTES;
    XMEMCPY( S->buf, S->buf + BLAKE2B_BLOCKBYTES, (wolfssl_word)S->buflen );
  }

  blake2b_increment_counter( S, S->buflen );
  blake2b_set_lastblock( S );
  XMEMSET( S->buf + S->buflen, 0, (wolfssl_word)(2 * BLAKE2B_BLOCKBYTES - S->buflen) );
         /* Padding */
  {
    ret = blake2b_compress( S, S->buf, m, v );
    if (ret < 0) goto out;
  }

  for( i = 0; i < 8; ++i ) /* Output full hash to temp buffer */
    store64( buffer + sizeof( S->h[i] ) * i, S->h[i] );

  XMEMCPY( out, buffer, outlen );

 out:

#ifdef WOLFSSL_SMALL_STACK
  XFREE(m, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

  return ret;
}

/* inlen, at least, should be word64. Others can be size_t. */
int blake2b( byte *out, const void *in, const void *key, const byte outlen,
             const word64 inlen, byte keylen )
{
  blake2b_state S[1];

  /* Verify parameters */
  if ( NULL == in ) return BAD_FUNC_ARG;

  if ( NULL == out ) return BAD_FUNC_ARG;

  if( NULL == key ) keylen = 0;

  if( keylen > 0 )
  {
    int ret = blake2b_init_key( S, outlen, key, keylen );
    if (ret < 0) return ret;
  }
  else
  {
    int ret = blake2b_init( S, outlen );
    if (ret < 0) return ret;
  }

  {
    int ret = blake2b_update( S, ( byte * )in, inlen );
    if (ret < 0) return ret;
  }

  return blake2b_final( S, out, outlen );
}

#if defined(BLAKE2B_SELFTEST)
#include <string.h>
#include "blake2-kat.h"
int main( int argc, char **argv )
{
  byte key[BLAKE2B_KEYBYTES];
  byte buf[KAT_LENGTH];

  for( word32 i = 0; i < BLAKE2B_KEYBYTES; ++i )
    key[i] = ( byte )i;

  for( word32 i = 0; i < KAT_LENGTH; ++i )
    buf[i] = ( byte )i;

  for( word32 i = 0; i < KAT_LENGTH; ++i )
  {
    byte hash[BLAKE2B_OUTBYTES];
    if ( blake2b( hash, buf, key, BLAKE2B_OUTBYTES, i, BLAKE2B_KEYBYTES ) < 0 )
    {
      puts( "error" );
      return -1;
    }

    if( 0 != XMEMCMP( hash, blake2b_keyed_kat[i], BLAKE2B_OUTBYTES ) )
    {
      puts( "error" );
      return -1;
    }
  }

  puts( "ok" );
  return 0;
}
#endif


/* wolfCrypt API */

/* Init Blake2b digest, track size in case final doesn't want to "remember" */
int wc_InitBlake2b(Blake2b* b2b, word32 digestSz)
{
    if (b2b == NULL){
        return BAD_FUNC_ARG;
    }
    b2b->digestSz = digestSz;

    return blake2b_init(b2b->S, (byte)digestSz);
}

/* Init Blake2b digest with key, track size in case final doesn't want to "remember" */
int wc_InitBlake2b_WithKey(Blake2b* b2b, word32 digestSz, const byte *key, word32 keylen)
{
    if (b2b == NULL){
        return BAD_FUNC_ARG;
    }
    b2b->digestSz = digestSz;

    if (keylen >= 256)
        return BAD_FUNC_ARG;

    if (key)
        return blake2b_init_key(b2b->S, (byte)digestSz, key, (byte)keylen);
    else
        return blake2b_init(b2b->S, (byte)digestSz);
}

/* Blake2b Update */
int wc_Blake2bUpdate(Blake2b* b2b, const byte* data, word32 sz)
{
    return blake2b_update(b2b->S, data, sz);
}


/* Blake2b Final, if pass in zero size we use init digestSz */
int wc_Blake2bFinal(Blake2b* b2b, byte* final, word32 requestSz)
{
    word32 sz = requestSz ? requestSz : b2b->digestSz;

    return blake2b_final(b2b->S, final, (byte)sz);
}


/* end CTaoCrypt API */

#endif  /* HAVE_BLAKE2 */

