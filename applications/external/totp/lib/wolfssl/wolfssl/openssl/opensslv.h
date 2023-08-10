/* opensslv.h
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

/* opensslv.h compatibility */

#ifndef WOLFSSL_OPENSSLV_H_
#define WOLFSSL_OPENSSLV_H_

#include <wolfssl/wolfcrypt/settings.h>

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)

/* api version compatibility */
#if defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER == 0x009070dfL) ||\
    defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER == 0x0090810fL) ||\
    defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER == 0x10100000L) ||\
    defined(OPENSSL_VERSION_NUMBER) && (OPENSSL_VERSION_NUMBER == 0x10001040L)
     /* valid version */
#elif defined(WOLFSSL_APACHE_HTTPD) || defined(HAVE_LIBEST) || \
      defined(WOLFSSL_BIND) || defined(WOLFSSL_NGINX) || \
      defined(WOLFSSL_RSYSLOG) || defined(WOLFSSL_KRB) || defined(HAVE_STUNNEL)
    /* For Apache httpd, Use 1.1.0 compatibility */
     #define OPENSSL_VERSION_NUMBER 0x10100003L
#elif defined(WOLFSSL_QT) || defined(WOLFSSL_PYTHON) || defined(WOLFSSL_KRB)
    /* For Qt and Python 3.8.5 compatibility */
     #define OPENSSL_VERSION_NUMBER 0x10101000L
#elif defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_FFMPEG)
     #define OPENSSL_VERSION_NUMBER 0x1010000fL
#elif defined(OPENSSL_ALL) || defined(HAVE_LIGHTY) || \
    defined(WOLFSSL_NGINX) || defined(WOLFSSL_OPENSSH) || defined(WOLFSSL_OPENVPN)
     /* version number can be increased for Lighty after compatibility for ECDH
        is added */
     #define OPENSSL_VERSION_NUMBER 0x10001040L
#else
     #define OPENSSL_VERSION_NUMBER 0x0090810fL
#endif

#define OPENSSL_VERSION_TEXT             "wolfSSL " LIBWOLFSSL_VERSION_STRING
#define OPENSSL_VERSION                  0

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#endif /* header */
