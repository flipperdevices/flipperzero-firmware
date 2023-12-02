/* logging.c
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

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#if defined(OPENSSL_EXTRA) && !defined(WOLFCRYPT_ONLY)
/* avoid adding WANT_READ and WANT_WRITE to error queue */
#include <wolfssl/error-ssl.h>
#endif

#ifdef WOLFSSL_FUNC_TIME
/* WARNING: This code is only to be used for debugging performance.
 *          The code is not thread-safe.
 *          Do not use WOLFSSL_FUNC_TIME in production code.
 */
static double wc_func_start[WC_FUNC_COUNT];
static double wc_func_time[WC_FUNC_COUNT] = { 0, };
static const char* wc_func_name[WC_FUNC_COUNT] = {
    "SendHelloRequest",
    "DoHelloRequest",
    "SendClientHello",
    "DoClientHello",
    "SendServerHello",
    "DoServerHello",
    "SendEncryptedExtensions",
    "DoEncryptedExtensions",
    "SendCertificateRequest",
    "DoCertificateRequest",
    "SendCertificate",
    "DoCertificate",
    "SendCertificateVerify",
    "DoCertificateVerify",
    "SendFinished",
    "DoFinished",
    "SendKeyUpdate",
    "DoKeyUpdate",
    "SendEarlyData",
    "DoEarlyData",
    "SendNewSessionTicket",
    "DoNewSessionTicket",
    "SendServerHelloDone",
    "DoServerHelloDone",
    "SendTicket",
    "DoTicket",
    "SendClientKeyExchange",
    "DoClientKeyExchange",
    "SendCertificateStatus",
    "DoCertificateStatus",
    "SendServerKeyExchange",
    "DoServerKeyExchange",
    "SendEarlyData",
    "DoEarlyData",
};

#include <sys/time.h>

/* WARNING: This function is not portable. */
static WC_INLINE double current_time(int reset)
{
    struct timeval tv;
    gettimeofday(&tv, 0);
    (void)reset;

    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
}
#endif /* WOLFSSL_FUNC_TIME */

#ifdef HAVE_WC_INTROSPECTION

const char *wolfSSL_configure_args(void) {
#ifdef LIBWOLFSSL_CONFIGURE_ARGS
  /* the spaces on either side are to make matching simple and efficient. */
  return " " LIBWOLFSSL_CONFIGURE_ARGS " ";
#else
  return NULL;
#endif
}

PEDANTIC_EXTENSION const char *wolfSSL_global_cflags(void) {
#ifdef LIBWOLFSSL_GLOBAL_CFLAGS
  /* the spaces on either side are to make matching simple and efficient. */
  return " " LIBWOLFSSL_GLOBAL_CFLAGS " ";
#else
  return NULL;
#endif
}

#endif /* HAVE_WC_INTROSPECTION */

#ifdef HAVE_STACK_SIZE_VERBOSE

THREAD_LS_T unsigned char *StackSizeCheck_myStack = NULL;
THREAD_LS_T size_t StackSizeCheck_stackSize = 0;
THREAD_LS_T size_t StackSizeCheck_stackSizeHWM = 0;
THREAD_LS_T size_t *StackSizeCheck_stackSizeHWM_ptr = 0;
THREAD_LS_T void *StackSizeCheck_stackOffsetPointer = 0;

#endif /* HAVE_STACK_SIZE_VERBOSE */

#ifdef DEBUG_WOLFSSL

/* Set these to default values initially. */
static wolfSSL_Logging_cb log_function = NULL;
static int loggingEnabled = 0;
THREAD_LS_T const char* log_prefix = NULL;

#if defined(WOLFSSL_APACHE_MYNEWT)
#include "log/log.h"
static struct log mynewt_log;
#endif /* WOLFSSL_APACHE_MYNEWT */

#endif /* DEBUG_WOLFSSL */

#ifdef DEBUG_VECTOR_REGISTER_ACCESS
THREAD_LS_T int wc_svr_count = 0;
THREAD_LS_T const char *wc_svr_last_file = NULL;
THREAD_LS_T int wc_svr_last_line = -1;
#endif


/* allow this to be set to NULL, so logs can be redirected to default output */
int wolfSSL_SetLoggingCb(wolfSSL_Logging_cb f)
{
#ifdef DEBUG_WOLFSSL
    log_function = f;
    return 0;
#else
    (void)f;
    return NOT_COMPILED_IN;
#endif
}

/* allow this to be set to NULL, so logs can be redirected to default output */
wolfSSL_Logging_cb wolfSSL_GetLoggingCb(void)
{
#ifdef DEBUG_WOLFSSL
    return log_function;
#else
    return NULL;
#endif
}


int wolfSSL_Debugging_ON(void)
{
#ifdef DEBUG_WOLFSSL
    loggingEnabled = 1;
#if defined(WOLFSSL_APACHE_MYNEWT)
    log_register("wolfcrypt", &mynewt_log, &log_console_handler, NULL, LOG_SYSLEVEL);
#endif /* WOLFSSL_APACHE_MYNEWT */
    return 0;
#else
    return NOT_COMPILED_IN;
#endif
}


void wolfSSL_Debugging_OFF(void)
{
#ifdef DEBUG_WOLFSSL
    loggingEnabled = 0;
#endif
}

WOLFSSL_API void wolfSSL_SetLoggingPrefix(const char* prefix)
{
#ifdef DEBUG_WOLFSSL
    log_prefix = prefix;
#else
    (void)prefix;
#endif
}

#ifdef WOLFSSL_FUNC_TIME
/* WARNING: This code is only to be used for debugging performance.
 *          The code is not thread-safe.
 *          Do not use WOLFSSL_FUNC_TIME in production code.
 */
