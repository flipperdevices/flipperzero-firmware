/* ssl.h
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



/*  ssl.h defines wolfssl_openssl compatibility layer
 *
 */


#ifndef WOLFSSL_OPENSSL_H_
#define WOLFSSL_OPENSSL_H_

#include <wolfssl/wolfcrypt/types.h>

/* wolfssl_openssl compatibility layer */
#ifndef OPENSSL_EXTRA_SSL_GUARD
#define OPENSSL_EXTRA_SSL_GUARD
#include <wolfssl/ssl.h>
#endif /* OPENSSL_EXTRA_SSL_GUARD */

#include <wolfssl/openssl/tls1.h>
#ifndef WOLFCRYPT_ONLY
#include <wolfssl/openssl/evp.h>
#endif
#include <wolfssl/openssl/bio.h>
#ifdef OPENSSL_EXTRA
#include <wolfssl/openssl/crypto.h>
#endif

#if defined(WOLFSSL_QT) || defined(OPENSSL_ALL)
#include <wolfssl/openssl/dh.h>
#include <wolfssl/openssl/objects.h>
#endif

/* need MIN_CODE_E to determine wolfSSL error range */
#include <wolfssl/wolfcrypt/error-crypt.h>

/* all NID_* values are in asn.h */
#include <wolfssl/wolfcrypt/asn.h>

#include <wolfssl/openssl/x509.h>

