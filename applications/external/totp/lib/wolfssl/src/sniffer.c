/* sniffer.c
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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>
#include <wolfssl/wolfcrypt/wc_port.h>

#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

/* Build Options:
 * WOLFSSL_SNIFFER_NO_RECOVERY: Do not track missed data count.
 */


/* xctime */
#ifndef XCTIME
   #define XCTIME ctime
#endif

/* only in this file, to avoid confusing future ports leave
 * these defines here. Do not move to wc_port.h */
#ifdef USER_CUSTOM_SNIFFX
    /* To be implemented in user_settings.h */
#elif defined(FUSION_RTOS)
    #include <fcl_network.h>
    #define XINET_NTOA FNS_INET_NTOA
    #define XINET_ATON FNS_INET_ATON
    #define XINET_PTON(a,b,c,d) FNS_INET_PTON((a),(b),(c),(d),NULL)
    #define XINET_NTOP(a,b,c,d) FNS_INET_NTOP((a),(b),(c),(d),NULL)
    #define XINET_ADDR FNS_INET_ADDR
    #define XHTONS FNS_HTONS
    #define XNTOHS FNS_NTOHS
    #define XHTONL FNS_HTONL
    #define XNTOHL FNS_NTOHL
    #define XINADDR_NONE FNS_INADDR_NONE
#else
    /* default */
    #define XINET_NTOA inet_ntoa
    #define XINET_ATON inet_aton
    #define XINET_PTON(a,b,c) inet_pton((a),(b),(c))
    #define XINET_NTOP inet_ntop
    #define XINET_ADDR inet_addr
    #define XHTONS htons
    #define XNTOHS ntohs
    #define XHTONL htonl
    #define XNTOHL ntohl
    #define XINADDR_NONE INADDR_NONE
#endif

#if !defined(WOLFCRYPT_ONLY) && !defined(NO_FILESYSTEM)
#ifdef WOLFSSL_SNIFFER

#include <time.h>

#ifdef FUSION_RTOS
    #include <fns_inet.h>
    #ifdef TCP_PROTOCOL
        #undef TCP_PROTOCOL
    #endif
#else
    #ifndef _WIN32
        #include <arpa/inet.h>
    #else
        #include <ws2tcpip.h>
    #endif
#endif

#ifdef _WIN32
    #define SNPRINTF _snprintf
#else
    #define SNPRINTF snprintf
#endif

#include <wolfssl/internal.h>
#include <wolfssl/error-ssl.h>
#include <wolfssl/sniffer.h>
#include <wolfssl/sniffer_error.h>

#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
#endif
#ifndef NO_DH
    #include <wolfssl/wolfcrypt/dh.h>
#endif
#if defined(HAVE_ECC) || defined(HAVE_CURVE25519)
    #include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifdef HAVE_CURVE25519
    #include <wolfssl/wolfcrypt/curve25519.h>
#endif

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
    #ifdef HAVE_INTEL_QA_SYNC
        #include <wolfssl/wolfcrypt/port/intel/quickassist_sync.h>
    #endif
    #ifdef HAVE_CAVIUM_OCTEON_SYNC
        #include <wolfssl/wolfcrypt/port/cavium/cavium_octeon_sync.h>
    #endif
#endif

#define ERROR_OUT(err, eLabel) { ret = (err); goto eLabel; }

#ifndef WOLFSSL_SNIFFER_TIMEOUT
    #define WOLFSSL_SNIFFER_TIMEOUT 900
    /* Cache unclosed Sessions for 15 minutes since last used */
#endif

/* Misc constants */
enum {
    MAX_SERVER_ADDRESS = 128, /* maximum server address length */
    MAX_SERVER_NAME    = 128, /* maximum server name length */
    MAX_ERROR_LEN      = 80,  /* maximum error length */
    ETHER_IF_ADDR_LEN  = 6,   /* ethernet interface address length */
    LOCAL_IF_ADDR_LEN  = 4,   /* localhost interface address length, !windows */
    TCP_PROTO          = 6,   /* TCP_PROTOCOL */
    IP_HDR_SZ          = 20,  /* IPv4 header length, min */
    IP6_HDR_SZ         = 40,  /* IPv6 header length, min */
    TCP_HDR_SZ         = 20,  /* TCP header length, min */
    IPV4               = 4,   /* IP version 4 */
    IPV6               = 6,   /* IP version 6 */
    TCP_PROTOCOL       = 6,   /* TCP Protocol id */
    NO_NEXT_HEADER     = 59,  /* IPv6 no headers follow */
    TRACE_MSG_SZ       = 80,  /* Trace Message buffer size */
    HASH_SIZE          = 499, /* Session Hash Table Rows */
    PSEUDO_HDR_SZ      = 12,  /* TCP Pseudo Header size in bytes */
    STREAM_INFO_SZ     = 44,  /* SnifferStreamInfo size in bytes */
    FATAL_ERROR_STATE  = 1,   /* SnifferSession fatal error state */
    TICKET_HINT_LEN    = 4,   /* Session Ticket Hint length */
    TICKET_HINT_AGE_LEN= 4,   /* Session Ticket Age add length */
    EXT_TYPE_SZ        = 2,   /* Extension type length */
    MAX_INPUT_SZ       = MAX_RECORD_SIZE + COMP_EXTRA + MAX_MSG_EXTRA +
                         MTU_EXTRA,  /* Max input sz of reassembly */

    /* TLS Extensions */
    EXT_SERVER_NAME                = 0x0000, /* a.k.a. SNI  */
    EXT_MAX_FRAGMENT_LENGTH        = 0x0001,
    EXT_TRUSTED_CA_KEYS            = 0x0003,
    EXT_TRUNCATED_HMAC             = 0x0004,
    EXT_STATUS_REQUEST             = 0x0005, /* a.k.a. OCSP stapling   */
    EXT_SUPPORTED_GROUPS           = 0x000a, /* a.k.a. Supported Curves */
    EXT_EC_POINT_FORMATS           = 0x000b,
    EXT_SIGNATURE_ALGORITHMS       = 0x000d,
    EXT_APPLICATION_LAYER_PROTOCOL = 0x0010, /* a.k.a. ALPN */
    EXT_STATUS_REQUEST_V2          = 0x0011, /* a.k.a. OCSP stapling v2 */
    EXT_ENCRYPT_THEN_MAC           = 0x0016, /* RFC 7366 */
    EXT_MASTER_SECRET              = 0x0017, /* Extended Master Secret Extension ID */
    EXT_TICKET_ID                  = 0x0023, /* Session Ticket Extension ID */
    EXT_PRE_SHARED_KEY             = 0x0029,
    EXT_EARLY_DATA                 = 0x002a,
    EXT_SUPPORTED_VERSIONS         = 0x002b,
    EXT_COOKIE                     = 0x002c,
    EXT_PSK_KEY_EXCHANGE_MODES     = 0x002d,
    EXT_POST_HANDSHAKE_AUTH        = 0x0031,
    EXT_SIGNATURE_ALGORITHMS_CERT  = 0x0032,
    EXT_KEY_SHARE                  = 0x0033,
    EXT_RENEGOTIATION_INFO         = 0xff01
};


#ifdef _WIN32

static HMODULE dllModule;  /* for error string resources */

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    static int didInit = 0;

    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (didInit == 0) {
            dllModule = hModule;
            ssl_InitSniffer();
            didInit = 1;
        }
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        if (didInit) {
            ssl_FreeSniffer();
            didInit = 0;
        }
        break;
    }
    return TRUE;
}

#endif /* _WIN32 */


static WOLFSSL_GLOBAL int TraceOn = 0;         /* Trace is off by default */
static WOLFSSL_GLOBAL XFILE TraceFile = 0;


/* windows uses .rc table for this */
#ifndef _WIN32

static const char* const msgTable[] =
{
    /* 1 */
    "Out of Memory",
    "New SSL Sniffer Server Registered",
    "Checking IP Header",
    "SSL Sniffer Server Not Registered",
    "Checking TCP Header",

    /* 6 */
    "SSL Sniffer Server Port Not Registered",
    "RSA Private Decrypt Error",
    "RSA Private Decode Error",
    "Set Cipher Spec Error",
    "Server Hello Input Malformed",

    /* 11 */
    "Couldn't Resume Session Error",
    "Server Did Resumption",
    "Client Hello Input Malformed",
    "Client Trying to Resume",
    "Handshake Input Malformed",

    /* 16 */
    "Got Hello Verify msg",
    "Got Server Hello msg",
    "Got Cert Request msg",
    "Got Server Key Exchange msg",
    "Got Cert msg",

    /* 21 */
    "Got Server Hello Done msg",
    "Got Finished msg",
    "Got Client Hello msg",
    "Got Client Key Exchange msg",
    "Got Cert Verify msg",

    /* 26 */
    "Got Unknown Handshake msg",
    "New SSL Sniffer Session created",
    "Couldn't create new SSL",
    "Got a Packet to decode",
    "No data present",

    /* 31 */
    "Session Not Found",
    "Got an Old Client Hello msg",
    "Old Client Hello Input Malformed",
    "Old Client Hello OK",
    "Bad Old Client Hello",

    /* 36 */
    "Bad Record Header",
    "Record Header Input Malformed",
    "Got a HandShake msg",
    "Bad HandShake msg",
    "Got a Change Cipher Spec msg",

    /* 41 */
    "Got Application Data msg",
    "Bad Application Data",
    "Got an Alert msg",
    "Another msg to Process",
    "Removing Session From Table",

    /* 46 */
    "Bad Key File",
    "Wrong IP Version",
    "Wrong Protocol type",
    "Packet Short for header processing",
    "Got Unknown Record Type",

    /* 51 */
    "Can't Open Trace File",
    "Session in Fatal Error State",
    "Partial SSL record received",
    "Buffer Error, malformed input",
    "Added to Partial Input",

    /* 56 */
    "Received a Duplicate Packet",
    "Received an Out of Order Packet",
    "Received an Overlap Duplicate Packet",
    "Received an Overlap Reassembly Begin Duplicate Packet",
    "Received an Overlap Reassembly End Duplicate Packet",

    /* 61 */
    "Missed the Client Hello Entirely",
    "Got Hello Request msg",
    "Got Session Ticket msg",
    "Bad Input",
    "Bad Decrypt Type",

    /* 66 */
    "Bad Finished Message Processing",
    "Bad Compression Type",
    "Bad DeriveKeys Error",
    "Saw ACK for Missing Packet Error",
    "Bad Decrypt Operation",

    /* 71 */
    "Decrypt Keys Not Set Up",
    "Late Key Load Error",
    "Got Certificate Status msg",
    "RSA Key Missing Error",
    "Secure Renegotiation Not Supported",

    /* 76 */
    "Get Session Stats Failure",
    "Reassembly Buffer Size Exceeded",
    "Dropping Lost Fragment",
    "Dropping Partial Record",
    "Clear ACK Fault",

    /* 81 */
    "Bad Decrypt Size",
    "Extended Master Secret Hash Error",
    "Handshake Message Split Across TLS Records",
    "ECC Private Decode Error",
    "ECC Public Decode Error",

    /* 86 */
    "Watch callback not set",
    "Watch hash failed",
    "Watch callback failed",
    "Bad Certificate Message",
    "Store data callback not set",

    /* 91 */
    "No data destination Error",
    "Store data callback failed",
    "Loading chain input",
    "Got encrypted extension",
    "Got Hello Retry Request",

    /* 96 */
    "Setting up keys",
    "Unsupported TLS Version",
    "Server Client Key Mismatch",
};


/* *nix version uses table above */
static void GetError(int idx, char* str)
{
    if (str == NULL ||
            idx <= 0 || idx > (int)(sizeof(msgTable)/sizeof(const char* const)))
        return;
    XSTRNCPY(str, msgTable[idx - 1], MAX_ERROR_LEN-1);
    str[MAX_ERROR_LEN-1] = '\0';
}


#else /* _WIN32 */


/* Windows version uses .rc table */
static void GetError(int idx, char* buffer)
{
    if (buffer == NULL)
        return;
    if (!LoadStringA(dllModule, idx, buffer, MAX_ERROR_LEN))
        buffer[0] = 0;
}


#endif /* _WIN32 */


/* Packet Buffer for reassembly list and ready list */
typedef struct PacketBuffer {
    word32  begin;      /* relative sequence begin */
    word32  end;        /* relative sequence end   */
    byte*   data;       /* actual data             */
    struct PacketBuffer* next; /* next on reassembly list or ready list */
} PacketBuffer;


#ifdef HAVE_SNI

/* NamedKey maps a SNI name to a specific private key */
typedef struct NamedKey {
    char             name[MAX_SERVER_NAME];      /* server DNS name */
    word32           nameSz;                     /* size of server DNS name */
    byte*            key;                        /* DER private key */
    word32           keySz;                      /* size of DER private key */
    int              isEphemeralKey;
    struct NamedKey* next;                       /* for list */
} NamedKey;

#endif

/* Sniffer Server holds info for each server/port monitored */
typedef struct SnifferServer {
    WOLFSSL_CTX*   ctx;                          /* SSL context */
    char           address[MAX_SERVER_ADDRESS];  /* passed in server address */
    IpAddrInfo     server;                       /* network order address */
    int            port;                         /* server port */
#ifdef HAVE_SNI
    NamedKey*      namedKeys;                    /* mapping of names and keys */
    wolfSSL_Mutex  namedKeysMutex;               /* mutex for namedKey list */
#endif
    struct SnifferServer* next;                  /* for list */
} SnifferServer;


/* Session Flags */
typedef struct Flags {
    byte           side;            /* which end is current packet headed */
    byte           serverCipherOn;  /* indicates whether cipher is active */
    byte           clientCipherOn;  /* indicates whether cipher is active */
    byte           resuming;        /* did this session come from resumption */
    byte           cached;          /* have we cached this session yet */
    byte           clientHello;     /* processed client hello yet, for SSLv2 */
    byte           finCount;        /* get both FINs before removing */
    byte           fatalError;      /* fatal error state */
    byte           cliAckFault;     /* client acked unseen data from server */
    byte           srvAckFault;     /* server acked unseen data from client */
    byte           cliSkipPartial;  /* client skips partial data to catch up */
    byte           srvSkipPartial;  /* server skips partial data to catch up */
#ifdef HAVE_EXTENDED_MASTER
    byte           expectEms;       /* expect extended master secret */
#endif
    byte           gotFinished;     /* processed finished */
    byte           secRenegEn;      /* secure renegotiation enabled */
#ifdef WOLFSSL_ASYNC_CRYPT
    byte           wasPolled;
#endif
} Flags;


/* Out of Order FIN capture */
typedef struct FinCapture {
    word32 cliFinSeq;               /* client relative sequence FIN  0 is no */
    word32 srvFinSeq;               /* server relative sequence FIN, 0 is no */
    byte   cliCounted;              /* did we count yet, detects duplicates */
    byte   srvCounted;              /* did we count yet, detects duplicates */
} FinCapture;


typedef struct HsHashes {
#ifndef NO_OLD_TLS
#ifndef NO_SHA
    wc_Sha hashSha;
#endif
#ifndef NO_MD5
    wc_Md5 hashMd5;
#endif
#endif /* !NO_OLD_TLS */
#ifndef NO_SHA256
    wc_Sha256 hashSha256;
#endif
#ifdef WOLFSSL_SHA384
    wc_Sha384 hashSha384;
#endif
} HsHashes;

typedef struct KeyShareInfo {
    word16      named_group;
    int         key_len;
    const byte* key;

    /* additional info */
    int         dh_key_bits;
    int         curve_id;
} KeyShareInfo;

/* maximum previous acks to capture */
#ifndef WC_SNIFFER_HS_ACK_HIST_MAX
#define WC_SNIFFER_HS_ACK_HIST_MAX 10
#endif

/* Sniffer Session holds info for each client/server SSL/TLS session */
typedef struct SnifferSession {
    SnifferServer* context;         /* server context */
    WOLFSSL*       sslServer;       /* SSL server side decode */
    WOLFSSL*       sslClient;       /* SSL client side decode */
    IpAddrInfo     server;          /* server address in network byte order */
    IpAddrInfo     client;          /* client address in network byte order */
    word16         srvPort;         /* server port */
    word16         cliPort;         /* client port */
    word32         cliSeqStart;     /* client start sequence */
    word32         srvSeqStart;     /* server start sequence */
    word32         cliExpected;     /* client expected sequence (relative) */
    word32         srvExpected;     /* server expected sequence (relative) */
    word32         cliAcks[WC_SNIFFER_HS_ACK_HIST_MAX]; /* history of acks during handshake */
    word32         srvAcks[WC_SNIFFER_HS_ACK_HIST_MAX]; /* history of acks during handshake */
    FinCapture     finCapture;      /* retain out of order FIN s */
    Flags          flags;           /* session flags */
    time_t         lastUsed;        /* last used ticks */
    word32         keySz;           /* size of the private key */
    PacketBuffer*  cliReassemblyList; /* client out of order packets */
    PacketBuffer*  srvReassemblyList; /* server out of order packets */
    word32         cliReassemblyMemory; /* client packet memory used */
    word32         srvReassemblyMemory; /* server packet memory used */
    struct SnifferSession* next;    /* for hash table list */
    byte*          ticketID;        /* mac ID of session ticket */
#ifdef HAVE_MAX_FRAGMENT
    byte*          tlsFragBuf;
    word32         tlsFragOffset;
    word32         tlsFragSize;
#endif
#ifdef HAVE_SNI
    const char*    sni;             /* server name indication */
#endif
#ifdef HAVE_EXTENDED_MASTER
    HsHashes*      hash;
#endif
#ifdef WOLFSSL_TLS13
    byte*          cliKeyShare;
    word32         cliKeyShareSz;
    KeyShareInfo   srvKs;
    KeyShareInfo   cliKs;
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    void*          userCtx;
    word32         pendSeq; /* when WC_PENDING_E is returned capture sequence */
#endif
    int            error;   /* store the last set error number */
    byte           verboseErr; /* Last set error is helpful and should
                                *  not be overwritten by FATAL_ERROR_STATE */
} SnifferSession;


/* Sniffer Server List and mutex */
static THREAD_LS_T WOLFSSL_GLOBAL SnifferServer* ServerList = NULL;
#ifndef HAVE_C___ATOMIC
static WOLFSSL_GLOBAL wolfSSL_Mutex ServerListMutex;
#endif

/* Session Hash Table, mutex, and count */
static THREAD_LS_T WOLFSSL_GLOBAL SnifferSession* SessionTable[HASH_SIZE];
#ifndef HAVE_C___ATOMIC
static WOLFSSL_GLOBAL wolfSSL_Mutex SessionMutex;
#endif
static THREAD_LS_T WOLFSSL_GLOBAL int SessionCount = 0;

static WOLFSSL_GLOBAL int RecoveryEnabled    = 0;  /* global switch */
static WOLFSSL_GLOBAL int MaxRecoveryMemory  = -1;
                                           /* per session max recovery memory */
#ifndef WOLFSSL_SNIFFER_NO_RECOVERY
/* Recovery of missed data switches and stats */
static WOLFSSL_GLOBAL wolfSSL_Mutex RecoveryMutex; /* for stats */
/* # of sessions with missed data */
static WOLFSSL_GLOBAL word32 MissedDataSessions = 0;
#endif

/* Connection Info Callback */
static WOLFSSL_GLOBAL SSLConnCb ConnectionCb;
static WOLFSSL_GLOBAL void*     ConnectionCbCtx = NULL;

#ifdef WOLFSSL_SNIFFER_STATS
/* Sessions Statistics */
static WOLFSSL_GLOBAL SSLStats SnifferStats;
static WOLFSSL_GLOBAL wolfSSL_Mutex StatsMutex;
#endif

#ifdef WOLFSSL_SNIFFER_KEY_CALLBACK
static WOLFSSL_GLOBAL SSLKeyCb KeyCb;
static WOLFSSL_GLOBAL void*    KeyCbCtx = NULL;
#endif

#ifdef WOLFSSL_SNIFFER_WATCH
/* Watch Key Callback */
static WOLFSSL_GLOBAL SSLWatchCb WatchCb;
static WOLFSSL_GLOBAL void*      WatchCbCtx = NULL;
#endif

#ifdef WOLFSSL_SNIFFER_STORE_DATA_CB
/* Store Data Callback */
static WOLFSSL_GLOBAL SSLStoreDataCb StoreDataCb;
#endif


#ifndef WOLFSSL_SNIFFER_NO_RECOVERY
static void UpdateMissedDataSessions(void)
{
    wc_LockMutex(&RecoveryMutex);
    MissedDataSessions += 1;
    wc_UnLockMutex(&RecoveryMutex);
}
#endif

#ifdef WOLFSSL_SNIFFER_STATS
    #ifdef HAVE_C___ATOMIC
        #define LOCK_STAT() WC_DO_NOTHING
        #define UNLOCK_STAT() WC_DO_NOTHING
        #define NOLOCK_ADD_TO_STAT(x,y) ({ TraceStat(#x, y); \
            __atomic_fetch_add(&x, y, __ATOMIC_RELAXED); })
    #else
        #define LOCK_STAT() wc_LockMutex(&StatsMutex)
        #define UNLOCK_STAT() wc_UnLockMutex(&StatsMutex)
        #define NOLOCK_ADD_TO_STAT(x,y) ({ TraceStat(#x, y); x += y; })
    #endif
    #define NOLOCK_INC_STAT(x) NOLOCK_ADD_TO_STAT(x,1)
    #define ADD_TO_STAT(x,y) do { LOCK_STAT(); \
        NOLOCK_ADD_TO_STAT(x,y); UNLOCK_STAT(); } while (0)
    #define INC_STAT(x) do { LOCK_STAT(); \
        NOLOCK_INC_STAT(x); UNLOCK_STAT(); } while (0)
#endif /* WOLFSSL_SNIFFER_STATS */

#ifdef HAVE_C___ATOMIC
    #define LOCK_SESSION() WC_DO_NOTHING
    #define UNLOCK_SESSION() WC_DO_NOTHING
    #define LOCK_SERVER_LIST() WC_DO_NOTHING
    #define UNLOCK_SERVER_LIST() WC_DO_NOTHING
#else
    #define LOCK_SESSION() wc_LockMutex(&SessionMutex)
    #define UNLOCK_SESSION() wc_UnLockMutex(&SessionMutex)
    #define LOCK_SERVER_LIST() wc_LockMutex(&ServerListMutex)
    #define UNLOCK_SERVER_LIST() wc_UnLockMutex(&ServerListMutex)
#endif


#if defined(WOLF_CRYPTO_CB) || defined(WOLFSSL_ASYNC_CRYPT)
    static WOLFSSL_GLOBAL int CryptoDeviceId = INVALID_DEVID;
#endif


/* Initialize overall Sniffer */
void ssl_InitSniffer_ex(int devId)
{
    wolfSSL_Init();
#ifndef HAVE_C___ATOMIC
    wc_InitMutex(&ServerListMutex);
    wc_InitMutex(&SessionMutex);
#endif
#ifndef WOLFSSL_SNIFFER_NO_RECOVERY
    wc_InitMutex(&RecoveryMutex);
#endif
#ifdef WOLFSSL_SNIFFER_STATS
    XMEMSET(&SnifferStats, 0, sizeof(SSLStats));
    wc_InitMutex(&StatsMutex);
#endif
#if defined(WOLF_CRYPTO_CB) || defined(WOLFSSL_ASYNC_CRYPT)
    CryptoDeviceId = devId;
#endif
    (void)devId;
}

static int GetDevId(void)
{
    int devId = INVALID_DEVID;

#ifdef WOLF_CRYPTO_CB
    #ifdef HAVE_INTEL_QA_SYNC
    devId = wc_CryptoCb_InitIntelQa();
    if (devId == INVALID_DEVID) {
        fprintf(stderr, "Couldn't init the Intel QA\n");
    }
    #endif
    #ifdef HAVE_CAVIUM_OCTEON_SYNC
    devId = wc_CryptoCb_InitOcteon();
    if (devId == INVALID_DEVID) {
        fprintf(stderr, "Couldn't init the Octeon\n");
    }
    #endif
#endif

    return devId;
}