void WOLFSSL_START(int funcNum)
{
    if (funcNum < WC_FUNC_COUNT) {
        double now = current_time(0) * 1000.0;
    #ifdef WOLFSSL_FUNC_TIME_LOG
        fprintf(stderr, "%17.3f: START - %s\n", now, wc_func_name[funcNum]);
    #endif
        wc_func_start[funcNum] = now;
    }
}

void WOLFSSL_END(int funcNum)
{
    if (funcNum < WC_FUNC_COUNT) {
        double now = current_time(0) * 1000.0;
        wc_func_time[funcNum] += now - wc_func_start[funcNum];
    #ifdef WOLFSSL_FUNC_TIME_LOG
        fprintf(stderr, "%17.3f: END   - %s\n", now, wc_func_name[funcNum]);
    #endif
    }
}

void WOLFSSL_TIME(int count)
{
    int i;
    double avg, total = 0;

    for (i = 0; i < WC_FUNC_COUNT; i++) {
        if (wc_func_time[i] > 0) {
            avg = wc_func_time[i] / count;
            fprintf(stderr, "%8.3f ms: %s\n", avg, wc_func_name[i]);
            total += avg;
        }
    }
    fprintf(stderr, "%8.3f ms\n", total);
}
#endif

#ifdef DEBUG_WOLFSSL

#if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    /* see wc_port.h for fio.h and nio.h includes */
#elif defined(WOLFSSL_SGX)
    /* Declare sprintf for ocall */
    int sprintf(char* buf, const char *fmt, ...);
#elif defined(WOLFSSL_DEOS)
#elif defined(MICRIUM)
    #if (BSP_SER_COMM_EN  == DEF_ENABLED)
        #include <bsp_ser.h>
    #endif
#elif defined(WOLFSSL_USER_LOG)
    /* user includes their own headers */
#elif defined(WOLFSSL_ESPIDF)
    #include "esp_types.h"
    #include "esp_log.h"
#elif defined(WOLFSSL_TELIT_M2MB)
    #include <stdio.h>
    #include "m2m_log.h"
#elif defined(WOLFSSL_ANDROID_DEBUG)
    #include <android/log.h>
#elif defined(WOLFSSL_XILINX)
    #include "xil_printf.h"
#elif defined(WOLFSSL_LINUXKM)
    /* the requisite linux/kernel.h is included in wc_port.h, with incompatible warnings masked out. */
#elif defined(FUSION_RTOS)
    #include <fclstdio.h>
    #include <wolfssl/wolfcrypt/wc_port.h>
    #define fprintf FCL_FPRINTF
#else
    #include <stdio.h>  /* for default printf stuff */
#endif

#if defined(THREADX) && !defined(THREADX_NO_DC_PRINTF)
    int dc_log_printf(char*, ...);
#endif

#ifdef HAVE_STACK_SIZE_VERBOSE
#include <wolfssl/wolfcrypt/mem_track.h>
#endif

static void wolfssl_log(const int logLevel, const char *const logMessage)
{
    if (log_function)
        log_function(logLevel, logMessage);
    else {
#if defined(WOLFSSL_USER_LOG)
        WOLFSSL_USER_LOG(logMessage);
#elif defined(WOLFSSL_LOG_PRINTF)
        printf("%s\n", logMessage);

#elif defined(THREADX) && !defined(THREADX_NO_DC_PRINTF)
        dc_log_printf("%s\n", logMessage);
#elif defined(WOLFSSL_DEOS)
        printf("%s\r\n", logMessage);
#elif defined(MICRIUM)
        BSP_Ser_Printf("%s\r\n", logMessage);
#elif defined(WOLFSSL_MDK_ARM)
        fflush(stdout) ;
        printf("%s\n", logMessage);
        fflush(stdout) ;
#elif defined(WOLFSSL_UTASKER)
        fnDebugMsg((char*)logMessage);
        fnDebugMsg("\r\n");
#elif defined(MQX_USE_IO_OLD)
        fprintf(_mqxio_stderr, "%s\n", logMessage);
#elif defined(WOLFSSL_APACHE_MYNEWT)
        LOG_DEBUG(&mynewt_log, LOG_MODULE_DEFAULT, "%s\n", logMessage);
#elif defined(WOLFSSL_ESPIDF)
        ESP_LOGI("wolfssl", "%s", logMessage);
#elif defined(WOLFSSL_ZEPHYR)
        printk("%s\n", logMessage);
#elif defined(WOLFSSL_TELIT_M2MB)
        M2M_LOG_INFO("%s\n", logMessage);
#elif defined(WOLFSSL_ANDROID_DEBUG)
        __android_log_print(ANDROID_LOG_VERBOSE, "[wolfSSL]", "%s", logMessage);
#elif defined(WOLFSSL_XILINX)
        xil_printf("%s\r\n", logMessage);
#elif defined(WOLFSSL_LINUXKM)
        printk("%s\n", logMessage);
#elif defined(WOLFSSL_RENESAS_RA6M4)
        myprintf("%s\n", logMessage);
#elif defined(STACK_SIZE_CHECKPOINT_MSG) && \
      defined(HAVE_STACK_SIZE_VERBOSE) && defined(HAVE_STACK_SIZE_VERBOSE_LOG)
        STACK_SIZE_CHECKPOINT_MSG(logMessage);
#else
        if (log_prefix != NULL)
            fprintf(stderr, "[%s]: %s\n", log_prefix, logMessage);
        else
            fprintf(stderr, "%s\n", logMessage);
#endif
    }
}

#ifndef WOLFSSL_DEBUG_ERRORS_ONLY

