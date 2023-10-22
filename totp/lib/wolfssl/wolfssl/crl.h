/* crl.h
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



#ifndef WOLFSSL_CRL_H
#define WOLFSSL_CRL_H


#ifdef HAVE_CRL

#include <wolfssl/ssl.h>
#include <wolfssl/wolfcrypt/asn.h>

#ifdef __cplusplus
    extern "C" {
#endif

WOLFSSL_LOCAL int  InitCRL(WOLFSSL_CRL* crl, WOLFSSL_CERT_MANAGER* cm);
WOLFSSL_LOCAL void FreeCRL(WOLFSSL_CRL* crl, int dynamic);

WOLFSSL_LOCAL int  LoadCRL(WOLFSSL_CRL* crl, const char* path, int type,
                           int monitor);
WOLFSSL_LOCAL int  BufferLoadCRL(WOLFSSL_CRL* crl, const byte* buff, long sz,
                                 int type, int verify);
WOLFSSL_LOCAL int  CheckCertCRL(WOLFSSL_CRL* crl, DecodedCert* cert);


#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* HAVE_CRL */
#endif /* WOLFSSL_CRL_H */
