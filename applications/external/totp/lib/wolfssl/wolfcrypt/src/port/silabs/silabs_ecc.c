/* silabs_ecc.c
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

#if defined(WOLFSSL_SILABS_SE_ACCEL)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/port/silabs/silabs_ecc.h>

#ifndef WOLFSSL_HAVE_ECC_KEY_GET_PRIV
    /* FIPS build has replaced ecc.h. */
    #define wc_ecc_key_get_priv(key) (&((key)->k))
    #define WOLFSSL_HAVE_ECC_KEY_GET_PRIV
#endif

#define SILABS_UNSUPPORTED_KEY_TYPE 0xFFFFFFFF

static sl_se_key_type_t silabs_map_key_type (ecc_curve_id curve_id)
{
    sl_se_key_type_t res = SILABS_UNSUPPORTED_KEY_TYPE;

    switch(curve_id) {
    case ECC_SECP192R1:
        res = SL_SE_KEY_TYPE_ECC_P192;
        break;
    case ECC_SECP256R1:
        res = SL_SE_KEY_TYPE_ECC_P256;
        break;

#ifdef SL_SE_KEY_TYPE_ECC_P384
    case ECC_SECP384R1:
        res = SL_SE_KEY_TYPE_ECC_P384;
        break;
#endif

#ifdef SL_SE_KEY_TYPE_ECC_P521
    case ECC_SECP521R1:
        res = SL_SE_KEY_TYPE_ECC_P521;
        break;
#endif

#if defined(HAVE_CURVE25519) && defined(SL_SE_KEY_TYPE_ECC_X25519)
    case ECC_X25519:
        res = SL_SE_KEY_TYPE_ECC_X25519;
        break;
#endif

#if defined(HAVE_CURVE448) && defined(SL_SE_KEY_TYPE_ECC_X448)
    case ECC_X448:
        res = SL_SE_KEY_TYPE_ECC_X448;
        break;
#endif

    default:
        res = SILABS_UNSUPPORTED_KEY_TYPE;
        break;
    }

    return res;
}

int silabs_ecc_sign_hash(const byte* in, word32 inlen, byte* out, word32 *outlen, ecc_key* key)
{
    sl_status_t sl_stat = sl_se_init_command_context(&(key->cmd_ctx));
    word32 siglen = *outlen;

    sl_stat = sl_se_validate_key(&(key->key));

    if (key->dp->size * 2 <= (int)siglen) {
        siglen = key->dp->size * 2;
    }

    sl_stat = sl_se_ecc_sign(
        &(key->cmd_ctx),
        &(key->key),
        0,
        1,
        in,
        inlen,
        out,
        siglen
        );

    return (sl_stat == SL_STATUS_OK) ? 0 : WC_HW_E;
}

#ifdef HAVE_ECC_VERIFY

int silabs_ecc_verify_hash(const byte* sig, word32 siglen,
                           const byte* hash, word32 hashlen,
                           int* stat, ecc_key* key)
{
    sl_status_t sl_stat = sl_se_init_command_context(&(key->cmd_ctx));

    sl_stat = sl_se_ecc_verify(
        &(key->cmd_ctx),
        &(key->key),
        0,
        1,
        hash,
        hashlen,
        sig,
        siglen);

    if (sl_stat == SL_STATUS_OK) {
        *stat = 1;
    } else if (sl_stat == SL_STATUS_INVALID_SIGNATURE) {
        *stat = 0;
    } else {
        return WC_HW_E;
    }

    return 0;
}
#endif

int silabs_ecc_make_key(ecc_key* key, int keysize)
{
    sl_status_t sl_stat;

    key->key.type = silabs_map_key_type(key->dp->id);
    if (SILABS_UNSUPPORTED_KEY_TYPE == key->key.type)
        return WC_HW_E;

    key->key.size = keysize;
    key->key.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
    key->key.flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
        | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
        | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY;

    sl_stat = sl_se_get_storage_size(&key->key, &key->key.storage.location.buffer.size);
    key->key.storage.location.buffer.pointer = key->key_raw;

    sl_stat = sl_se_generate_key(&(key->cmd_ctx),
                                 &(key->key));

    key->type = ECC_PRIVATEKEY;

    /* copy key to mp components */
    mp_read_unsigned_bin (key->pubkey.x,
                          key->key.storage.location.buffer.pointer,
                          keysize);
    mp_read_unsigned_bin (key->pubkey.y,
                          key->key.storage.location.buffer.pointer  + keysize,
                          keysize);
    mp_read_unsigned_bin (wc_ecc_key_get_priv(key),
                          key->key.storage.location.buffer.pointer + 2 * keysize,
                          keysize);

    return (sl_stat == SL_STATUS_OK) ? 0 : WC_HW_E;
}

int silabs_ecc_import(ecc_key* key, word32 keysize)
{
    sl_status_t sl_stat;
    int err = MP_OKAY;
    word32 used = keysize;

    key->key.type = silabs_map_key_type(key->dp->id);
    if (SILABS_UNSUPPORTED_KEY_TYPE == key->key.type)
        return WC_HW_E;

    key->key.size = keysize;
    key->key.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
    key->key.flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
        | SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
        | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY;

    sl_stat = sl_se_get_storage_size(&key->key, &key->key.storage.location.buffer.size);
    key->key.storage.location.buffer.pointer = key->key_raw;
    if (sl_stat != SL_STATUS_OK)
      return WC_HW_E;

    key->type = ECC_PRIVATEKEY;

    /* copy key from mp components */
    if (err == MP_OKAY)
        err = wc_export_int(key->pubkey.x, key->key.storage.location.buffer.pointer,
                            &used, keysize,
                            WC_TYPE_UNSIGNED_BIN);
    if (err == MP_OKAY)
        err = wc_export_int(key->pubkey.y, key->key.storage.location.buffer.pointer + keysize,
                            &used, keysize,
                            WC_TYPE_UNSIGNED_BIN);
    if (err == MP_OKAY)
        err = wc_export_int(wc_ecc_key_get_priv(key),
            key->key.storage.location.buffer.pointer + 2 * keysize, &used,
            keysize, WC_TYPE_UNSIGNED_BIN);

    return err;
}