#if defined(XVSNPRINTF) && !defined(NO_WOLFSSL_MSG_EX)
#include <stdarg.h> /* for var args */
#ifndef WOLFSSL_MSG_EX_BUF_SZ
#define WOLFSSL_MSG_EX_BUF_SZ 100
#endif
#ifdef __clang__
/* tell clang argument 1 is format */
__attribute__((__format__ (__printf__, 1, 0)))
#endif
void WOLFSSL_MSG_EX(const char* fmt, ...)
{
    if (loggingEnabled) {
        char msg[WOLFSSL_MSG_EX_BUF_SZ];
        int written;
        va_list args;
        va_start(args, fmt);
        written = XVSNPRINTF(msg, sizeof(msg), fmt, args);
        va_end(args);
        if (written > 0)
            wolfssl_log(INFO_LOG , msg);
    }
}
#endif

void WOLFSSL_MSG(const char* msg)
{
    if (loggingEnabled)
        wolfssl_log(INFO_LOG , msg);
}

#ifndef LINE_LEN
#define LINE_LEN 16
#endif
void WOLFSSL_BUFFER(const byte* buffer, word32 length)
{
    int i, buflen = (int)length;
    char line[(LINE_LEN * 4) + 3]; /* \t00..0F | chars...chars\0 */

    if (!loggingEnabled) {
        return;
    }

    if (!buffer) {
        wolfssl_log(INFO_LOG, "\tNULL");
        return;
    }

    while (buflen > 0) {
        int bufidx = 0;
        XSNPRINTF(&line[bufidx], sizeof(line)-bufidx, "\t");
        bufidx++;

        for (i = 0; i < LINE_LEN; i++) {
            if (i < buflen) {
                XSNPRINTF(&line[bufidx], sizeof(line)-bufidx, "%02x ", buffer[i]);
            }
            else {
                XSNPRINTF(&line[bufidx], sizeof(line)-bufidx, "   ");
            }
            bufidx += 3;
        }

        XSNPRINTF(&line[bufidx], sizeof(line)-bufidx, "| ");
        bufidx++;

        for (i = 0; i < LINE_LEN; i++) {
            if (i < buflen) {
                XSNPRINTF(&line[bufidx], sizeof(line)-bufidx,
                     "%c", 31 < buffer[i] && buffer[i] < 127 ? buffer[i] : '.');
                bufidx++;
            }
        }

        wolfssl_log(INFO_LOG, line);
        buffer += LINE_LEN;
        buflen -= LINE_LEN;
    }
}


void WOLFSSL_ENTER(const char* msg)
{
    if (loggingEnabled) {
        char buffer[WOLFSSL_MAX_ERROR_SZ];
        XSNPRINTF(buffer, sizeof(buffer), "wolfSSL Entering %s", msg);
        wolfssl_log(ENTER_LOG , buffer);
    }
}


void WOLFSSL_LEAVE(const char* msg, int ret)
{
    if (loggingEnabled) {
        char buffer[WOLFSSL_MAX_ERROR_SZ];
        XSNPRINTF(buffer, sizeof(buffer), "wolfSSL Leaving %s, return %d",
                msg, ret);
        wolfssl_log(LEAVE_LOG , buffer);
    }
}

WOLFSSL_API int WOLFSSL_IS_DEBUG_ON(void)
{
    return loggingEnabled;
}
#endif /* !WOLFSSL_DEBUG_ERRORS_ONLY */
#endif /* DEBUG_WOLFSSL */

#if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)

#ifdef WOLFSSL_HAVE_ERROR_QUEUE

#ifdef ERROR_QUEUE_PER_THREAD
/* Keep the error queue in thread-local-storage. The only ways this
 * API can deliver meaningful semantics in a multi-threaded setup.
 */
#ifndef ERROR_QUEUE_MAX
/* Same as OpenSSL v1.1.x limit, note that this is per thread */
#define ERROR_QUEUE_MAX 16
#endif

struct wc_error_entry {
    char   reason[WOLFSSL_MAX_ERROR_SZ];
    char   file[WOLFSSL_MAX_ERROR_SZ];
    int    line;
    int    err;
};

struct wc_error_queue {
    struct wc_error_entry entries[ERROR_QUEUE_MAX];
    size_t head_idx;
    size_t count;
};

/* The complete queue in a thread local without allocations */
static THREAD_LS_T struct wc_error_queue wc_errors;

/* Using thread-local-storage, we do not need a mutex. */
#define ERRQ_LOCK()            0
#define ERRQ_UNLOCK()          (void)0

/**
 * Given a relative index (from head of the error list), return
 * the absolute index in the `wc_errors->entries` array for
 * the entry or -1 if no such entry exists/is present.
 */
static int get_abs_idx(int relative_idx)
{
    if ((wc_errors.count == 0) || (relative_idx >= (int)wc_errors.count)) {
        return -1;
    }
    if (relative_idx < 0) {
        return (int)((wc_errors.head_idx + wc_errors.count - 1)
                      % ERROR_QUEUE_MAX);
    }
    return (int)((wc_errors.head_idx + (size_t)relative_idx) % ERROR_QUEUE_MAX);
}

/**
 * Return the error entry at the given relative index, if
 * it exists, e.g. `relative_idx` is in a valid range.
 */
static struct wc_error_entry *get_entry(int relative_idx)
{
    int abs_idx;

    abs_idx = get_abs_idx(relative_idx);
    if (abs_idx < 0) {
        return NULL;
    }
    return &wc_errors.entries[abs_idx];
}

/**
 * Return the error code in the given error `entry` and populate
 * `file`, `reason` and `line` with its values.
 * `entry` may be NULL, in which case BAD_STATE_E is returned.
 */
static int pass_entry(struct wc_error_entry *entry,
                      const char **file, const char **reason,
                      int *line)
{
    if (entry == NULL) {
        WOLFSSL_MSG("No Error found in queue");
        return BAD_STATE_E;
    }
    if (file != NULL) {
        *file = entry->file;
    }
    if (reason != NULL) {
        *reason = entry->reason;
    }
    if (line != NULL) {
        *line = entry->line;
    }
    return entry->err;
}

