/* iotsafe.h
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

/* IoT-safe module for communication with IoT-safe applet on SIM card */

#ifndef WOLFSSL_IOTSAFE_H
#define WOLFSSL_IOTSAFE_H

#include <wolfssl/wolfcrypt/settings.h>

#ifdef WOLFSSL_IOTSAFE
#include <wolfssl/ssl.h>

WOLFSSL_API int wolfSSL_CTX_iotsafe_enable(WOLFSSL_CTX *ctx);

WOLFSSL_API int wolfSSL_iotsafe_on(WOLFSSL *ssl, byte privkey_id,
       byte ecdh_keypair_slot, byte peer_pubkey_slot, byte peer_cert_slot);

WOLFSSL_API int wolfSSL_iotsafe_on_ex(WOLFSSL *ssl, byte *privkey_id,
       byte *ecdh_keypair_slot, byte *peer_pubkey_slot, byte *peer_cert_slot, word16 id_size);


typedef int  (*wolfSSL_IOTSafe_CSIM_write_cb)(const char*, int);
typedef int  (*wolfSSL_IOTSafe_CSIM_read_cb)(char *, int);

WOLFSSL_API void wolfIoTSafe_SetCSIM_read_cb(wolfSSL_IOTSafe_CSIM_read_cb rf);
WOLFSSL_API void wolfIoTSafe_SetCSIM_write_cb(wolfSSL_IOTSafe_CSIM_write_cb wf);

WOLFSSL_API int wolfIoTSafe_GetRandom(unsigned char* out, word32 sz);
WOLFSSL_API int wolfIoTSafe_GetCert(uint8_t id, unsigned char *output, unsigned long sz);
WOLFSSL_API int wolfIoTSafe_GetCert_ex(uint8_t *id, uint16_t id_sz, unsigned char *output, unsigned long sz);

#ifdef HAVE_ECC
#include <wolfssl/wolfcrypt/ecc.h>
#define IOTSAFE_ECC_KTYPE ECC_SECP256R1
#define IOTSAFE_ECC_KSIZE 32
WOLFSSL_API int wc_iotsafe_ecc_import_public(ecc_key *key, byte key_id);
WOLFSSL_API int wc_iotsafe_ecc_export_public(ecc_key *key, byte key_id);
WOLFSSL_API int wc_iotsafe_ecc_export_private(ecc_key *key, byte key_id);
WOLFSSL_API int wc_iotsafe_ecc_sign_hash(byte *in, word32 inlen, byte *out, word32 *outlen, byte key_id);
WOLFSSL_API int wc_iotsafe_ecc_verify_hash(byte *sig, word32 siglen, byte *hash, word32 hashlen, int *res, byte key_id);
WOLFSSL_API int wc_iotsafe_ecc_gen_k(byte key_id);

WOLFSSL_API int wc_iotsafe_ecc_import_public_ex(ecc_key *key, byte *key_id, word16 id_size);
WOLFSSL_API int wc_iotsafe_ecc_export_public_ex(ecc_key *key, byte *key_id, word16 id_size);
WOLFSSL_API int wc_iotsafe_ecc_export_private_ex(ecc_key *key, byte *key_id, word16 id_size);
WOLFSSL_API int wc_iotsafe_ecc_sign_hash_ex(byte *in, word32 inlen, byte *out, word32 *outlen, byte *key_id, word16 id_size);
WOLFSSL_API int wc_iotsafe_ecc_verify_hash_ex(byte *sig, word32 siglen, byte *hash, word32 hashlen, int *res, byte *key_id, word16 id_size);
WOLFSSL_API int wc_iotsafe_ecc_gen_k_ex(byte *key_id, word16 id_size);
#endif


#ifdef HAVE_IOTSAFE_HWRNG
    #ifndef HAVE_HASHDRBG
        #define CUSTOM_RAND_GENERATE_BLOCK wolfIoTSafe_GetRandom
    #else
        #define CUSTOM_RAND_GENERATE_SEED wolfIoTSafe_GetRandom
    #endif
#endif

#ifndef IOTSAFE_ID_SIZE
#   define IOTSAFE_ID_SIZE 1
#endif

struct wc_IOTSAFE {
    int enabled;

#if (IOTSAFE_ID_SIZE == 1)
    byte privkey_id;
    byte ecdh_keypair_slot;
    byte peer_pubkey_slot;
    byte peer_cert_slot;
#elif (IOTSAFE_ID_SIZE == 2)
    word16 privkey_id;
    word16 ecdh_keypair_slot;
    word16 peer_pubkey_slot;
    word16 peer_cert_slot;
#else
#error "IOTSAFE: ID_SIZE not supported"
#endif
};
typedef struct wc_IOTSAFE IOTSAFE;

/* Default IOT-Safe Class */
#define IOTSAFE_CLASS 0x81

