/* x509.h
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

/* x509.h for openssl */

#ifndef WOLFSSL_OPENSSL_509_H_
#define WOLFSSL_OPENSSL_509_H_

#include <wolfssl/openssl/ssl.h>
#include <wolfssl/openssl/crypto.h>
#include <wolfssl/openssl/dh.h>
#include <wolfssl/openssl/ec.h>
#include <wolfssl/openssl/ecdsa.h>
#include <wolfssl/openssl/pkcs7.h>

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

/* wolfSSL_X509_print_ex flags */
#define X509_FLAG_COMPAT        (0UL)
#define X509_FLAG_NO_HEADER     (1UL << 0)
#define X509_FLAG_NO_VERSION    (1UL << 1)
#define X509_FLAG_NO_SERIAL     (1UL << 2)
#define X509_FLAG_NO_SIGNAME    (1UL << 3)
#define X509_FLAG_NO_ISSUER     (1UL << 4)
#define X509_FLAG_NO_VALIDITY   (1UL << 5)
#define X509_FLAG_NO_SUBJECT    (1UL << 6)
#define X509_FLAG_NO_PUBKEY     (1UL << 7)
#define X509_FLAG_NO_EXTENSIONS (1UL << 8)
#define X509_FLAG_NO_SIGDUMP    (1UL << 9)
#define X509_FLAG_NO_AUX        (1UL << 10)
#define X509_FLAG_NO_ATTRIBUTES (1UL << 11)
#define X509_FLAG_NO_IDS        (1UL << 12)

#define XN_FLAG_FN_SN           0
#define XN_FLAG_ONELINE         0
#define XN_FLAG_COMPAT          0
#define XN_FLAG_RFC2253         1
#define XN_FLAG_SEP_COMMA_PLUS  (1 << 16)
#define XN_FLAG_SEP_CPLUS_SPC   (2 << 16)
#define XN_FLAG_SEP_SPLUS_SPC   (3 << 16)
#define XN_FLAG_SEP_MULTILINE   (4 << 16)
#define XN_FLAG_SEP_MASK        (0xF << 16)
#define XN_FLAG_DN_REV          (1 << 20)
#define XN_FLAG_FN_LN           (1 << 21)
#define XN_FLAG_FN_OID          (2 << 21)
#define XN_FLAG_FN_NONE         (3 << 21)
#define XN_FLAG_FN_MASK         (3 << 21)
#define XN_FLAG_SPC_EQ          (1 << 23)
#define XN_FLAG_DUMP_UNKNOWN_FIELDS (1 << 24)
#define XN_FLAG_FN_ALIGN        (1 << 25)

#define XN_FLAG_MULTILINE       0xFFFF

/*
 * All of these aren't actually used in wolfSSL. Some are included to
 * satisfy OpenSSL compatibility consumers to prevent compilation errors.
 * The list was taken from
 * https://github.com/openssl/openssl/blob/master/include/openssl/x509_vfy.h.in
 * One requirement for HAProxy is that the values should be literal constants.
 */

#define X509_V_OK                                       0
#define X509_V_ERR_UNSPECIFIED                          1
#define X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT            2
#define X509_V_ERR_UNABLE_TO_GET_CRL                    3
#define X509_V_ERR_UNABLE_TO_DECRYPT_CERT_SIGNATURE     4
#define X509_V_ERR_UNABLE_TO_DECRYPT_CRL_SIGNATURE      5
#define X509_V_ERR_UNABLE_TO_DECODE_ISSUER_PUBLIC_KEY   6
#define X509_V_ERR_CERT_SIGNATURE_FAILURE               7
#define X509_V_ERR_CRL_SIGNATURE_FAILURE                8
#define X509_V_ERR_CERT_NOT_YET_VALID                   9
#define X509_V_ERR_CERT_HAS_EXPIRED                     10
#define X509_V_ERR_CRL_NOT_YET_VALID                    11
#define X509_V_ERR_CRL_HAS_EXPIRED                      12
#define X509_V_ERR_ERROR_IN_CERT_NOT_BEFORE_FIELD       13
#define X509_V_ERR_ERROR_IN_CERT_NOT_AFTER_FIELD        14
#define X509_V_ERR_ERROR_IN_CRL_LAST_UPDATE_FIELD       15
#define X509_V_ERR_ERROR_IN_CRL_NEXT_UPDATE_FIELD       16
#define X509_V_ERR_OUT_OF_MEM                           17
#define X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT          18
#define X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN            19
#define X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY    20
#define X509_V_ERR_UNABLE_TO_VERIFY_LEAF_SIGNATURE      21
#define X509_V_ERR_CERT_CHAIN_TOO_LONG                  22
#define X509_V_ERR_CERT_REVOKED                         23
#define X509_V_ERR_NO_ISSUER_PUBLIC_KEY                 24
#define X509_V_ERR_PATH_LENGTH_EXCEEDED                 25
#define X509_V_ERR_INVALID_PURPOSE                      26
#define X509_V_ERR_CERT_UNTRUSTED                       27
#define X509_V_ERR_CERT_REJECTED                        28

