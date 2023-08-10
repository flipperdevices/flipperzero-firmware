/* io.h
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

/*!
    \file wolfssl/wolfio.h
*/

#ifndef WOLFSSL_IO_H
#define WOLFSSL_IO_H

#include <wolfssl/ssl.h>

#ifdef __cplusplus
    extern "C" {
#endif

/* Micrium uses NetSock I/O callbacks in wolfio.c */
#if !defined(WOLFSSL_USER_IO)
    /* OCSP and CRL_IO require HTTP client */
    #if defined(HAVE_OCSP) || defined(HAVE_CRL_IO)
        #ifndef HAVE_HTTP_CLIENT
            #define HAVE_HTTP_CLIENT
        #endif
    #endif
#endif

#if !defined(WOLFSSL_USER_IO)
    /* Micrium uses NetSock I/O callbacks in wolfio.c */
    #if !defined(USE_WOLFSSL_IO) && !defined(MICRIUM) && \
        !defined(WOLFSSL_CONTIKI) && !defined(WOLFSSL_NO_SOCK)
        #define USE_WOLFSSL_IO
    #endif
#endif


#if defined(USE_WOLFSSL_IO) || defined(HAVE_HTTP_CLIENT)

#ifdef HAVE_LIBZ
    #include "zlib.h"
#endif

#ifndef USE_WINDOWS_API
    #if defined(WOLFSSL_LWIP) && !defined(WOLFSSL_APACHE_MYNEWT)
        /* lwIP needs to be configured to use sockets API in this mode */
        /* LWIP_SOCKET 1 in lwip/opt.h or in build */
        #include "lwip/sockets.h"
        #ifndef LWIP_PROVIDE_ERRNO
            #include <errno.h>
            #define LWIP_PROVIDE_ERRNO 1
        #endif
    #elif defined(FREESCALE_MQX)
        #include <posix.h>
        #include <rtcs.h>
    #elif defined(FREESCALE_KSDK_MQX)
        #include <rtcs.h>
    #elif (defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET))
        #include "rl_net.h"
        #include "errno.h"
    #elif defined(WOLFSSL_CMSIS_RTOS)
        #include "cmsis_os.h"
    #elif defined(WOLFSSL_CMSIS_RTOSv2)
        #include "cmsis_os2.h"
    #elif defined(WOLFSSL_TIRTOS)
        #include <sys/socket.h>
    #elif defined(FREERTOS_TCP)
        #include "FreeRTOS_Sockets.h"
    #elif defined(WOLFSSL_IAR_ARM)
        /* nothing */
    #elif defined(HAVE_NETX_BSD)
        #ifdef NETX_DUO
            #include "nxd_bsd.h"
        #else
            #include "nx_bsd.h"
        #endif
    #elif defined(WOLFSSL_VXWORKS)
        #include <sockLib.h>
        #include <errno.h>
    #elif defined(WOLFSSL_NUCLEUS_1_2)
        #include <externs.h>
        #include <errno.h>
    #elif defined(WOLFSSL_LINUXKM)
        /* the requisite linux/net.h is included in wc_port.h, with incompatible warnings masked out. */
    #elif defined(WOLFSSL_ATMEL)
        #include "socket/include/socket.h"
    #elif defined(INTIME_RTOS)
        #undef MIN
        #undef MAX
        #include <rt.h>
        #include <sys/types.h>
        #include <sys/socket.h>
        #include <netdb.h>
        #include <netinet/in.h>
        #include <io.h>
        /* <sys/socket.h> defines these, to avoid conflict, do undef */
        #undef SOCKADDR
        #undef SOCKADDR_IN
    #elif defined(WOLFSSL_PRCONNECT_PRO)
        #include <prconnect_pro/prconnect_pro.h>
        #include <sys/types.h>
        #include <errno.h>
        #include <unistd.h>
        #include <fcntl.h>
        #include <netdb.h>
        #include <sys/ioctl.h>
    #elif defined(WOLFSSL_SGX)
        #include <errno.h>
    #elif defined(WOLFSSL_APACHE_MYNEWT) && !defined(WOLFSSL_LWIP)
        #include <mn_socket/mn_socket.h>
    #elif defined(WOLFSSL_DEOS)
        #include <socketapi.h>
        #include <lwip-socket.h>
        #include <errno.h>
    #elif defined(WOLFSSL_ZEPHYR)
        #include <zephyr/net/socket.h>
    #elif defined(MICROCHIP_PIC32)
        #include <sys/errno.h>
    #elif defined(HAVE_NETX)
        #include "nx_api.h"
        #include "errno.h"
    #elif defined(FUSION_RTOS)
        #include <sys/fcltypes.h>
        #include <fclerrno.h>
        #include <fclfcntl.h>
    #elif !defined(WOLFSSL_NO_SOCK)
        #include <sys/types.h>
        #include <errno.h>
        #ifndef EBSNET
            #include <unistd.h>
        #endif
        #include <fcntl.h>
        #define XFCNTL(fd, flag, block) fcntl((fd), (flag), (block))

        #if defined(HAVE_RTP_SYS)
            #include <socket.h>
        #elif defined(EBSNET)
            #include "rtipapi.h"  /* errno */
            #include "socket.h"
        #elif defined(NETOS)
            #include <sockapi.h>
        #elif !defined(DEVKITPRO) && !defined(WOLFSSL_PICOTCP) \
                && !defined(WOLFSSL_CONTIKI) && !defined(WOLFSSL_WICED) \
                && !defined(WOLFSSL_GNRC) && !defined(WOLFSSL_RIOT_OS)
            #include <sys/socket.h>
            #include <arpa/inet.h>
            #include <netinet/in.h>
            #include <netdb.h>
            #ifdef __PPU
                #include <netex/errno.h>
            #else
                #include <sys/ioctl.h>
            #endif
        #endif
    #endif

    #if defined(WOLFSSL_RENESAS_RA6M3G) || defined(WOLFSSL_RENESAS_RA6M3) ||\
                    defined(WOLFSSL_RENESAS_RA6M4)
      /* Uses FREERTOS_TCP */
        #include <errno.h>
    #endif

    #if defined(WOLFSSL_EMBOS)
        #include <errno.h>
    #endif

#endif /* USE_WINDOWS_API */

#ifdef __sun
    #include <sys/filio.h>
#endif

#ifdef USE_WINDOWS_API
    /* no epipe yet */
    #ifndef WSAEPIPE
        #define WSAEPIPE       -12345
    #endif
    #define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
    #define SOCKET_EAGAIN      WSAETIMEDOUT
    #define SOCKET_ECONNRESET  WSAECONNRESET
    #define SOCKET_EINTR       WSAEINTR
    #define SOCKET_EPIPE       WSAEPIPE
    #define SOCKET_ECONNREFUSED WSAENOTCONN
    #define SOCKET_ECONNABORTED WSAECONNABORTED
#elif defined(__PPU)
    #define SOCKET_EWOULDBLOCK SYS_NET_EWOULDBLOCK
    #define SOCKET_EAGAIN      SYS_NET_EAGAIN
    #define SOCKET_ECONNRESET  SYS_NET_ECONNRESET
    #define SOCKET_EINTR       SYS_NET_EINTR
    #define SOCKET_EPIPE       SYS_NET_EPIPE
    #define SOCKET_ECONNREFUSED SYS_NET_ECONNREFUSED
    #define SOCKET_ECONNABORTED SYS_NET_ECONNABORTED
#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    #if MQX_USE_IO_OLD
        /* RTCS old I/O doesn't have an EWOULDBLOCK */
        #define SOCKET_EWOULDBLOCK  EAGAIN
        #define SOCKET_EAGAIN       EAGAIN
        #define SOCKET_ECONNRESET   RTCSERR_TCP_CONN_RESET
        #define SOCKET_EINTR        EINTR
        #define SOCKET_EPIPE        EPIPE
        #define SOCKET_ECONNREFUSED RTCSERR_TCP_CONN_REFUSED
        #define SOCKET_ECONNABORTED RTCSERR_TCP_CONN_ABORTED
    #else
        #define SOCKET_EWOULDBLOCK  NIO_EWOULDBLOCK
        #define SOCKET_EAGAIN       NIO_EAGAIN
        #define SOCKET_ECONNRESET   NIO_ECONNRESET
        #define SOCKET_EINTR        NIO_EINTR
        #define SOCKET_EPIPE        NIO_EPIPE
        #define SOCKET_ECONNREFUSED NIO_ECONNREFUSED
        #define SOCKET_ECONNABORTED NIO_ECONNABORTED
    #endif
#elif defined(WOLFSSL_MDK_ARM)|| defined(WOLFSSL_KEIL_TCP_NET)
        #define SOCKET_EWOULDBLOCK BSD_ERROR_WOULDBLOCK
        #define SOCKET_EAGAIN      BSD_ERROR_LOCKED
        #define SOCKET_ECONNRESET  BSD_ERROR_CLOSED
        #define SOCKET_EINTR       BSD_ERROR
        #define SOCKET_EPIPE       BSD_ERROR
        #define SOCKET_ECONNREFUSED BSD_ERROR
        #define SOCKET_ECONNABORTED BSD_ERROR
#elif defined(WOLFSSL_PICOTCP)
    #define SOCKET_EWOULDBLOCK  PICO_ERR_EAGAIN
    #define SOCKET_EAGAIN       PICO_ERR_EAGAIN
    #define SOCKET_ECONNRESET   PICO_ERR_ECONNRESET
    #define SOCKET_EINTR        PICO_ERR_EINTR
    #define SOCKET_EPIPE        PICO_ERR_EIO
    #define SOCKET_ECONNREFUSED PICO_ERR_ECONNREFUSED
    #define SOCKET_ECONNABORTED PICO_ERR_ESHUTDOWN
#elif defined(FREERTOS_TCP)
    #define SOCKET_EWOULDBLOCK FREERTOS_EWOULDBLOCK
    #define SOCKET_EAGAIN       FREERTOS_EWOULDBLOCK
    #define SOCKET_ECONNRESET   FREERTOS_SOCKET_ERROR
    #define SOCKET_EINTR        FREERTOS_SOCKET_ERROR
    #define SOCKET_EPIPE        FREERTOS_SOCKET_ERROR
    #define SOCKET_ECONNREFUSED FREERTOS_SOCKET_ERROR
    #define SOCKET_ECONNABORTED FREERTOS_SOCKET_ERROR
#elif defined(WOLFSSL_NUCLEUS_1_2)
    #define SOCKET_EWOULDBLOCK  NU_WOULD_BLOCK
    #define SOCKET_EAGAIN       NU_WOULD_BLOCK
    #define SOCKET_ECONNRESET   NU_NOT_CONNECTED
    #define SOCKET_EINTR        NU_NOT_CONNECTED
    #define SOCKET_EPIPE        NU_NOT_CONNECTED
    #define SOCKET_ECONNREFUSED NU_CONNECTION_REFUSED
    #define SOCKET_ECONNABORTED NU_NOT_CONNECTED
#elif defined(WOLFSSL_DEOS)
    /* `sockaddr_storage` is not defined in DEOS. This workaround will
     * work for IPV4, but not IPV6
     */
    #define sockaddr_storage   sockaddr_in
    #define SOCKET_EWOULDBLOCK EAGAIN
    #define SOCKET_EAGAIN      EAGAIN
    #define SOCKET_ECONNRESET  EINTR
    #define SOCKET_EINTR       EINTR
    #define SOCKET_EPIPE       EPIPE
    #define SOCKET_ECONNREFUSED SOCKET_ERROR
    #define SOCKET_ECONNABORTED SOCKET_ERROR
#elif defined(HAVE_NETX)
    #define SOCKET_EWOULDBLOCK NX_NOT_CONNECTED
    #define SOCKET_EAGAIN      NX_NOT_CONNECTED
    #define SOCKET_ECONNRESET  NX_NOT_CONNECTED
    #define SOCKET_EINTR       NX_NOT_CONNECTED
    #define SOCKET_EPIPE       NX_NOT_CONNECTED
    #define SOCKET_ECONNREFUSED NX_NOT_CONNECTED
    #define SOCKET_ECONNABORTED NX_NOT_CONNECTED
#elif defined(FUSION_RTOS)
    #define SOCKET_EWOULDBLOCK FCL_EWOULDBLOCK
    #define SOCKET_EAGAIN      FCL_EAGAIN
    #define SOCKET_ECONNRESET  FNS_ECONNRESET
    #define SOCKET_EINTR       FCL_EINTR
    #define SOCKET_EPIPE       FCL_EPIPE
    #define SOCKET_ECONNREFUSED FCL_ECONNREFUSED
    #define SOCKET_ECONNABORTED FNS_ECONNABORTED
#elif defined(WOLFSSL_LWIP_NATIVE)
    #define SOCKET_EWOULDBLOCK ERR_WOULDBLOCK
    #define SOCKET_EAGAIN      ERR_WOULDBLOCK
    #define SOCKET_ECONNRESET  ERR_RST
    #define SOCKET_EINTR       ERR_CLSD
    #define SOCKET_EPIPE       ERR_CLSD
    #define SOCKET_ECONNREFUSED ERR_CONN
    #define SOCKET_ECONNABORTED ERR_ABRT
#elif defined(WOLFSSL_EMNET)
    #include <IP/IP.h>
    #define SOCKET_EWOULDBLOCK  IP_ERR_WOULD_BLOCK
    #define SOCKET_EAGAIN       IP_ERR_WOULD_BLOCK
    #define SOCKET_ECONNRESET   IP_ERR_CONN_RESET
    #define SOCKET_EINTR        IP_ERR_FAULT
    #define SOCKET_EPIPE        IP_ERR_PIPE
    #define SOCKET_ECONNREFUSED IP_ERR_CONN_REFUSED
    #define SOCKET_ECONNABORTED IP_ERR_CONN_ABORTED
#else
    #define SOCKET_EWOULDBLOCK EWOULDBLOCK
    #define SOCKET_EAGAIN      EAGAIN
    #define SOCKET_ECONNRESET  ECONNRESET
    #define SOCKET_EINTR       EINTR
    #define SOCKET_EPIPE       EPIPE
    #define SOCKET_ECONNREFUSED ECONNREFUSED
    #define SOCKET_ECONNABORTED ECONNABORTED
#endif /* USE_WINDOWS_API */

#ifdef DEVKITPRO
    /* from network.h */
    #include <network.h>
    #define SEND_FUNCTION net_send
    #define RECV_FUNCTION net_recv
#elif defined(WOLFSSL_LWIP) && !defined(WOLFSSL_APACHE_MYNEWT)
    #define SEND_FUNCTION lwip_send
    #define RECV_FUNCTION lwip_recv
#elif defined(WOLFSSL_PICOTCP)
    #define SEND_FUNCTION pico_send
    #define RECV_FUNCTION pico_recv
#elif defined(FREERTOS_TCP)
    #define RECV_FUNCTION(a,b,c,d)  FreeRTOS_recv((Socket_t)(a),(void*)(b), (size_t)(c), (BaseType_t)(d))
    #define SEND_FUNCTION(a,b,c,d)  FreeRTOS_send((Socket_t)(a),(void*)(b), (size_t)(c), (BaseType_t)(d))
#elif defined(WOLFSSL_VXWORKS)
    /*socket.h already has "typedef struct sockaddr SOCKADDR;"
      so don't redefine it in wolfSSL */
    #define HAVE_SOCKADDR_DEFINED
    #define SEND_FUNCTION send
    #define RECV_FUNCTION recv
#elif defined(WOLFSSL_NUCLEUS_1_2)
    #define SEND_FUNCTION NU_Send
    #define RECV_FUNCTION NU_Recv
#elif defined(FUSION_RTOS)
    #define SEND_FUNCTION FNS_SEND
    #define RECV_FUNCTION FNS_RECV
#elif defined(WOLFSSL_ZEPHYR)
    #ifndef WOLFSSL_MAX_SEND_SZ
        #define WOLFSSL_MAX_SEND_SZ       256
    #endif

    #define SEND_FUNCTION send
    #define RECV_FUNCTION recv
#elif defined(WOLFSSL_LINUXKM)
    #define SEND_FUNCTION linuxkm_send
    #define RECV_FUNCTION linuxkm_recv
#elif defined(WOLFSSL_SGX)
    #define SEND_FUNCTION send
    #define RECV_FUNCTION recv
#else
    #define SEND_FUNCTION send
    #define RECV_FUNCTION recv
    #if !defined(HAVE_SOCKADDR) && !defined(WOLFSSL_NO_SOCK)
        #define HAVE_SOCKADDR
    #endif
#endif

#ifdef USE_WINDOWS_API
    #if defined(__MINGW64__)
        typedef size_t SOCKET_T;
    #else
        typedef unsigned int SOCKET_T;
    #endif
    #ifndef SOCKET_INVALID
        #define SOCKET_INVALID INVALID_SOCKET
    #endif
#else
    typedef int SOCKET_T;
    #ifndef SOCKET_INVALID
        #define SOCKET_INVALID (-1)
    #endif
#endif

#ifndef WOLFSSL_NO_SOCK
    #ifndef XSOCKLENT
        #ifdef USE_WINDOWS_API
            #define XSOCKLENT int
        #else
            #define XSOCKLENT socklen_t
        #endif
    #endif
    #ifndef XSOCKOPT_TYPE_OPTVAL_TYPE
        #ifdef USE_WINDOWS_API
            #define XSOCKOPT_TYPE_OPTVAL_TYPE void*
        #else
            #define XSOCKOPT_TYPE_OPTVAL_TYPE char*
        #endif
    #endif

    /* Socket Addr Support */
    #ifdef HAVE_SOCKADDR
    #ifndef HAVE_SOCKADDR_DEFINED
        typedef struct sockaddr         SOCKADDR;
    #endif
        typedef struct sockaddr_storage SOCKADDR_S;
        typedef struct sockaddr_in      SOCKADDR_IN;
        #ifdef WOLFSSL_IPV6
            typedef struct sockaddr_in6 SOCKADDR_IN6;
        #endif
        typedef struct hostent          HOSTENT;
    #endif /* HAVE_SOCKADDR */

    #if defined(HAVE_GETADDRINFO)
        typedef struct addrinfo         ADDRINFO;
    #endif
#endif /* WOLFSSL_NO_SOCK */


/* IO API's */
#ifdef HAVE_IO_TIMEOUT
    WOLFSSL_API  int wolfIO_SetBlockingMode(SOCKET_T sockfd, int non_blocking);
    WOLFSSL_API void wolfIO_SetTimeout(int to_sec);
    WOLFSSL_API  int wolfIO_Select(SOCKET_T sockfd, int to_sec);
#endif
WOLFSSL_API  int wolfIO_TcpConnect(SOCKET_T* sockfd, const char* ip,
    unsigned short port, int to_sec);
#ifdef HAVE_SOCKADDR
WOLFSSL_API int wolfIO_TcpAccept(SOCKET_T sockfd, SOCKADDR* peer_addr, XSOCKLENT* peer_len);
#endif
WOLFSSL_API int wolfIO_TcpBind(SOCKET_T* sockfd, word16 port);
WOLFSSL_API  int wolfIO_Send(SOCKET_T sd, char *buf, int sz, int wrFlags);
WOLFSSL_API  int wolfIO_Recv(SOCKET_T sd, char *buf, int sz, int rdFlags);

#endif /* USE_WOLFSSL_IO || HAVE_HTTP_CLIENT */

#ifndef WOLFSSL_NO_SOCK
#ifdef USE_WINDOWS_API
    #ifndef CloseSocket
        #define CloseSocket(s) closesocket(s)
    #endif
    #define StartTCP() { WSADATA wsd; WSAStartup(0x0002, &wsd); }
#elif defined(WOLFSSL_MDK_ARM) || defined(WOLFSSL_KEIL_TCP_NET)
    #ifndef CloseSocket
        extern int closesocket(int);
        #define CloseSocket(s) closesocket(s)
    #endif
    #define StartTCP() WC_DO_NOTHING
#elif defined(FUSION_RTOS)
    #ifndef CloseSocket
        #define CloseSocket(s) do {                     \
                                    int err;            \
                                    FNS_CLOSE(s, &err); \
                                } while(0)
    #endif