#ifdef __cplusplus
    extern "C" {
#endif

#ifdef _WIN32
    /* wincrypt.h clashes */
    #undef X509_NAME
#endif

#ifdef WOLFSSL_UTASKER
    /* tcpip.h clashes */
    #undef ASN1_INTEGER
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

typedef WOLFSSL          SSL;
typedef WOLFSSL_SESSION  SSL_SESSION;
typedef WOLFSSL_METHOD   SSL_METHOD;
typedef WOLFSSL_CTX      SSL_CTX;

typedef WOLFSSL_X509       X509;
typedef WOLFSSL_X509       X509_REQ;
typedef WOLFSSL_X509_NAME  X509_NAME;
typedef WOLFSSL_X509_INFO  X509_INFO;
typedef WOLFSSL_X509_CHAIN X509_CHAIN;

/* STACK_OF(ASN1_OBJECT) */
typedef WOLFSSL_STACK      EXTENDED_KEY_USAGE;


/* redeclare guard */
#define WOLFSSL_TYPES_DEFINED

typedef WOLFSSL_BIO            BIO;
typedef WOLFSSL_BIO_METHOD     BIO_METHOD;
typedef WOLFSSL_CIPHER         SSL_CIPHER;
typedef WOLFSSL_X509_LOOKUP    X509_LOOKUP;
typedef WOLFSSL_X509_LOOKUP_METHOD X509_LOOKUP_METHOD;
typedef WOLFSSL_X509_CRL       X509_CRL;
typedef WOLFSSL_X509_EXTENSION X509_EXTENSION;
typedef WOLFSSL_X509_PUBKEY    X509_PUBKEY;
typedef WOLFSSL_X509_ALGOR     X509_ALGOR;
typedef WOLFSSL_ASN1_TIME      ASN1_TIME;
typedef WOLFSSL_ASN1_INTEGER   ASN1_INTEGER;
typedef WOLFSSL_ASN1_OBJECT    ASN1_OBJECT;
typedef WOLFSSL_ASN1_STRING    ASN1_STRING;
typedef WOLFSSL_ASN1_TYPE      ASN1_TYPE;
typedef WOLFSSL_X509_ATTRIBUTE X509_ATTRIBUTE;
typedef WOLFSSL_ASN1_BIT_STRING ASN1_BIT_STRING;
typedef WOLFSSL_dynlock_value  CRYPTO_dynlock_value;
typedef WOLFSSL_BUF_MEM        BUF_MEM;
typedef WOLFSSL_GENERAL_NAMES  GENERAL_NAMES;
typedef WOLFSSL_GENERAL_NAME   GENERAL_NAME;
typedef WOLFSSL_OBJ_NAME       OBJ_NAME;
typedef WOLFSSL_DIST_POINT_NAME DIST_POINT_NAME;
typedef WOLFSSL_DIST_POINT      DIST_POINT;

#define X509_L_FILE_LOAD  WOLFSSL_X509_L_FILE_LOAD
#define X509_L_ADD_DIR    WOLFSSL_X509_L_ADD_DIR
#define X509_L_ADD_STORE  WOLFSSL_X509_L_ADD_STORE
#define X509_L_LOAD_STORE WOLFSSL_X509_L_LOAD_STORE

#define ASN1_UTCTIME         WOLFSSL_ASN1_TIME
#define ASN1_GENERALIZEDTIME WOLFSSL_ASN1_TIME

typedef WOLFSSL_COMP_METHOD    COMP_METHOD;
typedef WOLFSSL_COMP           SSL_COMP;
typedef WOLFSSL_X509_REVOKED   X509_REVOKED;
typedef WOLFSSL_X509_LOOKUP_TYPE X509_LOOKUP_TYPE;
typedef WOLFSSL_X509_OBJECT    X509_OBJECT;
typedef WOLFSSL_X509_STORE     X509_STORE;
typedef WOLFSSL_X509_STORE_CTX X509_STORE_CTX;
typedef WOLFSSL_X509_VERIFY_PARAM X509_VERIFY_PARAM;

typedef int                    OSSL_HANDSHAKE_STATE;
#define TLS_ST_BEFORE          0 /* NULL_STATE from enum states */

#define EVP_CIPHER_INFO        EncryptedInfo

#define STACK_OF(x) WOLFSSL_STACK
#define OPENSSL_STACK WOLFSSL_STACK
#define _STACK OPENSSL_STACK

#define CONF_get1_default_config_file   wolfSSL_CONF_get1_default_config_file
typedef STACK_OF(ACCESS_DESCRIPTION) AUTHORITY_INFO_ACCESS;

#define CRYPTO_free                     wolfSSL_CRYPTO_free
#define CRYPTO_malloc                   wolfSSL_CRYPTO_malloc
#define CRYPTO_EX_new                   WOLFSSL_CRYPTO_EX_new
#define CRYPTO_EX_dup                   WOLFSSL_CRYPTO_EX_dup
#define CRYPTO_EX_free                  WOLFSSL_CRYPTO_EX_free
#ifdef HAVE_EX_DATA
    #define CRYPTO_EX_DATA                  WOLFSSL_CRYPTO_EX_DATA
#endif

#define CRYPTO_set_mem_functions        wolfSSL_CRYPTO_set_mem_functions

/* depreciated */
#define CRYPTO_thread_id                wolfSSL_thread_id
#define CRYPTO_set_id_callback          wolfSSL_set_id_callback

#define CRYPTO_LOCK             0x01
#define CRYPTO_UNLOCK           0x02
#define CRYPTO_READ             0x04
#define CRYPTO_WRITE            0x08

#define CRYPTO_set_locking_callback     wolfSSL_set_locking_callback
#define CRYPTO_get_locking_callback     wolfSSL_get_locking_callback
#define CRYPTO_set_dynlock_create_callback  wolfSSL_set_dynlock_create_callback
#define CRYPTO_set_dynlock_lock_callback wolfSSL_set_dynlock_lock_callback
#define CRYPTO_set_dynlock_destroy_callback wolfSSL_set_dynlock_destroy_callback
#define CRYPTO_num_locks                wolfSSL_num_locks
#define CRYPTO_dynlock_value            WOLFSSL_dynlock_value

#define CRYPTO_cleanup_all_ex_data      wolfSSL_cleanup_all_ex_data
#define set_ex_data                     wolfSSL_CRYPTO_set_ex_data
#define get_ex_data                     wolfSSL_CRYPTO_get_ex_data
#define CRYPTO_memcmp                   wolfSSL_CRYPTO_memcmp
#define CRYPTO_get_ex_new_index         wolfSSL_CRYPTO_get_ex_new_index

#define CRYPTO_get_ex_new_index         wolfSSL_CRYPTO_get_ex_new_index

/* this function was used to set the default malloc, free, and realloc */
#define CRYPTO_malloc_init() 0 /* CRYPTO_malloc_init is not needed */
#define OPENSSL_malloc_init() 0 /* OPENSSL_malloc_init is not needed */

#define SSL_get_client_random(ssl,out,outSz) \
                                  wolfSSL_get_client_random((ssl),(out),(outSz))
#define SSL_get_cipher_list(ctx,i)         wolfSSL_get_cipher_list_ex((ctx),(i))
#define SSL_get_cipher_name(ctx)           wolfSSL_get_cipher((ctx))
#define SSL_get_shared_ciphers(ctx,buf,len) \
                                   wolfSSL_get_shared_ciphers((ctx),(buf),(len))

/* at the moment only returns ok */
#define SSL_get_verify_result           wolfSSL_get_verify_result
#define SSL_get_verify_mode             wolfSSL_get_verify_mode
#define SSL_get_verify_depth            wolfSSL_get_verify_depth
#define SSL_CTX_get_verify_mode         wolfSSL_CTX_get_verify_mode
#define SSL_CTX_get_verify_depth        wolfSSL_CTX_get_verify_depth
#define SSL_get_certificate             wolfSSL_get_certificate
#define SSL_CTX_get0_certificate        wolfSSL_CTX_get0_certificate
#define SSL_use_certificate             wolfSSL_use_certificate
#define SSL_use_certificate_ASN1        wolfSSL_use_certificate_ASN1
#define d2i_PKCS8_PRIV_KEY_INFO_bio     wolfSSL_d2i_PKCS8_PKEY_bio
#define d2i_PKCS8_PRIV_KEY_INFO         wolfSSL_d2i_PKCS8_PKEY
#define i2d_PKCS8_PRIV_KEY_INFO         wolfSSL_i2d_PrivateKey
#define d2i_PKCS8PrivateKey_bio         wolfSSL_d2i_PKCS8PrivateKey_bio
#define i2d_PKCS8PrivateKey_bio         wolfSSL_PEM_write_bio_PKCS8PrivateKey
#define PKCS8_PRIV_KEY_INFO_free        wolfSSL_EVP_PKEY_free
#define d2i_PKCS12_fp                   wolfSSL_d2i_PKCS12_fp
#define SSL_CTX_set_ecdh_auto           wolfSSL_CTX_set_ecdh_auto

#define i2d_PUBKEY                      wolfSSL_i2d_PUBKEY
#define d2i_PUBKEY                      wolfSSL_d2i_PUBKEY
#define d2i_PUBKEY_bio                  wolfSSL_d2i_PUBKEY_bio
#define d2i_PublicKey                   wolfSSL_d2i_PublicKey
#define d2i_PrivateKey                  wolfSSL_d2i_PrivateKey
#define d2i_AutoPrivateKey              wolfSSL_d2i_AutoPrivateKey
#define SSL_use_PrivateKey              wolfSSL_use_PrivateKey
#define SSL_use_PrivateKey_ASN1         wolfSSL_use_PrivateKey_ASN1
#define SSL_use_RSAPrivateKey_ASN1      wolfSSL_use_RSAPrivateKey_ASN1
#define SSL_get_privatekey              wolfSSL_get_privatekey
#define SSL_CTX_use_PrivateKey_ASN1     wolfSSL_CTX_use_PrivateKey_ASN1

#define SSLv23_method                   wolfSSLv23_method
#define SSLv23_client_method            wolfSSLv23_client_method
#define SSLv2_client_method             wolfSSLv2_client_method
#define SSLv2_server_method             wolfSSLv2_server_method
#define SSLv3_server_method             wolfSSLv3_server_method
#define SSLv3_client_method             wolfSSLv3_client_method
#define TLS_client_method               wolfTLS_client_method
#define TLS_server_method               wolfTLS_server_method
#define TLSv1_method                    wolfTLSv1_method
#define TLSv1_server_method             wolfTLSv1_server_method
#define TLSv1_client_method             wolfTLSv1_client_method
#define TLSv1_1_method                  wolfTLSv1_1_method
#define TLSv1_1_server_method           wolfTLSv1_1_server_method
#define TLSv1_1_client_method           wolfTLSv1_1_client_method
#define TLSv1_2_method                  wolfTLSv1_2_method
#define TLSv1_2_server_method           wolfTLSv1_2_server_method
#define TLSv1_2_client_method           wolfTLSv1_2_client_method
#define TLSv1_3_method                  wolfTLSv1_3_method
#define TLSv1_3_server_method           wolfTLSv1_3_server_method
#define TLSv1_3_client_method           wolfTLSv1_3_client_method
#define TLS_method                      wolfSSLv23_method

#define X509_FILETYPE_PEM               WOLFSSL_FILETYPE_PEM
#define X509_FILETYPE_ASN1              WOLFSSL_FILETYPE_ASN1
#define X509_FILETYPE_DEFAULT           WOLFSSL_FILETYPE_DEFAULT

#define X509_F_X509_CHECK_PRIVATE_KEY   128

#ifdef WOLFSSL_DTLS
    #define DTLSv1_client_method        wolfDTLSv1_client_method
    #define DTLSv1_server_method        wolfDTLSv1_server_method
    #define DTLSv1_2_client_method      wolfDTLSv1_2_client_method
    #define DTLSv1_2_server_method      wolfDTLSv1_2_server_method
    #define DTLS_method                 wolfDTLS_method
#endif


#ifndef NO_FILESYSTEM
    #define SSL_CTX_use_certificate_file      wolfSSL_CTX_use_certificate_file
    #define SSL_CTX_use_PrivateKey_file       wolfSSL_CTX_use_PrivateKey_file
#ifdef WOLFSSL_APACHE_HTTPD
    #define SSL_CTX_load_verify_locations(ctx,file,path) \
        wolfSSL_CTX_load_verify_locations_ex(ctx,file,path,\
                                                   WOLFSSL_LOAD_FLAG_IGNORE_ERR)
#else
    #define SSL_CTX_load_verify_locations     wolfSSL_CTX_load_verify_locations
#endif
    #define SSL_CTX_set_default_verify_paths wolfSSL_CTX_set_default_verify_paths
    #define SSL_CTX_use_certificate_chain_file wolfSSL_CTX_use_certificate_chain_file
    #define SSL_CTX_use_RSAPrivateKey_file    wolfSSL_CTX_use_RSAPrivateKey_file

    #define SSL_use_certificate_file          wolfSSL_use_certificate_file
    #define SSL_use_PrivateKey_file           wolfSSL_use_PrivateKey_file
    #define SSL_use_certificate_chain_file    wolfSSL_use_certificate_chain_file
    #define SSL_use_RSAPrivateKey_file        wolfSSL_use_RSAPrivateKey_file
#endif

#define SSL_CTX_new(method)             wolfSSL_CTX_new((WOLFSSL_METHOD*)(method))
#ifdef OPENSSL_EXTRA
#define SSL_CTX_up_ref                  wolfSSL_CTX_up_ref
#endif
#define SSL_new                         wolfSSL_new
#define SSL_set_fd                      wolfSSL_set_fd
#define SSL_get_fd                      wolfSSL_get_fd
#define SSL_connect                     wolfSSL_connect
#define SSL_clear                       wolfSSL_clear
#define SSL_state                       wolfSSL_state
#define SSL_read_early_data(ssl, d, dLen, len)   wolfSSL_read_early_data(ssl, d, (int)(dLen), (int *)(len))
#define SSL_write_early_data(ssl, d, dLen, len)  wolfSSL_write_early_data(ssl, d, (int)(dLen), (int *)(len))

#define SSL_write                       wolfSSL_write
#define SSL_read                        wolfSSL_read
#define SSL_peek                        wolfSSL_peek
#define SSL_accept                      wolfSSL_accept
#define SSL_CTX_free                    wolfSSL_CTX_free
#define SSL_free                        wolfSSL_free
#define SSL_shutdown                    wolfSSL_shutdown
#define SSL_set_timeout                 wolfSSL_SSL_SESSION_set_timeout

#define SSL_CTX_set_quiet_shutdown      wolfSSL_CTX_set_quiet_shutdown
#define SSL_set_quiet_shutdown          wolfSSL_set_quiet_shutdown
#define SSL_get_error                   wolfSSL_get_error
#define SSL_set_session                 wolfSSL_set_session
#define SSL_get_session(x)              wolfSSL_get_session((WOLFSSL*) (x))
#define SSL_SESSION_get0_peer           wolfSSL_SESSION_get0_peer
#define SSL_flush_sessions              wolfSSL_flush_sessions
#define SSL_CTX_get_session_cache_mode(ctx) \
                                       wolfSSL_CTX_get_session_cache_mode((ctx))

#define SSL_CTX_set_verify              wolfSSL_CTX_set_verify
#define SSL_CTX_set_cert_verify_callback wolfSSL_CTX_set_cert_verify_callback
#define SSL_CTX_set_cert_cb             wolfSSL_CTX_set_cert_cb
#define SSL_set_verify                  wolfSSL_set_verify
#define SSL_set_verify_result           wolfSSL_set_verify_result
#define SSL_verify_client_post_handshake wolfSSL_verify_client_post_handshake
#define SSL_set_post_handshake_auth     wolfSSL_set_post_handshake_auth
#define SSL_CTX_set_post_handshake_auth wolfSSL_CTX_set_post_handshake_auth
#define SSL_pending(x)                  wolfSSL_pending((WOLFSSL*)(x))
#define SSL_has_pending                 wolfSSL_has_pending
#define SSL_load_error_strings          wolfSSL_load_error_strings
#define SSL_library_init                wolfSSL_library_init
#define OPENSSL_cleanup                 (void)wolfSSL_Cleanup
#define OPENSSL_init_ssl                wolfSSL_OPENSSL_init_ssl
#define OpenSSL_add_ssl_algorithms      wolfSSL_library_init
#define SSL_CTX_set_session_cache_mode  wolfSSL_CTX_set_session_cache_mode
#define SSL_CTX_set_cipher_list         wolfSSL_CTX_set_cipher_list
#define SSL_CTX_set_ciphersuites        wolfSSL_CTX_set_cipher_list
#define SSL_set_cipher_list             wolfSSL_set_cipher_list
/* wolfSSL does not support security levels */
#define SSL_CTX_set_security_level      wolfSSL_CTX_set_security_level
#define SSL_CTX_get_security_level      wolfSSL_CTX_get_security_level
#define SSL_export_keying_material      wolfSSL_export_keying_material

#define SSL_CTX_set1_sigalgs_list       wolfSSL_CTX_set1_sigalgs_list
#define SSL_set1_sigalgs_list           wolfSSL_set1_sigalgs_list
#define SSL_get_signature_nid           wolfSSL_get_signature_nid

#define SSL_CTX_set1_groups             wolfSSL_CTX_set1_groups
#define SSL_set1_groups                 wolfSSL_set1_groups

#define SSL_CTX_set1_groups_list        wolfSSL_CTX_set1_groups_list
#define SSL_set1_groups_list            wolfSSL_set1_groups_list

#define SSL_set_ex_data                 wolfSSL_set_ex_data
#define SSL_get_shutdown                wolfSSL_get_shutdown
#define SSL_get_finished                wolfSSL_get_finished
#define SSL_get_peer_finished           wolfSSL_get_peer_finished
#define SSL_set_rfd                     wolfSSL_set_rfd
#define SSL_set_wfd                     wolfSSL_set_wfd
#define SSL_set_shutdown                wolfSSL_set_shutdown
#define SSL_set_session_id_context      wolfSSL_set_session_id_context
#define SSL_set_connect_state           wolfSSL_set_connect_state
#define SSL_set_accept_state            wolfSSL_set_accept_state
#define SSL_SESSION_new                 wolfSSL_SESSION_new
#define SSL_session_reused              wolfSSL_session_reused
#define SSL_SESSION_up_ref              wolfSSL_SESSION_up_ref
#define SSL_SESSION_dup                 wolfSSL_SESSION_dup
#define SSL_SESSION_free                wolfSSL_SESSION_free
#define SSL_SESSION_set_cipher          wolfSSL_SESSION_set_cipher
#define SSL_is_init_finished            wolfSSL_is_init_finished

#define SSL_SESSION_set1_id             wolfSSL_SESSION_set1_id
#define SSL_SESSION_set1_id_context     wolfSSL_SESSION_set1_id_context

#define SSL_get_version                 wolfSSL_get_version
#define SSL_get_current_cipher          wolfSSL_get_current_cipher

/* use wolfSSL_get_cipher_name for its return format */
#define SSL_get_cipher                  wolfSSL_get_cipher_name
#define SSL_CIPHER_description          wolfSSL_CIPHER_description
#define SSL_CIPHER_get_name             wolfSSL_CIPHER_get_name
#define SSL_CIPHER_get_version          wolfSSL_CIPHER_get_version
#define SSL_CIPHER_get_id               wolfSSL_CIPHER_get_id
#define SSL_CIPHER_get_rfc_name         wolfSSL_CIPHER_get_name
#define SSL_CIPHER_standard_name        wolfSSL_CIPHER_get_name
#define SSL_CIPHER_get_auth_nid         wolfSSL_CIPHER_get_auth_nid
#define SSL_CIPHER_get_cipher_nid       wolfSSL_CIPHER_get_cipher_nid
#define SSL_CIPHER_get_digest_nid       wolfSSL_CIPHER_get_digest_nid
#define SSL_CIPHER_get_kx_nid           wolfSSL_CIPHER_get_kx_nid
#define SSL_CIPHER_is_aead              wolfSSL_CIPHER_is_aead
#define SSL_get_cipher_by_value         wolfSSL_get_cipher_by_value

#define SSL_get1_session                wolfSSL_get1_session

#define SSL_get_keyblock_size           wolfSSL_get_keyblock_size
#define SSL_get_keys                    wolfSSL_get_keys
#define SSL_SESSION_get_master_key      wolfSSL_SESSION_get_master_key
#define SSL_SESSION_get_master_key_length wolfSSL_SESSION_get_master_key_length
#define SSL_SESSION_get_max_early_data  wolfSSL_SESSION_get_max_early_data

#if defined(WOLFSSL_QT) || defined(OPENSSL_ALL)
    #define SSL_MODE_RELEASE_BUFFERS     0x00000010U
    #define ASN1_BOOLEAN                 WOLFSSL_ASN1_BOOLEAN
    #define X509_get_ext                 wolfSSL_X509_get_ext
    #define X509_get_ext_by_OBJ          wolfSSL_X509_get_ext_by_OBJ
    #define X509_cmp                     wolfSSL_X509_cmp
    #define X509_EXTENSION_get_object    wolfSSL_X509_EXTENSION_get_object
    #define X509_EXTENSION_get_critical  wolfSSL_X509_EXTENSION_get_critical
    #define X509_EXTENSION_get_data      wolfSSL_X509_EXTENSION_get_data
    #define X509_EXTENSION_new           wolfSSL_X509_EXTENSION_new
    #define X509_EXTENSION_free          wolfSSL_X509_EXTENSION_free
    #define X509_EXTENSION_create_by_OBJ wolfSSL_X509_EXTENSION_create_by_OBJ
#endif

#define DSA_dup_DH                      wolfSSL_DSA_dup_DH
/* wolfSSL does not support DSA as the cert public key */
#define EVP_PKEY_get0_DSA               wolfSSL_EVP_PKEY_get0_DSA
#define EVP_PKEY_param_check            wolfSSL_EVP_PKEY_param_check
#define EVP_PKEY_CTX_free               wolfSSL_EVP_PKEY_CTX_free
#define DSA_bits                        wolfSSL_DSA_bits

#define i2d_X509_bio                    wolfSSL_i2d_X509_bio
#define i2d_X509_REQ_bio                wolfSSL_i2d_X509_REQ_bio
#define d2i_X509_bio                    wolfSSL_d2i_X509_bio
#define d2i_X509_REQ_bio                wolfSSL_d2i_X509_REQ_bio
#define d2i_X509_REQ_fp                 wolfSSL_d2i_X509_REQ_fp
#define d2i_X509_fp                     wolfSSL_d2i_X509_fp
#define i2d_X509                        wolfSSL_i2d_X509
#define d2i_X509                        wolfSSL_d2i_X509
#define PEM_read_bio_X509               wolfSSL_PEM_read_bio_X509
#define PEM_read_bio_X509_REQ           wolfSSL_PEM_read_bio_X509_REQ
#define PEM_read_X509_REQ               wolfSSL_PEM_read_X509_REQ
#define PEM_read_bio_X509_CRL           wolfSSL_PEM_read_bio_X509_CRL
#define PEM_read_bio_X509_AUX           wolfSSL_PEM_read_bio_X509_AUX
#define PEM_read_X509                   wolfSSL_PEM_read_X509
#define PEM_write_bio_X509              wolfSSL_PEM_write_bio_X509
#define PEM_write_bio_X509_AUX          wolfSSL_PEM_write_bio_X509_AUX
#define PEM_X509_INFO_read_bio          wolfSSL_PEM_X509_INFO_read_bio
#define PEM_X509_INFO_read              wolfSSL_PEM_X509_INFO_read
#define i2d_PrivateKey                  wolfSSL_i2d_PrivateKey
#define i2d_PublicKey                   wolfSSL_i2d_PublicKey

#define i2d_X509_REQ                    wolfSSL_i2d_X509_REQ
#define d2i_X509_REQ                    wolfSSL_d2i_X509_REQ
#define X509_REQ_new                    wolfSSL_X509_REQ_new
#define X509_REQ_free                   wolfSSL_X509_REQ_free
#define X509_REQ_sign                   wolfSSL_X509_REQ_sign
#define X509_REQ_sign_ctx               wolfSSL_X509_REQ_sign_ctx
#define X509_REQ_add_extensions         wolfSSL_X509_REQ_add_extensions
#define X509_REQ_add1_attr_by_NID       wolfSSL_X509_REQ_add1_attr_by_NID
#define X509_REQ_add1_attr_by_txt       wolfSSL_X509_REQ_add1_attr_by_txt
#define X509_REQ_get_attr_by_NID        wolfSSL_X509_REQ_get_attr_by_NID
#define X509_REQ_get_attr               wolfSSL_X509_REQ_get_attr
#define X509_ATTRIBUTE_get0_type        wolfSSL_X509_ATTRIBUTE_get0_type
#define X509_to_X509_REQ                wolfSSL_X509_to_X509_REQ
#define X509_REQ_set_subject_name       wolfSSL_X509_REQ_set_subject_name
#define X509_REQ_set_pubkey             wolfSSL_X509_REQ_set_pubkey
#define PEM_write_bio_X509_REQ          wolfSSL_PEM_write_bio_X509_REQ

#define X509_new                        wolfSSL_X509_new
#define X509_up_ref                     wolfSSL_X509_up_ref
#define X509_free                       wolfSSL_X509_free
#define X509_load_certificate_file      wolfSSL_X509_load_certificate_file
#define X509_digest                     wolfSSL_X509_digest
#define X509_pubkey_digest              wolfSSL_X509_pubkey_digest
#define X509_get_extension_flags        wolfSSL_X509_get_extension_flags
#define X509_get_key_usage              wolfSSL_X509_get_key_usage
#define X509_get_extended_key_usage     wolfSSL_X509_get_extended_key_usage
#define X509_get_ext_count              wolfSSL_X509_get_ext_count
#define X509_get_ext_d2i                wolfSSL_X509_get_ext_d2i
#define X509V3_EXT_i2d                  wolfSSL_X509V3_EXT_i2d
#define X509_get0_extensions            wolfSSL_X509_get0_extensions
#define X509_get_extensions             wolfSSL_X509_get0_extensions
#define X509_REQ_get_extensions         wolfSSL_X509_REQ_get_extensions
#define X509_get_ext                    wolfSSL_X509_get_ext
#define X509_get_ext_by_NID             wolfSSL_X509_get_ext_by_NID
#define X509_get_issuer_name            wolfSSL_X509_get_issuer_name
#define X509_issuer_name_hash           wolfSSL_X509_issuer_name_hash
#define X509_subject_name_hash          wolfSSL_X509_subject_name_hash
#define X509_get_subject_name(x)        wolfSSL_X509_get_subject_name((WOLFSSL_X509*)(x))
#define X509_REQ_get_subject_name       wolfSSL_X509_get_subject_name
#define X509_get_pubkey                 wolfSSL_X509_get_pubkey
#define X509_get0_pubkey                wolfSSL_X509_get_pubkey
#define X509_REQ_get_pubkey             wolfSSL_X509_get_pubkey
#define X509_get_notBefore              wolfSSL_X509_get_notBefore
#define X509_get0_notBefore             wolfSSL_X509_get_notBefore
#define X509_getm_notBefore             wolfSSL_X509_get_notBefore
#define X509_get_notAfter               wolfSSL_X509_get_notAfter
#define X509_get0_notAfter              wolfSSL_X509_get_notAfter
#define X509_getm_notAfter              wolfSSL_X509_get_notAfter
#define X509_get_serialNumber           wolfSSL_X509_get_serialNumber
#define X509_get0_pubkey_bitstr         wolfSSL_X509_get0_pubkey_bitstr
#define X509_get_ex_new_index           wolfSSL_X509_get_ex_new_index
#define X509_get_ex_data                wolfSSL_X509_get_ex_data
#define X509_set_ex_data                wolfSSL_X509_set_ex_data
#define X509_get1_ocsp                  wolfSSL_X509_get1_ocsp
#define X509_get_version                wolfSSL_X509_get_version
#define X509_get_signature_nid          wolfSSL_X509_get_signature_nid
#define X509_set_subject_name           wolfSSL_X509_set_subject_name
#define X509_set_issuer_name            wolfSSL_X509_set_issuer_name
#define X509_set_pubkey                 wolfSSL_X509_set_pubkey
#define X509_set_notAfter               wolfSSL_X509_set_notAfter
#define X509_set_notBefore              wolfSSL_X509_set_notBefore
#define X509_set_serialNumber           wolfSSL_X509_set_serialNumber
#define X509_set_version                wolfSSL_X509_set_version
#define X509_REQ_set_version            wolfSSL_X509_set_version
#define X509_sign                       wolfSSL_X509_sign
#define X509_sign_ctx                   wolfSSL_X509_sign_ctx
#define X509_print                      wolfSSL_X509_print
#define X509_REQ_print                  wolfSSL_X509_print
#define X509_print_ex                   wolfSSL_X509_print_ex
#define X509_print_fp                   wolfSSL_X509_print_fp
#define X509_CRL_print                  wolfSSL_X509_CRL_print
#define X509_REQ_print_fp               wolfSSL_X509_print_fp
#define X509_signature_print            wolfSSL_X509_signature_print
#define X509_get0_signature             wolfSSL_X509_get0_signature
#define X509_verify_cert_error_string   wolfSSL_X509_verify_cert_error_string
#define X509_verify_cert                wolfSSL_X509_verify_cert
#define X509_verify                     wolfSSL_X509_verify
#define X509_REQ_verify                 wolfSSL_X509_REQ_verify
#define X509_check_private_key          wolfSSL_X509_check_private_key
#define X509_REQ_check_private_key      wolfSSL_X509_check_private_key
#define X509_check_ca                   wolfSSL_X509_check_ca
#define X509_check_host                 wolfSSL_X509_check_host
#define X509_check_email                wolfSSL_X509_check_email
#define X509_check_ip_asc               wolfSSL_X509_check_ip_asc
#define X509_email_free                 wolfSSL_X509_email_free
#define X509_check_issued               wolfSSL_X509_check_issued
#define X509_dup                        wolfSSL_X509_dup
#define X509_add_ext                    wolfSSL_X509_add_ext
#define X509_delete_ext                 wolfSSL_X509_delete_ext

#define X509_EXTENSION_get_object       wolfSSL_X509_EXTENSION_get_object
#define X509_EXTENSION_get_data         wolfSSL_X509_EXTENSION_get_data
#define X509_EXTENSION_dup              wolfSSL_X509_EXTENSION_dup

#define sk_X509_new                     wolfSSL_sk_X509_new
#define sk_X509_new_null                wolfSSL_sk_X509_new_null
#define sk_X509_num                     wolfSSL_sk_X509_num
#define sk_X509_value                   wolfSSL_sk_X509_value
#define sk_X509_shift                   wolfSSL_sk_X509_shift
#define sk_X509_push                    wolfSSL_sk_X509_push
#define sk_X509_pop                     wolfSSL_sk_X509_pop
#define sk_X509_pop_free                wolfSSL_sk_X509_pop_free
#define sk_X509_dup                     wolfSSL_sk_dup
#define sk_X509_free                    wolfSSL_sk_X509_free
#define X509_chain_up_ref               wolfSSL_X509_chain_up_ref

#define sk_X509_CRL_new                 wolfSSL_sk_X509_CRL_new
#define sk_X509_CRL_pop_free            wolfSSL_sk_X509_CRL_pop_free
#define sk_X509_CRL_free                wolfSSL_sk_X509_CRL_free
#define sk_X509_CRL_push                wolfSSL_sk_X509_CRL_push
#define sk_X509_CRL_value               wolfSSL_sk_X509_CRL_value
#define sk_X509_CRL_num                 wolfSSL_sk_X509_CRL_num

#define sk_X509_OBJECT_new              wolfSSL_sk_X509_OBJECT_new
#define sk_X509_OBJECT_free             wolfSSL_sk_X509_OBJECT_free
#define sk_X509_OBJECT_pop_free         wolfSSL_sk_X509_OBJECT_pop_free

#define sk_X509_EXTENSION_num           wolfSSL_sk_X509_EXTENSION_num
#define sk_X509_EXTENSION_value         wolfSSL_sk_X509_EXTENSION_value
#define sk_X509_EXTENSION_new_null      wolfSSL_sk_X509_EXTENSION_new_null
#define sk_X509_EXTENSION_pop_free      wolfSSL_sk_X509_EXTENSION_pop_free
#define sk_X509_EXTENSION_push          wolfSSL_sk_X509_EXTENSION_push

#define X509_INFO_new                   wolfSSL_X509_INFO_new
#define X509_INFO_free                  wolfSSL_X509_INFO_free

#define sk_X509_INFO_new_null           wolfSSL_sk_X509_INFO_new_null
#define sk_X509_INFO_num                wolfSSL_sk_X509_INFO_num
#define sk_X509_INFO_value              wolfSSL_sk_X509_INFO_value
#define sk_X509_INFO_push               wolfSSL_sk_X509_INFO_push
#define sk_X509_INFO_pop                wolfSSL_sk_X509_INFO_pop
#define sk_X509_INFO_pop_free           wolfSSL_sk_X509_INFO_pop_free
#define sk_X509_INFO_free               wolfSSL_sk_X509_INFO_free
#define sk_X509_INFO_shift              wolfSSL_sk_X509_INFO_pop

#define i2d_X509_NAME                   wolfSSL_i2d_X509_NAME
#define d2i_X509_NAME                   wolfSSL_d2i_X509_NAME
#define X509_NAME_new                   wolfSSL_X509_NAME_new
#define X509_NAME_free                  wolfSSL_X509_NAME_free
#define X509_NAME_dup                   wolfSSL_X509_NAME_dup
#define X509_NAME_get_sz                wolfSSL_X509_NAME_get_sz
#define X509_NAME_get_text_by_NID       wolfSSL_X509_NAME_get_text_by_NID
#define X509_NAME_get_index_by_OBJ      wolfSSL_X509_NAME_get_index_by_OBJ
#define X509_NAME_cmp                   wolfSSL_X509_NAME_cmp
#define X509_NAME_ENTRY_new             wolfSSL_X509_NAME_ENTRY_new
#define X509_NAME_ENTRY_free            wolfSSL_X509_NAME_ENTRY_free
#define X509_NAME_ENTRY_create_by_NID   wolfSSL_X509_NAME_ENTRY_create_by_NID
#define X509_NAME_ENTRY_create_by_txt   wolfSSL_X509_NAME_ENTRY_create_by_txt
#define X509_NAME_add_entry             wolfSSL_X509_NAME_add_entry
#define X509_NAME_add_entry_by_txt      wolfSSL_X509_NAME_add_entry_by_txt
#define X509_NAME_add_entry_by_NID      wolfSSL_X509_NAME_add_entry_by_NID
#define X509_NAME_delete_entry          wolfSSL_X509_NAME_delete_entry
#define X509_NAME_hash                  wolfSSL_X509_NAME_hash
#define X509_NAME_oneline               wolfSSL_X509_NAME_oneline
#define X509_NAME_get_index_by_NID      wolfSSL_X509_NAME_get_index_by_NID
#define X509_NAME_print_ex              wolfSSL_X509_NAME_print_ex
#define X509_NAME_print_ex_fp           wolfSSL_X509_NAME_print_ex_fp
#define X509_NAME_digest                wolfSSL_X509_NAME_digest
#define X509_cmp_current_time           wolfSSL_X509_cmp_current_time
#define X509_cmp_time                   wolfSSL_X509_cmp_time
#define X509_time_adj                   wolfSSL_X509_time_adj
#define X509_time_adj_ex                wolfSSL_X509_time_adj_ex
#define X509_gmtime_adj                 wolfSSL_X509_gmtime_adj

#define sk_ACCESS_DESCRIPTION_num       wolfSSL_sk_ACCESS_DESCRIPTION_num
#define sk_ACCESS_DESCRIPTION_value     wolfSSL_sk_ACCESS_DESCRIPTION_value

#define sk_X509_NAME_new                wolfSSL_sk_X509_NAME_new
#define sk_X509_NAME_new_null()         wolfSSL_sk_X509_NAME_new(NULL)
#define sk_X509_NAME_push               wolfSSL_sk_X509_NAME_push
#define sk_X509_NAME_find               wolfSSL_sk_X509_NAME_find
#define sk_X509_NAME_set_cmp_func       wolfSSL_sk_X509_NAME_set_cmp_func
#define sk_X509_NAME_num                wolfSSL_sk_X509_NAME_num
#define sk_X509_NAME_value              wolfSSL_sk_X509_NAME_value
#define sk_X509_NAME_pop                wolfSSL_sk_X509_NAME_pop
#define sk_X509_NAME_pop_free           wolfSSL_sk_X509_NAME_pop_free
#define sk_X509_NAME_free               wolfSSL_sk_X509_NAME_free

typedef WOLFSSL_X509_NAME_ENTRY X509_NAME_ENTRY;

#define X509_NAME_entry_count           wolfSSL_X509_NAME_entry_count
#define X509_NAME_get_entry             wolfSSL_X509_NAME_get_entry
#define X509_NAME_ENTRY_get_object      wolfSSL_X509_NAME_ENTRY_get_object
#define X509_NAME_ENTRY_get_data        wolfSSL_X509_NAME_ENTRY_get_data
#define X509_NAME_ENTRY_get_object      wolfSSL_X509_NAME_ENTRY_get_object

#define sk_X509_NAME_ENTRY_new          wolfSSL_sk_X509_NAME_ENTRY_new
#define sk_X509_NAME_ENTRY_push         wolfSSL_sk_X509_NAME_ENTRY_push
#define sk_X509_NAME_ENTRY_num          wolfSSL_sk_X509_NAME_ENTRY_num
#define sk_X509_NAME_ENTRY_value        wolfSSL_sk_X509_NAME_ENTRY_value
#define sk_X509_NAME_ENTRY_free         wolfSSL_sk_X509_NAME_ENTRY_free

#define X509_V_FLAG_CRL_CHECK     WOLFSSL_CRL_CHECK
#define X509_V_FLAG_CRL_CHECK_ALL WOLFSSL_CRL_CHECKALL

#define X509_V_FLAG_USE_CHECK_TIME WOLFSSL_USE_CHECK_TIME
#define X509_V_FLAG_NO_CHECK_TIME  WOLFSSL_NO_CHECK_TIME
#define X509_CHECK_FLAG_ALWAYS_CHECK_SUBJECT WOLFSSL_ALWAYS_CHECK_SUBJECT
#define X509_CHECK_FLAG_NO_WILDCARDS         WOLFSSL_NO_WILDCARDS
#define X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS WOLFSSL_NO_PARTIAL_WILDCARDS

#define X509_VP_FLAG_DEFAULT        WOLFSSL_VPARAM_DEFAULT
#define X509_VP_FLAG_OVERWRITE      WOLFSSL_VPARAM_OVERWRITE
#define X509_VP_FLAG_RESET_FLAGS    WOLFSSL_VPARAM_RESET_FLAGS
#define X509_VP_FLAG_LOCKED         WOLFSSL_VPARAM_LOCKED
#define X509_VP_FLAG_ONCE           WOLFSSL_VPARAM_ONCE

#define X509_STORE_CTX_get_current_cert wolfSSL_X509_STORE_CTX_get_current_cert
#define X509_STORE_CTX_set_verify_cb    wolfSSL_X509_STORE_CTX_set_verify_cb
#define X509_STORE_CTX_new              wolfSSL_X509_STORE_CTX_new
#define X509_STORE_CTX_free             wolfSSL_X509_STORE_CTX_free
#define X509_STORE_CTX_get_chain        wolfSSL_X509_STORE_CTX_get_chain
#define X509_STORE_CTX_get0_chain       wolfSSL_X509_STORE_CTX_get_chain
#define X509_STORE_CTX_get1_chain       wolfSSL_X509_STORE_CTX_get1_chain
#define X509_STORE_CTX_get0_parent_ctx  wolfSSL_X509_STORE_CTX_get0_parent_ctx
#define X509_STORE_CTX_get_error        wolfSSL_X509_STORE_CTX_get_error
#define X509_STORE_CTX_get_error_depth  wolfSSL_X509_STORE_CTX_get_error_depth
#define X509_STORE_CTX_init             wolfSSL_X509_STORE_CTX_init
#define X509_STORE_CTX_cleanup          wolfSSL_X509_STORE_CTX_cleanup
#define X509_STORE_CTX_set_error        wolfSSL_X509_STORE_CTX_set_error
#define X509_STORE_CTX_set_error_depth  wolfSSL_X509_STORE_CTX_set_error_depth
#define X509_STORE_CTX_get_ex_data      wolfSSL_X509_STORE_CTX_get_ex_data
#define X509_STORE_CTX_set_ex_data      wolfSSL_X509_STORE_CTX_set_ex_data
#define X509_STORE_CTX_set_depth        wolfSSL_X509_STORE_CTX_set_depth
#define X509_STORE_CTX_verify_cb        WOLFSSL_X509_STORE_CTX_verify_cb
#define X509_STORE_CTX_get0_current_issuer \
                                      wolfSSL_X509_STORE_CTX_get0_current_issuer
#define X509_STORE_CTX_get0_store       wolfSSL_X509_STORE_CTX_get0_store
#define X509_STORE_CTX_get0_cert        wolfSSL_X509_STORE_CTX_get0_cert
#define X509_STORE_CTX_trusted_stack    wolfSSL_X509_STORE_CTX_trusted_stack

#define X509_STORE_set_verify_cb(s, c) \
wolfSSL_X509_STORE_set_verify_cb((WOLFSSL_X509_STORE *)(s), (WOLFSSL_X509_STORE_CTX_verify_cb)(c))
#define X509_STORE_set_verify_cb_func(s, c) \
wolfSSL_X509_STORE_set_verify_cb((WOLFSSL_X509_STORE *)(s), (WOLFSSL_X509_STORE_CTX_verify_cb)(c))


#define X509_STORE_new                  wolfSSL_X509_STORE_new
#define X509_STORE_free                 wolfSSL_X509_STORE_free
#define X509_STORE_add_lookup           wolfSSL_X509_STORE_add_lookup
#define X509_STORE_add_cert             wolfSSL_X509_STORE_add_cert
#define X509_STORE_add_crl              wolfSSL_X509_STORE_add_crl
#define X509_STORE_set_flags            wolfSSL_X509_STORE_set_flags
#define X509_STORE_get1_certs           wolfSSL_X509_STORE_get1_certs
#define X509_STORE_get_by_subject       wolfSSL_X509_STORE_get_by_subject
#define X509_STORE_set_ex_data          wolfSSL_X509_STORE_set_ex_data
#define X509_STORE_get_ex_data          wolfSSL_X509_STORE_get_ex_data
#define X509_STORE_CTX_get1_issuer      wolfSSL_X509_STORE_CTX_get1_issuer
#define X509_STORE_CTX_set_time         wolfSSL_X509_STORE_CTX_set_time
#define X509_VERIFY_PARAM_new           wolfSSL_X509_VERIFY_PARAM_new
#define X509_VERIFY_PARAM_free          wolfSSL_X509_VERIFY_PARAM_free
#define X509_VERIFY_PARAM_set_flags     wolfSSL_X509_VERIFY_PARAM_set_flags
#define X509_VERIFY_PARAM_get_flags     wolfSSL_X509_VERIFY_PARAM_get_flags
#define X509_VERIFY_PARAM_clear_flags   wolfSSL_X509_VERIFY_PARAM_clear_flags
#define X509_VERIFY_PARAM_set_hostflags wolfSSL_X509_VERIFY_PARAM_set_hostflags
#define SSL_set1_host                   wolfSSL_set1_host
#define X509_VERIFY_PARAM_set1_host     wolfSSL_X509_VERIFY_PARAM_set1_host
#define X509_VERIFY_PARAM_set1_ip_asc   wolfSSL_X509_VERIFY_PARAM_set1_ip_asc
#define X509_VERIFY_PARAM_set1_ip       wolfSSL_X509_VERIFY_PARAM_set1_ip
#define X509_VERIFY_PARAM_set1          wolfSSL_X509_VERIFY_PARAM_set1
#define X509_STORE_load_locations       wolfSSL_X509_STORE_load_locations

#define X509_LOOKUP_add_dir             wolfSSL_X509_LOOKUP_add_dir
#define X509_LOOKUP_load_file           wolfSSL_X509_LOOKUP_load_file
#define X509_LOOKUP_hash_dir            wolfSSL_X509_LOOKUP_hash_dir
#define X509_LOOKUP_file                wolfSSL_X509_LOOKUP_file
#define X509_LOOKUP_ctrl                wolfSSL_X509_LOOKUP_ctrl

#define d2i_X509_CRL                    wolfSSL_d2i_X509_CRL
#define d2i_X509_CRL_fp                 wolfSSL_d2i_X509_CRL_fp
#define PEM_read_X509_CRL               wolfSSL_PEM_read_X509_CRL

#define X509_CRL_free                   wolfSSL_X509_CRL_free
#define X509_CRL_get_lastUpdate         wolfSSL_X509_CRL_get_lastUpdate
#define X509_CRL_get0_lastUpdate        wolfSSL_X509_CRL_get_lastUpdate
#define X509_CRL_get_nextUpdate         wolfSSL_X509_CRL_get_nextUpdate
#define X509_CRL_get0_nextUpdate        wolfSSL_X509_CRL_get_nextUpdate
#define X509_CRL_verify                 wolfSSL_X509_CRL_verify
#define X509_CRL_get_REVOKED            wolfSSL_X509_CRL_get_REVOKED
#define X509_CRL_get_issuer             wolfSSL_X509_CRL_get_issuer_name
#define X509_CRL_get_signature_nid      wolfSSL_X509_CRL_get_signature_nid
#define X509_CRL_get_version            wolfSSL_X509_CRL_version
#define X509_load_crl_file              wolfSSL_X509_load_crl_file

#define X509_get_X509_PUBKEY            wolfSSL_X509_get_X509_PUBKEY
#define X509_REQ_get_X509_PUBKEY        wolfSSL_X509_get_X509_PUBKEY
#define X509_get0_tbs_sigalg            wolfSSL_X509_get0_tbs_sigalg
#define X509_PUBKEY_get0_param          wolfSSL_X509_PUBKEY_get0_param
#define X509_PUBKEY_get                 wolfSSL_X509_PUBKEY_get
#define X509_PUBKEY_set                 wolfSSL_X509_PUBKEY_set
#define X509_ALGOR_get0                 wolfSSL_X509_ALGOR_get0
#define X509_ALGOR_set0                 wolfSSL_X509_ALGOR_set0

#define X509_ALGOR_new                  wolfSSL_X509_ALGOR_new
#define X509_ALGOR_free                 wolfSSL_X509_ALGOR_free
#define X509_PUBKEY_new                 wolfSSL_X509_PUBKEY_new
#define X509_PUBKEY_free                wolfSSL_X509_PUBKEY_free

#define sk_X509_REVOKED_num             wolfSSL_sk_X509_REVOKED_num
#define sk_X509_REVOKED_value           wolfSSL_sk_X509_REVOKED_value

#define X509_OBJECT_free_contents       wolfSSL_X509_OBJECT_free_contents

#define X509_OBJECT_get0_X509           wolfSSL_X509_OBJECT_get0_X509
#define X509_OBJECT_get0_X509_CRL       wolfSSL_X509_OBJECT_get0_X509_CRL

#define X509_REVOKED_get0_serialNumber   wolfSSL_X509_REVOKED_get0_serial_number
#define X509_REVOKED_get0_revocationDate wolfSSL_X509_REVOKED_get0_revocation_date

#define X509_check_purpose(...)         0

#define OCSP_parse_url                  wolfSSL_OCSP_parse_url

#define MD4_Init                        wolfSSL_MD4_Init
#define MD4_Update                      wolfSSL_MD4_Update
#define MD4_Final                       wolfSSL_MD4_Final

#define BIO_new                         wolfSSL_BIO_new
#define BIO_free                        wolfSSL_BIO_free
#define BIO_vfree                       wolfSSL_BIO_vfree
#define BIO_free_all                    wolfSSL_BIO_free_all
#define BIO_nread0                      wolfSSL_BIO_nread0
#define BIO_nread                       wolfSSL_BIO_nread
#define BIO_read                        wolfSSL_BIO_read
#define BIO_nwrite0                     wolfSSL_BIO_nwrite0
#define BIO_nwrite                      wolfSSL_BIO_nwrite
#define BIO_write                       wolfSSL_BIO_write
#define BIO_push                        wolfSSL_BIO_push
#define BIO_pop                         wolfSSL_BIO_pop
#define BIO_flush                       wolfSSL_BIO_flush
#define BIO_pending                     wolfSSL_BIO_pending

#define BIO_get_mem_data                wolfSSL_BIO_get_mem_data
#define BIO_new_mem_buf                 wolfSSL_BIO_new_mem_buf

#define BIO_f_buffer                    wolfSSL_BIO_f_buffer
#define BIO_set_write_buffer_size       wolfSSL_BIO_set_write_buffer_size
#define BIO_f_ssl                       wolfSSL_BIO_f_ssl
#define BIO_new_socket                  wolfSSL_BIO_new_socket
#define BIO_new_connect                 wolfSSL_BIO_new_connect
#define BIO_new_accept                  wolfSSL_BIO_new_accept
#define BIO_set_conn_port               wolfSSL_BIO_set_conn_port
#define BIO_do_connect                  wolfSSL_BIO_do_connect
#define BIO_do_accept                   wolfSSL_BIO_do_accept
#define BIO_do_handshake                wolfSSL_BIO_do_handshake
#define BIO_ssl_shutdown                wolfSSL_BIO_ssl_shutdown
#define SSL_set_bio                     wolfSSL_set_bio
#define BIO_method_type                 wolfSSL_BIO_method_type
#define BIO_set_ssl                     wolfSSL_BIO_set_ssl
#define BIO_get_ssl                     wolfSSL_BIO_get_ssl
#define BIO_new_ssl                     wolfSSL_BIO_new_ssl
#define BIO_new_ssl_connect             wolfSSL_BIO_new_ssl_connect
#define BIO_set_conn_hostname           wolfSSL_BIO_set_conn_hostname
#define BIO_eof                         wolfSSL_BIO_eof
#define BIO_set_ss                      wolfSSL_BIO_set_ss

#define BIO_f_md                        wolfSSL_BIO_f_md
#define BIO_get_md_ctx                  wolfSSL_BIO_get_md_ctx
#define BIO_s_mem                       wolfSSL_BIO_s_mem
#define BIO_f_base64                    wolfSSL_BIO_f_base64
#define BIO_set_flags                   wolfSSL_BIO_set_flags
#define BIO_set_nbio                    wolfSSL_BIO_set_nbio

#define SSLeay_add_ssl_algorithms       wolfSSL_add_all_algorithms
#define SSLeay_add_all_algorithms       wolfSSL_add_all_algorithms

#define RAND_screen                     wolfSSL_RAND_screen
#define RAND_file_name                  wolfSSL_RAND_file_name
#define RAND_write_file                 wolfSSL_RAND_write_file
#define RAND_load_file                  wolfSSL_RAND_load_file
#define RAND_egd                        wolfSSL_RAND_egd
#define RAND_seed                       wolfSSL_RAND_seed
#define RAND_cleanup                    wolfSSL_RAND_Cleanup
#define RAND_add                        wolfSSL_RAND_add
#define RAND_poll                       wolfSSL_RAND_poll
#define RAND_status                     wolfSSL_RAND_status
#define RAND_bytes                      wolfSSL_RAND_bytes
#define RAND_pseudo_bytes               wolfSSL_RAND_pseudo_bytes

#define COMP_zlib                       wolfSSL_COMP_zlib
#define COMP_rle                        wolfSSL_COMP_rle
#define SSL_COMP_add_compression_method wolfSSL_COMP_add_compression_method

#define SSL_get_ex_new_index            wolfSSL_get_ex_new_index
#define RSA_get_ex_new_index            wolfSSL_get_ex_new_index

#define ASN1_BIT_STRING_new             wolfSSL_ASN1_BIT_STRING_new
#define ASN1_BIT_STRING_free            wolfSSL_ASN1_BIT_STRING_free
#define ASN1_BIT_STRING_get_bit         wolfSSL_ASN1_BIT_STRING_get_bit
#define ASN1_BIT_STRING_set_bit         wolfSSL_ASN1_BIT_STRING_set_bit

#define sk_ASN1_OBJECT_free             wolfSSL_sk_ASN1_OBJECT_free

#ifndef NO_ASN_TIME
#define ASN1_TIME_new                   wolfSSL_ASN1_TIME_new
#define ASN1_UTCTIME_new                wolfSSL_ASN1_TIME_new
#define ASN1_TIME_free                  wolfSSL_ASN1_TIME_free
#define ASN1_UTCTIME_free               wolfSSL_ASN1_TIME_free
#define ASN1_TIME_adj                   wolfSSL_ASN1_TIME_adj
#define ASN1_TIME_print                 wolfSSL_ASN1_TIME_print
#define ASN1_TIME_to_string             wolfSSL_ASN1_TIME_to_string
#define ASN1_TIME_to_tm                 wolfSSL_ASN1_TIME_to_tm
#define ASN1_TIME_to_generalizedtime    wolfSSL_ASN1_TIME_to_generalizedtime
#endif
#define ASN1_TIME_set                   wolfSSL_ASN1_TIME_set
#define ASN1_TIME_set_string            wolfSSL_ASN1_TIME_set_string
#define ASN1_GENERALIZEDTIME_print      wolfSSL_ASN1_GENERALIZEDTIME_print
#define ASN1_GENERALIZEDTIME_free       wolfSSL_ASN1_GENERALIZEDTIME_free

#define ASN1_tag2str                    wolfSSL_ASN1_tag2str

#define a2i_ASN1_INTEGER                wolfSSL_a2i_ASN1_INTEGER
#define i2a_ASN1_INTEGER                wolfSSL_i2a_ASN1_INTEGER
#define i2c_ASN1_INTEGER                wolfSSL_i2c_ASN1_INTEGER
#define ASN1_INTEGER_new                wolfSSL_ASN1_INTEGER_new
#define ASN1_INTEGER_free               wolfSSL_ASN1_INTEGER_free
#define ASN1_INTEGER_cmp                wolfSSL_ASN1_INTEGER_cmp
#define ASN1_INTEGER_get                wolfSSL_ASN1_INTEGER_get
#define ASN1_INTEGER_set                wolfSSL_ASN1_INTEGER_set
#define ASN1_INTEGER_to_BN              wolfSSL_ASN1_INTEGER_to_BN
#define d2i_ASN1_INTEGER                wolfSSL_d2i_ASN1_INTEGER
#define i2d_ASN1_INTEGER                wolfSSL_i2d_ASN1_INTEGER

#define i2a_ASN1_OBJECT                 wolfSSL_i2a_ASN1_OBJECT
#define i2d_ASN1_OBJECT                 wolfSSL_i2d_ASN1_OBJECT

#define ASN1_STRING_new                 wolfSSL_ASN1_STRING_new
#define ASN1_OCTET_STRING_new           wolfSSL_ASN1_STRING_new
#define ASN1_STRING_free                wolfSSL_ASN1_STRING_free
#define ASN1_STRING_cmp                 wolfSSL_ASN1_STRING_cmp
#define ASN1_OCTET_STRING_cmp           wolfSSL_ASN1_STRING_cmp
#define ASN1_STRING_data                wolfSSL_ASN1_STRING_data
#define ASN1_STRING_get0_data           wolfSSL_ASN1_STRING_get0_data
#define ASN1_STRING_length              wolfSSL_ASN1_STRING_length
#define ASN1_STRING_to_UTF8             wolfSSL_ASN1_STRING_to_UTF8
#define ASN1_UNIVERSALSTRING_to_string  wolfSSL_ASN1_UNIVERSALSTRING_to_string
#define ASN1_STRING_print_ex            wolfSSL_ASN1_STRING_print_ex
#define ASN1_STRING_print(x, y)         wolfSSL_ASN1_STRING_print ((WOLFSSL_BIO*)(x), (WOLFSSL_ASN1_STRING*)(y))
#define d2i_DISPLAYTEXT                 wolfSSL_d2i_DISPLAYTEXT
#ifndef NO_WOLFSSL_STUB
#define ASN1_STRING_set_default_mask_asc(...) 1
#endif

#define ASN1_OCTET_STRING               WOLFSSL_ASN1_STRING
#define ASN1_OCTET_STRING_new           wolfSSL_ASN1_STRING_new
#define ASN1_OCTET_STRING_free          wolfSSL_ASN1_STRING_free
#define ASN1_OCTET_STRING_set           wolfSSL_ASN1_STRING_set

#define ASN1_UTF8STRING                 WOLFSSL_ASN1_STRING
#define ASN1_UTF8STRING_new             wolfSSL_ASN1_STRING_new
#define ASN1_UTF8STRING_free            wolfSSL_ASN1_STRING_free
#define ASN1_UTF8STRING_set             wolfSSL_ASN1_STRING_set

#define ASN1_IA5STRING                 WOLFSSL_ASN1_STRING
#define ASN1_IA5STRING_new             wolfSSL_ASN1_STRING_new
#define ASN1_IA5STRING_free            wolfSSL_ASN1_STRING_free
#define ASN1_IA5STRING_set             wolfSSL_ASN1_STRING_set

#define ASN1_PRINTABLE_type(...)        V_ASN1_PRINTABLESTRING

#define ASN1_UTCTIME_pr                 wolfSSL_ASN1_UTCTIME_pr

#define ASN1_IA5STRING                  WOLFSSL_ASN1_STRING

#define ASN1_BOOLEAN                    WOLFSSL_ASN1_BOOLEAN

#define SSL_load_client_CA_file         wolfSSL_load_client_CA_file

#define SSL_CTX_get_client_CA_list      wolfSSL_CTX_get_client_CA_list
#define SSL_CTX_set_client_CA_list      wolfSSL_CTX_set_client_CA_list
#define SSL_CTX_set_client_cert_cb      wolfSSL_CTX_set_client_cert_cb
#define SSL_CTX_set_cert_store          wolfSSL_CTX_set_cert_store
#ifdef OPENSSL_ALL
#define SSL_CTX_set1_verify_cert_store  wolfSSL_CTX_set1_verify_cert_store
#endif
#define SSL_set0_verify_cert_store      wolfSSL_set0_verify_cert_store
#define SSL_set1_verify_cert_store      wolfSSL_set1_verify_cert_store
#define SSL_CTX_get_cert_store(x)       wolfSSL_CTX_get_cert_store ((WOLFSSL_CTX*) (x))
#define SSL_get_client_CA_list          wolfSSL_get_client_CA_list
#define SSL_set_client_CA_list          wolfSSL_set_client_CA_list
#define SSL_get_ex_data_X509_STORE_CTX_idx wolfSSL_get_ex_data_X509_STORE_CTX_idx
#define SSL_get_ex_data                 wolfSSL_get_ex_data

#define SSL_CTX_set_default_passwd_cb_userdata wolfSSL_CTX_set_default_passwd_cb_userdata
#define SSL_CTX_set_default_passwd_cb   wolfSSL_CTX_set_default_passwd_cb

#define SSL_CTX_set_timeout(ctx, to)    \
                                 wolfSSL_CTX_set_timeout(ctx, (unsigned int)(to))
#define SSL_CTX_set_info_callback       wolfSSL_CTX_set_info_callback
#define SSL_CTX_set_alpn_protos         wolfSSL_CTX_set_alpn_protos

#define SSL_CTX_keylog_cb_func          wolfSSL_CTX_keylog_cb_func
#define SSL_CTX_set_keylog_callback     wolfSSL_CTX_set_keylog_callback
#define SSL_CTX_get_keylog_callback     wolfSSL_CTX_get_keylog_callback

#define SSL_alert_type_string           wolfSSL_alert_type_string
#define SSL_alert_desc_string           wolfSSL_alert_desc_string
#define SSL_state_string                wolfSSL_state_string

#define RSA_free                        wolfSSL_RSA_free
#define RSA_generate_key                wolfSSL_RSA_generate_key
#define SSL_CTX_set_tmp_rsa_callback    wolfSSL_CTX_set_tmp_rsa_callback
#define RSA_print                       wolfSSL_RSA_print
#define RSA_print_fp                    wolfSSL_RSA_print_fp
#define RSA_bits                        wolfSSL_RSA_bits
#define RSA_up_ref                      wolfSSL_RSA_up_ref
#define RSA_padding_add_PKCS1_PSS       wolfSSL_RSA_padding_add_PKCS1_PSS
#define RSA_verify_PKCS1_PSS            wolfSSL_RSA_verify_PKCS1_PSS

#define PEM_def_callback                wolfSSL_PEM_def_callback

#define SSL_CTX_sess_accept             wolfSSL_CTX_sess_accept
#define SSL_CTX_sess_connect            wolfSSL_CTX_sess_connect
#define SSL_CTX_sess_accept_good        wolfSSL_CTX_sess_accept_good
#define SSL_CTX_sess_connect_good       wolfSSL_CTX_sess_connect_good
#define SSL_CTX_sess_accept_renegotiate wolfSSL_CTX_sess_accept_renegotiate
#define SSL_CTX_sess_connect_renegotiate wolfSSL_CTX_sess_connect_renegotiate
#define SSL_CTX_sess_hits               wolfSSL_CTX_sess_hits
#define SSL_CTX_sess_cb_hits            wolfSSL_CTX_sess_cb_hits
#define SSL_CTX_sess_cache_full         wolfSSL_CTX_sess_cache_full
#define SSL_CTX_sess_misses             wolfSSL_CTX_sess_misses
#define SSL_CTX_sess_timeouts           wolfSSL_CTX_sess_timeouts
#define SSL_CTX_sess_number             wolfSSL_CTX_sess_number
#define SSL_CTX_sess_get_cache_size     wolfSSL_CTX_sess_get_cache_size


#define SSL_DEFAULT_CIPHER_LIST WOLFSSL_DEFAULT_CIPHER_LIST

#define SSL_CTX_set_psk_client_callback wolfSSL_CTX_set_psk_client_callback
#define SSL_set_psk_client_callback     wolfSSL_set_psk_client_callback

#define SSL_get_psk_identity_hint       wolfSSL_get_psk_identity_hint
#define SSL_get_psk_identity            wolfSSL_get_psk_identity

#define SSL_CTX_use_psk_identity_hint   wolfSSL_CTX_use_psk_identity_hint
#define SSL_use_psk_identity_hint       wolfSSL_use_psk_identity_hint

#define SSL_CTX_set_psk_server_callback wolfSSL_CTX_set_psk_server_callback
#define SSL_set_psk_server_callback     wolfSSL_set_psk_server_callback

/* system file ints for ERR_put_error */
#define SYS_F_ACCEPT      WOLFSSL_SYS_ACCEPT
#define SYS_F_BIND        WOLFSSL_SYS_BIND
#define SYS_F_CONNECT     WOLFSSL_SYS_CONNECT
#define SYS_F_FOPEN       WOLFSSL_SYS_FOPEN
#define SYS_F_FREAD       WOLFSSL_SYS_FREAD
#define SYS_F_GETADDRINFO WOLFSSL_SYS_GETADDRINFO
#define SYS_F_GETSOCKOPT  WOLFSSL_SYS_GETSOCKOPT
#define SYS_F_GETSOCKNAME WOLFSSL_SYS_GETSOCKNAME
#define SYS_F_OPENDIR     WOLFSSL_SYS_OPENDIR
#define SYS_F_SETSOCKOPT  WOLFSSL_SYS_SETSOCKOPT
#define SYS_F_SOCKET      WOLFSSL_SYS_SOCKET
#define SYS_F_GETHOSTBYNAME  WOLFSSL_SYS_GETHOSTBYNAME
#define SYS_F_GETNAMEINFO    WOLFSSL_SYS_GETNAMEINFO
#define SYS_F_GETSERVBYNAME  WOLFSSL_SYS_GETSERVBYNAME
#define SYS_F_IOCTLSOCKET    WOLFSSL_SYS_IOCTLSOCKET
#define SYS_F_LISTEN         WOLFSSL_SYS_LISTEN

#define ERR_GET_LIB                     wolfSSL_ERR_GET_LIB
#define ERR_GET_REASON                  wolfSSL_ERR_GET_REASON

#define ERR_put_error                   wolfSSL_ERR_put_error
#define ERR_peek_error                  wolfSSL_ERR_peek_error
#define ERR_peek_errors_fp              wolfSSL_ERR_peek_errors_fp
#define ERR_peek_error_line_data        wolfSSL_ERR_peek_error_line_data
#define ERR_peek_last_error             wolfSSL_ERR_peek_last_error
#define ERR_peek_last_error_line        wolfSSL_ERR_peek_last_error_line
#define ERR_get_error_line              wolfSSL_ERR_get_error_line
#define ERR_get_error_line_data         wolfSSL_ERR_get_error_line_data
#define ERR_get_error                   wolfSSL_ERR_get_error
#define ERR_print_errors_fp(file)       wolfSSL_ERR_dump_errors_fp((file))
#define ERR_print_errors_cb             wolfSSL_ERR_print_errors_cb
#define ERR_print_errors                wolfSSL_ERR_print_errors
#define ERR_clear_error                 wolfSSL_ERR_clear_error
#define ERR_free_strings                wolfSSL_ERR_free_strings
#define ERR_remove_state                wolfSSL_ERR_remove_state
#define ERR_remove_thread_state         wolfSSL_ERR_remove_thread_state
#define ERR_error_string                wolfSSL_ERR_error_string
#define ERR_error_string_n              wolfSSL_ERR_error_string_n
#define ERR_reason_error_string         wolfSSL_ERR_reason_error_string
#define ERR_func_error_string           wolfSSL_ERR_func_error_string
#define ERR_lib_error_string            wolfSSL_ERR_lib_error_string
#define ERR_load_BIO_strings            wolfSSL_ERR_load_BIO_strings

#ifndef WOLFCRYPT_ONLY
#define PEMerr(func, reason)            wolfSSL_ERR_put_error(ERR_LIB_PEM, \
                                        (func), (reason), __FILE__, __LINE__)
