/* x509v3.h
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

/* x509v3.h for openSSL */

#ifndef WOLFSSL_x509v3_H
#define WOLFSSL_x509v3_H

#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/openssl/compat_types.h>
#include <wolfssl/openssl/conf.h>
#include <wolfssl/openssl/bio.h>
#include <wolfssl/openssl/ssl.h>

#ifdef __cplusplus
    extern "C" {
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

#define EXFLAG_KUSAGE  0x2
#define EXFLAG_XKUSAGE 0x4

#define KU_DIGITAL_SIGNATURE    KEYUSE_DIGITAL_SIG
#define KU_NON_REPUDIATION      KEYUSE_CONTENT_COMMIT
#define KU_KEY_ENCIPHERMENT     KEYUSE_KEY_ENCIPHER
#define KU_DATA_ENCIPHERMENT    KEYUSE_DATA_ENCIPHER
#define KU_KEY_AGREEMENT        KEYUSE_KEY_AGREE
#define KU_KEY_CERT_SIGN        KEYUSE_KEY_CERT_SIGN
#define KU_CRL_SIGN             KEYUSE_CRL_SIGN
#define KU_ENCIPHER_ONLY        KEYUSE_ENCIPHER_ONLY
#define KU_DECIPHER_ONLY        KEYUSE_DECIPHER_ONLY

#define XKU_SSL_SERVER 0x1
#define XKU_SSL_CLIENT 0x2
#define XKU_SMIME      0x4
#define XKU_CODE_SIGN  0x8
#define XKU_SGC        0x10
#define XKU_OCSP_SIGN  0x20
#define XKU_TIMESTAMP  0x40
#define XKU_DVCS       0x80
#define XKU_ANYEKU     0x100

#define X509_PURPOSE_SSL_CLIENT       0
#define X509_PURPOSE_SSL_SERVER       1

#define NS_SSL_CLIENT                 WC_NS_SSL_CLIENT
#define NS_SSL_SERVER                 WC_NS_SSL_SERVER

/* Forward reference */

#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x0090801fL
typedef void *(*X509V3_EXT_D2I)(void *, const unsigned char **, long);
#else
typedef void *(*X509V3_EXT_D2I)(void *, unsigned char **, long);
#endif
typedef int (*X509V3_EXT_I2D) (void *, unsigned char **);
typedef STACK_OF(CONF_VALUE) *(*X509V3_EXT_I2V) (
                                struct WOLFSSL_v3_ext_method *method,
                                void *ext, STACK_OF(CONF_VALUE) *extlist);
typedef char *(*X509V3_EXT_I2S)(struct WOLFSSL_v3_ext_method *method, void *ext);
typedef int (*X509V3_EXT_I2R) (struct WOLFSSL_v3_ext_method *method,
                               void *ext, BIO *out, int indent);
typedef struct WOLFSSL_v3_ext_method X509V3_EXT_METHOD;

struct WOLFSSL_v3_ext_method {
    int ext_nid;
    int ext_flags;
    void *usr_data;
    X509V3_EXT_D2I d2i;
    X509V3_EXT_I2D i2d;
    X509V3_EXT_I2V i2v;
    X509V3_EXT_I2S i2s;
    X509V3_EXT_I2R i2r;
};

struct WOLFSSL_X509_EXTENSION {
    WOLFSSL_ASN1_OBJECT *obj;
    WOLFSSL_ASN1_BOOLEAN crit;
    ASN1_OCTET_STRING value; /* DER format of extension */
    WOLFSSL_v3_ext_method ext_method;
    WOLFSSL_STACK* ext_sk; /* For extension specific data */
};

#define WOLFSSL_ASN1_BOOLEAN int
#define GEN_OTHERNAME   0
#define GEN_EMAIL       1
#define GEN_DNS         2
#define GEN_X400        3
#define GEN_DIRNAME     4
#define GEN_EDIPARTY    5
#define GEN_URI         6
#define GEN_IPADD       7
#define GEN_RID         8
#define GEN_IA5         9

#define GENERAL_NAME       WOLFSSL_GENERAL_NAME

#define X509V3_CTX         WOLFSSL_X509V3_CTX

#define CTX_TEST           0x1

typedef struct WOLFSSL_AUTHORITY_KEYID AUTHORITY_KEYID;
typedef struct WOLFSSL_BASIC_CONSTRAINTS BASIC_CONSTRAINTS;
typedef struct WOLFSSL_ACCESS_DESCRIPTION ACCESS_DESCRIPTION;
typedef WOLF_STACK_OF(WOLFSSL_ACCESS_DESCRIPTION) WOLFSSL_AUTHORITY_INFO_ACCESS;

WOLFSSL_API WOLFSSL_BASIC_CONSTRAINTS* wolfSSL_BASIC_CONSTRAINTS_new(void);
WOLFSSL_API void wolfSSL_BASIC_CONSTRAINTS_free(WOLFSSL_BASIC_CONSTRAINTS *bc);
WOLFSSL_API WOLFSSL_AUTHORITY_KEYID* wolfSSL_AUTHORITY_KEYID_new(void);
WOLFSSL_API void wolfSSL_AUTHORITY_KEYID_free(WOLFSSL_AUTHORITY_KEYID *id);
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
WOLFSSL_API const WOLFSSL_v3_ext_method* wolfSSL_X509V3_EXT_get(
                                                    WOLFSSL_X509_EXTENSION* ex);
#else
WOLFSSL_API WOLFSSL_v3_ext_method* wolfSSL_X509V3_EXT_get(
                                                    WOLFSSL_X509_EXTENSION* ex);
#endif
WOLFSSL_API void* wolfSSL_X509V3_EXT_d2i(WOLFSSL_X509_EXTENSION* ex);
WOLFSSL_API char* wolfSSL_i2s_ASN1_STRING(WOLFSSL_v3_ext_method *method,
                                          const WOLFSSL_ASN1_STRING *s);
WOLFSSL_API int wolfSSL_X509V3_EXT_print(WOLFSSL_BIO *out,
        WOLFSSL_X509_EXTENSION *ext, unsigned long flag, int indent);
WOLFSSL_API int wolfSSL_X509V3_EXT_add_nconf(WOLFSSL_CONF *conf, WOLFSSL_X509V3_CTX *ctx,
        const char *section, WOLFSSL_X509 *cert);
WOLFSSL_API WOLFSSL_ASN1_STRING* wolfSSL_a2i_IPADDRESS(const char* ipa);

#define BASIC_CONSTRAINTS_free    wolfSSL_BASIC_CONSTRAINTS_free
#define AUTHORITY_KEYID_free      wolfSSL_AUTHORITY_KEYID_free
#define SSL_CTX_get_cert_store(x) wolfSSL_CTX_get_cert_store ((WOLFSSL_CTX*) (x))
#define ASN1_INTEGER              WOLFSSL_ASN1_INTEGER
#define ASN1_OCTET_STRING         WOLFSSL_ASN1_STRING
#define X509V3_EXT_get            wolfSSL_X509V3_EXT_get
#define X509V3_EXT_d2i            wolfSSL_X509V3_EXT_d2i
#define X509V3_EXT_add_nconf      wolfSSL_X509V3_EXT_add_nconf
#ifndef NO_WOLFSSL_STUB
#define X509V3_parse_list(...)    NULL
#endif
#define i2s_ASN1_OCTET_STRING     wolfSSL_i2s_ASN1_STRING
#define a2i_IPADDRESS             wolfSSL_a2i_IPADDRESS
#define X509V3_EXT_print          wolfSSL_X509V3_EXT_print
#define X509V3_EXT_conf_nid       wolfSSL_X509V3_EXT_conf_nid
#define X509V3_set_ctx            wolfSSL_X509V3_set_ctx
#ifndef NO_WOLFSSL_STUB
#define X509V3_set_nconf(...)     WC_DO_NOTHING
#define X509V3_EXT_cleanup(...)   WC_DO_NOTHING
#endif
#define X509V3_set_ctx_test(ctx)  wolfSSL_X509V3_set_ctx(ctx, NULL, NULL, NULL, NULL, CTX_TEST)
#define X509V3_set_ctx_nodb       wolfSSL_X509V3_set_ctx_nodb
#define X509v3_get_ext_count      wolfSSL_sk_num

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef  __cplusplus
}
#endif

#endif