#else
    #ifndef CloseSocket
        #define CloseSocket(s) close(s)
    #endif
    #define StartTCP() WC_DO_NOTHING
    #ifdef FREERTOS_TCP_WINSIM
        extern int close(int);
    #endif
#endif
#endif /* WOLFSSL_NO_SOCK */


WOLFSSL_API int BioSend(WOLFSSL* ssl, char *buf, int sz, void *ctx);
WOLFSSL_API int BioReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx);
#if defined(USE_WOLFSSL_IO)
    /* default IO callbacks */
    WOLFSSL_API int EmbedReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_API int EmbedSend(WOLFSSL* ssl, char* buf, int sz, void* ctx);

    #ifdef WOLFSSL_DTLS
        WOLFSSL_API int EmbedReceiveFrom(WOLFSSL *ssl, char *buf, int sz,
                                         void *ctx);
        WOLFSSL_API int EmbedSendTo(WOLFSSL* ssl, char *buf, int sz, void *ctx);
        WOLFSSL_API int EmbedGenerateCookie(WOLFSSL* ssl, byte *buf, int sz,
                                            void *ctx);
        #ifdef WOLFSSL_MULTICAST
            WOLFSSL_API int EmbedReceiveFromMcast(WOLFSSL *ssl, char *buf,
                                                  int sz, void *ctx);
        #endif /* WOLFSSL_MULTICAST */
    #endif /* WOLFSSL_DTLS */