#else
#define PEMerr(func, reason)            WOLFSSL_ERROR_LINE((reason), \
                                        NULL, __LINE__, __FILE__, NULL)
#endif
#ifndef WOLFCRYPT_ONLY
#define EVPerr(func, reason)            wolfSSL_ERR_put_error(ERR_LIB_EVP, \
                                        (func), (reason), __FILE__, __LINE__)
#else
#define EVPerr(func, reason)            WOLFSSL_ERROR_LINE((reason), \
                                        NULL, __LINE__, __FILE__, NULL)
#endif

#define SSLv23_server_method            wolfSSLv23_server_method
#define SSL_CTX_set_options             wolfSSL_CTX_set_options
#define SSL_CTX_get_options             wolfSSL_CTX_get_options
#define SSL_CTX_clear_options           wolfSSL_CTX_clear_options

#define SSL_CTX_check_private_key       wolfSSL_CTX_check_private_key
#define SSL_CTX_get0_privatekey         wolfSSL_CTX_get0_privatekey
#define SSL_check_private_key           wolfSSL_check_private_key

#define SSL_CTX_set_mode                wolfSSL_CTX_set_mode
#define SSL_CTX_clear_mode              wolfSSL_CTX_clear_mode
#define SSL_CTX_get_mode                wolfSSL_CTX_get_mode
#define SSL_CTX_set_default_read_ahead  wolfSSL_CTX_set_default_read_ahead

