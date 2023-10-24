/* ocsp.h
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

/* ocsp.h for libcurl */

#ifndef WOLFSSL_OCSP_H_
#define WOLFSSL_OCSP_H_

#ifdef HAVE_OCSP
#include <wolfssl/ocsp.h>

#define OCSP_REQUEST              OcspRequest
#define OCSP_RESPONSE             OcspResponse
#define OCSP_BASICRESP            WOLFSSL_OCSP_BASICRESP
#define OCSP_SINGLERESP           WOLFSSL_OCSP_SINGLERESP
#define OCSP_CERTID               WOLFSSL_OCSP_CERTID
#define OCSP_ONEREQ               WOLFSSL_OCSP_ONEREQ

#define OCSP_REVOKED_STATUS_NOSTATUS     (-1)


#define OCSP_RESPONSE_STATUS_SUCCESSFUL  0
#define OCSP_RESPONSE_STATUS_TRYLATER    3

#define V_OCSP_CERTSTATUS_GOOD           0
#define V_OCSP_CERTSTATUS_REVOKED        1
#define V_OCSP_CERTSTATUS_UNKNOWN        2

#define OCSP_resp_find_status     wolfSSL_OCSP_resp_find_status
#define OCSP_cert_status_str      wolfSSL_OCSP_cert_status_str
#define OCSP_check_validity       wolfSSL_OCSP_check_validity

#define OCSP_CERTID_free          wolfSSL_OCSP_CERTID_free
#define OCSP_cert_to_id           wolfSSL_OCSP_cert_to_id

#define OCSP_BASICRESP_free       wolfSSL_OCSP_BASICRESP_free
#define OCSP_basic_verify         wolfSSL_OCSP_basic_verify

#define OCSP_RESPONSE_free        wolfSSL_OCSP_RESPONSE_free
#define d2i_OCSP_RESPONSE_bio     wolfSSL_d2i_OCSP_RESPONSE_bio
#define d2i_OCSP_RESPONSE         wolfSSL_d2i_OCSP_RESPONSE
#define i2d_OCSP_RESPONSE         wolfSSL_i2d_OCSP_RESPONSE
#define OCSP_response_status      wolfSSL_OCSP_response_status
#define OCSP_response_status_str  wolfSSL_OCSP_response_status_str
#define OCSP_response_get1_basic  wolfSSL_OCSP_response_get1_basic
#define OCSP_response_create      wolfSSL_OCSP_response_create

#define OCSP_REQUEST_new          wolfSSL_OCSP_REQUEST_new
#define OCSP_REQUEST_free         wolfSSL_OCSP_REQUEST_free
#define i2d_OCSP_REQUEST          wolfSSL_i2d_OCSP_REQUEST
#define OCSP_request_add0_id      wolfSSL_OCSP_request_add0_id
#define OCSP_request_add1_nonce   wolfSSL_OCSP_request_add1_nonce
#define OCSP_check_nonce          wolfSSL_OCSP_check_nonce
#define OCSP_id_get0_info         wolfSSL_OCSP_id_get0_info
#define OCSP_crl_reason_str       wolfSSL_OCSP_crl_reason_str
#define OCSP_REQUEST_add_ext      wolfSSL_OCSP_REQUEST_add_ext

#define OCSP_CERTID_dup           wolfSSL_OCSP_CERTID_dup

#define i2d_OCSP_REQUEST_bio      wolfSSL_i2d_OCSP_REQUEST_bio

#define i2d_OCSP_CERTID           wolfSSL_i2d_OCSP_CERTID
#define d2i_OCSP_CERTID           wolfSSL_d2i_OCSP_CERTID
#define OCSP_SINGLERESP_get0_id   wolfSSL_OCSP_SINGLERESP_get0_id
#define OCSP_id_cmp               wolfSSL_OCSP_id_cmp
#define OCSP_single_get0_status   wolfSSL_OCSP_single_get0_status
#define OCSP_resp_count           wolfSSL_OCSP_resp_count
#define OCSP_resp_get0            wolfSSL_OCSP_resp_get0

#endif /* HAVE_OCSP */

#endif /* WOLFSSL_OCSP_H_ */

