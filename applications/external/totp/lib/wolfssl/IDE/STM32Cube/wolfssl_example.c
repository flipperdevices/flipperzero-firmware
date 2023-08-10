/* wolfssl_example.c
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

#include "wolfssl_example.h"

/* force certificate test buffers to be included via headers */
#undef  USE_CERT_BUFFERS_2048
#define USE_CERT_BUFFERS_2048
#undef  USE_CERT_BUFFERS_256
#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>

#include <wolfssl/wolfcrypt/hash.h> /* WC_MAX_DIGEST_SIZE */

#ifndef SINGLE_THREADED
    #include <cmsis_os.h>

    #ifdef WOLFSSL_DEBUG_MEMORY
        /* for memory debugging */
        #include <task.h>
    #endif
#endif

#include <stdio.h>
#include <string.h>

/*****************************************************************************
 * Configuration
 ****************************************************************************/


#if (!defined(NO_WOLFSSL_CLIENT) || !defined(NO_WOLFSSL_SERVER)) && \
    !defined(WOLFCRYPT_ONLY) && !defined(SINGLE_THREADED)
    #define ENABLE_TLS_BENCH
#endif

#if !defined(WOLFCRYPT_ONLY) && defined(WOLFSSL_TLS13) && !defined(NO_TLS_UART_TEST)
    #define ENABLE_TLS_UART
#endif

/* Defaults for configuration parameters */
#define BENCH_DEFAULT_HOST   "localhost"
#define BENCH_DEFAULT_PORT   11112
#define BENCH_RUNTIME_SEC    20
#define BENCH_SHOW_PEER_INFO 1
#define TEST_PACKET_SIZE     (2 * 1024)  /* TLS packet size */
#ifdef BENCH_EMBEDDED
#define TEST_MAX_SIZE        (4 * 1024)
#else
#define TEST_MAX_SIZE        (32 * 1024) /* Total bytes to benchmark */
#endif
/* Must be large enough to handle max packet size - TLS header MAX_MSG_EXTRA + MAX DIGEST */
#define MEM_BUFFER_SZ        (TEST_PACKET_SIZE + 38 + WC_MAX_DIGEST_SIZE)
/* make sure memory buffer size is large enough */
#if MEM_BUFFER_SZ < 2048
    #undef  MEM_BUFFER_SZ
    #define MEM_BUFFER_SZ 2048
#endif
#define SHOW_VERBOSE         0 /* 0=tab del (minimal), 1=info, 2=debug, 3=debug w/wolf logs */
#ifndef WOLFSSL_CIPHER_LIST_MAX_SIZE
#define WOLFSSL_CIPHER_LIST_MAX_SIZE 2048
#endif
#if 0
    /* define this to test only a specific cipher suite(s) (colon separated) */
    #define TEST_CIPHER_SUITE "ECDHE-RSA-AES256-GCM-SHA384:ECDHE-RSA-AES128-GCM-SHA256"
#endif
#if 0
    /* use non-blocking mode for read/write IO */
    #define BENCH_USE_NONBLOCK
#endif
#ifndef RECV_WAIT_TIMEOUT
    #define RECV_WAIT_TIMEOUT 10000
#endif

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

#ifdef WOLFSSL_STATIC_MEMORY
    #if 1 /* on-chip RAM */
        #define RAM_STATIC
    #else /* external RAM */
        /* requires .ld to be updated with ".extram" section */
        #define RAM_STATIC __attribute__ ((section (".extram")))
    #endif
    #define WOLF_GEN_MEM         (20*1024)
    #define WOLF_TLS_GEN_MEM     (90*1024)
    #define WOLF_TLS_IO_POOL_MEM (35*1024)

    RAM_STATIC static byte gWolfMem[WOLF_GEN_MEM];
    RAM_STATIC static byte gWolfCTXCli[WOLF_TLS_GEN_MEM];
    RAM_STATIC static byte gWolfIOCli[WOLF_TLS_IO_POOL_MEM];
    RAM_STATIC static byte gWolfCTXSrv[WOLF_TLS_GEN_MEM];
    RAM_STATIC static byte gWolfIOSrv[WOLF_TLS_IO_POOL_MEM];

    WOLFSSL_HEAP_HINT* HEAP_HINT = NULL;
#endif /* WOLFSSL_STATIC_MEMORY */


/* This sets which UART to use for the console.  It is something you will have
 * to configure in STMCubeIDE and then change here. */
#ifndef HAL_CONSOLE_UART
#define HAL_CONSOLE_UART huart4
#endif
extern UART_HandleTypeDef HAL_CONSOLE_UART;

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/
typedef struct func_args {
    int argc;
    char** argv;
    int return_code;
} func_args;

const char menu1[] = "\n"
        "\tt. wolfCrypt Test\n"
        "\tb. wolfCrypt Benchmark\n"
    #ifdef ENABLE_TLS_BENCH
        "\tl. wolfSSL TLS Bench\n"
    #endif
        "\te. Show Cipher List\n"
    #ifdef ENABLE_TLS_UART
        "\ts. Run TLS 1.3 Server over UART\n"
        "\tc. Run TLS 1.3 Client over UART\n"
    #endif
    ;

static void PrintMemStats(void);
double current_time(void);


#ifdef ENABLE_TLS_BENCH

static const char* kShutdown = "shutdown";

static const char* kTestStr =
"Biodiesel cupidatat marfa, cliche aute put a bird on it incididunt elit\n"
"polaroid. Sunt tattooed bespoke reprehenderit. Sint twee organic id\n"
"marfa. Commodo veniam ad esse gastropub. 3 wolf moon sartorial vero,\n"
"plaid delectus biodiesel squid +1 vice. Post-ironic keffiyeh leggings\n"
"selfies cray fap hoodie, forage anim. Carles cupidatat shoreditch, VHS\n"
"small batch meggings kogi dolore food truck bespoke gastropub.\n"
"\n"
"Terry richardson adipisicing actually typewriter tumblr, twee whatever\n"
"four loko you probably haven't heard of them high life. Messenger bag\n"
"whatever tattooed deep v mlkshk. Brooklyn pinterest assumenda chillwave\n"
"et, banksy ullamco messenger bag umami pariatur direct trade forage.\n"
"Typewriter culpa try-hard, pariatur sint brooklyn meggings. Gentrify\n"
"food truck next level, tousled irony non semiotics PBR ethical anim cred\n"
"readymade. Mumblecore brunch lomo odd future, portland organic terry\n"
"richardson elit leggings adipisicing ennui raw denim banjo hella. Godard\n"
"mixtape polaroid, pork belly readymade organic cray typewriter helvetica\n"
"four loko whatever street art yr farm-to-table.\n"
"\n"
"Vinyl keytar vice tofu. Locavore you probably haven't heard of them pug\n"
"pickled, hella tonx labore truffaut DIY mlkshk elit cosby sweater sint\n"
"et mumblecore. Elit swag semiotics, reprehenderit DIY sartorial nisi ugh\n"
"nesciunt pug pork belly wayfarers selfies delectus. Ethical hoodie\n"
"seitan fingerstache kale chips. Terry richardson artisan williamsburg,\n"
"eiusmod fanny pack irony tonx ennui lo-fi incididunt tofu YOLO\n"
"readymade. 8-bit sed ethnic beard officia. Pour-over iphone DIY butcher,\n"
"ethnic art party qui letterpress nisi proident jean shorts mlkshk\n"
"locavore.\n"
"\n"
"Narwhal flexitarian letterpress, do gluten-free voluptate next level\n"
"banh mi tonx incididunt carles DIY. Odd future nulla 8-bit beard ut\n"
"cillum pickled velit, YOLO officia you probably haven't heard of them\n"
"trust fund gastropub. Nisi adipisicing tattooed, Austin mlkshk 90's\n"
"small batch american apparel. Put a bird on it cosby sweater before they\n"
"sold out pork belly kogi hella. Street art mollit sustainable polaroid,\n"
"DIY ethnic ea pug beard dreamcatcher cosby sweater magna scenester nisi.\n"
"Sed pork belly skateboard mollit, labore proident eiusmod. Sriracha\n"
"excepteur cosby sweater, anim deserunt laborum eu aliquip ethical et\n"
"neutra PBR selvage.\n"
"\n"
"Raw denim pork belly truffaut, irony plaid sustainable put a bird on it\n"
"next level jean shorts exercitation. Hashtag keytar whatever, nihil\n"
"authentic aliquip disrupt laborum. Tattooed selfies deserunt trust fund\n"
"wayfarers. 3 wolf moon synth church-key sartorial, gastropub leggings\n"
"tattooed. Labore high life commodo, meggings raw denim fingerstache pug\n"
"trust fund leggings seitan forage. Nostrud ullamco duis, reprehenderit\n"
"incididunt flannel sustainable helvetica pork belly pug banksy you\n"
"probably haven't heard of them nesciunt farm-to-table. Disrupt nostrud\n"
"mollit magna, sriracha sartorial helvetica.\n"
"\n"
"Nulla kogi reprehenderit, skateboard sustainable duis adipisicing viral\n"
"ad fanny pack salvia. Fanny pack trust fund you probably haven't heard\n"
"of them YOLO vice nihil. Keffiyeh cray lo-fi pinterest cardigan aliqua,\n"
"reprehenderit aute. Culpa tousled williamsburg, marfa lomo actually anim\n"
"skateboard. Iphone aliqua ugh, semiotics pariatur vero readymade\n"
"organic. Marfa squid nulla, in laborum disrupt laboris irure gastropub.\n"
"Veniam sunt food truck leggings, sint vinyl fap.\n"
"\n"
"Hella dolore pork belly, truffaut carles you probably haven't heard of\n"
"them PBR helvetica in sapiente. Fashion axe ugh bushwick american\n"
"apparel. Fingerstache sed iphone, jean shorts blue bottle nisi bushwick\n"
"flexitarian officia veniam plaid bespoke fap YOLO lo-fi. Blog\n"
"letterpress mumblecore, food truck id cray brooklyn cillum ad sed.\n"
"Assumenda chambray wayfarers vinyl mixtape sustainable. VHS vinyl\n"
"delectus, culpa williamsburg polaroid cliche swag church-key synth kogi\n"
"magna pop-up literally. Swag thundercats ennui shoreditch vegan\n"
"pitchfork neutra truffaut etsy, sed single-origin coffee craft beer.\n"
"\n"
"Odio letterpress brooklyn elit. Nulla single-origin coffee in occaecat\n"
"meggings. Irony meggings 8-bit, chillwave lo-fi adipisicing cred\n"
"dreamcatcher veniam. Put a bird on it irony umami, trust fund bushwick\n"
"locavore kale chips. Sriracha swag thundercats, chillwave disrupt\n"
"tousled beard mollit mustache leggings portland next level. Nihil esse\n"
"est, skateboard art party etsy thundercats sed dreamcatcher ut iphone\n"
"swag consectetur et. Irure skateboard banjo, nulla deserunt messenger\n"
"bag dolor terry richardson sapiente.\n";

