/* tls1.h
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


#ifndef WOLFSSL_OPENSSL_TLS1_H_
#define WOLFSSL_OPENSSL_TLS1_H_

#ifndef TLS1_VERSION
#define TLS1_VERSION                    0x0301
#endif

#ifndef TLS1_1_VERSION
#define TLS1_1_VERSION                  0x0302
#endif

#ifndef TLS1_2_VERSION
#define TLS1_2_VERSION                  0x0303
#endif

#ifndef TLS1_3_VERSION
#define TLS1_3_VERSION                  0x0304
#endif

#ifndef TLS_MAX_VERSION
#define TLS_MAX_VERSION                 TLS1_3_VERSION
#endif

#ifdef WOLFSSL_QUIC
/* from rfc9001 */
#define TLSEXT_TYPE_quic_transport_parameters_draft   0xffa5
#define TLSEXT_TYPE_quic_transport_parameters         0x0039
#endif

#endif /* WOLFSSL_OPENSSL_TLS1_H_ */
