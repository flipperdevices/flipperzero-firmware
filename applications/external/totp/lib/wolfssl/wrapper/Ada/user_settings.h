/* user_settings.h
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


/* should be renamed to user_settings.h for customer use
 * generated from configure options ./configure --enable-all
 *
 * Cleaned up by David Garske
 */

#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H


#ifdef __cplusplus
extern "C" {
#endif

/* Usually comes from configure -> config.h */
#define HAVE_SYS_TIME_H

/* Features */
#define SINGLE_THREADED
#define WOLFSSL_IGNORE_FILE_WARN /* Ignore *.c include warnings */
#define WOLFSSL_PUBLIC_MP /* Make math API's public */
#define WOLFSSL_ENCRYPTED_KEYS /* Support for encrypted keys PKCS8 */
//#define WOLFSSL_SYS_CA_CERTS /* Enable ability to load CA certs from OS */

#if 0 /* Not needed */
    #define KEEP_PEER_CERT /* Retain peer's certificate */
    #define KEEP_OUR_CERT /* Keep our certificate */
    #define WOLFSSL_ALWAYS_VERIFY_CB /* Always call verify callback (configured via wolfSSL_CTX_set_verify API) */
    #define WOLFSSL_VERIFY_CB_ALL_CERTS /* Call verify callback for all intermediate certs */
    #define WOLFSSL_ALWAYS_KEEP_SNI
    #define WOLFSSL_EXTRA_ALERTS /* Allow sending other TLS alerts */
    #define HAVE_EX_DATA /* Enable "extra" EX data API's for user information in CTX/WOLFSSL */
    #define HAVE_EXT_CACHE
    #define ATOMIC_USER /* Enable Atomic Record Layer callbacks */
    #define HAVE_PK_CALLBACKS /* Enable public key callbacks */
    #define WOLFSSL_ALT_NAMES /* Allow alternate cert chain validation to any trusted cert (not entire chain presented by peer) */
    #define HAVE_NULL_CIPHER /* Enable use of TLS cipher suites without cipher (clear text / no encryption) */
    #define WOLFSSL_HAVE_CERT_SERVICE
    #define WOLFSSL_JNI
    #define WOLFSSL_SEP /* certificate policy set extension */
    #define WOLFCRYPT_HAVE_SRP
    #define WOLFSSL_HAVE_WOLFSCEP
    #define HAVE_PKCS7
    #define WOLFSSL_SIGNER_DER_CERT
    #define WOLFSSL_TRUST_PEER_CERT
    #define WOLFSSL_WOLFSSH
    #define WC_NO_ASYNC_THREADING
#endif

/* TLS Features */
#define WOLFSSL_TLS13
#define WOLFSSL_EITHER_SIDE /* allow generic server/client method for WOLFSSL_CTX new */
#define WOLFSSL_TLS13_NO_PEEK_HANDSHAKE_DONE

/* DTLS */
#if 0
    #define WOLFSSL_DTLS
    #define WOLFSSL_MULTICAST

    /* DTLS v1.3 is not yet included with enable-all */
    //#define WOLFSSL_DTLS13
#endif

/* DG Disabled SSLv3 and TLSv1.0 - should avoid using */
//#define WOLFSSL_ALLOW_SSLV3
//#define WOLFSSL_ALLOW_TLSV10

/* TLS Extensions */
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_ONE_TIME_AUTH
#define HAVE_SNI
#define HAVE_ALPN
#define HAVE_MAX_FRAGMENT
#define HAVE_TRUNCATED_HMAC
#define HAVE_SESSION_TICKET
#define WOLFSSL_TICKET_HAVE_ID
#define WOLFSSL_FORCE_CACHE_ON_TICKET
#define HAVE_EXTENDED_MASTER
#define HAVE_TRUSTED_CA
#define HAVE_ENCRYPT_THEN_MAC
#define WOLFSSL_POST_HANDSHAKE_AUTH
#define WOLFSSL_SEND_HRR_COOKIE /* Used by DTLS v1.3 */
#define HAVE_ANON /* anon cipher suites */
#define HAVE_FALLBACK_SCSV /* TLS_FALLBACK_SCSV */
#define WOLFSSL_EARLY_DATA
#define HAVE_SERVER_RENEGOTIATION_INFO

/* TLS Session Cache */
#define SESSION_CERTS
#define PERSIST_SESSION_CACHE
#define PERSIST_CERT_CACHE

/* Key and Certificate Generation */
#define WOLFSSL_KEY_GEN
#define WOLFSSL_CERT_GEN
#define WOLFSSL_CERT_REQ
#define WOLFSSL_CERT_EXT
#define WOLFSSL_MULTI_ATTRIB
#define HAVE_SMIME
#define WOLFSSL_DER_LOAD
#define ASN_BER_TO_DER /* BER to DER support */
#define WOLFSSL_HAVE_ISSUER_NAMES /* Store pointers to issuer name components and their lengths and encodings */
#define WOLFSSL_SUBJ_DIR_ATTR /* Enable support for SubjectDirectoryAttributes extension */
#define WOLFSSL_SUBJ_INFO_ACC /* Enable support for SubjectInfoAccess extension */
#define WOLFSSL_CERT_NAME_ALL /* Adds more certificate name capability at the cost of taking up more memory. Adds initials, givenname, dnQualifer for example */
#define WOLFSSL_FPKI /* Enable support for FPKI (Federal PKI) extensions */
#define WOLFSSL_AKID_NAME /* Enable support for full AuthorityKeyIdentifier extension. Only supports copying full AKID from an existing certificate */
#define HAVE_CTS /* Ciphertext stealing interface */
#define WOLFSSL_PEM_TO_DER
#define WOLFSSL_DER_TO_PEM
#define WOLFSSL_CUSTOM_OID
#define HAVE_OID_ENCODING
#define WOLFSSL_ASN_TEMPLATE

/* Certificate Revocation */
#define HAVE_OCSP
#define HAVE_CERTIFICATE_STATUS_REQUEST
#define HAVE_CERTIFICATE_STATUS_REQUEST_V2
#define HAVE_CRL
#define HAVE_CRL_IO
#define HAVE_IO_TIMEOUT
//#define HAVE_CRL_MONITOR /* DG Disabled (Monitors CRL files on filesystem) - not portable feature */


#if 1
    /* sp_int.c */
    #define WOLFSSL_SP_MATH_ALL
#else
    /* Fast math key size 4096-bit max */
    #define USE_FAST_MATH
#endif
//#define HAVE___UINT128_T 1 /* DG commented: May not be portable */

/* Max Sizes */
#define RSA_MAX_SIZE 4096
#define FP_MAX_BITS 8192
#define SP_INT_BITS 4096


/* Timing Resistance */
#define TFM_TIMING_RESISTANT
#define ECC_TIMING_RESISTANT
#define WC_RSA_BLINDING

/* DH Key Sizes */
#define HAVE_FFDHE_2048
#define HAVE_FFDHE_3072
#define WOLFSSL_DH_EXTRA /* Enable additional DH key import/export */
#define HAVE_DH_DEFAULT_PARAMS

/* ECC Features */
#define HAVE_ECC
#define TFM_ECC256
#define ECC_SHAMIR
#define WOLFSSL_CUSTOM_CURVES /* enable other curves (not just prime) */
#define HAVE_ECC_SECPR2
#define HAVE_ECC_SECPR3
#define HAVE_ECC_BRAINPOOL
#define HAVE_ECC_KOBLITZ
#define HAVE_ECC_CDH /* Co-factor */
#define HAVE_COMP_KEY /* Compressed key support */
#define FP_ECC /* Fixed point caching - speed repeated operations against same key */
#define HAVE_ECC_ENCRYPT
#define WOLFCRYPT_HAVE_ECCSI
#define WOLFCRYPT_HAVE_SAKKE
#define WOLFSSL_ECDSA_DETERMINISTIC_K_VARIANT

/* RSA */
#define WC_RSA_PSS
#define WOLFSSL_PSS_LONG_SALT
#define WC_RSA_NO_PADDING

/* AES */
#define HAVE_AES_DECRYPT
#define HAVE_AES_ECB
#define WOLFSSL_AES_DIRECT
#define WOLFSSL_AES_COUNTER
#define HAVE_AESGCM
#define GCM_TABLE_4BIT
#define WOLFSSL_AESGCM_STREAM
#define HAVE_AESCCM
#define WOLFSSL_AES_OFB
#define WOLFSSL_AES_CFB
#define WOLFSSL_AES_XTS
#define HAVE_AES_KEYWRAP
#define WOLFSSL_AES_CBC_LENGTH_CHECKS
#define WOLFSSL_USE_ALIGN
#define WOLFSSL_AES_SIV

/* Hashing */
#define WOLFSSL_SHA224
#define WOLFSSL_SHA512
#define WOLFSSL_SHA384
#define WOLFSSL_SHAKE256
#define WOLFSSL_SHA3
#define WOLFSSL_HASH_FLAGS /* enable hash flag API's */
#define WOLFSSL_SHAKE256

/* Additional Algorithms */
#define HAVE_HASHDRBG
#define HAVE_CURVE25519
#define HAVE_ED25519
#define WOLFSSL_ED25519_STREAMING_VERIFY
#define CURVED25519_SMALL
#define HAVE_ED448
#define WOLFSSL_ED448_STREAMING_VERIFY
#define HAVE_CURVE448
#define HAVE_POLY1305
#define HAVE_CHACHA
#define HAVE_XCHACHA
#define HAVE_HKDF
#define HAVE_X963_KDF
#define WOLFSSL_CMAC
#define WOLFSSL_DES_ECB
#define HAVE_BLAKE2
#define HAVE_BLAKE2B
#define HAVE_BLAKE2S
#define WOLFSSL_SIPHASH
#define HAVE_KEYING_MATERIAL
#define WOLFSSL_HAVE_PRF

/* Encrypted Client Hello */
#define HAVE_HPKE
#define HAVE_ECH

/* Non-Standard Algorithms (DG disabled) */
//#define HAVE_CAMELLIA
//#define WOLFSSL_RIPEMD
//#define HAVE_SCRYPT
//#define WOLFSSL_MD2
//#define WOLFSSL_ALLOW_RC4

/* Encoding */
#define WOLFSSL_BASE16
#define WOLFSSL_BASE64_ENCODE


/* Openssl compatibility */
#if 0 /* DG Disabled */
    /* Openssl compatibility API's */
    #define OPENSSL_EXTRA
    #define OPENSSL_ALL
    #define HAVE_OPENSSL_CMD
    #define SSL_TXT_TLSV1_2
    #define SSL_TXT_TLSV1_1
    #define OPENSSL_NO_SSL2
    #define OPENSSL_NO_SSL3
    #define NO_OLD_RNGNAME
    #define NO_OLD_WC_NAMES
    #define NO_OLD_SSL_NAMES
    #define NO_OLD_SHA_NAMES
    #define NO_OLD_MD5_NAME
    #define OPENSSL_NO_EC /* macro to enable ECC in openssl */
    #define WOLFSSL_VERBOSE_ERRORS
    #define ERROR_QUEUE_PER_THREAD
    #define WOLFSSL_ERROR_CODE_OPENSSL
    #define HAVE_WOLFSSL_SSL_H 1
    #define OPENSSL_COMPATIBLE_DEFAULTS

    /* Openssl compatibility application specific */
    #define WOLFSSL_LIBWEBSOCKETS
    #define WOLFSSL_OPENSSH
    #define WOLFSSL_QT
    #define FORTRESS
    #define HAVE_WEBSERVER
    #define HAVE_LIGHTY
    #define WOLFSSL_NGINX
    #define WOLFSSL_HAPROXY
    #define HAVE_STUNNEL
    #define WOLFSSL_ASIO
    #define ASIO_USE_WOLFSSL
    #define BOOST_ASIO_USE_WOLFSSL
    #define WOLFSSL_OPENVPN

    #define NO_WOLFSSL_STUB
#endif

/* TLS static cipher support - off by default */
#if 0
    #define WOLFSSL_STATIC_RSA
    #define WOLFSSL_STATIC_DH
    #define WOLFSSL_STATIC_PSK
#endif

/* TLS sniffer support - off by default */
#if 0
    #define WOLFSSL_STATIC_EPHEMERAL
    #define WOLFSSL_SNIFFER
#endif

/* Deprecated */
#define NO_DSA
#define NO_MD4
#define NO_MD5
#define NO_OLD_TLS

/* Used to manually test disable edge cases */
#ifdef TEST_DISABLES
    #define NO_SESSION_CACHE

    //#define NO_ECC256
    //#define NO_ECC_KEY_EXPORT
    //#define NO_ECC_DHE
    //#define NO_ECC_SIGN
    //#define NO_ECC_VERIFY

    //#define NO_RSA
    #define NO_DH

    #define NO_SHA
    #define NO_SHA256
    #ifdef NO_SHA256
        #undef WOLFSSL_SHA224
    #endif
    #define NO_SHA512
    #ifdef NO_SHA512
        #undef WOLFSSL_SHA384
        #undef WOLFSSL_SHA512
        #undef HAVE_ED25519
    #endif

    //#define NO_KDF
    //#define NO_HMAC

    #define NO_RC4
    #define NO_DES3
    //#define NO_AES
    #define NO_AES_CBC
    #define WOLFSSL_NO_SHAKE128

    #define NO_PSK
    #define NO_PWDBASED

    //#define WOLFSSL_NO_TLS12

    //#define NO_64BIT
    #define WOLFSSL_SP_NO_MALLOC
    #define NO_FILESYSTEM
    #define NO_WRITEV

    #define NO_ERROR_STRINGS
    //#define NO_WOLFSSL_CLIENT
    //#define NO_WOLFSSL_SERVER

    #define NO_MULTIBYTE_PRINT
    //#define NO_ASN_TIME
    //#define NO_ASN_CRYPT
    //#define NO_CODING
    #define NO_SIG_WRAPPER
    //#define NO_HASH_WRAPPER
    //#define WC_NO_HARDEN

    //#define NO_CERTS
    //#define NO_ASN
#endif

#ifdef __cplusplus
}
#endif


#endif /* WOLFSSL_USER_SETTINGS_H */