#if !defined(NO_DH)
#define MIN_DHKEY_BITS      1024

/* dh2048 p */
static const unsigned char p[] =
{
    0xb0, 0xa1, 0x08, 0x06, 0x9c, 0x08, 0x13, 0xba, 0x59, 0x06, 0x3c, 0xbc, 0x30,
    0xd5, 0xf5, 0x00, 0xc1, 0x4f, 0x44, 0xa7, 0xd6, 0xef, 0x4a, 0xc6, 0x25, 0x27,
    0x1c, 0xe8, 0xd2, 0x96, 0x53, 0x0a, 0x5c, 0x91, 0xdd, 0xa2, 0xc2, 0x94, 0x84,
    0xbf, 0x7d, 0xb2, 0x44, 0x9f, 0x9b, 0xd2, 0xc1, 0x8a, 0xc5, 0xbe, 0x72, 0x5c,
    0xa7, 0xe7, 0x91, 0xe6, 0xd4, 0x9f, 0x73, 0x07, 0x85, 0x5b, 0x66, 0x48, 0xc7,
    0x70, 0xfa, 0xb4, 0xee, 0x02, 0xc9, 0x3d, 0x9a, 0x4a, 0xda, 0x3d, 0xc1, 0x46,
    0x3e, 0x19, 0x69, 0xd1, 0x17, 0x46, 0x07, 0xa3, 0x4d, 0x9f, 0x2b, 0x96, 0x17,
    0x39, 0x6d, 0x30, 0x8d, 0x2a, 0xf3, 0x94, 0xd3, 0x75, 0xcf, 0xa0, 0x75, 0xe6,
    0xf2, 0x92, 0x1f, 0x1a, 0x70, 0x05, 0xaa, 0x04, 0x83, 0x57, 0x30, 0xfb, 0xda,
    0x76, 0x93, 0x38, 0x50, 0xe8, 0x27, 0xfd, 0x63, 0xee, 0x3c, 0xe5, 0xb7, 0xc8,
    0x09, 0xae, 0x6f, 0x50, 0x35, 0x8e, 0x84, 0xce, 0x4a, 0x00, 0xe9, 0x12, 0x7e,
    0x5a, 0x31, 0xd7, 0x33, 0xfc, 0x21, 0x13, 0x76, 0xcc, 0x16, 0x30, 0xdb, 0x0c,
    0xfc, 0xc5, 0x62, 0xa7, 0x35, 0xb8, 0xef, 0xb7, 0xb0, 0xac, 0xc0, 0x36, 0xf6,
    0xd9, 0xc9, 0x46, 0x48, 0xf9, 0x40, 0x90, 0x00, 0x2b, 0x1b, 0xaa, 0x6c, 0xe3,
    0x1a, 0xc3, 0x0b, 0x03, 0x9e, 0x1b, 0xc2, 0x46, 0xe4, 0x48, 0x4e, 0x22, 0x73,
    0x6f, 0xc3, 0x5f, 0xd4, 0x9a, 0xd6, 0x30, 0x07, 0x48, 0xd6, 0x8c, 0x90, 0xab,
    0xd4, 0xf6, 0xf1, 0xe3, 0x48, 0xd3, 0x58, 0x4b, 0xa6, 0xb9, 0xcd, 0x29, 0xbf,
    0x68, 0x1f, 0x08, 0x4b, 0x63, 0x86, 0x2f, 0x5c, 0x6b, 0xd6, 0xb6, 0x06, 0x65,
    0xf7, 0xa6, 0xdc, 0x00, 0x67, 0x6b, 0xbb, 0xc3, 0xa9, 0x41, 0x83, 0xfb, 0xc7,
    0xfa, 0xc8, 0xe2, 0x1e, 0x7e, 0xaf, 0x00, 0x3f, 0x93
};

/* dh2048 g */
static const unsigned char g[] =
{
    0x02,
};
#endif /* !NO_DH */

typedef struct {
    unsigned char buf[MEM_BUFFER_SZ];
    int write_bytes;
    int write_idx;
    int read_bytes;
    int read_idx;
} memBuf_t;

typedef struct {
    double connTime;
    double rxTime;
    double txTime;
    int connCount;
    int rxTotal;
    int txTotal;
} stats_t;

typedef struct {
    int ret;

    osThreadId threadId;
#ifdef CMSIS_OS2_H_
    osSemaphoreId_t mutex;
#else
    osThreadDef_t threadDef;
    osSemaphoreDef_t mutexDef;
    osSemaphoreId mutex;
#endif

    byte shutdown:1;
    byte done:1;
} side_t;

typedef struct {
    const char* cipher;
    const char* host;
    word32 port;
    int packetSize; /* The data payload size in the packet */
    int maxSize;
    int runTimeSec;
    int showPeerInfo;
    int showVerbose;
    int doShutdown;

    side_t client;
    side_t server;

    /* client messages to server in memory */
    memBuf_t to_server;

    /* server messages to client in memory */
    memBuf_t to_client;

    /* server */
    stats_t server_stats;

    /* client */
    stats_t client_stats;
} info_t;


/*****************************************************************************
 * Private functions
 ****************************************************************************/
static double gettime_secs(int reset)
{
    return current_time();
}

static void PrintTlsStats(stats_t* wcStat, const char* desc, const char* cipher, int verbose)
{
    const char* formatStr;

    if (verbose) {
        formatStr = "wolfSSL %s Benchmark on %s:\n"
               "\tTotal       : %9d bytes\n"
               "\tNum Conns   : %9d\n"
               "\tRx Total    : %9.3f ms\n"
               "\tTx Total    : %9.3f ms\n"
               "\tRx          : %9.3f MB/s\n"
               "\tTx          : %9.3f MB/s\n"
               "\tConnect     : %9.3f ms\n"
               "\tConnect Avg : %9.3f ms\n";
    }
    else {
        formatStr = "%-6s  %-33s  %11d  %9d  %9.3f  %9.3f  %9.3f  %9.3f  %17.3f  %15.3f\n";
    }

    printf(formatStr,
           desc,
           cipher,
           wcStat->txTotal + wcStat->rxTotal,
           wcStat->connCount,
           wcStat->rxTime * 1000,
           wcStat->txTime * 1000,
           wcStat->rxTotal / wcStat->rxTime / 1024 / 1024,
           wcStat->txTotal / wcStat->txTime / 1024 / 1024,
           wcStat->connTime * 1000,
           wcStat->connTime * 1000 / wcStat->connCount);
}
#endif /* ENABLE_TLS_BENCH */


#if defined(ENABLE_TLS_BENCH) || defined(ENABLE_TLS_UART)
#if defined(KEEP_PEER_CERT) || defined(KEEP_OUR_CERT)
static const char* client_showx509_msg[] = {
    "issuer",
    "subject",
    "altname",
    "serial number",
    NULL
};