/**
 * Assign entry with values, resets all previously present values.
 */
static void set_entry(struct wc_error_entry *entry, int error,
                      const char *file, const char *reason, int line)
{
    size_t sz;

    XMEMSET(entry, 0, sizeof(struct wc_error_entry));
    entry->err = error;

    entry->line  = line;
    sz = XSTRLEN(reason);
    if (sz > WOLFSSL_MAX_ERROR_SZ - 1) {
        sz = WOLFSSL_MAX_ERROR_SZ - 1;
    }
    if (sz > 0) {
        XMEMCPY(entry->reason, reason, sz);
        entry->reason[WOLFSSL_MAX_ERROR_SZ - 1] = '\0';
    }

    sz = XSTRLEN(file);
    if (sz > WOLFSSL_MAX_ERROR_SZ - 1) {
        sz = WOLFSSL_MAX_ERROR_SZ - 1;
    }
    if (sz > 0) {
        XMEMCPY(entry->file, file, sz);
        entry->file[WOLFSSL_MAX_ERROR_SZ - 1]  = '\0';
    }
}

/* Internal function that is called by wolfCrypt_Init() */
int wc_LoggingInit(void)
{
    return 0;
}

/* internal function that is called by wolfCrypt_Cleanup */
int wc_LoggingCleanup(void)
{
    /* clear logging entries */
    wc_ClearErrorNodes();
    return 0;
}

/**
 * Get the values from the HEAD of the ERR queue, but keep it in place.
 * If the queue is empty, return BAD_STATE_E.
 */
int wc_PeekErrorNode(int idx, const char **file, const char **reason,
                     int *line)
{
    return pass_entry(get_entry(idx), file, reason, line);
}

/**
 * Get the values from the HEAD of the ERR queue and remove it.
 * If the queue is empty, return BAD_STATE_E.
 */
int wc_PullErrorNode(const char **file, const char **reason, int *line)
{
    struct wc_error_entry *entry;
    int ret;

    entry = get_entry(0);
    ret = pass_entry(entry, file, reason, line);

    if (entry != NULL) {
      wc_RemoveErrorNode(0);
    }
    return ret;
}

/* create new error node and add it to the queue
 * buffers are assumed to be of size WOLFSSL_MAX_ERROR_SZ for this internal
 * function. */
int wc_AddErrorNode(int error, int line, char* reason, char* file)
{
    struct wc_error_entry *entry;
    size_t idx;

    if (wc_errors.count >= ERROR_QUEUE_MAX) {
        WOLFSSL_MSG("Error queue is full, at ERROR_QUEUE_MAX");
        return MEMORY_E;
    }

    idx = (wc_errors.head_idx + wc_errors.count) % ERROR_QUEUE_MAX;
    entry = &wc_errors.entries[idx];
    set_entry(entry, error, file, reason, line);
    ++wc_errors.count;

    return 0;
}

/**
 * Remove the entry at relative position `relative_idx` from the ERR queue.
 * For `relative_idx == 0` it removes the queue's head entry, for -1
 * it removes the last entry in the queue.
 */
void wc_RemoveErrorNode(int relative_idx)
{
    int abs_idx = get_abs_idx(relative_idx);

    if (abs_idx >= 0) {
        size_t move_count;
        if (abs_idx >= (int)wc_errors.head_idx) {
            /* removed entry sits "above" head (or is head),
             * move entries below it "up" */
            move_count = (size_t)abs_idx - wc_errors.head_idx;
            if (move_count > 0) {
                XMEMMOVE(&wc_errors.entries[wc_errors.head_idx + 1],
                         &wc_errors.entries[wc_errors.head_idx],
                         sizeof(wc_errors.entries[0]) * move_count);
            }
            wc_errors.head_idx = (wc_errors.head_idx + 1) % ERROR_QUEUE_MAX;
            --wc_errors.count;
        }
        else {
            /* removed entry sits "below" head (wrap around),
             * move entries above it "down" */
            int last_idx = get_abs_idx(-1);
            if (last_idx >= abs_idx) {  /* this SHOULD always be true */
                move_count = (size_t)(last_idx - abs_idx);
                if (move_count > 0) {
                    XMEMMOVE(&wc_errors.entries[abs_idx],
                             &wc_errors.entries[abs_idx + 1],
                             sizeof(wc_errors.entries[0]) * move_count);
                }
                --wc_errors.count;
            }
        }
    }
}

/**
 * Clear the ERR queue.
 */
void wc_ClearErrorNodes(void)
{
    if (wc_errors.count > 0) {
        XMEMSET(&wc_errors, 0, sizeof(wc_errors));
    }
}

int wc_SetLoggingHeap(void* h)
{
    (void)h;
    return 0;
}

int wc_ERR_remove_state(void)
{
    wc_ClearErrorNodes();
    return 0;
}

/**
 * Get the first entry's values in the ERR queue that is not filtered
 * by the provided `ignore_err` callback. All ignored entries are removed,
 * making the returned entry the head of the ERR queue afterwards.
 *
 * In case all entries are ignored, the ERR queue will be empty afterwards.
 * For an empty ERR queue  0 is returned.
 *
 * `ignore_err` may be NULL, in which case this returns the HEAD values.
 *
 * `flags` is present for OpenSSL compatibility, but will always be
 * set to 0, since we do not keep flags at ERR entries.
 */