#define SSL_CTX_sess_set_cache_size     wolfSSL_CTX_sess_set_cache_size
#define SSL_CTX_set_default_verify_paths wolfSSL_CTX_set_default_verify_paths
#define X509_get_default_cert_file_env  wolfSSL_X509_get_default_cert_file_env
#define X509_get_default_cert_file      wolfSSL_X509_get_default_cert_file
#define X509_get_default_cert_dir_env   wolfSSL_X509_get_default_cert_dir_env
#define X509_get_default_cert_dir       wolfSSL_X509_get_default_cert_dir

#define SSL_CTX_set_session_id_context  wolfSSL_CTX_set_session_id_context
#define SSL_get_peer_certificate        wolfSSL_get_peer_certificate
#define SSL_get_peer_cert_chain         wolfSSL_get_peer_cert_chain

#define SSL_want                        wolfSSL_want
#define SSL_want_read                   wolfSSL_want_read
#define SSL_want_write                  wolfSSL_want_write

#define BIO_prf                         wolfSSL_BIO_prf

#define sk_num                          wolfSSL_sk_num
#define sk_ASN1_OBJECT_num              wolfSSL_sk_num
#define OPENSSL_sk_num                  wolfSSL_sk_num
#define sk_value                        wolfSSL_sk_value
#define sk_ASN1_OBJECT_value            wolfSSL_sk_value
#define OPENSSL_sk_value                wolfSSL_sk_value

