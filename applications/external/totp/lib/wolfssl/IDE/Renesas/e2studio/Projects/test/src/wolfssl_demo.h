/* wolfssl_demo.h
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

#ifndef WOLFSSL_DEMO_H_
#define WOLFSSL_DEMO_H_

#define FREQ 10000 /* Hz */

/* Enable wolfcrypt test */
/* can be enabled with benchmark test */
/* #define CRYPT_TEST    */

/* Enable benchmark               */
/* can be enabled with cyrpt test */
/* #define BENCHMARK     */

/* Enable TLS client     */
/* cannot enable with other definition */
/* #define TLS_CLIENT */

/* Enable TLS server     */
/* cannot enable with other definition */
/* #define TLS_SERVER */

void wolfSSL_TLS_client_init();
void wolfSSL_TLS_client();
void wolfSSL_TLS_server_init();
void wolfSSL_TLS_server();

#endif /* WOLFSSL_DEMO_H_ */