#endif /* USE_WOLFSSL_IO */

#ifdef HAVE_OCSP
    WOLFSSL_API int wolfIO_HttpBuildRequestOcsp(const char* domainName,
        const char* path, int ocspReqSz, unsigned char* buf, int bufSize);
    WOLFSSL_API int wolfIO_HttpProcessResponseOcsp(int sfd,
        unsigned char** respBuf, unsigned char* httpBuf, int httpBufSz,
        void* heap);

    WOLFSSL_API int EmbedOcspLookup(void* ctx, const char* url, int urlSz,
                        byte* ocspReqBuf, int ocspReqSz, byte** ocspRespBuf);
    WOLFSSL_API void EmbedOcspRespFree(void* ctx, byte *resp);
#endif

#ifdef HAVE_CRL_IO
    WOLFSSL_API int wolfIO_HttpBuildRequestCrl(const char* url, int urlSz,
        const char* domainName, unsigned char* buf, int bufSize);
    WOLFSSL_API int wolfIO_HttpProcessResponseCrl(WOLFSSL_CRL* crl, int sfd,
        unsigned char* httpBuf, int httpBufSz);

    WOLFSSL_API int EmbedCrlLookup(WOLFSSL_CRL* crl, const char* url,
        int urlSz);
#endif


#if defined(HAVE_HTTP_CLIENT)
    WOLFSSL_API  int wolfIO_DecodeUrl(const char* url, int urlSz, char* outName,
        char* outPath, unsigned short* outPort);

    WOLFSSL_API  int wolfIO_HttpBuildRequest(const char* reqType,
        const char* domainName, const char* path, int pathLen, int reqSz,
        const char* contentType, unsigned char* buf, int bufSize);
    WOLFSSL_LOCAL int wolfIO_HttpBuildRequest_ex(const char* reqType,
        const char* domainName, const char* path, int pathLen, int reqSz,
        const char* contentType, const char *exHdrs, unsigned char* buf, int bufSize);
    WOLFSSL_API  int wolfIO_HttpProcessResponse(int sfd, const char** appStrList,
        unsigned char** respBuf, unsigned char* httpBuf, int httpBufSz,
        int dynType, void* heap);