/* These are 'informational' when looking for issuer cert */
#define X509_V_ERR_SUBJECT_ISSUER_MISMATCH              29
#define X509_V_ERR_AKID_SKID_MISMATCH                   30
#define X509_V_ERR_AKID_ISSUER_SERIAL_MISMATCH          31
#define X509_V_ERR_KEYUSAGE_NO_CERTSIGN                 32
#define X509_V_ERR_UNABLE_TO_GET_CRL_ISSUER             33
#define X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION         34
#define X509_V_ERR_KEYUSAGE_NO_CRL_SIGN                 35
#define X509_V_ERR_UNHANDLED_CRITICAL_CRL_EXTENSION     36
#define X509_V_ERR_INVALID_NON_CA                       37
#define X509_V_ERR_PROXY_PATH_LENGTH_EXCEEDED           38
#define X509_V_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE        39
#define X509_V_ERR_PROXY_CERTIFICATES_NOT_ALLOWED       40
#define X509_V_ERR_INVALID_EXTENSION                    41
#define X509_V_ERR_INVALID_POLICY_EXTENSION             42
#define X509_V_ERR_NO_EXPLICIT_POLICY                   43
#define X509_V_ERR_DIFFERENT_CRL_SCOPE                  44
#define X509_V_ERR_UNSUPPORTED_EXTENSION_FEATURE        45
#define X509_V_ERR_UNNESTED_RESOURCE                    46
#define X509_V_ERR_PERMITTED_VIOLATION                  47
#define X509_V_ERR_EXCLUDED_VIOLATION                   48
#define X509_V_ERR_SUBTREE_MINMAX                       49
/* The application is not happy */
#define X509_V_ERR_APPLICATION_VERIFICATION             50
#define X509_V_ERR_UNSUPPORTED_CONSTRAINT_TYPE          51
#define X509_V_ERR_UNSUPPORTED_CONSTRAINT_SYNTAX        52
#define X509_V_ERR_UNSUPPORTED_NAME_SYNTAX              53
#define X509_V_ERR_CRL_PATH_VALIDATION_ERROR            54
/* Another issuer check debug option */
#define X509_V_ERR_PATH_LOOP                            55
/* Suite B mode algorithm violation */
#define X509_V_ERR_SUITE_B_INVALID_VERSION              56
#define X509_V_ERR_SUITE_B_INVALID_ALGORITHM            57
#define X509_V_ERR_SUITE_B_INVALID_CURVE                58
#define X509_V_ERR_SUITE_B_INVALID_SIGNATURE_ALGORITHM  59
#define X509_V_ERR_SUITE_B_LOS_NOT_ALLOWED              60
#define X509_V_ERR_SUITE_B_CANNOT_SIGN_P_384_WITH_P_256 61
/* Host, email and IP check errors */
#define X509_V_ERR_HOSTNAME_MISMATCH                    62
#define X509_V_ERR_EMAIL_MISMATCH                       63
#define X509_V_ERR_IP_ADDRESS_MISMATCH                  64
/* DANE TLSA errors */
#define X509_V_ERR_DANE_NO_MATCH                        65
/* security level errors */
#define X509_V_ERR_EE_KEY_TOO_SMALL                     66
#define X509_V_ERR_CA_KEY_TOO_SMALL                     67
#define X509_V_ERR_CA_MD_TOO_WEAK                       68
/* Caller error */
#define X509_V_ERR_INVALID_CALL                         69
/* Issuer lookup error */
#define X509_V_ERR_STORE_LOOKUP                         70
/* Certificate transparency */
#define X509_V_ERR_NO_VALID_SCTS                        71

#define X509_V_ERR_PROXY_SUBJECT_NAME_VIOLATION         72
/* OCSP status errors */
#define X509_V_ERR_OCSP_VERIFY_NEEDED                   73
#define X509_V_ERR_OCSP_VERIFY_FAILED                   74
#define X509_V_ERR_OCSP_CERT_UNKNOWN                    75

#define X509_V_ERR_UNSUPPORTED_SIGNATURE_ALGORITHM      76
#define X509_V_ERR_SIGNATURE_ALGORITHM_MISMATCH         77

/* Errors in case a check in X509_V_FLAG_X509_STRICT mode fails */
#define X509_V_ERR_SIGNATURE_ALGORITHM_INCONSISTENCY    78
#define X509_V_ERR_INVALID_CA                           79
#define X509_V_ERR_PATHLEN_INVALID_FOR_NON_CA           80
#define X509_V_ERR_PATHLEN_WITHOUT_KU_KEY_CERT_SIGN     81
#define X509_V_ERR_KU_KEY_CERT_SIGN_INVALID_FOR_NON_CA  82
#define X509_V_ERR_ISSUER_NAME_EMPTY                    83
#define X509_V_ERR_SUBJECT_NAME_EMPTY                   84
#define X509_V_ERR_MISSING_AUTHORITY_KEY_IDENTIFIER     85
#define X509_V_ERR_MISSING_SUBJECT_KEY_IDENTIFIER       86
#define X509_V_ERR_EMPTY_SUBJECT_ALT_NAME               87
#define X509_V_ERR_EMPTY_SUBJECT_SAN_NOT_CRITICAL       88
#define X509_V_ERR_CA_BCONS_NOT_CRITICAL                89
#define X509_V_ERR_AUTHORITY_KEY_IDENTIFIER_CRITICAL    90
#define X509_V_ERR_SUBJECT_KEY_IDENTIFIER_CRITICAL      91
#define X509_V_ERR_CA_CERT_MISSING_KEY_USAGE            92
#define X509_V_ERR_EXTENSIONS_REQUIRE_VERSION_3         93
#define X509_V_ERR_EC_KEY_EXPLICIT_PARAMS               94
#define X509_R_CERT_ALREADY_IN_HASH_TABLE               101

#define X509_EXTENSION_set_critical wolfSSL_X509_EXTENSION_set_critical
#define X509_EXTENSION_set_object   wolfSSL_X509_EXTENSION_set_object
#define X509_EXTENSION_set_data     wolfSSL_X509_EXTENSION_set_data

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#endif /* WOLFSSL_OPENSSL_509_H_ */