unsigned long wc_PeekErrorNodeLineData(const char **file, int *line,
                                       const char **data, int *flags,
                                       int (*ignore_err)(int err))
{
    WOLFSSL_ENTER("wc_PeekErrorNodeLineData");

    /* No data or flags stored - error display only in Nginx. */
    if (data != NULL) {
        *data = "";
    }
    if (flags != NULL) {
        *flags = 0;
    }

    while (1) {
        int ret = wc_PeekErrorNode(0, file, NULL, line);
        if (ret == BAD_STATE_E) {
            WOLFSSL_MSG("Issue peeking at error node in queue");
            return 0;
        }
        /* OpenSSL uses positive error codes */
        if (ret < 0) {
            ret = -ret;
        }
        /* an error that the caller wants to ignore? */
        if (ignore_err && ignore_err(ret)) {
          wc_RemoveErrorNode(0);
          continue;
        }

        return (unsigned long)ret;
    }
}

/**
 * Get the error value at the HEAD of the ERR queue or 0 if the queue
 * is empty. The HEAD entry is removed by this call.
 */
unsigned long wc_GetErrorNodeErr(void)
{
    int ret;

    WOLFSSL_ENTER("wc_GetErrorNodeErr");

    ret = wc_PullErrorNode(NULL, NULL, NULL);
    if (ret < 0) {
        if (ret == BAD_STATE_E) {
            ret = 0; /* no errors in queue */
        }
        else {
            WOLFSSL_MSG("Error with pulling error node!");
            WOLFSSL_LEAVE("wolfSSL_ERR_get_error", ret);
            ret = 0 - ret; /* return absolute value of error */
            /* panic and try to clear out nodes */
            wc_ClearErrorNodes();
        }
    }
    return (unsigned long)ret;
}

#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
/* This callback allows the application to provide a custom error printing
 * function. */
void wc_ERR_print_errors_cb(int (*cb)(const char *str, size_t len, void *u),
                            void *u)
{
    size_t i;

    WOLFSSL_ENTER("wc_ERR_print_errors_cb");

    if (cb == NULL) {
        /* Invalid param */
        return;
    }

    for (i = 0; i < wc_errors.count; ++i) {
        struct wc_error_entry *entry = get_entry((int)i);
        if (entry == NULL)
            break;
        cb(entry->reason, XSTRLEN(entry->reason), u);
    }

    wc_ClearErrorNodes();
}
#endif /* !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM) */

#else /* ERROR_QUEUE_PER_THREAD */
/* Error queue is a global list. This is the original implementation and
 * the fallback on platforms that do not have thread-local-storage.
 *
 * Access and manipulations of the list are protected by a mutex, however
 * that does not prevent errors from another thread showing up. Therefore,
 * its usefulness  is limited to applications with restricted thread
 * concurrency in using wolfSSL.
 */
#ifndef ERROR_QUEUE_MAX
/* With a global list, we allow a higher limit. */
#define ERROR_QUEUE_MAX 100
#endif

/* The information we keep about a single error */
struct wc_error_queue {
    void*  heap; /* the heap hint used with nodes creation */
    struct wc_error_queue* next;
    struct wc_error_queue* prev;
    char   error[WOLFSSL_MAX_ERROR_SZ];
    char   file[WOLFSSL_MAX_ERROR_SZ];
    int    value;
    int    line;
};

/* The global list of errors encountered */
static struct wc_error_queue* wc_errors;
static int wc_errors_count = 0;
/* pointer to last node in queue to make insertion O(1) */
static struct wc_error_queue* wc_last_node;
/* The 'current' cursor the application is using to access the list */
static struct wc_error_queue* wc_current_node;

/* heap info currently used for allocation of entries */
static void* wc_error_heap;

/* mutex for list operation protection */
static wolfSSL_Mutex wc_error_mutex;
#define ERRQ_MUTEX_INIT()      wc_InitMutex(&wc_error_mutex)
#define ERRQ_MUTEX_FREE()      wc_FreeMutex(&wc_error_mutex)
#define ERRQ_LOCK()            wc_LockMutex(&wc_error_mutex)
#define ERRQ_UNLOCK()          wc_UnLockMutex(&wc_error_mutex)

/* Internal function that is called by wolfCrypt_Init() */
int wc_LoggingInit(void)
{
    if (ERRQ_MUTEX_INIT() != 0) {
        WOLFSSL_MSG("Bad Init Mutex");
        return BAD_MUTEX_E;
    }
    wc_errors_count = 0;
    wc_errors          = NULL;
    wc_current_node    = NULL;
    wc_last_node       = NULL;
    return 0;
}


/* internal function that is called by wolfCrypt_Cleanup */
int wc_LoggingCleanup(void)
{
    /* clear logging entries */
    wc_ClearErrorNodes();
    /* free mutex */
    if (ERRQ_MUTEX_FREE() != 0) {
        WOLFSSL_MSG("Bad Mutex free");
        return BAD_MUTEX_E;
    }
    return 0;
}

static int peekErrorNode(int idx, const char **file, const char **reason,
        int *line)
{
    struct wc_error_queue* err;

    if (idx < 0) {
        err = wc_last_node;
    }
    else {
        int i;

        err = (struct wc_error_queue*)wc_errors;
        for (i = 0; i < idx; i++) {
            if (err == NULL) {
                WOLFSSL_MSG("Error node not found. Bad index?");
                return BAD_FUNC_ARG;
            }
            err = err->next;
        }
    }

    if (err == NULL) {
        WOLFSSL_MSG("No Errors in queue");
        return BAD_STATE_E;
    }

    if (file != NULL) {
        *file = err->file;
    }
    if (reason != NULL) {
        *reason = err->error;
    }
    if (line != NULL) {
        *line = err->line;
    }

    return err->value;
}

/* peek at an error node
 *
 * idx : if -1 then the most recent node is looked at,
 *       otherwise search through queue for node at the given index starting
 *          from the absolute head wc_errors
 * file  : pointer to internal file string
 * reason : pointer to internal error reason
 * line  : line number that error happened at
 *
 * Returns a negative value in error case, on success returns the nodes error
 * value which is positive (absolute value)
 */