static void ShowX509(WOLFSSL_X509* x509, const char* hdr)
{
    char* altName;
    char* issuer;
    char* subject;
    byte  serial[32];
    int   ret;
    int   sz = sizeof(serial);
    const char** words = client_showx509_msg;

    if (x509 == NULL) {
        printf("%s No Cert\n", hdr);
        return;
    }

    issuer  = wolfSSL_X509_NAME_oneline(
                                      wolfSSL_X509_get_issuer_name(x509), 0, 0);
    subject = wolfSSL_X509_NAME_oneline(
                                     wolfSSL_X509_get_subject_name(x509), 0, 0);

    printf("%s\n %s : %s\n %s: %s\n", hdr, words[0], issuer, words[1], subject);

    while ( (altName = wolfSSL_X509_get_next_altname(x509)) != NULL)
        printf(" %s = %s\n", words[2], altName);

    ret = wolfSSL_X509_get_serial_number(x509, serial, &sz);
    if (ret == WOLFSSL_SUCCESS) {
        int  i;
        int  strLen;
        char serialMsg[80];

        /* testsuite has multiple threads writing to stdout, get output
           message ready to write once */
        strLen = sprintf(serialMsg, " %s", words[3]);
        for (i = 0; i < sz; i++)
            sprintf(serialMsg + strLen + (i*3), ":%02x ", serial[i]);
        printf("%s\n", serialMsg);
    }

    XFREE(subject, 0, DYNAMIC_TYPE_OPENSSL);
    XFREE(issuer,  0, DYNAMIC_TYPE_OPENSSL);

#if defined(OPENSSL_EXTRA)
    {
        WOLFSSL_BIO* bio;
        char buf[256]; /* should be size of ASN_NAME_MAX */
        int  textSz;

        /* print out domain component if certificate has it */
        textSz = wolfSSL_X509_NAME_get_text_by_NID(
                wolfSSL_X509_get_subject_name(x509), NID_domainComponent,
                buf, sizeof(buf));
        if (textSz > 0) {
            printf("Domain Component = %s\n", buf);
        }

        bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
        if (bio != NULL) {
            wolfSSL_BIO_set_fp(bio, stdout, BIO_NOCLOSE);
            wolfSSL_X509_print(bio, x509);
            wolfSSL_BIO_free(bio);
        }
    }
#endif /* OPENSSL_EXTRA */
}
#endif /* KEEP_PEER_CERT || KEEP_OUR_CERT */


static const char* client_showpeer_msg[] = {
    "SSL version is",
    "SSL cipher suite is",
    "SSL curve name is",
    "SSL DH size is",
    "SSL reused session",
    "Alternate cert chain used",
    "peer's cert info:",
    NULL
};

static void ShowPeer(WOLFSSL* ssl)
{
    WOLFSSL_CIPHER* cipher;
    const char** words = client_showpeer_msg;
#if defined(HAVE_ECC) || !defined(NO_DH)
    const char *name;
#endif
#ifndef NO_DH
    int bits;
#endif
#ifdef KEEP_PEER_CERT
    WOLFSSL_X509* peer = wolfSSL_get_peer_certificate(ssl);
    if (peer)
        ShowX509(peer, words[6]);
    else
        printf("peer has no cert!\n");
    wolfSSL_FreeX509(peer);
#endif
#if defined(KEEP_OUR_CERT) && \
    (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL))
    ShowX509(wolfSSL_get_certificate(ssl), "our cert info:");
    printf("Peer verify result = %lu\n", wolfSSL_get_verify_result(ssl));
#endif /* SHOW_CERTS && KEEP_OUR_CERT */
    printf("%s %s\n", words[0], wolfSSL_get_version(ssl));

    cipher = wolfSSL_get_current_cipher(ssl);
    printf("%s %s\n", words[1], wolfSSL_CIPHER_get_name(cipher));
#if defined(HAVE_ECC) || !defined(NO_DH)
    if ((name = wolfSSL_get_curve_name(ssl)) != NULL)
        printf("%s %s\n", words[2], name);
#endif
#ifndef NO_DH
    else if ((bits = wolfSSL_GetDhKey_Sz(ssl)) > 0)
        printf("%s %d bits\n", words[3], bits);
#endif
    if (wolfSSL_session_reused(ssl))
        printf("%s\n", words[4]);
#ifdef WOLFSSL_ALT_CERT_CHAINS
    if (wolfSSL_is_peer_alt_cert_chain(ssl))
        printf("%s\n", words[5]);
#endif

  (void)ssl;
}
#endif /* ENABLE_TLS_BENCH || ENABLE_TLS_UART */

#ifdef ENABLE_TLS_BENCH

/* server send callback */
static int ServerMemSend(info_t* info, char* buf, int sz)
{
#ifdef CMSIS_OS2_H_
    osSemaphoreAcquire(info->client.mutex, osWaitForever);
#else
    osSemaphoreWait(info->client.mutex, osWaitForever);
#endif

#ifndef BENCH_USE_NONBLOCK
    /* check for overflow */
    if (info->to_client.write_idx + sz > MEM_BUFFER_SZ) {
        osSemaphoreRelease(info->client.mutex);
        printf("ServerMemSend overflow\n");
        return -1;
    }
#else
    if (info->to_client.write_idx + sz > MEM_BUFFER_SZ)
        sz = MEM_BUFFER_SZ - info->to_client.write_idx;
#endif

    if (info->showVerbose >= 2)
        printf("Server Send: %d\n", sz);
    XMEMCPY(&info->to_client.buf[info->to_client.write_idx], buf, sz);
    info->to_client.write_idx += sz;
    info->to_client.write_bytes += sz;

#ifdef CMSIS_OS2_H_
    osThreadFlagsSet(info->client.threadId, 1);
#else
    osSignalSet(info->client.threadId, 1);
#endif
    osSemaphoreRelease(info->client.mutex);

#ifdef BENCH_USE_NONBLOCK
    if (sz == 0)
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
#endif
    return sz;
}

/* server recv callback */
static int ServerMemRecv(info_t* info, char* buf, int sz)
{
#ifdef CMSIS_OS2_H_
    osSemaphoreAcquire(info->server.mutex, osWaitForever);
#else
    osSemaphoreWait(info->server.mutex, osWaitForever);
#endif

#ifndef BENCH_USE_NONBLOCK
    while (info->to_server.write_idx - info->to_server.read_idx < sz &&
            !info->client.done) {
        osSemaphoreRelease(info->server.mutex);
#ifdef CMSIS_OS2_H_
        if (osThreadFlagsWait(1, osFlagsWaitAny, RECV_WAIT_TIMEOUT) == osFlagsErrorTimeout) {
            printf("Server Recv: Timeout!\n");
            return WOLFSSL_CBIO_ERR_TIMEOUT;
        }
        osSemaphoreAcquire(info->server.mutex, osWaitForever);
#else
        if (osSignalWait(1, RECV_WAIT_TIMEOUT).status == osEventTimeout) {
            printf("Server Recv: Timeout!\n");
            return WOLFSSL_CBIO_ERR_TIMEOUT;
        }
        osSemaphoreWait(info->server.mutex, osWaitForever);
#endif
    }
#else
    if (info->to_server.write_idx - info->to_server.read_idx < sz)
        sz = info->to_server.write_idx - info->to_server.read_idx;
#endif

    XMEMCPY(buf, &info->to_server.buf[info->to_server.read_idx], sz);
    info->to_server.read_idx += sz;
    info->to_server.read_bytes += sz;

    /* if the rx has caught up with pending then reset buffer positions */
    if (info->to_server.read_bytes == info->to_server.write_bytes) {
        info->to_server.read_bytes = info->to_server.read_idx = 0;
        info->to_server.write_bytes = info->to_server.write_idx = 0;
    }
    if (info->showVerbose >= 2)
        printf("Server Recv: %d\n", sz);

    osSemaphoreRelease(info->server.mutex);


#ifdef BENCH_USE_NONBLOCK
    if (sz == 0)
        return WOLFSSL_CBIO_ERR_WANT_READ;
#endif
    return sz;
}

