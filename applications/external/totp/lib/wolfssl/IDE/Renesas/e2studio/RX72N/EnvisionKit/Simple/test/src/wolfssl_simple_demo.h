/* wolfssl_simple_demo.h
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

#ifndef WOLFSSL_SIMPLE_DEMO_H_
#define WOLFSSL_SIMPLE_DEMO_H_

#define FREQ 10000 /* Hz */

/* Enable wolfcrypt test */
/* can be enabled with benchmark test */
#define CRYPT_TEST

/* Enable benchmark               */
/* can be enabled with cyrpt test */
/*#define BENCHMARK*/

/* Enable TLS client     */
/* cannot enable with other definition */
/* simplest tcp client*/
/*#define SIMPLE_TCP_CLIENT */
/* software TLS　client */
/* #define SIMPLE_TLS_CLIENT */
/* use TSIP　Acceleration */
/*#define SIMPLE_TLS_TSIP_CLIENT*/

/* simplest tcp server */
/*#define SIMPLE_TCP_SERVER*/
/* simple TLS server */
/* #define SIMPLE_TLS_SERVER */

void wolfSSL_TLS_client_init();
void wolfSSL_TLS_client();
void simple_tcp_client();
void wolfSSL_TLS_server_init();
void wolfSSL_TLS_server();
void simple_tcp_server();

#if defined(SIMPLE_TCP_CLIENT) || defined(SIMPLE_TLS_CLIENT) || \
    defined(SIMPLE_TCP_SERVER) || defined(SIMPLE_TLS_SERVER)
 #define min(x,y) ((x)<(y)?(x):(y))
 int isascii(const char *s);
#endif

#endif /* WOLFSSL_SIMPLE_DEMO_H_ */