#endif /* HAVE_HTTP_CLIENT */


/* I/O callbacks */
typedef int (*CallbackIORecv)(WOLFSSL *ssl, char *buf, int sz, void *ctx);
typedef int (*CallbackIOSend)(WOLFSSL *ssl, char *buf, int sz, void *ctx);
WOLFSSL_API void wolfSSL_CTX_SetIORecv(WOLFSSL_CTX *ctx, CallbackIORecv CBIORecv);
WOLFSSL_API void wolfSSL_CTX_SetIOSend(WOLFSSL_CTX *ctx, CallbackIOSend CBIOSend);
WOLFSSL_API void wolfSSL_SSLSetIORecv(WOLFSSL *ssl, CallbackIORecv CBIORecv);
WOLFSSL_API void wolfSSL_SSLSetIOSend(WOLFSSL *ssl, CallbackIOSend CBIOSend);
/* deprecated old name */
#define wolfSSL_SetIORecv wolfSSL_CTX_SetIORecv
#define wolfSSL_SetIOSend wolfSSL_CTX_SetIOSend

WOLFSSL_API void wolfSSL_SetIOReadCtx( WOLFSSL* ssl, void *ctx);
WOLFSSL_API void wolfSSL_SetIOWriteCtx(WOLFSSL* ssl, void *ctx);