int wc_PeekErrorNode(int idx, const char **file, const char **reason,
        int *line)
{
    int ret;

    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return BAD_MUTEX_E;
    }
    ret = peekErrorNode(idx, file, reason, line);
    ERRQ_UNLOCK();
    return ret;
}

static int pullErrorNode(const char **file, const char **reason, int *line)
{
    struct wc_error_queue* err;
    int value;

    err = wc_current_node;
    if (err == NULL) {
        WOLFSSL_MSG("No Errors in queue");
        return BAD_STATE_E;
    }

    if (file != NULL) {
        *file = err->file;
    }
    if (reason != NULL) {
        *reason = err->error;
    }
    if (line != NULL) {
        *line = err->line;
    }
    value = err->value;
    wc_current_node = err->next;

    return value;
}

/* Pulls the current node from error queue and increments current state.
 * Note: this does not delete nodes because input arguments are pointing to
 *       node buffers.
 *
 * file   pointer to file that error was in. Can be NULL to return no file.
 * reason error string giving reason for error. Can be NULL to return no reason.
 * line   return line number of where error happened.
 *
 * returns the error value on success and BAD_MUTEX_E or BAD_STATE_E on failure
 */
int wc_PullErrorNode(const char **file, const char **reason, int *line)
{
    int value;

    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return BAD_MUTEX_E;
    }

    value = pullErrorNode(file, reason, line);

    ERRQ_UNLOCK();
    return value;
}

/* create new error node and add it to the queue
 * buffers are assumed to be of size WOLFSSL_MAX_ERROR_SZ for this internal
 * function. */
int wc_AddErrorNode(int error, int line, char* buf, char* file)
{
    struct wc_error_queue* err;

    if (wc_errors_count >= ERROR_QUEUE_MAX) {
        WOLFSSL_MSG("Error queue is full, at ERROR_QUEUE_MAX");
        return MEMORY_E;
    }

    err = (struct wc_error_queue*)XMALLOC(
            sizeof(struct wc_error_queue), wc_error_heap, DYNAMIC_TYPE_LOG);
    if (err == NULL) {
        WOLFSSL_MSG("Unable to create error node for log");
        return MEMORY_E;
    }
    else {
        int sz;

        XMEMSET(err, 0, sizeof(struct wc_error_queue));
        err->heap = wc_error_heap;
        sz = (int)XSTRLEN(buf);
        if (sz > WOLFSSL_MAX_ERROR_SZ - 1) {
            sz = WOLFSSL_MAX_ERROR_SZ - 1;
        }
        if (sz > 0) {
            XMEMCPY(err->error, buf, sz);
        }

        sz = (int)XSTRLEN(file);
        if (sz > WOLFSSL_MAX_ERROR_SZ - 1) {
            sz = WOLFSSL_MAX_ERROR_SZ - 1;
        }
        if (sz > 0) {
            XMEMCPY(err->file, file, sz);
        }

        err->value = error;
        err->line  = line;

        /* make sure is terminated */
        err->error[WOLFSSL_MAX_ERROR_SZ - 1] = '\0';
        err->file[WOLFSSL_MAX_ERROR_SZ - 1]  = '\0';


        /* since is queue place new node at last of the list */
        if (wc_last_node == NULL) {
            /* case of first node added to queue */
            if (wc_errors != NULL) {
                /* check for unexpected case before over writing wc_errors */
                WOLFSSL_MSG("ERROR in adding new node to logging queue!!");
                /* In the event both wc_last_node and wc_errors are NULL, err
                 * goes unassigned to external wc_errors, wc_last_node. Free
                 * err in this instance since wc_ClearErrorNodes will not
                 */
                XFREE(err, wc_error_heap, DYNAMIC_TYPE_LOG);
            }
            else {
                wc_errors    = err;
                wc_last_node = err;
                wc_current_node = err;
            }
        }
        else {
            wc_last_node->next = err;
            err->prev = wc_last_node;
            wc_last_node = err;

            /* check the case where have read to the end of the queue and the
             * current node to read needs updated */
            if (wc_current_node == NULL) {
                wc_current_node = err;
            }
        }
        wc_errors_count++;
    }
    return 0;
}

/* returns the current index into the queue, which is the node that
 * wc_current_node is pointing to. It can be greater than zero in cases
 * where wc_PullErrorNode() has been called without the node having been
 * removed. */
static int getErrorNodeCurrentIdx(void)
{
    int ret = 0;
    struct wc_error_queue* current;

    current = (struct wc_error_queue*)wc_errors;
    while (current != wc_current_node && current != NULL) {
        current = current->next;
        ret++;
    }

    /* wc_current_node was not found in the list! use index 0 */
    if (current == NULL) {
        ret = 0;
    }
    return ret;
}

static void removeErrorNode(int idx)
{
    struct wc_error_queue* current;

    if (idx == -1) {
        current = wc_last_node;
    }
    else {
        current = (struct wc_error_queue*)wc_errors;
        for (; current != NULL && idx > 0; idx--)
             current = current->next;
    }
    if (current != NULL) {
        if (current->prev != NULL)
            current->prev->next = current->next;
        if (current->next != NULL)
            current->next->prev = current->prev;
        if (wc_last_node == current)
            wc_last_node = current->prev;
        if (wc_errors == current)
            wc_errors = current->next;
        if (wc_current_node == current)
            wc_current_node = current->next;
        XFREE(current, current->heap, DYNAMIC_TYPE_LOG);
        wc_errors_count--;

        /* last node left in list was free'd, reset list head */
        if (wc_errors_count == 0) {
            wc_errors       = NULL;
            wc_last_node    = NULL;
            wc_current_node = NULL;
        }
    }
}

/* Removes the error node at the specified index.
 * idx : if -1 then the most recent node is looked at,
 *       otherwise search through queue for node at the given index starting
 *          from the absolute head wc_errors
 */