int silabs_ecc_import_private(ecc_key* key, word32 keysize)
{
    sl_status_t sl_stat;
    int ret = 0;
    word32 keySz = keysize;
    key->key.type = silabs_map_key_type(key->dp->id);
    if (SILABS_UNSUPPORTED_KEY_TYPE == key->key.type)
        return WC_HW_E;

    key->key.size = key->dp->size;
    key->key.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
    key->key.flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY
        | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY;

    sl_stat = sl_se_get_storage_size(&key->key, &key->key.storage.location.buffer.size);
    key->key.storage.location.buffer.pointer = key->key_raw;
    if (sl_stat != SL_STATUS_OK)
          return WC_HW_E;

    ret = wc_export_int(wc_ecc_key_get_priv(key),
        key->key.storage.location.buffer.pointer, &keySz, keySz,
        WC_TYPE_UNSIGNED_BIN);

    if (keySz != keysize)
        ret = WC_HW_E;

    return ret;
}

int silabs_ecc_sig_to_rs(ecc_key* key, word32 keySz)
{
    sl_status_t sl_stat;
    int err = MP_OKAY;

    key->key.type = silabs_map_key_type(key->dp->id);
    if (SILABS_UNSUPPORTED_KEY_TYPE == key->key.type)
        return WC_HW_E;

    key->key.size = keySz;
    key->key.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
    key->key.flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY
        | SL_SE_KEY_FLAG_ASYMMMETRIC_SIGNING_ONLY;

    sl_stat = sl_se_get_storage_size(&key->key, &key->key.storage.location.buffer.size);
    key->key.storage.location.buffer.pointer = key->key_raw;
    if (sl_stat != SL_STATUS_OK)
          return WC_HW_E;

    if (err == MP_OKAY) {
        keySz = key->dp->size;
        err = wc_export_int(key->pubkey.x,
                            key->key.storage.location.buffer.pointer,
                            &keySz, keySz, WC_TYPE_UNSIGNED_BIN);
        if (err == MP_OKAY)
            err = wc_export_int(key->pubkey.y,
                                key->key.storage.location.buffer.pointer + keySz,
                                &keySz, keySz, WC_TYPE_UNSIGNED_BIN);
    }

    return err;
}

int silabs_ecc_import_private_raw(ecc_key* key, word32 keySz, const char* d, int encType)
{
    sl_status_t sl_stat;
    int err = MP_OKAY;
    key->type = ECC_PRIVATEKEY;
    key->key.flags |= SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PRIVATE_KEY;

    sl_stat = sl_se_get_storage_size(&key->key, &key->key.storage.location.buffer.size);
    if (sl_stat != SL_STATUS_OK)
          return WC_HW_E;

    if (encType == WC_TYPE_HEX_STR)
        err = mp_read_radix(wc_ecc_key_get_priv(key), d, MP_RADIX_HEX);
    else
        err = mp_read_unsigned_bin(wc_ecc_key_get_priv(key), (const byte*)d,
                                   key->dp->size);
    if (err == MP_OKAY) {
        err = wc_export_int(wc_ecc_key_get_priv(key),
            key->key.storage.location.buffer.pointer + (2 * keySz), &keySz,
            keySz, WC_TYPE_UNSIGNED_BIN);
    }

    return err;
}

int silabs_ecc_shared_secret(ecc_key* private_key, ecc_key* public_key,
                             byte* out, word32* outlen)
{
    sl_se_command_context_t cmd;
    sl_se_key_descriptor_t key_out;
    sl_se_key_descriptor_t pub_key;
    uint32_t pub_sz = 0;
    sl_status_t sl_stat;

    /* `sl_se_ecdh_compute_shared_secret` returns the full coordinate
     * point, but `wc_ecc_shared_secret` should only return the x
     * coordinate. This buffer is used to hold the output of the
     * secure element output and only the first half is copied to the
     * `out` buffer.
     */
    byte fullpoint[2 * ECC_MAX_CRYPTO_HW_SIZE];

    pub_key = public_key->key;
    pub_key.flags = SL_SE_KEY_FLAG_ASYMMETRIC_BUFFER_HAS_PUBLIC_KEY;

    if (*outlen < pub_key.size) {
        return BUFFER_E;
    }

    pub_sz = pub_key.size * 2;

    XMEMSET(&key_out, 0, sizeof(key_out));
    key_out.type = SL_SE_KEY_TYPE_SYMMETRIC;
    key_out.storage.method = SL_SE_KEY_STORAGE_EXTERNAL_PLAINTEXT;
    key_out.storage.location.buffer.pointer = fullpoint;
    key_out.size = pub_sz;
    key_out.storage.location.buffer.size = pub_sz;

    sl_stat = sl_se_ecdh_compute_shared_secret(
        &cmd,
        &(private_key->key),
        &pub_key,
        &key_out);

    if (sl_stat == SL_STATUS_OK) {
        *outlen = pub_key.size;
        XMEMCPY(out, fullpoint, *outlen);
    }

    return (sl_stat == SL_STATUS_OK) ? 0 : WC_HW_E;
}

#endif /* WOLFSSL_SILABS_SE_ACCEL */
