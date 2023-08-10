/* wolfssl_thread_entry.h
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
#ifndef WOLFSSL_THREAD_ENTRY_H_
#define WOLFSSL_THREAD_ENTRY_H_

#include <wolfssl/certs_test.h>

extern uint8_t g_ether0_mac_address[6];

static const byte ucIPAddress[4]          = { 192, 168, 11, 241 };
static const byte ucNetMask[4]            = { 255, 255, 255, 0 };
static const byte ucGatewayAddress[4]     = { 192, 168, 11, 1 };
static const byte ucDNSServerAddress[4]   = { 192, 168, 11, 1 };

/* Server Cert and Key */
#define CERT_BUF        server_cert_der_1024
#define SIZEOF_CERT_BUF sizeof_server_cert_der_1024
#define KEY_BUF         server_key_der_1024
#define SIZEOF_KEY_BUF  sizeof_server_key_der_1024

/* Port That the wolfSSL Server connects to. */
#define DEFAULT_PORT 11111

#define FR_SOCKET_SUCCESS 0

#endif /* WOLFSSL_THREAD_ENTRY_H_ */