#define d2i_PKCS12_bio                  wolfSSL_d2i_PKCS12_bio
#define d2i_PKCS12_fp                   wolfSSL_d2i_PKCS12_fp
#define i2d_PKCS12_bio                  wolfSSL_i2d_PKCS12_bio

#define d2i_RSAPublicKey                wolfSSL_d2i_RSAPublicKey
#define d2i_RSAPrivateKey               wolfSSL_d2i_RSAPrivateKey
#define i2d_RSAPrivateKey               wolfSSL_i2d_RSAPrivateKey
#define i2d_RSAPublicKey                wolfSSL_i2d_RSAPublicKey

#define SSL_CTX_get_ex_data             wolfSSL_CTX_get_ex_data
#define SSL_CTX_set_ex_data             wolfSSL_CTX_set_ex_data
#define SSL_CTX_sess_set_get_cb         wolfSSL_CTX_sess_set_get_cb
#define SSL_CTX_sess_set_new_cb         wolfSSL_CTX_sess_set_new_cb
#define SSL_CTX_sess_set_remove_cb      wolfSSL_CTX_sess_set_remove_cb

#define i2d_SSL_SESSION                 wolfSSL_i2d_SSL_SESSION
#define d2i_SSL_SESSION                 wolfSSL_d2i_SSL_SESSION
#define SSL_SESSION_has_ticket          wolfSSL_SESSION_has_ticket
#define SSL_SESSION_get_ticket_lifetime_hint \
                                        wolfSSL_SESSION_get_ticket_lifetime_hint
