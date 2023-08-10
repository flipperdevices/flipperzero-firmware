/* wolfio.c
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


#ifndef WOLFSSL_STRERROR_BUFFER_SIZE
#define WOLFSSL_STRERROR_BUFFER_SIZE 256
#endif

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#ifndef WOLFCRYPT_ONLY

#ifdef _WIN32_WCE
    /* On WinCE winsock2.h must be included before windows.h for socket stuff */
    #include <winsock2.h>
#endif

#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/wolfio.h>

#if defined(HAVE_HTTP_CLIENT)
    #include <stdlib.h>   /* strtol() */
#endif

/*
Possible IO enable options:
 * WOLFSSL_USER_IO:     Disables default Embed* callbacks and     default: off
                        allows user to define their own using
                        wolfSSL_CTX_SetIORecv and wolfSSL_CTX_SetIOSend
 * USE_WOLFSSL_IO:      Enables the wolfSSL IO functions          default: on
 * HAVE_HTTP_CLIENT:    Enables HTTP client API's                 default: off
                                     (unless HAVE_OCSP or HAVE_CRL_IO defined)
 * HAVE_IO_TIMEOUT:     Enables support for connect timeout       default: off
 *
 * DTLS_RECEIVEFROM_NO_TIMEOUT_ON_INVALID_PEER: This flag has effect only if
 * ASN_NO_TIME is enabled. If enabled invalid peers messages are ignored
 * indefinetely. If not enabled EmbedReceiveFrom will return timeout after
 * DTLS_RECEIVEFROM_MAX_INVALID_PEER number of packets from invalid peers. When
 * enabled, without a timer, EmbedReceivefrom can't check if the timeout is
 * expired and it may never return under a continuous flow of invalid packets.
 *                                                                default: off
 */


/* if user writes own I/O callbacks they can define WOLFSSL_USER_IO to remove
   automatic setting of default I/O functions EmbedSend() and EmbedReceive()
   but they'll still need SetCallback xxx() at end of file
*/

#if defined(NO_ASN_TIME) && !defined(DTLS_RECEIVEFROM_NO_TIMEOUT_ON_INVALID_PEER) \
  && !defined(DTLS_RECEIVEFROM_MAX_INVALID_PEER)
#define DTLS_RECEIVEFROM_MAX_INVALID_PEER 10
#endif

#if defined(USE_WOLFSSL_IO) || defined(HAVE_HTTP_CLIENT)

/* Translates return codes returned from
 * send() and recv() if need be.
 */
static WC_INLINE int TranslateReturnCode(int old, int sd)
{
    (void)sd;

#if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    if (old == 0) {
        errno = SOCKET_EWOULDBLOCK;
        return -1;  /* convert to BSD style wouldblock as error */
    }

    if (old < 0) {
        errno = RTCS_geterror(sd);
        if (errno == RTCSERR_TCP_CONN_CLOSING)
            return 0;   /* convert to BSD style closing */
        if (errno == RTCSERR_TCP_CONN_RLSD)
            errno = SOCKET_ECONNRESET;
        if (errno == RTCSERR_TCP_TIMED_OUT)
            errno = SOCKET_EAGAIN;
    }
#endif

    return old;
}

static WC_INLINE int wolfSSL_LastError(int err)
{
    (void)err; /* Suppress unused arg */

#ifdef USE_WINDOWS_API
    return WSAGetLastError();
#elif defined(EBSNET)
    return xn_getlasterror();
#elif defined(WOLFSSL_LINUXKM)
    return err; /* Return provided error value */
#elif defined(FUSION_RTOS)
    #include <fclerrno.h>
    return FCL_GET_ERRNO;
#else
    return errno;
#endif
}

static int TranslateIoError(int err)
{
#ifdef  _WIN32
    size_t errstr_offset;
    char errstr[WOLFSSL_STRERROR_BUFFER_SIZE];
#endif /* _WIN32 */


    if (err > 0)
        return err;

    err = wolfSSL_LastError(err);
#if SOCKET_EWOULDBLOCK != SOCKET_EAGAIN
    if ((err == SOCKET_EWOULDBLOCK) || (err == SOCKET_EAGAIN))
#else
    if (err == SOCKET_EWOULDBLOCK)
#endif
    {
        WOLFSSL_MSG("\tWould block");
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
    else if (err == SOCKET_ECONNRESET) {
        WOLFSSL_MSG("\tConnection reset");
        return WOLFSSL_CBIO_ERR_CONN_RST;
    }
    else if (err == SOCKET_EINTR) {
        WOLFSSL_MSG("\tSocket interrupted");
        return WOLFSSL_CBIO_ERR_ISR;
    }
    else if (err == SOCKET_EPIPE) {
        WOLFSSL_MSG("\tBroken pipe");
        return WOLFSSL_CBIO_ERR_CONN_CLOSE;
    }
    else if (err == SOCKET_ECONNABORTED) {
        WOLFSSL_MSG("\tConnection aborted");
        return WOLFSSL_CBIO_ERR_CONN_CLOSE;
    }

#if defined(_WIN32)
    strcpy_s(errstr, sizeof(errstr), "\tGeneral error: ");
    errstr_offset = strlen(errstr);
    FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)(errstr + errstr_offset),
        (DWORD)(sizeof(errstr) - errstr_offset),
        NULL);
    WOLFSSL_MSG(errstr);
#else
    WOLFSSL_MSG("\tGeneral error");
#endif
    return WOLFSSL_CBIO_ERR_GENERAL;
}
#endif /* USE_WOLFSSL_IO || HAVE_HTTP_CLIENT */

#ifdef OPENSSL_EXTRA
#ifndef NO_BIO
/* Use the WOLFSSL read BIO for receiving data. This is set by the function
 * wolfSSL_set_bio and can also be set by wolfSSL_CTX_SetIORecv.
 *
 * ssl  WOLFSSL struct passed in that has this function set as the receive
 *      callback.
 * buf  buffer to fill with data read
 * sz   size of buf buffer
 * ctx  a user set context
 *
 * returns the amount of data read or want read. See WOLFSSL_CBIO_ERR_* values.
 */
int BioReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    int recvd = WOLFSSL_CBIO_ERR_GENERAL;

    WOLFSSL_ENTER("BioReceive");

    if (ssl->biord == NULL) {
        WOLFSSL_MSG("WOLFSSL biord not set");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    recvd = wolfSSL_BIO_read(ssl->biord, buf, sz);
    if (recvd <= 0) {
        if (/* ssl->biowr->wrIdx is checked for Bind9 */
            wolfSSL_BIO_method_type(ssl->biowr) == WOLFSSL_BIO_BIO &&
            wolfSSL_BIO_wpending(ssl->biowr) != 0 &&
            /* Not sure this pending check is necessary but let's double
             * check that the read BIO is empty before we signal a write
             * need */
            wolfSSL_BIO_supports_pending(ssl->biord) &&
            wolfSSL_BIO_ctrl_pending(ssl->biord) == 0) {
            /* Let's signal to the app layer that we have
             * data pending that needs to be sent. */
            return WOLFSSL_CBIO_ERR_WANT_WRITE;
        }
        else if (ssl->biord->type == WOLFSSL_BIO_SOCKET) {
            if (recvd == 0) {
                WOLFSSL_MSG("BioReceive connection closed");
                return WOLFSSL_CBIO_ERR_CONN_CLOSE;
            }
        #ifdef USE_WOLFSSL_IO
            recvd = TranslateIoError(recvd);
        #endif
            return recvd;
        }

        /* If retry and read flags are set, return WANT_READ */
        if ((ssl->biord->flags & WOLFSSL_BIO_FLAG_READ) &&
            (ssl->biord->flags & WOLFSSL_BIO_FLAG_RETRY)) {
            return WOLFSSL_CBIO_ERR_WANT_READ;
        }

        WOLFSSL_MSG("BIO general error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    (void)ctx;
    return recvd;
}


/* Use the WOLFSSL write BIO for sending data. This is set by the function
 * wolfSSL_set_bio and can also be set by wolfSSL_CTX_SetIOSend.
 *
 * ssl  WOLFSSL struct passed in that has this function set as the send callback.
 * buf  buffer with data to write out
 * sz   size of buf buffer
 * ctx  a user set context
 *
 * returns the amount of data sent or want send. See WOLFSSL_CBIO_ERR_* values.
 */
int BioSend(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    int sent = WOLFSSL_CBIO_ERR_GENERAL;

    WOLFSSL_ENTER("BioSend");

    if (ssl->biowr == NULL) {
        WOLFSSL_MSG("WOLFSSL biowr not set");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    sent = wolfSSL_BIO_write(ssl->biowr, buf, sz);
    if (sent <= 0) {
        if (ssl->biowr->type == WOLFSSL_BIO_SOCKET) {
        #ifdef USE_WOLFSSL_IO
            sent = TranslateIoError(sent);
        #endif
            return sent;
        }
        else if (ssl->biowr->type == WOLFSSL_BIO_BIO) {
            if (sent == WOLFSSL_BIO_ERROR) {
                WOLFSSL_MSG("\tWould Block");
                return WOLFSSL_CBIO_ERR_WANT_WRITE;
            }
        }

        /* If retry and write flags are set, return WANT_WRITE */
        if ((ssl->biord->flags & WOLFSSL_BIO_FLAG_WRITE) &&
            (ssl->biord->flags & WOLFSSL_BIO_FLAG_RETRY)) {
            return WOLFSSL_CBIO_ERR_WANT_WRITE;
        }

        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    (void)ctx;

    return sent;
}
#endif /* !NO_BIO */
#endif /* OPENSSL_EXTRA */


#ifdef USE_WOLFSSL_IO

/* The receive embedded callback
 *  return : nb bytes read, or error
 */
int EmbedReceive(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    int recvd;
#ifndef WOLFSSL_LINUXKM
    int sd = *(int*)ctx;
#else
    struct socket *sd = (struct socket*)ctx;
#endif

    recvd = wolfIO_Recv(sd, buf, sz, ssl->rflags);
    if (recvd < 0) {
        WOLFSSL_MSG("Embed Receive error");
        return TranslateIoError(recvd);
    }
    else if (recvd == 0) {
        WOLFSSL_MSG("Embed receive connection closed");
        return WOLFSSL_CBIO_ERR_CONN_CLOSE;
    }

    return recvd;
}

/* The send embedded callback
 *  return : nb bytes sent, or error
 */
int EmbedSend(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    int sent;
#ifndef WOLFSSL_LINUXKM
    int sd = *(int*)ctx;
#else
    struct socket *sd = (struct socket*)ctx;
#endif

#ifdef WOLFSSL_MAX_SEND_SZ
    if (sz > WOLFSSL_MAX_SEND_SZ)
        sz = WOLFSSL_MAX_SEND_SZ;
#endif

    sent = wolfIO_Send(sd, buf, sz, ssl->wflags);
    if (sent < 0) {
        WOLFSSL_MSG("Embed Send error");
        return TranslateIoError(sent);
    }

    return sent;
}


#ifdef WOLFSSL_DTLS

#include <wolfssl/wolfcrypt/sha.h>

#ifndef DTLS_SENDTO_FUNCTION
    #define DTLS_SENDTO_FUNCTION sendto
#endif
#ifndef DTLS_RECVFROM_FUNCTION
    #define DTLS_RECVFROM_FUNCTION recvfrom
#endif

static int sockAddrEqual(
    SOCKADDR_S *a, XSOCKLENT aLen, SOCKADDR_S *b, XSOCKLENT bLen)
{
    if (aLen != bLen)
        return 0;

    if (a->ss_family != b->ss_family)
        return 0;

    if (a->ss_family == WOLFSSL_IP4) {

        if (aLen < (XSOCKLENT)sizeof(SOCKADDR_IN))
            return 0;

        if (((SOCKADDR_IN*)a)->sin_port != ((SOCKADDR_IN*)b)->sin_port)
            return 0;

        if (((SOCKADDR_IN*)a)->sin_addr.s_addr !=
            ((SOCKADDR_IN*)b)->sin_addr.s_addr)
            return 0;

        return 1;
    }

#ifdef WOLFSSL_IPV6
    if (a->ss_family == WOLFSSL_IP6) {
        SOCKADDR_IN6 *a6, *b6;

        if (aLen < (XSOCKLENT)sizeof(SOCKADDR_IN6))
            return 0;

        a6 = (SOCKADDR_IN6*)a;
        b6 = (SOCKADDR_IN6*)b;

        if (((SOCKADDR_IN6*)a)->sin6_port != ((SOCKADDR_IN6*)b)->sin6_port)
            return 0;

        if (XMEMCMP((void*)&a6->sin6_addr, (void*)&b6->sin6_addr,
                sizeof(a6->sin6_addr)) != 0)
            return 0;

        return 1;
    }
#endif /* WOLFSSL_IPV6 */

    return 0;
}

#ifndef WOLFSSL_IPV6
static int PeerIsIpv6(const SOCKADDR_S *peer, XSOCKLENT len)
{
    if (len < (XSOCKLENT)sizeof(peer->ss_family))
        return 0;
    return peer->ss_family == WOLFSSL_IP6;
}
#endif /* !WOLFSSL_IPV6 */

static int isDGramSock(int sfd)
{
    int type = 0;
    /* optvalue 'type' is of size int */
    XSOCKLENT length = (XSOCKLENT)sizeof(type);

    if (getsockopt(sfd, SOL_SOCKET, SO_TYPE, (XSOCKOPT_TYPE_OPTVAL_TYPE)&type,
            &length) == 0 && type != SOCK_DGRAM) {
        return 0;
    }
    else {
        return 1;
    }
}

/* The receive embedded callback
 *  return : nb bytes read, or error
 */
int EmbedReceiveFrom(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    int recvd;
    int sd = dtlsCtx->rfd;
    int dtls_timeout = wolfSSL_dtls_get_current_timeout(ssl);
    byte doDtlsTimeout;
    SOCKADDR_S lclPeer;
    SOCKADDR_S* peer;
    XSOCKLENT peerSz = 0;
#ifndef NO_ASN_TIME
    word32 start = 0;
#elif !defined(DTLS_RECEIVEFROM_NO_TIMEOUT_ON_INVALID_PEER)
    word32 invalidPeerPackets = 0;
#endif

    WOLFSSL_ENTER("EmbedReceiveFrom");

    if (dtlsCtx->connected) {
        peer = NULL;
    }
    else if (dtlsCtx->userSet) {
#ifndef WOLFSSL_IPV6
        if (PeerIsIpv6((SOCKADDR_S*)dtlsCtx->peer.sa, dtlsCtx->peer.sz)) {
            WOLFSSL_MSG("ipv6 dtls peer set but no ipv6 support compiled");
            return NOT_COMPILED_IN;
        }
#endif
        peer = &lclPeer;
        XMEMSET(&lclPeer, 0, sizeof(lclPeer));
        peerSz = sizeof(lclPeer);
    }
    else {
        /* Store the peer address. It is used to calculate the DTLS cookie. */
        if (dtlsCtx->peer.sa == NULL) {
            dtlsCtx->peer.sa = (void*)XMALLOC(sizeof(SOCKADDR_S),
                    ssl->heap, DYNAMIC_TYPE_SOCKADDR);
            dtlsCtx->peer.sz = 0;
            if (dtlsCtx->peer.sa != NULL)
                dtlsCtx->peer.bufSz = sizeof(SOCKADDR_S);
            else
                dtlsCtx->peer.bufSz = 0;
        }
        peer = (SOCKADDR_S*)dtlsCtx->peer.sa;
        peerSz = dtlsCtx->peer.bufSz;
    }

    /* Don't use ssl->options.handShakeDone since it is true even if
     * we are in the process of renegotiation */
    doDtlsTimeout = ssl->options.handShakeState != HANDSHAKE_DONE;

#ifdef WOLFSSL_DTLS13
    if (ssl->options.dtls && IsAtLeastTLSv1_3(ssl->version)) {
        doDtlsTimeout =
            doDtlsTimeout || ssl->dtls13Rtx.rtxRecords != NULL ||
            (ssl->dtls13FastTimeout && ssl->dtls13Rtx.seenRecords != NULL);
    }
#endif /* WOLFSSL_DTLS13 */

    do {

        if (!doDtlsTimeout) {
            dtls_timeout = 0;
        }
        else {
#ifndef NO_ASN_TIME
            if (start == 0) {
                start = LowResTimer();
            }
            else {
                dtls_timeout -= LowResTimer() - start;
                start = LowResTimer();
                if (dtls_timeout < 0 || dtls_timeout > DTLS_TIMEOUT_MAX)
                    return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
#endif
        }

        if (!wolfSSL_get_using_nonblock(ssl)) {
        #ifdef USE_WINDOWS_API
            DWORD timeout = dtls_timeout * 1000;
            #ifdef WOLFSSL_DTLS13
            if (wolfSSL_dtls13_use_quick_timeout(ssl) &&
                IsAtLeastTLSv1_3(ssl->version))
                timeout /= 4;
            #endif /* WOLFSSL_DTLS13 */
        #else
            struct timeval timeout;
            XMEMSET(&timeout, 0, sizeof(timeout));
            #ifdef WOLFSSL_DTLS13
            if (wolfSSL_dtls13_use_quick_timeout(ssl) &&
                IsAtLeastTLSv1_3(ssl->version)) {
                if (dtls_timeout >= 4)
                    timeout.tv_sec = dtls_timeout / 4;
                else
                    timeout.tv_usec = dtls_timeout * 1000000 / 4;
            }
            else
            #endif /* WOLFSSL_DTLS13 */
                timeout.tv_sec = dtls_timeout;
        #endif
            if (setsockopt(sd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout,
                    sizeof(timeout)) != 0) {
                WOLFSSL_MSG("setsockopt rcvtimeo failed");
            }
        }
#ifndef NO_ASN_TIME
        else if (IsSCR(ssl)) {
            if (ssl->dtls_start_timeout &&
                LowResTimer() - ssl->dtls_start_timeout >
                    (word32)dtls_timeout) {
                ssl->dtls_start_timeout = 0;
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
            else if (!ssl->dtls_start_timeout) {
                ssl->dtls_start_timeout = LowResTimer();
            }
        }
#endif /* !NO_ASN_TIME */

        recvd = (int)DTLS_RECVFROM_FUNCTION(sd, buf, sz, ssl->rflags,
            (SOCKADDR*)peer, peer != NULL ? &peerSz : NULL);

        /* From the RECV(2) man page
         * The returned address is truncated if the buffer provided is too
         * small; in this case, addrlen will return a value greater than was
         * supplied to the call.
         */
        if (dtlsCtx->connected) {
            /* No need to sanitize the value of peerSz */
        }
        else if (dtlsCtx->userSet) {
            /* Truncate peer size */
            if (peerSz > (XSOCKLENT)sizeof(lclPeer))
                peerSz = (XSOCKLENT)sizeof(lclPeer);
        }
        else {
            /* Truncate peer size */
            if (peerSz > (XSOCKLENT)dtlsCtx->peer.bufSz)
                peerSz = (XSOCKLENT)dtlsCtx->peer.bufSz;
        }

        recvd = TranslateReturnCode(recvd, sd);

        if (recvd < 0) {
            WOLFSSL_MSG("Embed Receive From error");
            recvd = TranslateIoError(recvd);
            if (recvd == WOLFSSL_CBIO_ERR_WANT_READ &&
                !wolfSSL_dtls_get_using_nonblock(ssl)) {
                recvd = WOLFSSL_CBIO_ERR_TIMEOUT;
            }
            return recvd;
        }
        else if (recvd == 0) {
            if (!isDGramSock(sd)) {
                /* Closed TCP connection */
                recvd = WOLFSSL_CBIO_ERR_CONN_CLOSE;
            }
            else {
                WOLFSSL_MSG("Ignoring 0-length datagram");
                continue;
            }
            return recvd;
        }
        else if (dtlsCtx->connected) {
            /* Nothing to do */
        }
        else if (dtlsCtx->userSet) {
            /* Check we received the packet from the correct peer */
            if (dtlsCtx->peer.sz > 0 &&
                (peerSz != (XSOCKLENT)dtlsCtx->peer.sz ||
                    !sockAddrEqual(peer, peerSz, (SOCKADDR_S*)dtlsCtx->peer.sa,
                        dtlsCtx->peer.sz))) {
                WOLFSSL_MSG("    Ignored packet from invalid peer");
#if defined(NO_ASN_TIME) &&                                                    \
    !defined(DTLS_RECEIVEFROM_NO_TIMEOUT_ON_INVALID_PEER)
                if (doDtlsTimeout) {
                    invalidPeerPackets++;
                    if (invalidPeerPackets > DTLS_RECEIVEFROM_MAX_INVALID_PEER)
                        return wolfSSL_dtls_get_using_nonblock(ssl)
                                   ? WOLFSSL_CBIO_ERR_WANT_READ
                                   : WOLFSSL_CBIO_ERR_TIMEOUT;
                }
#endif /* NO_ASN_TIME && !DTLS_RECEIVEFROM_NO_TIMEOUT_ON_INVALID_PEER */
                continue;
            }
        }
        else {
            /* Store size of saved address */
            dtlsCtx->peer.sz = peerSz;
        }
#ifndef NO_ASN_TIME
        ssl->dtls_start_timeout = 0;
#endif /* !NO_ASN_TIME */
        break;
    } while (1);

    return recvd;
}


/* The send embedded callback
 *  return : nb bytes sent, or error
 */
int EmbedSendTo(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    int sd = dtlsCtx->wfd;
    int sent;
    const SOCKADDR_S* peer = NULL;
    XSOCKLENT peerSz = 0;

    WOLFSSL_ENTER("EmbedSendTo");

    if (!isDGramSock(sd)) {
        /* Probably a TCP socket. peer and peerSz MUST be NULL and 0 */
    }
    else if (!dtlsCtx->connected) {
        peer   = (const SOCKADDR_S*)dtlsCtx->peer.sa;
        peerSz = dtlsCtx->peer.sz;
#ifndef WOLFSSL_IPV6
        if (PeerIsIpv6(peer, peerSz)) {
            WOLFSSL_MSG("ipv6 dtls peer set but no ipv6 support compiled");
            return NOT_COMPILED_IN;
        }
#endif
    }

    sent = (int)DTLS_SENDTO_FUNCTION(sd, buf, sz, ssl->wflags,
            (const SOCKADDR*)peer, peerSz);

    sent = TranslateReturnCode(sent, sd);

    if (sent < 0) {
        WOLFSSL_MSG("Embed Send To error");
        return TranslateIoError(sent);
    }

    return sent;
}


#ifdef WOLFSSL_MULTICAST

/* The alternate receive embedded callback for Multicast
 *  return : nb bytes read, or error
 */
int EmbedReceiveFromMcast(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    int recvd;
    int sd = dtlsCtx->rfd;

    WOLFSSL_ENTER("EmbedReceiveFromMcast");

    recvd = (int)DTLS_RECVFROM_FUNCTION(sd, buf, sz, ssl->rflags, NULL, NULL);

    recvd = TranslateReturnCode(recvd, sd);

    if (recvd < 0) {
        WOLFSSL_MSG("Embed Receive From error");
        recvd = TranslateIoError(recvd);
        if (recvd == WOLFSSL_CBIO_ERR_WANT_READ &&
            !wolfSSL_dtls_get_using_nonblock(ssl)) {
            recvd = WOLFSSL_CBIO_ERR_TIMEOUT;
        }
        return recvd;
    }

    return recvd;
}
#endif /* WOLFSSL_MULTICAST */


/* The DTLS Generate Cookie callback
 *  return : number of bytes copied into buf, or error
 */
int EmbedGenerateCookie(WOLFSSL* ssl, byte *buf, int sz, void *ctx)
{
    int sd = ssl->wfd;
    SOCKADDR_S peer;
    XSOCKLENT peerSz = sizeof(peer);
    byte digest[WC_SHA256_DIGEST_SIZE];
    int  ret = 0;

    (void)ctx;

    XMEMSET(&peer, 0, sizeof(peer));
    if (getpeername(sd, (SOCKADDR*)&peer, &peerSz) != 0) {
        WOLFSSL_MSG("getpeername failed in EmbedGenerateCookie");
        return GEN_COOKIE_E;
    }

    ret = wc_Sha256Hash((byte*)&peer, peerSz, digest);
    if (ret != 0)
        return ret;

    if (sz > WC_SHA256_DIGEST_SIZE)
        sz = WC_SHA256_DIGEST_SIZE;
    XMEMCPY(buf, digest, sz);

    return sz;
}
#endif /* WOLFSSL_DTLS */

#ifdef WOLFSSL_SESSION_EXPORT

#ifdef WOLFSSL_DTLS
    static int EmbedGetPeerDTLS(WOLFSSL* ssl, char* ip, int* ipSz,
                                                 unsigned short* port, int* fam)
    {
        SOCKADDR_S peer;
        word32     peerSz;
        int        ret;

        /* get peer information stored in ssl struct */
        peerSz = sizeof(SOCKADDR_S);
        if ((ret = wolfSSL_dtls_get_peer(ssl, (void*)&peer, &peerSz))
                                                               != WOLFSSL_SUCCESS) {
            return ret;
        }

        /* extract family, ip, and port */
        *fam = ((SOCKADDR_S*)&peer)->ss_family;
        switch (*fam) {
            case WOLFSSL_IP4:
                if (XINET_NTOP(*fam, &(((SOCKADDR_IN*)&peer)->sin_addr),
                                                           ip, *ipSz) == NULL) {
                    WOLFSSL_MSG("XINET_NTOP error");
                    return SOCKET_ERROR_E;
                }
                *port = XNTOHS(((SOCKADDR_IN*)&peer)->sin_port);
                break;

            case WOLFSSL_IP6:
            #ifdef WOLFSSL_IPV6
                if (XINET_NTOP(*fam, &(((SOCKADDR_IN6*)&peer)->sin6_addr),
                                                           ip, *ipSz) == NULL) {
                    WOLFSSL_MSG("XINET_NTOP error");
                    return SOCKET_ERROR_E;
                }
                *port = XNTOHS(((SOCKADDR_IN6*)&peer)->sin6_port);
            #endif /* WOLFSSL_IPV6 */
                break;

            default:
                WOLFSSL_MSG("Unknown family type");
                return SOCKET_ERROR_E;
        }
        ip[*ipSz - 1] = '\0'; /* make sure has terminator */
        *ipSz = (word16)XSTRLEN(ip);

        return WOLFSSL_SUCCESS;
    }

    static int EmbedSetPeerDTLS(WOLFSSL* ssl, char* ip, int ipSz,
                                                   unsigned short port, int fam)
    {
        int    ret;
        SOCKADDR_S addr;

        /* sanity checks on arguments */
        if (ssl == NULL || ip == NULL || ipSz < 0 || ipSz > MAX_EXPORT_IP) {
            return BAD_FUNC_ARG;
        }

        addr.ss_family = fam;
        switch (addr.ss_family) {
            case WOLFSSL_IP4:
                if (XINET_PTON(addr.ss_family, ip,
                                     &(((SOCKADDR_IN*)&addr)->sin_addr)) <= 0) {
                    WOLFSSL_MSG("XINET_PTON error");
                    return SOCKET_ERROR_E;
                }
                ((SOCKADDR_IN*)&addr)->sin_port = XHTONS(port);

                /* peer sa is free'd in SSL_ResourceFree */
                if ((ret = wolfSSL_dtls_set_peer(ssl, (SOCKADDR_IN*)&addr,
                                          sizeof(SOCKADDR_IN)))!= WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Import DTLS peer info error");
                    return ret;
                }
                break;

            case WOLFSSL_IP6:
            #ifdef WOLFSSL_IPV6
                if (XINET_PTON(addr.ss_family, ip,
                                   &(((SOCKADDR_IN6*)&addr)->sin6_addr)) <= 0) {
                    WOLFSSL_MSG("XINET_PTON error");
                    return SOCKET_ERROR_E;
                }
                ((SOCKADDR_IN6*)&addr)->sin6_port = XHTONS(port);

                /* peer sa is free'd in SSL_ResourceFree */
                if ((ret = wolfSSL_dtls_set_peer(ssl, (SOCKADDR_IN6*)&addr,
                                         sizeof(SOCKADDR_IN6)))!= WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Import DTLS peer info error");
                    return ret;
                }
            #endif /* WOLFSSL_IPV6 */
                break;

            default:
                WOLFSSL_MSG("Unknown address family");
                return BUFFER_E;
        }

        return WOLFSSL_SUCCESS;
    }
#endif

    /* get the peer information in human readable form (ip, port, family)
     * default function assumes BSD sockets
     * can be overridden with wolfSSL_CTX_SetIOGetPeer
     */
    int EmbedGetPeer(WOLFSSL* ssl, char* ip, int* ipSz,
                                                 unsigned short* port, int* fam)
    {
        if (ssl == NULL || ip == NULL || ipSz == NULL ||
                                                  port == NULL || fam == NULL) {
            return BAD_FUNC_ARG;
        }

        if (ssl->options.dtls) {
        #ifdef WOLFSSL_DTLS
            return EmbedGetPeerDTLS(ssl, ip, ipSz, port, fam);
        #else
            return NOT_COMPILED_IN;
        #endif
        }
        else {
            *port = wolfSSL_get_fd(ssl);
            ip[0] = '\0';
            *ipSz = 0;
            *fam  = 0;
            return WOLFSSL_SUCCESS;
        }
    }

    /* set the peer information in human readable form (ip, port, family)
     * default function assumes BSD sockets
     * can be overridden with wolfSSL_CTX_SetIOSetPeer
     */
    int EmbedSetPeer(WOLFSSL* ssl, char* ip, int ipSz,
                                                   unsigned short port, int fam)
    {
        /* sanity checks on arguments */
        if (ssl == NULL || ip == NULL || ipSz < 0 || ipSz > MAX_EXPORT_IP) {
            return BAD_FUNC_ARG;
        }

        if (ssl->options.dtls) {
        #ifdef WOLFSSL_DTLS
            return EmbedSetPeerDTLS(ssl, ip, ipSz, port, fam);
        #else
            return NOT_COMPILED_IN;
        #endif
        }
        else {
            wolfSSL_set_fd(ssl, port);
            (void)fam;
            return WOLFSSL_SUCCESS;
        }
    }
#endif /* WOLFSSL_SESSION_EXPORT */

#ifdef WOLFSSL_LINUXKM
static int linuxkm_send(struct socket *socket, void *buf, int size,
    unsigned int flags)
{
    int ret;
    struct kvec vec = { .iov_base = buf, .iov_len = size };
    struct msghdr msg = { .msg_flags = flags };
    ret = kernel_sendmsg(socket, &msg, &vec, 1, size);
    return ret;
}

static int linuxkm_recv(struct socket *socket, void *buf, int size,
    unsigned int flags)
{
    int ret;
    struct kvec vec = { .iov_base = buf, .iov_len = size };
    struct msghdr msg = { .msg_flags = flags };
    ret = kernel_recvmsg(socket, &msg, &vec, 1, size, msg.msg_flags);
    return ret;
}
#endif /* WOLFSSL_LINUXKM */


int wolfIO_Recv(SOCKET_T sd, char *buf, int sz, int rdFlags)
{
    int recvd;

    recvd = (int)RECV_FUNCTION(sd, buf, sz, rdFlags);
    recvd = TranslateReturnCode(recvd, (int)sd);

    return recvd;
}

int wolfIO_Send(SOCKET_T sd, char *buf, int sz, int wrFlags)
{
    int sent;

    sent = (int)SEND_FUNCTION(sd, buf, sz, wrFlags);
    sent = TranslateReturnCode(sent, (int)sd);

    return sent;
}

#endif /* USE_WOLFSSL_IO */


#ifdef HAVE_HTTP_CLIENT

#ifndef HAVE_IO_TIMEOUT
    #define io_timeout_sec 0
#else

    #ifndef DEFAULT_TIMEOUT_SEC
        #define DEFAULT_TIMEOUT_SEC 0 /* no timeout */
    #endif

    static int io_timeout_sec = DEFAULT_TIMEOUT_SEC;

    void wolfIO_SetTimeout(int to_sec)
    {
        io_timeout_sec = to_sec;
    }

    int wolfIO_SetBlockingMode(SOCKET_T sockfd, int non_blocking)
    {
        int ret = 0;

    #ifdef USE_WINDOWS_API
        unsigned long blocking = non_blocking;
        ret = ioctlsocket(sockfd, FIONBIO, &blocking);
        if (ret == SOCKET_ERROR)
            ret = -1;
    #else
        ret = fcntl(sockfd, F_GETFL, 0);
        if (ret >= 0) {
            if (non_blocking)
                ret |= O_NONBLOCK;
            else
                ret &= ~O_NONBLOCK;
            ret = fcntl(sockfd, F_SETFL, ret);
        }
    #endif
        if (ret < 0) {
            WOLFSSL_MSG("wolfIO_SetBlockingMode failed");
        }

        return ret;
    }

    int wolfIO_Select(SOCKET_T sockfd, int to_sec)
    {
        fd_set rfds, wfds;
        int nfds = 0;
        struct timeval timeout = { (to_sec > 0) ? to_sec : 0, 0};
        int ret;

    #ifndef USE_WINDOWS_API
        nfds = (int)sockfd + 1;

        if ((sockfd < 0) || (sockfd >= FD_SETSIZE)) {
            WOLFSSL_MSG("socket fd out of FDSET range");
            return -1;
        }
    #endif

        FD_ZERO(&rfds);
        FD_SET(sockfd, &rfds);
        wfds = rfds;

        ret = select(nfds, &rfds, &wfds, NULL, &timeout);
        if (ret == 0) {
        #ifdef DEBUG_HTTP
            fprintf(stderr, "Timeout: %d\n", ret);
        #endif
            return HTTP_TIMEOUT;
        }
        else if (ret > 0) {
            if (FD_ISSET(sockfd, &wfds)) {
                if (!FD_ISSET(sockfd, &rfds)) {
                    return 0;
                }
            }
        }

        WOLFSSL_MSG("Select error");
        return SOCKET_ERROR_E;
    }
#endif /* HAVE_IO_TIMEOUT */

static int wolfIO_Word16ToString(char* d, word16 number)
{
    int i = 0;
    word16 order = 10000;
    word16 digit;

    if (d == NULL)
        return i;

    if (number == 0)
        d[i++] = '0';
    else {
        while (order) {
            digit = number / order;
            if (i > 0 || digit != 0)
                d[i++] = (char)digit + '0';
            if (digit != 0)
                number %= digit * order;

            order = (order > 1) ? order / 10 : 0;
        }
    }
    d[i] = 0; /* null terminate */

    return i;
}

int wolfIO_TcpConnect(SOCKET_T* sockfd, const char* ip, word16 port, int to_sec)
{
#ifdef HAVE_SOCKADDR
    int ret = 0;
    SOCKADDR_S addr;
    int sockaddr_len;
#if defined(HAVE_GETADDRINFO)
    /* use getaddrinfo */
    ADDRINFO hints;
    ADDRINFO* answer = NULL;
    char strPort[6];
#else
    /* use gethostbyname */
#if !defined(WOLFSSL_USE_POPEN_HOST)
#if defined(__GLIBC__) && (__GLIBC__ >= 2) && defined(__USE_MISC) && \
    !defined(SINGLE_THREADED)
    HOSTENT entry_buf, *entry = NULL;
    char *ghbn_r_buf = NULL;
    int ghbn_r_errno;
#else
    HOSTENT *entry;
#endif
#endif
#ifdef WOLFSSL_IPV6
    SOCKADDR_IN6 *sin;
#else
    SOCKADDR_IN *sin;
#endif
#endif /* HAVE_SOCKADDR */

    if (sockfd == NULL || ip == NULL) {
        return -1;
    }

#if !defined(HAVE_GETADDRINFO)
#ifdef WOLFSSL_IPV6
    sockaddr_len = sizeof(SOCKADDR_IN6);
#else
    sockaddr_len = sizeof(SOCKADDR_IN);
#endif
#endif
    XMEMSET(&addr, 0, sizeof(addr));

#ifdef WOLFIO_DEBUG
    printf("TCP Connect: %s:%d\n", ip, port);
#endif

    /* use gethostbyname for c99 */
#if defined(HAVE_GETADDRINFO)
    XMEMSET(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC; /* detect IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (wolfIO_Word16ToString(strPort, port) == 0) {
        WOLFSSL_MSG("invalid port number for responder");
        return -1;
    }

    if (getaddrinfo(ip, strPort, &hints, &answer) < 0 || answer == NULL) {
        WOLFSSL_MSG("no addr info for responder");
        return -1;
    }

    sockaddr_len = answer->ai_addrlen;
    XMEMCPY(&addr, answer->ai_addr, sockaddr_len);
    freeaddrinfo(answer);
#elif defined(WOLFSSL_USE_POPEN_HOST) && !defined(WOLFSSL_IPV6)
    {
        char host_ipaddr[4] = { 127, 0, 0, 1 };
        int found = 1;

        if ((XSTRNCMP(ip, "localhost", 10) != 0) &&
            (XSTRNCMP(ip, "127.0.0.1", 10) != 0)) {
            FILE* fp;
            char host_out[100];
            char cmd[100];

            XSTRNCPY(cmd, "host ", 6);
            XSTRNCAT(cmd, ip, 99 - XSTRLEN(cmd));
            found = 0;
            fp = popen(cmd, "r");
            if (fp != NULL) {
                while (fgets(host_out, sizeof(host_out), fp) != NULL) {
                    int i;
                    int j = 0;
                    for (j = 0; host_out[j] != '\0'; j++) {
                        if ((host_out[j] >= '0') && (host_out[j] <= '9')) {
                            break;
                        }
                    }
                    found = (host_out[j] >= '0') && (host_out[j] <= '9');
                    if (!found) {
                        continue;
                    }

                    for (i = 0; i < 4; i++) {
                        host_ipaddr[i] = atoi(host_out + j);
                        while ((host_out[j] >= '0') && (host_out[j] <= '9')) {
                            j++;
                        }
                        if (host_out[j] == '.') {
                            j++;
                            found &= (i != 3);
                        }
                        else {
                            found &= (i == 3);
                            break;
                        }
                    }
                    if (found) {
                        break;
                    }
                }
                pclose(fp);
            }
        }
        if (found) {
            sin = (SOCKADDR_IN *)&addr;
            sin->sin_family = AF_INET;
            sin->sin_port = XHTONS(port);
            XMEMCPY(&sin->sin_addr.s_addr, host_ipaddr, sizeof(host_ipaddr));
        }
        else {
            WOLFSSL_MSG("no addr info for responder");
            return -1;
        }
    }
#else
#if defined(__GLIBC__) && (__GLIBC__ >= 2) && defined(__USE_MISC) && \
    !defined(SINGLE_THREADED)
    /* 2048 is a magic number that empirically works.  the header and
     * documentation provide no guidance on appropriate buffer size other than
     * "if buf is too small, the functions will return ERANGE, and the call
     * should be retried with a larger buffer."
     */
    ghbn_r_buf = (char *)XMALLOC(2048, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (ghbn_r_buf != NULL) {
        gethostbyname_r(ip, &entry_buf, ghbn_r_buf, 2048, &entry, &ghbn_r_errno);
    }
#else
    entry = gethostbyname(ip);
#endif

    if (entry) {
    #ifdef WOLFSSL_IPV6
        sin = (SOCKADDR_IN6 *)&addr;
        sin->sin6_family = AF_INET6;
        sin->sin6_port = XHTONS(port);
        XMEMCPY(&sin->sin6_addr, entry->h_addr_list[0], entry->h_length);
    #else
        sin = (SOCKADDR_IN *)&addr;
        sin->sin_family = AF_INET;
        sin->sin_port = XHTONS(port);
        XMEMCPY(&sin->sin_addr.s_addr, entry->h_addr_list[0], entry->h_length);
    #endif
    }

#if defined(__GLIBC__) && (__GLIBC__ >= 2) && defined(__USE_MISC) && \
    !defined(SINGLE_THREADED)
    XFREE(ghbn_r_buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (entry == NULL) {
        WOLFSSL_MSG("no addr info for responder");
        return -1;
    }
#endif

    *sockfd = (SOCKET_T)socket(addr.ss_family, SOCK_STREAM, 0);
#ifdef USE_WINDOWS_API
    if (*sockfd == SOCKET_INVALID)
#else
    if (*sockfd <= SOCKET_INVALID)
#endif
    {
        WOLFSSL_MSG("bad socket fd, out of fds?");
        *sockfd = SOCKET_INVALID;
        return -1;
    }

#ifdef HAVE_IO_TIMEOUT
    /* if timeout value provided then set socket non-blocking */
    if (to_sec > 0) {
        wolfIO_SetBlockingMode(*sockfd, 1);
    }
#else
    (void)to_sec;
#endif

    ret = connect(*sockfd, (SOCKADDR *)&addr, sockaddr_len);
#ifdef HAVE_IO_TIMEOUT
    if ((ret != 0) && (to_sec > 0)) {
#ifdef USE_WINDOWS_API
        if ((ret == SOCKET_ERROR) && (wolfSSL_LastError(ret) == WSAEWOULDBLOCK))
#else
        if (errno == EINPROGRESS)
#endif
        {
            /* wait for connect to complete */
            ret = wolfIO_Select(*sockfd, to_sec);

            /* restore blocking mode */
            wolfIO_SetBlockingMode(*sockfd, 0);
        }
    }
#endif
    if (ret != 0) {
        WOLFSSL_MSG("Responder tcp connect failed");
        CloseSocket(*sockfd);
        *sockfd = SOCKET_INVALID;
        return -1;
    }
    return ret;
#else
    (void)sockfd;
    (void)ip;
    (void)port;
    (void)to_sec;
    return -1;
#endif /* HAVE_SOCKADDR */
}

int wolfIO_TcpBind(SOCKET_T* sockfd, word16 port)
{
#ifdef HAVE_SOCKADDR
    int ret = 0;
    SOCKADDR_S addr;
    int sockaddr_len = sizeof(SOCKADDR_IN);
    SOCKADDR_IN *sin = (SOCKADDR_IN *)&addr;

    if (sockfd == NULL || port < 1) {
        return -1;
    }

    XMEMSET(&addr, 0, sizeof(addr));

    sin->sin_family = AF_INET;
    sin->sin_addr.s_addr = INADDR_ANY;
    sin->sin_port = XHTONS(port);
    *sockfd = (SOCKET_T)socket(AF_INET, SOCK_STREAM, 0);

#ifdef USE_WINDOWS_API
    if (*sockfd == SOCKET_INVALID)
#else
    if (*sockfd <= SOCKET_INVALID)
#endif
    {
        WOLFSSL_MSG("socket failed");
        *sockfd = SOCKET_INVALID;
        return -1;
    }

#if !defined(USE_WINDOWS_API) && !defined(WOLFSSL_MDK_ARM)\
                   && !defined(WOLFSSL_KEIL_TCP_NET) && !defined(WOLFSSL_ZEPHYR)
    {
        int optval  = 1;
        XSOCKLENT optlen = sizeof(optval);
        ret = setsockopt(*sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, optlen);
    }
#endif

    if (ret == 0)
        ret = bind(*sockfd, (SOCKADDR *)sin, sockaddr_len);
    if (ret == 0)
        ret = listen(*sockfd, SOMAXCONN);

    if (ret != 0) {
        WOLFSSL_MSG("wolfIO_TcpBind failed");
        CloseSocket(*sockfd);
        *sockfd = SOCKET_INVALID;
        ret = -1;
    }

    return ret;
#else
    (void)sockfd;
    (void)port;
    return -1;
#endif /* HAVE_SOCKADDR */
}

#ifdef HAVE_SOCKADDR
int wolfIO_TcpAccept(SOCKET_T sockfd, SOCKADDR* peer_addr, XSOCKLENT* peer_len)
{
    return (int)accept(sockfd, peer_addr, peer_len);
}
#endif /* HAVE_SOCKADDR */

#ifndef HTTP_SCRATCH_BUFFER_SIZE
    #define HTTP_SCRATCH_BUFFER_SIZE 512
#endif
#ifndef MAX_URL_ITEM_SIZE
    #define MAX_URL_ITEM_SIZE   80
#endif

int wolfIO_DecodeUrl(const char* url, int urlSz, char* outName, char* outPath,
    word16* outPort)
{
    int result = -1;

    if (url == NULL || urlSz == 0) {
        if (outName)
            *outName = 0;
        if (outPath)
            *outPath = 0;
        if (outPort)
            *outPort = 0;
    }
    else {
        int i, cur;

        /* need to break the url down into scheme, address, and port */
        /*     "http://example.com:8080/" */
        /*     "http://[::1]:443/"        */
        if (XSTRNCMP(url, "http://", 7) == 0) {
            cur = 7;
        } else cur = 0;

        i = 0;
        if (url[cur] == '[') {
            cur++;
            /* copy until ']' */
            while (i < MAX_URL_ITEM_SIZE-1 && cur < urlSz && url[cur] != 0 &&
                    url[cur] != ']') {
                if (outName)
                    outName[i] = url[cur];
                i++; cur++;
            }
            cur++; /* skip ']' */
        }
        else {
            while (i < MAX_URL_ITEM_SIZE-1 && cur < urlSz && url[cur] != 0 &&
                    url[cur] != ':' && url[cur] != '/') {
                if (outName)
                    outName[i] = url[cur];
                i++; cur++;
            }
        }
        if (outName)
            outName[i] = 0;
        /* Need to pick out the path after the domain name */

        if (cur < urlSz && url[cur] == ':') {
            char port[6];
            int j;
            word32 bigPort = 0;
            i = 0;
            cur++;
            while (i < 6 && cur < urlSz && url[cur] != 0 && url[cur] != '/') {
                port[i] = url[cur];
                i++; cur++;
            }

            for (j = 0; j < i; j++) {
                if (port[j] < '0' || port[j] > '9') return -1;
                bigPort = (bigPort * 10) + (port[j] - '0');
            }
            if (outPort)
                *outPort = (word16)bigPort;
        }
        else if (outPort)
            *outPort = 80;


        if (cur < urlSz && url[cur] == '/') {
            i = 0;
            while (i < MAX_URL_ITEM_SIZE-1 && cur < urlSz && url[cur] != 0) {
                if (outPath)
                    outPath[i] = url[cur];
                i++; cur++;
            }
            if (outPath)
                outPath[i] = 0;
        }
        else if (outPath) {
            outPath[0] = '/';
            outPath[1] = 0;
        }

        result = 0;
    }

    return result;
}

static int wolfIO_HttpProcessResponseBuf(int sfd, byte **recvBuf,
    int* recvBufSz, int chunkSz, char* start, int len, int dynType, void* heap)
{
    byte* newRecvBuf = NULL;
    int newRecvSz = *recvBufSz + chunkSz;
    int pos = 0;

    WOLFSSL_MSG("Processing HTTP response");
#ifdef WOLFIO_DEBUG
    printf("HTTP Chunk %d->%d\n", *recvBufSz, chunkSz);
#endif

    (void)heap;
    (void)dynType;

    if (chunkSz < 0 || len < 0) {
        WOLFSSL_MSG("wolfIO_HttpProcessResponseBuf invalid chunk or length size");
        return MEMORY_E;
    }

    if (newRecvSz <= 0) {
        WOLFSSL_MSG("wolfIO_HttpProcessResponseBuf new receive size overflow");
        return MEMORY_E;
    }

    newRecvBuf = (byte*)XMALLOC(newRecvSz, heap, dynType);
    if (newRecvBuf == NULL) {
        WOLFSSL_MSG("wolfIO_HttpProcessResponseBuf malloc failed");
        return MEMORY_E;
    }

    /* if buffer already exists, then we are growing it */
    if (*recvBuf) {
        XMEMCPY(&newRecvBuf[pos], *recvBuf, *recvBufSz);
        XFREE(*recvBuf, heap, dynType);
        pos += *recvBufSz;
        *recvBuf = NULL;
    }

    /* copy the remainder of the httpBuf into the respBuf */
    if (len != 0) {
        if (pos + len <= newRecvSz) {
            XMEMCPY(&newRecvBuf[pos], start, len);
            pos += len;
        }
        else {
            WOLFSSL_MSG("wolfIO_HttpProcessResponseBuf bad size");
            XFREE(newRecvBuf, heap, dynType);
            return -1;
        }
    }

    /* receive the remainder of chunk */
    while (len < chunkSz) {
        int rxSz = wolfIO_Recv(sfd, (char*)&newRecvBuf[pos], chunkSz-len, 0);
        if (rxSz > 0) {
            len += rxSz;
            pos += rxSz;
        }
        else {
            WOLFSSL_MSG("wolfIO_HttpProcessResponseBuf recv failed");
            XFREE(newRecvBuf, heap, dynType);
            return -1;
        }
    }

    *recvBuf = newRecvBuf;
    *recvBufSz = newRecvSz;

    return 0;
}

int wolfIO_HttpProcessResponse(int sfd, const char** appStrList,
    byte** respBuf, byte* httpBuf, int httpBufSz, int dynType, void* heap)
{
    static const char HTTP_PROTO[] = "HTTP/1.";
    static const char HTTP_STATUS_200[] = "200";
    int result = 0;
    int len = 0;
    char *start, *end;
    int respBufSz = 0;
    int isChunked = 0, chunkSz = 0;
    enum phr_state { phr_init, phr_http_start, phr_have_length, phr_have_type,
                     phr_wait_end, phr_get_chunk_len, phr_get_chunk_data,
                     phr_http_end
    } state = phr_init;

    WOLFSSL_ENTER("wolfIO_HttpProcessResponse");

    *respBuf = NULL;
    start = end = NULL;
    do {
        if (state == phr_get_chunk_data) {
            /* get chunk of data */
            result = wolfIO_HttpProcessResponseBuf(sfd, respBuf, &respBufSz,
                chunkSz, start, len, dynType, heap);

            state = (result != 0) ? phr_http_end : phr_get_chunk_len;
            end = NULL;
            len = 0;
        }

        /* read data if no \r\n or first time */
        if ((start == NULL) || (end == NULL)) {
            result = wolfIO_Recv(sfd, (char*)httpBuf+len, httpBufSz-len-1, 0);
            if (result > 0) {
                len += result;
                start = (char*)httpBuf;
                start[len] = 0;
            }
            else {
                result = TranslateReturnCode(result, sfd);
                result = wolfSSL_LastError(result);
                if (result == SOCKET_EWOULDBLOCK || result == SOCKET_EAGAIN) {
                    return OCSP_WANT_READ;
                }

                WOLFSSL_MSG("wolfIO_HttpProcessResponse recv http from peer failed");
                return HTTP_RECV_ERR;
            }
        }
        end = XSTRSTR(start, "\r\n"); /* locate end */

        /* handle incomplete rx */
        if (end == NULL) {
            if (len != 0)
                XMEMMOVE(httpBuf, start, len);
            start = end = NULL;
        }
        /* when start is "\r\n" */
        else if (end == start) {
            /* if waiting for end or need chunk len */
            if (state == phr_wait_end || state == phr_get_chunk_len) {
                state = (isChunked) ? phr_get_chunk_len : phr_http_end;
                len -= 2; start += 2; /* skip \r\n */
             }
             else {
                WOLFSSL_MSG("wolfIO_HttpProcessResponse header ended early");
                return HTTP_HEADER_ERR;
             }
        }
        else {
            *end = 0; /* null terminate */
            len -= (int)(end - start) + 2;
                /* adjust len to remove the first line including the /r/n */

        #ifdef WOLFIO_DEBUG
            printf("HTTP Resp: %s\n", start);
        #endif

            switch (state) {
                case phr_init:
                    /* length of "HTTP/1.x 200" == 12*/
                    if (XSTRLEN(start) < 12) {
                        WOLFSSL_MSG("wolfIO_HttpProcessResponse HTTP header "
                            "too short.");
                        return HTTP_HEADER_ERR;
                    }
                    if (XSTRNCASECMP(start, HTTP_PROTO,
                                     sizeof(HTTP_PROTO) - 1) != 0) {
                        WOLFSSL_MSG("wolfIO_HttpProcessResponse HTTP header "
                            "doesn't start with HTTP/1.");
                        return HTTP_PROTO_ERR;
                    }
                    /* +2 for HTTP minor version and space between version and
                     * status code. */
                    start += sizeof(HTTP_PROTO) - 1 + 2 ;
                    if (XSTRNCASECMP(start, HTTP_STATUS_200,
                                     sizeof(HTTP_STATUS_200) - 1) != 0) {
                        WOLFSSL_MSG("wolfIO_HttpProcessResponse HTTP header "
                            "doesn't have status code 200.");
                        return HTTP_STATUS_ERR;
                    }
                    state = phr_http_start;
                    break;
                case phr_http_start:
                case phr_have_length:
                case phr_have_type:
                    if (XSTRNCASECMP(start, "Content-Type:", 13) == 0) {
                        int i;

                        start += 13;
                        while (*start == ' ') start++;

                        /* try and match against appStrList */
                        i = 0;
                        while (appStrList[i] != NULL) {
                            if (XSTRNCASECMP(start, appStrList[i],
                                                XSTRLEN(appStrList[i])) == 0) {
                                break;
                            }
                            i++;
                        }
                        if (appStrList[i] == NULL) {
                            WOLFSSL_MSG("wolfIO_HttpProcessResponse appstr mismatch");
                            return HTTP_APPSTR_ERR;
                        }
                        state = (state == phr_http_start) ? phr_have_type : phr_wait_end;
                    }
                    else if (XSTRNCASECMP(start, "Content-Length:", 15) == 0) {
                        start += 15;
                        while (*start == ' ') start++;
                        chunkSz = XATOI(start);
                        state = (state == phr_http_start) ? phr_have_length : phr_wait_end;
                    }
                    else if (XSTRNCASECMP(start, "Transfer-Encoding:", 18) == 0) {
                        start += 18;
                        while (*start == ' ') start++;
                        if (XSTRNCASECMP(start, "chunked", 7) == 0) {
                            isChunked = 1;
                            state = (state == phr_http_start) ? phr_have_length : phr_wait_end;
                        }
                    }
                    break;
                case phr_get_chunk_len:
                    chunkSz = (int)strtol(start, NULL, 16); /* hex format */
                    state = (chunkSz == 0) ? phr_http_end : phr_get_chunk_data;
                    break;
                case phr_get_chunk_data:
                    /* processing for chunk data done above, since \r\n isn't required */
                case phr_wait_end:
                case phr_http_end:
                    /* do nothing */
                    break;
            } /* switch (state) */

            /* skip to end plus \r\n */
            start = end + 2;
        }
    } while (state != phr_http_end);

    if (!isChunked) {
        result = wolfIO_HttpProcessResponseBuf(sfd, respBuf, &respBufSz, chunkSz,
                                                    start, len, dynType, heap);
    }

    if (result >= 0) {
        result = respBufSz;
    }
    else {
        WOLFSSL_ERROR(result);
    }

    return result;
}
int wolfIO_HttpBuildRequest(const char *reqType, const char *domainName,
                               const char *path, int pathLen, int reqSz, const char *contentType,
                               byte *buf, int bufSize)
{
    return wolfIO_HttpBuildRequest_ex(reqType, domainName, path, pathLen, reqSz, contentType, "", buf, bufSize);
}

    int wolfIO_HttpBuildRequest_ex(const char *reqType, const char *domainName,
                                const char *path, int pathLen, int reqSz, const char *contentType,
                                const char *exHdrs, byte *buf, int bufSize)
    {
    word32 reqTypeLen, domainNameLen, reqSzStrLen, contentTypeLen, exHdrsLen, maxLen;
    char reqSzStr[6];
    char* req = (char*)buf;
    const char* blankStr = " ";
    const char* http11Str = " HTTP/1.1";
    const char* hostStr = "\r\nHost: ";
    const char* contentLenStr = "\r\nContent-Length: ";
    const char* contentTypeStr = "\r\nContent-Type: ";
    const char* singleCrLfStr = "\r\n";
    const char* doubleCrLfStr = "\r\n\r\n";
    word32 blankStrLen, http11StrLen, hostStrLen, contentLenStrLen,
        contentTypeStrLen, singleCrLfStrLen, doubleCrLfStrLen;

    reqTypeLen = (word32)XSTRLEN(reqType);
    domainNameLen = (word32)XSTRLEN(domainName);
    reqSzStrLen = wolfIO_Word16ToString(reqSzStr, (word16)reqSz);
    contentTypeLen = (word32)XSTRLEN(contentType);

    blankStrLen = (word32)XSTRLEN(blankStr);
    http11StrLen = (word32)XSTRLEN(http11Str);
    hostStrLen = (word32)XSTRLEN(hostStr);
    contentLenStrLen = (word32)XSTRLEN(contentLenStr);
    contentTypeStrLen = (word32)XSTRLEN(contentTypeStr);

    if(exHdrs){
        singleCrLfStrLen = (word32)XSTRLEN(singleCrLfStr);
        exHdrsLen = (word32)XSTRLEN(exHdrs);
    } else {
        singleCrLfStrLen = 0;
        exHdrsLen = 0;
    }

    doubleCrLfStrLen = (word32)XSTRLEN(doubleCrLfStr);

    /* determine max length and check it */
    maxLen =
        reqTypeLen +
        blankStrLen +
        pathLen +
        http11StrLen +
        hostStrLen +
        domainNameLen +
        contentLenStrLen +
        reqSzStrLen +
        contentTypeStrLen +
        contentTypeLen +
        singleCrLfStrLen +
        exHdrsLen +
        doubleCrLfStrLen +
        1 /* null term */;
    if (maxLen > (word32)bufSize)
        return 0;

    XSTRNCPY((char*)buf, reqType, bufSize);
    buf += reqTypeLen; bufSize -= reqTypeLen;
    XSTRNCPY((char*)buf, blankStr, bufSize);
    buf += blankStrLen; bufSize -= blankStrLen;
    XSTRNCPY((char*)buf, path, bufSize);
    buf += pathLen; bufSize -= pathLen;
    XSTRNCPY((char*)buf, http11Str, bufSize);
    buf += http11StrLen; bufSize -= http11StrLen;
    if (domainNameLen > 0) {
        XSTRNCPY((char*)buf, hostStr, bufSize);
        buf += hostStrLen; bufSize -= hostStrLen;
        XSTRNCPY((char*)buf, domainName, bufSize);
        buf += domainNameLen; bufSize -= domainNameLen;
    }
    if (reqSz > 0 && reqSzStrLen > 0) {
        XSTRNCPY((char*)buf, contentLenStr, bufSize);
        buf += contentLenStrLen; bufSize -= contentLenStrLen;
        XSTRNCPY((char*)buf, reqSzStr, bufSize);
        buf += reqSzStrLen; bufSize -= reqSzStrLen;
    }
    if (contentTypeLen > 0) {
        XSTRNCPY((char*)buf, contentTypeStr, bufSize);
        buf += contentTypeStrLen; bufSize -= contentTypeStrLen;
        XSTRNCPY((char*)buf, contentType, bufSize);
        buf += contentTypeLen; bufSize -= contentTypeLen;
    }
    if (exHdrsLen > 0)
    {
        XSTRNCPY((char *)buf, singleCrLfStr, bufSize);
        buf += singleCrLfStrLen;
        bufSize -= singleCrLfStrLen;
        XSTRNCPY((char *)buf, exHdrs, bufSize);
        buf += exHdrsLen;
        bufSize -= exHdrsLen;
    }
    XSTRNCPY((char*)buf, doubleCrLfStr, bufSize);
    buf += doubleCrLfStrLen;

#ifdef WOLFIO_DEBUG
    printf("HTTP %s: %s", reqType, req);
#endif

    /* calculate actual length based on original and new pointer */
    return (int)((char*)buf - req);
}


#ifdef HAVE_OCSP

int wolfIO_HttpBuildRequestOcsp(const char* domainName, const char* path,
                                    int ocspReqSz, byte* buf, int bufSize)
{
    const char *cacheCtl = "Cache-Control: no-cache";
    return wolfIO_HttpBuildRequest_ex("POST", domainName, path, (int)XSTRLEN(path),
        ocspReqSz, "application/ocsp-request", cacheCtl, buf, bufSize);
}

/* return: >0 OCSP Response Size
 *         -1 error */
int wolfIO_HttpProcessResponseOcsp(int sfd, byte** respBuf,
                                       byte* httpBuf, int httpBufSz, void* heap)
{
    const char* appStrList[] = {
        "application/ocsp-response",
        NULL
    };

    return wolfIO_HttpProcessResponse(sfd, appStrList,
        respBuf, httpBuf, httpBufSz, DYNAMIC_TYPE_OCSP, heap);
}

/* in default wolfSSL callback ctx is the heap pointer */
int EmbedOcspLookup(void* ctx, const char* url, int urlSz,
                        byte* ocspReqBuf, int ocspReqSz, byte** ocspRespBuf)
{
    SOCKET_T sfd = SOCKET_INVALID;
    word16   port;
    int      ret = -1;
#ifdef WOLFSSL_SMALL_STACK
    char*    path;
    char*    domainName;
#else
    char     path[MAX_URL_ITEM_SIZE];
    char     domainName[MAX_URL_ITEM_SIZE];
#endif

#ifdef WOLFSSL_SMALL_STACK
    path = (char*)XMALLOC(MAX_URL_ITEM_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (path == NULL)
        return MEMORY_E;

    domainName = (char*)XMALLOC(MAX_URL_ITEM_SIZE, NULL,
            DYNAMIC_TYPE_TMP_BUFFER);
    if (domainName == NULL) {
        XFREE(path, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    if (ocspReqBuf == NULL || ocspReqSz == 0) {
        WOLFSSL_MSG("OCSP request is required for lookup");
    }
    else if (ocspRespBuf == NULL) {
        WOLFSSL_MSG("Cannot save OCSP response");
    }
    else if (wolfIO_DecodeUrl(url, urlSz, domainName, path, &port) < 0) {
        WOLFSSL_MSG("Unable to decode OCSP URL");
    }
    else {
        /* Note, the library uses the EmbedOcspRespFree() callback to
         * free this buffer. */
        int   httpBufSz = HTTP_SCRATCH_BUFFER_SIZE;
        byte* httpBuf   = (byte*)XMALLOC(httpBufSz, ctx, DYNAMIC_TYPE_OCSP);

        if (httpBuf == NULL) {
            WOLFSSL_MSG("Unable to create OCSP response buffer");
        }
        else {
            httpBufSz = wolfIO_HttpBuildRequestOcsp(domainName, path, ocspReqSz,
                                                            httpBuf, httpBufSz);

            ret = wolfIO_TcpConnect(&sfd, domainName, port, io_timeout_sec);
            if (ret != 0) {
                WOLFSSL_MSG("OCSP Responder connection failed");
            }
            else if (wolfIO_Send(sfd, (char*)httpBuf, httpBufSz, 0) !=
                                                                    httpBufSz) {
                WOLFSSL_MSG("OCSP http request failed");
            }
            else if (wolfIO_Send(sfd, (char*)ocspReqBuf, ocspReqSz, 0) !=
                                                                    ocspReqSz) {
                WOLFSSL_MSG("OCSP ocsp request failed");
            }
            else {
                ret = wolfIO_HttpProcessResponseOcsp((int)sfd, ocspRespBuf, httpBuf,
                                                 HTTP_SCRATCH_BUFFER_SIZE, ctx);
            }
            if (sfd != SOCKET_INVALID)
                CloseSocket(sfd);
            XFREE(httpBuf, ctx, DYNAMIC_TYPE_OCSP);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(path,       NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(domainName, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

/* in default callback ctx is heap hint */
void EmbedOcspRespFree(void* ctx, byte *resp)
{
    if (resp)
        XFREE(resp, ctx, DYNAMIC_TYPE_OCSP);

    (void)ctx;
}
#endif /* HAVE_OCSP */


#if defined(HAVE_CRL) && defined(HAVE_CRL_IO)

int wolfIO_HttpBuildRequestCrl(const char* url, int urlSz,
    const char* domainName, byte* buf, int bufSize)
{
    const char *cacheCtl = "Cache-Control: no-cache";
    return wolfIO_HttpBuildRequest_ex("GET", domainName, url, urlSz, 0, "",
                                   cacheCtl, buf, bufSize);
}

int wolfIO_HttpProcessResponseCrl(WOLFSSL_CRL* crl, int sfd, byte* httpBuf,
    int httpBufSz)
{
    int ret;
    byte *respBuf = NULL;

    const char* appStrList[] = {
        "application/pkix-crl",
        "application/x-pkcs7-crl",
        NULL
    };


    ret = wolfIO_HttpProcessResponse(sfd, appStrList,
        &respBuf, httpBuf, httpBufSz, DYNAMIC_TYPE_CRL, crl->heap);
    if (ret >= 0) {
        ret = BufferLoadCRL(crl, respBuf, ret, WOLFSSL_FILETYPE_ASN1, 0);
    }
    XFREE(respBuf, crl->heap, DYNAMIC_TYPE_CRL);

    return ret;
}

int EmbedCrlLookup(WOLFSSL_CRL* crl, const char* url, int urlSz)
{
    SOCKET_T sfd = SOCKET_INVALID;
    word16   port;
    int      ret = -1;
#ifdef WOLFSSL_SMALL_STACK
    char*    domainName;
#else
    char     domainName[MAX_URL_ITEM_SIZE];
#endif

#ifdef WOLFSSL_SMALL_STACK
    domainName = (char*)XMALLOC(MAX_URL_ITEM_SIZE, crl->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (domainName == NULL) {
        return MEMORY_E;
    }
#endif

    if (wolfIO_DecodeUrl(url, urlSz, domainName, NULL, &port) < 0) {
        WOLFSSL_MSG("Unable to decode CRL URL");
    }
    else {
        int   httpBufSz = HTTP_SCRATCH_BUFFER_SIZE;
        byte* httpBuf   = (byte*)XMALLOC(httpBufSz, crl->heap,
                                                              DYNAMIC_TYPE_CRL);
        if (httpBuf == NULL) {
            WOLFSSL_MSG("Unable to create CRL response buffer");
        }
        else {
            httpBufSz = wolfIO_HttpBuildRequestCrl(url, urlSz, domainName,
                httpBuf, httpBufSz);

            ret = wolfIO_TcpConnect(&sfd, domainName, port, io_timeout_sec);
            if (ret != 0) {
                WOLFSSL_MSG("CRL connection failed");
            }
            else if (wolfIO_Send(sfd, (char*)httpBuf, httpBufSz, 0)
                                                                 != httpBufSz) {
                WOLFSSL_MSG("CRL http get failed");
            }
            else {
                ret = wolfIO_HttpProcessResponseCrl(crl, sfd, httpBuf,
                                                      HTTP_SCRATCH_BUFFER_SIZE);
            }
            if (sfd != SOCKET_INVALID)
                CloseSocket(sfd);
            XFREE(httpBuf, crl->heap, DYNAMIC_TYPE_CRL);
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(domainName, crl->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}
#endif /* HAVE_CRL && HAVE_CRL_IO */

#endif /* HAVE_HTTP_CLIENT */



void wolfSSL_CTX_SetIORecv(WOLFSSL_CTX *ctx, CallbackIORecv CBIORecv)
{
    if (ctx) {
        ctx->CBIORecv = CBIORecv;
    #ifdef OPENSSL_EXTRA
        ctx->cbioFlag |= WOLFSSL_CBIO_RECV;
    #endif
    }
}


void wolfSSL_CTX_SetIOSend(WOLFSSL_CTX *ctx, CallbackIOSend CBIOSend)
{
    if (ctx) {
        ctx->CBIOSend = CBIOSend;
    #ifdef OPENSSL_EXTRA
        ctx->cbioFlag |= WOLFSSL_CBIO_SEND;
    #endif
    }
}


/* sets the IO callback to use for receives at WOLFSSL level */
void wolfSSL_SSLSetIORecv(WOLFSSL *ssl, CallbackIORecv CBIORecv)
{
    if (ssl) {
        ssl->CBIORecv = CBIORecv;
    #ifdef OPENSSL_EXTRA
        ssl->cbioFlag |= WOLFSSL_CBIO_RECV;
    #endif
    }
}


/* sets the IO callback to use for sends at WOLFSSL level */
void wolfSSL_SSLSetIOSend(WOLFSSL *ssl, CallbackIOSend CBIOSend)
{
    if (ssl) {
        ssl->CBIOSend = CBIOSend;
    #ifdef OPENSSL_EXTRA
        ssl->cbioFlag |= WOLFSSL_CBIO_SEND;
    #endif
    }
}


void wolfSSL_SetIOReadCtx(WOLFSSL* ssl, void *rctx)
{
    if (ssl)
        ssl->IOCB_ReadCtx = rctx;
}


void wolfSSL_SetIOWriteCtx(WOLFSSL* ssl, void *wctx)
{
    if (ssl)
        ssl->IOCB_WriteCtx = wctx;
}


void* wolfSSL_GetIOReadCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->IOCB_ReadCtx;

    return NULL;
}


void* wolfSSL_GetIOWriteCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->IOCB_WriteCtx;

    return NULL;
}


void wolfSSL_SetIOReadFlags(WOLFSSL* ssl, int flags)
{
    if (ssl)
        ssl->rflags = flags;
}


void wolfSSL_SetIOWriteFlags(WOLFSSL* ssl, int flags)
{
    if (ssl)
        ssl->wflags = flags;
}


#ifdef WOLFSSL_DTLS

void wolfSSL_CTX_SetGenCookie(WOLFSSL_CTX* ctx, CallbackGenCookie cb)
{
    if (ctx)
        ctx->CBIOCookie = cb;
}


void wolfSSL_SetCookieCtx(WOLFSSL* ssl, void *ctx)
{
    if (ssl)
        ssl->IOCB_CookieCtx = ctx;
}


void* wolfSSL_GetCookieCtx(WOLFSSL* ssl)
{
    if (ssl)
        return ssl->IOCB_CookieCtx;

    return NULL;
}
#endif /* WOLFSSL_DTLS */

#ifdef WOLFSSL_SESSION_EXPORT

void wolfSSL_CTX_SetIOGetPeer(WOLFSSL_CTX* ctx, CallbackGetPeer cb)
{
    if (ctx)
        ctx->CBGetPeer = cb;
}


void wolfSSL_CTX_SetIOSetPeer(WOLFSSL_CTX* ctx, CallbackSetPeer cb)
{
    if (ctx)
        ctx->CBSetPeer = cb;
}

#endif /* WOLFSSL_SESSION_EXPORT */


#ifdef HAVE_NETX

/* The NetX receive callback
 *  return :  bytes read, or error
 */
int NetX_Receive(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    NetX_Ctx* nxCtx = (NetX_Ctx*)ctx;
    ULONG left;
    ULONG total;
    ULONG copied = 0;
    UINT  status;

    (void)ssl;

    if (nxCtx == NULL || nxCtx->nxSocket == NULL) {
        WOLFSSL_MSG("NetX Recv NULL parameters");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    if (nxCtx->nxPacket == NULL) {
        status = nx_tcp_socket_receive(nxCtx->nxSocket, &nxCtx->nxPacket,
                                       nxCtx->nxWait);
        if (status != NX_SUCCESS) {
            WOLFSSL_MSG("NetX Recv receive error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    if (nxCtx->nxPacket) {
        status = nx_packet_length_get(nxCtx->nxPacket, &total);
        if (status != NX_SUCCESS) {
            WOLFSSL_MSG("NetX Recv length get error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }

        left = total - nxCtx->nxOffset;
        status = nx_packet_data_extract_offset(nxCtx->nxPacket, nxCtx->nxOffset,
                                               buf, sz, &copied);
        if (status != NX_SUCCESS) {
            WOLFSSL_MSG("NetX Recv data extract offset error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }

        nxCtx->nxOffset += copied;

        if (copied == left) {
            WOLFSSL_MSG("NetX Recv Drained packet");
            nx_packet_release(nxCtx->nxPacket);
            nxCtx->nxPacket = NULL;
            nxCtx->nxOffset = 0;
        }
    }

    return copied;
}


/* The NetX send callback
 *  return : bytes sent, or error
 */
int NetX_Send(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    NetX_Ctx*       nxCtx = (NetX_Ctx*)ctx;
    NX_PACKET*      packet;
    NX_PACKET_POOL* pool;   /* shorthand */
    UINT            status;

    (void)ssl;

    if (nxCtx == NULL || nxCtx->nxSocket == NULL) {
        WOLFSSL_MSG("NetX Send NULL parameters");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    pool = nxCtx->nxSocket->nx_tcp_socket_ip_ptr->nx_ip_default_packet_pool;
    status = nx_packet_allocate(pool, &packet, NX_TCP_PACKET,
                                nxCtx->nxWait);
    if (status != NX_SUCCESS) {
        WOLFSSL_MSG("NetX Send packet alloc error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    status = nx_packet_data_append(packet, buf, sz, pool, nxCtx->nxWait);
    if (status != NX_SUCCESS) {
        nx_packet_release(packet);
        WOLFSSL_MSG("NetX Send data append error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    status = nx_tcp_socket_send(nxCtx->nxSocket, packet, nxCtx->nxWait);
    if (status != NX_SUCCESS) {
        nx_packet_release(packet);
        WOLFSSL_MSG("NetX Send socket send error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    return sz;
}


/* like set_fd, but for default NetX context */
void wolfSSL_SetIO_NetX(WOLFSSL* ssl, NX_TCP_SOCKET* nxSocket, ULONG waitOption)
{
    if (ssl) {
        ssl->nxCtx.nxSocket = nxSocket;
        ssl->nxCtx.nxWait   = waitOption;
    }
}

#endif /* HAVE_NETX */


#ifdef MICRIUM

/* Micrium uTCP/IP port, using the NetSock API
 * TCP and UDP are currently supported with the callbacks below.
 *
 * WOLFSSL_SESSION_EXPORT is not yet supported, would need EmbedGetPeer()
 * and EmbedSetPeer() callbacks implemented.
 *
 * HAVE_CRL is not yet supported, would need an EmbedCrlLookup()
 * callback implemented.
 *
 * HAVE_OCSP is not yet supported, would need an EmbedOCSPLookup()
 * callback implemented.
 */

/* The Micrium uTCP/IP send callback
 * return : bytes sent, or error
 */
int MicriumSend(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    NET_SOCK_ID sd = *(int*)ctx;
    NET_SOCK_RTN_CODE ret;
    NET_ERR err;

    ret = NetSock_TxData(sd, buf, sz, ssl->wflags, &err);
    if (ret < 0) {
        WOLFSSL_MSG("Embed Send error");

        if (err == NET_ERR_TX) {
            WOLFSSL_MSG("\tWould block");
            return WOLFSSL_CBIO_ERR_WANT_WRITE;

        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return ret;
}

/* The Micrium uTCP/IP receive callback
 *  return : nb bytes read, or error
 */
int MicriumReceive(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    NET_SOCK_ID sd = *(int*)ctx;
    NET_SOCK_RTN_CODE ret;
    NET_ERR err;

    #ifdef WOLFSSL_DTLS
    {
        int dtls_timeout = wolfSSL_dtls_get_current_timeout(ssl);
        /* Don't use ssl->options.handShakeDone since it is true even if
         * we are in the process of renegotiation */
        byte doDtlsTimeout = ssl->options.handShakeState != HANDSHAKE_DONE;
        #ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls && IsAtLeastTLSv1_3(ssl->version)) {
            doDtlsTimeout =
                doDtlsTimeout || ssl->dtls13Rtx.rtxRecords != NULL ||
                (ssl->dtls13FastTimeout && ssl->dtls13Rtx.seenRecords != NULL);
        }
        #endif /* WOLFSSL_DTLS13 */

        if (!doDtlsTimeout)
            dtls_timeout = 0;

        if (!wolfSSL_dtls_get_using_nonblock(ssl)) {
            /* needs timeout in milliseconds */
            #ifdef WOLFSSL_DTLS13
            if (wolfSSL_dtls13_use_quick_timeout(ssl) &&
                IsAtLeastTLSv1_3(ssl->version)) {
                dtls_timeout = (1000 * dtls_timeout) / 4;
            } else
            #endif /* WOLFSSL_DTLS13 */
                dtls_timeout = 1000 * dtls_timeout;
            NetSock_CfgTimeoutRxQ_Set(sd, dtls_timeout, &err);
            if (err != NET_SOCK_ERR_NONE) {
                WOLFSSL_MSG("NetSock_CfgTimeoutRxQ_Set failed");
            }
        }
    }
    #endif

    ret = NetSock_RxData(sd, buf, sz, ssl->rflags, &err);
    if (ret < 0) {
        WOLFSSL_MSG("Embed Receive error");

        if (err == NET_ERR_RX || err == NET_SOCK_ERR_RX_Q_EMPTY ||
            err == NET_ERR_FAULT_LOCK_ACQUIRE) {
            if (!wolfSSL_dtls(ssl) || wolfSSL_dtls_get_using_nonblock(ssl)) {
                WOLFSSL_MSG("\tWould block");
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
            else {
                WOLFSSL_MSG("\tSocket timeout");
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }

        } else if (err == NET_SOCK_ERR_CLOSED) {
            WOLFSSL_MSG("Embed receive connection closed");
            return WOLFSSL_CBIO_ERR_CONN_CLOSE;

        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return ret;
}

/* The Micrium uTCP/IP receivefrom callback
 *  return : nb bytes read, or error
 */
int MicriumReceiveFrom(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    NET_SOCK_ID       sd = dtlsCtx->rfd;
    NET_SOCK_ADDR     peer;
    NET_SOCK_ADDR_LEN peerSz = sizeof(peer);
    NET_SOCK_RTN_CODE ret;
    NET_ERR err;

    WOLFSSL_ENTER("MicriumReceiveFrom");

#ifdef WOLFSSL_DTLS
    {
        int dtls_timeout = wolfSSL_dtls_get_current_timeout(ssl);
        /* Don't use ssl->options.handShakeDone since it is true even if
         * we are in the process of renegotiation */
        byte doDtlsTimeout = ssl->options.handShakeState != HANDSHAKE_DONE;

        #ifdef WOLFSSL_DTLS13
        if (ssl->options.dtls && IsAtLeastTLSv1_3(ssl->version)) {
            doDtlsTimeout =
                doDtlsTimeout || ssl->dtls13Rtx.rtxRecords != NULL ||
                (ssl->dtls13FastTimeout && ssl->dtls13Rtx.seenRecords != NULL);
        }
        #endif /* WOLFSSL_DTLS13 */

        if (!doDtlsTimeout)
            dtls_timeout = 0;

        if (!wolfSSL_dtls_get_using_nonblock(ssl)) {
            /* needs timeout in milliseconds */
            #ifdef WOLFSSL_DTLS13
            if (wolfSSL_dtls13_use_quick_timeout(ssl) &&
                IsAtLeastTLSv1_3(ssl->version)) {
                dtls_timeout = (1000 * dtls_timeout) / 4;
            } else
            #endif /* WOLFSSL_DTLS13 */
                dtls_timeout = 1000 * dtls_timeout;
            NetSock_CfgTimeoutRxQ_Set(sd, dtls_timeout, &err);
            if (err != NET_SOCK_ERR_NONE) {
                WOLFSSL_MSG("NetSock_CfgTimeoutRxQ_Set failed");
            }
        }
    }
#endif /* WOLFSSL_DTLS */

    ret = NetSock_RxDataFrom(sd, buf, sz, ssl->rflags, &peer, &peerSz,
                             0, 0, 0, &err);
    if (ret < 0) {
        WOLFSSL_MSG("Embed Receive From error");

        if (err == NET_ERR_RX || err == NET_SOCK_ERR_RX_Q_EMPTY ||
            err == NET_ERR_FAULT_LOCK_ACQUIRE) {
            if (wolfSSL_dtls_get_using_nonblock(ssl)) {
                WOLFSSL_MSG("\tWould block");
                return WOLFSSL_CBIO_ERR_WANT_READ;
            }
            else {
                WOLFSSL_MSG("\tSocket timeout");
                return WOLFSSL_CBIO_ERR_TIMEOUT;
            }
        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
    else {
        if (dtlsCtx->peer.sz > 0
                && peerSz != (NET_SOCK_ADDR_LEN)dtlsCtx->peer.sz
                && XMEMCMP(&peer, dtlsCtx->peer.sa, peerSz) != 0) {
            WOLFSSL_MSG("\tIgnored packet from invalid peer");
            return WOLFSSL_CBIO_ERR_WANT_READ;
        }
    }

    return ret;
}

/* The Micrium uTCP/IP sendto callback
 *  return : nb bytes sent, or error
 */
int MicriumSendTo(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    WOLFSSL_DTLS_CTX* dtlsCtx = (WOLFSSL_DTLS_CTX*)ctx;
    NET_SOCK_ID sd = dtlsCtx->wfd;
    NET_SOCK_RTN_CODE ret;
    NET_ERR err;

    WOLFSSL_ENTER("MicriumSendTo");

    ret = NetSock_TxDataTo(sd, buf, sz, ssl->wflags,
                           (NET_SOCK_ADDR*)dtlsCtx->peer.sa,
                           (NET_SOCK_ADDR_LEN)dtlsCtx->peer.sz,
                           &err);
    if (err < 0) {
        WOLFSSL_MSG("Embed Send To error");

        if (err == NET_ERR_TX) {
            WOLFSSL_MSG("\tWould block");
            return WOLFSSL_CBIO_ERR_WANT_WRITE;

        } else {
            WOLFSSL_MSG("\tGeneral error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }

    return ret;
}

/* Micrium DTLS Generate Cookie callback
 *  return : number of bytes copied into buf, or error
 */
int MicriumGenerateCookie(WOLFSSL* ssl, byte *buf, int sz, void *ctx)
{
    NET_SOCK_ADDR peer;
    NET_SOCK_ADDR_LEN peerSz = sizeof(peer);
    byte digest[WC_SHA_DIGEST_SIZE];
    int  ret = 0;

    (void)ctx;

    XMEMSET(&peer, 0, sizeof(peer));
    if (wolfSSL_dtls_get_peer(ssl, (void*)&peer,
                              (unsigned int*)&peerSz) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("getpeername failed in MicriumGenerateCookie");
        return GEN_COOKIE_E;
    }

    ret = wc_ShaHash((byte*)&peer, peerSz, digest);
    if (ret != 0)
        return ret;

    if (sz > WC_SHA_DIGEST_SIZE)
        sz = WC_SHA_DIGEST_SIZE;
    XMEMCPY(buf, digest, sz);

    return sz;
}

#endif /* MICRIUM */

#if defined(WOLFSSL_APACHE_MYNEWT) && !defined(WOLFSSL_LWIP)

#include <os/os_error.h>
#include <os/os_mbuf.h>
#include <os/os_mempool.h>

#define MB_NAME "wolfssl_mb"

typedef struct Mynewt_Ctx {
        struct mn_socket *mnSocket;          /* send/recv socket handler */
        struct mn_sockaddr_in mnSockAddrIn;  /* socket address */
        struct os_mbuf *mnPacket;            /* incoming packet handle
                                                for short reads */
        int reading;                         /* reading flag */

        /* private */
        void *mnMemBuffer;                   /* memory buffer for mempool */
        struct os_mempool mnMempool;         /* mempool */
        struct os_mbuf_pool mnMbufpool;      /* mbuf pool */
} Mynewt_Ctx;

void mynewt_ctx_clear(void *ctx) {
    Mynewt_Ctx *mynewt_ctx = (Mynewt_Ctx*)ctx;
    if(!mynewt_ctx) return;

    if(mynewt_ctx->mnPacket) {
        os_mbuf_free_chain(mynewt_ctx->mnPacket);
        mynewt_ctx->mnPacket = NULL;
    }
    os_mempool_clear(&mynewt_ctx->mnMempool);
    XFREE(mynewt_ctx->mnMemBuffer, 0, 0);
    XFREE(mynewt_ctx, 0, 0);
}

/* return Mynewt_Ctx instance */
void* mynewt_ctx_new() {
    int rc = 0;
    Mynewt_Ctx *mynewt_ctx;
    int mem_buf_count = MYNEWT_VAL(WOLFSSL_MNSOCK_MEM_BUF_COUNT);
    int mem_buf_size = MYNEWT_VAL(WOLFSSL_MNSOCK_MEM_BUF_SIZE);
    int mempool_bytes = OS_MEMPOOL_BYTES(mem_buf_count, mem_buf_size);

    mynewt_ctx = (Mynewt_Ctx *)XMALLOC(sizeof(struct Mynewt_Ctx),
                                       NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if(!mynewt_ctx) return NULL;

    XMEMSET(mynewt_ctx, 0, sizeof(Mynewt_Ctx));
    mynewt_ctx->mnMemBuffer = (void *)XMALLOC(mempool_bytes, 0, 0);
    if(!mynewt_ctx->mnMemBuffer) {
        mynewt_ctx_clear((void*)mynewt_ctx);
        return NULL;
    }

    rc = os_mempool_init(&mynewt_ctx->mnMempool,
                         mem_buf_count, mem_buf_size,
                         mynewt_ctx->mnMemBuffer, MB_NAME);
    if(rc != 0) {
        mynewt_ctx_clear((void*)mynewt_ctx);
        return NULL;
    }
    rc = os_mbuf_pool_init(&mynewt_ctx->mnMbufpool, &mynewt_ctx->mnMempool,
                           mem_buf_count, mem_buf_size);
    if(rc != 0) {
        mynewt_ctx_clear((void*)mynewt_ctx);
        return NULL;
    }

    return mynewt_ctx;
}

static void mynewt_sock_writable(void *arg, int err);
static void mynewt_sock_readable(void *arg, int err);
static const union mn_socket_cb mynewt_sock_cbs = {
    .socket.writable = mynewt_sock_writable,
    .socket.readable = mynewt_sock_readable,
};
static void mynewt_sock_writable(void *arg, int err)
{
    /* do nothing */
}
static void mynewt_sock_readable(void *arg, int err)
{
    Mynewt_Ctx *mynewt_ctx = (Mynewt_Ctx *)arg;
    if (err && mynewt_ctx->reading) {
        mynewt_ctx->reading = 0;
    }
}

/* The Mynewt receive callback
 *  return :  bytes read, or error
 */
int Mynewt_Receive(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    Mynewt_Ctx *mynewt_ctx = (Mynewt_Ctx*)ctx;
    int rc = 0;
    struct mn_sockaddr_in from;
    struct os_mbuf *m;
    int read_sz = 0;
    word16 total;

    if (mynewt_ctx == NULL || mynewt_ctx->mnSocket == NULL) {
        WOLFSSL_MSG("Mynewt Recv NULL parameters");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    if(mynewt_ctx->mnPacket == NULL) {
        mynewt_ctx->mnPacket = os_mbuf_get_pkthdr(&mynewt_ctx->mnMbufpool, 0);
        if(mynewt_ctx->mnPacket == NULL) {
            return MEMORY_E;
        }

        mynewt_ctx->reading = 1;
        while(mynewt_ctx->reading && rc == 0) {
            rc = mn_recvfrom(mynewt_ctx->mnSocket, &m, (struct mn_sockaddr *) &from);
            if(rc == MN_ECONNABORTED) {
                rc = 0;
                mynewt_ctx->reading = 0;
                break;
            }
            if (!(rc == 0 || rc == MN_EAGAIN)) {
                WOLFSSL_MSG("Mynewt Recv receive error");
                mynewt_ctx->reading = 0;
                break;
            }
            if(rc == 0) {
                int len = OS_MBUF_PKTLEN(m);
                if(len == 0) {
                    break;
                }
                rc = os_mbuf_appendfrom(mynewt_ctx->mnPacket, m, 0, len);
                if(rc != 0) {
                    WOLFSSL_MSG("Mynewt Recv os_mbuf_appendfrom error");
                    break;
                }
                os_mbuf_free_chain(m);
                m = NULL;
            } else if(rc == MN_EAGAIN) {
                /* continue to until reading all of packet data. */
                rc = 0;
                break;
            }
        }
        if(rc != 0) {
            mynewt_ctx->reading = 0;
            os_mbuf_free_chain(mynewt_ctx->mnPacket);
            mynewt_ctx->mnPacket = NULL;
            return rc;
        }
    }

    if(mynewt_ctx->mnPacket) {
        total = OS_MBUF_PKTLEN(mynewt_ctx->mnPacket);
        read_sz = (total >= sz)? sz : total;

        os_mbuf_copydata(mynewt_ctx->mnPacket, 0, read_sz, (void*)buf);
        os_mbuf_adj(mynewt_ctx->mnPacket, read_sz);

        if (read_sz == total) {
            WOLFSSL_MSG("Mynewt Recv Drained packet");
            os_mbuf_free_chain(mynewt_ctx->mnPacket);
            mynewt_ctx->mnPacket = NULL;
        }
    }

    return read_sz;
}

/* The Mynewt send callback
 *  return : bytes sent, or error
 */
int Mynewt_Send(WOLFSSL* ssl, char *buf, int sz, void *ctx)
{
    Mynewt_Ctx *mynewt_ctx = (Mynewt_Ctx*)ctx;
    int rc = 0;
    struct os_mbuf *m;
    int write_sz = 0;
    m = os_msys_get_pkthdr(sz, 0);
    if (!m) {
        WOLFSSL_MSG("Mynewt Send os_msys_get_pkthdr error");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    rc = os_mbuf_copyinto(m, 0, buf, sz);
    if (rc != 0) {
        WOLFSSL_MSG("Mynewt Send os_mbuf_copyinto error");
        os_mbuf_free_chain(m);
        return rc;
    }
    rc = mn_sendto(mynewt_ctx->mnSocket, m, (struct mn_sockaddr *)&mynewt_ctx->mnSockAddrIn);
    if(rc != 0) {
        WOLFSSL_MSG("Mynewt Send mn_sendto error");
        os_mbuf_free_chain(m);
        return rc;
    }
    write_sz = sz;
    return write_sz;
}

/* like set_fd, but for default NetX context */
void wolfSSL_SetIO_Mynewt(WOLFSSL* ssl, struct mn_socket* mnSocket, struct mn_sockaddr_in* mnSockAddrIn)
{
    if (ssl && ssl->mnCtx) {
        Mynewt_Ctx *mynewt_ctx = (Mynewt_Ctx *)ssl->mnCtx;
        mynewt_ctx->mnSocket = mnSocket;
        XMEMCPY(&mynewt_ctx->mnSockAddrIn, mnSockAddrIn, sizeof(struct mn_sockaddr_in));
        mn_socket_set_cbs(mynewt_ctx->mnSocket, mnSocket, &mynewt_sock_cbs);
    }
}

#endif /* defined(WOLFSSL_APACHE_MYNEWT) && !defined(WOLFSSL_LWIP) */

#ifdef WOLFSSL_UIP
#include <uip.h>
#include <stdio.h>

/* uIP TCP/IP port, using the native tcp/udp socket api.
 * TCP and UDP are currently supported with the callbacks below.
 *
 */
/* The uIP tcp send callback
 * return : bytes sent, or error
 */
int uIPSend(WOLFSSL* ssl, char* buf, int sz, void* _ctx)
{
    uip_wolfssl_ctx *ctx = (struct uip_wolfssl_ctx *)_ctx;
    int total_written = 0;
    (void)ssl;
    do {
        int ret;
        unsigned int bytes_left = sz - total_written;
        unsigned int max_sendlen = tcp_socket_max_sendlen(&ctx->conn.tcp);
        if (bytes_left > max_sendlen) {
            fprintf(stderr, "uIPSend: Send limited by buffer\r\n");
            bytes_left = max_sendlen;
        }
        if (bytes_left == 0) {
            fprintf(stderr, "uIPSend: Buffer full!\r\n");
            break;
        }
        ret = tcp_socket_send(&ctx->conn.tcp, (unsigned char *)buf + total_written, bytes_left);
        if (ret <= 0)
            break;
        total_written += ret;
    } while(total_written < sz);
    if (total_written == 0)
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
    return total_written;
}

int uIPSendTo(WOLFSSL* ssl, char* buf, int sz, void* _ctx)
{
    uip_wolfssl_ctx *ctx = (struct uip_wolfssl_ctx *)_ctx;
    int ret = 0;
    (void)ssl;
    ret = udp_socket_sendto(&ctx->conn.udp, (unsigned char *)buf, sz, &ctx->peer_addr, ctx->peer_port );
    if (ret == 0)
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
    return ret;
}

/* The uIP uTCP/IP receive callback
 *  return : nb bytes read, or error
 */
int uIPReceive(WOLFSSL *ssl, char *buf, int sz, void *_ctx)
{
    uip_wolfssl_ctx *ctx = (uip_wolfssl_ctx *)_ctx;
    if (!ctx || !ctx->ssl_rx_databuf)
        return -1;
    (void)ssl;
    if (ctx->ssl_rb_len > 0) {
        if (sz > ctx->ssl_rb_len - ctx->ssl_rb_off)
            sz = ctx->ssl_rb_len - ctx->ssl_rb_off;
        XMEMCPY(buf, ctx->ssl_rx_databuf + ctx->ssl_rb_off, sz);
        ctx->ssl_rb_off += sz;
        if (ctx->ssl_rb_off >= ctx->ssl_rb_len) {
            ctx->ssl_rb_len = 0;
            ctx->ssl_rb_off = 0;
        }
        return sz;
    } else {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
}

/* uIP DTLS Generate Cookie callback
 *  return : number of bytes copied into buf, or error
 */
int uIPGenerateCookie(WOLFSSL* ssl, byte *buf, int sz, void *_ctx)
{
    uip_wolfssl_ctx *ctx = (uip_wolfssl_ctx *)_ctx;
    byte token[32];
    byte digest[WC_SHA_DIGEST_SIZE];
    int  ret = 0;
    XMEMSET(token, 0, sizeof(token));
    XMEMCPY(token, &ctx->peer_addr, sizeof(uip_ipaddr_t));
    XMEMCPY(token + sizeof(uip_ipaddr_t), &ctx->peer_port, sizeof(word16));
    ret = wc_ShaHash(token, sizeof(uip_ipaddr_t) + sizeof(word16), digest);
    if (ret != 0)
        return ret;
    if (sz > WC_SHA_DIGEST_SIZE)
        sz = WC_SHA_DIGEST_SIZE;
    XMEMCPY(buf, digest, sz);
    return sz;
}

#endif /* WOLFSSL_UIP */

#ifdef WOLFSSL_GNRC

#include <net/sock.h>
#include <net/sock/tcp.h>
#include <stdio.h>

/* GNRC TCP/IP port, using the native tcp/udp socket api.
 * TCP and UDP are currently supported with the callbacks below.
 *
 */
/* The GNRC tcp send callback
 * return : bytes sent, or error
 */

int GNRC_SendTo(WOLFSSL* ssl, char* buf, int sz, void* _ctx)
{
    sock_tls_t *ctx = (sock_tls_t *)_ctx;
    int ret = 0;
    (void)ssl;
    if (!ctx)
        return WOLFSSL_CBIO_ERR_GENERAL;
    ret = sock_udp_send(&ctx->conn.udp, (unsigned char *)buf, sz, &ctx->peer_addr);
    if (ret == 0)
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
    return ret;
}

/* The GNRC TCP/IP receive callback
 *  return : nb bytes read, or error
 */
int GNRC_ReceiveFrom(WOLFSSL *ssl, char *buf, int sz, void *_ctx)
{
    sock_udp_ep_t ep;
    int ret;
    word32 timeout = wolfSSL_dtls_get_current_timeout(ssl) * 1000000;
    sock_tls_t *ctx = (sock_tls_t *)_ctx;
    if (!ctx)
        return WOLFSSL_CBIO_ERR_GENERAL;
    (void)ssl;
    if (wolfSSL_get_using_nonblock(ctx->ssl)) {
        timeout = 0;
    }
    ret = sock_udp_recv(&ctx->conn.udp, buf, sz, timeout, &ep);
    if (ret > 0) {
        if (ctx->peer_addr.port == 0)
            XMEMCPY(&ctx->peer_addr, &ep, sizeof(sock_udp_ep_t));
    }
    if (ret == -ETIMEDOUT) {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
    return ret;
}

/* GNRC DTLS Generate Cookie callback
 *  return : number of bytes copied into buf, or error
 */
#define GNRC_MAX_TOKEN_SIZE (32)
int GNRC_GenerateCookie(WOLFSSL* ssl, byte *buf, int sz, void *_ctx)
{
    sock_tls_t *ctx = (sock_tls_t *)_ctx;
    if (!ctx)
        return WOLFSSL_CBIO_ERR_GENERAL;
    byte token[GNRC_MAX_TOKEN_SIZE];
    byte digest[WC_SHA_DIGEST_SIZE];
    int  ret = 0;
    size_t token_size = sizeof(sock_udp_ep_t);
    (void)ssl;
    if (token_size > GNRC_MAX_TOKEN_SIZE)
        token_size = GNRC_MAX_TOKEN_SIZE;
    XMEMSET(token, 0, GNRC_MAX_TOKEN_SIZE);
    XMEMCPY(token, &ctx->peer_addr, token_size);
    ret = wc_ShaHash(token, token_size, digest);
    if (ret != 0)
        return ret;
    if (sz > WC_SHA_DIGEST_SIZE)
        sz = WC_SHA_DIGEST_SIZE;
    XMEMCPY(buf, digest, sz);
    return sz;
}

#endif /* WOLFSSL_GNRC */

#ifdef WOLFSSL_LWIP_NATIVE
int LwIPNativeSend(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    err_t ret;
    WOLFSSL_LWIP_NATIVE_STATE* nlwip = (WOLFSSL_LWIP_NATIVE_STATE*)ctx;

    ret = tcp_write(nlwip->pcb, buf, sz, TCP_WRITE_FLAG_COPY);
    if (ret != ERR_OK) {
        sz = -1;
    }

    return sz;
}


int LwIPNativeReceive(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    struct pbuf *current, *head;
    WOLFSSL_LWIP_NATIVE_STATE* nlwip;
    int ret = 0;

    if (ctx == NULL) {
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    nlwip = (WOLFSSL_LWIP_NATIVE_STATE*)ctx;

    current = nlwip->pbuf;
    if (current == NULL || sz > current->tot_len) {
        WOLFSSL_MSG("LwIP native pbuf list is null or not enough data, want read");
        ret = WOLFSSL_CBIO_ERR_WANT_READ;
    }
    else {
        int read = 0; /* total amount read */
        head = nlwip->pbuf; /* save pointer to current head */

        /* loop through buffers reading data */
        while (current != NULL) {
            int len; /* current amount to be read */

            len = (current->len - nlwip->pulled < sz) ?
                                            (current->len - nlwip->pulled) : sz;

            if (read + len > sz) {
                /* should never be hit but have sanity check before use */
                return WOLFSSL_CBIO_ERR_GENERAL;
            }

            /* check if is a partial read from before */
            XMEMCPY(&buf[read],
                   (const char *)&(((char *)(current->payload))[nlwip->pulled]),

                    len);
            nlwip->pulled = nlwip->pulled + len;
            if (nlwip->pulled >= current->len) {
                WOLFSSL_MSG("Native LwIP read full pbuf");
                nlwip->pbuf = current->next;
                current = nlwip->pbuf;
                nlwip->pulled = 0;
            }
            read = read + len;
            ret  = read;

            /* read enough break out */
            if (read >= sz) {
                /* if more pbuf's are left in the chain then increment the
                 * ref count for next in chain and free all from beginning till
                 * next */
                if (current != NULL) {
                    pbuf_ref(current);
                }

                /* ack and start free'ing from the current head of the chain */
                pbuf_free(head);
                break;
            }
        }
    }
    WOLFSSL_LEAVE("LwIPNativeReceive", ret);
    return ret;
}


static err_t LwIPNativeReceiveCB(void* cb, struct tcp_pcb* pcb,
                                struct pbuf* pbuf, err_t err)
{
    WOLFSSL_LWIP_NATIVE_STATE* nlwip;

    if (cb == NULL || pcb == NULL) {
        WOLFSSL_MSG("Expected callback was null, abort");
        return ERR_ABRT;
    }

    nlwip = (WOLFSSL_LWIP_NATIVE_STATE*)cb;
    if (pbuf == NULL && err == ERR_OK) {
        return ERR_OK;
    }

    if (nlwip->pbuf == NULL) {
        nlwip->pbuf = pbuf;
    }
    else {
        if (nlwip->pbuf != pbuf) {
            tcp_recved(nlwip->pcb, pbuf->tot_len);
            pbuf_cat(nlwip->pbuf, pbuf); /* add chain to head */
        }
    }

    if (nlwip->recv_fn) {
        return nlwip->recv_fn(nlwip->arg, pcb, pbuf, err);
    }

    WOLFSSL_LEAVE("LwIPNativeReceiveCB", nlwip->pbuf->tot_len);
    return ERR_OK;
}


static err_t LwIPNativeSentCB(void* cb, struct tcp_pcb* pcb, u16_t len)
{
    WOLFSSL_LWIP_NATIVE_STATE* nlwip;

    if (cb == NULL || pcb == NULL) {
        WOLFSSL_MSG("Expected callback was null, abort");
        return ERR_ABRT;
    }

    nlwip = (WOLFSSL_LWIP_NATIVE_STATE*)cb;
    if (nlwip->sent_fn) {
        return nlwip->sent_fn(nlwip->arg, pcb, len);
    }
    return ERR_OK;
}


int wolfSSL_SetIO_LwIP(WOLFSSL* ssl, void* pcb,
                          tcp_recv_fn recv_fn, tcp_sent_fn sent_fn, void *arg)
{
    if (ssl == NULL || pcb == NULL)
        return BAD_FUNC_ARG;

    ssl->lwipCtx.pcb = (struct tcp_pcb *)pcb;
    ssl->lwipCtx.recv_fn = recv_fn; /*  recv user callback */
    ssl->lwipCtx.sent_fn = sent_fn; /*  sent user callback */
    ssl->lwipCtx.arg  = arg;
    ssl->lwipCtx.pbuf = 0;
    ssl->lwipCtx.pulled = 0;
    ssl->lwipCtx.wait   = 0;

    /* wolfSSL_LwIP_recv/sent_cb invokes recv/sent user callback in them. */
    tcp_recv(pcb, LwIPNativeReceiveCB);
    tcp_sent(pcb, LwIPNativeSentCB);
    tcp_arg (pcb, (void *)&ssl->lwipCtx);
    wolfSSL_SetIOReadCtx(ssl, &ssl->lwipCtx);
    wolfSSL_SetIOWriteCtx(ssl, &ssl->lwipCtx);

    return ERR_OK;
}
#endif

#ifdef WOLFSSL_ISOTP
static int isotp_send_single_frame(struct isotp_wolfssl_ctx *ctx, char *buf,
        word16 length)
{
    /* Length will be at most 7 bytes to get here. Packet is length and type
     * for the first byte, then up to 7 bytes of data */
    ctx->frame.data[0] = ((byte)length) | (ISOTP_FRAME_TYPE_SINGLE << 4);
    XMEMCPY(&ctx->frame.data[1], buf, length);
    ctx->frame.length = length + 1;
    return ctx->send_fn(&ctx->frame, ctx->arg);
}

static int isotp_send_flow_control(struct isotp_wolfssl_ctx *ctx,
        byte overflow)
{
    int ret;
    /* Overflow is set it if we have been asked to receive more data than the
     * user allocated a buffer for */
    if (overflow) {
        ctx->frame.data[0] = ISOTP_FLOW_CONTROL_ABORT |
            (ISOTP_FRAME_TYPE_CONTROL << 4);
    } else {
        ctx->frame.data[0] = ISOTP_FLOW_CONTROL_CTS |
            (ISOTP_FRAME_TYPE_CONTROL << 4);
    }
    /* Set the number of frames between flow control to infinite */
    ctx->frame.data[1] = ISOTP_FLOW_CONTROL_FRAMES;
    /* User specified frame delay */
    ctx->frame.data[2] = ctx->receive_delay;
    ctx->frame.length = ISOTP_FLOW_CONTROL_PACKET_SIZE;
    ret = ctx->send_fn(&ctx->frame, ctx->arg);
    return ret;
}

static int isotp_receive_flow_control(struct isotp_wolfssl_ctx *ctx)
{
    int ret;
    enum isotp_frame_type type;
    enum isotp_flow_control flow_control;
    ret = ctx->recv_fn(&ctx->frame, ctx->arg, ISOTP_DEFAULT_TIMEOUT);
    if (ret == 0) {
        return WOLFSSL_CBIO_ERR_TIMEOUT;
    } else if (ret < 0) {
        WOLFSSL_MSG("ISO-TP error receiving flow control packet");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    /* Flow control is the frame type and flow response for the first byte,
     * number of frames until the next flow control packet for the second
     * byte, time between frames for the third byte */
    type = ctx->frame.data[0] >> 4;

    if (type != ISOTP_FRAME_TYPE_CONTROL) {
        WOLFSSL_MSG("ISO-TP frames out of sequence");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }

    flow_control = ctx->frame.data[0] & 0xf;

    ctx->flow_counter = 0;
    ctx->flow_packets = ctx->frame.data[1];
    ctx->frame_delay = ctx->frame.data[2];

    return flow_control;
}

static int isotp_send_consecutive_frame(struct isotp_wolfssl_ctx *ctx)
{
    /* Sequence is 0 - 15 and then starts again, the first frame has an
     * implied sequence of '0' */
    ctx->sequence += 1;
    if (ctx->sequence > ISOTP_MAX_SEQUENCE_COUNTER) {
        ctx->sequence = 0;
    }
    ctx->flow_counter++;
    /* First byte it type and sequence number, up to 7 bytes of data */
    ctx->frame.data[0] = ctx->sequence | (ISOTP_FRAME_TYPE_CONSECUTIVE << 4);
    if (ctx->buf_length > ISOTP_MAX_CONSECUTIVE_FRAME_DATA_SIZE) {
        XMEMCPY(&ctx->frame.data[1], ctx->buf_ptr,
                ISOTP_MAX_CONSECUTIVE_FRAME_DATA_SIZE);
        ctx->buf_ptr += ISOTP_MAX_CONSECUTIVE_FRAME_DATA_SIZE;
        ctx->buf_length -= ISOTP_MAX_CONSECUTIVE_FRAME_DATA_SIZE;
        ctx->frame.length = ISOTP_CAN_BUS_PAYLOAD_SIZE;
    } else {
        XMEMCPY(&ctx->frame.data[1], ctx->buf_ptr, ctx->buf_length);
        ctx->frame.length = ctx->buf_length + 1;
        ctx->buf_length = 0;
    }
    return ctx->send_fn(&ctx->frame, ctx->arg);

}

static int isotp_send_first_frame(struct isotp_wolfssl_ctx *ctx, char *buf,
        word16 length)
{
    int ret;
    ctx->sequence = 0;
    /* Set to 1 to trigger a flow control straight away, the flow control
     * packet will set these properly */
    ctx->flow_packets = ctx->flow_counter = 1;
    /* First frame has 1 nibble for type, 3 nibbles for length followed by
     * 6 bytes for data*/
    ctx->frame.data[0] = (length >> 8) | (ISOTP_FRAME_TYPE_FIRST << 4);
    ctx->frame.data[1] = length & 0xff;
    XMEMCPY(&ctx->frame.data[2], buf, ISOTP_FIRST_FRAME_DATA_SIZE);
    ctx->buf_ptr = buf + ISOTP_FIRST_FRAME_DATA_SIZE;
    ctx->buf_length = length - ISOTP_FIRST_FRAME_DATA_SIZE;
    ctx->frame.length = ISOTP_CAN_BUS_PAYLOAD_SIZE;
    ret = ctx->send_fn(&ctx->frame, ctx->arg);
    if (ret <= 0) {
        WOLFSSL_MSG("ISO-TP error sending first frame");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    while(ctx->buf_length) {
        /* The receiver can set how often to get a flow control packet. If it
         * is time, then get the packet. Note that this will always happen
         * after the first packet */
        if ((ctx->flow_packets > 0) &&
                (ctx->flow_counter == ctx->flow_packets)) {
            ret = isotp_receive_flow_control(ctx);
        }
        /* Frame delay <= 0x7f is in ms, 0xfX is X * 100 us */
        if (ctx->frame_delay) {
            if (ctx->frame_delay <= ISOTP_MAX_MS_FRAME_DELAY) {
                ctx->delay_fn(ctx->frame_delay * 1000);
            } else {
                ctx->delay_fn((ctx->frame_delay & 0xf) * 100);
            }
        }
        switch (ret) {
            /* Clear to send */
            case ISOTP_FLOW_CONTROL_CTS:
                if (isotp_send_consecutive_frame(ctx) < 0) {
                    WOLFSSL_MSG("ISO-TP error sending consecutive frame");
                    return WOLFSSL_CBIO_ERR_GENERAL;
                }
                break;
            /* Receiver says "WAIT", so we wait for another flow control
             * packet, or abort if we have waited too long */
            case ISOTP_FLOW_CONTROL_WAIT:
                ctx->wait_counter += 1;
                if (ctx->wait_counter > ISOTP_DEFAULT_WAIT_COUNT) {
                    WOLFSSL_MSG("ISO-TP receiver told us to wait too many"
                            " times");
                    return WOLFSSL_CBIO_ERR_WANT_WRITE;
                }
                break;
            /* Receiver is not ready to receive packet, so abort */
            case ISOTP_FLOW_CONTROL_ABORT:
                WOLFSSL_MSG("ISO-TP receiver aborted transmission");
                return WOLFSSL_CBIO_ERR_WANT_WRITE;
            default:
                WOLFSSL_MSG("ISO-TP got unexpected flow control packet");
                return WOLFSSL_CBIO_ERR_GENERAL;
        }
    }
    return 0;
}

int ISOTP_Send(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    int ret;
    struct isotp_wolfssl_ctx *isotp_ctx;
    (void) ssl;

    if (!ctx) {
        WOLFSSL_MSG("ISO-TP requires wolfSSL_SetIO_ISOTP to be called first");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    isotp_ctx = (struct isotp_wolfssl_ctx*) ctx;

    /* ISO-TP cannot send more than 4095 bytes, this limits the packet size
     * and wolfSSL will try again with the remaining data */
    if (sz > ISOTP_MAX_DATA_SIZE) {
        sz = ISOTP_MAX_DATA_SIZE;
    }
    /* Can't send whilst we are receiving */
    if (isotp_ctx->state != ISOTP_CONN_STATE_IDLE) {
        return WOLFSSL_ERROR_WANT_WRITE;
    }
    isotp_ctx->state = ISOTP_CONN_STATE_SENDING;

    /* Assuming normal addressing */
    if (sz <= ISOTP_SINGLE_FRAME_DATA_SIZE) {
        ret = isotp_send_single_frame(isotp_ctx, buf, (word16)sz);
    } else {
        ret = isotp_send_first_frame(isotp_ctx, buf, (word16)sz);
    }
    isotp_ctx->state = ISOTP_CONN_STATE_IDLE;

    if (ret == 0) {
        return sz;
    }
    return ret;
}

static int isotp_receive_single_frame(struct isotp_wolfssl_ctx *ctx)
{
    byte data_size;

    /* 1 nibble for data size which will be 1 - 7 in a regular 8 byte CAN
     * packet */
    data_size = (byte)ctx->frame.data[0] & 0xf;
    if (ctx->receive_buffer_size < (int)data_size) {
        WOLFSSL_MSG("ISO-TP buffer is too small to receive data");
        return BUFFER_E;
    }
    XMEMCPY(ctx->receive_buffer, &ctx->frame.data[1], data_size);
    return data_size;
}

static int isotp_receive_multi_frame(struct isotp_wolfssl_ctx *ctx)
{
    int ret;
    word16 data_size;
    byte delay = 0;

    /* Increase receive timeout for enforced ms delay */
    if (ctx->receive_delay <= ISOTP_MAX_MS_FRAME_DELAY) {
        delay = ctx->receive_delay;
    }
    /* Still processing first frame.
     * Full data size is lower nibble of first byte for the most significant
     * followed by the second byte for the rest. Last 6 bytes are data */
    data_size = ((ctx->frame.data[0] & 0xf) << 8) + ctx->frame.data[1];
    XMEMCPY(ctx->receive_buffer, &ctx->frame.data[2], ISOTP_FIRST_FRAME_DATA_SIZE);
    /* Need to send a flow control packet to either cancel or continue
     * transmission of data */
    if (ctx->receive_buffer_size < data_size) {
        isotp_send_flow_control(ctx, TRUE);
        WOLFSSL_MSG("ISO-TP buffer is too small to receive data");
        return BUFFER_E;
    }
    isotp_send_flow_control(ctx, FALSE);

    ctx->buf_length = ISOTP_FIRST_FRAME_DATA_SIZE;
    ctx->buf_ptr = ctx->receive_buffer + ISOTP_FIRST_FRAME_DATA_SIZE;
    data_size -= ISOTP_FIRST_FRAME_DATA_SIZE;
    ctx->sequence = 1;

    while(data_size) {
        enum isotp_frame_type type;
        byte sequence;
        byte frame_len;
        ret = ctx->recv_fn(&ctx->frame, ctx->arg, ISOTP_DEFAULT_TIMEOUT +
                (delay / 1000));
        if (ret == 0) {
            return WOLFSSL_CBIO_ERR_TIMEOUT;
        }
        type = ctx->frame.data[0] >> 4;
        /* Consecutive frames have sequence number as lower nibble */
        sequence = ctx->frame.data[0] & 0xf;
        if (type != ISOTP_FRAME_TYPE_CONSECUTIVE) {
            WOLFSSL_MSG("ISO-TP frames out of sequence");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
        if (sequence != ctx->sequence) {
            WOLFSSL_MSG("ISO-TP frames out of sequence");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
        /* Last 7 bytes or whatever we got after the first byte is data */
        frame_len = ctx->frame.length - 1;
        XMEMCPY(ctx->buf_ptr, &ctx->frame.data[1], frame_len);
        ctx->buf_ptr += frame_len;
        ctx->buf_length += frame_len;
        data_size -= frame_len;

        /* Sequence is 0 - 15 (first 0 is implied for first packet */
        ctx->sequence++;
        if (ctx->sequence > ISOTP_MAX_SEQUENCE_COUNTER) {
            ctx->sequence = 0;
        }
    }
    return ctx->buf_length;

}

/* The wolfSSL receive callback, needs to buffer because we need to grab all
 * incoming data, even if wolfSSL doesn't want it all yet */
int ISOTP_Receive(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    enum isotp_frame_type type;
    int ret;
    struct isotp_wolfssl_ctx *isotp_ctx;
    (void) ssl;

    if (!ctx) {
        WOLFSSL_MSG("ISO-TP requires wolfSSL_SetIO_ISOTP to be called first");
        return WOLFSSL_CBIO_ERR_TIMEOUT;
    }
    isotp_ctx = (struct isotp_wolfssl_ctx*)ctx;

    /* Is buffer empty? If so, fill it */
    if (!isotp_ctx->receive_buffer_len) {
        /* Can't send whilst we are receiving */
        if (isotp_ctx->state != ISOTP_CONN_STATE_IDLE) {
            return WOLFSSL_ERROR_WANT_READ;
        }
        isotp_ctx->state = ISOTP_CONN_STATE_RECEIVING;
        do {
            ret = isotp_ctx->recv_fn(&isotp_ctx->frame, isotp_ctx->arg,
                    ISOTP_DEFAULT_TIMEOUT);
        } while (ret == 0);
        if (ret == 0) {
            isotp_ctx->state = ISOTP_CONN_STATE_IDLE;
            return WOLFSSL_CBIO_ERR_TIMEOUT;
        } else if (ret < 0) {
            isotp_ctx->state = ISOTP_CONN_STATE_IDLE;
            WOLFSSL_MSG("ISO-TP receive error");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }

        type = (enum isotp_frame_type) isotp_ctx->frame.data[0] >> 4;

        if (type == ISOTP_FRAME_TYPE_SINGLE) {
            isotp_ctx->receive_buffer_len =
                isotp_receive_single_frame(isotp_ctx);
        } else if (type == ISOTP_FRAME_TYPE_FIRST) {
            isotp_ctx->receive_buffer_len =
                isotp_receive_multi_frame(isotp_ctx);
        } else {
            /* Should never get here */
            isotp_ctx->state = ISOTP_CONN_STATE_IDLE;
            WOLFSSL_MSG("ISO-TP frames out of sequence");
            return WOLFSSL_CBIO_ERR_GENERAL;
        }
        if (isotp_ctx->receive_buffer_len <= 1) {
            isotp_ctx->state = ISOTP_CONN_STATE_IDLE;
            return isotp_ctx->receive_buffer_len;
        } else {
            isotp_ctx->receive_buffer_ptr = isotp_ctx->receive_buffer;
        }
        isotp_ctx->state = ISOTP_CONN_STATE_IDLE;
    }

    /* Return from the buffer */
    if (isotp_ctx->receive_buffer_len >= sz) {
        XMEMCPY(buf, isotp_ctx->receive_buffer_ptr, sz);
        isotp_ctx->receive_buffer_ptr+= sz;
        isotp_ctx->receive_buffer_len-= sz;
        return sz;
    } else {
        XMEMCPY(buf, isotp_ctx->receive_buffer_ptr,
                isotp_ctx->receive_buffer_len);
        sz = isotp_ctx->receive_buffer_len;
        isotp_ctx->receive_buffer_len = 0;
        return sz;
    }
}

int wolfSSL_SetIO_ISOTP(WOLFSSL *ssl, isotp_wolfssl_ctx *ctx,
        can_recv_fn recv_fn, can_send_fn send_fn, can_delay_fn delay_fn,
        word32 receive_delay, char *receive_buffer, int receive_buffer_size,
        void *arg)
{
    if (!ctx || !recv_fn || !send_fn || !delay_fn || !receive_buffer) {
        WOLFSSL_MSG("ISO-TP has missing required parameter");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    ctx->recv_fn = recv_fn;
    ctx->send_fn = send_fn;
    ctx->arg = arg;
    ctx->delay_fn = delay_fn;
    ctx->frame_delay = 0;
    ctx->receive_buffer = receive_buffer;
    ctx->receive_buffer_size = receive_buffer_size;
    ctx->receive_buffer_len = 0;
    ctx->state = ISOTP_CONN_STATE_IDLE;

    wolfSSL_SetIOReadCtx(ssl, ctx);
    wolfSSL_SetIOWriteCtx(ssl, ctx);

    /* Delay of 100 - 900us is 0xfX where X is value / 100. Delay of
     * >= 1000 is divided by 1000. > 127ms is invalid */
    if (receive_delay < 1000) {
        ctx->receive_delay = 0xf0 + (receive_delay / 100);
    } else if (receive_delay <= ISOTP_MAX_MS_FRAME_DELAY * 1000) {
        ctx->receive_delay = receive_delay / 1000;
    } else {
        WOLFSSL_MSG("ISO-TP delay parameter out of bounds");
        return WOLFSSL_CBIO_ERR_GENERAL;
    }
    return 0;
}
#endif
#endif /* WOLFCRYPT_ONLY */