/* client send callback */
static int ClientMemSend(info_t* info, char* buf, int sz)
{
#ifdef CMSIS_OS2_H_
    osSemaphoreAcquire(info->server.mutex, osWaitForever);
#else
    osSemaphoreWait(info->server.mutex, osWaitForever);
#endif

#ifndef BENCH_USE_NONBLOCK
    /* check for overflow */
    if (info->to_server.write_idx + sz > MEM_BUFFER_SZ) {
        printf("ClientMemSend overflow %d %d %d\n",
            info->to_server.write_idx, sz, MEM_BUFFER_SZ);
        osSemaphoreRelease(info->server.mutex);
        return -1;
    }
#else
    if (info->to_server.write_idx + sz > MEM_BUFFER_SZ)
        sz = MEM_BUFFER_SZ - info->to_server.write_idx;
#endif

    if (info->showVerbose >= 2)
        printf("Client Send: %d\n", sz);
    XMEMCPY(&info->to_server.buf[info->to_server.write_idx], buf, sz);
    info->to_server.write_idx += sz;
    info->to_server.write_bytes += sz;

#ifdef CMSIS_OS2_H_
    osThreadFlagsSet(info->server.threadId, 1);
#else
    osSignalSet(info->server.threadId, 1);
#endif
    osSemaphoreRelease(info->server.mutex);

#ifdef BENCH_USE_NONBLOCK
    if (sz == 0)
        return WOLFSSL_CBIO_ERR_WANT_WRITE;
#endif
    return sz;
}

/* client recv callback */
static int ClientMemRecv(info_t* info, char* buf, int sz)
{
#ifdef CMSIS_OS2_H_
    osSemaphoreAcquire(info->client.mutex, osWaitForever);
#else
    osSemaphoreWait(info->client.mutex, osWaitForever);
#endif

#ifndef BENCH_USE_NONBLOCK
    while (info->to_client.write_idx - info->to_client.read_idx < sz &&
            !info->server.done) {
        osSemaphoreRelease(info->client.mutex);
#ifdef CMSIS_OS2_H_
        if (osThreadFlagsWait(1, osFlagsWaitAny, RECV_WAIT_TIMEOUT) == osFlagsErrorTimeout) {
            printf("Client Recv: Timeout!\n");
            return WOLFSSL_CBIO_ERR_TIMEOUT;
        }
        osSemaphoreAcquire(info->client.mutex, osWaitForever);
#else
        if (osSignalWait(1, RECV_WAIT_TIMEOUT).status == osEventTimeout) {
            printf("Client Recv: Timeout!\n");
            return WOLFSSL_CBIO_ERR_TIMEOUT;
        }
        osSemaphoreWait(info->client.mutex, osWaitForever);
#endif
    }
#else
    if (info->to_client.write_idx - info->to_client.read_idx < sz)
        sz = info->to_client.write_idx - info->to_client.read_idx;
#endif

    XMEMCPY(buf, &info->to_client.buf[info->to_client.read_idx], sz);
    info->to_client.read_idx += sz;
    info->to_client.read_bytes += sz;

    /* if the rx has caught up with pending then reset buffer positions */
    if (info->to_client.read_bytes == info->to_client.write_bytes) {
        info->to_client.read_bytes = info->to_client.read_idx = 0;
        info->to_client.write_bytes = info->to_client.write_idx = 0;
    }
    if (info->showVerbose >= 2)
        printf("Client Recv: %d\n", sz);

    osSemaphoreRelease(info->client.mutex);

#ifdef BENCH_USE_NONBLOCK
    if (sz == 0)
        return WOLFSSL_CBIO_ERR_WANT_READ;
#endif
    return sz;
}

static int ServerSend(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    info_t* info = (info_t*)ctx;
    (void)ssl;

    return ServerMemSend(info, buf, sz);
}
static int ServerRecv(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    info_t* info = (info_t*)ctx;
    (void)ssl;
    return ServerMemRecv(info, buf, sz);
}

static int ClientSend(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    info_t* info = (info_t*)ctx;
    (void)ssl;
    return ClientMemSend(info, buf, sz);
}
static int ClientRecv(WOLFSSL* ssl, char* buf, int sz, void* ctx)
{
    info_t* info = (info_t*)ctx;
    (void)ssl;
    return ClientMemRecv(info, buf, sz);
}


static int bench_tls_client(info_t* info)
{
    byte *writeBuf = NULL, *readBuf = NULL;
    double start, total = 0;
    int ret = 0, readBufSz;
    WOLFSSL_CTX* cli_ctx = NULL;
    WOLFSSL* cli_ssl = NULL;
    int haveShownPeerInfo = 0;
    int tls13 = XSTRNCMP(info->cipher, "TLS13", 5) == 0;
    int total_sz;

    total = gettime_secs(0);

    /* set up client */
#ifdef WOLFSSL_TLS13
    if (tls13) {
    #ifdef WOLFSSL_STATIC_MEMORY
        ret = wolfSSL_CTX_load_static_memory(&cli_ctx, wolfTLSv1_3_client_method_ex,
            gWolfCTXCli, sizeof(gWolfCTXCli), WOLFMEM_GENERAL , 10);
    #else
        cli_ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method());
    #endif
    }
#endif
    if (!tls13) {
#if !defined(WOLFSSL_TLS13)
    #ifdef WOLFSSL_STATIC_MEMORY
        ret = wolfSSL_CTX_load_static_memory(&cli_ctx, wolfSSLv23_client_method_ex,
            gWolfCTXCli, sizeof(gWolfCTXCli), WOLFMEM_GENERAL , 10);
    #else
        cli_ctx = wolfSSL_CTX_new(wolfSSLv23_client_method());
    #endif
#elif !defined(WOLFSSL_NO_TLS12)
    #ifdef WOLFSSL_STATIC_MEMORY
        ret = wolfSSL_CTX_load_static_memory(&cli_ctx, wolfTLSv1_2_client_method_ex,
            gWolfCTXCli, sizeof(gWolfCTXCli), WOLFMEM_GENERAL , 10);
    #else
        cli_ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
    #endif
#endif
    }

    if (cli_ctx == NULL || ret != 0) {
        printf("error creating ctx: ret %d\n", ret);
        ret = MEMORY_E; goto exit;
    }

#ifdef WOLFSSL_STATIC_MEMORY
    ret = wolfSSL_CTX_load_static_memory(&cli_ctx, 0, gWolfIOCli, sizeof(gWolfIOCli),
        WOLFMEM_IO_POOL, 10 );
#endif

#ifndef NO_CERTS
#ifdef HAVE_ECC
    if (XSTRSTR(info->cipher, "ECDSA")) {
        ret = wolfSSL_CTX_load_verify_buffer(cli_ctx, ca_ecc_cert_der_256,
            sizeof_ca_ecc_cert_der_256, WOLFSSL_FILETYPE_ASN1);
    }
    else
#endif
    {
        ret = wolfSSL_CTX_load_verify_buffer(cli_ctx, ca_cert_der_2048,
            sizeof_ca_cert_der_2048, WOLFSSL_FILETYPE_ASN1);
    }
    if (ret != WOLFSSL_SUCCESS) {
        printf("error loading CA\n");
        goto exit;
    }
#endif /* !NO_CERTS */

    wolfSSL_CTX_SetIOSend(cli_ctx, ClientSend);
    wolfSSL_CTX_SetIORecv(cli_ctx, ClientRecv);

    /* set cipher suite */
    ret = wolfSSL_CTX_set_cipher_list(cli_ctx, info->cipher);
    if (ret != WOLFSSL_SUCCESS) {
        printf("error setting cipher suite\n");
        goto exit;
    }

#ifndef NO_DH
    ret = wolfSSL_CTX_SetMinDhKey_Sz(cli_ctx, MIN_DHKEY_BITS);
    if (ret != WOLFSSL_SUCCESS) {
        printf("Error setting minimum DH key size\n");
        goto exit;
    }