void wc_RemoveErrorNode(int idx)
{
    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return;
    }

    removeErrorNode(idx);

    ERRQ_UNLOCK();
}

static void clearErrorNodes(void)
{
    struct wc_error_queue* current;
    struct wc_error_queue* next;
    /* free all nodes from error queue (even previously 'pulled' ones) starting
     * at the lists absolute head of wc_errors */

    current = (struct wc_error_queue*)wc_errors;
    while (current != NULL) {
        next = current->next;
        XFREE(current, current->heap, DYNAMIC_TYPE_LOG);
        current = next;
    }

    wc_errors_count = 0;
    wc_errors       = NULL;
    wc_last_node    = NULL;
    wc_current_node = NULL;
}

/* Clears out the list of error nodes.
 */
void wc_ClearErrorNodes(void)
{
    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return;
    }

    clearErrorNodes();

    ERRQ_UNLOCK();
}

int wc_SetLoggingHeap(void* h)
{
    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return BAD_MUTEX_E;
    }
    wc_error_heap = h;
    ERRQ_UNLOCK();
    return 0;
}

/* frees all nodes in the queue
 *
 * id  this is the thread id
 */
int wc_ERR_remove_state(void)
{
    struct wc_error_queue* current;
    struct wc_error_queue* next;

    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return BAD_MUTEX_E;
    }

    /* free all nodes from error queue */
    current = (struct wc_error_queue*)wc_errors;
    while (current != NULL) {
        next = current->next;
        XFREE(current, current->heap, DYNAMIC_TYPE_LOG);
        current = next;
    }

    wc_errors_count = 0;
    wc_errors          = NULL;
    wc_last_node       = NULL;

    ERRQ_UNLOCK();

    return 0;
}

unsigned long wc_PeekErrorNodeLineData(const char **file, int *line,
                                       const char **data, int *flags,
                                       int (*ignore_err)(int err))
{
    int idx;

    WOLFSSL_ENTER("wc_PeekErrorNodeLineData");

/* No data or flags stored - error display only in Nginx. */
    if (data != NULL) {
        *data = "";
    }
    if (flags != NULL) {
        *flags = 0;
    }

    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return (unsigned long)(0 - BAD_MUTEX_E);
    }

    idx = getErrorNodeCurrentIdx();
    while (1) {
        int ret = peekErrorNode(idx, file, NULL, line);
        if (ret == BAD_MUTEX_E || ret == BAD_FUNC_ARG || ret == BAD_STATE_E) {
            ERRQ_UNLOCK();
            WOLFSSL_MSG("Issue peeking at error node in queue");
            return 0;
        }
        /* OpenSSL uses positive error codes */
        if (ret < 0) {
            ret = -ret;
        }

        if (ignore_err && ignore_err(ret)) {
          removeErrorNode(idx);
          continue;
        }

        ERRQ_UNLOCK();
        return (unsigned long)ret;
    }
}

unsigned long wc_GetErrorNodeErr(void)
{
    int ret;

    WOLFSSL_ENTER("wc_GetErrorNodeErr");

    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
        return (unsigned long)(0 - BAD_MUTEX_E);
    }

    ret = pullErrorNode(NULL, NULL, NULL);
    if (ret < 0) {
        if (ret == BAD_STATE_E) {
            ret = 0; /* no errors in queue */
        }
        else {
            WOLFSSL_MSG("Error with pulling error node!");
            WOLFSSL_LEAVE("wolfSSL_ERR_get_error", ret);
            ret = 0 - ret; /* return absolute value of error */
            /* panic and try to clear out nodes */
            clearErrorNodes();
        }
    }
    else {
        int idx = getErrorNodeCurrentIdx();
        if (idx < 0) {
            WOLFSSL_MSG("Error with getting current index!");
            ret = BAD_STATE_E;
            WOLFSSL_LEAVE("wolfSSL_ERR_get_error", ret);

            /* panic and try to clear out nodes and reset queue state */
            clearErrorNodes();
        }
        else if (idx > 0) {
            idx -= 1;
            removeErrorNode(idx);
        }
        else {
            /* if current idx is 0 then the queue only had one node */
            removeErrorNode(idx);
        }
    }

    ERRQ_UNLOCK();
    return ret;
}

#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
/* This callback allows the application to provide a custom error printing
 * function. */
void wc_ERR_print_errors_cb(int (*cb)(const char *str, size_t len, void *u),
                            void *u)
{
    WOLFSSL_ENTER("wc_ERR_print_errors_cb");

    if (cb == NULL) {
        /* Invalid param */
        return;
    }

    if (ERRQ_LOCK() != 0) {
        WOLFSSL_MSG("Lock debug mutex failed");
    }
    else {
        /* free all nodes from error queue and print them to file */
        struct wc_error_queue *current;
        struct wc_error_queue *next;

        current = (struct wc_error_queue *)wc_errors;
        while (current != NULL)
        {
            next = current->next;
            cb(current->error, XSTRLEN(current->error), u);
            XFREE(current, current->heap, DYNAMIC_TYPE_LOG);
            current = next;
        }

        /* set global pointers to match having been freed */
        wc_errors_count = 0;
        wc_errors = NULL;
        wc_last_node = NULL;

        ERRQ_UNLOCK();
    }
}
#endif /* !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM) */

#endif /* !ERROR_QUEUE_PER_THREAD */

#else  /* WOLFSSL_HAVE_ERROR_QUEUE */
/* NO ERROR_QUEUE at all */

int wc_LoggingInit(void)
{
    return 0;
}


/* internal function that is called by wolfCrypt_Cleanup */
int wc_LoggingCleanup(void)
{
    return 0;
}

int wc_PeekErrorNode(int idx, const char **file, const char **reason,
        int *line)
{
    (void)idx;
    (void)file;
    (void)reason;
    (void)line;
    WOLFSSL_MSG("Error queue turned off, can not peak nodes");
    return NOT_COMPILED_IN;
}