void ssl_InitSniffer(void)
{
    int devId = GetDevId();

#ifdef WOLFSSL_ASYNC_CRYPT
    if (wolfAsync_DevOpen(&devId) < 0) {
        fprintf(stderr, "Async device open failed\nRunning without async\n");
        devId = INVALID_DEVID;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    (void)devId;

    ssl_InitSniffer_ex(devId);
}

void ssl_InitSniffer_ex2(int threadNum)
{
    int devId = GetDevId();

#ifdef WOLFSSL_ASYNC_CRYPT
#ifndef WC_NO_ASYNC_THREADING
    if (wolfAsync_DevOpenThread(&devId, &threadNum) < 0)
#else
    if (wolfAsync_DevOpen(&devId) < 0)
#endif
    {
        fprintf(stderr, "Async device open failed\nRunning without async\n");
        devId = INVALID_DEVID;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

    (void)devId;
    (void)threadNum;

    ssl_InitSniffer_ex(devId);
}

#ifdef HAVE_SNI

/* Free Named Key and the zero out the private key it holds */
static void FreeNamedKey(NamedKey* in)
{
    if (in) {
        if (in->key) {
            ForceZero(in->key, in->keySz);
            XFREE(in->key, NULL, DYNAMIC_TYPE_X509);
        }
        XFREE(in, NULL, DYNAMIC_TYPE_SNIFFER_NAMED_KEY);
    }
}


static void FreeNamedKeyList(NamedKey* in)
{
    NamedKey* next;

    while (in) {
        next = in->next;
        FreeNamedKey(in);
        in = next;
    }
}

#endif


/* Free Sniffer Server's resources/self */
static void FreeSnifferServer(SnifferServer* srv)
{
    if (srv) {
#ifdef HAVE_SNI
        wc_LockMutex(&srv->namedKeysMutex);
        FreeNamedKeyList(srv->namedKeys);
        wc_UnLockMutex(&srv->namedKeysMutex);
        wc_FreeMutex(&srv->namedKeysMutex);
#endif
        wolfSSL_CTX_free(srv->ctx);
    }
    XFREE(srv, NULL, DYNAMIC_TYPE_SNIFFER_SERVER);
}


/* free PacketBuffer's resources/self */
static void FreePacketBuffer(PacketBuffer* del)
{
    if (del) {
        XFREE(del->data, NULL, DYNAMIC_TYPE_SNIFFER_PB_BUFFER);
        XFREE(del, NULL, DYNAMIC_TYPE_SNIFFER_PB);
    }
}


/* remove PacketBuffer List */
static void FreePacketList(PacketBuffer* in)
{
    if (in) {
        PacketBuffer* del;
        PacketBuffer* packet = in;

        while (packet) {
            del = packet;
            packet = packet->next;
            FreePacketBuffer(del);
        }
    }
}


/* Free Sniffer Session's resources/self */
static void FreeSnifferSession(SnifferSession* session)
{
    if (session) {
        wolfSSL_free(session->sslClient);
        wolfSSL_free(session->sslServer);

        FreePacketList(session->cliReassemblyList);
        FreePacketList(session->srvReassemblyList);

        XFREE(session->ticketID, NULL, DYNAMIC_TYPE_SNIFFER_TICKET_ID);
#ifdef HAVE_EXTENDED_MASTER
        XFREE(session->hash, NULL, DYNAMIC_TYPE_HASHES);
#endif
#ifdef WOLFSSL_TLS13
        if (session->cliKeyShare)
            XFREE(session->cliKeyShare, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
#ifdef HAVE_MAX_FRAGMENT
        if (session->tlsFragBuf) {
            XFREE(session->tlsFragBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            session->tlsFragBuf = NULL;
        }
#endif
    }
    XFREE(session, NULL, DYNAMIC_TYPE_SNIFFER_SESSION);
}


/* Free overall Sniffer */
void ssl_FreeSniffer(void)
{
    SnifferServer*  srv;
    SnifferServer*  removeServer;
    SnifferSession* session;
    SnifferSession* removeSession;
    int i;

    LOCK_SERVER_LIST();
    LOCK_SESSION();

    /* Free sessions (wolfSSL objects) first */
    for (i = 0; i < HASH_SIZE; i++) {
        session = SessionTable[i];
        while (session) {
            removeSession = session;
            session = session->next;
            FreeSnifferSession(removeSession);
        }
    }
    XMEMSET(SessionTable, 0, sizeof(SessionTable));
    SessionCount = 0;

    /* Then server (wolfSSL_CTX) */
    srv = ServerList;
    while (srv) {
        removeServer = srv;
        srv = srv->next;
        FreeSnifferServer(removeServer);
    }
    ServerList = NULL;

    UNLOCK_SESSION();
    UNLOCK_SERVER_LIST();
#ifndef WOLFSSL_SNIFFER_NO_RECOVERY
    wc_FreeMutex(&RecoveryMutex);
#endif
#ifndef HAVE_C___ATOMIC
    wc_FreeMutex(&SessionMutex);
    wc_FreeMutex(&ServerListMutex);
#endif

#ifdef WOLF_CRYPTO_CB
    #ifdef HAVE_INTEL_QA_SYNC
    wc_CryptoCb_CleanupIntelQa(&CryptoDeviceId);
    #endif
    #ifdef HAVE_CAVIUM_OCTEON_SYNC
    wc_CryptoCb_CleanupOcteon(&CryptoDeviceId);
    #endif
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    wolfAsync_DevClose(&CryptoDeviceId);
#endif

    if (TraceFile) {
        TraceOn = 0;
        XFCLOSE(TraceFile);
        TraceFile = NULL;
    }

    wolfSSL_Cleanup();
}


#ifdef HAVE_EXTENDED_MASTER

static int HashInit(HsHashes* hash)
{
    int ret = 0;

    XMEMSET(hash, 0, sizeof(HsHashes));

#ifndef NO_OLD_TLS
#ifndef NO_SHA
    if (ret == 0)
        ret = wc_InitSha(&hash->hashSha);
#endif
#ifndef NO_MD5
    if (ret == 0)
        ret = wc_InitMd5(&hash->hashMd5);
#endif
#endif /* !NO_OLD_TLS */
#ifndef NO_SHA256
    if (ret == 0)
        ret = wc_InitSha256(&hash->hashSha256);
#endif
#ifdef WOLFSSL_SHA384
    if (ret == 0)
        ret = wc_InitSha384(&hash->hashSha384);
#endif

    return ret;
}

static int HashUpdate(HsHashes* hash, const byte* input, int sz)
{
    int ret = 0;

    input -= HANDSHAKE_HEADER_SZ;
    sz += HANDSHAKE_HEADER_SZ;

#ifndef NO_OLD_TLS
#ifndef NO_SHA
    if (ret == 0)
        ret = wc_ShaUpdate(&hash->hashSha, input, sz);
#endif
#ifndef NO_MD5
    if (ret == 0)
        ret = wc_Md5Update(&hash->hashMd5, input, sz);
#endif
#endif /* !NO_OLD_TLS */
#ifndef NO_SHA256
    if (ret == 0)
        ret = wc_Sha256Update(&hash->hashSha256, input, sz);
#endif
#ifdef WOLFSSL_SHA384
    if (ret == 0)
        ret = wc_Sha384Update(&hash->hashSha384, input, sz);
#endif

    return ret;
}

static int HashCopy(HS_Hashes* d, HsHashes* s)
{
#ifndef NO_OLD_TLS
#ifndef NO_SHA
    XMEMCPY(&d->hashSha, &s->hashSha, sizeof(wc_Sha));
#endif
#ifndef NO_MD5
    XMEMCPY(&d->hashMd5, &s->hashMd5, sizeof(wc_Md5));
#endif
#endif /* !NO_OLD_TLS */
#ifndef NO_SHA256
    XMEMCPY(&d->hashSha256, &s->hashSha256, sizeof(wc_Sha256));
#endif
#ifdef WOLFSSL_SHA384
    XMEMCPY(&d->hashSha384, &s->hashSha384, sizeof(wc_Sha384));
#endif

    return 0;
}

#endif


/* Initialize a SnifferServer */
static void InitSnifferServer(SnifferServer* sniffer)
{
    XMEMSET(sniffer, 0, sizeof(SnifferServer));
}


/* Initialize session flags */
static void InitFlags(Flags* flags)
{
    XMEMSET(flags, 0, sizeof(Flags));
}


/* Initialize FIN Capture */
static void InitFinCapture(FinCapture* cap)
{
    XMEMSET(cap, 0, sizeof(FinCapture));
}


/* Initialize a Sniffer Session */
static void InitSession(SnifferSession* session)
{
    XMEMSET(session, 0, sizeof(SnifferSession));
    InitFlags(&session->flags);
    InitFinCapture(&session->finCapture);
}


/* IP Info from IP Header */
typedef struct IpInfo {
    int    length;        /* length of this header */
    int    total;         /* total length of fragment */
    IpAddrInfo src;       /* network order source address */
    IpAddrInfo dst;       /* network order destination address */
} IpInfo;


/* TCP Info from TCP Header */
typedef struct TcpInfo {
    int    srcPort;       /* source port */
    int    dstPort;       /* destination port */
    int    length;        /* length of this header */
    word32 sequence;      /* sequence number */
    word32 ackNumber;     /* ack number */
    byte   fin;           /* FIN set */
    byte   rst;           /* RST set */
    byte   syn;           /* SYN set */
    byte   ack;           /* ACK set */
} TcpInfo;


/* Tcp Pseudo Header for Checksum calculation */
typedef struct TcpPseudoHdr {
    word32  src;        /* source address */
    word32  dst;        /* destination address */
    byte    rsv;        /* reserved, always 0 */
    byte    protocol;   /* IP protocol */
    word16  length;     /* tcp header length + data length (doesn't include */
                        /* pseudo header length) network order */
} TcpPseudoHdr;


#ifdef WOLFSSL_ENCRYPTED_KEYS
/* Password Setting Callback */
static int SetPassword(char* passwd, int sz, int rw, void* userdata)
{
    (void)rw;
    XSTRNCPY(passwd, (const char*)userdata, sz);
    return (int)XSTRLEN((const char*)userdata);
}
#endif

/* Ethernet Header */
typedef struct EthernetHdr {
    byte   dst[ETHER_IF_ADDR_LEN];    /* destination host address */
    byte   src[ETHER_IF_ADDR_LEN];    /* source  host address */
    word16 type;                      /* IP, ARP, etc */
} EthernetHdr;


/* IPv4 Header */
typedef struct IpHdr {
    byte    ver_hl;              /* version/header length */
    byte    tos;                 /* type of service */
    word16  length;              /* total length */
    word16  id;                  /* identification */
    word16  offset;              /* fragment offset field */
    byte    ttl;                 /* time to live */
    byte    protocol;            /* protocol */
    word16  sum;                 /* checksum */
    word32  src;                 /* source address */
    word32  dst;                 /* destination address */
} IpHdr;


/* IPv6 Header */
typedef struct Ip6Hdr {
    byte    ver_hl;              /* version/traffic class high */
    byte    tc_fl;               /* traffic class low/flow label high */
    word16  fl;                  /* flow label low */
    word16  length;              /* payload length */
    byte    next_header;         /* next header (6 for TCP, any other skip) */
    byte    hl;                  /* hop limit */
    byte    src[16];             /* source address */
    byte    dst[16];             /* destination address */
} Ip6Hdr;


/* IPv6 extension header */
typedef struct Ip6ExtHdr {
    byte next_header;            /* next header (6 for TCP, any other skip) */
    byte length;                 /* length in 8-octet units - 1 */
    byte reserved[6];
} Ip6ExtHdr;


#define IP_HL(ip)      ( (((ip)->ver_hl) & 0x0f) * 4)
#define IP_V(ip)       ( ((ip)->ver_hl) >> 4)

/* TCP Header */
typedef struct TcpHdr {
    word16  srcPort;            /* source port */
    word16  dstPort;            /* destination port */
    word32  sequence;           /* sequence number */
    word32  ack;                /* acknowledgment number */
    byte    offset;             /* data offset, reserved */
    byte    flags;              /* option flags */
    word16  window;             /* window */
    word16  sum;                /* checksum */
    word16  urgent;             /* urgent pointer */
} TcpHdr;

#define TCP_LEN(tcp)  ( (((tcp)->offset & 0xf0) >> 4) * 4)
#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_ACK 0x10





/* Use platform specific GetError to write to trace file if tracing */
static void TraceError(int idx, char* error)
{
    if (TraceOn) {
        char myBuffer[MAX_ERROR_LEN];
        if (error == NULL) {
            error = myBuffer;
            GetError(idx, myBuffer);
        }
        XFPRINTF(TraceFile, "\t%s\n", error);
#ifdef DEBUG_SNIFFER
        XFPRINTF(stderr,    "\t%s\n", error);
#endif
    }
}

static void Trace(int idx)
{
    TraceError(idx, NULL);
}


/* Show TimeStamp for beginning of packet Trace */
static void TraceHeader(void)
{
    if (TraceOn) {
        time_t ticks = wc_Time(NULL);
        XFPRINTF(TraceFile, "\n%s", XCTIME(&ticks));
    }
}


/* Show Set Server info for Trace */
static void TraceSetServer(const char* srv, int port, const char* keyFile)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tTrying to install a new Sniffer Server with\n");
        XFPRINTF(TraceFile, "\tserver: %s, port: %d, keyFile: %s\n", srv, port,
                                                                    keyFile);
    }
}


#ifdef HAVE_SNI

/* Show Set Named Server info for Trace */
static void TraceSetNamedServer(const char* name,
                                 const char* srv, int port, const char* keyFile)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tTrying to install a new Sniffer Server with\n");
        XFPRINTF(TraceFile, "\tname: %s, server: %s, port: %d, keyFile: %s\n",
        name ? name : "",
        srv ? srv : "",
        port,
        keyFile ? keyFile : "");
    }
}

#endif


/* Trace got packet number */
static void TracePacket(void)
{
    if (TraceOn) {
        static word32 packetNumber = 0;
        XFPRINTF(TraceFile, "\tGot a Packet to decode, packet %u\n",
                ++packetNumber);
    }
}


/* Convert network byte order address into human readable */
static const char* IpToS(int version, void* src, char* dst)
{
    return XINET_NTOP(version, src, dst, TRACE_MSG_SZ);
}


/* Show destination and source address from Ip Hdr for packet Trace */
static void TraceIP(IpHdr* iphdr)
{
    if (TraceOn) {
        char src[TRACE_MSG_SZ];
        char dst[TRACE_MSG_SZ];
        XFPRINTF(TraceFile, "\tdst:%s src:%s\n",
                IpToS(AF_INET, &iphdr->dst, dst),
                IpToS(AF_INET, &iphdr->src, src));
    }
}


/* Show destination and source address from Ip6Hdr for packet Trace */
static void TraceIP6(Ip6Hdr* iphdr)
{
    if (TraceOn) {
        char src[TRACE_MSG_SZ];
        char dst[TRACE_MSG_SZ];
        XFPRINTF(TraceFile, "\tdst: %s src: %s\n",
                IpToS(AF_INET6, iphdr->dst, dst),
                IpToS(AF_INET6, iphdr->src, src));
    }
}


/* Show destination and source port from Tcp Hdr for packet Trace */
static void TraceTcp(TcpHdr* tcphdr)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tdstPort:%u srcPort:%u\n", XNTOHS(tcphdr->dstPort),
                XNTOHS(tcphdr->srcPort));
    }
}


/* Show sequence and payload length for Trace */
static void TraceSequence(word32 seq, int len)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tSequence:%u, payload length:%d\n", seq, len);
    }
}


/* Show sequence and payload length for Trace */
static void TraceAck(word32 acknowledgement, word32 expected)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tAck:%u Expected:%u\n", acknowledgement,
                 expected);
    }
}


/* Show relative expected and relative received sequences */
static void TraceRelativeSequence(word32 expected, word32 got)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tExpected sequence:%u, received sequence:%u\n",
                expected, got);
    }
}


/* Show server sequence startup from SYN */
static void TraceServerSyn(word32 seq)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tServer SYN, Sequence Start:%u\n", seq);
    }
}


/* Show client sequence startup from SYN */
static void TraceClientSyn(word32 seq)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tClient SYN, Sequence Start:%u\n", seq);
    }
}


/* Show client FIN capture */
static void TraceClientFin(word32 finSeq, word32 relSeq)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tClient FIN capture:%u, current SEQ:%u\n",
                finSeq, relSeq);
    }
}


/* Show server FIN capture */
static void TraceServerFin(word32 finSeq, word32 relSeq)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tServer FIN capture:%u, current SEQ:%u\n",
                finSeq, relSeq);
    }
}


/* Show number of SSL data bytes decoded, could be 0 (ok) */
static void TraceGotData(int bytes)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\t%d bytes of SSL App data processed\n", bytes);
    }
}


/* Show bytes added to old SSL App data */
static void TraceAddedData(int newBytes, int existingBytes)
{
    if (TraceOn) {
        XFPRINTF(TraceFile,
                "\t%d bytes added to %d existing bytes in User Buffer\n",
                newBytes, existingBytes);
    }
}


/* Show Stale Session */
static void TraceStaleSession(void)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tFound a stale session\n");
    }
}


/* Show Finding Stale Sessions */
static void TraceFindingStale(void)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tTrying to find Stale Sessions\n");
    }
}


/* Show Removed Session */
static void TraceRemovedSession(void)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tRemoved it\n");
    }
}


/* Show SSLInfo if provided and is valid. */
static void TraceSessionInfo(SSLInfo* sslInfo)
{
    if (TraceOn) {
        if (sslInfo != NULL && sslInfo->isValid) {
            XFPRINTF(TraceFile,
                    "\tver:(%u %u) suiteId:(%02x %02x) suiteName:(%s) "
                    #ifdef HAVE_SNI
                    "sni:(%s) "
                    #endif
                    "keySize:(%u)\n",
                    sslInfo->protocolVersionMajor,
                    sslInfo->protocolVersionMinor,
                    sslInfo->serverCipherSuite0,
                    sslInfo->serverCipherSuite,
                    sslInfo->serverCipherSuiteName,
                    #ifdef HAVE_SNI
                    sslInfo->serverNameIndication,
                    #endif
                    sslInfo->keySize);
        }
    }
}


#ifdef WOLFSSL_SNIFFER_STATS

/* Show value added to a named statistic. */
static void TraceStat(const char* name, int add)
{
    if (TraceOn) {
        XFPRINTF(TraceFile, "\tAdding %d to %s\n", add, name);
    }
}

#endif


/* Set user error string */
static void SetError(int idx, char* error, SnifferSession* session, int fatal)
{
    GetError(idx, error);
    TraceError(idx, error);
    if (session)
        session->error = idx;
    if (session && fatal == FATAL_ERROR_STATE)
        session->flags.fatalError = 1;
}


/* Compare IpAddrInfo structs */
static WC_INLINE int MatchAddr(IpAddrInfo l, IpAddrInfo r)
{
    if (l.version == r.version) {
        if (l.version == IPV4)
            return (l.ip4 == r.ip4);
        else if (l.version == IPV6)
            return (0 == XMEMCMP(l.ip6, r.ip6, sizeof(l.ip6)));
    }
    return 0;
}


#ifndef WOLFSSL_SNIFFER_WATCH

/* See if this IPV4 network order address has been registered */
/* return 1 is true, 0 is false */
static int IsServerRegistered(word32 addr)
{
    int ret = 0;     /* false */
    SnifferServer* sniffer;

    LOCK_SERVER_LIST();

    sniffer = ServerList;
    while (sniffer) {
        if (sniffer->server.ip4 == addr) {
            ret = 1;
            break;
        }
        sniffer = sniffer->next;
    }

    UNLOCK_SERVER_LIST();

    return ret;
}


/* See if this port has been registered to watch */
/* See if this IPV4 network order address has been registered */
/* return 1 is true, 0 is false */
static int IsServerRegistered6(byte* addr)
{
    int ret = 0;     /* false */
    SnifferServer* sniffer;

    LOCK_SERVER_LIST();

    sniffer = ServerList;
    while (sniffer) {
        if (sniffer->server.version == IPV6 &&
                0 == XMEMCMP(sniffer->server.ip6, addr, sizeof(sniffer->server.ip6))) {
            ret = 1;
            break;
        }
        sniffer = sniffer->next;
    }

    UNLOCK_SERVER_LIST();

    return ret;
}


/* See if this port has been registered to watch */
/* return 1 is true, 0 is false */
static int IsPortRegistered(word32 port)
{
    int ret = 0;    /* false */
    SnifferServer* sniffer;

    LOCK_SERVER_LIST();

    sniffer = ServerList;
    while (sniffer) {
        if (sniffer->port == (int)port) {
            ret = 1;
            break;
        }
        sniffer = sniffer->next;
    }

    UNLOCK_SERVER_LIST();

    return ret;
}

#endif


/* Get SnifferServer from IP and Port */
static SnifferServer* GetSnifferServer(IpInfo* ipInfo, TcpInfo* tcpInfo)
{
    SnifferServer* sniffer;

    LOCK_SERVER_LIST();

    sniffer = ServerList;

#ifndef WOLFSSL_SNIFFER_WATCH
    while (sniffer) {
        if (sniffer->port == tcpInfo->srcPort &&
                MatchAddr(sniffer->server, ipInfo->src))
            break;
        if (sniffer->port == tcpInfo->dstPort &&
                MatchAddr(sniffer->server, ipInfo->dst))
            break;

        if (sniffer->next)
            sniffer = sniffer->next;
        else
           break;
    }
#else
    (void)ipInfo;
    (void)tcpInfo;
#endif

    UNLOCK_SERVER_LIST();

    return sniffer;
}


/* Hash the Session Info, return hash row */
static word32 SessionHash(IpInfo* ipInfo, TcpInfo* tcpInfo)
{
    word32 hash = 1;

    if (ipInfo->src.version == IPV4) {
        hash *= ipInfo->src.ip4 * ipInfo->dst.ip4;
    }
    else if (ipInfo->src.version == IPV6) {
        word32* x;
        word32  y;
        x = (word32*)ipInfo->src.ip6;
        y = x[0] ^ x[1] ^ x[2] ^ x[3];
        hash *= y;
        x = (word32*)ipInfo->dst.ip6;
        y = x[0] ^ x[1] ^ x[2] ^ x[3];
        hash *= y;
    }
    hash *= tcpInfo->srcPort * tcpInfo->dstPort;

    return hash % HASH_SIZE;
}


/* Get Existing SnifferSession from IP and Port */
static SnifferSession* GetSnifferSession(IpInfo* ipInfo, TcpInfo* tcpInfo)
{
    SnifferSession* session;
    time_t          currTime = wc_Time(NULL);
    word32          row = SessionHash(ipInfo, tcpInfo);

    LOCK_SESSION();
    session = SessionTable[row];
    while (session) {
        if (MatchAddr(session->server, ipInfo->src) &&
            MatchAddr(session->client, ipInfo->dst) &&
                    session->srvPort == tcpInfo->srcPort &&
                    session->cliPort == tcpInfo->dstPort)
            break;

        if (MatchAddr(session->client, ipInfo->src) &&
            MatchAddr(session->server, ipInfo->dst) &&
                    session->cliPort == tcpInfo->srcPort &&
                    session->srvPort == tcpInfo->dstPort)
            break;

        session = session->next;
    }

    if (session)
        session->lastUsed= currTime; /* keep session alive, remove stale will */
                                     /* leave alone */
    UNLOCK_SESSION();

    /* determine side */
    if (session) {
        if (MatchAddr(ipInfo->dst, session->server) &&
            tcpInfo->dstPort == session->srvPort) {

            session->flags.side = WOLFSSL_SERVER_END;
        }
        else {
            session->flags.side = WOLFSSL_CLIENT_END;
        }
    }

    return session;
}


#if defined(HAVE_SNI) || defined(WOLFSSL_SNIFFER_WATCH)

static int LoadKeyFile(byte** keyBuf, word32* keyBufSz,
                const char* keyFile, int keySz, int typeKey,
                const char* password)
{
    byte* loadBuf;
    long fileSz = 0;
    XFILE file;
    int ret = -1;

    if (keyBuf == NULL || keyBufSz == NULL || keyFile == NULL) {
        return -1;
    }

    if (keySz == 0) {
        /* load from file */
        file = XFOPEN(keyFile, "rb");
        if (file == XBADFILE) return -1;
        if(XFSEEK(file, 0, XSEEK_END) != 0) {
            XFCLOSE(file);
            return -1;
        }
        fileSz = XFTELL(file);
        if (fileSz > MAX_WOLFSSL_FILE_SIZE || fileSz < 0) {
            XFCLOSE(file);
            return -1;
        }
        if(XFSEEK(file, 0, XSEEK_SET) != 0) {
            XFCLOSE(file);
            return -1;
        }

        loadBuf = (byte*)XMALLOC(fileSz, NULL, DYNAMIC_TYPE_FILE);
        if (loadBuf == NULL) {
            XFCLOSE(file);
            return -1;
        }

        ret = (int)XFREAD(loadBuf, 1, fileSz, file);
        XFCLOSE(file);

        if (ret != fileSz) {
            XFREE(loadBuf, NULL, DYNAMIC_TYPE_FILE);
            return -1;
        }
    }
    else {
        /* use buffer directly */
        loadBuf = (byte*)XMALLOC(keySz, NULL, DYNAMIC_TYPE_FILE);
        if (loadBuf == NULL) {
            return -1;
        }
        fileSz = keySz;
        XMEMCPY(loadBuf, keyFile, fileSz);
    }

    if (typeKey == WOLFSSL_FILETYPE_PEM) {
        byte* saveBuf   = (byte*)XMALLOC(fileSz, NULL, DYNAMIC_TYPE_X509);
        int   saveBufSz = 0;

        ret = -1;
        if (saveBuf != NULL) {
            saveBufSz = wc_KeyPemToDer(loadBuf, (int)fileSz,
                                                saveBuf, (int)fileSz, password);
            if (saveBufSz < 0) {
                saveBufSz = 0;
                XFREE(saveBuf, NULL, DYNAMIC_TYPE_X509);
                saveBuf = NULL;
            }
            else
                ret = 0;
        }

        ForceZero(loadBuf, (word32)fileSz);
        XFREE(loadBuf, NULL, DYNAMIC_TYPE_FILE);

        if (saveBuf) {
            *keyBuf = saveBuf;
            *keyBufSz = (word32)saveBufSz;
        }
    }
    else {
        *keyBuf = loadBuf;
        *keyBufSz = (word32)fileSz;
    }

    if (ret < 0) {
        return -1;
    }

    return ret;
}

#endif


#ifdef WOLFSSL_SNIFFER_WATCH

static int CreateWatchSnifferServer(char* error)
{
    SnifferServer* sniffer;

    sniffer = (SnifferServer*)XMALLOC(sizeof(SnifferServer), NULL,
            DYNAMIC_TYPE_SNIFFER_SERVER);
    if (sniffer == NULL) {
        SetError(MEMORY_STR, error, NULL, 0);
        return -1;
    }
    InitSnifferServer(sniffer);
    sniffer->ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    if (!sniffer->ctx) {
        SetError(MEMORY_STR, error, NULL, 0);
        FreeSnifferServer(sniffer);
        return -1;
    }
#if defined(WOLF_CRYPTO_CB) || defined(WOLFSSL_ASYNC_CRYPT)
    if (CryptoDeviceId != INVALID_DEVID)
        wolfSSL_CTX_SetDevId(sniffer->ctx, CryptoDeviceId);
#endif

    /* add to server list */
    LOCK_SERVER_LIST();
    sniffer->next = ServerList;
    ServerList = sniffer;
    UNLOCK_SERVER_LIST();

    return 0;
}

#endif

