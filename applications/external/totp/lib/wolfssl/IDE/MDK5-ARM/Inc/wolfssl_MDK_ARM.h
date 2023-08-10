/* wolfssl_MDK_ARM.h
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

/******************************************************************************/
/**   This file is for defining types, values for specific to KEIL-MDK-ARM.  **/
/******************************************************************************/
#ifndef WOLFSSL_KEIL_ARM_H
#define WOLFSSL_KEIL_ARM_H

#include <stdio.h>

#define SOCKET_T int

/*** #include <socket.h> ***/
#define  NUMBITSPERBYTE 8
#define FD_SETSIZE 10

typedef long fd_mask;
#define NFDBITS   (sizeof(fd_mask) * NUMBITSPERBYTE)  /* bits per mask */

typedef struct fd_set {
    fd_mask fds_bits[(FD_SETSIZE + NFDBITS - 1) / NFDBITS];
} fd_set;

/*** #include <sys/types.h> ***/
struct timeval {
   long tv_sec;     /* seconds      */
   long tv_usec;    /* microseconds */
};


#if defined(WOLFSSL_KEIL_TCP_NET)

#define SCK_EWOULDBLOCK     BSD_ERROR_WOULDBLOCK
#define SCK_ETIMEOUT        BSD_ERROR_TIMEOUT
#include "rl_net.h"

typedef int socklen_t ;

/* for avoiding conflict with KEIL-TCPnet BSD socket */
/* Bodies are in cyassl_KEIL_RL.c                    */
#if defined(HAVE_KEIL_RTX)
    #define sleep(t) os_dly_wait(t/1000+1) ;
#elif defined (WOLFSSL_CMSIS_RTOS)
    #define sleep(t)  osDelay(t/1000+1) ;
#endif

/* for avoiding conflicting with KEIL-TCPnet TCP socket */
/* Bodies are in test.h */
#define tcp_connect   wolfssl_tcp_connect
#define tcp_socket    wolfssl_tcp_soket
#define tcp_listen    wolfssl_tcp_listen
#define connect(a,b,c)    connect(a, (SOCKADDR *)(b), c)
#define SOCKADDR struct sockaddr

/** KEIL-RL TCPnet ****/
/* TCPnet BSD socket does not have following functions. */
extern char *inet_ntoa(struct in_addr in);
extern unsigned long inet_addr(const char *cp);
extern int select(int nfds, fd_set *readfds, fd_set *writefds,
                          fd_set *exceptfds, const struct timeval *timeout);

#endif /* WOLFSSL_KEIL_TCP_NET */

#endif /* WOLFSSL_KEIL_ARM_H */