int wc_PullErrorNode(const char **file, const char **reason, int *line)
{
    (void)file;
    (void)reason;
    (void)line;
    WOLFSSL_MSG("Error queue turned off, can not pull nodes");
    return NOT_COMPILED_IN;
}

int wc_AddErrorNode(int error, int line, char* buf, char* file)
{
    (void)error;
    (void)line;
    (void)buf;
    (void)file;
    WOLFSSL_MSG("Error queue turned off, can not add nodes");
    return NOT_COMPILED_IN;
}

void wc_RemoveErrorNode(int idx)
{
    (void)idx;
    WOLFSSL_MSG("Error queue turned off, can not remove nodes");
}

void wc_ClearErrorNodes(void)
{
    WOLFSSL_MSG("Error queue turned off, can not clear nodes");
}

int wc_SetLoggingHeap(void* h)
{
    (void)h;
    return 0;
}

int wc_ERR_remove_state(void)
{
    return 0;
}

unsigned long wc_PeekErrorNodeLineData(const char **file, int *line,
                                       const char **data, int *flags,
                                       int (*ignore_err)(int err))
{
    WOLFSSL_ENTER("wc_PeekErrorNodeLineData");

    (void)line;
    (void)file;
    (void)ignore_err;
    if (data != NULL) {
        *data = "";
    }
    if (flags != NULL) {
        *flags = 0;
    }
    return (unsigned long)(0 - NOT_COMPILED_IN);
}

unsigned long wc_GetErrorNodeErr(void)
{
    WOLFSSL_ENTER("wc_GetErrorNodeErr");
    return (unsigned long)(0 - NOT_COMPILED_IN);
}

#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
void wc_ERR_print_errors_cb(int (*cb)(const char *str, size_t len, void *u),
                            void *u)
{
    WOLFSSL_ENTER("wc_ERR_print_errors_cb");
    (void)cb;
    (void)u;
}
#endif /* !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM) */

#endif /* !WOLFSSL_HAVE_ERROR_QUEUE */

#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
/* empties out the error queue into the file */
static int wc_ERR_dump_to_file (const char *str, size_t len, void *u)
{
    XFILE fp = (XFILE ) u;
    if (fprintf(fp, "%-*.*s\n", (int)len, (int)len, str) < 0)
        return IO_FAILED_E;
    return 0;
}

void wc_ERR_print_errors_fp(XFILE fp)
{
    WOLFSSL_ENTER("wc_ERR_print_errors_fp");

    /* Send all errors to the wc_ERR_dump_to_file function */
    wc_ERR_print_errors_cb(wc_ERR_dump_to_file, fp);
}

#endif /* !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM) */

#endif /* defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE) */

/*
 * When using OPENSSL_EXTRA or DEBUG_WOLFSSL_VERBOSE macro then WOLFSSL_ERROR is
 * mapped to new function WOLFSSL_ERROR_LINE which gets the line # and function
 * name where WOLFSSL_ERROR is called at.
 */
#if defined(DEBUG_WOLFSSL) || defined(OPENSSL_ALL) || \
    defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) || \
    defined(OPENSSL_EXTRA)

#ifdef WOLFSSL_HAVE_ERROR_QUEUE
void WOLFSSL_ERROR_LINE(int error, const char* func, unsigned int line,
        const char* file, void* usrCtx)
#else
void WOLFSSL_ERROR(int error)
#endif
{
#ifdef WOLFSSL_ASYNC_CRYPT
    if (error != WC_PENDING_E)
#endif
    {
        char buffer[WOLFSSL_MAX_ERROR_SZ];

    #ifdef WOLFSSL_HAVE_ERROR_QUEUE
        (void)usrCtx; /* a user ctx for future flexibility */
        (void)func;

        if (ERRQ_LOCK() != 0) {
            WOLFSSL_MSG("Lock debug mutex failed");
            (void)XSNPRINTF(buffer, sizeof(buffer),
                    "wolfSSL error occurred, error = %d", error);
        }
        else {
            #if defined(OPENSSL_EXTRA) && !defined(WOLFCRYPT_ONLY)
            /* If running in compatibility mode do not add want read and
               want right to error queue */
            if (error != WANT_READ && error != WANT_WRITE) {
            #endif
            if (error < 0)
                error = error - (2 * error); /* get absolute value */
            (void)XSNPRINTF(buffer, sizeof(buffer),
                    "wolfSSL error occurred, error = %d line:%u file:%s",
                    error, line, file);

            if (wc_AddErrorNode(error, (int)line, buffer, (char*)file) != 0) {
                WOLFSSL_MSG("Error creating logging node");
                /* with void function there is no return here, continue on
                 * to unlock mutex and log what buffer was created. */
            }
            #if defined(OPENSSL_EXTRA) && !defined(WOLFCRYPT_ONLY)
            }
            else {
                (void)XSNPRINTF(buffer, sizeof(buffer),
                    "wolfSSL error occurred, error = %d", error);
            }
            #endif

            ERRQ_UNLOCK();
        }
    #else
        (void)XSNPRINTF(buffer, sizeof(buffer),
                "wolfSSL error occurred, error = %d", error);
    #endif

    #ifdef DEBUG_WOLFSSL
        if (loggingEnabled)
            wolfssl_log(ERROR_LOG , buffer);
    #endif
    }
}

void WOLFSSL_ERROR_MSG(const char* msg)
{
#ifdef DEBUG_WOLFSSL
    if (loggingEnabled)
        wolfssl_log(ERROR_LOG , msg);
#else
    (void)msg;
#endif
}

#endif  /* DEBUG_WOLFSSL || WOLFSSL_NGINX || WOLFSSL_HAPROXY */