#define SSL_SESSION_set_timeout         wolfSSL_SSL_SESSION_set_timeout
#define SSL_SESSION_get_timeout         wolfSSL_SESSION_get_timeout
#define SSL_SESSION_set_time            wolfSSL_SESSION_set_time
#define SSL_SESSION_get_time            wolfSSL_SESSION_get_time

#define SSL_CTX_get_ex_new_index        wolfSSL_CTX_get_ex_new_index
#define PEM_read                        wolfSSL_PEM_read
#define PEM_write                       wolfSSL_PEM_write
#define PEM_get_EVP_CIPHER_INFO         wolfSSL_PEM_get_EVP_CIPHER_INFO
#define PEM_do_header                   wolfSSL_PEM_do_header

/*#if OPENSSL_API_COMPAT < 0x10100000L*/
#define CONF_modules_free()             WC_DO_NOTHING
#define ENGINE_cleanup()                WC_DO_NOTHING
#define SSL_CTX_need_tmp_RSA(ctx)       0
#define SSL_CTX_set_tmp_rsa(ctx,rsa)    1
#define SSL_need_tmp_RSA(ssl)           0
#define SSL_set_tmp_rsa(ssl,rsa)        1
/*#endif*/

#define CONF_modules_unload(a)          WC_DO_NOTHING
#define CONF_get1_default_config_file   wolfSSL_CONF_get1_default_config_file

#define SSL_get_hit                     wolfSSL_session_reused

/* yassl had set the default to be 500 */
#define SSL_get_default_timeout(ctx)    500

#define DTLSv1_get_timeout(ssl, timeleft)   wolfSSL_DTLSv1_get_timeout((ssl), (WOLFSSL_TIMEVAL*)(timeleft))
#define DTLSv1_handle_timeout               wolfSSL_DTLSv1_handle_timeout
#define DTLSv1_set_initial_timeout_duration wolfSSL_DTLSv1_set_initial_timeout_duration

/* DTLS SRTP */
#ifdef WOLFSSL_SRTP
typedef WOLFSSL_SRTP_PROTECTION_PROFILE      SRTP_PROTECTION_PROFILE;
#endif
#define SSL_CTX_set_tlsext_use_srtp          wolfSSL_CTX_set_tlsext_use_srtp
#define SSL_set_tlsext_use_srtp              wolfSSL_set_tlsext_use_srtp
#define SSL_get_selected_srtp_profile        wolfSSL_get_selected_srtp_profile
#define SSL_get_srtp_profiles                wolfSSL_get_srtp_profiles

#ifndef NO_WOLFSSL_STUB
#define SSL_CTX_set_current_time_cb(ssl, cb) ({ (void)(ssl); (void)(cb); })
#endif

#define SSL_CTX_use_certificate         wolfSSL_CTX_use_certificate
#define SSL_CTX_add0_chain_cert         wolfSSL_CTX_add0_chain_cert
#define SSL_CTX_add1_chain_cert         wolfSSL_CTX_add1_chain_cert
#define SSL_add0_chain_cert             wolfSSL_add0_chain_cert
#define SSL_add1_chain_cert             wolfSSL_add1_chain_cert
#define SSL_CTX_use_PrivateKey          wolfSSL_CTX_use_PrivateKey
#define BIO_read_filename               wolfSSL_BIO_read_filename
#define SSL_CTX_set_verify_depth        wolfSSL_CTX_set_verify_depth
#define SSL_set_verify_depth            wolfSSL_set_verify_depth
#define SSL_get_app_data                wolfSSL_get_app_data
#define SSL_set_app_data                wolfSSL_set_app_data
#define SHA1                            wolfSSL_SHA1

#define SSL_dup_CA_list                 wolfSSL_dup_CA_list

#define sk_X509_NAME_find               wolfSSL_sk_X509_NAME_find

#define DHparams_dup                    wolfSSL_DH_dup
#define PEM_read_bio_DHparams           wolfSSL_PEM_read_bio_DHparams
#define PEM_read_DHparams               wolfSSL_PEM_read_DHparams
#define PEM_read_bio_DSAparams          wolfSSL_PEM_read_bio_DSAparams

#if defined(OPENSSL_ALL) || defined(WOLFSSL_HAPROXY)
#define SSL_get_rbio                    wolfSSL_SSL_get_rbio
#define SSL_get_wbio                    wolfSSL_SSL_get_wbio
#define SSL_do_handshake                wolfSSL_SSL_do_handshake
#define SSL_get_ciphers(x)              wolfSSL_get_ciphers_compat(x)
#define SSL_SESSION_get_id              wolfSSL_SESSION_get_id
#define SSL_get_cipher_bits(s,np)       \
                          wolfSSL_CIPHER_get_bits(SSL_get_current_cipher(s),np)
#define sk_SSL_CIPHER_num               wolfSSL_sk_SSL_CIPHER_num
#define sk_SSL_COMP_zero                wolfSSL_sk_SSL_COMP_zero
#define sk_SSL_CIPHER_value             wolfSSL_sk_SSL_CIPHER_value
#endif /* OPENSSL_ALL || WOLFSSL_HAPROXY */
#define sk_SSL_CIPHER_dup               wolfSSL_sk_dup
#define sk_SSL_CIPHER_free              wolfSSL_sk_SSL_CIPHER_free
#define sk_SSL_CIPHER_find              wolfSSL_sk_SSL_CIPHER_find

#if defined(OPENSSL_ALL) || defined(WOLFSSL_ASIO) || defined(WOLFSSL_HAPROXY) \
    || defined(WOLFSSL_NGINX)
#include <wolfssl/openssl/pem.h>

#define SSL_CTRL_CHAIN       88
#define SSL_R_SHORT_READ     10
#define ERR_R_PEM_LIB        9
#define SSL_CTRL_MODE        33

#define SSL_CTRL_CLEAR_EXTRA_CHAIN_CERTS        83

#define SSL_CTX_clear_chain_certs(ctx) SSL_CTX_set0_chain(ctx,NULL)
#define d2i_RSAPrivateKey_bio           wolfSSL_d2i_RSAPrivateKey_bio
#define SSL_CTX_use_RSAPrivateKey       wolfSSL_CTX_use_RSAPrivateKey
#define d2i_PrivateKey_bio              wolfSSL_d2i_PrivateKey_bio
#define BIO_new_bio_pair                wolfSSL_BIO_new_bio_pair
#define SSL_get_verify_callback         wolfSSL_get_verify_callback

#define SSL_set_mode(ssl,op)         wolfSSL_ctrl((ssl),SSL_CTRL_MODE,(op),NULL)

#define SSL_CTX_use_certificate_ASN1    wolfSSL_CTX_use_certificate_ASN1
#define SSL_CTX_set0_chain(ctx,sk) \
                             wolfSSL_CTX_ctrl(ctx,SSL_CTRL_CHAIN,0,(char *)(sk))
#define SSL_CTX_get_app_data(ctx)       wolfSSL_CTX_get_ex_data(ctx,0)
#define SSL_CTX_set_app_data(ctx,arg)   wolfSSL_CTX_set_ex_data(ctx,0, \
                                                                  (char *)(arg))
#endif /* OPENSSL_ALL || WOLFSSL_ASIO || WOLFSSL_HAPROXY */

#define SSL_CTX_set_tmp_dh              wolfSSL_CTX_set_tmp_dh

#define TLSEXT_STATUSTYPE_ocsp  1

#define TLSEXT_max_fragment_length_512   WOLFSSL_MFL_2_9
#define TLSEXT_max_fragment_length_1024  WOLFSSL_MFL_2_10
#define TLSEXT_max_fragment_length_2048  WOLFSSL_MFL_2_11
#define TLSEXT_max_fragment_length_4096  WOLFSSL_MFL_2_12

#define SSL_set_options                 wolfSSL_set_options
#define SSL_get_options                 wolfSSL_get_options
#define SSL_clear_options               wolfSSL_clear_options
#define SSL_set_tmp_dh                  wolfSSL_set_tmp_dh
#define SSL_clear_num_renegotiations    wolfSSL_clear_num_renegotiations
#define SSL_total_renegotiations        wolfSSL_total_renegotiations
#define SSL_num_renegotiations          wolfSSL_num_renegotiations
#define SSL_renegotiate                 wolfSSL_Rehandshake
#define SSL_get_secure_renegotiation_support wolfSSL_SSL_get_secure_renegotiation_support
#define SSL_renegotiate_pending         wolfSSL_SSL_renegotiate_pending
#define SSL_set_tlsext_debug_arg        wolfSSL_set_tlsext_debug_arg
#define SSL_set_tlsext_status_type      wolfSSL_set_tlsext_status_type
#define SSL_get_tlsext_status_type      wolfSSL_get_tlsext_status_type
#define SSL_set_tlsext_status_exts      wolfSSL_set_tlsext_status_exts
#define SSL_get_tlsext_status_ids       wolfSSL_get_tlsext_status_ids
#define SSL_set_tlsext_status_ids       wolfSSL_set_tlsext_status_ids
#define SSL_get_tlsext_status_ocsp_res  wolfSSL_get_tlsext_status_ocsp_resp
#define SSL_set_tlsext_status_ocsp_res  wolfSSL_set_tlsext_status_ocsp_resp
#define SSL_set_tlsext_status_ocsp_resp  wolfSSL_set_tlsext_status_ocsp_resp
#define SSL_get_tlsext_status_ocsp_resp  wolfSSL_get_tlsext_status_ocsp_resp
#define SSL_set_tlsext_max_fragment_length wolfSSL_set_tlsext_max_fragment_length

#define SSL_CTX_add_extra_chain_cert    wolfSSL_CTX_add_extra_chain_cert
#define SSL_get_read_ahead              wolfSSL_get_read_ahead
#define SSL_set_read_ahead              wolfSSL_set_read_ahead
#define SSL_CTX_get_read_ahead          wolfSSL_CTX_get_read_ahead
#define SSL_CTX_set_read_ahead          wolfSSL_CTX_set_read_ahead
#define SSL_CTX_set_tlsext_status_arg   wolfSSL_CTX_set_tlsext_status_arg
#define SSL_CTX_set_tlsext_opaque_prf_input_callback_arg \
                            wolfSSL_CTX_set_tlsext_opaque_prf_input_callback_arg
#define SSL_CTX_set_tlsext_max_fragment_length \
                            wolfSSL_CTX_set_tlsext_max_fragment_length
#define SSL_get_server_random           wolfSSL_get_server_random
#define SSL_get_server_tmp_key          wolfSSL_get_server_tmp_key

#define SSL_CTX_set_min_proto_version   wolfSSL_CTX_set_min_proto_version
#define SSL_CTX_set_max_proto_version   wolfSSL_CTX_set_max_proto_version
#define SSL_set_min_proto_version       wolfSSL_set_min_proto_version
#define SSL_set_max_proto_version       wolfSSL_set_max_proto_version
#define SSL_CTX_get_min_proto_version   wolfSSL_CTX_get_min_proto_version
#define SSL_CTX_get_max_proto_version   wolfSSL_CTX_get_max_proto_version

#define SSL_get_tlsext_status_exts      wolfSSL_get_tlsext_status_exts

#define SSL_CTX_get_tlsext_ticket_keys  wolfSSL_CTX_get_tlsext_ticket_keys
#define SSL_CTX_set_tlsext_ticket_keys  wolfSSL_CTX_set_tlsext_ticket_keys
#define SSL_CTX_get_tlsext_status_cb    wolfSSL_CTX_get_tlsext_status_cb
#define SSL_CTX_set_tlsext_status_cb    wolfSSL_CTX_set_tlsext_status_cb
#define SSL_CTX_set_num_tickets         wolfSSL_CTX_set_num_tickets
#define SSL_CTX_get_num_tickets         wolfSSL_CTX_get_num_tickets

#define SSL_CTRL_CLEAR_NUM_RENEGOTIATIONS         11
#define SSL_CTRL_GET_TOTAL_RENEGOTIATIONS         12
#define SSL_CTRL_SET_TMP_DH                       3
#define SSL_CTRL_SET_TMP_ECDH                     4
#define SSL_CTRL_SET_SESS_CACHE_MODE              44
#define SSL_CTRL_SET_TLSEXT_DEBUG_ARG             57
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_TYPE       65
#define SSL_CTRL_GET_TLSEXT_STATUS_REQ_TYPE       651
#define SSL_CTRL_GET_TLSEXT_STATUS_REQ_EXTS       66
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_EXTS       67
#define SSL_CTRL_GET_TLSEXT_STATUS_REQ_IDS        68
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_IDS        69
#define SSL_CTRL_GET_TLSEXT_STATUS_REQ_OCSP_RESP  70
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_OCSP_RESP  71
#define SSL_CTRL_SET_GROUPS                       91
#define SSL_CTRL_GET_PEER_TMP_KEY                 109
#define SSL_CTRL_GET_SERVER_TMP_KEY               SSL_CTRL_GET_PEER_TMP_KEY
#define SSL_CTRL_GET_CHAIN_CERTS                  115
#define SSL_CTRL_SET_MIN_PROTO_VERSION            123
#define SSL_CTRL_SET_MAX_PROTO_VERSION            124
#define SSL_CTRL_GET_MIN_PROTO_VERSION            125
#define SSL_CTRL_GET_MAX_PROTO_VERSION            126
#define SSL_CTRL_SET_CURVES                       SSL_CTRL_SET_GROUPS

#define SSL_CTRL_EXTRA_CHAIN_CERT               14
#define SSL_CTRL_OPTIONS                        32

#define SSL_CTRL_SET_SESS_CACHE_SIZE            42
#define SSL_CTRL_GET_READ_AHEAD                 40
#define SSL_CTRL_SET_READ_AHEAD                 41