#endif /* !NO_DH */

    /* Allocate and initialize a packet sized buffer */
    writeBuf = (unsigned char*)XMALLOC(info->packetSize, NULL,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (writeBuf == NULL) {
        printf("failed to allocate write memory\n");
        ret = MEMORY_E; goto exit;
    }

    /* Allocate read buffer */
    readBufSz = info->packetSize;
    readBuf = (unsigned char*)XMALLOC(readBufSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (readBuf == NULL) {
        printf("failed to allocate read memory\n");
        ret = MEMORY_E; goto exit;
    }

    /* BENCHMARK CONNECTIONS LOOP */
    while (!info->client.shutdown) {
        int writeSz = info->packetSize;
    #ifdef BENCH_USE_NONBLOCK
        int err;
    #endif

        cli_ssl = wolfSSL_new(cli_ctx);
        if (cli_ssl == NULL) {
            printf("error creating client object\n");
            ret = MEMORY_E; goto exit;
        }

        wolfSSL_SetIOReadCtx(cli_ssl, info);
        wolfSSL_SetIOWriteCtx(cli_ssl, info);

        /* perform connect */
        start = gettime_secs(1);
    #ifndef BENCH_USE_NONBLOCK
        ret = wolfSSL_connect(cli_ssl);
    #else
        do
        {
            ret = wolfSSL_connect(cli_ssl);
            err = wolfSSL_get_error(cli_ssl, ret);
        }
        while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    #endif
        start = gettime_secs(0) - start;
        if (ret != WOLFSSL_SUCCESS) {
            ret = wolfSSL_get_error(cli_ssl, ret);
            printf("error %d connecting client\n", ret);
            goto exit;
        }
        info->client_stats.connTime += start;
        info->client_stats.connCount++;

        if ((info->showPeerInfo) && (!haveShownPeerInfo)) {
            haveShownPeerInfo = 1;
            ShowPeer(cli_ssl);
        }

        /* check for run time completion and issue shutdown */
        if (gettime_secs(0) - total >= info->runTimeSec) {
            info->client.shutdown = 1;

            writeSz = (int)XSTRLEN(kShutdown) + 1;
            XMEMCPY(writeBuf, kShutdown, writeSz); /* include null term */
            if (info->showVerbose) {
                printf("Sending shutdown\n");
            }

            ret = wolfSSL_write(cli_ssl, writeBuf, writeSz);
            if (ret < 0) {
                ret = wolfSSL_get_error(cli_ssl, ret);
                printf("error %d on client write\n", ret);
                goto exit;
            }
        }
        else {
            XMEMSET(writeBuf, 0, info->packetSize);
            XSTRNCPY((char*)writeBuf, kTestStr, info->packetSize);
        }

        /* write / read echo loop */
        ret = 0;
        total_sz = 0;
        while (ret == 0 && total_sz < info->maxSize && !info->client.shutdown) {
            /* write test message to server */
            start = gettime_secs(1);
        #ifndef BENCH_USE_NONBLOCK
            ret = wolfSSL_write(cli_ssl, writeBuf, writeSz);
        #else
            do {
                ret = wolfSSL_write(cli_ssl, writeBuf, writeSz);
                err = wolfSSL_get_error(cli_ssl, ret);
            }
            while (err == WOLFSSL_ERROR_WANT_WRITE);
        #endif
            info->client_stats.txTime += gettime_secs(0) - start;
            if (ret < 0) {
                ret = wolfSSL_get_error(cli_ssl, ret);
                printf("error %d on client write\n", ret);
                goto exit;
            }
            info->client_stats.txTotal += ret;
            total_sz += ret;

            /* read echo of message from server */
            XMEMSET(readBuf, 0, readBufSz);
            start = gettime_secs(1);
        #ifndef BENCH_USE_NONBLOCK
            ret = wolfSSL_read(cli_ssl, readBuf, readBufSz);
        #else
            do {
                ret = wolfSSL_read(cli_ssl, readBuf, readBufSz);
                err = wolfSSL_get_error(cli_ssl, ret);
            }
            while (err == WOLFSSL_ERROR_WANT_READ);
        #endif
            info->client_stats.rxTime += gettime_secs(0) - start;
            if (ret < 0) {
                ret = wolfSSL_get_error(cli_ssl, ret);
                printf("error %d on client read\n", ret);
                goto exit;
            }
            info->client_stats.rxTotal += ret;
            ret = 0; /* reset return code */

            /* validate echo */
            if (XMEMCMP((char*)writeBuf, (char*)readBuf, writeSz) != 0) {
                printf("echo check failed!\n");
                goto exit;
            }
        }

        wolfSSL_free(cli_ssl);
        cli_ssl = NULL;
    }

exit:

    if (ret != 0 && ret != WOLFSSL_SUCCESS) {
        info->doShutdown = 1;
        printf("Client Error: %d (%s)\n", ret,
            wolfSSL_ERR_reason_error_string(ret));
    }

    /* cleanup */
    if (cli_ssl != NULL)
        wolfSSL_free(cli_ssl);
    if (cli_ctx != NULL)
        wolfSSL_CTX_free(cli_ctx);
    XFREE(readBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(writeBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    info->client.ret = ret;

    return ret;
}

#ifdef CMSIS_OS2_H_
static void client_thread(void* args)
#else
static void client_thread(const void* args)
#endif
{
    int ret;
    info_t* info = (info_t*)args;

#ifdef CMSIS_OS2_H_
    info->client.threadId = osThreadGetId();
#endif
    do {
        ret = bench_tls_client(info);

        /* signal server */
        if (!info->server.done && info->server.threadId != 0) {
#ifdef CMSIS_OS2_H_
            osThreadFlagsSet(info->server.threadId, 1);
#else
            osSignalSet(info->server.threadId, 1);
#endif
        }
        info->client.ret = ret;
        info->client.done = 1;
        osThreadSuspend(info->client.threadId);

        if (info->doShutdown)
            info->server.done = 1;
    } while (!info->doShutdown);

    osThreadTerminate(info->client.threadId);
    info->client.threadId = NULL;
}

static int bench_tls_server(info_t* info)
{
    byte *readBuf = NULL;
    double start;
    int ret = 0, len = 0, readBufSz;
    WOLFSSL_CTX* srv_ctx = NULL;
    WOLFSSL* srv_ssl = NULL;
    int tls13 = XSTRNCMP(info->cipher, "TLS13", 5) == 0;
    int total_sz;

    /* set up server */
#ifdef WOLFSSL_TLS13
    if (tls13) {
    #ifdef WOLFSSL_STATIC_MEMORY
        ret = wolfSSL_CTX_load_static_memory(&srv_ctx, wolfTLSv1_3_server_method_ex,
            gWolfCTXSrv, sizeof(gWolfCTXSrv), WOLFMEM_GENERAL , 10);
    #else
        srv_ctx = wolfSSL_CTX_new(wolfTLSv1_3_server_method());
    #endif
    }
#endif
    if (!tls13) {
#if !defined(WOLFSSL_TLS13)
    #ifdef WOLFSSL_STATIC_MEMORY
        ret = wolfSSL_CTX_load_static_memory(&srv_ctx, wolfSSLv23_server_method_ex,
            gWolfCTXSrv, sizeof(gWolfCTXSrv), WOLFMEM_GENERAL , 10);
    #else
        srv_ctx = wolfSSL_CTX_new(wolfSSLv23_server_method());
    #endif
#elif !defined(WOLFSSL_NO_TLS12)
    #ifdef WOLFSSL_STATIC_MEMORY
        ret = wolfSSL_CTX_load_static_memory(&srv_ctx, wolfTLSv1_2_server_method_ex,
            gWolfCTXSrv, sizeof(gWolfCTXSrv), WOLFMEM_GENERAL , 10);
    #else
        srv_ctx = wolfSSL_CTX_new(wolfTLSv1_2_server_method());
    #endif
#endif
    }

    if (srv_ctx == NULL || ret != 0) {
        printf("error creating server ctx: ret %d\n", ret);
        ret = MEMORY_E; goto exit;
    }

#ifdef WOLFSSL_STATIC_MEMORY
    ret = wolfSSL_CTX_load_static_memory(&srv_ctx, 0, gWolfIOSrv, sizeof(gWolfIOSrv),
        WOLFMEM_IO_POOL, 10 );
#endif

#ifndef NO_CERTS
#ifdef HAVE_ECC
    if (XSTRSTR(info->cipher, "ECDSA")) {
        ret = wolfSSL_CTX_use_PrivateKey_buffer(srv_ctx, ecc_key_der_256,
            sizeof_ecc_key_der_256, WOLFSSL_FILETYPE_ASN1);
    }
    else
#endif
    {
        ret = wolfSSL_CTX_use_PrivateKey_buffer(srv_ctx, server_key_der_2048,
            sizeof_server_key_der_2048, WOLFSSL_FILETYPE_ASN1);
    }
    if (ret != WOLFSSL_SUCCESS) {
        printf("error loading server key\n");
        goto exit;
    }

#ifdef HAVE_ECC
    if (XSTRSTR(info->cipher, "ECDSA")) {
        ret = wolfSSL_CTX_use_certificate_buffer(srv_ctx, serv_ecc_der_256,
            sizeof_serv_ecc_der_256, WOLFSSL_FILETYPE_ASN1);
    }
    else
#endif
    {
        ret = wolfSSL_CTX_use_certificate_buffer(srv_ctx, server_cert_der_2048,
            sizeof_server_cert_der_2048, WOLFSSL_FILETYPE_ASN1);
    }
    if (ret != WOLFSSL_SUCCESS) {
        printf("error loading server cert\n");
        goto exit;
    }
#endif /* !NO_CERTS */

    wolfSSL_CTX_SetIOSend(srv_ctx, ServerSend);
    wolfSSL_CTX_SetIORecv(srv_ctx, ServerRecv);

    /* set cipher suite */
    ret = wolfSSL_CTX_set_cipher_list(srv_ctx, info->cipher);
    if (ret != WOLFSSL_SUCCESS) {
        printf("error setting cipher suite\n");
        goto exit;
    }

#ifndef NO_DH
    ret = wolfSSL_CTX_SetMinDhKey_Sz(srv_ctx, MIN_DHKEY_BITS);
    if (ret != WOLFSSL_SUCCESS) {
        printf("Error setting minimum DH key size\n");
        goto exit;
    }
#endif

    /* Allocate read buffer */
    readBufSz = info->packetSize;
    readBuf = (unsigned char*)XMALLOC(readBufSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (readBuf == NULL) {
        printf("failed to allocate read memory\n");
        ret = MEMORY_E; goto exit;
    }

    /* BENCHMARK CONNECTIONS LOOP */
    while (!info->server.shutdown) {
    #ifdef BENCH_USE_NONBLOCK
        int err;
    #endif

    #ifdef BENCH_USE_NONBLOCK
        if (ret == -2) {
            osDelay(0);
            continue;
        }
    #endif

        srv_ssl = wolfSSL_new(srv_ctx);
        if (srv_ssl == NULL) {
            printf("error creating server object\n");
            ret = MEMORY_E; goto exit;
        }

        wolfSSL_SetIOReadCtx(srv_ssl, info);
        wolfSSL_SetIOWriteCtx(srv_ssl, info);

    #ifndef NO_DH
        wolfSSL_SetTmpDH(srv_ssl, p, sizeof(p), g, sizeof(g));
    #endif

        /* accept TLS connection */
        start = gettime_secs(1);
    #ifndef BENCH_USE_NONBLOCK
        ret = wolfSSL_accept(srv_ssl);
    #else
        do {
            ret = wolfSSL_accept(srv_ssl);
            err = wolfSSL_get_error(srv_ssl, ret);
        }
        while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    #endif
        start = gettime_secs(0) - start;
        if (ret != WOLFSSL_SUCCESS) {
            ret = wolfSSL_get_error(srv_ssl, ret);
            printf("error %d on server accept\n", ret);
            goto exit;
        }

        info->server_stats.connTime += start;
        info->server_stats.connCount++;

        /* echo loop */
        ret = 0;
        total_sz = 0;
        while (ret == 0 && total_sz < info->maxSize) {
            double rxTime;

            /* read message from client */
            XMEMSET(readBuf, 0, readBufSz);
            start = gettime_secs(1);
        #ifndef BENCH_USE_NONBLOCK
            ret = wolfSSL_read(srv_ssl, readBuf, readBufSz);
        #else
            do {
                ret = wolfSSL_read(srv_ssl, readBuf, readBufSz);
                err = wolfSSL_get_error(srv_ssl, ret);
            }
            while (err == WOLFSSL_ERROR_WANT_READ);
        #endif
            rxTime = gettime_secs(0) - start;

            /* shutdown signals, no more connections for this cipher */
            if (XSTRSTR((const char*)readBuf, kShutdown) != NULL) {
                info->server.shutdown = 1;
                if (info->showVerbose) {
                    printf("Server shutdown done\n");
                }
                ret = 0; /* success */
                break;
            }

            info->server_stats.rxTime += rxTime;
            if (ret < 0) {
                ret = wolfSSL_get_error(srv_ssl, ret);
                printf("error %d on server read\n", ret);
                goto exit;
            }
            info->server_stats.rxTotal += ret;
            len = ret;
            total_sz += ret;

            /* write message back to client */
            start = gettime_secs(1);
        #ifndef BENCH_USE_NONBLOCK
            ret = wolfSSL_write(srv_ssl, readBuf, len);
        #else
            do {
                ret = wolfSSL_write(srv_ssl, readBuf, len);
                err = wolfSSL_get_error(srv_ssl, ret);
            }
            while (err == WOLFSSL_ERROR_WANT_WRITE);
        #endif
            info->server_stats.txTime += gettime_secs(0) - start;
            if (ret < 0) {
                ret = wolfSSL_get_error(srv_ssl, ret);
                printf("error %d on server write\n", ret);
                goto exit;
            }
            info->server_stats.txTotal += ret;
            ret = 0; /* reset return code */
        }

        wolfSSL_free(srv_ssl);
        srv_ssl = NULL;
    }

exit:

    if (ret != 0 && ret != WOLFSSL_SUCCESS) {
        info->doShutdown = 1;
        printf("Server Error: %d (%s)\n", ret,
            wolfSSL_ERR_reason_error_string(ret));
    }

    /* clean up */
    if (srv_ssl != NULL)
        wolfSSL_free(srv_ssl);
    if (srv_ctx != NULL)
        wolfSSL_CTX_free(srv_ctx);
    XFREE(readBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    info->server.ret = ret;

    return ret;
}

#ifdef CMSIS_OS2_H_
static void server_thread(void* args)
#else
static void server_thread(const void* args)
#endif
{
    int ret;
    info_t* info = (info_t*)args;

#ifdef CMSIS_OS2_H_
    info->server.threadId = osThreadGetId();
#endif
    do {
        ret = bench_tls_server(info);

        /* signal client */
        if (!info->client.done && info->client.threadId != 0) {
#ifdef CMSIS_OS2_H_
            osThreadFlagsSet(info->client.threadId, 1);
#else
            osSignalSet(info->client.threadId, 1);
#endif
        }
        info->server.ret = ret;
        info->server.done = 1;
        osThreadSuspend(info->server.threadId);

        if (info->doShutdown)
            info->client.done = 1;
    } while (!info->doShutdown);

    osThreadTerminate(info->server.threadId);
    info->server.threadId = NULL;
}

#ifdef CMSIS_OS2_H_
static const osThreadAttr_t server_thread_attributes = {
  .name = "server_thread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = WOLF_EXAMPLES_STACK
};
static const osThreadAttr_t client_thread_attributes = {
  .name = "client_thread",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = WOLF_EXAMPLES_STACK
};
#endif

int bench_tls(void* args)
{
    int ret = 0;
    info_t *info = NULL;
    char *cipher, *next_cipher, *ciphers;

    /* Runtime variables */
    int argRuntimeSec = BENCH_RUNTIME_SEC;
    int argTestPacketSize = TEST_PACKET_SIZE;
    int argTestMaxSize = TEST_MAX_SIZE;
    int argShowVerbose = SHOW_VERBOSE;
    const char* argHost = BENCH_DEFAULT_HOST;
    int argPort = BENCH_DEFAULT_PORT;
    int argShowPeerInfo = BENCH_SHOW_PEER_INFO;

#ifdef DEBUG_WOLFSSL
    if (argShowVerbose >= 3) {
        wolfSSL_Debugging_ON();
    }
    else {
        wolfSSL_Debugging_OFF();
    }
#endif

#ifdef TEST_CIPHER_SUITE
    ciphers = TEST_CIPHER_SUITE;
#else
    /* Get cipher suite list */
    ciphers = (char*)XMALLOC(WOLFSSL_CIPHER_LIST_MAX_SIZE, NULL,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (ciphers == NULL) {
        ret = MEMORY_E; goto exit;
    }
    wolfSSL_get_ciphers(ciphers, WOLFSSL_CIPHER_LIST_MAX_SIZE);
#endif
    cipher = ciphers;

    /* Allocate test info */
    info = (info_t*)XMALLOC(sizeof(info_t), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (info == NULL) {
        ret = MEMORY_E; goto exit;
    }
    XMEMSET(info, 0, sizeof(info_t));

    info->host = argHost;
    info->port = argPort;
    info->packetSize = argTestPacketSize;
    info->runTimeSec = argRuntimeSec;
    info->maxSize = argTestMaxSize;
    info->showPeerInfo = argShowPeerInfo;
    info->showVerbose = argShowVerbose;

#ifdef CMSIS_OS2_H_
    info->server.mutex = osSemaphoreNew(1, 0, NULL);
    info->client.mutex = osSemaphoreNew(1, 0, NULL);
#else
    info->server.mutex = osSemaphoreCreate(&info->server.mutexDef, 1);
    info->client.mutex = osSemaphoreCreate(&info->client.mutexDef, 1);

    /* threads */
    info->server.threadDef.name = "server_thread";
    info->server.threadDef.pthread = server_thread;
    info->server.threadDef.tpriority = osPriorityNormal;
    info->server.threadDef.stacksize = WOLF_EXAMPLES_STACK;

    info->client.threadDef.name = "client_thread";
    info->client.threadDef.pthread = client_thread;
    info->client.threadDef.tpriority = osPriorityNormal;
    info->client.threadDef.stacksize = WOLF_EXAMPLES_STACK;
#endif

    /* parse by : */
    while ((cipher != NULL) && (cipher[0] != '\0')) {
        next_cipher = strchr(cipher, ':');
        if (next_cipher != NULL) {
            cipher[next_cipher - cipher] = '\0';
        }

        if (argShowVerbose) {
            printf("Cipher: %s\n", cipher);
        }

        /* set cipher suite */
        info->cipher = cipher;

        /* reset thread info */
        info->server.ret = info->client.ret = 0;
        info->server.done = info->client.done = 0;
        info->server.shutdown = info->client.shutdown = 0;
        XMEMSET(&info->server_stats, 0, sizeof(info->server_stats));
        XMEMSET(&info->client_stats, 0, sizeof(info->client_stats));
        XMEMSET(&info->to_server, 0, sizeof(info->to_server));
        XMEMSET(&info->to_client, 0, sizeof(info->to_client));

        /* make sure nothing is locking it */
        osSemaphoreRelease(info->server.mutex);
        osSemaphoreRelease(info->client.mutex);

        /* start threads */
        if (info->server.threadId == 0) {
#ifdef CMSIS_OS2_H_
            osThreadNew(server_thread, info, &server_thread_attributes);
#else
            info->server.threadId = osThreadCreate(&info->server.threadDef, info);
#endif
        }
        else {
            osThreadResume(info->server.threadId);
        }
        if (info->client.threadId == 0) {
#ifdef CMSIS_OS2_H_
            osThreadNew(client_thread, info, &client_thread_attributes);
#else
            info->client.threadId = osThreadCreate(&info->client.threadDef, info);
#endif
        }
        else {
            osThreadResume(info->client.threadId);
        }

        /* Wait until threads are marked done */
        while (!info->client.done || !info->server.done) {
            osThreadYield(); /* Allow other threads to run */
        }

        if (argShowVerbose) {
            /* print results */
            PrintTlsStats(&info->server_stats, "Server", info->cipher, 1);
            PrintTlsStats(&info->client_stats, "Client", info->cipher, 1);
        }

        printf("%-6s  %-33s  %11s  %9s  %9s  %9s  %9s  %9s  %17s  %15s\n",
            "Side", "Cipher", "Total Bytes", "Num Conns", "Rx ms", "Tx ms",
            "Rx MB/s", "Tx MB/s", "Connect Total ms", "Connect Avg ms");
        PrintTlsStats(&info->server_stats, "Server", info->cipher, 0);
        PrintTlsStats(&info->client_stats, "Client", info->cipher, 0);

        PrintMemStats();

        /* target next cipher */
        cipher = (next_cipher != NULL) ? (next_cipher + 1) : NULL;
    } /* while */

    /* do thread shutdown */
    info->doShutdown = 1;
    info->server.done = 0;
    info->client.done = 0;
    osThreadResume(info->server.threadId);
    osThreadResume(info->client.threadId);
    /* Wait until threads are marked done */
    while (!info->client.done || !info->server.done) {
        osThreadYield(); /* Allow other threads to run */
    }

exit:

    /* cleanup thread info */
    osSemaphoreDelete(info->server.mutex);
    osSemaphoreDelete(info->client.mutex);
    XFREE(info, NULL, DYNAMIC_TYPE_TMP_BUFFER);

#ifndef TEST_CIPHER_SUITE
    /* Free cipher list */
    XFREE(ciphers, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    /* set return code */
    if (args)
        ((func_args*)args)->return_code = ret;

    return ret;
}
#endif /* ENABLE_TLS_BENCH */


#ifndef WOLFCRYPT_ONLY
static void ShowCiphers(void)
{
    int ret;
    char* ciphers = (char*)XMALLOC(WOLFSSL_CIPHER_LIST_MAX_SIZE, NULL,
        DYNAMIC_TYPE_TMP_BUFFER);
    if (ciphers) {
        ret = wolfSSL_get_ciphers(ciphers, WOLFSSL_CIPHER_LIST_MAX_SIZE);
        if (ret == WOLFSSL_SUCCESS)
            printf("%s\n", ciphers);
        XFREE(ciphers, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
}
#endif

static void PrintMemStats(void)
{
#ifdef WOLFSSL_DEBUG_MEMORY
    printf("\nHeap MinEver %d, Free %d, Stack %lu\n",
        xPortGetMinimumEverFreeHeapSize(),
        xPortGetFreeHeapSize(),
        uxTaskGetStackHighWaterMark(NULL));
#endif
}


#ifdef ENABLE_TLS_UART

/* UART DMA IO Routines */
#ifndef B115200
#define B115200 115200
#endif

/* Max buffer for a single TLS frame */
#ifndef MAX_RECORD_SIZE
#define MAX_RECORD_SIZE (16 * 1024)
#endif

typedef struct {
    int curr_index;
    int data_len;
    char buf[MAX_RECORD_SIZE];
} tls13_buf;

/* This sets which UART to do the TLS 1.3 connection over.  It is something you
 * will have to configure in STMCubeIDE and then change here. */
#ifndef TLS_UART
#define TLS_UART huart2
#endif
/* If you get an undefined error here you can optionally disable the TLS
 * over UART test using NO_TLS_UART_TEST */
extern UART_HandleTypeDef TLS_UART;

static int msg_length = 0;

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
   if (huart->Instance == TLS_UART.Instance) {
       msg_length = Size;
   }
}

static int uartIORx(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    HAL_StatusTypeDef status;
    tls13_buf *tb = ctx;

#ifdef DEBUG_UART_IO
    printf("UART Read: In %d\n", sz);
#endif

    if (tb->curr_index + sz <= tb->data_len) {
        XMEMCPY(buf, tb->buf + tb->curr_index, sz);
        tb->curr_index += sz;
#ifdef DEBUG_UART_IO
        printf("UART Read1: Out %d\n", sz);
#endif
        return sz;
    }

    msg_length = 0;
    XMEMSET(tb, 0, sizeof(*tb));

    /* Now setup the DMA RX */
    /* This requires enabling the UART RX DMA in the STM32Cube tool
     * Under Connectivity click on your TLS UART (USART2) and goto DMA Settings
     * and Add one for USART2_RX with default options */
    status = HAL_UARTEx_ReceiveToIdle_DMA(&TLS_UART, (uint8_t *)tb->buf, MAX_RECORD_SIZE);
    if (status != HAL_OK) {
        return WOLFSSL_CBIO_ERR_WANT_READ;
    }
    else {
        /* We now go into an infinite loop waiting for msg_length to be set to a
         * value other than 0. This will be done when the other side writes to
         * UART and then idles. That will trigger HAL_UARTEx_RxEventCallback()
         * which will set msg_length to the length of data written.
         *
         * If you mistakenly get stuck here, please simply reset the board.
         */
        while (msg_length == 0) {
            HAL_Delay(10);
        }
#ifdef DEBUG_UART_IO
        printf("Message received! length = %d\n", msg_length);
#endif
    }

    /* now return the number of bytes requested. */
    XMEMCPY(buf, tb->buf, sz);
    tb->data_len = msg_length;
    tb->curr_index = sz;

#ifdef DEBUG_UART_IO
    printf("UART Read2: Out %d\n", tb->data_len);
#endif

    return sz;
}

static int uartIOTx(WOLFSSL *ssl, char *buf, int sz, void *ctx)
{
    HAL_StatusTypeDef status;
    int ret = sz;
    (void)ctx;

#ifdef DEBUG_UART_IO
    printf("UART Write: In %d\n", sz);
#endif

    status = HAL_UART_Transmit(&TLS_UART, (uint8_t *)buf, sz, 0xFFFF);
    if (status != HAL_OK) {
        ret = WOLFSSL_CBIO_ERR_WANT_WRITE;
    }

#ifdef DEBUG_UART_IO
    printf("UART Write: Out %d\n", ret);
#endif

    return ret;
}

static void uartReset(void)
{
    HAL_UART_Abort_IT(&TLS_UART);
}

/* UART TLS 1.3 client and server */
#ifndef NO_WOLFSSL_SERVER
static int tls13_uart_server(void)
{
    int ret = -1, err;
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL* ssl = NULL;
    byte echoBuffer[100];
#ifdef WOLFSSL_SMALL_STACK
    tls13_buf* tbuf = (tls13_buf*)XMALLOC(sizeof(*tbuf), NULL,
                                          DYNAMIC_TYPE_TMP_BUFFER);
    if (tbuf == NULL) {
        printf("Memory allocation error\n");
        goto done;
    }
#else
    tls13_buf tbuf[1];
#endif

    XMEMSET(tbuf, 0, sizeof(tls13_buf));

    ctx = wolfSSL_CTX_new(wolfTLSv1_3_server_method());
    if (ctx == NULL) {
        printf("Error creating WOLFSSL_CTX\n");
        goto done;
    }

    /* Register wolfSSL send/recv callbacks */
    uartReset();
    wolfSSL_CTX_SetIOSend(ctx, uartIOTx);
    wolfSSL_CTX_SetIORecv(ctx, uartIORx);

    ret = wolfSSL_CTX_use_PrivateKey_buffer(ctx, ecc_key_der_256,
        sizeof_ecc_key_der_256, WOLFSSL_FILETYPE_ASN1);
    if (ret != WOLFSSL_SUCCESS) {
        printf("error loading server private key\n");
        goto done;
    }

    ret = wolfSSL_CTX_use_certificate_buffer(ctx, serv_ecc_der_256,
        sizeof_serv_ecc_der_256, WOLFSSL_FILETYPE_ASN1);
    if (ret != WOLFSSL_SUCCESS) {
        printf("error loading server certificate\n");
        goto done;
    }

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        printf("Error creating WOLFSSL\n");
        goto done;
    }

    wolfSSL_SetIOReadCtx(ssl, tbuf);

    printf("Waiting for client\n");
    do {
        ret = wolfSSL_accept(ssl);
        err = wolfSSL_get_error(ssl, ret);
    } while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    if (ret != WOLFSSL_SUCCESS) {
        printf("TLS accept error %d\n", err);
        goto done;
    }
    printf("TLS Accept handshake done\n");

    /* Waiting for data to echo */
    XMEMSET(echoBuffer, 0, sizeof(echoBuffer));
    do {
        ret = wolfSSL_read(ssl, echoBuffer, sizeof(echoBuffer)-1);
        err = wolfSSL_get_error(ssl, ret);
    } while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    printf("Read (%d): %s\n", err, echoBuffer);

    do {
        ret = wolfSSL_write(ssl, echoBuffer, XSTRLEN((char*)echoBuffer));
        err = wolfSSL_get_error(ssl, ret);
    } while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    printf("Sent (%d): %s\n", err, echoBuffer);

    ret = 0; /* Success */

done:
    if (ssl) {
        wolfSSL_shutdown(ssl);
        wolfSSL_free(ssl);
    }
    if (ctx) {
        wolfSSL_CTX_free(ctx);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (tbuf != NULL) {
        XFREE(tbuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif

    return ret;
}
#endif

#ifdef ENABLE_TLS_UART
static int tls13_uart_client(void)
{
    int ret = -1, err;
    WOLFSSL_CTX* ctx = NULL;
    WOLFSSL* ssl = NULL;
    const char testStr[] = "Testing 1, 2 and 3\r\n";
    byte readBuf[100];
#ifdef WOLFSSL_SMALL_STACK
    tls13_buf* tbuf = (tls13_buf*)XMALLOC(sizeof(*tbuf), NULL,
                                            DYNAMIC_TYPE_TMP_BUFFER);
    if (tbuf == NULL) {
        printf("Memory allocation error\n");
        goto done;
    }
#else
    tls13_buf tbuf[1];
#endif

    XMEMSET(tbuf, 0, sizeof(tls13_buf));

    ctx = wolfSSL_CTX_new(wolfTLSv1_3_client_method());
    if (ctx == NULL) {
        printf("Error creating WOLFSSL_CTX\n");
        goto done;
    }

    /* Register wolfSSL send/recv callbacks */
    uartReset();
    wolfSSL_CTX_SetIOSend(ctx, uartIOTx);
    wolfSSL_CTX_SetIORecv(ctx, uartIORx);

    /* Load the root certificate. */
    wolfSSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);

    ssl = wolfSSL_new(ctx);
    if (ssl == NULL) {
        printf("Error creating WOLFSSL\n");
        goto done;
    }

    wolfSSL_SetIOReadCtx(ssl, tbuf);

#ifdef HAVE_PQC
    if (wolfSSL_UseKeyShare(ssl, WOLFSSL_KYBER_LEVEL1) != WOLFSSL_SUCCESS) {
        printf("wolfSSL_UseKeyShare Error!!");
    }
#endif

    do {
        ret = wolfSSL_connect(ssl);
        err = wolfSSL_get_error(ssl, ret);
    } while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    if (ret != WOLFSSL_SUCCESS) {
        printf("TLS connect error %d\n", err);
        goto done;
    }

    ShowPeer(ssl);

    printf("TLS Connect handshake done\n");
    printf("Sending test string\n");
    do {
        ret = wolfSSL_write(ssl, testStr, XSTRLEN(testStr));
        err = wolfSSL_get_error(ssl, ret);
    } while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    printf("Sent (%d): %s\n", err, testStr);

    XMEMSET(readBuf, 0, sizeof(readBuf));
    do {
        ret = wolfSSL_read(ssl, readBuf, sizeof(readBuf)-1);
        err = wolfSSL_get_error(ssl, ret);
    } while (err == WOLFSSL_ERROR_WANT_READ || err == WOLFSSL_ERROR_WANT_WRITE);
    printf("Read (%d): %s\n", err, readBuf);

    ret = 0; /* Success */

done:
    if (ssl) {
        wolfSSL_shutdown(ssl);
        wolfSSL_free(ssl);
    }
    if (ctx) {
        wolfSSL_CTX_free(ctx);
    }
#ifdef WOLFSSL_SMALL_STACK
    if (tbuf != NULL) {
        XFREE(tbuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif

    return ret;
}
#endif
#endif /* ENABLE_TLS_UART */


/*****************************************************************************
 * Public functions
 ****************************************************************************/
#ifdef HAL_RTC_MODULE_ENABLED
extern RTC_HandleTypeDef hrtc;
double current_time(void)
{
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;
    uint32_t subsec = 0;

    /* must get time and date here due to STM32 HW bug */
    HAL_RTC_GetTime(&hrtc, &time, FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, FORMAT_BIN);
    /* Not all STM32 RTCs have subseconds in the struct */
#ifdef RTC_ALARMSUBSECONDMASK_ALL
    subsec = (255 - time.SubSeconds) * 1000 / 255;
#endif

    (void) date;

    /* return seconds.milliseconds */
    return ((double) time.Hours * 24) + ((double) time.Minutes * 60)
            + (double) time.Seconds + ((double) subsec / 1000);
}
#endif /* HAL_RTC_MODULE_ENABLED */

#ifdef CMSIS_OS2_H_
void wolfCryptDemo(void* argument)
#else
void wolfCryptDemo(const void* argument)
#endif
{
    HAL_StatusTypeDef halRet;
    uint8_t buffer[2];
    func_args args;

#if 0
    wolfSSL_Debugging_ON();
#endif

    /* initialize wolfSSL */
#ifdef WOLFCRYPT_ONLY
    wolfCrypt_Init();
#else
    wolfSSL_Init();
#endif

#ifdef WOLFSSL_STATIC_MEMORY
    if (wc_LoadStaticMemory(&HEAP_HINT, gWolfMem, sizeof(gWolfMem),
            WOLFMEM_GENERAL, 10) != 0) {
        printf("unable to load static memory");
    }
#endif

    //wolfSSL_SetAllocators(wolfMallocCb, wolfFreeCb, wolfReallocCb);

    while (1) {
        memset(&args, 0, sizeof(args));
        args.return_code = NOT_COMPILED_IN; /* default */

        printf("\n\t\t\t\tMENU\n");
        printf(menu1);
        printf("Please select one of the above options:\n");

        do {
            halRet = HAL_UART_Receive(&HAL_CONSOLE_UART, buffer, sizeof(buffer), 100);
        } while (halRet != HAL_OK || buffer[0] == '\n' || buffer[0] == '\r');

        switch (buffer[0]) {
        case 't':
            printf("Running wolfCrypt Tests...\n");
        #ifndef NO_CRYPT_TEST
            args.return_code = 0;
            wolfcrypt_test(&args);
        #else
            args.return_code = NOT_COMPILED_IN;
        #endif
            printf("Crypt Test: Return code %d\n", args.return_code);
            break;

        case 'b':
            printf("Running wolfCrypt Benchmarks...\n");
        #ifndef NO_CRYPT_BENCHMARK
            args.return_code = 0;
            benchmark_test(&args);
        #else
            args.return_code = NOT_COMPILED_IN;
        #endif
            printf("Benchmark Test: Return code %d\n", args.return_code);
            break;

        case 'l':
            printf("Running TLS Benchmarks...\n");
        #ifdef ENABLE_TLS_BENCH
            bench_tls(&args);
        #else
            args.return_code = NOT_COMPILED_IN;
        #endif
            printf("TLS Benchmarks: Return code %d\n", args.return_code);
            break;

        case 'e':
        #ifndef WOLFCRYPT_ONLY
            ShowCiphers();
        #else
            printf("Not compiled in\n");
        #endif
            break;
#ifdef ENABLE_TLS_UART
        case 's':
        #ifndef NO_WOLFSSL_SERVER
            printf("Running TLS 1.3 server...\n");
            args.return_code = tls13_uart_server();
        #else
            args.return_code = NOT_COMPILED_IN;
        #endif
            printf("TLS 1.3 Server: Return code %d\n", args.return_code);
            break;

        case 'c':
        #ifndef NO_WOLFSSL_CLIENT
            printf("Running TLS 1.3 client...\n");
            args.return_code = tls13_uart_client();
        #else
            args.return_code = NOT_COMPILED_IN;
        #endif
            printf("TLS 1.3 Client: Return code %d\n", args.return_code);
            break;
#endif /* ENABLE_TLS_UART */
            /* All other cases go here */
        default:
            printf("\nSelection out of range\n");
            break;
        }

        PrintMemStats();
    }

#ifdef WOLFCRYPT_ONLY
    wolfCrypt_Cleanup();
#else
    wolfSSL_Cleanup();
#endif
}