/* Caller locks ServerListMutex */
static int SetNamedPrivateKey(const char* name, const char* address, int port,
    const char* keyFile, int keySz, int typeKey, const char* password,
    char* error, int isEphemeralKey)
{
    SnifferServer* sniffer;
    int            ret;
    int            type = (typeKey == FILETYPE_PEM) ? WOLFSSL_FILETYPE_PEM :
                                                      WOLFSSL_FILETYPE_ASN1;
    int            isNew = 0;
    IpAddrInfo     serverIp;

#ifdef HAVE_SNI
    NamedKey* namedKey = NULL;
#endif

    (void)name;
#ifdef HAVE_SNI
    if (name != NULL) {
        namedKey = (NamedKey*)XMALLOC(sizeof(NamedKey),
                NULL, DYNAMIC_TYPE_SNIFFER_NAMED_KEY);
        if (namedKey == NULL) {
            SetError(MEMORY_STR, error, NULL, 0);
            return -1;
        }
        XMEMSET(namedKey, 0, sizeof(NamedKey));

        namedKey->nameSz = (word32)XSTRLEN(name);
        if (namedKey->nameSz > sizeof(namedKey->name)-1)
            namedKey->nameSz = sizeof(namedKey->name)-1;
        XSTRNCPY(namedKey->name, name, namedKey->nameSz);
        namedKey->name[MAX_SERVER_NAME-1] = '\0';
        namedKey->isEphemeralKey = isEphemeralKey;
        ret = LoadKeyFile(&namedKey->key, &namedKey->keySz,
                          keyFile, keySz, type, password);
        if (ret < 0) {
            SetError(KEY_FILE_STR, error, NULL, 0);
            FreeNamedKey(namedKey);
            return -1;
        }
    }
#endif

    serverIp.version = IPV4;
    serverIp.ip4 = XINET_ADDR(address);
    if (serverIp.ip4 == XINADDR_NONE) {
    #ifdef FUSION_RTOS
        if (XINET_PTON(AF_INET6, address, serverIp.ip6,
                       sizeof(serverIp.ip4)) == 1) {
    #else
        if (XINET_PTON(AF_INET6, address, serverIp.ip6) == 1) {
    #endif
            serverIp.version = IPV6;
        }
    }

    sniffer = ServerList;
    while (sniffer != NULL &&
            (!MatchAddr(sniffer->server, serverIp) || sniffer->port != port)) {
        sniffer = sniffer->next;
    }

    if (sniffer == NULL) {
        isNew = 1;
        sniffer = (SnifferServer*)XMALLOC(sizeof(SnifferServer),
                NULL, DYNAMIC_TYPE_SNIFFER_SERVER);
        if (sniffer == NULL) {
            SetError(MEMORY_STR, error, NULL, 0);
#ifdef HAVE_SNI
            FreeNamedKey(namedKey);
#endif
            return -1;
        }
        InitSnifferServer(sniffer);

        XSTRNCPY(sniffer->address, address, MAX_SERVER_ADDRESS-1);
        sniffer->address[MAX_SERVER_ADDRESS-1] = '\0';
        sniffer->server = serverIp;
        sniffer->port = port;

        sniffer->ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
        if (!sniffer->ctx) {
            SetError(MEMORY_STR, error, NULL, 0);
#ifdef HAVE_SNI
            FreeNamedKey(namedKey);
#endif
            FreeSnifferServer(sniffer);
            return -1;
        }
    #if defined(WOLF_CRYPTO_CB) || defined(WOLFSSL_ASYNC_CRYPT)
        if (CryptoDeviceId != INVALID_DEVID)
            wolfSSL_CTX_SetDevId(sniffer->ctx, CryptoDeviceId);
    #endif
    }

    if (name == NULL) {
        if (password) {
    #ifdef WOLFSSL_ENCRYPTED_KEYS
            wolfSSL_CTX_set_default_passwd_cb(sniffer->ctx, SetPassword);
            wolfSSL_CTX_set_default_passwd_cb_userdata(
                                                 sniffer->ctx, (void*)password);
    #endif
        }

    #ifdef WOLFSSL_STATIC_EPHEMERAL
        if (isEphemeralKey) {
            /* auto detect key type with WC_PK_TYPE_NONE */
            /* keySz == 0 mean load file */
            ret = wolfSSL_CTX_set_ephemeral_key(sniffer->ctx, WC_PK_TYPE_NONE,
                keyFile, keySz, type);
            if (ret == 0)
                ret = WOLFSSL_SUCCESS;
        }
        else
    #endif
        {
            if (keySz == 0) {
                ret = wolfSSL_CTX_use_PrivateKey_file(sniffer->ctx, keyFile, type);
            }
            else {
                ret = wolfSSL_CTX_use_PrivateKey_buffer(sniffer->ctx,
                                            (const byte*)keyFile, keySz, type);
            }
        }
        if (ret != WOLFSSL_SUCCESS) {
            SetError(KEY_FILE_STR, error, NULL, 0);
            if (isNew)
                FreeSnifferServer(sniffer);
            return -1;
        }
    #ifdef WOLF_CRYPTO_CB
        wolfSSL_CTX_SetDevId(sniffer->ctx, CryptoDeviceId);
    #endif
    }
#ifdef HAVE_SNI
    else {
        wc_LockMutex(&sniffer->namedKeysMutex);
        namedKey->next = sniffer->namedKeys;
        sniffer->namedKeys = namedKey;
        wc_UnLockMutex(&sniffer->namedKeysMutex);
    }
#endif

    if (isNew) {
        sniffer->next = ServerList;
        ServerList = sniffer;
    }

#ifndef WOLFSSL_STATIC_EPHEMERAL
    (void)isEphemeralKey;
#endif

    return 0;
}


#ifdef HAVE_SNI
/* Sets the private key for a specific name, server and port  */
/* returns 0 on success, -1 on error */
int ssl_SetNamedPrivateKey(const char* name,
                           const char* address, int port,
                           const char* keyFile, int typeKey,
                           const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetNamedServer(name, address, port, keyFile);

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(name, address, port, keyFile, 0,
                             typeKey, password, error, 0);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}

int ssl_SetNamedPrivateKeyBuffer(const char* name,
                                 const char* address, int port,
                                 const char* keyBuf, int keySz, int typeKey,
                                 const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetNamedServer(name, address, port, NULL);

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(name, address, port, keyBuf, keySz,
                             typeKey, password, error, 0);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}
#endif /* HAVE_SNI */

/* Sets the private key for a specific server and port  */
/* returns 0 on success, -1 on error */
int ssl_SetPrivateKey(const char* address, int port,
                      const char* keyFile, int typeKey,
                      const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetServer(address, port, keyFile);

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(NULL, address, port, keyFile, 0,
                             typeKey, password, error, 0);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}

int ssl_SetPrivateKeyBuffer(const char* address, int port,
                            const char* keyBuf, int keySz, int typeKey,
                            const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetServer(address, port, "from buffer");

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(NULL, address, port, keyBuf, keySz,
                             typeKey, password, error, 0);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}

#ifdef WOLFSSL_STATIC_EPHEMERAL
#ifdef HAVE_SNI
/* Sets the ephemeral key for a specific name, server and port  */
/* returns 0 on success, -1 on error */
int ssl_SetNamedEphemeralKey(const char* name,
                             const char* address, int port,
                             const char* keyFile, int typeKey,
                             const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetNamedServer(name, address, port, keyFile);

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(name, address, port, keyFile, 0,
                             typeKey, password, error, 1);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}

int ssl_SetNamedEphemeralKeyBuffer(const char* name,
                                   const char* address, int port,
                                   const char* keyBuf, int keySz, int typeKey,
                                   const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetNamedServer(name, address, port, NULL);

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(name, address, port, keyBuf, keySz,
                             typeKey, password, error, 1);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}
#endif /* HAVE_SNI */

/* Sets the ephemeral key for a specific server and port  */
/* returns 0 on success, -1 on error */
int ssl_SetEphemeralKey(const char* address, int port,
                        const char* keyFile, int typeKey,
                        const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetServer(address, port, keyFile);

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(NULL, address, port, keyFile, 0,
                             typeKey, password, error, 1);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}

int ssl_SetEphemeralKeyBuffer(const char* address, int port,
                              const char* keyBuf, int keySz, int typeKey,
                              const char* password, char* error)
{
    int ret;

    TraceHeader();
    TraceSetServer(address, port, "from buffer");

    LOCK_SERVER_LIST();
    ret = SetNamedPrivateKey(NULL, address, port, keyBuf, keySz,
                             typeKey, password, error, 1);
    UNLOCK_SERVER_LIST();

    if (ret == 0)
        Trace(NEW_SERVER_STR);

    return ret;
}
#endif /* WOLFSSL_STATIC_EPHEMERAL */

/* Check IP Header for IPV6, TCP, and a registered server address */
/* returns 0 on success, -1 on error */
static int CheckIp6Hdr(Ip6Hdr* iphdr, IpInfo* info, int length, char* error)
{
    int        version = IP_V(iphdr);
    int        exthdrsz = IP6_HDR_SZ;

    TraceIP6(iphdr);
    Trace(IP_CHECK_STR);

    if (version != IPV6) {
        SetError(BAD_IPVER_STR, error, NULL, 0);
        return -1;
    }

    /* Here, we need to move onto next header if not TCP. */
    if (iphdr->next_header != TCP_PROTOCOL) {
        Ip6ExtHdr* exthdr = (Ip6ExtHdr*)((byte*)iphdr + IP6_HDR_SZ);
        do {
            int hdrsz = (exthdr->length + 1) * 8;
            if (hdrsz > length - exthdrsz) {
                SetError(PACKET_HDR_SHORT_STR, error, NULL, 0);
                return -1;
            }
            exthdrsz += hdrsz;
            exthdr = (Ip6ExtHdr*)((byte*)exthdr + hdrsz);
        }
        while (exthdr->next_header != TCP_PROTOCOL &&
                exthdr->next_header != NO_NEXT_HEADER);
    }

#ifndef WOLFSSL_SNIFFER_WATCH
    if (!IsServerRegistered6(iphdr->src) && !IsServerRegistered6(iphdr->dst)) {
        SetError(SERVER_NOT_REG_STR, error, NULL, 0);
        return -1;
    }
#endif

    info->length = exthdrsz;
    info->total = XNTOHS(iphdr->length) + info->length;
    /* IPv6 doesn't include its own header size in the length like v4. */
    info->src.version = IPV6;
    XMEMCPY(info->src.ip6, iphdr->src, sizeof(info->src.ip6));
    info->dst.version = IPV6;
    XMEMCPY(info->dst.ip6, iphdr->dst, sizeof(info->dst.ip6));

    return 0;
}


/* Check IP Header for IPV4, TCP, and a registered server address */
/* If header IPv6, pass to CheckIp6Hdr(). */
/* returns 0 on success, -1 on error */
static int CheckIpHdr(IpHdr* iphdr, IpInfo* info, int length, char* error,
                      int trace)
{
    int version = IP_V(iphdr);

    if (version == IPV6)
        return CheckIp6Hdr((Ip6Hdr*)iphdr, info, length, error);

    if (trace) {
        TraceIP(iphdr);
        Trace(IP_CHECK_STR);
    }

    if (version != IPV4) {
        SetError(BAD_IPVER_STR, error, NULL, 0);
        return -1;
    }

    if (iphdr->protocol != TCP_PROTOCOL) {
        SetError(BAD_PROTO_STR, error, NULL, 0);
        return -1;
    }

    info->length  = IP_HL(iphdr);
    info->total   = XNTOHS(iphdr->length);
    info->src.version = IPV4;
    info->src.ip4 = iphdr->src;
    info->dst.version = IPV4;
    info->dst.ip4 = iphdr->dst;

    if (info->total == 0)
        info->total = length;  /* reassembled may be off */

    return 0;
}


/* Check TCP Header for a registered port */
/* returns 0 on success, -1 on error */
static int CheckTcpHdr(TcpHdr* tcphdr, TcpInfo* info, char* error, int trace)
{
    if (trace) {
        TraceTcp(tcphdr);
        Trace(TCP_CHECK_STR);
    }

    info->srcPort   = XNTOHS(tcphdr->srcPort);
    info->dstPort   = XNTOHS(tcphdr->dstPort);
    info->length    = TCP_LEN(tcphdr);
    info->sequence  = XNTOHL(tcphdr->sequence);
    info->fin       = tcphdr->flags & TCP_FIN;
    info->rst       = tcphdr->flags & TCP_RST;
    info->syn       = tcphdr->flags & TCP_SYN;
    info->ack       = tcphdr->flags & TCP_ACK;
    if (info->ack)
        info->ackNumber = XNTOHL(tcphdr->ack);

    (void)error;

    return 0;
}


/* Decode Record Layer Header */
static int GetRecordHeader(const byte* input, RecordLayerHeader* rh, int* size)
{
    XMEMCPY(rh, input, RECORD_HEADER_SZ);
    *size = (rh->length[0] << 8) | rh->length[1];

    if (*size > (MAX_RECORD_SIZE + COMP_EXTRA + MAX_MSG_EXTRA))
        return LENGTH_ERROR;

    return 0;
}


/* Copies the session's information to the provided sslInfo. Skip copy if
 * SSLInfo is not provided. */
static void CopySessionInfo(SnifferSession* session, SSLInfo* sslInfo)
{
    if (NULL != sslInfo) {
        XMEMSET(sslInfo, 0, sizeof(SSLInfo));

        /* Pass back Session Info after we have processed the Server Hello. */
        if (0 != session->sslServer->options.cipherSuite) {
            const char* pCipher;

            sslInfo->isValid = 1;
            sslInfo->protocolVersionMajor = session->sslServer->version.major;
            sslInfo->protocolVersionMinor = session->sslServer->version.minor;
            sslInfo->serverCipherSuite0 =
                        session->sslServer->options.cipherSuite0;
            sslInfo->serverCipherSuite =
                        session->sslServer->options.cipherSuite;

            pCipher = wolfSSL_get_cipher(session->sslServer);
            if (NULL != pCipher) {
                XSTRNCPY((char*)sslInfo->serverCipherSuiteName, pCipher,
                         sizeof(sslInfo->serverCipherSuiteName) - 1);
                sslInfo->serverCipherSuiteName
                         [sizeof(sslInfo->serverCipherSuiteName) - 1] = '\0';
            }
            sslInfo->keySize = session->keySz;
        #ifdef HAVE_SNI
            if (NULL != session->sni) {
                XSTRNCPY((char*)sslInfo->serverNameIndication,
                    session->sni, sizeof(sslInfo->serverNameIndication) - 1);
                sslInfo->serverNameIndication
                         [sizeof(sslInfo->serverNameIndication) - 1] = '\0';
            }
        #endif
            TraceSessionInfo(sslInfo);
        }
    }
}


/* Call the session connection start callback. */
static void CallConnectionCb(SnifferSession* session)
{
    if (ConnectionCb != NULL) {
        SSLInfo info;
        CopySessionInfo(session, &info);
        ConnectionCb((const void*)session, &info, ConnectionCbCtx);
    }
}

#ifdef SHOW_SECRETS
static void PrintSecret(const char* desc, const byte* buf, int sz)
{
    int i;
    printf("%s: ", desc);
    for (i = 0; i < sz; i++) {
        printf("%02x", buf[i]);
    }
    printf("\n");
}

static void ShowTlsSecrets(SnifferSession* session)
{
    PrintSecret("server master secret", session->sslServer->arrays->masterSecret, SECRET_LEN);
    PrintSecret("client master secret", session->sslClient->arrays->masterSecret, SECRET_LEN);
    printf("server suite = %d\n", session->sslServer->options.cipherSuite);
    printf("client suite = %d\n", session->sslClient->options.cipherSuite);
}
#endif /* SHOW_SECRETS */

typedef struct {
    int type;
    union {
    #ifndef NO_RSA
        RsaKey rsa;
    #endif
    #if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA)
        struct {
            word32 pLen; /* modulus length */
            word32 privKeySz;
            byte   privKey[WC_DH_PRIV_MAX_SZ]; /* max for TLS */
            DhKey  key;
        } dh;
    #endif
    #ifdef HAVE_ECC
        ecc_key ecc;
    #endif
    #ifdef HAVE_CURVE25519
        curve25519_key x25519;
    #endif
    #ifdef HAVE_CURVE448
        curve448_key x448;
    #endif
    } priv;

#if defined(HAVE_ECC) || defined(HAVE_CURVE25519) || defined(HAVE_CURVE448)
    union {
        /* RSA is for static RSA only */
        /* DH does not use public DhKey for Agree */
    #ifdef HAVE_ECC
        ecc_key ecc;
    #endif
    #ifdef HAVE_CURVE25519
        curve25519_key x25519;
    #endif
    #ifdef HAVE_CURVE448
        curve448_key x448;
    #endif
    } pub;
#endif
    byte initPriv:1;
    byte initPub:1;
} SnifferKey;

typedef struct SetupKeysArgs {
#ifdef WOLFSSL_ASYNC_CRYPT
    SnifferKey* key;
#else
    SnifferKey  key[1];
#endif
    DerBuffer*  keyBuf;
    int         length;
    byte        keyBufFree:1;
    byte        keyLocked:1;
} SetupKeysArgs;

static void FreeSetupKeysArgs(WOLFSSL* ssl, void* pArgs)
{
    SetupKeysArgs* args = (SetupKeysArgs*)pArgs;

    if (args == NULL) {
        return;
    }
    (void)ssl;
#ifdef WOLFSSL_ASYNC_CRYPT
    if (args->key != NULL)
#endif
    {
    #ifndef NO_RSA
        if (args->key->type == WC_PK_TYPE_RSA) {
            if (args->key->initPriv) {
                wc_FreeRsaKey(&args->key->priv.rsa);
            }
        }
    #endif
    #if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA)
        if (args->key->type == WC_PK_TYPE_DH) {
            if (args->key->initPriv) {
                wc_FreeDhKey(&args->key->priv.dh.key);
            }
        }
    #endif
    #ifdef HAVE_ECC
        if (args->key->type == WC_PK_TYPE_ECDH) {
            if (args->key->initPriv) {
                wc_ecc_free(&args->key->priv.ecc);
            }
            if (args->key->initPub) {
                wc_ecc_free(&args->key->pub.ecc);
            }
        }
    #endif
    #ifdef HAVE_CURVE25519
        if (args->key->type == WC_PK_TYPE_CURVE25519) {
            if (args->key->initPriv) {
                wc_curve25519_free(&args->key->priv.x25519);
            }
            if (args->key->initPub) {
                wc_curve25519_free(&args->key->pub.x25519);
            }
        }
    #endif
    #ifdef HAVE_CURVE448
        if (args->key->type == WC_PK_TYPE_CURVE448) {
            if (args->key->initPriv) {
                wc_curve448_free(&args->key->priv.x448);
            }
            if (args->key->initPub) {
                wc_curve448_free(&args->key->pub.x448);
            }
        }
    #endif
        args->key->type = WC_PK_TYPE_NONE;
        args->key->initPriv = 0; args->key->initPub = 0;

#ifdef WOLFSSL_ASYNC_CRYPT
        XFREE(args->key, NULL, DYNAMIC_TYPE_SNIFFER_KEY);
        args->key = NULL;
#else
        XMEMSET(args->key, 0, sizeof(args->key));
#endif
    }

    if (args->keyBuf != NULL && args->keyBufFree) {
        FreeDer(&args->keyBuf);
        args->keyBufFree = 0;
    }
}

/* Process Keys */
static int SetupKeys(const byte* input, int* sslBytes, SnifferSession* session,
    char* error, KeyShareInfo* ksInfo)
{
    word32 idx;
    int ret;
    int devId = INVALID_DEVID;
    WOLFSSL_CTX* ctx = session->context->ctx;
    WOLFSSL* ssl = session->sslServer;

#ifdef WOLFSSL_ASYNC_CRYPT
    SetupKeysArgs* args = NULL;
    WOLFSSL_ASSERT_SIZEOF_GE(ssl->async->args, *args);
#else
    SetupKeysArgs  args[1];
#endif

    Trace(SNIFFER_KEY_SETUP_STR);

    if (session->sslServer->arrays == NULL ||
        session->sslClient->arrays == NULL) {
        /* Secret's have already been established and released.
         * This can happen with secure renegotiation. */
        return 0;
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->async == NULL) {
        ssl->async = (struct WOLFSSL_ASYNC*)
                XMALLOC(sizeof(struct WOLFSSL_ASYNC), ssl->heap,
                        DYNAMIC_TYPE_ASYNC);
        if (ssl->async == NULL)
            ERROR_OUT(MEMORY_E, exit_sk);
    }
    args = (SetupKeysArgs*)ssl->async->args;

    ret = wolfSSL_AsyncPop(ssl, &ssl->options.asyncState);
    if (ret != WC_NOT_PENDING_E) {
        /* Check for error */
        if (ret < 0)
            goto exit_sk;
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->options.asyncState = TLS_ASYNC_BEGIN;
        XMEMSET(args, 0, sizeof(SetupKeysArgs));
    #ifdef WOLFSSL_ASYNC_CRYPT
        ssl->async->freeArgs = FreeSetupKeysArgs;
    #endif
    #ifdef WOLFSSL_ASYNC_CRYPT
        args->key = (SnifferKey*)XMALLOC(sizeof(SnifferKey), NULL,
            DYNAMIC_TYPE_SNIFFER_KEY);
    #endif
    }

#if defined(WOLF_CRYPTO_CB) || defined(WOLFSSL_ASYNC_CRYPT)
    devId = CryptoDeviceId;
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    if (args->key == NULL) {
        ERROR_OUT(MEMORY_E, exit_sk);
    }
#endif

    switch (ssl->options.asyncState) {
    case TLS_ASYNC_BEGIN:
    {
    #if defined(HAVE_ECC) || defined(HAVE_CURVE25519) || defined(HAVE_CURVE448)
        int useCurveId = 0;
        if (ksInfo && ksInfo->curve_id != 0) {
            useCurveId = ksInfo->curve_id;
        }
    #endif

    #if defined(WOLFSSL_STATIC_EPHEMERAL) && !defined(SINGLE_THREADED)
        if (ctx->staticKELockInit &&
            wc_LockMutex(&ctx->staticKELock) == 0) {
            args->keyLocked = 1;
        }
    #endif

    #ifndef NO_RSA
        /* Static RSA */
        if (ksInfo == NULL && ssl->buffers.key) {
            ret = wc_InitRsaKey_ex(&args->key->priv.rsa, NULL, devId);
            if (ret == 0) {
                args->key->type = WC_PK_TYPE_RSA;
                args->key->initPriv = 1;
                args->keyBuf = ssl->buffers.key;
            #ifdef WOLFSSL_ASYNC_CRYPT
                ret = wolfSSL_AsyncInit(ssl, &args->key->priv.rsa.asyncDev,
                    WC_ASYNC_FLAG_CALL_AGAIN);
            #endif
            }
            if (ret == 0) {
                idx = 0;
                ret = wc_RsaPrivateKeyDecode(args->keyBuf->buffer, &idx,
                    &args->key->priv.rsa, args->keyBuf->length);
                if (ret != 0) {
                #ifndef HAVE_ECC
                    SetError(RSA_DECODE_STR, error, session,
                        FATAL_ERROR_STATE);
                    break;
                #else
                    /* If we can do ECC, this isn't fatal. Not loading a key
                     * later will be fatal, though. */
                    SetError(RSA_DECODE_STR, error, session, 0);
                    args->keyBuf = NULL;
                #endif
                }
            }

            if (ret == 0) {
                args->length = wc_RsaEncryptSize(&args->key->priv.rsa);
                if (IsTLS(session->sslServer)) {
                    input += 2;     /* tls pre length */
                }

                if (args->length > *sslBytes) {
                    SetError(PARTIAL_INPUT_STR, error, session,
                        FATAL_ERROR_STATE);
                    ret = -1;
                }
            }

        #ifdef WC_RSA_BLINDING
            if (ret == 0) {
                ret = wc_RsaSetRNG(&args->key->priv.rsa,
                    session->sslServer->rng);
                if (ret != 0) {
                    SetError(RSA_DECRYPT_STR, error, session,
                        FATAL_ERROR_STATE);
                }
            }
        #endif

            if (ret == 0) {
                session->keySz = args->length * WOLFSSL_BIT_SIZE;
                /* length is the key size in bytes */
                session->sslServer->arrays->preMasterSz = SECRET_LEN;
            }
        }
    #endif /* !NO_RSA */

    #if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA)
        /* Static DH Key */
        if (ksInfo && ksInfo->dh_key_bits != 0 && args->keyBuf == NULL) {
        #ifdef HAVE_PUBLIC_FFDHE
            const DhParams* params;
        #endif

            /* try and load static ephemeral */
        #ifdef WOLFSSL_STATIC_EPHEMERAL
            args->keyBuf = ssl->staticKE.dhKey;
            if (args->keyBuf == NULL)
                args->keyBuf = ctx->staticKE.dhKey;
        #endif

            ret = 0;
        #ifdef WOLFSSL_SNIFFER_KEY_CALLBACK
            if (KeyCb != NULL) {
                if (args->keyBuf == NULL) {
                    ret = AllocDer(&args->keyBuf, FILE_BUFFER_SIZE,
                        PRIVATEKEY_TYPE, NULL);
                    if (ret == 0)
                        args->keyBufFree = 1;
                }
                ret = KeyCb(session, ksInfo->named_group,
                    session->srvKs.key, session->srvKs.key_len,
                    session->cliKs.key, session->cliKs.key_len,
                    args->keyBuf, KeyCbCtx, error);
                if (ret != 0) {
                    SetError(-1, error, session, FATAL_ERROR_STATE);
                }
            }
        #endif
            if (ret == 0 && args->keyBuf == NULL) {
                ret = BUFFER_E;
            }

        #ifdef HAVE_PUBLIC_FFDHE
            if (ret == 0) {
                /* get DH params */
                switch (ksInfo->named_group) {
                #ifdef HAVE_FFDHE_2048
                    case WOLFSSL_FFDHE_2048:
                        params = wc_Dh_ffdhe2048_Get();
                        args->key->priv.dh.privKeySz = 29;
                        break;
                #endif
                #ifdef HAVE_FFDHE_3072
                    case WOLFSSL_FFDHE_3072:
                        params = wc_Dh_ffdhe3072_Get();
                        args->key->priv.dh.privKeySz = 34;
                        break;
                #endif
                #ifdef HAVE_FFDHE_4096
                    case WOLFSSL_FFDHE_4096:
                        params = wc_Dh_ffdhe4096_Get();
                        args->key->priv.dh.privKeySz = 39;
                        break;
                #endif
                #ifdef HAVE_FFDHE_6144
                    case WOLFSSL_FFDHE_6144:
                        params = wc_Dh_ffdhe6144_Get();
                        args->key->priv.dh.privKeySz = 46;
                        break;
                #endif
                #ifdef HAVE_FFDHE_8192
                    case WOLFSSL_FFDHE_8192:
                        params = wc_Dh_ffdhe8192_Get();
                        args->key->priv.dh.privKeySz = 52;
                        break;
                #endif
                    default:
                        ret = BAD_FUNC_ARG;
                }
            }
        #endif

            if (ret == 0) {
                ret = wc_InitDhKey_ex(&args->key->priv.dh.key, NULL, devId);
                if (ret == 0) {
                    args->key->type = WC_PK_TYPE_DH;
                    args->key->initPriv = 1;
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wolfSSL_AsyncInit(ssl,
                        &args->key->priv.dh.key.asyncDev, WC_ASYNC_FLAG_NONE);
                #endif
                }
            }
            if (ret == 0) {
        #ifdef HAVE_PUBLIC_FFDHE
                ret = wc_DhSetKey(&args->key->priv.dh.key,
                    (byte*)params->p, params->p_len,
                    (byte*)params->g, params->g_len);
                args->key->priv.dh.pLen = params->p_len;
        #else
                ret = wc_DhSetNamedKey(&args->key->priv.dh.key,
                    ksInfo->named_group);
                if (ret == 0) {
                    args->key->priv.dh.privKeySz =
                        wc_DhGetNamedKeyMinSize(ksInfo->named_group);
                    ret = wc_DhGetNamedKeyParamSize(ksInfo->named_group,
                            &args->key->priv.dh.pLen, NULL, NULL);
                }
        #endif
            }
            if (ret == 0) {
                idx = 0;
                ret = wc_DhKeyDecode(args->keyBuf->buffer, &idx,
                    &args->key->priv.dh.key, args->keyBuf->length);
            }
            if (ret == 0) {
                ret = wc_DhExportKeyPair(&args->key->priv.dh.key,
                    args->key->priv.dh.privKey, &args->key->priv.dh.privKeySz,
                    NULL, NULL);
            }
        }
    #endif /* !NO_DH && WOLFSSL_DH_EXTRA */

    #ifdef HAVE_ECC
        /* Static ECC Key */
        if (useCurveId >= 0 && args->keyBuf == NULL
        #ifdef HAVE_CURVE25519
            && useCurveId != ECC_X25519
        #endif
        #ifdef HAVE_CURVE448
            && useCurveId != ECC_X448
        #endif
        ) {
            /* try and load static ephemeral */
        #ifdef WOLFSSL_STATIC_EPHEMERAL
            args->keyBuf = ssl->staticKE.ecKey;
            if (args->keyBuf == NULL)
                args->keyBuf = ctx->staticKE.ecKey;
        #endif

            /* try static ECC */
            if (args->keyBuf == NULL) {
                args->keyBuf = session->sslServer->buffers.key;
            }

            ret = 0;
        #ifdef WOLFSSL_SNIFFER_KEY_CALLBACK
            if (KeyCb != NULL && ksInfo) {
                if (args->keyBuf == NULL) {
                    ret = AllocDer(&args->keyBuf, FILE_BUFFER_SIZE,
                        PRIVATEKEY_TYPE, NULL);
                    if (ret == 0)
                        args->keyBufFree = 1;
                }
                ret = KeyCb(session, ksInfo->named_group,
                    session->srvKs.key, session->srvKs.key_len,
                    session->cliKs.key, session->cliKs.key_len,
                    args->keyBuf, KeyCbCtx, error);
                if (ret != 0) {
                    SetError(-1, error, session, FATAL_ERROR_STATE);
                }
            }
        #endif

            if (ret == 0 && args->keyBuf == NULL) {
                ret = BUFFER_E;
            }
            if (ret == 0) {
                ret = wc_ecc_init_ex(&args->key->priv.ecc, NULL, devId);
                if (ret == 0) {
                    args->key->type = WC_PK_TYPE_ECDH;
                    args->key->initPriv = 1;
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wolfSSL_AsyncInit(ssl, &args->key->priv.ecc.asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                }
        }
        #if defined(ECC_TIMING_RESISTANT) && (!defined(HAVE_FIPS) || \
            (!defined(HAVE_FIPS_VERSION) || (HAVE_FIPS_VERSION != 2))) && \
            !defined(HAVE_SELFTEST)
            if (ret == 0) {
                ret = wc_ecc_set_rng(&args->key->priv.ecc,
                    session->sslServer->rng);
            }
        #endif
            if (ret == 0) {
                idx = 0;
                ret = wc_EccPrivateKeyDecode(args->keyBuf->buffer, &idx,
                    &args->key->priv.ecc, args->keyBuf->length);
                if (ret != 0) {
                    SetError(ECC_DECODE_STR, error, session, FATAL_ERROR_STATE);
                }
            }

            if (ret == 0) {
                args->length = wc_ecc_size(&args->key->priv.ecc) * 2 + 1;
                /* The length should be 2 times the key size (x and y), plus 1
                * for the type byte. */
                if (!IsAtLeastTLSv1_3(session->sslServer->version)) {
                    input += 1; /* Don't include the TLS length for the key. */
                }

                if (args->length > *sslBytes) {
                    SetError(PARTIAL_INPUT_STR, error, session,
                        FATAL_ERROR_STATE);
                    ret = -1;
                }

                /* if curve not provided in key share data, then use private
                 * key curve */
                if (useCurveId == 0 && args->key->priv.ecc.dp) {
                    /* this is for the static ECC case */
                    useCurveId = args->key->priv.ecc.dp->id;
                }
            }
            if (ret == 0) {
                ret = wc_ecc_init(&args->key->pub.ecc);
                if (ret == 0)
                    args->key->initPub = 1;
            }
            if (ret == 0) {
                ret = wc_ecc_import_x963_ex(input, args->length,
                    &args->key->pub.ecc, useCurveId);
                if (ret != 0) {
                    SetError(ECC_PUB_DECODE_STR, error, session,
                        FATAL_ERROR_STATE);
                }
            }
            if (ret == 0) {
                session->keySz = ((args->length - 1) / 2) * WOLFSSL_BIT_SIZE;
                /* Length is in bytes. Subtract 1 for the ECC key type. Divide
                * by two as the key is in (x,y) coordinates, where x and y are
                * the same size, the key size. Convert from bytes to bits. */
                session->sslServer->arrays->preMasterSz = ENCRYPT_LEN;
            }
        }
    #endif /* HAVE_ECC */

    #ifdef HAVE_CURVE25519
        /* Static Curve25519 Key */
        if (useCurveId == ECC_X25519) {
            /* try and load static ephemeral */
        #ifdef WOLFSSL_STATIC_EPHEMERAL
            args->keyBuf = ssl->staticKE.x25519Key;
            if (args->keyBuf == NULL)
                args->keyBuf = ctx->staticKE.x25519Key;
        #endif

            ret = 0;
        #ifdef WOLFSSL_SNIFFER_KEY_CALLBACK
            if (KeyCb != NULL && ksInfo) {
                if (args->keyBuf == NULL) {
                    ret = AllocDer(&args->keyBuf, FILE_BUFFER_SIZE,
                        PRIVATEKEY_TYPE, NULL);
                    if (ret == 0)
                        args->keyBufFree = 1;
                }
                ret = KeyCb(session, ksInfo->named_group,
                    session->srvKs.key, session->srvKs.key_len,
                    session->cliKs.key, session->cliKs.key_len,
                    args->keyBuf, KeyCbCtx, error);
                if (ret != 0) {
                    SetError(-1, error, session, FATAL_ERROR_STATE);
                    break;
                }
            }
        #endif

            if (ret == 0 && args->keyBuf == NULL) {
                ret = BUFFER_E;
            }
            if (ret == 0) {
                ret = wc_curve25519_init_ex(&args->key->priv.x25519, NULL,
                    devId);
                if (ret == 0) {
                    args->key->type = WC_PK_TYPE_CURVE25519;
                    args->key->initPriv = 1;
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wolfSSL_AsyncInit(ssl,
                        &args->key->priv.x25519.asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                }
            }
            if (ret == 0) {
                idx = 0;
                ret = wc_Curve25519PrivateKeyDecode(args->keyBuf->buffer, &idx,
                    &args->key->priv.x25519, args->keyBuf->length);
                if (ret != 0) {
                    SetError(ECC_DECODE_STR, error, session, FATAL_ERROR_STATE);
                }
            }

            if (ret == 0) {
                args->length = CURVE25519_KEYSIZE;
                if (args->length > *sslBytes) {
                    SetError(PARTIAL_INPUT_STR, error, session,
                        FATAL_ERROR_STATE);
                    ret = -1;
                }
            }
            if (ret == 0) {
                ret = wc_curve25519_init(&args->key->pub.x25519);
                if (ret == 0)
                    args->key->initPub = 1;
            }
            if (ret == 0) {
                ret = wc_curve25519_import_public_ex(input, args->length,
                    &args->key->pub.x25519, EC25519_LITTLE_ENDIAN);
                if (ret != 0) {
                    SetError(ECC_PUB_DECODE_STR, error, session,
                        FATAL_ERROR_STATE);
                }
            }

            if (ret == 0) {
                /* For Curve25519 length is always 32 */
                session->keySz = CURVE25519_KEYSIZE;
                session->sslServer->arrays->preMasterSz = ENCRYPT_LEN;
            }
        }
    #endif /* HAVE_CURVE25519 */

    #ifdef HAVE_CURVE448
        /* Static Curve448 Key */
        if (useCurveId == ECC_X448) {
            /* try and load static ephemeral */
        #ifdef WOLFSSL_STATIC_EPHEMERAL
            args->keyBuf = ssl->staticKE.x448Key;
            if (args->keyBuf == NULL)
                args->keyBuf = ctx->staticKE.x448Key;
        #endif

            ret = 0;
        #ifdef WOLFSSL_SNIFFER_KEY_CALLBACK
            if (KeyCb != NULL && ksInfo) {
                if (args->keyBuf == NULL) {
                    ret = AllocDer(&args->keyBuf, FILE_BUFFER_SIZE,
                        PRIVATEKEY_TYPE, NULL);
                    if (ret == 0)
                        args->keyBufFree = 1;
                }
                ret = KeyCb(session, ksInfo->named_group,
                    session->srvKs.key, session->srvKs.key_len,
                    session->cliKs.key, session->cliKs.key_len,
                    args->keyBuf, KeyCbCtx, error);
                if (ret != 0) {
                    SetError(-1, error, session, FATAL_ERROR_STATE);
                    break;
                }
            }
        #endif

            if (ret == 0 && args->keyBuf == NULL) {
                ret = BUFFER_E;
            }
            if (ret == 0) {
                ret = wc_curve448_init(&args->key->priv.x448);
                if (ret == 0) {
                    args->key->type = WC_PK_TYPE_CURVE448;
                    args->key->initPriv = 1;
                #ifdef WOLFSSL_ASYNC_CRYPT
                    ret = wolfSSL_AsyncInit(ssl, &args->key->priv.x448.asyncDev,
                        WC_ASYNC_FLAG_CALL_AGAIN);
                #endif
                }
            }
            if (ret == 0) {
                idx = 0;
                ret = wc_Curve448PrivateKeyDecode(args->keyBuf->buffer, &idx,
                    &args->key->priv.x448, args->keyBuf->length);
                if (ret != 0) {
                    SetError(ECC_DECODE_STR, error, session, FATAL_ERROR_STATE);
                }
            }

            if (ret == 0) {
                args->length = CURVE448_KEY_SIZE;
                if (args->length > *sslBytes) {
                    SetError(PARTIAL_INPUT_STR, error, session,
                        FATAL_ERROR_STATE);
                    ret = -1;
                }
            }
            if (ret == 0) {
                ret = wc_curve448_init(&args->key->pub.x448);
                if (ret == 0)
                    args->key->initPub = 1;
            }
            if (ret == 0) {
                ret = wc_curve448_import_public_ex(input, args->length,
                    &args->key->pub.x448, EC448_LITTLE_ENDIAN);
                if (ret != 0) {
                    SetError(ECC_PUB_DECODE_STR, error, session,
                        FATAL_ERROR_STATE);
                }
            }

            if (ret == 0) {
                session->keySz = CURVE448_KEY_SIZE;
                session->sslServer->arrays->preMasterSz = ENCRYPT_LEN;
            }
        }
    #endif /* HAVE_CURVE448 */

    #if defined(WOLFSSL_STATIC_EPHEMERAL) && !defined(SINGLE_THREADED)
        if (args->keyLocked) {
            wc_UnLockMutex(&ctx->staticKELock);
        }
    #endif

        /* make sure a key type was found */
        if (args->key->type == WC_PK_TYPE_NONE) {
            ret = NOT_COMPILED_IN;
        }

        /* check for errors before moving to next state */
        if (ret < 0) {
            break;
        }

        /* Advance state and proceed */
        ssl->options.asyncState = TLS_ASYNC_DO;
    } /* case TLS_ASYNC_BEGIN */
    FALL_THROUGH;

    case TLS_ASYNC_DO:
    {
    #ifdef WOLFSSL_ASYNC_CRYPT
        WC_ASYNC_DEV* asyncDev = NULL;
    #endif
    #ifndef NO_RSA
        if (args->key->type == WC_PK_TYPE_RSA) {
            ret = wc_RsaPrivateDecrypt(input, args->length,
                    session->sslServer->arrays->preMasterSecret,
                    session->sslServer->arrays->preMasterSz,
                    &args->key->priv.rsa);
        #ifdef WOLFSSL_ASYNC_CRYPT
            asyncDev = &args->key->priv.rsa.asyncDev;
        #endif
        }
    #endif /* !NO_RSA */
    #if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA)
        if (args->key->type == WC_PK_TYPE_DH) {
            /* Derive secret from private key and peer's public key */
            ret = wc_DhAgree(&args->key->priv.dh.key,
                session->sslServer->arrays->preMasterSecret,
                &session->sslServer->arrays->preMasterSz,
                args->key->priv.dh.privKey, args->key->priv.dh.privKeySz,
                input, *sslBytes);
        #ifdef WOLFSSL_ASYNC_CRYPT
            asyncDev = &args->key->priv.dh.key.asyncDev;
        #endif
        }
    #endif /* !NO_DH && WOLFSSL_DH_EXTRA */
    #ifdef HAVE_ECC
        if (args->key->type == WC_PK_TYPE_ECDH) {
            ret = wc_ecc_shared_secret(&args->key->priv.ecc,
                &args->key->pub.ecc,
                session->sslServer->arrays->preMasterSecret,
                &session->sslServer->arrays->preMasterSz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            asyncDev = &args->key->priv.ecc.asyncDev;
        #endif
        }
    #endif /* HAVE_ECC */
    #ifdef HAVE_CURVE25519
        if (args->key->type == WC_PK_TYPE_CURVE25519) {
            ret = wc_curve25519_shared_secret_ex(&args->key->priv.x25519,
                &args->key->pub.x25519,
                session->sslServer->arrays->preMasterSecret,
                &session->sslServer->arrays->preMasterSz,
                EC25519_LITTLE_ENDIAN);
        #ifdef WOLFSSL_ASYNC_CRYPT
            asyncDev = &args->key->priv.x25519.asyncDev;
        #endif
        }
    #endif /* HAVE_CURVE25519 */
    #ifdef HAVE_CURVE448
        if (args->key->type == WC_PK_TYPE_CURVE448) {
            ret = wc_curve448_shared_secret_ex(&args->key->priv.x448,
                &args->key->pub.x448,
                session->sslServer->arrays->preMasterSecret,
                &session->sslServer->arrays->preMasterSz, EC448_LITTLE_ENDIAN);
        #ifdef WOLFSSL_ASYNC_CRYPT
            asyncDev = &args->key->priv.x448.asyncDev;
        #endif
        }
    #endif /* HAVE_CURVE448 */

    #ifdef WOLFSSL_ASYNC_CRYPT
        if (ret == WC_PENDING_E) {
            /* Handle async pending response */
            ret = wolfSSL_AsyncPush(ssl, asyncDev);
            break;
        }
    #endif /* WOLFSSL_ASYNC_CRYPT */

        /* check for errors before moving to next state */
        if (ret < 0) {
            break;
        }

        /* Advance state and proceed */
        ssl->options.asyncState = TLS_ASYNC_VERIFY;
    } /* case TLS_ASYNC_DO */
    FALL_THROUGH;

    case TLS_ASYNC_VERIFY:
    {
    #ifndef NO_RSA
        if (args->key->type == WC_PK_TYPE_RSA) {
            if (ret != SECRET_LEN) {
                SetError(RSA_DECRYPT_STR, error, session, FATAL_ERROR_STATE);
                ret = RSA_BUFFER_E;
            }
        }
    #endif /* !NO_RSA */
    #if !defined(NO_DH) && defined(WOLFSSL_DH_EXTRA)
        if (args->key->type == WC_PK_TYPE_DH) {
            /* left-padded with zeros up to the size of the prime */
            if (args->key->priv.dh.pLen >
                                      session->sslServer->arrays->preMasterSz) {
                word32 diff = args->key->priv.dh.pLen -
                              session->sslServer->arrays->preMasterSz;
                XMEMMOVE(session->sslServer->arrays->preMasterSecret + diff,
                        session->sslServer->arrays->preMasterSecret,
                        session->sslServer->arrays->preMasterSz);
                XMEMSET(session->sslServer->arrays->preMasterSecret, 0, diff);
                session->sslServer->arrays->preMasterSz=args->key->priv.dh.pLen;
            }
        }
    #endif /* !NO_DH && WOLFSSL_DH_EXTRA */

        /* check for errors before moving to next state */
        if (ret < 0) {
            break;
        }

        /* Advance state and proceed */
        ssl->options.asyncState = TLS_ASYNC_FINALIZE;
    } /* case TLS_ASYNC_VERIFY */
    FALL_THROUGH;

    case TLS_ASYNC_FINALIZE:
    {
        /* store for client side as well */
        XMEMCPY(session->sslClient->arrays->preMasterSecret,
            session->sslServer->arrays->preMasterSecret,
            session->sslServer->arrays->preMasterSz);
        session->sslClient->arrays->preMasterSz =
            session->sslServer->arrays->preMasterSz;

    #ifdef SHOW_SECRETS
        PrintSecret("pre master secret",
                    session->sslServer->arrays->preMasterSecret,
                    session->sslServer->arrays->preMasterSz);
    #endif

        if (SetCipherSpecs(session->sslServer) != 0) {
            SetError(BAD_CIPHER_SPEC_STR, error, session, FATAL_ERROR_STATE);
            session->verboseErr = 1;
            ret = -1; break;
        }

        if (SetCipherSpecs(session->sslClient) != 0) {
            SetError(BAD_CIPHER_SPEC_STR, error, session, FATAL_ERROR_STATE);
            session->verboseErr = 1;
            ret = -1; break;
        }

    #ifdef WOLFSSL_TLS13
        /* TLS v1.3 derive handshake key */
        if (IsAtLeastTLSv1_3(session->sslServer->version)) {
            ret  = DeriveEarlySecret(session->sslServer);
            ret += DeriveEarlySecret(session->sslClient);
            ret += DeriveHandshakeSecret(session->sslServer);
            ret += DeriveHandshakeSecret(session->sslClient);
            ret += DeriveTls13Keys(session->sslServer, handshake_key,
                ENCRYPT_AND_DECRYPT_SIDE, 1);
            ret += DeriveTls13Keys(session->sslClient, handshake_key,
                ENCRYPT_AND_DECRYPT_SIDE, 1);
        #ifdef WOLFSSL_EARLY_DATA
            ret += SetKeysSide(session->sslServer, DECRYPT_SIDE_ONLY);
            ret += SetKeysSide(session->sslClient, DECRYPT_SIDE_ONLY);
        #else
            ret += SetKeysSide(session->sslServer, ENCRYPT_AND_DECRYPT_SIDE);
            ret += SetKeysSide(session->sslClient, ENCRYPT_AND_DECRYPT_SIDE);
        #endif
        }
        else
    #endif /* WOLFSSL_TLS13 */
        {
            ret  = MakeMasterSecret(session->sslServer);
            ret += MakeMasterSecret(session->sslClient);
            ret += SetKeysSide(session->sslServer, ENCRYPT_AND_DECRYPT_SIDE);
            ret += SetKeysSide(session->sslClient, ENCRYPT_AND_DECRYPT_SIDE);
        }
        if (ret != 0) {
            SetError(BAD_DERIVE_STR, error, session, FATAL_ERROR_STATE);
            ret = -1; break;
        }

    #ifdef SHOW_SECRETS
        #ifdef WOLFSSL_TLS13
        if (!IsAtLeastTLSv1_3(session->sslServer->version))
        #endif
        {
            ShowTlsSecrets(session);
        }
    #endif

        CallConnectionCb(session);

        break;
    } /* case TLS_ASYNC_FINALIZE */

    default:
        ret = INPUT_CASE_ERROR;
    } /* switch(ssl->options.asyncState) */

#ifdef WOLFSSL_ASYNC_CRYPT
exit_sk:

    /* Handle async pending response */
    if (ret == WC_PENDING_E) {
        return ret;
    }
#endif /* WOLFSSL_ASYNC_CRYPT */

#ifdef WOLFSSL_SNIFFER_STATS
    if (ret < 0)
        INC_STAT(SnifferStats.sslKeyFails);
#endif

    /* Final cleanup */
#ifdef WOLFSSL_ASYNC_CRYPT
    FreeAsyncCtx(ssl, 1);
#else
    FreeSetupKeysArgs(ssl, args);
#endif
#ifndef WOLFSSL_STATIC_EPHEMERAL
    (void)ctx;
#endif

    return ret;
}

/* Process Client Key Exchange */
static int ProcessClientKeyExchange(const byte* input, int* sslBytes,
                            SnifferSession* session, char* error)
{
    int ret;

#ifndef WOLFSSL_STATIC_EPHEMERAL
    if (session->sslServer->buffers.key == NULL ||
        session->sslServer->buffers.key->buffer == NULL ||
        session->sslServer->buffers.key->length == 0) {

        SetError(RSA_KEY_MISSING_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
#endif

    ret = SetupKeys(input, sslBytes, session, error, NULL);

    return ret;
}

#ifdef WOLFSSL_TLS13
static int ProcessKeyShare(KeyShareInfo* info, const byte* input, int len,
    word16 filter_group)
{
    int index = 0;
    while (index < len) {
        /* clear info (reset dh_key_bits and curve_id) */
        XMEMSET(info, 0, sizeof(KeyShareInfo));

        /* Named group and public key */
        info->named_group = (word16)((input[index] << 8) | input[index+1]);
        index += OPAQUE16_LEN;
        info->key_len = 0;
        info->key = NULL;
        /* If key was provided... (a hello_retry_request will not send a key) */
        if (index + 2 <= len) {
            info->key_len = (word16)((input[index] << 8) | input[index+1]);
            index += OPAQUE16_LEN;
            if (info->key_len == 0 || info->key_len > len - index) {
                return -1;
            }
            info->key = &input[index];
            index += info->key_len;
        }

        switch (info->named_group) {
    #ifndef NO_DH
        #ifdef HAVE_FFDHE_2048
            case WOLFSSL_FFDHE_2048:
                info->dh_key_bits = 2048;
                break;
        #endif
        #ifdef HAVE_FFDHE_3072
            case WOLFSSL_FFDHE_3072:
                info->dh_key_bits = 3072;
                break;
        #endif
        #ifdef HAVE_FFDHE_4096
            case WOLFSSL_FFDHE_4096:
                info->dh_key_bits = 4096;
                break;
        #endif
        #ifdef HAVE_FFDHE_6144
            case WOLFSSL_FFDHE_6144:
                info->dh_key_bits = 6144;
                break;
        #endif
        #ifdef HAVE_FFDHE_8192
            case WOLFSSL_FFDHE_8192:
                info->dh_key_bits = 8192;
                break;
        #endif
    #endif /* !NO_DH */
    #ifdef HAVE_ECC
        #if !defined(NO_ECC256) || defined(HAVE_ALL_CURVES)
            #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP256R1:
                info->curve_id = ECC_SECP256R1;
                break;
            #endif /* !NO_ECC_SECP */
            #ifdef WOLFSSL_SM2
            case WOLFSSL_ECC_SM2P256V1:
                info->curve_id = ECC_SM2P256V1;
                break;
            #endif /* WOLFSSL_SM2 */
        #endif
        #if defined(HAVE_ECC384) || defined(HAVE_ALL_CURVES)
            #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP384R1:
                info->curve_id = ECC_SECP384R1;
                break;
            #endif /* !NO_ECC_SECP */
        #endif
        #if defined(HAVE_ECC521) || defined(HAVE_ALL_CURVES)
            #ifndef NO_ECC_SECP
            case WOLFSSL_ECC_SECP521R1:
                info->curve_id = ECC_SECP521R1;
                break;
            #endif /* !NO_ECC_SECP */
        #endif
    #endif /* HAVE_ECC */
        #ifdef HAVE_CURVE25519
            case WOLFSSL_ECC_X25519:
                info->curve_id = ECC_X25519;
                break;
        #endif
        #ifdef HAVE_CURVE448
            case WOLFSSL_ECC_X448:
                info->curve_id = ECC_X448;
                break;
        #endif
            default:
                /* do not throw error here, keep iterating the client key share */
                break;
        }

        if (filter_group == 0 || filter_group == info->named_group) {
            return 0;
        }
    }
    return NO_PEER_KEY; /* unsupported key type */
}

static int ProcessServerKeyShare(SnifferSession* session, const byte* input, int len,
    char* error)
{
    int ret;

    if (session->cliKeyShare == NULL || session->cliKeyShareSz == 0) {
        /* session->cliKeyShareSz could not be provided yet if the client_hello
            did not send a key share to force a hello_retry_request */
        return 0;
    }

    /* Get server_hello key share (and key) */
    ret = ProcessKeyShare(&session->srvKs, input, len, 0);
    if (ret == 0 && session->srvKs.key_len > 0) {
        /* Get client_hello key share */
        ret = ProcessKeyShare(&session->cliKs, session->cliKeyShare,
            session->cliKeyShareSz, session->srvKs.named_group);
    }
    if (ret != 0) {
        SetError(SERVER_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    return ret;
}
#endif /* WOLFSSL_TLS13 */

/* Process Session Ticket */
static int ProcessSessionTicket(const byte* input, int* sslBytes,
                                SnifferSession* session, char* error)
{
    word16 len;

#ifdef WOLFSSL_TLS13
    WOLFSSL* ssl;

    if (session->flags.side == WOLFSSL_SERVER_END)
        ssl = session->sslServer;
    else
        ssl = session->sslClient;
#endif

    /* make sure can read through hint len */
    if (TICKET_HINT_LEN > *sslBytes) {
        SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    input     += TICKET_HINT_LEN; /* skip over hint len */
    *sslBytes -= TICKET_HINT_LEN;

#ifdef WOLFSSL_TLS13
    /* TLS v1.3 has hint age and nonce */
    if (IsAtLeastTLSv1_3(ssl->version)) {
        /* make sure can read through hint age and nonce len */
        if (TICKET_HINT_AGE_LEN + 1 > *sslBytes) {
            SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        input     += TICKET_HINT_AGE_LEN; /* skip over hint age */
        *sslBytes -= TICKET_HINT_AGE_LEN;

        /* ticket nonce */
        len = input[0];
        if (len > MAX_TICKET_NONCE_STATIC_SZ) {
            SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        input += OPAQUE8_LEN;
        *sslBytes -= OPAQUE8_LEN;
    #ifdef HAVE_SESSION_TICKET
        /* store nonce in server for DeriveResumptionPSK */
        session->sslServer->session->ticketNonce.len = len;
        if (len > 0)
            XMEMCPY(session->sslServer->session->ticketNonce.data, input, len);
    #endif
        input += len;
        *sslBytes -= len;
    }
#endif

    /* make sure can read through len */
    if (OPAQUE16_LEN > *sslBytes) {
        SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    len = (word16)((input[0] << 8) | input[1]);
    input     += OPAQUE16_LEN;
    *sslBytes -= OPAQUE16_LEN;

    /* make sure can read through ticket */
    if (len > *sslBytes) {
        SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

#ifdef WOLFSSL_TLS13
    /* TLS v1.3 has hint age and nonce */
    if (IsAtLeastTLSv1_3(ssl->version)) {
        /* Note: Must use server session for sessions */
    #ifdef HAVE_SESSION_TICKET
        if (SetTicket(session->sslServer, input, len) != 0) {
            SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }

        /* set haveSessionId to use the wolfSession cache */
        session->sslServer->options.haveSessionId = 1;

        /* Use the wolf Session cache to retain resumption secret */
        if (session->flags.cached == 0) {
            WOLFSSL_SESSION* sess = wolfSSL_GetSession(session->sslServer,
                NULL, 0);
            if (sess == NULL) {
                SetupSession(session->sslServer);
                AddSession(session->sslServer); /* don't re add */
            #ifdef WOLFSSL_SNIFFER_STATS
                INC_STAT(SnifferStats.sslResumptionInserts);
            #endif
            }
            session->flags.cached = 1;
        }
    #endif /* HAVE_SESSION_TICKET */
    }
    else
#endif /* WOLFSSL_TLS13 */
    {
        /* capture last part of sessionID as macID (32 bytes) */
        if (len < ID_LEN) {
            SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        /* store session with macID as sessionID */
        session->sslServer->options.haveSessionId = 1;
        if (session->sslServer->arrays) {
            XMEMCPY(session->sslServer->arrays->sessionID,
                input + len - ID_LEN, ID_LEN);
            session->sslServer->arrays->sessionIDSz = ID_LEN;
        }
    }

    return 0;
}

static int DoResume(SnifferSession* session, char* error)
{
    int ret = 0;
    WOLFSSL_SESSION* resume;

#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(session->sslServer->version)) {
        resume = wolfSSL_GetSession(session->sslServer,
                                    session->sslServer->session->masterSecret, 0);
        if (resume == NULL) {
            /* TLS v1.3 with hello_retry uses session_id even for new session,
                so ignore error here */
            return 0;
        }
    }
    else
#endif
    {
    #ifdef HAVE_SESSION_TICKET
        /* make sure "useTicket" is not set, otherwise the session will not be
         * properly retrieved */
        session->sslServer->options.useTicket = 0;
    #endif
        resume = wolfSSL_GetSession(session->sslServer,
                                    session->sslServer->arrays->masterSecret, 0);
        if (resume == NULL) {
        #ifdef WOLFSSL_SNIFFER_STATS
            INC_STAT(SnifferStats.sslResumeMisses);
        #endif
            SetError(BAD_SESSION_RESUME_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
    }

    /* make sure client has master secret too */
#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(session->sslServer->version)) {
        XMEMCPY(session->sslClient->session->masterSecret,
                session->sslServer->session->masterSecret, SECRET_LEN);
    }
    else
#endif
    {
        XMEMCPY(session->sslClient->arrays->masterSecret,
                session->sslServer->arrays->masterSecret, SECRET_LEN);
    }
    session->flags.resuming = 1;

    Trace(SERVER_DID_RESUMPTION_STR);
#ifdef WOLFSSL_SNIFFER_STATS
    INC_STAT(SnifferStats.sslResumedConns);
#endif
    if (SetCipherSpecs(session->sslServer) != 0) {
        SetError(BAD_CIPHER_SPEC_STR, error, session, FATAL_ERROR_STATE);
        session->verboseErr = 1;
        return -1;
    }

    if (SetCipherSpecs(session->sslClient) != 0) {
        SetError(BAD_CIPHER_SPEC_STR, error, session, FATAL_ERROR_STATE);
        session->verboseErr = 1;
        return -1;
    }

#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(session->sslServer->version)) {
    #ifdef HAVE_SESSION_TICKET
        /* Resumption PSK is resumption master secret. */
        session->sslServer->arrays->psk_keySz = session->sslServer->specs.hash_size;
        session->sslClient->arrays->psk_keySz = session->sslClient->specs.hash_size;
        ret  = DeriveResumptionPSK(session->sslServer,
            session->sslServer->session->ticketNonce.data,
            session->sslServer->session->ticketNonce.len,
            session->sslServer->arrays->psk_key);
        /* Copy resumption PSK to client */
        XMEMCPY(session->sslClient->arrays->psk_key,
            session->sslServer->arrays->psk_key,
            session->sslServer->arrays->psk_keySz);
    #endif
        /* handshake key setup below and traffic keys done in SetupKeys */
    }
    else
#endif
    {
        if (IsTLS(session->sslServer)) {
            ret =  DeriveTlsKeys(session->sslServer);
            ret += DeriveTlsKeys(session->sslClient);
        }
        else {
#ifndef NO_OLD_TLS
            ret =  DeriveKeys(session->sslServer);
            ret += DeriveKeys(session->sslClient);
#endif
        }
        ret += SetKeysSide(session->sslServer, ENCRYPT_AND_DECRYPT_SIDE);
        ret += SetKeysSide(session->sslClient, ENCRYPT_AND_DECRYPT_SIDE);
    }

    if (ret != 0) {
        SetError(BAD_DERIVE_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    return ret;
}

/* Process Server Hello */
static int ProcessServerHello(int msgSz, const byte* input, int* sslBytes,
                              SnifferSession* session, char* error)
{
    int             ret = 0;
    ProtocolVersion pv;
    byte            b, b0;
    int             toRead = VERSION_SZ + RAN_LEN + ENUM_LEN;
    int             doResume = 0;
    const byte*     inputHello = input;
    int             initialBytes = *sslBytes;

    (void)msgSz;

    /* make sure we didn't miss ClientHello */
    if (session->flags.clientHello == 0 || session->sslClient->arrays == NULL) {
        SetError(MISSED_CLIENT_HELLO_STR, error, session, 0);
        return 0; /* do not throw error, just ignore packet */
    }

    /* make sure can read through session len */
    if (toRead > *sslBytes) {
        SetError(SERVER_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    XMEMCPY(&pv, input, VERSION_SZ);
    input     += VERSION_SZ;
    *sslBytes -= VERSION_SZ;

    session->sslServer->version = pv;
    session->sslClient->version = pv;
    if (pv.minor >= TLSv1_MINOR) {
        session->sslServer->options.tls = 1;
        session->sslClient->options.tls = 1;
    }

    XMEMCPY(session->sslServer->arrays->serverRandom, input, RAN_LEN);
    XMEMCPY(session->sslClient->arrays->serverRandom, input, RAN_LEN);
    input     += RAN_LEN;
    *sslBytes -= RAN_LEN;

    b = *input++;
    *sslBytes -= 1;

    /* make sure can read through compression */
    if ( (b + SUITE_LEN + ENUM_LEN) > *sslBytes) {
        SetError(SERVER_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    if (b) {
    #ifdef WOLFSSL_TLS13
        XMEMCPY(session->sslServer->session->sessionID, input, ID_LEN);
        session->sslServer->session->sessionIDSz = ID_LEN;
    #endif
        XMEMCPY(session->sslServer->arrays->sessionID, input, ID_LEN);
        session->sslServer->arrays->sessionIDSz = ID_LEN;
        session->sslServer->options.haveSessionId = 1;
    }
    input     += b;
    *sslBytes -= b;

    /* cipher suite */
    b0 = *input++;  /* first byte, ECC or not */
    session->sslServer->options.cipherSuite0 = b0;
    session->sslClient->options.cipherSuite0 = b0;
    b = *input++;
    session->sslServer->options.cipherSuite = b;
    session->sslClient->options.cipherSuite = b;
    *sslBytes -= SUITE_LEN;

#ifdef WOLFSSL_SNIFFER_STATS
    {
        const CipherSuiteInfo* suites = GetCipherNames();
        int suitesSz = GetCipherNamesSize();
        int match = 0;

        while (suitesSz) {
            if (b0 == suites->cipherSuite0 && b == suites->cipherSuite) {
                match = 1;
                break;
            }
            suites++;
            suitesSz--;
        }
        if (!match)
            INC_STAT(SnifferStats.sslCiphersUnsupported);
    }
#endif /* WOLFSSL_SNIFFER_STATS */

    /* compression */
    b = *input++;
    *sslBytes -= ENUM_LEN;

    if (b) {
        SetError(BAD_COMPRESSION_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    /* extensions */
    if ((initialBytes - *sslBytes) < msgSz) {
        word16 len;

        /* skip extensions until extended master secret */
        /* make sure can read len */
        if (SUITE_LEN > *sslBytes) {
            SetError(SERVER_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        len = (word16)((input[0] << 8) | input[1]);
        input     += SUITE_LEN;
        *sslBytes -= SUITE_LEN;
        /* make sure can read through all extensions */
        if (len > *sslBytes) {
            SetError(SERVER_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }

        while (len >= EXT_TYPE_SZ + LENGTH_SZ) {
            word16 extType;
            word16 extLen;

            extType    = (word16)((input[0] << 8) | input[1]);
            input     += EXT_TYPE_SZ;
            *sslBytes -= EXT_TYPE_SZ;

            extLen     = (word16)((input[0] << 8) | input[1]);
            input     += LENGTH_SZ;
            *sslBytes -= LENGTH_SZ;

            /* make sure can read through individual extension */
            if (extLen > *sslBytes) {
                SetError(SERVER_HELLO_INPUT_STR, error, session,
                         FATAL_ERROR_STATE);
                return -1;
            }
        #ifdef DEBUG_SNIFFER
            printf("\tserver_hello ext: 0x%02x (len %d)\n", extType, extLen);
        #endif

            switch (extType) {
        #ifdef WOLFSSL_TLS13
            case EXT_KEY_SHARE:
                ret = ProcessServerKeyShare(session, input, extLen, error);
                if (ret != 0) {
                    SetError(SERVER_HELLO_INPUT_STR, error, session,
                        FATAL_ERROR_STATE);
                    return -1;
                }
                break;
        #endif
        #ifdef HAVE_SESSION_TICKET
            case EXT_PRE_SHARED_KEY:
                /* indicates we want to use resumption */
                session->sslServer->options.resuming = 1;
                session->sslClient->options.resuming = 1;
            #ifdef WOLFSSL_TLS13
                /* default nonce to len = 1, data = 0 */
                session->sslServer->session->ticketNonce.len = 1;
                session->sslServer->session->ticketNonce.data[0] = 0;
                session->sslClient->session->ticketNonce.len = 1;
                session->sslClient->session->ticketNonce.data[0] = 0;
            #endif
                break;
        #endif
        #ifdef HAVE_MAX_FRAGMENT
            case EXT_MAX_FRAGMENT_LENGTH:
            {
                word16 max_fragment = MAX_RECORD_SIZE;
                switch (input[0]) {
                    case WOLFSSL_MFL_2_8 : max_fragment =  256; break;
                    case WOLFSSL_MFL_2_9 : max_fragment =  512; break;
                    case WOLFSSL_MFL_2_10: max_fragment = 1024; break;
                    case WOLFSSL_MFL_2_11: max_fragment = 2048; break;
                    case WOLFSSL_MFL_2_12: max_fragment = 4096; break;
                    case WOLFSSL_MFL_2_13: max_fragment = 8192; break;
                    default: break;
                }
                session->sslServer->max_fragment = max_fragment;
                session->sslClient->max_fragment = max_fragment;
                break;
            }
        #endif
            case EXT_SUPPORTED_VERSIONS:
                session->sslServer->version.major = input[0];
                session->sslServer->version.minor = input[1];
                session->sslClient->version.major = input[0];
                session->sslClient->version.minor = input[1];
                if (IsAtLeastTLSv1_3(session->sslServer->version)) {
                    /* The server side handshake encryption is on for future packets */
                    session->flags.serverCipherOn = 1;
                }
                break;
            case EXT_MASTER_SECRET:
            #ifdef HAVE_EXTENDED_MASTER
                session->flags.expectEms = 1;
            #endif
                break;
            case EXT_RENEGOTIATION_INFO:
                session->flags.secRenegEn = 1;
                break;
            } /* switch (extType) */

            input     += extLen;
            *sslBytes -= extLen;
            len       -= extLen + EXT_TYPE_SZ + LENGTH_SZ;
        }
    }

    if (IsAtLeastTLSv1_3(session->sslServer->version)) {
#ifndef WOLFSSL_TLS13
        SetError(UNSUPPORTED_TLS_VER_STR, error, session, FATAL_ERROR_STATE);
        session->verboseErr = 1;
        return -1;
#endif
    }
    else {
#ifdef WOLFSSL_NO_TLS12
        SetError(UNSUPPORTED_TLS_VER_STR, error, session, FATAL_ERROR_STATE);
        session->verboseErr = 1;
        return -1;
#endif
    }

#ifdef HAVE_EXTENDED_MASTER
    if (!session->flags.expectEms) {
        XFREE(session->hash, NULL, DYNAMIC_TYPE_HASHES);
        session->hash = NULL;
    }
#endif

    if (session->sslServer->options.haveSessionId) {
        if (XMEMCMP(session->sslServer->arrays->sessionID,
                session->sslClient->arrays->sessionID, ID_LEN) == 0) {
            doResume = 1;
        }
    }
    else if (session->sslClient->options.haveSessionId == 0 &&
             session->sslServer->options.haveSessionId == 0 &&
             session->ticketID) {
        doResume = 1;
    }

    if (session->ticketID && doResume) {
        /* use ticketID to retrieve from session, prefer over sessionID */
        XMEMCPY(session->sslServer->arrays->sessionID,session->ticketID,ID_LEN);
        session->sslServer->arrays->sessionIDSz = ID_LEN;
        session->sslServer->options.haveSessionId = 1;  /* may not have
                                                           actual sessionID */
    }

#ifdef WOLFSSL_TLS13
    /* Is TLS v1.3 hello_retry_request? */
    if (IsAtLeastTLSv1_3(session->sslServer->version) && session->srvKs.key_len == 0) {
        Trace(GOT_HELLO_RETRY_REQ_STR);

        /* do not compute keys yet */
        session->flags.serverCipherOn = 0;

        /* make sure the mac and digest size are set */
        SetCipherSpecs(session->sslServer);
        SetCipherSpecs(session->sslClient);

        /* reset hashes */
        RestartHandshakeHash(session->sslServer);
        RestartHandshakeHash(session->sslClient);

        doResume = 0;
    }
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    if (session->sslServer->error != WC_PENDING_E && session->pendSeq == 0)
#endif
    {
        /* hash server_hello */
        HashRaw(session->sslServer, inputHello - HANDSHAKE_HEADER_SZ,
            initialBytes + HANDSHAKE_HEADER_SZ);
        HashRaw(session->sslClient, inputHello - HANDSHAKE_HEADER_SZ,
            initialBytes + HANDSHAKE_HEADER_SZ);
    }

    if (doResume) {
        ret = DoResume(session, error);
        if (ret != 0) {
            return ret;
        }
    }
    else {
#ifdef WOLFSSL_SNIFFER_STATS
        INC_STAT(SnifferStats.sslStandardConns);
#endif
    }

#ifdef SHOW_SECRETS
    printf("cipher suite = 0x%02x\n", session->sslServer->options.cipherSuite);
    PrintSecret("server random", session->sslServer->arrays->serverRandom, RAN_LEN);
#endif

#ifdef WOLFSSL_TLS13
    /* Setup handshake keys */
    if (IsAtLeastTLSv1_3(session->sslServer->version) && session->srvKs.key_len > 0) {
        ret = SetupKeys(session->cliKs.key, &session->cliKs.key_len,
            session, error, &session->cliKs);
        if (ret != 0) {
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                return ret;
            }
        #endif
            SetError(KEY_MISMATCH_STR, error, session, FATAL_ERROR_STATE);
            session->verboseErr = 1;
            return ret;
        }

        if (session->flags.side == WOLFSSL_SERVER_END)
            session->flags.serverCipherOn = 1;
        else
            session->flags.clientCipherOn = 1;
    }
#endif

    return 0;
}

#ifdef HAVE_SNI
/* Function return value must be 0 for success */
static int LoadNamedKey(SnifferSession* session, const byte* name, word16 nameSz)
{
    int ret = 0;
    WOLFSSL* ssl = session->sslServer;
    NamedKey* namedKey;

    wc_LockMutex(&session->context->namedKeysMutex);
    namedKey = session->context->namedKeys;
    while (namedKey != NULL) {
        if (nameSz == namedKey->nameSz &&
                    XSTRNCMP((char*)name, namedKey->name, nameSz) == 0) {
        #ifdef WOLFSSL_STATIC_EPHEMERAL
            if (namedKey->isEphemeralKey) {
                /* auto detect key type with WC_PK_TYPE_NONE */
                ret = wolfSSL_set_ephemeral_key(ssl,
                    WC_PK_TYPE_NONE, (const char*)namedKey->key,
                    namedKey->keySz, WOLFSSL_FILETYPE_ASN1);
            }
            else
        #endif
            {
                ret = wolfSSL_use_PrivateKey_buffer(ssl,
                    namedKey->key, namedKey->keySz,
                    WOLFSSL_FILETYPE_ASN1);
                /* translate return code */
                ret = (ret == WOLFSSL_SUCCESS) ? 0 : -1;
            }
            if (ret == 0) {
                session->sni = namedKey->name;
            }
            break;
        }
        namedKey = namedKey->next;
    }
    wc_UnLockMutex(&session->context->namedKeysMutex);
    return ret;
}
#endif

/* Process normal Client Hello */
static int ProcessClientHello(const byte* input, int* sslBytes,
                              SnifferSession* session, char* error)
{
    int ret = 0;
    byte   bLen;
    word16 len;
    int    toRead = VERSION_SZ + RAN_LEN + ENUM_LEN;
    const byte* inputHello = input;
    int inputHelloSz = *sslBytes;
    WOLFSSL* ssl = session->sslServer;
    int didHash = 0;

    session->flags.clientHello = 1;  /* don't process again */

    /* make sure can read up to session len */
    if (toRead > *sslBytes) {
        SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    /* skip, get negotiated one from server hello */
    input     += VERSION_SZ;
    *sslBytes -= VERSION_SZ;

    /* for secure renegotiation server arrays can be NULL */
    if (session->sslServer->arrays)
        XMEMCPY(session->sslServer->arrays->clientRandom, input, RAN_LEN);
    if (session->sslClient->arrays)
        XMEMCPY(session->sslClient->arrays->clientRandom, input, RAN_LEN);

    input     += RAN_LEN;
    *sslBytes -= RAN_LEN;

    /* store session in case trying to resume */
    bLen = *input++;
    *sslBytes -= ENUM_LEN;
    if (bLen) {
        if (ID_LEN > *sslBytes) {
            SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        Trace(CLIENT_RESUME_TRY_STR);
#ifdef WOLFSSL_TLS13
        XMEMCPY(session->sslClient->session->sessionID, input, ID_LEN);
        session->sslClient->session->sessionIDSz = ID_LEN;
#endif
        if (session->sslClient->arrays) {
            XMEMCPY(session->sslClient->arrays->sessionID, input, ID_LEN);
            session->sslClient->arrays->sessionIDSz = ID_LEN;
        }
        session->sslClient->options.haveSessionId = 1;
    }

#ifdef SHOW_SECRETS
    if (ssl->arrays)
        PrintSecret("client random", ssl->arrays->clientRandom, RAN_LEN);
#endif

    input     += bLen;
    *sslBytes -= bLen;

    /* skip cipher suites */
    /* make sure can read len */
    if (SUITE_LEN > *sslBytes) {
        SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    len = (word16)((input[0] << 8) | input[1]);
    input     += SUITE_LEN;
    *sslBytes -= SUITE_LEN;
    /* make sure can read suites + comp len */
    if (len + ENUM_LEN > *sslBytes) {
        SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    input     += len;
    *sslBytes -= len;

    /* skip compression */
    bLen       = *input++;
    *sslBytes -= ENUM_LEN;
    /* make sure can read len */
    if (bLen > *sslBytes) {
        SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    input     += bLen;
    *sslBytes -= bLen;

    if (*sslBytes == 0) {
        /* no extensions */
        return 0;
    }

    /* skip extensions until session ticket */
    /* make sure can read len */
    if (SUITE_LEN > *sslBytes) {
        SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    len = (word16)((input[0] << 8) | input[1]);
    input     += SUITE_LEN;
    *sslBytes -= SUITE_LEN;
    /* make sure can read through all extensions */
    if (len > *sslBytes) {
        SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    while (len >= EXT_TYPE_SZ + LENGTH_SZ) {
        word16 extType;
        word16 extLen;

        extType    = (word16)((input[0] << 8) | input[1]);
        input     += EXT_TYPE_SZ;
        *sslBytes -= EXT_TYPE_SZ;

        extLen     = (word16)((input[0] << 8) | input[1]);
        input     += LENGTH_SZ;
        *sslBytes -= LENGTH_SZ;

        /* make sure can read through individual extension */
        if (extLen > *sslBytes) {
            SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }

    #ifdef DEBUG_SNIFFER
        printf("\tclient_hello ext: 0x%02x (len %d)\n", extType, extLen);
    #endif

        switch (extType) {
    #ifdef HAVE_SNI
        case EXT_SERVER_NAME:
        {
            word16 listLen = 0, offset = 0;

            ato16(input + offset, &listLen);
            offset += OPAQUE16_LEN;

            if (extLen < offset + listLen)
                return BUFFER_ERROR;

            while (listLen > ENUM_LEN + OPAQUE16_LEN) {
                byte   sniType = input[offset++];
                word16 sniLen;

                ato16(input + offset, &sniLen);
                offset += OPAQUE16_LEN;

                if (extLen < offset + sniLen)
                    return BUFFER_ERROR;

                if (sniType == WOLFSSL_SNI_HOST_NAME) {
                    ret = LoadNamedKey(session, input + offset, sniLen);
                    if (ret < 0) {
                        /* don't treat this as fatal error */
                        SetError(CLIENT_HELLO_LATE_KEY_STR, error, session, 0);
                        break;
                    }
                }
                offset  += sniLen;
                listLen -= min(ENUM_LEN + OPAQUE16_LEN + sniLen, listLen);
            }
            break;
        }
    #endif
    #ifdef WOLFSSL_TLS13
        case EXT_KEY_SHARE:
        {
            word16 ksLen = (word16)((input[0] << 8) | input[1]);
            if (ksLen + OPAQUE16_LEN > extLen) {
                SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
                return -1;
            }
            /* cache key share data till server_hello */
            session->cliKeyShareSz = ksLen;
            if (ksLen > 0) {
                session->cliKeyShare = (byte*)XMALLOC(ksLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                if (session->cliKeyShare == NULL) {
                    SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
                    break;
                }
                XMEMCPY(session->cliKeyShare, &input[2], ksLen);
            }
            break;
        }
        #ifdef HAVE_SESSION_TICKET
        case EXT_PRE_SHARED_KEY:
        {
            word16 idsLen, idLen, bindersLen, idx = 0;
            word32 ticketAge;
            const byte *identity, *binders;

            idsLen = (word16)((input[idx] << 8) | input[idx+1]);
            if (idsLen + OPAQUE16_LEN + idx > extLen) {
                SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
                return -1;
            }
            idx += OPAQUE16_LEN;

            /* PSK identity */
            idLen = (word16)((input[idx] << 8) | input[idx+1]);
            if (idLen + OPAQUE16_LEN + idx > extLen) {
                SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
                return -1;
            }
            idx += OPAQUE16_LEN;
            identity = &input[idx];
            idx += idLen;

            /* Obfuscated Ticket Age 32-bits */
            ticketAge = (word32)((input[idx] << 24) | (input[idx+1] << 16) |
                                 (input[idx+2] << 8) | input[idx+3]);
            (void)ticketAge; /* not used */
            idx += OPAQUE32_LEN;

            /* binders - all binders */
            bindersLen = (word16)((input[idx] << 8) | input[idx+1]);
            if (bindersLen + OPAQUE16_LEN + idx > extLen) {
                SetError(CLIENT_HELLO_INPUT_STR, error, session, FATAL_ERROR_STATE);
                return -1;
            }
            idx += OPAQUE16_LEN;
            binders = &input[idx];
            bindersLen += OPAQUE16_LEN; /* includes 2 bytes for total len */
            (void)binders; /* not used */

            /* Hash data up to binders for deriving binders in PSK extension. */
            HashRaw(session->sslServer, inputHello - HANDSHAKE_HEADER_SZ,
                inputHelloSz - bindersLen + HANDSHAKE_HEADER_SZ);
            HashRaw(session->sslClient, inputHello - HANDSHAKE_HEADER_SZ,
                inputHelloSz - bindersLen + HANDSHAKE_HEADER_SZ);

            /* call to decrypt session ticket */
            if (DoClientTicket(ssl, identity, idLen) != WOLFSSL_TICKET_RET_OK) {
                /* we aren't decrypting the resumption, since we know the master secret */
                /* ignore errors */
            }
            ssl->options.resuming  = 1;

            /* Hash the rest of the ClientHello. */
            HashRaw(session->sslServer, inputHello + inputHelloSz - bindersLen, bindersLen);
            HashRaw(session->sslClient, inputHello + inputHelloSz - bindersLen, bindersLen);
            didHash = 1;
            break;
        }
        #endif /* HAVE_SESSION_TICKET */
    #endif /* WOLFSSL_TLS13 */
        case EXT_SUPPORTED_VERSIONS:
            break;
        case EXT_TICKET_ID:
            /* make sure can read through ticket if there is a non blank one */
            if (extLen && extLen < ID_LEN) {
                SetError(CLIENT_HELLO_INPUT_STR, error, session,
                         FATAL_ERROR_STATE);
                return -1;
            }
            if (extLen) {
                if (session->ticketID == NULL) {
                    session->ticketID = (byte*)XMALLOC(ID_LEN,
                            NULL, DYNAMIC_TYPE_SNIFFER_TICKET_ID);
                    if (session->ticketID == 0) {
                        SetError(MEMORY_STR, error, session,
                                 FATAL_ERROR_STATE);
                        return -1;
                    }
                }

            #ifdef HAVE_SESSION_TICKET
                /* do not set "ssl->options.useTicket", since the sniffer uses
                 * the cache differently for retaining the master secret only */
            #endif
                XMEMCPY(session->ticketID, input + extLen - ID_LEN, ID_LEN);
            }
            break;
        }

        input     += extLen;
        *sslBytes -= extLen;
        len       -= extLen + EXT_TYPE_SZ + LENGTH_SZ;
    }

    if (!didHash) {
        HashRaw(session->sslServer, inputHello - HANDSHAKE_HEADER_SZ,
            inputHelloSz + HANDSHAKE_HEADER_SZ);
        HashRaw(session->sslClient, inputHello - HANDSHAKE_HEADER_SZ,
            inputHelloSz + HANDSHAKE_HEADER_SZ);
    }

    (void)ssl;

    return ret;
}


#ifdef WOLFSSL_SNIFFER_WATCH

static int KeyWatchCall(SnifferSession* session, const byte* data, int dataSz,
    char* error)
{
    int ret;
    Sha256 sha;
    byte digest[SHA256_DIGEST_SIZE];

    if (WatchCb == NULL) {
        SetError(WATCH_CB_MISSING_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    ret = wc_InitSha256(&sha);
    if (ret == 0)
        ret = wc_Sha256Update(&sha, data, dataSz);
    if (ret == 0)
        ret = wc_Sha256Final(&sha, digest);
    if (ret != 0) {
        SetError(WATCH_HASH_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    ret = WatchCb((void*)session, digest, sizeof(digest),
            data, dataSz, WatchCbCtx, error);
    if (ret != 0) {
#ifdef WOLFSSL_SNIFFER_STATS
        INC_STAT(SnifferStats.sslKeysUnmatched);
#endif
        SetError(WATCH_FAIL_STR, error, session, FATAL_ERROR_STATE);
        ret = -1;
    }
    else {
#ifdef WOLFSSL_SNIFFER_STATS
        INC_STAT(SnifferStats.sslKeyMatches);
#endif
    }
    return ret;
}

/* Process Certificate */
static int ProcessCertificate(const byte* input, int* sslBytes,
        SnifferSession* session, char* error)
{
    word32 certChainSz;
    word32 certSz;

    /* If the receiver is the server, this is the client certificate message,
     * and it should be ignored at this point. */
    if (session->flags.side == WOLFSSL_SERVER_END)
        return 0;

    if (*sslBytes < CERT_HEADER_SZ) {
        SetError(BAD_CERT_MSG_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(session->sslServer->version)) {
        /* skip 1 byte (Request context len) */
        input += OPAQUE8_LEN;
        *sslBytes -= OPAQUE8_LEN;
    }
#endif

    ato24(input, &certChainSz);
    *sslBytes -= CERT_HEADER_SZ;
    input += CERT_HEADER_SZ;

    if (*sslBytes < (int)certChainSz) {
        SetError(BAD_CERT_MSG_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    ato24(input, &certSz);
    input += OPAQUE24_LEN;
    if (*sslBytes < (int)certSz) {
        SetError(BAD_CERT_MSG_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    *sslBytes -= certChainSz;

    return KeyWatchCall(session, input, certSz, error);
}

#endif /* WOLFSSL_SNIFFER_WATCH */


/* Process Finished */
static int ProcessFinished(const byte* input, int size, int* sslBytes,
                           SnifferSession* session, char* error)
{
    WOLFSSL* ssl;
    word32 inOutIdx = 0;
    int    ret;

    if (session->flags.side == WOLFSSL_SERVER_END)
        ssl = session->sslServer;
    else
        ssl = session->sslClient;

#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(ssl->version)) {
        ret = DoTls13Finished(ssl, input, &inOutIdx, (word32)size,
            (word32)*sslBytes, SNIFF);

        ssl->options.handShakeState = HANDSHAKE_DONE;
        ssl->options.handShakeDone  = 1;
    }
    else
#endif
    {
        ret = DoFinished(ssl, input, &inOutIdx, (word32)size,
            (word32)*sslBytes, SNIFF);
    }
    *sslBytes -= (int)inOutIdx;

    if (ret < 0) {
        SetError(BAD_FINISHED_MSG, error, session, FATAL_ERROR_STATE);
        return ret;
    }

    if (ret == 0 && session->flags.cached == 0) {
        if (session->sslServer->options.haveSessionId) {
        #ifndef NO_SESSION_CACHE
            WOLFSSL_SESSION* sess = wolfSSL_GetSession(session->sslServer, NULL, 0);
            if (sess == NULL) {
                SetupSession(session->sslServer);
                AddSession(session->sslServer); /* don't re add */
            #ifdef WOLFSSL_SNIFFER_STATS
                INC_STAT(SnifferStats.sslResumptionInserts);
            #endif
            }
            session->flags.cached = 1;
        #endif
         }
    }

#ifdef WOLFSSL_TLS13
    /* Derive TLS v1.3 traffic keys */
    if (IsAtLeastTLSv1_3(ssl->version)) {
        if (!session->flags.gotFinished) {
            /* When either side gets "finished" derive master secret and keys */
            ret  = DeriveMasterSecret(session->sslServer);
            ret += DeriveMasterSecret(session->sslClient);
        #ifdef WOLFSSL_EARLY_DATA
            ret += DeriveTls13Keys(session->sslServer, traffic_key, ENCRYPT_AND_DECRYPT_SIDE, ssl->earlyData == no_early_data);
            ret += DeriveTls13Keys(session->sslClient, traffic_key, ENCRYPT_AND_DECRYPT_SIDE, ssl->earlyData == no_early_data);
        #else
            ret += DeriveTls13Keys(session->sslServer, traffic_key, ENCRYPT_AND_DECRYPT_SIDE, 1);
            ret += DeriveTls13Keys(session->sslClient, traffic_key, ENCRYPT_AND_DECRYPT_SIDE, 1);
        #endif

            if (ret != 0) {
                SetError(BAD_FINISHED_MSG, error, session, FATAL_ERROR_STATE);
                return -1;
            }

            session->flags.gotFinished = 1;
        #ifdef SHOW_SECRETS
            ShowTlsSecrets(session);
        #endif
        }

        if (session->flags.side == WOLFSSL_SERVER_END) {
            /* finished from client to server */
            ret  = SetKeysSide(session->sslServer, DECRYPT_SIDE_ONLY);
            ret += SetKeysSide(session->sslClient, ENCRYPT_SIDE_ONLY);

        #ifdef HAVE_SESSION_TICKET
            /* derive resumption secret for next session - on finished (from client) */
            ret += DeriveResumptionSecret(session->sslClient,
                session->sslClient->session->masterSecret);

            /* copy resumption secret to server */
            XMEMCPY(session->sslServer->session->masterSecret,
                    session->sslClient->session->masterSecret, SECRET_LEN);
            #ifdef SHOW_SECRETS
            PrintSecret("resumption secret",
                session->sslClient->session->masterSecret, SECRET_LEN);
            #endif
        #endif
        }
        else {
            /* finished from server to client */
            ret  = SetKeysSide(session->sslServer, ENCRYPT_SIDE_ONLY);
            ret += SetKeysSide(session->sslClient, DECRYPT_SIDE_ONLY);
        }

        if (ret != 0) {
            SetError(BAD_FINISHED_MSG, error, session, FATAL_ERROR_STATE);
            return -1;
        }
    }
#endif

    /* Do not free handshake resources yet if secure renegotiation */
    if (session->flags.secRenegEn == 0) {
        /* If receiving a finished message from one side, free the resources
         * from the other side's tracker. */
        if (session->flags.side == WOLFSSL_SERVER_END)
            FreeHandshakeResources(session->sslClient);
        else
            FreeHandshakeResources(session->sslServer);
    }

    return ret;
}


/* Process HandShake input */
static int DoHandShake(const byte* input, int* sslBytes,
                       SnifferSession* session, char* error, word16 rhSize)
{
    byte type;
    int  size;
    int  ret = 0;
    WOLFSSL* ssl;
    int startBytes;

    (void)rhSize;

#ifdef HAVE_MAX_FRAGMENT
    if (session->tlsFragBuf) {
        XMEMCPY(session->tlsFragBuf + session->tlsFragOffset, input, rhSize);
        session->tlsFragOffset += rhSize;
        *sslBytes -= rhSize;

        if (session->tlsFragOffset < session->tlsFragSize) {
            return 0;
        }

        /* reassembled complete fragment */
        input = session->tlsFragBuf;
        *sslBytes = session->tlsFragSize;
        rhSize = session->tlsFragSize;
    }
#endif

    if (*sslBytes < HANDSHAKE_HEADER_SZ) {
        SetError(HANDSHAKE_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    type = input[0];
    size = (input[1] << 16) | (input[2] << 8) | input[3];

    input     += HANDSHAKE_HEADER_SZ;
    *sslBytes -= HANDSHAKE_HEADER_SZ;
    startBytes = *sslBytes;

    if (*sslBytes < size) {
        Trace(SPLIT_HANDSHAKE_MSG_STR);
        *sslBytes = 0;
        return ret;
    }

    if (session->flags.side == WOLFSSL_SERVER_END)
        ssl = session->sslServer;
    else
        ssl = session->sslClient;

#ifdef HAVE_MAX_FRAGMENT
    if (rhSize < size) {
        /* partial fragment, let's reassemble */
        if (session->tlsFragBuf == NULL) {
            session->tlsFragOffset = 0;
            session->tlsFragSize = size + HANDSHAKE_HEADER_SZ;
            session->tlsFragBuf = (byte*)XMALLOC(session->tlsFragSize, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (session->tlsFragBuf == NULL) {
                SetError(MEMORY_STR, error, NULL, 0);
                return 0;
            }

            /* include the handshake header */
            input -= HANDSHAKE_HEADER_SZ;
            *sslBytes += HANDSHAKE_HEADER_SZ;
        }

        XMEMCPY(session->tlsFragBuf + session->tlsFragOffset, input, rhSize);
        session->tlsFragOffset += rhSize;
        *sslBytes -= rhSize;
        return 0;
    }
#endif

#ifdef WOLFSSL_TLS13
    if (type != client_hello && type != server_hello
    #ifdef WOLFSSL_ASYNC_CRYPT
        && session->sslServer->error != WC_PENDING_E && session->pendSeq == 0
    #endif
    ) {
        /* For resumption the hash is before / after client_hello PSK binder */
        /* hash the packet including header */
        /* TLS v1.3 requires the hash for the handshake and transfer key derivation */
        /* we hash even for non TLS v1.3, since we don't know if its actually
            TLS v1.3 till later at EXT_SUPPORTED_VERSIONS in server_hello */
        /* hello retry request restarts hash prior to server_hello hash calc */
        HashRaw(session->sslServer, input - HANDSHAKE_HEADER_SZ, size + HANDSHAKE_HEADER_SZ);
        HashRaw(session->sslClient, input - HANDSHAKE_HEADER_SZ, size + HANDSHAKE_HEADER_SZ);
    }
#endif
#ifdef HAVE_EXTENDED_MASTER
    if (session->hash) {
        if (HashUpdate(session->hash, input, size) != 0) {
            SetError(EXTENDED_MASTER_HASH_STR, error,
                     session, FATAL_ERROR_STATE);
            ret = -1;
            goto exit;
        }
    }
#endif

    switch (type) {
        case hello_verify_request:
            Trace(GOT_HELLO_VERIFY_STR);
            break;
        case hello_request:
            Trace(GOT_HELLO_REQUEST_STR);
            break;
        case session_ticket:
            Trace(GOT_SESSION_TICKET_STR);
            ret = ProcessSessionTicket(input, sslBytes, session, error);
            break;
        case server_hello:
            Trace(GOT_SERVER_HELLO_STR);
            ret = ProcessServerHello(size, input, sslBytes, session, error);
            break;
        case certificate_request:
            Trace(GOT_CERT_REQ_STR);
            break;
        case server_key_exchange:
#ifdef WOLFSSL_SNIFFER_STATS
            INC_STAT(SnifferStats.sslEphemeralMisses);
#endif
            Trace(GOT_SERVER_KEY_EX_STR);
            /* can't know temp key passively */
            SetError(BAD_CIPHER_SPEC_STR, error, session, FATAL_ERROR_STATE);
            session->verboseErr = 1;
            ret = -1;
            break;
        case encrypted_extensions:
            Trace(GOT_ENC_EXT_STR);
            ssl->msgsReceived.got_encrypted_extensions = 1;
            break;
        case certificate:
            Trace(GOT_CERT_STR);
            if (session->flags.side == WOLFSSL_SERVER_END) {
#ifdef WOLFSSL_SNIFFER_STATS
                INC_STAT(SnifferStats.sslClientAuthConns);
#endif
            }
#ifdef WOLFSSL_SNIFFER_WATCH
            ret = ProcessCertificate(input, sslBytes, session, error);
#endif
            break;
        case server_hello_done:
            Trace(GOT_SERVER_HELLO_DONE_STR);
            break;
        case finished:
            Trace(GOT_FINISHED_STR);
            ret = ProcessFinished(input, size, sslBytes, session, error);
            break;
        case client_hello:
            Trace(GOT_CLIENT_HELLO_STR);
            ret = ProcessClientHello(input, sslBytes, session, error);
            break;
        case client_key_exchange:
            Trace(GOT_CLIENT_KEY_EX_STR);
#ifdef HAVE_EXTENDED_MASTER
            if (session->flags.expectEms) {
                /* on async reentry the session->hash is already copied
                 * and free'd */
                if (session->hash != NULL) {
                    if (HashCopy(session->sslServer->hsHashes,
                                session->hash) == 0 &&
                        HashCopy(session->sslClient->hsHashes,
                                session->hash) == 0) {

                        session->sslServer->options.haveEMS = 1;
                        session->sslClient->options.haveEMS = 1;
                    }
                    else {
                        SetError(EXTENDED_MASTER_HASH_STR, error,
                                session, FATAL_ERROR_STATE);
                        ret = -1;
                    }
                    XMEMSET(session->hash, 0, sizeof(HsHashes));
                    XFREE(session->hash, NULL, DYNAMIC_TYPE_HASHES);
                    session->hash = NULL;
                }
            }
            else {
                session->sslServer->options.haveEMS = 0;
                session->sslClient->options.haveEMS = 0;
            }
#endif
            if (ret == 0) {
                ret = ProcessClientKeyExchange(input, sslBytes, session, error);
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E)
                    return ret;
            #endif
                if (ret != 0) {
                    SetError(KEY_MISMATCH_STR, error, session, FATAL_ERROR_STATE);
                    session->verboseErr = 1;
                }
            }
            break;
        case certificate_verify:
            Trace(GOT_CERT_VER_STR);
            break;
        case certificate_status:
            Trace(GOT_CERT_STATUS_STR);
            break;
        default:
            SetError(GOT_UNKNOWN_HANDSHAKE_STR, error, session, 0);
            ret = -1;
            break;
    }

#ifdef HAVE_EXTENDED_MASTER
exit:
#endif
#ifdef HAVE_MAX_FRAGMENT
    if (session->tlsFragBuf) {
        XFREE(session->tlsFragBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        session->tlsFragBuf = NULL;
    }
#endif

    *sslBytes = startBytes - size;  /* actual bytes of full process */

    return ret;
}

/* For ciphers that use AEAD use the encrypt routine to
 * bypass the auth tag checking */
static int DecryptDo(WOLFSSL* ssl, byte* plain, const byte* input,
                           word16 sz)
{
    int ret = 0;

    (void)plain;
    (void)input;
    (void)sz;

    switch (ssl->specs.bulk_cipher_algorithm)
    {
    #ifndef NO_RC4
        case wolfssl_rc4:
            wc_Arc4Process(ssl->decrypt.arc4, plain, input, sz);
            break;
    #endif

    #ifndef NO_DES3
        case wolfssl_triple_des:
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* initialize event */
            ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.des3->asyncDev,
                WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret != 0)
                break;
        #endif

            ret = wc_Des3_CbcDecrypt(ssl->decrypt.des3, plain, input, sz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                ret = wolfSSL_AsyncPush(ssl, &ssl->decrypt.des3->asyncDev);
            }
        #endif
            break;
    #endif

    #if !defined(NO_AES) && defined(HAVE_AES_CBC)
        case wolfssl_aes:
        #ifdef WOLFSSL_ASYNC_CRYPT
            /* initialize event */
            ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.aes->asyncDev,
                WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret != 0)
                break;
        #endif
            ret = wc_AesCbcDecrypt(ssl->decrypt.aes, plain, input, sz);
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (ret == WC_PENDING_E) {
                ret = wolfSSL_AsyncPush(ssl, &ssl->decrypt.aes->asyncDev);
            }
        #endif
            break;
    #endif

    #if defined(HAVE_AESGCM) || defined(HAVE_AESCCM)
        case wolfssl_aes_gcm:
        case wolfssl_aes_ccm: /* GCM AEAD macros use same size as CCM */
        {
            wc_AesAuthEncryptFunc aes_auth_fn;

        #ifdef WOLFSSL_ASYNC_CRYPT
            /* initialize event */
            ret = wolfSSL_AsyncInit(ssl, &ssl->decrypt.aes->asyncDev,
                WC_ASYNC_FLAG_CALL_AGAIN);
            if (ret != 0)
                break;
        #endif

        #if defined(HAVE_AESGCM) && defined(HAVE_AESCCM)
            aes_auth_fn = (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm)
                            ? wc_AesGcmEncrypt : wc_AesCcmEncrypt;
        #elif defined(HAVE_AESGCM)
            aes_auth_fn = wc_AesGcmEncrypt;
        #else
            aes_auth_fn = wc_AesCcmEncrypt;
        #endif

            XMEMSET(ssl->decrypt.additional, 0, AEAD_AUTH_DATA_SZ);

            XMEMCPY(ssl->decrypt.nonce, ssl->keys.aead_dec_imp_IV, AESGCM_IMP_IV_SZ);
            XMEMCPY(ssl->decrypt.nonce + AESGCM_IMP_IV_SZ, input, AESGCM_EXP_IV_SZ);

            if ((ret = aes_auth_fn(ssl->decrypt.aes,
                        plain,
                        input + AESGCM_EXP_IV_SZ,
                          sz - AESGCM_EXP_IV_SZ - ssl->specs.aead_mac_size,
                        ssl->decrypt.nonce, AESGCM_NONCE_SZ,
                        ssl->decrypt.additional, ssl->specs.aead_mac_size,
                        NULL, 0)) < 0) {
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E) {
                    ret = wolfSSL_AsyncPush(ssl, &ssl->decrypt.aes->asyncDev);
                }
            #endif
            }
        }
        break;
    #endif /* HAVE_AESGCM || HAVE_AESCCM */

    #ifdef HAVE_ARIA
        case wolfssl_aria_gcm:
            ret = wc_AriaDecrypt(ssl->decrypt.aria,
                        plain,
                        (byte *)input + AESGCM_EXP_IV_SZ,
                          sz - AESGCM_EXP_IV_SZ - ssl->specs.aead_mac_size,
                        ssl->decrypt.nonce, AESGCM_NONCE_SZ,
                        ssl->decrypt.additional, ssl->specs.aead_mac_size,
                        NULL, 0);
            break;
    #endif

    #ifdef HAVE_CAMELLIA
        case wolfssl_camellia:
            ret = wc_CamelliaCbcDecrypt(ssl->decrypt.cam, plain, input, sz);
            break;
    #endif

    #if defined(HAVE_CHACHA) && defined(HAVE_POLY1305) && \
        !defined(NO_CHAPOL_AEAD)
        case wolfssl_chacha:
            ret = ChachaAEADEncrypt(ssl, plain, input, sz);
            break;
    #endif

    #ifdef HAVE_NULL_CIPHER
        case wolfssl_cipher_null:
            if (input != plain) {
                XMEMMOVE(plain, input, sz);
            }
            break;
    #endif

        default:
            WOLFSSL_MSG("wolfSSL Decrypt programming error");
            ret = DECRYPT_ERROR;
    }

    return ret;
}

static int DecryptTls(WOLFSSL* ssl, byte* plain, const byte* input,
                           word16 sz)
{
    int ret = 0;

#ifdef WOLFSSL_ASYNC_CRYPT
    if (ssl->decrypt.state != CIPHER_STATE_BEGIN) {
        ret = wolfSSL_AsyncPop(ssl, &ssl->decrypt.state);
        if (ret != WC_NOT_PENDING_E) {
            /* check for still pending */
            if (ret == WC_PENDING_E)
                return ret;

            ssl->error = 0; /* clear async */

            /* let failures through so CIPHER_STATE_END logic is run */
        }
    }
    else
#endif
    {
        /* Reset state */
        ret = 0;
        ssl->decrypt.state = CIPHER_STATE_BEGIN;
    }

    switch (ssl->decrypt.state) {
        case CIPHER_STATE_BEGIN:
        {
            if (ssl->decrypt.setup == 0) {
                WOLFSSL_MSG("Decrypt ciphers not setup");
                return DECRYPT_ERROR;
            }

        #if defined(HAVE_AESGCM) || defined(HAVE_AESCCM)
            /* make sure AES GCM/CCM memory is allocated */
            /* free for these happens in FreeCiphers */
            if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm ||
                ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm) {
                /* make sure auth iv and auth are allocated */
                if (ssl->decrypt.additional == NULL)
                    ssl->decrypt.additional = (byte*)XMALLOC(AEAD_AUTH_DATA_SZ,
                                            ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
                if (ssl->decrypt.nonce == NULL)
                    ssl->decrypt.nonce = (byte*)XMALLOC(AESGCM_NONCE_SZ,
                                            ssl->heap, DYNAMIC_TYPE_AES_BUFFER);
                if (ssl->decrypt.additional == NULL ||
                         ssl->decrypt.nonce == NULL) {
                    return MEMORY_E;
                }
            }
        #endif /* HAVE_AESGCM || HAVE_AESCCM */

            /* Advance state and proceed */
            ssl->decrypt.state = CIPHER_STATE_DO;
        }
        FALL_THROUGH;
        case CIPHER_STATE_DO:
        {
            ret = DecryptDo(ssl, plain, input, sz);

            /* Advance state */
            ssl->decrypt.state = CIPHER_STATE_END;

        #ifdef WOLFSSL_ASYNC_CRYPT
            /* If pending, return now */
            if (ret == WC_PENDING_E) {
                return ret;
            }
        #endif
        }
        FALL_THROUGH;
        case CIPHER_STATE_END:
        {
        #if defined(HAVE_AESGCM) || defined(HAVE_AESCCM)
            /* make sure AES GCM/CCM nonce is cleared */
            if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes_ccm ||
                ssl->specs.bulk_cipher_algorithm == wolfssl_aes_gcm) {
                if (ssl->decrypt.nonce)
                    ForceZero(ssl->decrypt.nonce, AESGCM_NONCE_SZ);

                if (ret < 0)
                    ret = VERIFY_MAC_ERROR;
            }
        #endif /* HAVE_AESGCM || HAVE_AESCCM */
            break;
        }

        default:
            break;
    }

    /* Reset state */
    ssl->decrypt.state = CIPHER_STATE_BEGIN;

    return ret;
}


/* Decrypt input message into output, adjust output steam if needed */
static const byte* DecryptMessage(WOLFSSL* ssl, const byte* input, word32 sz,
                byte* output, int* error, int* advance, RecordLayerHeader* rh)
{
    int ivExtra = 0;
    int ret;

#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(ssl->version)) {
        ret = DecryptTls13(ssl, output, input, sz, (byte*)rh, RECORD_HEADER_SZ);
    }
    else
#endif
    {
        XMEMCPY(&ssl->curRL, rh, RECORD_HEADER_SZ);
        ret = DecryptTls(ssl, output, input, sz);
    }
#ifdef WOLFSSL_ASYNC_CRYPT
    /* for async the symmetric operations are blocking */
    if (ret == WC_PENDING_E) {
        do {
            ret = wolfSSL_AsyncPoll(ssl, WOLF_POLL_FLAG_CHECK_HW);
        } while (ret == 0);
        if (ret >= 0) {
            /* remove from event queue list */
            ret = wolfSSL_AsyncPop(ssl, NULL);
        }
    }
#endif
    if (ret != 0) {
        *error = ret;
        return NULL;
    }

    ssl->keys.encryptSz = sz;
    if (ssl->options.tls1_1 && ssl->specs.cipher_type == block) {
        output += ssl->specs.block_size; /* go past TLSv1.1 IV */
        ivExtra = ssl->specs.block_size;
        *advance = ssl->specs.block_size;
    }

    if (ssl->specs.cipher_type == aead) {
        *advance = ssl->specs.aead_mac_size;
        ssl->keys.padSz = ssl->specs.aead_mac_size;
    }
    else
        ssl->keys.padSz = ssl->specs.hash_size;

    if (ssl->specs.cipher_type == block) {
        /* last pad bytes indicates length */
        word32 pad = 0;
        if ((int)sz > ivExtra) {
            /* get value of last pad byte */
            pad = *(output + sz - ivExtra - 1) + 1;
        }
        ssl->keys.padSz += pad;
    }

#ifdef WOLFSSL_TLS13
    if (IsAtLeastTLSv1_3(ssl->version)) {
        word16 i = (word16)(sz - ssl->keys.padSz);
        /* Remove padding from end of plain text. */
        for (--i; i > 0; i--) {
            if (output[i] != 0)
                break;
        }
        /* Get the real content type from the end of the data. */
        rh->type = output[i];
        ssl->keys.padSz = sz - i;
    }
#endif
    (void)rh;

    return output;
}


/* remove session from table, use rowHint if no info (means we have a lock) */
static void RemoveSession(SnifferSession* session, IpInfo* ipInfo,
                        TcpInfo* tcpInfo, word32 rowHint)
{
    SnifferSession* previous = 0;
    SnifferSession* current;
    word32          row = rowHint;
#ifndef HAVE_C___ATOMIC
    int             haveLock = 0;
#endif
    Trace(REMOVE_SESSION_STR);

    if (ipInfo && tcpInfo)
        row = SessionHash(ipInfo, tcpInfo);
#ifndef HAVE_C___ATOMIC
    else
        haveLock = 1;
#endif

    if (row >= HASH_SIZE)
        return;

#ifndef HAVE_C___ATOMIC
    if (!haveLock) {
        LOCK_SESSION();
    }
#endif

    current = SessionTable[row];

    while (current) {
        if (current == session) {
            if (previous)
                previous->next = current->next;
            else
                SessionTable[row] = current->next;
            FreeSnifferSession(session);
            TraceRemovedSession();
            break;
        }
        previous = current;
        current  = current->next;
    }

#ifndef HAVE_C___ATOMIC
    if (!haveLock) {
        UNLOCK_SESSION();
    }
#endif
}


/* Remove stale sessions from the Session Table, have a lock */
static void RemoveStaleSessions(void)
{
    word32 i;
    SnifferSession* session;

    for (i = 0; i < HASH_SIZE; i++) {
        session = SessionTable[i];
        while (session) {
            SnifferSession* next = session->next;
            if (wc_Time(NULL) >= session->lastUsed + WOLFSSL_SNIFFER_TIMEOUT) {
                TraceStaleSession();
                RemoveSession(session, NULL, NULL, i);
            }
            session = next;
        }
    }
}


/* Create a new Sniffer Session */
static SnifferSession* CreateSession(IpInfo* ipInfo, TcpInfo* tcpInfo,
                                     char* error)
{
    SnifferSession* session = 0;
    int row;

    Trace(NEW_SESSION_STR);
    /* create a new one */
    session = (SnifferSession*)XMALLOC(sizeof(SnifferSession),
            NULL, DYNAMIC_TYPE_SNIFFER_SESSION);
    if (session == NULL) {
        SetError(MEMORY_STR, error, NULL, 0);
        return NULL;
    }
    InitSession(session);
#ifdef HAVE_EXTENDED_MASTER
    {
        HsHashes* newHash = (HsHashes*)XMALLOC(sizeof(HsHashes),
                NULL, DYNAMIC_TYPE_HASHES);
        if (newHash == NULL) {
            SetError(MEMORY_STR, error, NULL, 0);
            XFREE(session, NULL, DYNAMIC_TYPE_SNIFFER_SESSION);
            return NULL;
        }
        if (HashInit(newHash) != 0) {
            SetError(EXTENDED_MASTER_HASH_STR, error, NULL, 0);
            XFREE(session, NULL, DYNAMIC_TYPE_SNIFFER_SESSION);
            return NULL;
        }
        session->hash = newHash;
    }
#endif
    session->server  = ipInfo->dst;
    session->client  = ipInfo->src;
    session->srvPort = (word16)tcpInfo->dstPort;
    session->cliPort = (word16)tcpInfo->srcPort;
    session->cliSeqStart = tcpInfo->sequence;
    session->cliExpected = 1;  /* relative */
    session->lastUsed= wc_Time(NULL);
    session->keySz = 0;
    session->error = 0;
    session->verboseErr = 0;
#ifdef HAVE_SNI
    session->sni = NULL;
#endif

    session->context = GetSnifferServer(ipInfo, tcpInfo);
    if (session->context == NULL) {
        SetError(SERVER_NOT_REG_STR, error, NULL, 0);
        XFREE(session, NULL, DYNAMIC_TYPE_SNIFFER_SESSION);
        return NULL;
    }

    session->sslServer = wolfSSL_new(session->context->ctx);
    if (session->sslServer == NULL) {
        SetError(BAD_NEW_SSL_STR, error, session, FATAL_ERROR_STATE);
        XFREE(session, NULL, DYNAMIC_TYPE_SNIFFER_SESSION);
        return NULL;
    }
    session->sslClient = wolfSSL_new(session->context->ctx);
    if (session->sslClient == NULL) {
        wolfSSL_free(session->sslServer);
        session->sslServer = 0;

        SetError(BAD_NEW_SSL_STR, error, session, FATAL_ERROR_STATE);
        XFREE(session, NULL, DYNAMIC_TYPE_SNIFFER_SESSION);
        return NULL;
    }
    /* put server back into server mode */
    session->sslServer->options.side = WOLFSSL_SERVER_END;

    row = SessionHash(ipInfo, tcpInfo);

    /* add it to the session table */
    LOCK_SESSION();

    session->next = SessionTable[row];
    SessionTable[row] = session;

    SessionCount++;

    if ( (SessionCount % HASH_SIZE) == 0) {
        TraceFindingStale();
        RemoveStaleSessions();
    }

    UNLOCK_SESSION();

    /* CreateSession is called in response to a SYN packet, we know this
     * is headed to the server. Also we know the server is one we care
     * about as we've passed the GetSnifferServer() successfully. */
    session->flags.side = WOLFSSL_SERVER_END;

    return session;
}


#ifdef OLD_HELLO_ALLOWED

/* Process Old Client Hello Input */
static int DoOldHello(SnifferSession* session, const byte* sslFrame,
                      int* rhSize, int* sslBytes, char* error)
{
    const byte* input = sslFrame;
    byte        b0, b1;
    word32      idx = 0;
    int         ret;

    Trace(GOT_OLD_CLIENT_HELLO_STR);
    session->flags.clientHello = 1;    /* don't process again */
    b0 = *input++;
    b1 = *input++;
    *sslBytes -= 2;
    *rhSize = ((b0 & 0x7f) << 8) | b1;

    if (*rhSize > *sslBytes) {
        SetError(OLD_CLIENT_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    ret = ProcessOldClientHello(session->sslServer, input, &idx, *sslBytes,
                                (word16)*rhSize);
    if (ret < 0 && ret != MATCH_SUITE_ERROR) {
        SetError(BAD_OLD_CLIENT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }

    Trace(OLD_CLIENT_OK_STR);
    XMEMCPY(session->sslClient->arrays->clientRandom,
           session->sslServer->arrays->clientRandom, RAN_LEN);

    *sslBytes -= *rhSize;
    return 0;
}

#endif /* OLD_HELLO_ALLOWED */


#if 0
/* Calculate the TCP checksum, see RFC 1071 */
/* return 0 for success, -1 on error */
/* can be called from decode() with
   TcpChecksum(&ipInfo, &tcpInfo, sslBytes, packet + ipInfo.length);
   could also add a 64bit version if type available and using this
*/
static int TcpChecksum(IpInfo* ipInfo, TcpInfo* tcpInfo, int dataLen,
                const byte* packet)
{
    TcpPseudoHdr  pseudo;
    int           count = PSEUDO_HDR_SZ;
    const word16* data = (word16*)&pseudo;
    word32        sum = 0;
    word16        checksum;

    pseudo.src = ipInfo->src.ip4;
    pseudo.dst = ipInfo->dst.ip4;
    pseudo.rsv = 0;
    pseudo.protocol = TCP_PROTO;
    pseudo.length = htons(tcpInfo->length + dataLen);

    /* pseudo header sum */
    while (count >= 2) {
        sum   += *data++;
        count -= 2;
    }

    count = tcpInfo->length + dataLen;
    data = (word16*)packet;

    /* main sum */
    while (count > 1) {
        sum   += *data++;
        count -=2;
    }

    /* get left-over, if any */
    packet = (byte*)data;
    if (count > 0) {
        sum += *packet;
    }

    /* fold 32bit sum into 16 bits */
    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    checksum = (word16)~sum;
    /* checksum should now equal 0, since included already calcd checksum */
    /* field, but tcp checksum offloading could negate calculation */
    if (checksum == 0)
        return 0;
    return -1;
}
#endif


/* Check IP and TCP headers, set payload */
/* returns 0 on success, -1 on error */
static int CheckHeaders(IpInfo* ipInfo, TcpInfo* tcpInfo, const byte* packet,
    int length, const byte** sslFrame, int* sslBytes, char* error,
    int checkReg, int trace)
{
    IpHdr* iphdr = (IpHdr*)packet;
    TcpHdr* tcphdr;
    int version;

    if (trace) {
        TraceHeader();
        TracePacket();
    }

    /* ip header */
    if (length < IP_HDR_SZ) {
        SetError(PACKET_HDR_SHORT_STR, error, NULL, 0);
        return -1;
    }

    version = IP_V(iphdr);
    if (version != IPV6 && version != IPV4) {
        /* Is this VLAN IEEE 802.1Q Frame? TPID = 0x8100 */
        if (packet[2] == 0x81 && packet[3] == 0x00) {
            /* trim VLAN header and try again */
            packet += 8;
            length -= 8;
        }
    }

    if (CheckIpHdr(iphdr, ipInfo, length, error, trace) != 0)
        return -1;

#ifndef WOLFSSL_SNIFFER_WATCH
    if (checkReg &&
           !IsServerRegistered(iphdr->src) && !IsServerRegistered(iphdr->dst)) {
        SetError(SERVER_NOT_REG_STR, error, NULL, 0);
        return -1;
    }
#endif

    /* tcp header */
    if (length < (ipInfo->length + TCP_HDR_SZ)) {
        SetError(PACKET_HDR_SHORT_STR, error, NULL, 0);
        return -1;
    }
    tcphdr = (TcpHdr*)(packet + ipInfo->length);
    if (CheckTcpHdr(tcphdr, tcpInfo, error, trace) != 0)
        return -1;

#ifndef WOLFSSL_SNIFFER_WATCH
    if (checkReg &&
         !IsPortRegistered(tcpInfo->srcPort) &&
            !IsPortRegistered(tcpInfo->dstPort)) {
        SetError(SERVER_PORT_NOT_REG_STR, error, NULL, 0);
        return -1;
    }
#endif

    /* setup */
    *sslFrame = packet + ipInfo->length + tcpInfo->length;
    if (*sslFrame > packet + length) {
        SetError(PACKET_HDR_SHORT_STR, error, NULL, 0);
        return -1;
    }

    /* We only care about the data in the TCP/IP record. There may be extra
     * data after the IP record for the FCS for Ethernet. */
    *sslBytes = (int)(packet + ipInfo->total - *sslFrame);

    (void)checkReg;

    return 0;
}


/* Create or Find existing session */
/* returns 0 on success (continue), -1 on error, 1 on success (end) */
static int CheckSession(IpInfo* ipInfo, TcpInfo* tcpInfo, int sslBytes,
                        SnifferSession** session, char* error)
{
    /* create a new SnifferSession on client SYN */
    if (tcpInfo->syn && !tcpInfo->ack) {
    #ifdef WOLFSSL_ASYNC_CRYPT
        /* if session already exists and is pending do not create another */
        *session = GetSnifferSession(ipInfo, tcpInfo);
        if (*session != NULL) {
            if ((*session)->pendSeq != 0) {
                return WC_PENDING_E;
            }
        }
    #endif

        TraceClientSyn(tcpInfo->sequence);
#ifdef WOLFSSL_SNIFFER_STATS
        INC_STAT(SnifferStats.sslEncryptedConns);
#endif
        *session = CreateSession(ipInfo, tcpInfo, error);
        if (*session == NULL) {
            *session = GetSnifferSession(ipInfo, tcpInfo);
            /* already had existing, so OK */
            if (*session)
                return 1;

            SetError(MEMORY_STR, error, NULL, 0);
            return -1;
        }
        return 1;
    }
    /* get existing sniffer session */
    else {
        *session = GetSnifferSession(ipInfo, tcpInfo);
        if (*session == NULL) {
            /* don't worry about extraneous RST or duplicate FINs */
            if (tcpInfo->fin || tcpInfo->rst)
                return 1;
            /* don't worry about duplicate ACKs either */
            if (sslBytes == 0 && tcpInfo->ack)
                return 1;

#ifdef WOLFSSL_SNIFFER_STATS
            LOCK_STAT();
            NOLOCK_INC_STAT(SnifferStats.sslDecryptedPackets);
            NOLOCK_ADD_TO_STAT(SnifferStats.sslDecryptedBytes, sslBytes);
            UNLOCK_STAT();
#endif

            SetError(BAD_SESSION_STR, error, NULL, 0);
            return -1;
        }
    }
    return 0;
}


/* Create a Packet Buffer from *begin - end, adjust new *begin and bytesLeft */
static PacketBuffer* CreateBuffer(word32* begin, word32 end, const byte* data,
                                  int* bytesLeft)
{
    PacketBuffer* pb;
    int added = (int)(end - *begin + 1);

    if (added <= 0) {
        return NULL;
    }

    pb = (PacketBuffer*)XMALLOC(sizeof(PacketBuffer),
            NULL, DYNAMIC_TYPE_SNIFFER_PB);
    if (pb == NULL) return NULL;

    pb->next  = 0;
    pb->begin = *begin;
    pb->end   = end;
    pb->data  = (byte*)XMALLOC(added, NULL, DYNAMIC_TYPE_SNIFFER_PB_BUFFER);

    if (pb->data == NULL) {
        XFREE(pb, NULL, DYNAMIC_TYPE_SNIFFER_PB);
        return NULL;
    }
    XMEMCPY(pb->data, data, added);

    *bytesLeft -= added;
    *begin      = pb->end + 1;

    return pb;
}

/* Add sslFrame to Reassembly List */
/* returns 1 (end) on success, -1, on error */
static int AddToReassembly(byte from, word32 seq, const byte* sslFrame,
                           int sslBytes, SnifferSession* session, char* error)
{
    PacketBuffer*  add;
    PacketBuffer** front = (from == WOLFSSL_SERVER_END) ?
                       &session->cliReassemblyList: &session->srvReassemblyList;
    PacketBuffer*  curr = *front;
    PacketBuffer*  prev = curr;

    word32* reassemblyMemory = (from == WOLFSSL_SERVER_END) ?
                  &session->cliReassemblyMemory : &session->srvReassemblyMemory;
    word32  startSeq = seq;
    int     added;
    int     bytesLeft = sslBytes;  /* could be overlapping fragment */

    /* if list is empty add full frame to front */
    if (!curr) {
        if (MaxRecoveryMemory != -1 &&
                      (int)(*reassemblyMemory + sslBytes) > MaxRecoveryMemory) {
            SetError(REASSEMBLY_MAX_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        add = CreateBuffer(&seq, seq + sslBytes - 1, sslFrame, &bytesLeft);
        if (add == NULL) {
            SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        *front = add;
        *reassemblyMemory += sslBytes;
        return 1;
    }

    /* add to front if before current front, up to next->begin */
    if (seq < curr->begin) {
        word32 end = seq + sslBytes - 1;

        if (end >= curr->begin)
            end = curr->begin - 1;

        if (MaxRecoveryMemory -1 &&
                      (int)(*reassemblyMemory + sslBytes) > MaxRecoveryMemory) {
            SetError(REASSEMBLY_MAX_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        add = CreateBuffer(&seq, end, sslFrame, &bytesLeft);
        if (add == NULL) {
            SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        add->next = curr;
        *front = add;
        *reassemblyMemory += sslBytes;
    }

    /* while we have bytes left, try to find a gap to fill */
    while (bytesLeft > 0) {
        /* get previous packet in list */
        while (curr && (seq >= curr->begin)) {
            prev = curr;
            curr = curr->next;
        }

        /* don't add  duplicate data */
        if (prev->end >= seq) {
            if ( (seq + bytesLeft - 1) <= prev->end)
                return 1;
            seq = prev->end + 1;
            bytesLeft = startSeq + sslBytes - seq;
        }

        if (!curr)
            /* we're at the end */
            added = bytesLeft;
        else
            /* we're in between two frames */
            added = min(bytesLeft, (int)(curr->begin - seq));

        /* data already there */
        if (added <= 0)
            continue;

        if (MaxRecoveryMemory != -1 &&
                         (int)(*reassemblyMemory + added) > MaxRecoveryMemory) {
            SetError(REASSEMBLY_MAX_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        add = CreateBuffer(&seq, seq + added - 1, &sslFrame[seq - startSeq],
                           &bytesLeft);
        if (add == NULL) {
            SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        add->next  = prev->next;
        prev->next = add;
        *reassemblyMemory += added;
    }
    return 1;
}

/* Add out of order FIN capture */
/* returns 1 for success (end) */
static int AddFinCapture(SnifferSession* session, word32 sequence)
{
    if (session->flags.side == WOLFSSL_SERVER_END) {
        if (session->finCapture.cliCounted == 0)
            session->finCapture.cliFinSeq = sequence;
    }
    else {
        if (session->finCapture.srvCounted == 0)
            session->finCapture.srvFinSeq = sequence;
    }
    return 1;
}

static int FindPrevAck(SnifferSession* session, word32 realAck)
{
    int i;
    word32* acks = (session->flags.side == WOLFSSL_SERVER_END) ?
        session->cliAcks : session->srvAcks;
    /* if previous ack found return 1, otherwise 0 */
    for (i=0; i<WC_SNIFFER_HS_ACK_HIST_MAX; i++) {
        if (acks[i] == realAck) {
            return 1;
        }

    }
    return 0;
}
static void AddAck(SnifferSession* session, word32 realAck)
{
    int i;
    word32* acks = (session->flags.side == WOLFSSL_SERVER_END) ?
        session->cliAcks : session->srvAcks;
    /* find first empty ack slot */
    for (i=0; i<WC_SNIFFER_HS_ACK_HIST_MAX; i++) {
        if (acks[i] == 0) {
            break;
        }
    }
    /* if out of slots, find oldest */
    if (i == WC_SNIFFER_HS_ACK_HIST_MAX) {
        int idx = 0;
        word32 lastAck = realAck;
        for (i=0; i<WC_SNIFFER_HS_ACK_HIST_MAX; i++) {
            if (acks[i] < lastAck) {
                idx = i;
                lastAck = acks[i];
            }
        }
        i  = idx;
    }

    acks[i] = realAck;
}

/* Adjust incoming sequence based on side */
/* returns 0 on success (continue), -1 on error, 1 on success (end) */
static int AdjustSequence(TcpInfo* tcpInfo, SnifferSession* session,
                          int* sslBytes, const byte** sslFrame, char* error)
{
    int ret = 0;
    word32  seqStart = (session->flags.side == WOLFSSL_SERVER_END) ?
                                     session->cliSeqStart :session->srvSeqStart;
    word32  real     = tcpInfo->sequence - seqStart;
    word32* expected = (session->flags.side == WOLFSSL_SERVER_END) ?
                                  &session->cliExpected : &session->srvExpected;
    PacketBuffer* reassemblyList = (session->flags.side == WOLFSSL_SERVER_END) ?
                        session->cliReassemblyList : session->srvReassemblyList;
    byte  skipPartial = (session->flags.side == WOLFSSL_SERVER_END) ?
                                session->flags.srvSkipPartial :
                                session->flags.cliSkipPartial;

    /* handle rollover of sequence */
    if (tcpInfo->sequence < seqStart)
        real = 0xffffffffU - seqStart + tcpInfo->sequence + 1;

    TraceRelativeSequence(*expected, real);

    if (real < *expected) {

        if (real + *sslBytes > *expected) {
        #ifdef WOLFSSL_ASYNC_CRYPT
            if (session->sslServer->error != WC_PENDING_E &&
                session->pendSeq != tcpInfo->sequence)
        #endif
            {
                Trace(OVERLAP_DUPLICATE_STR);
            }

            /* The following conditional block is duplicated below. It is the
             * same action but for a different setup case. If changing this
             * block be sure to also update the block below. */
            if (reassemblyList) {
                int overlap = *expected - real;
                word32 newEnd;

                /* adjust to expected, remove duplicate */
                *sslFrame += overlap;
                *sslBytes = (*sslBytes > overlap) ? *sslBytes - overlap : 0;

                newEnd = *expected + *sslBytes;
                if (newEnd > reassemblyList->begin) {
                    int covered_data_len;

                    Trace(OVERLAP_REASSEMBLY_BEGIN_STR);

                    /* remove bytes already on reassembly list */
                    covered_data_len = newEnd - reassemblyList->begin;
                    *sslFrame += covered_data_len;
                    *sslBytes = (*sslBytes > covered_data_len) ?
                                 *sslBytes - covered_data_len : 0;
                }
                if ((*sslBytes  > 0) && (newEnd > reassemblyList->end)) {
                    Trace(OVERLAP_REASSEMBLY_END_STR);

                    /* may be past reassembly list end (could have more on list)
                       so try to add what's past the front->end */
                    AddToReassembly(session->flags.side, reassemblyList->end + 1,
                             *sslFrame + (reassemblyList->end - *expected + 1),
                                 newEnd - reassemblyList->end, session, error);
                }
            }
            else {
                /* DUP overlap, allow */
                if (*sslBytes > 0) {
                    skipPartial = 0; /* do not reset sslBytes */
                }
            }
            ret = 0;
        }
        else {
            /* This can happen with unseen acks, out of order packets, or
             * possible spurious retransmission. */
            if (*sslBytes > 0) {
                /* If packet has data attempt to process packet, if hasn't
                 * already been ack'd during handshake */
                if (
                #ifdef WOLFSSL_ASYNC_CRYPT
                    session->sslServer->error != WC_PENDING_E &&
                    session->pendSeq != tcpInfo->sequence &&
                #endif
                                                   FindPrevAck(session, real)) {
                    Trace(DUPLICATE_STR);
                    ret = 1;
                }
                else {
                    /* DUP: allow */
                    skipPartial = 0; /* do not reset sslBytes */
                    ret = 0;
                }
            }
            else {
                /* DUP empty, ignore */
                ret = 1;
            }
        }
    }
    else if (real > *expected) {
        Trace(OUT_OF_ORDER_STR);
        if (*sslBytes > 0) {
            int addResult = AddToReassembly(session->flags.side, real,
                                          *sslFrame, *sslBytes, session, error);
            ret = (skipPartial) ? 0 : addResult;
        }
        else if (tcpInfo->fin) {
            ret = AddFinCapture(session, real);
        }
    }
    else if (*sslBytes > 0) {
        if (skipPartial) {
            AddToReassembly(session->flags.side, real,
                                          *sslFrame, *sslBytes, session, error);
            ret = 0;
        }
        /* The following conditional block is duplicated above. It is the
         * same action but for a different setup case. If changing this
         * block be sure to also update the block above. */
        else if (reassemblyList) {
            word32 newEnd = *expected + *sslBytes;

            if (newEnd > reassemblyList->begin) {
                int covered_data_len;

                Trace(OVERLAP_REASSEMBLY_BEGIN_STR);

                /* remove bytes already on reassembly list */
                covered_data_len = newEnd - reassemblyList->begin;
                *sslFrame += covered_data_len;
                *sslBytes = (*sslBytes > covered_data_len) ?
                             *sslBytes - covered_data_len : 0;
            }
            if ((*sslBytes > 0) && (newEnd > reassemblyList->end)) {
                Trace(OVERLAP_REASSEMBLY_END_STR);

                /* may be past reassembly list end (could have more on list)
                   so try to add what's past the front->end */
                AddToReassembly(session->flags.side, reassemblyList->end + 1,
                         *sslFrame + (reassemblyList->end - *expected + 1),
                             newEnd - reassemblyList->end, session, error);
            }
        }
    }
    else {
        /* no data present */
    }

    if (ret == 0) {
        /* got expected sequence */
        *expected += *sslBytes;
        if (tcpInfo->fin)
            *expected += 1;
    }
    if (*sslBytes > 0) {
        AddAck(session, real);
    }
    if (*sslBytes > 0 && skipPartial) {
        *sslBytes = 0;
    }

    return ret;
}


static int FindNextRecordInAssembly(SnifferSession* session,
                                    const byte** sslFrame, int* sslBytes,
                                    const byte** end, char* error)
{
    PacketBuffer**     front = (session->flags.side == WOLFSSL_SERVER_END) ?
                                    &session->cliReassemblyList :
                                    &session->srvReassemblyList;
    PacketBuffer*       curr = *front;
    PacketBuffer*       prev = NULL;
    byte*        skipPartial = (session->flags.side == WOLFSSL_SERVER_END) ?
                                    &session->flags.srvSkipPartial :
                                    &session->flags.cliSkipPartial;
    int* reassemblyMemory = (session->flags.side == WOLFSSL_SERVER_END) ?
                                    (int*)&session->cliReassemblyMemory :
                                    (int*)&session->srvReassemblyMemory;
    WOLFSSL*             ssl = (session->flags.side == WOLFSSL_SERVER_END) ?
                                    session->sslServer :
                                    session->sslClient;
    ProtocolVersion       pv = ssl->version;
    word32*         expected = (session->flags.side == WOLFSSL_SERVER_END) ?
                                    &session->cliExpected :
                                    &session->srvExpected;

    while (curr != NULL) {
        *expected = curr->end + 1;

        if (curr->data[0] == application_data &&
            curr->data[1] == pv.major &&
            curr->data[2] == pv.minor) {

            if (ssl->buffers.inputBuffer.length > 0)
                Trace(DROPPING_PARTIAL_RECORD);

            *sslBytes = (int)(curr->end - curr->begin + 1);
            if ( *sslBytes > (int)ssl->buffers.inputBuffer.bufferSize) {
                if (GrowInputBuffer(ssl, *sslBytes, 0) < 0) {
                    SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
                    return -1;
                }
            }

            XMEMCPY(ssl->buffers.inputBuffer.buffer, curr->data, *sslBytes);

            *front = curr->next;
            *reassemblyMemory -= *sslBytes;
            FreePacketBuffer(curr);

            ssl->buffers.inputBuffer.length = *sslBytes;
            *sslFrame = ssl->buffers.inputBuffer.buffer;
            *end = *sslFrame + *sslBytes;
            *skipPartial = 0;

            return 0;
        }
        else if (ssl->specs.cipher_type == block) {
            int ivPos = (int)(curr->end - curr->begin -
                                                     ssl->specs.block_size + 1);
            if (ssl->specs.bulk_cipher_algorithm == wolfssl_aes) {
#ifndef NO_AES
                if (ivPos >= 0)
                    wc_AesSetIV(ssl->decrypt.aes, curr->data + ivPos);
#endif
            }
            else if (ssl->specs.bulk_cipher_algorithm == wolfssl_triple_des) {
#ifndef NO_DES3
                if (ivPos >= 0)
                    wc_Des3_SetIV(ssl->decrypt.des3, curr->data + ivPos);
#endif
            }
        }

        Trace(DROPPING_LOST_FRAG_STR);
#ifdef WOLFSSL_SNIFFER_STATS
        INC_STAT(SnifferStats.sslDecodeFails);
#endif
        prev = curr;
        curr = curr->next;
        *reassemblyMemory -= (int)(prev->end - prev->begin + 1);
        FreePacketBuffer(prev);
    }

    *front = curr;

    return 0;
}


static int FixSequence(TcpInfo* tcpInfo, SnifferSession* session)
{
    word32*   expected = (session->flags.side == WOLFSSL_SERVER_END) ?
                                &session->srvExpected : &session->cliExpected;
    word32    seqStart = (session->flags.side == WOLFSSL_SERVER_END) ?
                                session->srvSeqStart : session->cliSeqStart;
    PacketBuffer* list = (session->flags.side == WOLFSSL_SERVER_END) ?
                                session->srvReassemblyList :
                                session->cliReassemblyList;
    byte*  skipPartial = (session->flags.side != WOLFSSL_SERVER_END) ?
                                &session->flags.srvSkipPartial :
                                &session->flags.cliSkipPartial;

    *skipPartial = 1;

    if (list != NULL)
        *expected = list->begin;
    else
        *expected = tcpInfo->ackNumber - seqStart;

    return 1;
}


/* Check latest ack number for missing packets
   return 0 ok, <0 on error */
static int CheckAck(TcpInfo* tcpInfo, SnifferSession* session)
{
    if (tcpInfo->ack) {
        word32  seqStart = (session->flags.side == WOLFSSL_SERVER_END) ?
                                     session->srvSeqStart :session->cliSeqStart;
        word32  real     = tcpInfo->ackNumber - seqStart;
        word32  expected = (session->flags.side == WOLFSSL_SERVER_END) ?
                                  session->srvExpected : session->cliExpected;

        /* handle rollover of sequence */
        if (tcpInfo->ackNumber < seqStart)
            real = 0xffffffffU - seqStart + tcpInfo->ackNumber + 1;

        TraceAck(real, expected);

        if (real > expected)
            return -1;  /* we missed a packet, ACKing data we never saw */
    }
    return 0;
}


/* Check TCP Sequence status */
/* returns 0 on success (continue), -1 on error, 1 on success (end) */
static int CheckSequence(IpInfo* ipInfo, TcpInfo* tcpInfo,
                         SnifferSession* session, int* sslBytes,
                         const byte** sslFrame, char* error)
{
    int actualLen;
    byte* ackFault = (session->flags.side == WOLFSSL_SERVER_END) ?
                        &session->flags.cliAckFault :
                        &session->flags.srvAckFault;

#ifdef WOLFSSL_ASYNC_CRYPT
    if (session->sslServer->error == 0 && session->pendSeq != 0 &&
        session->pendSeq == tcpInfo->sequence) {
        return 0; /* ready to process, but skip sequence checking below (already done) */
    }
    /* check if this session is pending */
    else if (session->pendSeq != 0 && session->pendSeq != tcpInfo->sequence) {
        return WC_PENDING_E;
    }
#endif

    /* init SEQ from server to client - if not ack fault */
    if (tcpInfo->syn && tcpInfo->ack && !*ackFault) {
        session->srvSeqStart = tcpInfo->sequence;
        session->srvExpected = 1;
        TraceServerSyn(tcpInfo->sequence);
        return 1;
    }

    /* adjust potential ethernet trailer */
    actualLen = ipInfo->total - ipInfo->length - tcpInfo->length;
    if (*sslBytes > actualLen) {
        *sslBytes = actualLen;
    }

    TraceSequence(tcpInfo->sequence, *sslBytes);
    if (CheckAck(tcpInfo, session) < 0) {
        if (!RecoveryEnabled) {
        #ifndef WOLFSSL_SNIFFER_NO_RECOVERY
            UpdateMissedDataSessions();
        #endif
            SetError(ACK_MISSED_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        else {
            SetError(ACK_MISSED_STR, error, session, 0);
            if (*ackFault == 0) {
                *ackFault = 1;
            #ifndef WOLFSSL_SNIFFER_NO_RECOVERY
                UpdateMissedDataSessions();
            #endif
            }
            return FixSequence(tcpInfo, session);
        }
    }

    if (*ackFault) {
        Trace(CLEAR_ACK_FAULT);
        *ackFault = 0;
    }

    return AdjustSequence(tcpInfo, session, sslBytes, sslFrame, error);
}


/* Check Status before record processing */
/* returns 0 on success (continue), -1 on error, 1 on success (end) */
static int CheckPreRecord(IpInfo* ipInfo, TcpInfo* tcpInfo,
                          const byte** sslFrame, SnifferSession** pSession,
                          int* sslBytes, const byte** end,
                          void* vChain, word32 chainSz, char* error)
{
    word32 length;
    SnifferSession* session = *pSession;
    WOLFSSL* ssl = (session->flags.side == WOLFSSL_SERVER_END) ?
                                  session->sslServer : session->sslClient;
    byte skipPartial = (session->flags.side == WOLFSSL_SERVER_END) ?
                        session->flags.srvSkipPartial :
                        session->flags.cliSkipPartial;

#ifdef WOLFSSL_ASYNC_CRYPT
    /* if this is a pending async packet do not "grow" on partial (we already did) */
    if (session->pendSeq == tcpInfo->sequence) {
        if (session->sslServer->error == WC_PENDING_E) {
            return 0; /* don't check pre-record again */
        }
        /* if record check already done then restore, otherwise process normal */
        if (ssl->buffers.inputBuffer.length > 0) {
            *sslBytes = ssl->buffers.inputBuffer.length;
            *sslFrame = ssl->buffers.inputBuffer.buffer;
            *end = *sslFrame + *sslBytes;
            return 0;
        }
    }
#endif

    /* remove SnifferSession on 2nd FIN or RST */
    if (tcpInfo->fin || tcpInfo->rst) {
        /* flag FIN and RST */
        if (tcpInfo->fin)
            session->flags.finCount += 1;
        else if (tcpInfo->rst)
            session->flags.finCount += 2;

        if (session->flags.finCount >= 2) {
            RemoveSession(session, ipInfo, tcpInfo, 0);
            *pSession = NULL;
            return 1;
        }
    }

    if (session->flags.fatalError == FATAL_ERROR_STATE) {
        SetError(FATAL_ERROR_STR, error, NULL, 0);
        return -1;
    }

    if (skipPartial) {
        if (FindNextRecordInAssembly(session,
                                     sslFrame, sslBytes, end, error) < 0) {
            return -1;
        }
    }

    if (*sslBytes <= 0) {
        Trace(NO_DATA_STR);
        return 1;
    }

    /* if current partial data, add to end of partial */
    /* if skipping, the data is already at the end of partial */
    length = ssl->buffers.inputBuffer.length;
    if ( !skipPartial && length ) {
        Trace(PARTIAL_ADD_STR);

        if ( (*sslBytes + length) > ssl->buffers.inputBuffer.bufferSize) {
            if (GrowInputBuffer(ssl, *sslBytes, length) < 0) {
                SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
                return -1;
            }
        }
        if (vChain == NULL) {
            XMEMCPY(&ssl->buffers.inputBuffer.buffer[length],
                    *sslFrame, *sslBytes);
            *sslBytes += length;
            ssl->buffers.inputBuffer.length = *sslBytes;
            *sslFrame = ssl->buffers.inputBuffer.buffer;
            *end = *sslFrame + *sslBytes;
        }
        else {
    #ifdef WOLFSSL_SNIFFER_CHAIN_INPUT
            struct iovec* chain = (struct iovec*)vChain;
            word32 i, offset, headerSz, qty, remainder;

            Trace(CHAIN_INPUT_STR);
            headerSz = (word32)((const byte*)*sslFrame - (const byte*)chain[0].iov_base);
            remainder = *sslBytes;

            if ( (*sslBytes + length) > ssl->buffers.inputBuffer.bufferSize) {
                if (GrowInputBuffer(ssl, *sslBytes, length) < 0) {
                    SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
                    return -1;
                }
            }

            qty = min(*sslBytes, (word32)chain[0].iov_len - headerSz);
            XMEMCPY(&ssl->buffers.inputBuffer.buffer[length],
                (byte*)chain[0].iov_base + headerSz, qty);
            offset = length;
            for (i = 1; i < chainSz; i++) {
                offset += qty;
                remainder -= qty;

                if (chain[i].iov_len > remainder)
                    qty = remainder;
                else
                    qty = (word32)chain[i].iov_len;
                XMEMCPY(ssl->buffers.inputBuffer.buffer + offset,
                        chain[i].iov_base, qty);
            }

            *sslBytes += length;
            ssl->buffers.inputBuffer.length = *sslBytes;
            *sslFrame = ssl->buffers.inputBuffer.buffer;
            *end = *sslFrame + *sslBytes;
    #endif
            (void)chainSz;
        }
    }

    if (session->flags.clientHello == 0 && **sslFrame != handshake) {
        /* Sanity check the packet for an old style client hello. */
        int rhSize = (((*sslFrame)[0] & 0x7f) << 8) | ((*sslFrame)[1]);

        if ((rhSize <= (*sslBytes - 2)) &&
            (*sslFrame)[2] == OLD_HELLO_ID && (*sslFrame)[3] == SSLv3_MAJOR) {
#ifdef OLD_HELLO_ALLOWED
        int ret = DoOldHello(session, *sslFrame, &rhSize, sslBytes, error);
        if (ret < 0)
            return -1;  /* error already set */
        if (*sslBytes <= 0)
            return 1;
#endif
        }
        else {
#ifdef STARTTLS_ALLOWED
            if (ssl->buffers.inputBuffer.dynamicFlag) {
                ssl->buffers.inputBuffer.length = 0;
                ShrinkInputBuffer(ssl, NO_FORCED_FREE);
            }
            return 1;
#endif
        }
    }

    return 0;
}


/* See if input on the reassembly list is ready for consuming */
/* returns 1 for TRUE, 0 for FALSE */
static int HaveMoreInput(SnifferSession* session, const byte** sslFrame,
                         int* sslBytes, const byte** end, char* error)
{
    /* sequence and reassembly based on from, not to */
    int            moreInput = 0;
    PacketBuffer** front = (session->flags.side == WOLFSSL_SERVER_END) ?
                      &session->cliReassemblyList : &session->srvReassemblyList;
    word32*        expected = (session->flags.side == WOLFSSL_SERVER_END) ?
                                  &session->cliExpected : &session->srvExpected;
    /* buffer is on receiving end */
    word32*          length = (session->flags.side == WOLFSSL_SERVER_END) ?
                               &session->sslServer->buffers.inputBuffer.length :
                               &session->sslClient->buffers.inputBuffer.length;
    byte**         myBuffer = (session->flags.side == WOLFSSL_SERVER_END) ?
                               &session->sslServer->buffers.inputBuffer.buffer :
                               &session->sslClient->buffers.inputBuffer.buffer;
    word32*      bufferSize = (session->flags.side == WOLFSSL_SERVER_END) ?
                           &session->sslServer->buffers.inputBuffer.bufferSize :
                           &session->sslClient->buffers.inputBuffer.bufferSize;
    WOLFSSL*           ssl  = (session->flags.side == WOLFSSL_SERVER_END) ?
                            session->sslServer : session->sslClient;
    word32*     reassemblyMemory = (session->flags.side == WOLFSSL_SERVER_END) ?
                  &session->cliReassemblyMemory : &session->srvReassemblyMemory;

    while (*front && ((*front)->begin == *expected) ) {
        int room = (int)(*bufferSize - *length);
        int packetLen = (int)((*front)->end - (*front)->begin + 1);

        if (packetLen > room && *bufferSize < MAX_INPUT_SZ) {
            if (GrowInputBuffer(ssl, packetLen, *length) < 0) {
                SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
                return 0;
            }
            room = *bufferSize - *length;   /* bufferSize is now bigger */
        }

        if (packetLen <= room) {
            PacketBuffer* del = *front;
            byte*         buf = *myBuffer;

            XMEMCPY(&buf[*length], (*front)->data, packetLen);
            *length   += packetLen;
            *expected += packetLen;

            /* remove used packet */
            *front = (*front)->next;

            *reassemblyMemory -= packetLen;
            FreePacketBuffer(del);

            moreInput = 1;
        }
        else
            break;
    }
    if (moreInput) {
        *sslFrame = *myBuffer;
        *sslBytes = *length;
        *end      = *myBuffer + *length;
    }
    return moreInput;
}



/* Process Message(s) from sslFrame */
/* return Number of bytes on success, 0 for no data yet, and -1 on error */
static int ProcessMessage(const byte* sslFrame, SnifferSession* session,
                          int sslBytes, byte** data, const byte* end,
                          void* ctx, char* error)
{
    const byte*       sslBegin = sslFrame;
    const byte*       recordEnd;   /* end of record indicator */
    const byte*       inRecordEnd; /* indicator from input stream not decrypt */
    RecordLayerHeader rh;
    int               rhSize;
    int               ret;
    int               errCode = 0;
    int               decoded = 0;      /* bytes stored for user in data */
    int               notEnough;        /* notEnough bytes yet flag */
    int               decrypted = 0;    /* was current msg decrypted */
    WOLFSSL*          ssl = (session->flags.side == WOLFSSL_SERVER_END) ?
                            session->sslServer : session->sslClient;
doMessage:

    notEnough = 0;
    rhSize = 0;
    if (sslBytes < 0) {
        SetError(PACKET_HDR_SHORT_STR, error, session, FATAL_ERROR_STATE);
        return -1;
    }
    if (sslBytes >= RECORD_HEADER_SZ) {
        if (GetRecordHeader(sslFrame, &rh, &rhSize) != 0) {
            /* ignore packet if record header errors */
            SetError(BAD_RECORD_HDR_STR, error, session, 0);
            return 0;
        }
    }
    else {
        notEnough = 1;
    }

    if (notEnough || rhSize > (sslBytes - RECORD_HEADER_SZ)) {
        /* don't have enough input yet to process full SSL record */
        Trace(PARTIAL_INPUT_STR);

        /* store partial if not there already or we advanced */
        if (ssl->buffers.inputBuffer.length == 0 || sslBegin != sslFrame) {
            if (sslBytes > (int)ssl->buffers.inputBuffer.bufferSize) {
                if (GrowInputBuffer(ssl, sslBytes, 0) < 0) {
                    SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
                    return -1;
                }
            }
            XMEMMOVE(ssl->buffers.inputBuffer.buffer, sslFrame, sslBytes);
            ssl->buffers.inputBuffer.length = sslBytes;
        }
        if (HaveMoreInput(session, &sslFrame, &sslBytes, &end, error))
            goto doMessage;
        return decoded;
    }
    sslFrame += RECORD_HEADER_SZ;
    sslBytes -= RECORD_HEADER_SZ;
    recordEnd = sslFrame + rhSize;   /* may have more than one record */
    inRecordEnd = recordEnd;

    /* Make sure cipher is on for client, if we get an application data packet
     * and handshake is done for server. This workaround is required if client
     * handshake packets were missed, retransmitted or sent out of order. */
    if ((enum ContentType)rh.type == application_data &&
                  ssl->options.handShakeDone && session->flags.serverCipherOn) {
        session->flags.clientCipherOn = 1;
        session->sslClient->options.handShakeState = HANDSHAKE_DONE;
        session->sslClient->options.handShakeDone  = 1;
    }

    /* decrypt if needed */
    if ((session->flags.side == WOLFSSL_SERVER_END &&
                                               session->flags.serverCipherOn)
     || (session->flags.side == WOLFSSL_CLIENT_END &&
                                               session->flags.clientCipherOn)) {
        int ivAdvance = 0;  /* TLSv1.1 advance amount */

        /* change_cipher_spec is not encrypted */
        if (rh.type == change_cipher_spec) {
            goto doPart;
        }
        if (ssl->decrypt.setup != 1) {
            SetError(DECRYPT_KEYS_NOT_SETUP, error, session, FATAL_ERROR_STATE);
            return -1;
        }
        if (CheckAvailableSize(ssl, rhSize) < 0) {
            SetError(MEMORY_STR, error, session, FATAL_ERROR_STATE);
            return -1;
        }

        sslFrame = DecryptMessage(ssl, sslFrame, rhSize,
                                  ssl->buffers.outputBuffer.buffer, &errCode,
                                  &ivAdvance, &rh);
        recordEnd = sslFrame - ivAdvance + rhSize;  /* sslFrame moved so
                                                       should recordEnd */
        decrypted = 1;

#ifdef WOLFSSL_SNIFFER_STATS
        if (errCode != 0) {
            INC_STAT(SnifferStats.sslKeyFails);
        }
        else {
            LOCK_STAT();
            NOLOCK_INC_STAT(SnifferStats.sslDecryptedPackets);
            NOLOCK_ADD_TO_STAT(SnifferStats.sslDecryptedBytes, sslBytes);
            UNLOCK_STAT();
        }
#endif
        if (errCode != 0) {
            if ((enum ContentType)rh.type == application_data) {
                SetError(BAD_DECRYPT, error, session, FATAL_ERROR_STATE);
                return -1;
            }
            /* do not end session for failures on handshake packets */
            return 0;
        }
    }

doPart:

    switch ((enum ContentType)rh.type) {
        case handshake:
            {
                int startIdx = sslBytes;
                int used;

                Trace(GOT_HANDSHAKE_STR);
                ret = DoHandShake(sslFrame, &sslBytes, session, error, rhSize);
            #ifdef WOLFSSL_ASYNC_CRYPT
                if (ret == WC_PENDING_E)
                    return ret;
            #endif
                if (ret != 0 || sslBytes > startIdx) {
                    if (session->flags.fatalError == 0)
                        SetError(BAD_HANDSHAKE_STR, error, session,
                                 FATAL_ERROR_STATE);
                    return -1;
                }

                /* DoHandShake now fully decrements sslBytes to remaining */
                used = startIdx - sslBytes;
                sslFrame += used;
                if (decrypted)
                    sslFrame += ssl->keys.padSz;
            }
            break;
        case change_cipher_spec:
            if (session->flags.side == WOLFSSL_SERVER_END) {
            #ifdef WOLFSSL_TLS13
                if (IsAtLeastTLSv1_3(session->sslServer->version) && session->srvKs.key_len == 0) {
                    session->flags.serverCipherOn = 0;
                }
                else
            #endif
                {
                    session->flags.serverCipherOn = 1;
                }
            }
            else
                session->flags.clientCipherOn = 1;
            Trace(GOT_CHANGE_CIPHER_STR);
            ssl->options.handShakeState = HANDSHAKE_DONE;
            ssl->options.handShakeDone  = 1;

            sslFrame += 1;
            sslBytes -= 1;

            break;
        case application_data:
            Trace(GOT_APP_DATA_STR);
            {
                word32 inOutIdx = 0;

                ret = DoApplicationData(ssl, (byte*)sslFrame, &inOutIdx, SNIFF);
                if (ret == 0) {
                    ret = ssl->buffers.clearOutputBuffer.length;
                    TraceGotData(ret);
                    if (ret) {  /* may be blank message */
                        if (data != NULL) {
                            byte* tmpData;  /* don't leak on realloc free */
                            /* add an extra byte at end of allocation in case
                             * user wants to null terminate plaintext */
                            tmpData = (byte*)XREALLOC(*data, decoded + ret + 1,
                                    NULL, DYNAMIC_TYPE_TMP_BUFFER);
                            if (tmpData == NULL) {
                                ForceZero(*data, decoded);
                                XFREE(*data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                                *data = NULL;
                                SetError(MEMORY_STR, error, session,
                                         FATAL_ERROR_STATE);
                                return -1;
                            }
                            *data = tmpData;
                            XMEMCPY(*data + decoded,
                                    ssl->buffers.clearOutputBuffer.buffer, ret);
                        }
                        else {
#ifdef WOLFSSL_SNIFFER_STORE_DATA_CB
                            if (StoreDataCb) {
                                const byte* buf;
                                word32 offset = 0;
                                word32 bufSz;
                                int stored;

                                buf = ssl->buffers.clearOutputBuffer.buffer;
                                bufSz = ssl->buffers.clearOutputBuffer.length;
                                do {
                                    stored = StoreDataCb(buf, bufSz, offset,
                                            ctx);
                                    if (stored <= 0) {
                                        return -1;
                                    }
                                    offset += stored;
                                } while (offset < bufSz);
                            }
                            else {
                                SetError(STORE_DATA_CB_MISSING_STR, error,
                                        session, FATAL_ERROR_STATE);
                                return -1;
                            }
#else
                            (void)ctx;
                            SetError(NO_DATA_DEST_STR, error, session,
                                    FATAL_ERROR_STATE);
                            return -1;
#endif
                        }
                        TraceAddedData(ret, decoded);
                        decoded += ret;
                        ssl->buffers.clearOutputBuffer.length = 0;
                    }
                }
                else {
                    /* set error, but do not treat fatal */
                    SetError(BAD_APP_DATA_STR, error,session, 0);
                    return -1;
                }
                if (ssl->buffers.outputBuffer.dynamicFlag)
                    ShrinkOutputBuffer(ssl);

                sslFrame += inOutIdx;
                sslBytes -= inOutIdx;
            }
            break;
        case alert:
            Trace(GOT_ALERT_STR);
#ifdef WOLFSSL_SNIFFER_STATS
            INC_STAT(SnifferStats.sslAlerts);
#endif
            sslFrame += rhSize;
            sslBytes -= rhSize;
            break;
#ifdef WOLFSSL_DTLS13
        case ack:
            /* TODO */
#endif /* WOLFSSL_DTLS13 */
        case no_type:
        default:
            SetError(GOT_UNKNOWN_RECORD_STR, error, session, FATAL_ERROR_STATE);
            return -1;
    }

    /* do we have another msg in record ? */
    if (sslFrame < recordEnd) {
        Trace(ANOTHER_MSG_STR);
        goto doPart;
    }

    /* back to input stream instead of potential decrypt buffer */
    recordEnd = inRecordEnd;

    /* do we have more records ? */
    if (recordEnd < end) {
        Trace(ANOTHER_MSG_STR);
        sslFrame = recordEnd;
        sslBytes = (int)(end - recordEnd);
        goto doMessage;
    }

    /* clear used input */
    ssl->buffers.inputBuffer.length = 0;

    /* could have more input ready now */
    if (HaveMoreInput(session, &sslFrame, &sslBytes, &end, error))
        goto doMessage;

    if (ssl->buffers.inputBuffer.dynamicFlag)
        ShrinkInputBuffer(ssl, NO_FORCED_FREE);

    return decoded;
}


/* See if we need to process any pending FIN captures */
/* Return 0=normal, else = session removed */
static int CheckFinCapture(IpInfo* ipInfo, TcpInfo* tcpInfo,
                            SnifferSession* session)
{
    int ret = 0;
    if (session->finCapture.cliFinSeq && session->finCapture.cliFinSeq <=
                                         session->cliExpected) {
        if (session->finCapture.cliCounted == 0) {
            session->flags.finCount += 1;
            session->finCapture.cliCounted = 1;
            TraceClientFin(session->finCapture.cliFinSeq, session->cliExpected);
        }
    }

    if (session->finCapture.srvFinSeq && session->finCapture.srvFinSeq <=
                                         session->srvExpected) {
        if (session->finCapture.srvCounted == 0) {
            session->flags.finCount += 1;
            session->finCapture.srvCounted = 1;
            TraceServerFin(session->finCapture.srvFinSeq, session->srvExpected);
        }
    }

    if (session->flags.finCount >= 2) {
        RemoveSession(session, ipInfo, tcpInfo, 0);
        ret = 1;
    }
    return ret;
}


/* If session is in fatal error state free resources now
   return true if removed, 0 otherwise */
static int RemoveFatalSession(IpInfo* ipInfo, TcpInfo* tcpInfo,
                              SnifferSession* session, char* error)
{
    if (session && session->flags.fatalError == FATAL_ERROR_STATE) {
        RemoveSession(session, ipInfo, tcpInfo, 0);
        if (!session->verboseErr) {
            SetError(FATAL_ERROR_STR, error, NULL, 0);
        }
        return 1;
    }
    return 0;
}

int ssl_DecodePacket_GetStream(SnifferStreamInfo* info, const byte* packet,
        int length, char* error )
{
    TcpInfo           tcpInfo;
    IpInfo            ipInfo;
    const byte*       sslFrame = NULL;
    int               sslBytes = 0;

    XMEMSET(&tcpInfo, 0, sizeof(tcpInfo));
    XMEMSET(&ipInfo, 0, sizeof(ipInfo));

    if (CheckHeaders(&ipInfo, &tcpInfo, packet, length, &sslFrame, &sslBytes,
            error, 0, 0) != 0) {
        return WOLFSSL_SNIFFER_ERROR;
    }

    info->src     = ipInfo.src;
    info->dst     = ipInfo.dst;
    info->srcPort = tcpInfo.srcPort;
    info->dstPort = tcpInfo.dstPort;

    return 0;
}

/* Passes in an IP/TCP packet for decoding (ethernet/localhost frame) removed */
/* returns Number of bytes on success, 0 for no data yet, and
 * WOLFSSL_SNIFFER_ERROR on error and WOLFSSL_SNIFFER_FATAL_ERROR on fatal state
 * error
 */
static int ssl_DecodePacketInternal(const byte* packet, int length, int isChain,
                                    byte** data, SSLInfo* sslInfo,
                                    void* ctx, char* error, int asyncOkay)
{
    TcpInfo           tcpInfo;
    IpInfo            ipInfo;
    const byte*       sslFrame;
    const byte*       end;
    int               sslBytes;                /* ssl bytes unconsumed */
    int               ret;
    SnifferSession*   session = NULL;
    void* vChain = NULL;
    word32 chainSz = 0;

    if (isChain) {
#ifdef WOLFSSL_SNIFFER_CHAIN_INPUT
        struct iovec* chain;
        word32 i;

        vChain = (void*)packet;
        chainSz = (word32)length;

        chain = (struct iovec*)vChain;
        length = 0;
        for (i = 0; i < chainSz; i++)
            length += chain[i].iov_len;
        packet = (const byte*)chain[0].iov_base;
#else
        SetError(BAD_INPUT_STR, error, session, FATAL_ERROR_STATE);
        return WOLFSSL_SNIFFER_ERROR;
#endif
    }

    if (CheckHeaders(&ipInfo, &tcpInfo, packet, length, &sslFrame, &sslBytes,
                     error, 1, 1) != 0) {
        return WOLFSSL_SNIFFER_ERROR;
    }

    end = sslFrame + sslBytes;

    ret = CheckSession(&ipInfo, &tcpInfo, sslBytes, &session, error);
    if (RemoveFatalSession(&ipInfo, &tcpInfo, session, error))
        return WOLFSSL_SNIFFER_FATAL_ERROR;
#ifdef WOLFSSL_ASYNC_CRYPT
    else if (ret == WC_PENDING_E) return WC_PENDING_E;
#endif
    else if (ret == -1) return WOLFSSL_SNIFFER_ERROR;
    else if (ret ==  1) {
#ifdef WOLFSSL_SNIFFER_STATS
        if (sslBytes > 0) {
            LOCK_STAT();
            NOLOCK_INC_STAT(SnifferStats.sslEncryptedPackets);
            NOLOCK_ADD_TO_STAT(SnifferStats.sslEncryptedBytes, sslBytes);
            UNLOCK_STAT();
        }
        else {
            INC_STAT(SnifferStats.sslDecryptedPackets);
        }
#endif
         return 0; /* done for now */
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    session->userCtx = ctx;
#endif

    ret = CheckSequence(&ipInfo, &tcpInfo, session, &sslBytes, &sslFrame,error);
    if (RemoveFatalSession(&ipInfo, &tcpInfo, session, error))
        return WOLFSSL_SNIFFER_FATAL_ERROR;
    else if (ret == -1) return WOLFSSL_SNIFFER_ERROR;
    else if (ret ==  1) {
#ifdef WOLFSSL_SNIFFER_STATS
        INC_STAT(SnifferStats.sslDecryptedPackets);
#endif
        return 0; /* done for now */
    }
    else if (ret != 0) {
        /* return specific error case */
        return ret;
    }

    ret = CheckPreRecord(&ipInfo, &tcpInfo, &sslFrame, &session, &sslBytes,
                         &end, vChain, chainSz, error);
    if (RemoveFatalSession(&ipInfo, &tcpInfo, session, error))
        return WOLFSSL_SNIFFER_FATAL_ERROR;
    else if (ret == -1) return WOLFSSL_SNIFFER_ERROR;
    else if (ret ==  1) {
#ifdef WOLFSSL_SNIFFER_STATS
        INC_STAT(SnifferStats.sslDecryptedPackets);
#endif
        return 0; /* done for now */
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    /* make sure this server was polled */
    if (asyncOkay && session->sslServer->error == WC_PENDING_E &&
        !session->flags.wasPolled) {
        return WC_PENDING_E;
    }
#endif

#ifdef WOLFSSL_SNIFFER_STATS
    #ifdef WOLFSSL_ASYNC_CRYPT
    if (session->sslServer->error != WC_PENDING_E)
    #endif
    {
        if (sslBytes > 0) {
            LOCK_STAT();
            NOLOCK_INC_STAT(SnifferStats.sslEncryptedPackets);
            NOLOCK_ADD_TO_STAT(SnifferStats.sslEncryptedBytes, sslBytes);
            UNLOCK_STAT();
        }
        else {
            INC_STAT(SnifferStats.sslDecryptedPackets);
        }
    }
#endif

#ifdef WOLFSSL_ASYNC_CRYPT
    do {
#endif
        ret = ProcessMessage(sslFrame, session, sslBytes, data, end, ctx, error);
        session->sslServer->error = ret;
#ifdef WOLFSSL_ASYNC_CRYPT
        /* capture the seq pending for this session */
        if (ret == WC_PENDING_E) {
            session->flags.wasPolled = 0;
            session->pendSeq = tcpInfo.sequence;
            if (!asyncOkay || CryptoDeviceId == INVALID_DEVID) {
                /* If devId has not been set then we need to block here by
                 * polling and looping */
                wolfSSL_AsyncPoll(session->sslServer, WOLF_POLL_FLAG_CHECK_HW);
            }
            else {
                return ret; /* return to caller */
            }
        }
        else {
            session->pendSeq = 0;
        }
    } while (ret == WC_PENDING_E);
#else
    (void)asyncOkay;
#endif

    if (RemoveFatalSession(&ipInfo, &tcpInfo, session, error))
        return WOLFSSL_SNIFFER_FATAL_ERROR;
    if (CheckFinCapture(&ipInfo, &tcpInfo, session) == 0) {
        CopySessionInfo(session, sslInfo);
    }

    return ret;
}


/* Passes in an IP/TCP packet for decoding (ethernet/localhost frame) removed */
/* returns Number of bytes on success, 0 for no data yet, WOLFSSL_SNIFFER_ERROR.
 * on error and WOLFSSL_SNIFFER_FATAL_ERROR on fatal state error */
/* Also returns Session Info if available */
int ssl_DecodePacketWithSessionInfo(const unsigned char* packet, int length,
    unsigned char** data, SSLInfo* sslInfo, char* error)
{
    return ssl_DecodePacketInternal(packet, length, 0, data, sslInfo,
            NULL, error, 0);
}


/* Passes in an IP/TCP packet for decoding (ethernet/localhost frame) removed */
/* returns Number of bytes on success, 0 for no data yet, WOLFSSL_SNIFFER_ERROR.
 * on error and WOLFSSL_SNIFFER_FATAL_ERROR on fatal state error */
int ssl_DecodePacket(const byte* packet, int length, byte** data, char* error)
{
    return ssl_DecodePacketInternal(packet, length, 0, data, NULL, NULL,
            error, 0);
}


#ifdef WOLFSSL_SNIFFER_STORE_DATA_CB

/* returns Number of bytes on success, 0 for no data yet, WOLFSSL_SNIFFER_ERROR.
 * on error and WOLFSSL_SNIFFER_FATAL_ERROR on fatal state error */
int ssl_DecodePacketWithSessionInfoStoreData(const unsigned char* packet,
        int length, void* ctx, SSLInfo* sslInfo, char* error)
{
    return ssl_DecodePacketInternal(packet, length, 0, NULL, sslInfo,
            ctx, error, 0);
}

#endif


#ifdef WOLFSSL_SNIFFER_CHAIN_INPUT

/* returns Number of bytes on success, 0 for no data yet, WOLFSSL_SNIFFER_ERROR.
 * on error and WOLFSSL_SNIFFER_FATAL_ERROR on fatal state error */
int ssl_DecodePacketWithChain(void* vChain, word32 chainSz, byte** data,
        char* error)
{
    return ssl_DecodePacketInternal((const byte*)vChain, chainSz, 1, data,
        NULL, NULL, error, 0);
}

#endif


#if defined(WOLFSSL_SNIFFER_CHAIN_INPUT) && \
     defined(WOLFSSL_SNIFFER_STORE_DATA_CB)

/*
 * returns WOLFSSL_SNIFFER_ERROR on error and WOLFSSL_SNIFFER_FATAL_ERROR on
 * fatal state error
 */
int ssl_DecodePacketWithChainSessionInfoStoreData(void* vChain, word32 chainSz,
        void* ctx, SSLInfo* sslInfo, char* error)
{
    return ssl_DecodePacketInternal(vChain, chainSz, 1, NULL, sslInfo,
            ctx, error, 0);
}

#endif


/* Deallocator for the decoded data buffer. */
/* returns 0 on success, -1 on error */
int ssl_FreeDecodeBuffer(byte** data, char* error)
{
    return ssl_FreeZeroDecodeBuffer(data, 0, error);
}


/* Deallocator for the decoded data buffer, zeros out buffer. */
/* returns 0 on success, -1 on error */
int ssl_FreeZeroDecodeBuffer(byte** data, int sz, char* error)
{
    (void)error;

    if (sz < 0) {
        return -1;
    }

    if (data != NULL) {
        ForceZero(*data, (word32)sz);
        XFREE(*data, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        *data = NULL;
    }

    return 0;
}


/* Enables (if traceFile)/ Disables debug tracing */
/* returns 0 on success, -1 on error */
int ssl_Trace(const char* traceFile, char* error)
{
    if (traceFile) {
        /* Don't try to reopen the file */
        if (TraceFile == NULL) {
            TraceFile = XFOPEN(traceFile, "a");
            if (!TraceFile) {
                SetError(BAD_TRACE_FILE_STR, error, NULL, 0);
                return -1;
             }
            TraceOn = 1;
        }
    }
    else
        TraceOn = 0;

    return 0;
}


/* Enables/Disables Recovery of missed data if later packets allow
 * maxMemory is number of bytes to use for reassembly buffering per session,
 * -1 means unlimited
 * returns 0 on success, -1 on error */
int ssl_EnableRecovery(int onOff, int maxMemory, char* error)
{
    (void)error;

    RecoveryEnabled = onOff;
    if (onOff)
        MaxRecoveryMemory = maxMemory;

    return 0;
}



#if defined(WOLFSSL_SESSION_STATS) && !defined(NO_SESSION_CACHE)

int ssl_GetSessionStats(unsigned int* active,     unsigned int* total,
                        unsigned int* peak,       unsigned int* maxSessions,
                        unsigned int* missedData, unsigned int* reassemblyMem,
                        char* error)
{
    int ret;

    if (missedData) {
    #ifndef WOLFSSL_SNIFFER_NO_RECOVERY
        wc_LockMutex(&RecoveryMutex);
        *missedData = MissedDataSessions;
        wc_UnLockMutex(&RecoveryMutex);
    #endif
    }

    if (reassemblyMem) {
        SnifferSession* session;
        int i;

        *reassemblyMem = 0;
        LOCK_SESSION();

        for (i = 0; i < HASH_SIZE; i++) {
            session = SessionTable[i];
            while (session) {
                *reassemblyMem += session->cliReassemblyMemory;
                *reassemblyMem += session->srvReassemblyMemory;
                session = session->next;
            }
        }
        UNLOCK_SESSION();
    }

    ret = wolfSSL_get_session_stats(active, total, peak, maxSessions);

    if (ret == WOLFSSL_SUCCESS)
        return 0;
    else {
        SetError(BAD_SESSION_STATS, error, NULL, 0);
        return -1;
    }
}

#endif



int ssl_SetConnectionCb(SSLConnCb cb)
{
    ConnectionCb = cb;
    return 0;
}



int ssl_SetConnectionCtx(void* ctx)
{
    ConnectionCbCtx = ctx;
    return 0;
}


#ifdef WOLFSSL_SNIFFER_STATS

/* Resets the statistics tracking global structure.
 * returns 0 on success, -1 on error */
int ssl_ResetStatistics(void)
{
    wc_LockMutex(&StatsMutex);
    XMEMSET(&SnifferStats, 0, sizeof(SSLStats));
    wc_UnLockMutex(&StatsMutex);
    return 0;
}


/* Copies the SSL statistics into the provided stats record.
 * returns 0 on success, -1 on error */
int ssl_ReadStatistics(SSLStats* stats)
{
    if (stats == NULL)
        return -1;

    LOCK_STAT();
    XMEMCPY(stats, &SnifferStats, sizeof(SSLStats));
    UNLOCK_STAT();
    return 0;
}

/* Copies the SSL statistics into the provided stats record then
 * resets the statistics tracking global structure.
 * returns 0 on success, -1 on error */
int ssl_ReadResetStatistics(SSLStats* stats)
{
    if (stats == NULL)
        return -1;

    LOCK_STAT();
    XMEMCPY(stats, &SnifferStats, sizeof(SSLStats));
    XMEMSET(&SnifferStats, 0, sizeof(SSLStats));
    UNLOCK_STAT();
    return 0;
}

#endif /* WOLFSSL_SNIFFER_STATS */


#ifdef WOLFSSL_SNIFFER_WATCH

int ssl_SetWatchKeyCallback_ex(SSLWatchCb cb, int devId, char* error)
{
#ifdef WOLF_CRYPTO_CB
    if (CryptoDeviceId == INVALID_DEVID)
        CryptoDeviceId = devId;
#else
    (void)devId;
#endif
    WatchCb = cb;
    return CreateWatchSnifferServer(error);
}

int ssl_SetWatchKeyCallback(SSLWatchCb cb, char* error)
{
    WatchCb = cb;
    return CreateWatchSnifferServer(error);
}

int ssl_SetWatchKeyCtx(void* ctx, char* error)
{
    (void)error;
    WatchCbCtx = ctx;
    return 0;
}

int ssl_SetWatchKey_buffer(void* vSniffer, const byte* key, word32 keySz,
        int keyType, char* error)
{
    SnifferSession* sniffer;
    int ret;

    if (vSniffer == NULL) {
        return -1;
    }
    if (key == NULL || keySz == 0) {
        return -1;
    }

    sniffer = (SnifferSession*)vSniffer;
    /* Remap the keyType from what the user can use to
     * what wolfSSL_use_PrivateKey_buffer expects. */
    keyType = (keyType == FILETYPE_PEM) ? WOLFSSL_FILETYPE_PEM :
                                          WOLFSSL_FILETYPE_ASN1;

#ifdef WOLFSSL_STATIC_EPHEMERAL
    /* try setting static ephemeral first */
    /* auto detect key type with WC_PK_TYPE_NONE */
    ret = wolfSSL_set_ephemeral_key(sniffer->sslServer,
        WC_PK_TYPE_NONE, (const char*)key, keySz,
            WOLFSSL_FILETYPE_ASN1);
    if (ret != 0) {
    #ifdef DEBUG_SNIFFER
        /* print warnings */
        fprintf(stderr, "key watch set ephemeral failed %d\n", ret);
    #endif
    }
#endif

    /* always try and load private key */
    ret = wolfSSL_use_PrivateKey_buffer(sniffer->sslServer,
        key, keySz, keyType);

    if (ret != WOLFSSL_SUCCESS) {
        SetError(KEY_FILE_STR, error, sniffer, FATAL_ERROR_STATE);
        return -1;
    }

    return 0;
}

int ssl_SetWatchKey_file(void* vSniffer, const char* keyFile, int keyType,
        const char* password, char* error)
{
    byte* keyBuf = NULL;
    word32 keyBufSz = 0;
    int ret;

    if (vSniffer == NULL) {
        return -1;
    }
    if (keyFile == NULL) {
        return -1;
    }

    /* Remap the keyType from what the user can use to
     * what LoadKeyFile expects. */
    keyType = (keyType == FILETYPE_PEM) ? WOLFSSL_FILETYPE_PEM :
                                          WOLFSSL_FILETYPE_ASN1;

    ret = LoadKeyFile(&keyBuf, &keyBufSz, keyFile, 0, keyType, password);
    if (ret < 0) {
        SetError(KEY_FILE_STR, error, NULL, 0);
        XFREE(keyBuf, NULL, DYNAMIC_TYPE_X509);
        return -1;
    }

    ret = ssl_SetWatchKey_buffer(vSniffer, keyBuf, keyBufSz, FILETYPE_DER,
            error);
    XFREE(keyBuf, NULL, DYNAMIC_TYPE_X509);

    return ret;
}

#endif /* WOLFSSL_SNIFFER_WATCH */


#ifdef WOLFSSL_SNIFFER_STORE_DATA_CB

int ssl_SetStoreDataCallback(SSLStoreDataCb cb)
{
    StoreDataCb = cb;
    return 0;
}

#endif /* WOLFSSL_SNIFFER_STORE_DATA_CB */

#ifdef WOLFSSL_SNIFFER_KEY_CALLBACK
int ssl_SetKeyCallback(SSLKeyCb cb, void* cbCtx)
{
    KeyCb = cb;
    KeyCbCtx = cbCtx;
    return 0;
}
#endif

#ifdef WOLFSSL_ASYNC_CRYPT

int ssl_DecodePacketAsync(void* packet, unsigned int packetSz,
    int isChain, unsigned char** data, char* error, SSLInfo* sslInfo,
    void* userCtx)
{
    return ssl_DecodePacketInternal(packet, packetSz, isChain, data, sslInfo,
        userCtx, error, 1);
}

static SnifferSession* FindSession(WOLFSSL* ssl)
{
    int i;
    SnifferSession* session;
    for (i = 0; i < HASH_SIZE; i++) {
        session = SessionTable[i];
        while (session) {
            if (session->sslServer == ssl) {
                return session;
            }
            session = session->next;
        }
    }
    return NULL;
}

int ssl_PollSniffer(WOLF_EVENT** events, int maxEvents, WOLF_EVENT_FLAG flags,
    int* pEventCount)
{
    int ret = 0;
    int eventCount = 0;
    int i;
    SnifferServer* srv;

    LOCK_SERVER_LIST();

    /* Iterate the open sniffer sessions calling wolfSSL_CTX_AsyncPoll */
    srv = ServerList;
    while (srv) {
        int nMax = maxEvents - eventCount, nReady = 0;
        if (nMax <= 0) {
            break; /* out of room in events list */
        }
        ret = wolfSSL_CTX_AsyncPoll(srv->ctx, events + nReady, nMax, flags,
                                    &nReady);
        if (ret == 0) {
            eventCount += nReady;
        }
        else {
        #ifdef DEBUG_SNIFFER
            fprintf(stderr, "Sniffer Server %p: Poll error: %d\n", srv, ret);
        #endif
            break;
        }
        srv = srv->next;
    }

    UNLOCK_SERVER_LIST();


    /* iterate list and mark polled */
    LOCK_SESSION();
    for (i=0; i<eventCount; i++) {
        WOLFSSL* ssl = (WOLFSSL*)events[i]->context;
        SnifferSession* session = FindSession(ssl);
        if (session) {
            session->flags.wasPolled = 1;
            session->sslServer->error = events[i]->ret;
        }
    }
    UNLOCK_SESSION();

    *pEventCount = eventCount;

    return ret;
}
#endif

#undef ERROR_OUT

#endif /* WOLFSSL_SNIFFER */
#endif /* !WOLFCRYPT_ONLY && !NO_FILESYSTEM */