#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_CB       63
#define SSL_CTRL_SET_TLSEXT_STATUS_REQ_CB_ARG   64

#define SSL_CTRL_GET_EXTRA_CHAIN_CERTS          82
#define SSL_CTRL_GET_SESSION_REUSED             0

#define SSL_ctrl                        wolfSSL_ctrl
#define SSL_CTX_ctrl                    wolfSSL_CTX_ctrl
#define SSL_CTX_callback_ctrl           wolfSSL_CTX_callback_ctrl

#define SSL3_RANDOM_SIZE                32 /* same as RAN_LEN in internal.h */

/* Used as message callback types */
#define SSL3_RT_CHANGE_CIPHER_SPEC       20
#define SSL3_RT_ALERT                    21
#define SSL3_RT_HANDSHAKE                22
#define SSL3_RT_APPLICATION_DATA         23

#define OPENSSL_INIT_LOAD_SSL_STRINGS    0x00200000L
#define OPENSSL_INIT_LOAD_CRYPTO_STRINGS 0x00000002L

#define TLS_ANY_VERSION                  0x10000
#define DTLS1_2_VERSION                  0xFEFD
#define DTLS_MAX_VERSION                 DTLS1_2_VERSION

/* apache and lighty use SSL_CONF_FLAG_FILE to enable conf support */
#define SSL_CONF_FLAG_CMDLINE            WOLFSSL_CONF_FLAG_CMDLINE
#define SSL_CONF_FLAG_FILE               WOLFSSL_CONF_FLAG_FILE
#define SSL_CONF_FLAG_CERTIFICATE        WOLFSSL_CONF_FLAG_CERTIFICATE
#define SSL_CONF_FLAG_SERVER             WOLFSSL_CONF_FLAG_SERVER
#define SSL_CONF_FLAG_CLIENT             WOLFSSL_CONF_FLAG_CLIENT
#define SSL_CONF_FLAG_SHOW_ERRORS        WOLFSSL_CONF_FLAG_SHOW_ERRORS
#define SSL_CONF_TYPE_UNKNOWN            WOLFSSL_CONF_TYPE_UNKNOWN
#define SSL_CONF_TYPE_STRING             WOLFSSL_CONF_TYPE_STRING
#define SSL_CONF_TYPE_FILE               WOLFSSL_CONF_TYPE_FILE
#define SSL_CONF_TYPE_DIR                WOLFSSL_CONF_TYPE_DIR

#if defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || \
    defined(WOLFSSL_HAPROXY) || defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL)

#define SSL23_ST_SR_CLNT_HELLO_A        (0x210|0x2000)
#define SSL3_ST_SR_CLNT_HELLO_A         (0x110|0x2000)

#define SSL3_AD_BAD_CERTIFICATE          bad_certificate
#define SSL_AD_BAD_CERTIFICATE           SSL3_AD_BAD_CERTIFICATE
#define SSL_AD_UNRECOGNIZED_NAME         unrecognized_name
#define SSL_AD_NO_RENEGOTIATION          no_renegotiation
#define SSL_AD_INTERNAL_ERROR            80
#define SSL_AD_NO_APPLICATION_PROTOCOL   no_application_protocol
#define SSL_AD_MISSING_EXTENSION         missing_extension

#define ASN1_STRFLGS_ESC_MSB             4

#define SSL_MAX_MASTER_KEY_LENGTH       WOLFSSL_MAX_MASTER_KEY_LENGTH

#define SSL_alert_desc_string_long      wolfSSL_alert_desc_string_long
#define SSL_alert_type_string_long      wolfSSL_alert_type_string_long
#define SSL_CIPHER_get_bits             wolfSSL_CIPHER_get_bits
#define sk_GENERAL_NAME_num             wolfSSL_sk_GENERAL_NAME_num
#define SSL_CTX_get_options             wolfSSL_CTX_get_options

#define SSL_CTX_flush_sessions          wolfSSL_flush_sessions
#define SSL_CTX_add_session             wolfSSL_CTX_add_session
#define SSL_version(x)                  wolfSSL_version ((WOLFSSL*) (x))
#define SSL_get_state                   wolfSSL_get_state
#define SSL_state_string_long           wolfSSL_state_string_long

#define GENERAL_NAME_new                wolfSSL_GENERAL_NAME_new
#define GENERAL_NAME_free               wolfSSL_GENERAL_NAME_free
#define GENERAL_NAME_dup                wolfSSL_GENERAL_NAME_dup
#define GENERAL_NAME_print              wolfSSL_GENERAL_NAME_print
#define GENERAL_NAME_set0_othername     wolfSSL_GENERAL_NAME_set0_othername
#define GENERAL_NAME_set0_value         wolfSSL_GENERAL_NAME_set0_value
#define sk_GENERAL_NAME_push            wolfSSL_sk_GENERAL_NAME_push
#define sk_GENERAL_NAME_value           wolfSSL_sk_GENERAL_NAME_value

#define DIST_POINT_new                  wolfSSL_DIST_POINT_new
#define DIST_POINT_free                 wolfSSL_DIST_POINT_free
#define DIST_POINTS_free                wolfSSL_DIST_POINTS_free
#define CRL_DIST_POINTS_free(cdp)       wolfSSL_sk_DIST_POINT_pop_free((cdp), NULL)
#define sk_DIST_POINT_push              wolfSSL_sk_DIST_POINT_push
#define sk_DIST_POINT_value             wolfSSL_sk_DIST_POINT_value
#define sk_DIST_POINT_num               wolfSSL_sk_DIST_POINT_num
#define sk_DIST_POINT_pop_free          wolfSSL_sk_DIST_POINT_pop_free
#define sk_DIST_POINT_free              wolfSSL_sk_DIST_POINT_free

#define SSL_SESSION_get_ex_data         wolfSSL_SESSION_get_ex_data
#define SSL_SESSION_set_ex_data         wolfSSL_SESSION_set_ex_data
#define SSL_SESSION_get_ex_new_index    wolfSSL_SESSION_get_ex_new_index
#define SSL_SESSION_get_id              wolfSSL_SESSION_get_id
#define SSL_SESSION_get0_id_context     wolfSSL_SESSION_get0_id_context
#define SSL_SESSION_set1_id             wolfSSL_SESSION_set1_id
#define SSL_SESSION_set1_id_context     wolfSSL_SESSION_set1_id_context
#define SSL_SESSION_print               wolfSSL_SESSION_print
#define sk_GENERAL_NAME_pop_free        wolfSSL_sk_GENERAL_NAME_pop_free
#define sk_GENERAL_NAME_new             wolfSSL_sk_GENERAL_NAME_new
#define sk_GENERAL_NAME_free            wolfSSL_sk_GENERAL_NAME_free
#define sk_ASN1_OBJECT_pop_free         wolfSSL_sk_ASN1_OBJECT_pop_free
#define GENERAL_NAME_free               wolfSSL_GENERAL_NAME_free
#define GENERAL_NAMES_free              wolfSSL_GENERAL_NAMES_free

#define AUTHORITY_INFO_ACCESS_free      wolfSSL_AUTHORITY_INFO_ACCESS_free
#define AUTHORITY_INFO_ACCESS_pop_free  wolfSSL_AUTHORITY_INFO_ACCESS_pop_free
#define sk_ACCESS_DESCRIPTION_pop_free  wolfSSL_sk_ACCESS_DESCRIPTION_pop_free
#define sk_ACCESS_DESCRIPTION_free      wolfSSL_sk_ACCESS_DESCRIPTION_free
#define ACCESS_DESCRIPTION_free         wolfSSL_ACCESS_DESCRIPTION_free

/* Alert types, matched to AlertDescription enum in wolfssl/ssl.h */
#define SSL_AD_CLOSE_NOTIFY             close_notify
#define SSL_AD_UNEXPECTED_MESSAGE       unexpected_message
#define SSL_AD_BAD_RECORD_MAC           bad_record_mac
#define SSL_AD_RECORD_OVERFLOW          record_overflow
#define SSL_AD_DECOMPRESSION_FAILURE    decompression_failure
#define SSL_AD_HANDSHAKE_FAILURE        handshake_failure
#define SSL_AD_UNSUPPORTED_CERTIFICATE  unsupported_certificate
#define SSL_AD_CERTIFICATE_REVOKED      certificate_revoked
#define SSL_AD_CERTIFICATE_EXPIRED      certificate_expired
#define SSL_AD_CERTIFICATE_UNKNOWN      certificate_unknown
#define SSL_AD_ILLEGAL_PARAMETER        illegal_parameter
#define SSL_AD_UNKNOWN_CA               unknown_ca
#define SSL_AD_ACCESS_DENIED            access_denied
#define SSL_AD_DECODE_ERROR             decode_error
#define SSL_AD_DECRYPT_ERROR            decrypt_error
#ifdef WOLFSSL_MYSQL_COMPATIBLE
    #define SSL_AD_PROTOCOL_VERSION     wc_protocol_version
#else
    #define SSL_AD_PROTOCOL_VERSION     protocol_version
#endif
#define SSL_AD_INSUFFICIENT_SECURITY    insufficient_security
#define SSL_AD_USER_CANCELLED           user_canceled

#define SSL3_AL_FATAL                   2
#define SSL_TLSEXT_ERR_OK               0
#define SSL_TLSEXT_ERR_ALERT_WARNING    warning_return
#define SSL_TLSEXT_ERR_ALERT_FATAL      fatal_return
#define SSL_TLSEXT_ERR_NOACK            noack_return
#define TLSEXT_NAMETYPE_host_name       WOLFSSL_SNI_HOST_NAME

#define SSL_set_tlsext_host_name        wolfSSL_set_tlsext_host_name
#define SSL_get_servername              wolfSSL_get_servername
#define SSL_set_SSL_CTX                 wolfSSL_set_SSL_CTX
#define SSL_CTX_get_verify_callback     wolfSSL_CTX_get_verify_callback
#define SSL_CTX_set_tlsext_servername_callback wolfSSL_CTX_set_tlsext_servername_callback
#define SSL_CTX_set_tlsext_servername_arg wolfSSL_CTX_set_servername_arg

#define PSK_MAX_PSK_LEN                 256
#define PSK_MAX_IDENTITY_LEN            128
#define SSL_CTX_clear_options           wolfSSL_CTX_clear_options

#define SSL_CTX_add_server_custom_ext(...) 0

#define SSL_get0_verified_chain         wolfSSL_get0_verified_chain
#define X509_chain_up_ref               wolfSSL_X509_chain_up_ref

#endif /* HAVE_STUNNEL || WOLFSSL_NGINX */

#ifndef NO_WOLFSSL_STUB
#define b2i_PrivateKey_bio(...)         NULL
#define b2i_PVK_bio(...)                NULL
#endif

#define SSL_CTX_get_default_passwd_cb   wolfSSL_CTX_get_default_passwd_cb
#define SSL_CTX_get_default_passwd_cb_userdata wolfSSL_CTX_get_default_passwd_cb_userdata

#define SSL_CTX_set_msg_callback        wolfSSL_CTX_set_msg_callback
#define SSL_set_msg_callback            wolfSSL_set_msg_callback
#define SSL_CTX_set_msg_callback_arg    wolfSSL_CTX_set_msg_callback_arg
#define SSL_set_msg_callback_arg        wolfSSL_set_msg_callback_arg

#define SSL_CTX_clear_extra_chain_certs wolfSSL_CTX_clear_extra_chain_certs

#define SSL_certs_clear                 wolfSSL_certs_clear

/* Nginx uses this to determine if reached end of certs in file.
 * PEM_read_bio_X509 is called and the return error is lost.
 * The error that needs to be detected is: SSL_NO_PEM_HEADER.
 */
#define ERR_GET_FUNC(l) (int)((((unsigned long)(l)) >> 12L) & 0xfffL)

#define PEM_F_PEM_DEF_CALLBACK  100

/* Avoid wolfSSL error code range */
#define PEM_R_NO_START_LINE             (-MIN_CODE_E + 1)
#define PEM_R_PROBLEMS_GETTING_PASSWORD (-MIN_CODE_E + 2)
#define PEM_R_BAD_PASSWORD_READ         (-MIN_CODE_E + 3)
#define PEM_R_BAD_DECRYPT               (-MIN_CODE_E + 4)
#define ASN1_R_HEADER_TOO_LONG          (-MIN_CODE_E + 5)

#define ERR_LIB_RSA             4
#define ERR_LIB_EC              16
#define ERR_LIB_SSL             20
#define ERR_LIB_PKCS12          35
#define ERR_LIB_PEM             9
#define ERR_LIB_X509            10
#define ERR_LIB_EVP             11
#define ERR_LIB_ASN1            12
#define ERR_LIB_DIGEST          13
#define ERR_LIB_CIPHER          14
#define ERR_LIB_USER            15

#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) || \
    defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(OPENSSL_EXTRA) || \
    defined(HAVE_LIGHTY) || defined(HAVE_STUNNEL) || \
    defined(WOLFSSL_WPAS_SMALL)

#include <wolfssl/error-ssl.h>

#define OPENSSL_STRING    WOLFSSL_STRING
#define OPENSSL_CSTRING   WOLFSSL_STRING

#define TLSEXT_TYPE_application_layer_protocol_negotiation    16

#define OPENSSL_NPN_UNSUPPORTED 0
#define OPENSSL_NPN_NEGOTIATED  1
#define OPENSSL_NPN_NO_OVERLAP  2