WOLFSSL_API void* wolfSSL_GetIOReadCtx( WOLFSSL* ssl);
WOLFSSL_API void* wolfSSL_GetIOWriteCtx(WOLFSSL* ssl);

WOLFSSL_API void wolfSSL_SetIOReadFlags( WOLFSSL* ssl, int flags);
WOLFSSL_API void wolfSSL_SetIOWriteFlags(WOLFSSL* ssl, int flags);


#ifdef HAVE_NETX
    WOLFSSL_LOCAL int NetX_Receive(WOLFSSL *ssl, char *buf, int sz, void *ctx);
    WOLFSSL_LOCAL int NetX_Send(WOLFSSL *ssl, char *buf, int sz, void *ctx);

    WOLFSSL_API void wolfSSL_SetIO_NetX(WOLFSSL* ssl, NX_TCP_SOCKET* nxsocket,
                                      ULONG waitoption);
#endif /* HAVE_NETX */

#ifdef MICRIUM
    WOLFSSL_LOCAL int MicriumSend(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_LOCAL int MicriumReceive(WOLFSSL* ssl, char* buf, int sz,
                                     void* ctx);
    WOLFSSL_LOCAL int MicriumReceiveFrom(WOLFSSL* ssl, char* buf, int sz,
                                         void* ctx);
    WOLFSSL_LOCAL int MicriumSendTo(WOLFSSL* ssl, char* buf, int sz, void* ctx);
#endif /* MICRIUM */

#if defined(WOLFSSL_APACHE_MYNEWT) && !defined(WOLFSSL_LWIP)
    WOLFSSL_LOCAL int Mynewt_Receive(WOLFSSL *ssl, char *buf, int sz, void *ctx);
    WOLFSSL_LOCAL int Mynewt_Send(WOLFSSL* ssl, char *buf, int sz, void *ctx);
    WOLFSSL_API void wolfSSL_SetIO_Mynewt(WOLFSSL* ssl, struct mn_socket* mnSocket,
                                          struct mn_sockaddr_in* mnSockAddrIn);
#endif /* defined(WOLFSSL_APACHE_MYNEWT) && !defined(WOLFSSL_LWIP) */

#ifdef WOLFSSL_UIP

    struct uip_wolfssl_ctx {
        union socket_connector {
            struct tcp_socket tcp;
            struct udp_socket udp;
        } conn;
        WOLFSSL_CTX *ctx;
        WOLFSSL *ssl;
        byte *input_databuf;
        byte *output_databuf;
        byte *ssl_rx_databuf;
        int ssl_rb_len;
        int ssl_rb_off;
        struct process *process;
        tcp_socket_data_callback_t input_callback;
        tcp_socket_event_callback_t event_callback;
        int closing;
        uip_ipaddr_t peer_addr;
        word16 peer_port;
    };

    typedef struct uip_wolfssl_ctx uip_wolfssl_ctx;

    WOLFSSL_LOCAL int uIPSend(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_LOCAL int uIPReceive(WOLFSSL* ssl, char* buf, int sz,
                                     void* ctx);
    WOLFSSL_LOCAL int uIPReceiveFrom(WOLFSSL* ssl, char* buf, int sz,
                                         void* ctx);
    WOLFSSL_LOCAL int uIPSendTo(WOLFSSL* ssl, char* buf, int sz, void* ctx);

#endif

#ifdef WOLFSSL_GNRC
    #include <sock_types.h>
    #include <net/gnrc.h>
    #include <net/af.h>
    #include <net/sock.h>
    #include <net/gnrc/tcp.h>
    #include <net/gnrc/udp.h>

    struct gnrc_wolfssl_ctx {
        union socket_connector {
        #ifdef MODULE_SOCK_TCP
            sock_tcp_t tcp;
        #endif
            sock_udp_t udp;
        } conn;
        WOLFSSL_CTX *ctx;
        WOLFSSL *ssl;

        int closing;
        struct _sock_tl_ep peer_addr;
    };

    typedef struct gnrc_wolfssl_ctx sock_tls_t;

    WOLFSSL_LOCAL int GNRC_ReceiveFrom(WOLFSSL* ssl, char* buf, int sz,
                                     void* ctx);
    WOLFSSL_LOCAL int GNRC_SendTo(WOLFSSL* ssl, char* buf, int sz, void* ctx);

#endif

#ifdef WOLFSSL_LWIP_NATIVE
    #include "lwip/tcp.h"
    #include "lwip/sockets.h"

    typedef struct WOLFSSL_LWIP_NATIVE_STATE {
        struct tcp_pcb * pcb;
        tcp_recv_fn recv_fn;
        tcp_sent_fn sent_fn;
        int    pulled;
        struct pbuf *pbuf;
        int    wait;
        void * arg;   /* arg for application */
        int    idle_count;
    } WOLFSSL_LWIP_NATIVE_STATE;

    WOLFSSL_LOCAL int LwIPNativeSend(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_LOCAL int LwIPNativeReceive(WOLFSSL* ssl, char* buf, int sz,
                                     void* ctx);
    WOLFSSL_API   int wolfSSL_SetIO_LwIP(WOLFSSL* ssl, void *pcb,
                                tcp_recv_fn recv, tcp_sent_fn sent, void *arg);
#endif
#ifdef WOLFSSL_ISOTP
    #define ISOTP_DEFAULT_TIMEOUT 100
    #define ISOTP_DEFAULT_WAIT_COUNT 3
    #define ISOTP_FIRST_FRAME_DATA_SIZE 6
    #define ISOTP_SINGLE_FRAME_DATA_SIZE 7
    #define ISOTP_MAX_CONSECUTIVE_FRAME_DATA_SIZE 7
    #define ISOTP_MAX_MS_FRAME_DELAY 0x7f
    #define ISOTP_CAN_BUS_PAYLOAD_SIZE 8
    #define ISOTP_MAX_DATA_SIZE 4095
    /* Packets will never be larger than the ISO-TP max data size */
    #define ISOTP_DEFAULT_BUFFER_SIZE ISOTP_MAX_DATA_SIZE
    #define ISOTP_FLOW_CONTROL_PACKET_SIZE 3
    #define ISOTP_FLOW_CONTROL_FRAMES 0 /* infinite */
    #define ISOTP_MAX_SEQUENCE_COUNTER 15

    enum isotp_frame_type {
        ISOTP_FRAME_TYPE_SINGLE      = 0,
        ISOTP_FRAME_TYPE_FIRST       = 1,
        ISOTP_FRAME_TYPE_CONSECUTIVE = 2,
        ISOTP_FRAME_TYPE_CONTROL     = 3
    };

    enum isotp_flow_control {
        ISOTP_FLOW_CONTROL_CTS   = 0,
        ISOTP_FLOW_CONTROL_WAIT  = 1,
        ISOTP_FLOW_CONTROL_ABORT = 2
    };

    enum isotp_connection_state {
        ISOTP_CONN_STATE_IDLE,
        ISOTP_CONN_STATE_SENDING,
        ISOTP_CONN_STATE_RECEIVING
    };

    typedef struct isotp_can_data {
        byte data[ISOTP_CAN_BUS_PAYLOAD_SIZE];
        byte length;
    } isotp_can_data;

    /* User supplied functions for sending/receiving CAN bus messages of up to
     * 8 bytes, as well as a function to add an artificial delay when a
     * receiver requests one. */
    typedef int (*can_recv_fn)(struct isotp_can_data *data, void *arg,
            int timeout);
    typedef int (*can_send_fn)(struct isotp_can_data *data, void *arg);
    typedef void (*can_delay_fn)(int microseconds);

    typedef struct isotp_wolfssl_ctx {
        struct isotp_can_data frame;
        char *buf_ptr;
        char *receive_buffer;
        char *receive_buffer_ptr;
        can_recv_fn recv_fn;
        can_send_fn send_fn;
        can_delay_fn delay_fn;
        void *arg;
        int receive_buffer_len;
        int receive_buffer_size;
        enum isotp_connection_state state;
        word16 buf_length;
        byte sequence;
        byte flow_packets;
        byte flow_counter;
        byte frame_delay;
        byte wait_counter;
        byte receive_delay;
    } isotp_wolfssl_ctx;

    WOLFSSL_LOCAL int ISOTP_Receive(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_LOCAL int ISOTP_Send(WOLFSSL* ssl, char* buf, int sz, void* ctx);
    WOLFSSL_API int wolfSSL_SetIO_ISOTP(WOLFSSL *ssl, isotp_wolfssl_ctx *ctx,
            can_recv_fn recv_fn, can_send_fn send_fn, can_delay_fn delay_fn,
            word32 receive_delay, char *receive_buffer,
            int receive_buffer_size, void *arg);

#endif
#ifdef WOLFSSL_DTLS
    typedef int (*CallbackGenCookie)(WOLFSSL* ssl, unsigned char* buf, int sz,
                                     void* ctx);
    WOLFSSL_API void  wolfSSL_CTX_SetGenCookie(WOLFSSL_CTX* ctx,
                                               CallbackGenCookie cb);
    WOLFSSL_API void  wolfSSL_SetCookieCtx(WOLFSSL* ssl, void *ctx);
    WOLFSSL_API void* wolfSSL_GetCookieCtx(WOLFSSL* ssl);

#endif
#ifdef WOLFSSL_SESSION_EXPORT
    typedef int (*CallbackGetPeer)(WOLFSSL* ssl, char* ip, int* ipSz,
                                        unsigned short* port, int* fam);
    typedef int (*CallbackSetPeer)(WOLFSSL* ssl, char* ip, int ipSz,
                                          unsigned short port, int fam);

    WOLFSSL_API void wolfSSL_CTX_SetIOGetPeer(WOLFSSL_CTX*, CallbackGetPeer);
    WOLFSSL_API void wolfSSL_CTX_SetIOSetPeer(WOLFSSL_CTX*, CallbackSetPeer);
    WOLFSSL_API int EmbedGetPeer(WOLFSSL* ssl, char* ip, int* ipSz,
                                                unsigned short* port, int* fam);
    WOLFSSL_API int EmbedSetPeer(WOLFSSL* ssl, char* ip, int ipSz,
                                                  unsigned short port, int fam);
#endif /* WOLFSSL_SESSION_EXPORT */



#ifndef XINET_NTOP
    #define XINET_NTOP(a,b,c,d) inet_ntop((a),(b),(c),(d))
    #ifdef USE_WINDOWS_API /* Windows-friendly definition */
        #undef  XINET_NTOP
        #define XINET_NTOP(a,b,c,d) InetNtop((a),(b),(c),(d))
    #endif
#endif
#ifndef XINET_PTON
    #define XINET_PTON(a,b,c)   inet_pton((a),(b),(c))
    #ifdef USE_WINDOWS_API /* Windows-friendly definition */
        #undef  XINET_PTON
        #if defined(__MINGW64__) && !defined(UNICODE)
            #define XINET_PTON(a,b,c)   InetPton((a),(b),(c))
        #else
            #define XINET_PTON(a,b,c)   InetPton((a),(PCWSTR)(b),(c))
        #endif
    #endif
#endif

#ifndef XHTONS
    #if !defined(WOLFSSL_NO_SOCK) && (defined(USE_WOLFSSL_IO) || defined(HAVE_HTTP_CLIENT))
        #define XHTONS(a) htons((a))
    #else
        /* we don't have sockets, so define our own htons and ntohs */
        #ifdef BIG_ENDIAN_ORDER
            #define XHTONS(a) (a)
        #else
            #define XHTONS(a) ((((a) >> 8) & 0xff) | (((a) & 0xff) << 8))
        #endif
    #endif
#endif
#ifndef XNTOHS
    #if !defined(WOLFSSL_NO_SOCK) && (defined(USE_WOLFSSL_IO) || defined(HAVE_HTTP_CLIENT))
        #define XNTOHS(a) ntohs((a))
    #else
        /* we don't have sockets, so define our own htons and ntohs */
        #ifdef BIG_ENDIAN_ORDER
            #define XNTOHS(a) (a)
        #else
            #define XNTOHS(a) ((((a) >> 8) & 0xff) | (((a) & 0xff) << 8))
        #endif
    #endif
#endif

#ifndef WOLFSSL_IP4
    #define WOLFSSL_IP4 AF_INET
#endif
#ifndef WOLFSSL_IP6
    #define WOLFSSL_IP6 AF_INET6
#endif


#ifdef __cplusplus
    }  /* extern "C" */
#endif

#endif /* WOLFSSL_IO_H */
