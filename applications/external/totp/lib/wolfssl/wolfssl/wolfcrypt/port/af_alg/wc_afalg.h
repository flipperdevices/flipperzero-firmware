/* wc_afalg.h
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


#ifndef WOLFSSL_AFALG_H
#define WOLFSSL_AFALG_H

#include <wolfssl/wolfcrypt/types.h>

#include <sys/socket.h>
#include <linux/if_alg.h>
#include <linux/socket.h>

#define WC_SOCK_NOTSET -1

/* In some cases these flags are not set in AF_ALG header files.
 * Documentation provided at kernel.org/doc/html/v4.16/crypto/userspace-if.html
 * suggests using these values if not set */
#ifndef AF_ALG
    #define AF_ALG 38
#endif
#ifndef SOL_ALG
    #define SOL_ALG 279
#endif

WOLFSSL_LOCAL void wc_Afalg_SockAddr(struct sockaddr_alg* in, const char* type, const char* name);
WOLFSSL_LOCAL int wc_Afalg_Accept(struct sockaddr_alg* in, int inSz, int sock);
WOLFSSL_LOCAL int wc_Afalg_Socket(void);
WOLFSSL_LOCAL int wc_Afalg_CreateRead(int sock, const char* type, const char* name);
WOLFSSL_LOCAL int wc_Afalg_SetIv(struct cmsghdr* cmsg, byte* iv, word32 ivSz);
WOLFSSL_LOCAL int wc_Afalg_SetOp(struct cmsghdr* cmsg, int dir);
WOLFSSL_LOCAL int wc_Afalg_SetAad(struct cmsghdr* cmsg, word32 sz);

#endif /* WOLFSSL_AFALG_H */

