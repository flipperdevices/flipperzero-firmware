/* wolfssl_KEIL_RL.h
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
#ifndef WOLFSSL_KEIL_RL_H
#define WOLFSSL_KEIL_RL_H



#include <stdio.h>

/* Go to STDIN */
#define fgets(buff, sz, fd)   wolfssl_fgets(buff, sz, fd)
extern char * wolfssl_fgets ( char * str, int num, FILE * f ) ;

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

#if defined(WOLFSSL_MDK5)
#define SCK_EWOULDBLOCK     BSD_ERROR_WOULDBLOCK
#define SCK_ETIMEOUT        BSD_ERROR_TIMEOUT
#include "rl_net.h"
#endif

typedef int socklen_t ;

/* for avoiding conflict with KEIL-TCPnet BSD socket */
/* Bodies are in wolfssl_KEIL_RL.c                    */
#define connect(a,b,c)             wolfssl_connect(a,  (struct sockaddr* )(b), c)
#define accept              wolfssl_accept
#define recv                wolfssl_recv
#define send                wolfssl_send
#define sleep               wolfssl_sleep

/* for avoiding conflicting with KEIL-TCPnet TCP socket */
/* Bodies are in test.h */
#define tcp_connect wolfssl_tcp_connect
#define tcp_socket    wolfssl_tcp_soket
#define tcp_listen      wolfssl_tcp_listen
#define tcp_select     wolfssl_tcp_select

extern int wolfssl_connect(int sd,  const  struct sockaddr* sa, 	int sz) ;
extern int wolfssl_accept(int sd, struct sockaddr*addr, socklen_t *addrlen);
extern int wolfssl_recv(int sd, void *buf, size_t len, int flags);
extern int wolfssl_send(int sd, const void *buf, size_t len, int flags);
extern void wolfssl_sleep(int sec) ;
extern int wolfssl_tcp_select(int sd, int timeout) ;

/** KEIL-RL TCPnet ****/
/* TCPnet BSD socket does not have following functions. */
extern char *inet_ntoa(struct in_addr in);
extern unsigned long inet_addr(const char *cp);
extern int setsockopt(int sockfd, int level, int optname,
                                      const void *optval, socklen_t optlen);
extern int select(int nfds, fd_set *readfds, fd_set *writefds,
                          fd_set *exceptfds, const struct timeval *timeout);

#endif /* WOLFSSL_KEIL_TCP_NET */

#endif /* WOLFSSL_KEIL_RL_H */