/* Nginx checks these to see if the error was a handshake error. */
#define SSL_R_BAD_CHANGE_CIPHER_SPEC               LENGTH_ERROR
#define SSL_R_BLOCK_CIPHER_PAD_IS_WRONG            BUFFER_E
#define SSL_R_DIGEST_CHECK_FAILED                  VERIFY_MAC_ERROR
#define SSL_R_ERROR_IN_RECEIVED_CIPHER_LIST        SUITES_ERROR
#define SSL_R_EXCESSIVE_MESSAGE_SIZE               BUFFER_ERROR
#define SSL_R_LENGTH_MISMATCH                      LENGTH_ERROR
#define SSL_R_NO_CIPHERS_SPECIFIED                 SUITES_ERROR
#define SSL_R_NO_COMPRESSION_SPECIFIED             COMPRESSION_ERROR
#define SSL_R_NO_SHARED_CIPHER                     MATCH_SUITE_ERROR
#define SSL_R_RECORD_LENGTH_MISMATCH               HANDSHAKE_SIZE_ERROR
#define SSL_R_UNEXPECTED_MESSAGE                   OUT_OF_ORDER_E
#define SSL_R_UNEXPECTED_RECORD                    SANITY_MSG_E
#define SSL_R_UNKNOWN_ALERT_TYPE                   BUFFER_ERROR
#define SSL_R_BAD_DIGEST_LENGTH                    BUFFER_ERROR
#define SSL_R_BAD_PACKET_LENGTH                    BUFFER_ERROR
#define SSL_R_DATA_LENGTH_TOO_LONG                 BUFFER_ERROR
#define SSL_R_ENCRYPTED_LENGTH_TOO_LONG            BUFFER_ERROR
#define SSL_R_BAD_LENGTH                           BUFFER_ERROR
#define SSL_R_UNKNOWN_PROTOCOL                     VERSION_ERROR
#define SSL_R_WRONG_VERSION_NUMBER                 VERSION_ERROR
#define SSL_R_DECRYPTION_FAILED_OR_BAD_RECORD_MAC  ENCRYPT_ERROR
#define SSL_R_HTTPS_PROXY_REQUEST                  PARSE_ERROR
#define SSL_R_HTTP_REQUEST                         PARSE_ERROR
#define SSL_R_UNSUPPORTED_PROTOCOL                 VERSION_ERROR
#define SSL_R_CERTIFICATE_VERIFY_FAILED            VERIFY_CERT_ERROR
#define SSL_R_CERT_CB_ERROR                        CLIENT_CERT_CB_ERROR
#define SSL_R_NULL_SSL_METHOD_PASSED               BAD_FUNC_ARG
#define SSL_R_CCS_RECEIVED_EARLY                   OUT_OF_ORDER_E

#ifdef HAVE_SESSION_TICKET
#define SSL_CTRL_SET_TLSEXT_TICKET_KEY_CB 72
#endif

/* Some openssl consumers try to detect these options with ifdef, defining
 * here since we use an enum internally instead */
#define SSL_OP_SINGLE_DH_USE            WOLFSSL_OP_SINGLE_DH_USE
#define SSL_OP_SINGLE_ECDH_USE          WOLFSSL_OP_SINGLE_ECDH_USE
#define SSL_OP_CIPHER_SERVER_PREFERENCE WOLFSSL_OP_CIPHER_SERVER_PREFERENCE

#define OPENSSL_config                  wolfSSL_OPENSSL_config
#define OPENSSL_memdup                  wolfSSL_OPENSSL_memdup
#define OPENSSL_cleanse                 wolfSSL_OPENSSL_cleanse
#define SSL_CTX_get_timeout             wolfSSL_SSL_CTX_get_timeout
#define SSL_CTX_set_tmp_ecdh            wolfSSL_SSL_CTX_set_tmp_ecdh
#define SSL_CTX_remove_session          wolfSSL_SSL_CTX_remove_session
#define SSL_get_rbio                    wolfSSL_SSL_get_rbio
#define SSL_get_wbio                    wolfSSL_SSL_get_wbio
#define SSL_do_handshake                wolfSSL_SSL_do_handshake
#define SSL_in_init                     wolfSSL_SSL_in_init
#define SSL_in_connect_init             wolfSSL_SSL_in_connect_init
#define SSL_get0_session                wolfSSL_SSL_get0_session
#define SSL_CTX_set_tlsext_ticket_key_cb wolfSSL_CTX_set_tlsext_ticket_key_cb
#define SSL_CTX_set_tlsext_status_cb    wolfSSL_CTX_set_tlsext_status_cb
#define SSL_CTX_get_extra_chain_certs   wolfSSL_CTX_get_extra_chain_certs
#define SSL_CTX_get0_chain_certs        wolfSSL_CTX_get0_chain_certs
#define SSL_get0_chain_certs            wolfSSL_get0_chain_certs
#define sk_OPENSSL_STRING_num           wolfSSL_sk_WOLFSSL_STRING_num
#define sk_OPENSSL_STRING_value         wolfSSL_sk_WOLFSSL_STRING_value
#define sk_OPENSSL_PSTRING_num          wolfSSL_sk_WOLFSSL_STRING_num
#define sk_OPENSSL_PSTRING_value        (WOLFSSL_STRING*)wolfSSL_sk_WOLFSSL_STRING_value
#define sk_OPENSSL_STRING_free          wolfSSL_sk_free
#define SSL_get0_alpn_selected          wolfSSL_get0_alpn_selected
#define SSL_select_next_proto           wolfSSL_select_next_proto
#define SSL_CTX_set_alpn_select_cb      wolfSSL_CTX_set_alpn_select_cb
#define SSL_CTX_set_next_protos_advertised_cb  wolfSSL_CTX_set_next_protos_advertised_cb
#define SSL_CTX_set_next_proto_select_cb wolfSSL_CTX_set_next_proto_select_cb
#define SSL_set_alpn_protos             wolfSSL_set_alpn_protos
#define SSL_get0_next_proto_negotiated  wolfSSL_get0_next_proto_negotiated
#define SSL_is_server                   wolfSSL_is_server

#endif /* WOLFSSL_NGINX || WOLFSSL_HAPROXY || WOLFSSL_MYSQL_COMPATIBLE ||
          OPENSSL_ALL || HAVE_LIGHTY */

#if (defined(OPENSSL_EXTRA) || defined(HAVE_CURL)) && defined(HAVE_ECC)
#define SSL_CTX_set1_curves_list        wolfSSL_CTX_set1_curves_list
#define SSL_set1_curves_list            wolfSSL_set1_curves_list
#endif

#ifdef OPENSSL_EXTRA
#define SSL_CTX_add_client_CA           wolfSSL_CTX_add_client_CA
#define SSL_CTX_set_srp_password        wolfSSL_CTX_set_srp_password
#define SSL_CTX_set_srp_username        wolfSSL_CTX_set_srp_username
#define SSL_CTX_set_srp_strength        wolfSSL_CTX_set_srp_strength
#define SSL_get_SSL_CTX                 wolfSSL_get_SSL_CTX
#define SSL_get0_param                  wolfSSL_get0_param
#define SSL_CTX_get0_param              wolfSSL_CTX_get0_param
#define SSL_CTX_set1_param              wolfSSL_CTX_set1_param
#define SSL_get_srp_username            wolfSSL_get_srp_username

#define ERR_NUM_ERRORS                  16
#define SN_pkcs9_emailAddress           "Email"
#define LN_pkcs9_emailAddress           "emailAddress"
#define NID_pkcs9_emailAddress          48
#define OBJ_pkcs9_emailAddress          1L,2L,840L,113539L,1L,9L,1L

#define LN_basic_constraints            "X509v3 Basic Constraints"
#define LN_key_usage                    "X509v3 Key Usage"
#define LN_subject_key_identifier       "X509v3 Subject Key Identifier"
#define LN_ext_key_usage                "X509v3 Extended Key Usage"

#define SSL_get_rbio                    wolfSSL_SSL_get_rbio
#define SSL_get_wbio                    wolfSSL_SSL_get_wbio
#define SSL_do_handshake                wolfSSL_SSL_do_handshake

#if defined(WOLFSSL_EARLY_DATA)
#define SSL_EARLY_DATA_NOT_SENT         WOLFSSL_EARLY_DATA_NOT_SENT
#define SSL_EARLY_DATA_REJECTED         WOLFSSL_EARLY_DATA_REJECTED
#define SSL_EARLY_DATA_ACCEPTED         WOLFSSL_EARLY_DATA_ACCEPTED

#define SSL_get_early_data_status       wolfSSL_get_early_data_status
#define SSL_set_max_early_data          wolfSSL_set_max_early_data
#define SSL_get_max_early_data          wolfSSL_get_max_early_data
#define SSL_CTX_set_max_early_data      wolfSSL_CTX_set_max_early_data
#define SSL_CTX_get_max_early_data      wolfSSL_CTX_get_max_early_data
#endif

#endif  /* OPENSSL_EXTRA */

/* cipher suites for compatibility */
#define TLS1_CK_ECDHE_RSA_WITH_AES_128_CBC_SHA            (0xc013)
#define TLS1_CK_ECDHE_RSA_WITH_AES_256_CBC_SHA            (0xc014)
#define TLS1_CK_ECDHE_RSA_WITH_AES_128_GCM_SHA256         (0xc02f)
#define TLS1_CK_ECDHE_RSA_WITH_CHACHA20_POLY1305_SHA256   (0xcca8)
#define TLS1_CK_ECDHE_ECDSA_WITH_AES_128_CBC_SHA          (0xc009)
#define TLS1_CK_ECDHE_ECDSA_WITH_AES_256_CBC_SHA          (0xc00a)
#define TLS1_CK_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256       (0xc02b)
#define TLS1_CK_ECDHE_ECDSA_WITH_CHACHA20_POLY1305_SHA256 (0xcca9)

#define X509_LU_NONE                    WOLFSSL_X509_LU_NONE
#define X509_LU_X509                    WOLFSSL_X509_LU_X509
#define X509_LU_CRL                     WOLFSSL_X509_LU_CRL

#define X509_STORE_get0_objects         wolfSSL_X509_STORE_get0_objects
#define sk_X509_OBJECT_num              wolfSSL_sk_X509_OBJECT_num
#define sk_X509_OBJECT_value            wolfSSL_sk_X509_OBJECT_value
#define sk_X509_OBJECT_delete           wolfSSL_sk_X509_OBJECT_delete
#define X509_OBJECT_new                 wolfSSL_X509_OBJECT_new
#define X509_OBJECT_free                wolfSSL_X509_OBJECT_free
#define X509_OBJECT_get_type            wolfSSL_X509_OBJECT_get_type
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
#define OpenSSL_version(x)              wolfSSL_OpenSSL_version(x)
#else
#define OpenSSL_version(x)              wolfSSL_OpenSSL_version()
#endif

#ifndef NO_WOLFSSL_STUB
#define OBJ_create_objects(...)         WC_DO_NOTHING
#define sk_SSL_COMP_free(...)           WC_DO_NOTHING
#endif

#define OBJ_dup                         wolfSSL_ASN1_OBJECT_dup

#define SSL_set_psk_use_session_callback    wolfSSL_set_psk_use_session_callback
#define SSL_SESSION_is_resumable    wolfSSL_SESSION_is_resumable
typedef WOLFSSL_CONF_CTX SSL_CONF_CTX;

#define SSL_CONF_CTX_new                wolfSSL_CONF_CTX_new
#define SSL_CONF_CTX_free               wolfSSL_CONF_CTX_free
#define SSL_CONF_CTX_set_ssl_ctx        wolfSSL_CONF_CTX_set_ssl_ctx
#define SSL_CONF_CTX_set_flags          wolfSSL_CONF_CTX_set_flags
#define SSL_CONF_CTX_finish             wolfSSL_CONF_CTX_finish
#define SSL_CONF_cmd                    wolfSSL_CONF_cmd
#define SSL_CONF_cmd_value_type         wolfSSL_CONF_cmd_value_type

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */


#ifdef WOLFSSL_QUIC

#include <wolfssl/quic.h>

/* Used by Chromium/QUIC - according to quictls/openssl fork */
#define X25519_PRIVATE_KEY_LEN          32
#define X25519_PUBLIC_VALUE_LEN         32

/* TLSv1.3 cipher ids as defined in RFC 8446, returned by
 * SSL_CIPHER_get_id(cipher)
 * used by QUIC implementations, such as HAProxy
 */
#define TLS1_3_CK_AES_128_GCM_SHA256       0x1301
#define TLS1_3_CK_AES_256_GCM_SHA384       0x1302
#define TLS1_3_CK_CHACHA20_POLY1305_SHA256 0x1303
#define TLS1_3_CK_AES_128_CCM_SHA256       0x1304
#define TLS1_3_CK_AES_128_CCM_8_SHA256     0x1305

#define SSL_R_MISSING_QUIC_TRANSPORT_PARAMETERS_EXTENSION   QUIC_TP_MISSING_E
#define SSL_R_WRONG_ENCRYPTION_LEVEL_RECEIVED               QUIC_WRONG_ENC_LEVEL

#define ssl_quic_method_st              wolfssl_quic_method_t
typedef WOLFSSL_QUIC_METHOD             SSL_QUIC_METHOD;

#define ssl_encryption_level_t          wolfssl_encryption_level_t
typedef WOLFSSL_ENCRYPTION_LEVEL        OSSL_ENCRYPTION_LEVEL;
#define ssl_encryption_initial          wolfssl_encryption_initial
#define ssl_encryption_early_data       wolfssl_encryption_early_data
#define ssl_encryption_handshake        wolfssl_encryption_handshake
#define ssl_encryption_application      wolfssl_encryption_application

#define SSL_CTX_set_quic_method         wolfSSL_CTX_set_quic_method
#define SSL_set_quic_method             wolfSSL_set_quic_method

#define SSL_set_quic_transport_params   wolfSSL_set_quic_transport_params
#define SSL_get_peer_quic_transport_params  wolfSSL_get_peer_quic_transport_params

#define SSL_quic_max_handshake_flight_len   wolfSSL_quic_max_handshake_flight_len
#define SSL_quic_read_level             wolfSSL_quic_read_level
#define SSL_quic_write_level            wolfSSL_quic_write_level
#define SSL_provide_quic_data           wolfSSL_provide_quic_data
#define SSL_process_quic_post_handshake     wolfSSL_process_quic_post_handshake

#define SSL_is_quic                     wolfSSL_is_quic

#define SSL_set_quic_transport_version      wolfSSL_set_quic_transport_version
#define SSL_get_quic_transport_version      wolfSSL_get_quic_transport_version
#define SSL_get_peer_quic_transport_version wolfSSL_get_peer_quic_transport_version

#define SSL_set_quic_early_data_enabled     wolfSSL_set_quic_early_data_enabled

/* BoringSSL API - according to quictls/openssl fork */
#define SSL_set_quic_use_legacy_codepoint   wolfSSL_set_quic_use_legacy_codepoint

/* TODO: we do not have this in our QUIC api and HAProxy does not use it
int SSL_CIPHER_get_prf_nid(const SSL_CIPHER *c);
*/

#endif /* WOLFSSL_QUIC */


#ifdef __cplusplus
    } /* extern "C" */
#endif

#endif /* !WOLFSSL_OPENSSL_H_ */