/* Command codes */
#define IOTSAFE_INS_PUT_PUBLIC_INIT    0x24
#define IOTSAFE_INS_PUT_PUBLIC_UPDATE  0xD8
#define IOTSAFE_INS_SIGN_INIT     0x2A
#define IOTSAFE_INS_SIGN_UPDATE   0x2B
#define IOTSAFE_INS_VERIFY_INIT   0x2C
#define IOTSAFE_INS_VERIFY_UPDATE 0x2D
#define IOTSAFE_INS_COMPUTE_DH    0x46
#define IOTSAFE_INS_GETRANDOM     0x84
#define IOTSAFE_INS_READ_FILE     0xB0
#define IOTSAFE_INS_GEN_KEYPAIR   0xB9
#define IOTSAFE_INS_GETRESPONSE   0xC0
#define IOTSAFE_INS_GETDATA       0xCB
#define IOTSAFE_INS_READ_KEY      0xCD
#define IOTSAFE_INS_HKDF_EXTRACT 0x4A

/* Tags */
#define IOTSAFE_TAG_ECC_KEY_FIELD       0x34
#define IOTSAFE_TAG_ECC_KEY_FIELD_SZ    0x45
#define IOTSAFE_TAG_ECC_KEY_TYPE        0x49
#define IOTSAFE_TAG_ECC_KEY_TYPE_SZ     0x43
#define IOTSAFE_TAG_ECC_KEY_XY          0x86
#define IOTSAFE_TAG_ECC_KEY_XY_SZ       0x41
#define IOTSAFE_TAG_ECC_KEY_XY_HDR_BYTE 0x04

#define IOTSAFE_TAG_HASH_FIELD          0x9E
#define IOTSAFE_TAG_SIGNATURE_FIELD     0x33
#define IOTSAFE_TAG_FILE_ID             0x83
#define IOTSAFE_TAG_PRIVKEY_ID          0x84
#define IOTSAFE_TAG_PUBKEY_ID           0x85
#define IOTSAFE_TAG_HASH_ALGO           0x91
#define IOTSAFE_TAG_SIGN_ALGO           0x92
#define IOTSAFE_TAG_MODE_OF_OPERATION   0xA1
#define IOTSAFE_TAG_SECRET              0xD1
#define IOTSAFE_TAG_SALT                0xD5

/* Flags - data */
#define IOTSAFE_GETDATA_FILE      0xC3
#define IOTSAFE_DATA_LAST         0x80
#define IOTSAFE_CMDSIZE_MAX       512

/* IoT-safe tables of constants */

/* 2.5.1 Algorithms for hash */
#define IOTSAFE_HASH_SHA256 0x0001
#define IOTSAFE_HASH_SHA384 0x0002
#define IOTSAFE_HASH_SHA512 0x0004

/* 2.5.2 Algorithms for signature */
#define IOTSAFE_SIGN_RSA_PKCS15 0x01
#define IOTSAFE_SIGN_RSA_PSS    0x02
#define IOTSAFE_SIGN_ECDSA      0x04

/* 2.5.3 Algorithms for key agreement */
#define IOTSAFE_KA_ECKA 0x01

/* 2.5.4 Algorithms for key derivation */
#define IOTSAFE_KD_PRF_SHA256 0x01
#define IOTSAFE_KD_HKDF 0x02

/* 2.5.14 Mode of Operation for signature commands */
#define IOTSAFE_MOO_HASH_FULLTEXT  0x01
#define IOTSAFE_MOO_HASH_LASTBLOCK 0x02
#define IOTSAFE_MOO_SIGN_ONLY      0x03


/* IoT-safe Public key format */
#define IOTSAFE_TAG_PKEY_ID     0x85
#define IOTSAFE_TAG_PKEY_ACCESS 0x60
    #define IOTSAFE_ACCESS_READ   (1 << 0)
    #define IOTSAFE_ACCESS_UPDATE (1 << 1)

#define IOTSAFE_TAG_PKEY_OBJSTATE 0x4A /* 1 == active */

#define IOTSAFE_TAG_PKEY_TYPE   0x4B
    #define IOTSAFE_KEYTYPE_RSA2048 0x03
    #define IOTSAFE_KEYTYPE_SECP256R1_PERSISTENT 0x13
    #define IOTSAFE_KEYTYPE_SECP256R1_VOLATILE 0x14
    #define IOTSAFE_KEYTYPE_BRAINPOOL_PERSISTENT 0x23
    #define IOTSAFE_KEYTYPE_BRAINPOOL_VOLATILE 0x24
    #define IOTSAFE_KEYTYPE_HMAC_CAPABLE    0xA0

#define IOTSAFE_TAG_PKEY_USAGE  0x4E
    #define IOTSAFE_KEYUSAGE_GENERAL 0x01
    #define IOTSAFE_KEYUSAGE_CERT_VERIFY_TLS12 0x02
    #define IOTSAFE_KEYUSAGE_CERT_VERIFY_TLS13 0x03

#define IOTSAFE_TAG_PKEY_CRYPTO 0x61
    #define IOTSAFE_CRYPTO_SIGN_VERIFY     (1 << 0)
    #define IOTSAFE_CRYPTO_KEYGEN          (1 << 1)
    #define IOTSAFE_CRYPTO_KEYAGREE        (1 << 2)
    #define IOTSAFE_CRYPTO_KDF             (1 << 3)
    #define IOTSAFE_CRYPTO_ALL             (0x0F)

#define IOTSAFE_TAG_PKEY_SKA    0x6F
    #define IOTSAFE_SKA_ECKA (1 << 0)
#endif /* WOLFSSL_IOTSAFE */
#endif /* WOLFSSL_IOTSAFE_H */
