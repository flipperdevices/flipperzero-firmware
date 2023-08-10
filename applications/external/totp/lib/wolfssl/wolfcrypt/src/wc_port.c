/* port.c
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
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/wc_port.h>
#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
#endif
#ifdef WOLFSSL_ASYNC_CRYPT
    #include <wolfssl/wolfcrypt/async.h>
#endif

/* IPP header files for library initialization */
#ifdef HAVE_FAST_RSA
    #include <ipp.h>
    #include <ippcp.h>
#endif

#ifdef FREESCALE_LTC_TFM
    #include <wolfssl/wolfcrypt/port/nxp/ksdk_port.h>
#endif

#ifdef WOLFSSL_PSOC6_CRYPTO
    #include <wolfssl/wolfcrypt/port/cypress/psoc6_crypto.h>
#endif

#ifdef MAXQ10XX_MODULE_INIT
    #include <wolfssl/wolfcrypt/port/maxim/maxq10xx.h>
#endif

#if defined(WOLFSSL_ATMEL) || defined(WOLFSSL_ATECC508A) || \
    defined(WOLFSSL_ATECC608A)
    #include <wolfssl/wolfcrypt/port/atmel/atmel.h>
#endif
#if defined(WOLFSSL_RENESAS_TSIP)
    #include <wolfssl/wolfcrypt/port/Renesas/renesas-tsip-crypt.h>
#endif
#if defined(WOLFSSL_RENESAS_SCEPROTECT) || \
            defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
    #include <wolfssl/wolfcrypt/port/Renesas/renesas-sce-crypt.h>
#endif
#if defined(WOLFSSL_RENESAS_RX64_HASH)
    #include <wolfssl/wolfcrypt/port/Renesas/renesas-rx64-hw-crypt.h>
#endif
#if defined(WOLFSSL_STSAFEA100)
    #include <wolfssl/wolfcrypt/port/st/stsafe.h>
#endif

#if (defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER)) \
    && !defined(WOLFCRYPT_ONLY)
    #include <wolfssl/openssl/evp.h>
#endif

#include <wolfssl/wolfcrypt/memory.h>
#if defined(USE_WOLFSSL_MEMORY) && defined(WOLFSSL_TRACK_MEMORY)
    #include <wolfssl/wolfcrypt/mem_track.h>
#endif

#if defined(WOLFSSL_CAAM)
    #include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#endif
#if defined(HAVE_ARIA)
    #include <wolfssl/wolfcrypt/port/aria/aria-cryptocb.h>
#endif
#if defined(WOLFSSL_DEVCRYPTO)
    #include <wolfssl/wolfcrypt/port/devcrypto/wc_devcrypto.h>
#endif
#ifdef WOLFSSL_IMXRT_DCP
    #include <wolfssl/wolfcrypt/port/nxp/dcp_port.h>
#endif

#ifdef WOLF_CRYPTO_CB
    #include <wolfssl/wolfcrypt/cryptocb.h>
#endif

#ifdef HAVE_INTEL_QA_SYNC
    #include <wolfssl/wolfcrypt/port/intel/quickassist_sync.h>
#endif

#ifdef HAVE_CAVIUM_OCTEON_SYNC
    #include <wolfssl/wolfcrypt/port/cavium/cavium_octeon_sync.h>
#endif

#if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_INIT)
#include <wolfssl/wolfcrypt/port/nxp/se050_port.h>
#endif

#ifdef WOLFSSL_SCE
    #include "hal_data.h"
#endif

#if defined(WOLFSSL_DSP) && !defined(WOLFSSL_DSP_BUILD)
    #include "rpcmem.h"
#endif

#ifdef _MSC_VER
    /* 4996 warning to use MS extensions e.g., strcpy_s instead of strncpy */
    #pragma warning(disable: 4996)
#endif

#if defined(WOLFSSL_HAVE_PSA)
    #include <wolfssl/wolfcrypt/port/psa/psa.h>
#endif


/* prevent multiple mutex initializations */
static volatile int initRefCount = 0;

/* Used to initialize state for wolfcrypt
   return 0 on success
 */
WOLFSSL_ABI
int wolfCrypt_Init(void)
{
    int ret = 0;
    if (initRefCount == 0) {
        WOLFSSL_ENTER("wolfCrypt_Init");

    #ifdef WOLFSSL_CHECK_MEM_ZERO
        /* Initialize the mutex for access to the list of memory locations that
         * must be freed. */
        wc_MemZero_Init();
    #endif
    #ifdef WOLFSSL_MEM_FAIL_COUNT
        wc_MemFailCount_Init();
    #endif

    #ifdef WOLFSSL_FORCE_MALLOC_FAIL_TEST
        {
            word32 rngMallocFail;
            time_t seed = time(NULL);
            srand((word32)seed);
            rngMallocFail = rand() % 2000; /* max 2000 */
            fprintf(stderr, "\n--- RNG MALLOC FAIL AT %u ---\n", rngMallocFail);
            wolfSSL_SetMemFailCount(rngMallocFail);
        }
    #endif

    #ifdef WOLF_CRYPTO_CB
        wc_CryptoCb_Init();
    #endif

    #ifdef WOLFSSL_ASYNC_CRYPT
        ret = wolfAsync_HardwareStart();
        if (ret != 0) {
            WOLFSSL_MSG("Async hardware start failed");
            /* don't return failure, allow operation to continue */
        }
    #endif

    #if defined(WOLFSSL_RENESAS_TSIP)
        ret = tsip_Open( );
        if( ret != TSIP_SUCCESS ) {
            WOLFSSL_MSG("RENESAS TSIP Open failed");
            /* not return 1 since WOLFSSL_SUCCESS=1*/
            ret = -1;/* FATAL ERROR */
            return ret;
        }
    #endif

    #if defined(WOLFSSL_RENESAS_RX64_HASH)
    ret = rx64_hw_Open();
    if( ret != 0 ) {
        WOLFSSL_MSG("Renesas RX64 HW Open failed");
        /* not return 1 since WOLFSSL_SUCCESS=1*/
        ret = -1;/* FATAL ERROR */
        return ret;
    }
    #endif

    #if defined(WOLFSSL_RENESAS_SCEPROTECT) || \
            defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
        ret = wc_sce_Open( );
        if( ret != FSP_SUCCESS ) {
            WOLFSSL_MSG("RENESAS SCE Open failed");
            /* not return 1 since WOLFSSL_SUCCESS=1*/
            ret = -1;/* FATAL ERROR */
            return ret;
        }
    #endif

    #if defined(WOLFSSL_TRACK_MEMORY) && !defined(WOLFSSL_STATIC_MEMORY)
        ret = InitMemoryTracker();
        if (ret != 0) {
            WOLFSSL_MSG("InitMemoryTracker failed");
            return ret;
        }
    #endif

    #ifdef WOLFSSL_LINUXKM_USE_SAVE_VECTOR_REGISTERS
        ret = allocate_wolfcrypt_linuxkm_fpu_states();
        if (ret != 0) {
            WOLFSSL_MSG("allocate_wolfcrypt_linuxkm_fpu_states failed");
            return ret;
        }
    #endif

    #if WOLFSSL_CRYPT_HW_MUTEX
        /* If crypto hardware mutex protection is enabled, then initialize it */
        ret = wolfSSL_CryptHwMutexInit();
        if (ret != 0) {
            WOLFSSL_MSG("Hw crypt mutex init failed");
            return ret;
        }
    #endif

    /* if defined have fast RSA then initialize Intel IPP */
    #ifdef HAVE_FAST_RSA
        WOLFSSL_MSG("Attempting to use optimized IPP Library");
        if ((ret = ippInit()) != ippStsNoErr) {
            /* possible to get a CPU feature support status on optimized IPP
              library but still use default library and see competitive speeds */
            WOLFSSL_MSG("Warning when trying to set up optimization");
            WOLFSSL_MSG(ippGetStatusString(ret));
            WOLFSSL_MSG("Using default fast IPP library");
            ret = 0;
            (void)ret; /* suppress not read warning */
        }
    #endif

    #if defined(FREESCALE_LTC_TFM) || defined(FREESCALE_LTC_ECC)
        ret = ksdk_port_init();
        if (ret != 0) {
            WOLFSSL_MSG("KSDK port init failed");
            return ret;
        }
    #endif

    #if defined(WOLFSSL_ATMEL) || defined(WOLFSSL_ATECC508A) || \
        defined(WOLFSSL_ATECC608A)
        ret = atmel_init();
        if (ret != 0) {
            WOLFSSL_MSG("CryptoAuthLib init failed");
            return ret;
        }
    #endif
    #if defined(WOLFSSL_CRYPTOCELL)
        /* enable and initialize the ARM CryptoCell 3xx runtime library */
        ret = cc310_Init();
        if (ret != 0) {
            WOLFSSL_MSG("CRYPTOCELL init failed");
            return ret;
        }
    #endif
    #if defined(WOLFSSL_STSAFEA100)
        stsafe_interface_init();
    #endif

    #if defined(WOLFSSL_PSOC6_CRYPTO)
        ret = psoc6_crypto_port_init();
        if (ret != 0) {
            WOLFSSL_MSG("PSoC6 crypto engine init failed");
            return ret;
        }
    #endif

    #ifdef MAXQ10XX_MODULE_INIT
        ret = maxq10xx_port_init();
        if (ret != 0) {
            WOLFSSL_MSG("MAXQ10xx port init failed");
            return ret;
        }
    #endif

    #ifdef WOLFSSL_SILABS_SE_ACCEL
        /* init handles if it is already initialized */
        ret = sl_se_init();
    #endif

    #if defined(WOLFSSL_SE050) && defined(WOLFSSL_SE050_INIT)
        ret = wc_se050_init(NULL);
        if (ret != 0) {
            WOLFSSL_MSG("SE050 init failed");
            return ret;
        }
    #endif

    #ifdef WOLFSSL_ARMASM
        WOLFSSL_MSG("Using ARM hardware acceleration");
    #endif

    #ifdef WOLFSSL_AFALG
        WOLFSSL_MSG("Using AF_ALG for crypto acceleration");
    #endif

    #if !defined(WOLFCRYPT_ONLY) && defined(OPENSSL_EXTRA)
        wolfSSL_EVP_init();
    #endif

    #if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
        if ((ret = wc_LoggingInit()) != 0) {
            WOLFSSL_MSG("Error creating logging mutex");
            return ret;
        }
    #endif

    #if defined(WOLFSSL_HAVE_PSA)
        if ((ret = wc_psa_init()) != 0)
            return ret;
    #endif

#ifdef HAVE_ENTROPY_MEMUSE
    ret = Entropy_Init();
    if (ret != 0) {
        WOLFSSL_MSG("Error initializing entropy");
        return ret;
    }
#endif

#ifdef HAVE_ECC
    #ifdef FP_ECC
        wc_ecc_fp_init();
    #endif
    #ifdef ECC_CACHE_CURVE
        if ((ret = wc_ecc_curve_cache_init()) != 0) {
            WOLFSSL_MSG("Error creating curve cache");
            return ret;
        }
    #endif
#endif

#ifdef WOLFSSL_SCE
        ret = (int)WOLFSSL_SCE_GSCE_HANDLE.p_api->open(
                WOLFSSL_SCE_GSCE_HANDLE.p_ctrl, WOLFSSL_SCE_GSCE_HANDLE.p_cfg);
        if (ret == SSP_ERR_CRYPTO_SCE_ALREADY_OPEN) {
            WOLFSSL_MSG("SCE already open");
            ret = 0;
        }
        if (ret != SSP_SUCCESS) {
            WOLFSSL_MSG("Error opening SCE");
            return -1; /* FATAL_ERROR */
        }
#endif

#if defined(WOLFSSL_DEVCRYPTO)
        if ((ret = wc_DevCryptoInit()) != 0) {
            return ret;
        }
#endif

#if defined(WOLFSSL_CAAM)
        if ((ret = wc_caamInit()) != 0) {
            return ret;
        }
#endif

#if defined(HAVE_ARIA)
        if ((ret = wc_AriaInit()) != 0) {
            return ret;
        }
#endif

#ifdef WOLFSSL_IMXRT_DCP
        if ((ret = wc_dcp_init()) != 0) {
            return ret;
        }
#endif

#if defined(WOLFSSL_DSP) && !defined(WOLFSSL_DSP_BUILD)
        if ((ret = wolfSSL_InitHandle()) != 0) {
            return ret;
        }
        rpcmem_init();
#endif
    }
    initRefCount++;

    return ret;
}

#if defined(WOLFSSL_TRACK_MEMORY_VERBOSE) && !defined(WOLFSSL_STATIC_MEMORY)
long wolfCrypt_heap_peakAllocs_checkpoint(void) {
    long ret = ourMemStats.peakAllocsTripOdometer;
    ourMemStats.peakAllocsTripOdometer = ourMemStats.totalAllocs -
        ourMemStats.totalDeallocs;
    return ret;
}
long wolfCrypt_heap_peakBytes_checkpoint(void) {
    long ret = ourMemStats.peakBytesTripOdometer;
    ourMemStats.peakBytesTripOdometer = ourMemStats.currentBytes;
    return ret;
}
#endif

/* return success value is the same as wolfCrypt_Init */
WOLFSSL_ABI
int wolfCrypt_Cleanup(void)
{
    int ret = 0;

    initRefCount--;
    if (initRefCount < 0)
        initRefCount = 0;

    if (initRefCount == 0) {
        WOLFSSL_ENTER("wolfCrypt_Cleanup");

#ifdef HAVE_ECC
    #ifdef FP_ECC
        wc_ecc_fp_free();
    #endif
    #ifdef ECC_CACHE_CURVE
        wc_ecc_curve_cache_free();
    #endif
#endif /* HAVE_ECC */

    #if defined(OPENSSL_EXTRA) || defined(DEBUG_WOLFSSL_VERBOSE)
        ret = wc_LoggingCleanup();
    #endif

    #if defined(WOLFSSL_TRACK_MEMORY) && !defined(WOLFSSL_STATIC_MEMORY)
        ShowMemoryTracker();
    #endif

    #ifdef WOLFSSL_ASYNC_CRYPT
        wolfAsync_HardwareStop();
    #endif

    #ifdef WOLFSSL_RENESAS_TSIP
        tsip_Close();
    #endif

    #if defined(WOLFSSL_RENESAS_RX64_HASH)
        rx64_hw_Close();
    #endif

    #if defined(WOLFSSL_RENESAS_SCEPROTECT) || \
        defined(WOLFSSL_RENESAS_SCEPROTECT_CRYPTONLY)
        wc_sce_Close();
    #endif

    #ifdef WOLFSSL_SCE
        WOLFSSL_SCE_GSCE_HANDLE.p_api->close(WOLFSSL_SCE_GSCE_HANDLE.p_ctrl);
    #endif

    #if defined(WOLFSSL_CAAM)
        wc_caamFree();
    #endif
    #if defined(WOLFSSL_CRYPTOCELL)
        cc310_Free();
    #endif
    #ifdef WOLFSSL_SILABS_SE_ACCEL
        ret = sl_se_deinit();
    #endif
    #if defined(WOLFSSL_RENESAS_TSIP)
        tsip_Close();
    #endif
    #if defined(WOLFSSL_DEVCRYPTO)
        wc_DevCryptoCleanup();
    #endif
    #if defined(WOLFSSL_DSP) && !defined(WOLFSSL_DSP_BUILD)
        rpcmem_deinit();
        wolfSSL_CleanupHandle();
    #endif
    #ifdef WOLFSSL_LINUXKM_USE_SAVE_VECTOR_REGISTERS
        free_wolfcrypt_linuxkm_fpu_states();
    #endif

    #ifdef HAVE_ENTROPY_MEMUSE
        Entropy_Final();
    #endif

    #ifdef WOLF_CRYPTO_CB
        wc_CryptoCb_Cleanup();
    #endif

    #if defined(WOLFSSL_MEM_FAIL_COUNT) && defined(WOLFCRYPT_ONLY)
        wc_MemFailCount_Free();
    #endif
    #ifdef WOLFSSL_CHECK_MEM_ZERO
        /* Free the mutex for access to the list of memory locations that
         * must be freed. */
        wc_MemZero_Free();
    #endif
    }

    return ret;
}

#ifndef NO_FILESYSTEM

/* Helpful function to load file into allocated buffer */
int wc_FileLoad(const char* fname, unsigned char** buf, size_t* bufLen,
    void* heap)
{
    int ret;
    ssize_t fileSz;
    XFILE f;

    if (fname == NULL || buf == NULL || bufLen == NULL) {
        return BAD_FUNC_ARG;
    }

    /* set defaults */
    *buf = NULL;
    *bufLen = 0;

    /* open file (read-only binary) */
    f = XFOPEN(fname, "rb");
    if (!f) {
        WOLFSSL_MSG("wc_LoadFile file load error");
        return BAD_PATH_ERROR;
    }

    if (XFSEEK(f, 0, XSEEK_END) != 0) {
        WOLFSSL_MSG("wc_LoadFile file seek error");
        XFCLOSE(f);
        return BAD_PATH_ERROR;
    }
    fileSz = XFTELL(f);
    if (fileSz < 0) {
        WOLFSSL_MSG("wc_LoadFile ftell error");
        XFCLOSE(f);
        return BAD_PATH_ERROR;
    }
    if (XFSEEK(f, 0, XSEEK_SET) != 0) {
        WOLFSSL_MSG("wc_LoadFile file seek error");
        XFCLOSE(f);
        return BAD_PATH_ERROR;
    }
    if (fileSz > 0) {
        *bufLen = (size_t)fileSz;
        *buf = (byte*)XMALLOC(*bufLen, heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (*buf == NULL) {
            WOLFSSL_MSG("wc_LoadFile memory error");
            ret = MEMORY_E;
        }
        else {
            size_t readLen = XFREAD(*buf, 1, *bufLen, f);

            /* check response code */
            ret = (readLen == *bufLen) ? 0 : -1;
        }
    }
    else {
        ret = BUFFER_E;
    }
    XFCLOSE(f);

    (void)heap;

    return ret;
}

#if !defined(NO_WOLFSSL_DIR) && \
    !defined(WOLFSSL_NUCLEUS) && !defined(WOLFSSL_NUCLEUS_1_2)
/* File Handling Helper */
/* returns 0 if file exists, WC_ISFILEEXIST_NOFILE if file doesn't exist */
int wc_FileExists(const char* fname)
{
    struct ReadDirCtx ctx;

    XMEMSET(&ctx, 0, sizeof(ctx));

    if (fname == NULL)
        return 0;

    if (XSTAT(fname, &ctx.s) != 0) {
         WOLFSSL_MSG("stat on name failed");
         return BAD_PATH_ERROR;
    } else {
#if defined(USE_WINDOWS_API)
        if (XS_ISREG(ctx.s.st_mode)) {
            return 0;
        }
#elif defined(WOLFSSL_ZEPHYR)
        if (XS_ISREG(ctx.s.type)) {
            return 0;
        }
#elif defined(WOLFSSL_TELIT_M2MB)
        if (XS_ISREG(ctx.s.st_mode)) {
            return 0;
        }
#else
        if (XS_ISREG(ctx.s.st_mode)) {
            return 0;
        }
#endif
    }
    return WC_ISFILEEXIST_NOFILE;
}

/* File Handling Helpers */
/* returns 0 if file found, WC_READDIR_NOFILE if no files or negative error */
int wc_ReadDirFirst(ReadDirCtx* ctx, const char* path, char** name)
{
    int ret = WC_READDIR_NOFILE; /* default to no files found */
    int pathLen = 0;

    if (name)
        *name = NULL;

    if (ctx == NULL || path == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(ctx, 0, sizeof(ReadDirCtx));
    pathLen = (int)XSTRLEN(path);

#ifdef USE_WINDOWS_API
    if (pathLen > MAX_FILENAME_SZ - 3)
        return BAD_PATH_ERROR;

    XSTRNCPY(ctx->name, path, MAX_FILENAME_SZ - 3);
    XSTRNCPY(ctx->name + pathLen, "\\*", (size_t)(MAX_FILENAME_SZ - pathLen));

    ctx->hFind = FindFirstFileA(ctx->name, &ctx->FindFileData);
    if (ctx->hFind == INVALID_HANDLE_VALUE) {
        WOLFSSL_MSG("FindFirstFile for path verify locations failed");
        return BAD_PATH_ERROR;
    }

    do {
        if (!(ctx->FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            int dnameLen = (int)XSTRLEN(ctx->FindFileData.cFileName);

            if (pathLen + dnameLen + 2 > MAX_FILENAME_SZ) {
                return BAD_PATH_ERROR;
            }
            XSTRNCPY(ctx->name, path, (size_t)pathLen + 1);
            ctx->name[pathLen] = '\\';
            XSTRNCPY(ctx->name + pathLen + 1,
                     ctx->FindFileData.cFileName,
                     (size_t)(MAX_FILENAME_SZ - pathLen - 1));
            if (name)
                *name = ctx->name;
            return 0;
        }
    } while (FindNextFileA(ctx->hFind, &ctx->FindFileData));

#elif defined(INTIME_RTOS)
    if (pathLen > MAX_FILENAME_SZ - 3)
        return BAD_PATH_ERROR;

    XSTRNCPY(ctx->name, path, MAX_FILENAME_SZ - 3);
    XSTRNCPY(ctx->name + pathLen, "\\*", MAX_FILENAME_SZ - pathLen);

    if (!IntimeFindFirst(ctx->name, &ctx->FindFileData)) {
        WOLFSSL_MSG("FindFirstFile for path verify locations failed");
        return BAD_PATH_ERROR;
    }

    do {
        int dnameLen = (int)XSTRLEN(IntimeFilename(ctx));

        if (pathLen + dnameLen + 2 > MAX_FILENAME_SZ) {
            return BAD_PATH_ERROR;
        }
        XSTRNCPY(ctx->name, path, pathLen + 1);
        ctx->name[pathLen] = '\\';
        XSTRNCPY(ctx->name + pathLen + 1,
                 IntimeFilename(ctx),
                 MAX_FILENAME_SZ - pathLen - 1);
        if (0 == wc_FileExists(ctx->name)) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    } while (IntimeFindNext(&ctx->FindFileData));

#elif defined(WOLFSSL_ZEPHYR)
    if (fs_opendir(&ctx->dir, path) != 0) {
        WOLFSSL_MSG("opendir path verify locations failed");
        return BAD_PATH_ERROR;
    }
    ctx->dirp = &ctx->dir;

    while ((fs_readdir(&ctx->dir, &ctx->entry)) != 0) {
        int dnameLen = (int)XSTRLEN(ctx->entry.name);

        if (pathLen + dnameLen + 2 >= MAX_FILENAME_SZ) {
            ret = BAD_PATH_ERROR;
            break;
        }
        XSTRNCPY(ctx->name, path, pathLen + 1);
        ctx->name[pathLen] = '/';

        /* Use dnameLen + 1 for GCC 8 warnings of truncating d_name. Because
         * of earlier check it is known that dnameLen is less than
         * MAX_FILENAME_SZ - (pathLen + 2)  so dnameLen +1 will fit */
        XSTRNCPY(ctx->name + pathLen + 1, ctx->entry.name, dnameLen + 1);
        if ((ret = wc_FileExists(ctx->name)) == 0) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    }
#elif defined(WOLFSSL_TELIT_M2MB)
    ctx->dir = m2mb_fs_opendir((const CHAR*)path);
    if (ctx->dir == NULL) {
        WOLFSSL_MSG("opendir path verify locations failed");
        return BAD_PATH_ERROR;
    }

    while ((ctx->entry = m2mb_fs_readdir(ctx->dir)) != NULL) {
        int dnameLen = (int)XSTRLEN(ctx->entry->d_name);

        if (pathLen + dnameLen + 2 >= MAX_FILENAME_SZ) {
            ret = BAD_PATH_ERROR;
            break;
        }
        XSTRNCPY(ctx->name, path, pathLen + 1);
        ctx->name[pathLen] = '/';

        /* Use dnameLen + 1 for GCC 8 warnings of truncating d_name. Because
         * of earlier check it is known that dnameLen is less than
         * MAX_FILENAME_SZ - (pathLen + 2)  so dnameLen +1 will fit */
        XSTRNCPY(ctx->name + pathLen + 1, ctx->entry->d_name, dnameLen + 1);

        if ((ret = wc_FileExists(ctx->name)) == 0) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    }
#else
    ctx->dir = opendir(path);
    if (ctx->dir == NULL) {
        WOLFSSL_MSG("opendir path verify locations failed");
        return BAD_PATH_ERROR;
    }

    while ((ctx->entry = readdir(ctx->dir)) != NULL) {
        int dnameLen = (int)XSTRLEN(ctx->entry->d_name);

        if (pathLen + dnameLen + 2 >= MAX_FILENAME_SZ) {
            ret = BAD_PATH_ERROR;
            break;
        }
        XSTRNCPY(ctx->name, path, (size_t)pathLen + 1);
        ctx->name[pathLen] = '/';

        /* Use dnameLen + 1 for GCC 8 warnings of truncating d_name. Because
         * of earlier check it is known that dnameLen is less than
         * MAX_FILENAME_SZ - (pathLen + 2)  so dnameLen +1 will fit */
        XSTRNCPY(ctx->name + pathLen + 1, ctx->entry->d_name, (size_t)dnameLen + 1);
        if ((ret = wc_FileExists(ctx->name)) == 0) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    }
#endif
    wc_ReadDirClose(ctx);

    return ret;
}

/* returns 0 if file found, WC_READDIR_NOFILE if no more files */
int wc_ReadDirNext(ReadDirCtx* ctx, const char* path, char** name)
{
    int ret = WC_READDIR_NOFILE; /* default to no file found */
    int pathLen = 0;

    if (name)
        *name = NULL;

    if (ctx == NULL || path == NULL) {
        return BAD_FUNC_ARG;
    }

    XMEMSET(ctx->name, 0, MAX_FILENAME_SZ);
    pathLen = (int)XSTRLEN(path);

#ifdef USE_WINDOWS_API
    while (FindNextFileA(ctx->hFind, &ctx->FindFileData)) {
        if (!(ctx->FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            int dnameLen = (int)XSTRLEN(ctx->FindFileData.cFileName);

            if (pathLen + dnameLen + 2 > MAX_FILENAME_SZ) {
                return BAD_PATH_ERROR;
            }
            XSTRNCPY(ctx->name, path, (size_t)pathLen + 1);
            ctx->name[pathLen] = '\\';
            XSTRNCPY(ctx->name + pathLen + 1,
                     ctx->FindFileData.cFileName,
                     (size_t)(MAX_FILENAME_SZ - pathLen - 1));
            if (name)
                *name = ctx->name;
            return 0;
        }
    }

#elif defined(INTIME_RTOS)
    while (IntimeFindNext(&ctx->FindFileData)) {
        int dnameLen = (int)XSTRLEN(IntimeFilename(ctx));

        if (pathLen + dnameLen + 2 > MAX_FILENAME_SZ) {
            return BAD_PATH_ERROR;
        }
        XSTRNCPY(ctx->name, path, pathLen + 1);
        ctx->name[pathLen] = '\\';
        XSTRNCPY(ctx->name + pathLen + 1,
                 IntimeFilename(ctx),
                 MAX_FILENAME_SZ - pathLen - 1);
        if (0 == wc_FileExists(ctx->name)) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    }

#elif defined(WOLFSSL_ZEPHYR)
    while ((fs_readdir(&ctx->dir, &ctx->entry)) != 0) {
        int dnameLen = (int)XSTRLEN(ctx->entry.name);

        if (pathLen + dnameLen + 2 >= MAX_FILENAME_SZ) {
            ret = BAD_PATH_ERROR;
            break;
        }
        XSTRNCPY(ctx->name, path, pathLen + 1);
        ctx->name[pathLen] = '/';
        /* Use dnameLen + 1 for GCC 8 warnings of truncating d_name. Because
         * of earlier check it is known that dnameLen is less than
         * MAX_FILENAME_SZ - (pathLen + 2) so that dnameLen +1 will fit */
        XSTRNCPY(ctx->name + pathLen + 1, ctx->entry.name, dnameLen + 1);

       if ((ret = wc_FileExists(ctx->name)) == 0) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    }
#elif defined(WOLFSSL_TELIT_M2MB)
    while ((ctx->entry = m2mb_fs_readdir(ctx->dir)) != NULL) {
        int dnameLen = (int)XSTRLEN(ctx->entry->d_name);

        if (pathLen + dnameLen + 2 >= MAX_FILENAME_SZ) {
            ret = BAD_PATH_ERROR;
            break;
        }
        XSTRNCPY(ctx->name, path, pathLen + 1);
        ctx->name[pathLen] = '/';

        /* Use dnameLen + 1 for GCC 8 warnings of truncating d_name. Because
         * of earlier check it is known that dnameLen is less than
         * MAX_FILENAME_SZ - (pathLen + 2)  so dnameLen +1 will fit */
        XSTRNCPY(ctx->name + pathLen + 1, ctx->entry->d_name, dnameLen + 1);

        if ((ret = wc_FileExists(ctx->name)) == 0) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    }
#else
    while ((ctx->entry = readdir(ctx->dir)) != NULL) {
        int dnameLen = (int)XSTRLEN(ctx->entry->d_name);

        if (pathLen + dnameLen + 2 >= MAX_FILENAME_SZ) {
            ret = BAD_PATH_ERROR;
            break;
        }
        XSTRNCPY(ctx->name, path, (size_t)pathLen + 1);
        ctx->name[pathLen] = '/';
        /* Use dnameLen + 1 for GCC 8 warnings of truncating d_name. Because
         * of earlier check it is known that dnameLen is less than
         * MAX_FILENAME_SZ - (pathLen + 2) so that dnameLen +1 will fit */
        XSTRNCPY(ctx->name + pathLen + 1, ctx->entry->d_name, (size_t)dnameLen + 1);

        if ((ret = wc_FileExists(ctx->name)) == 0) {
            if (name)
                *name = ctx->name;
            return 0;
        }
    }
#endif

    wc_ReadDirClose(ctx);

    return ret;
}

void wc_ReadDirClose(ReadDirCtx* ctx)
{
    if (ctx == NULL) {
        return;
    }

#ifdef USE_WINDOWS_API
    if (ctx->hFind != INVALID_HANDLE_VALUE) {
        FindClose(ctx->hFind);
        ctx->hFind = INVALID_HANDLE_VALUE;
    }

#elif defined(INTIME_RTOS)
    IntimeFindClose(&ctx->FindFileData);

#elif defined(WOLFSSL_ZEPHYR)
    if (ctx->dirp) {
        fs_closedir(ctx->dirp);
        ctx->dirp = NULL;
    }
#elif defined(WOLFSSL_TELIT_M2MB)
    if (ctx->dir) {
        m2mb_fs_closedir(ctx->dir);
        ctx->dir = NULL;
    }
#else
    if (ctx->dir) {
        if (closedir(ctx->dir) < 0)
            WOLFSSL_MSG("closedir() failed");
        ctx->dir = NULL;
    }
#endif
}

#endif /* !NO_WOLFSSL_DIR */
#endif /* !NO_FILESYSTEM */

#if !defined(NO_FILESYSTEM) && defined(WOLFSSL_ZEPHYR)
XFILE z_fs_open(const char* filename, const char* mode)
{
    XFILE file;
    fs_mode_t flags = 0;

    if (mode == NULL)
        return NULL;

    /* Parse mode */
    switch (*mode++) {
        case 'r':
            flags |= FS_O_READ;
            break;
        case 'w':
            flags |= FS_O_WRITE|FS_O_CREATE;
            break;
        case 'a':
            flags |= FS_O_APPEND|FS_O_CREATE;
            break;
        default:
            return NULL;
    }

    /* Ignore binary flag */
    if (*mode == 'b')
        mode++;
    if (*mode == '+') {
        flags |= FS_O_READ;
        /* Don't add write flag if already appending */
        if (!(flags & FS_O_APPEND))
            flags |= FS_O_RDWR;
    }
    /* Ignore binary flag */
    if (*mode == 'b')
        mode++;
    /* Incorrect mode string */
    if (*mode != '\0')
        return NULL;

    file = (XFILE)XMALLOC(sizeof(*file), NULL, DYNAMIC_TYPE_FILE);
    if (file != NULL) {
        if (fs_open(file, filename, flags) != 0) {
            XFREE(file, NULL, DYNAMIC_TYPE_FILE);
            file = NULL;
        }
    }

    return file;
}

int z_fs_close(XFILE file)
{
    int ret;

    if (file == NULL)
        return -1;
    ret = (fs_close(file) == 0) ? 0 : -1;

    XFREE(file, NULL, DYNAMIC_TYPE_FILE);

    return ret;
}

#endif /* !NO_FILESYSTEM && !WOLFSSL_ZEPHYR */

#if !defined(WOLFSSL_USER_MUTEX)
wolfSSL_Mutex* wc_InitAndAllocMutex(void)
{
    wolfSSL_Mutex* m = (wolfSSL_Mutex*) XMALLOC(sizeof(wolfSSL_Mutex), NULL,
            DYNAMIC_TYPE_MUTEX);
    if (m != NULL) {
        if (wc_InitMutex(m) != 0) {
            WOLFSSL_MSG("Init Mutex failed");
            XFREE(m, NULL, DYNAMIC_TYPE_MUTEX);
            m = NULL;
        }
    }
    else {
        WOLFSSL_MSG("Memory error with Mutex allocation");
    }

    return m;
}
#endif

#ifdef USE_WOLF_STRTOK
/* String token (delim) search. If str is null use nextp. */
char* wc_strtok(char *str, const char *delim, char **nextp)
{
    char* ret;
    int i, j;

    /* Use next if str is NULL */
    if (str == NULL && nextp)
        str = *nextp;

    /* verify str input */
    if (str == NULL || *str == '\0')
        return NULL;

    /* match on entire delim */
    for (i = 0; str[i]; i++) {
        for (j = 0; delim[j]; j++) {
            if (delim[j] == str[i])
                break;
        }
        if (!delim[j])
            break;
    }
    str += i;
    /* if end of string, not found so return NULL */
    if (*str == '\0')
        return NULL;

    ret = str;

    /* match on first delim */
    for (i = 0; str[i]; i++) {
        for (j = 0; delim[j]; j++) {
            if (delim[j] == str[i])
                break;
        }
        if (delim[j] == str[i])
            break;
    }
    str += i;

    /* null terminate found string */
    if (*str)
        *str++ = '\0';

    /* return pointer to next */
    if (nextp)
        *nextp = str;

    return ret;
}
#endif /* USE_WOLF_STRTOK */

#ifdef USE_WOLF_STRSEP
char* wc_strsep(char **stringp, const char *delim)
{
    char *s, *tok;
    const char *spanp;

    /* null check */
    if (stringp == NULL || *stringp == NULL)
        return NULL;

    s = *stringp;
    for (tok = s; *tok; ++tok) {
        for (spanp = delim; *spanp; ++spanp) {
            /* found delimiter */
            if (*tok == *spanp) {
                *tok = '\0'; /* replace delim with null term */
                *stringp = tok + 1; /* return past delim */
                return s;
            }
        }
    }

    *stringp = NULL;
    return s;
}
#endif /* USE_WOLF_STRSEP */

#ifdef USE_WOLF_STRLCPY
size_t wc_strlcpy(char *dst, const char *src, size_t dstSize)
{
    size_t i;

    if (!dstSize)
        return 0;

    /* Always have to leave a space for NULL */
    for (i = 0; i < (dstSize - 1) && *src != '\0'; i++) {
        *dst++ = *src++;
    }
    *dst = '\0';

    return i; /* return length without NULL */
}
#endif /* USE_WOLF_STRLCPY */

#ifdef USE_WOLF_STRLCAT
size_t wc_strlcat(char *dst, const char *src, size_t dstSize)
{
    size_t dstLen;

    if (!dstSize)
        return 0;

    dstLen = XSTRLEN(dst);

    if (dstSize < dstLen)
        return dstLen + XSTRLEN(src);

    return dstLen + wc_strlcpy(dst + dstLen, src, dstSize - dstLen);
}
#endif /* USE_WOLF_STRLCAT */

#ifdef USE_WOLF_STRCASECMP
int wc_strcasecmp(const char *s1, const char *s2)
{
    char c1, c2;
    for (;;++s1, ++s2) {
        c1 = *s1;
        if ((c1 >= 'a') && (c1 <= 'z'))
            c1 -= ('a' - 'A');
        c2 = *s2;
        if ((c2 >= 'a') && (c2 <= 'z'))
            c2 -= ('a' - 'A');
        if ((c1 != c2) || (c1 == 0))
            break;
    }
    return (c1 - c2);
}
#endif /* USE_WOLF_STRCASECMP */

#ifdef USE_WOLF_STRNCASECMP
int wc_strncasecmp(const char *s1, const char *s2, size_t n)
{
    char c1, c2;
    for (c1 = 0, c2 = 0; n > 0; --n, ++s1, ++s2) {
        c1 = *s1;
        if ((c1 >= 'a') && (c1 <= 'z'))
            c1 -= ('a' - 'A');
        c2 = *s2;
        if ((c2 >= 'a') && (c2 <= 'z'))
            c2 -= ('a' - 'A');
        if ((c1 != c2) || (c1 == 0))
            break;
    }
    return (c1 - c2);
}
#endif /* USE_WOLF_STRNCASECMP */

#ifdef WOLFSSL_ATOMIC_OPS

#ifdef HAVE_C___ATOMIC
/* Atomic ops using standard C lib */
#ifdef __cplusplus
/* C++ using direct calls to compiler built-in functions */
void wolfSSL_Atomic_Int_Init(wolfSSL_Atomic_Int* c, int i)
{
    *c = i;
}

int wolfSSL_Atomic_Int_FetchAdd(wolfSSL_Atomic_Int* c, int i)
{
    return __atomic_fetch_add(c, i, __ATOMIC_RELAXED);
}

int wolfSSL_Atomic_Int_FetchSub(wolfSSL_Atomic_Int* c, int i)
{
    return __atomic_fetch_sub(c, i, __ATOMIC_RELAXED);
}
#else
/* Default C Implementation */
void wolfSSL_Atomic_Int_Init(wolfSSL_Atomic_Int* c, int i)
{
    atomic_init(c, i);
}

int wolfSSL_Atomic_Int_FetchAdd(wolfSSL_Atomic_Int* c, int i)
{
    return atomic_fetch_add_explicit(c, i, memory_order_relaxed);
}

int wolfSSL_Atomic_Int_FetchSub(wolfSSL_Atomic_Int* c, int i)
{
    return atomic_fetch_sub_explicit(c, i, memory_order_relaxed);
}
#endif /* __cplusplus */

#elif defined(_MSC_VER)

/* Default C Implementation */
void wolfSSL_Atomic_Int_Init(wolfSSL_Atomic_Int* c, int i)
{
    *c = i;
}

int wolfSSL_Atomic_Int_FetchAdd(wolfSSL_Atomic_Int* c, int i)
{
    return (int)_InterlockedExchangeAdd(c, (long)i);
}

int wolfSSL_Atomic_Int_FetchSub(wolfSSL_Atomic_Int* c, int i)
{
    return (int)_InterlockedExchangeAdd(c, (long)-i);
}

#endif

#endif /* WOLFSSL_ATOMIC_OPS */

#if !defined(SINGLE_THREADED) && !defined(WOLFSSL_ATOMIC_OPS)
void wolfSSL_RefInit(wolfSSL_Ref* ref, int* err)
{
    int ret = wc_InitMutex(&ref->mutex);
    if (ret != 0) {
        WOLFSSL_MSG("Failed to create mutex for reference counting!");
    }
    ref->count = 1;

    *err = ret;
}

void wolfSSL_RefFree(wolfSSL_Ref* ref)
{
    if (wc_FreeMutex(&ref->mutex) != 0) {
        WOLFSSL_MSG("Failed to free mutex of reference counting!");
    }
}

void wolfSSL_RefInc(wolfSSL_Ref* ref, int* err)
{
    int ret = wc_LockMutex(&ref->mutex);
    if (ret != 0) {
        WOLFSSL_MSG("Failed to lock mutex for reference increment!");
    }
    else {
        ref->count++;
        wc_UnLockMutex(&ref->mutex);
    }
    *err = ret;
}

void wolfSSL_RefDec(wolfSSL_Ref* ref, int* isZero, int* err)
{
    int ret = wc_LockMutex(&ref->mutex);
    if (ret != 0) {
        WOLFSSL_MSG("Failed to lock mutex for reference decrement!");
        /* Can't say count is zero. */
        *isZero = 0;
    }
    else {
        if (ref->count > 0) {
            ref->count--;
        }
        *isZero = (ref->count == 0);
        wc_UnLockMutex(&ref->mutex);
    }
    *err = ret;
}
#endif

#if WOLFSSL_CRYPT_HW_MUTEX
/* Mutex for protection of cryptography hardware */
static wolfSSL_Mutex wcCryptHwMutex;
static int wcCryptHwMutexInit = 0;

int wolfSSL_CryptHwMutexInit(void)
{
    int ret = 0;
    if (wcCryptHwMutexInit == 0) {
        ret = wc_InitMutex(&wcCryptHwMutex);
        if (ret == 0) {
            wcCryptHwMutexInit = 1;
        }
    }
    return ret;
}
int wolfSSL_CryptHwMutexLock(void)
{
    int ret = BAD_MUTEX_E;
    /* Make sure HW Mutex has been initialized */
    ret = wolfSSL_CryptHwMutexInit();
    if (ret == 0) {
        ret = wc_LockMutex(&wcCryptHwMutex);
    }
    return ret;
}
int wolfSSL_CryptHwMutexUnLock(void)
{
    int ret = BAD_MUTEX_E;
    if (wcCryptHwMutexInit) {
        ret = wc_UnLockMutex(&wcCryptHwMutex);
    }
    return ret;
}
#endif /* WOLFSSL_CRYPT_HW_MUTEX */


/* ---------------------------------------------------------------------------*/
/* Mutex Ports */
/* ---------------------------------------------------------------------------*/
#if defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER)
    static mutex_cb*     compat_mutex_cb = NULL;

    /* Function that locks or unlocks a mutex based on the flag passed in.
     *
     * flag lock or unlock i.e. CRYPTO_LOCK
     * type the type of lock to unlock or lock
     * file name of the file calling
     * line the line number from file calling
     */
    int wc_LockMutex_ex(int flag, int type, const char* file, int line)
    {
        if (compat_mutex_cb != NULL) {
            compat_mutex_cb(flag, type, file, line);
            return 0;
        }
        else {
            WOLFSSL_MSG("Mutex call back function not set. Call wc_SetMutexCb");
            return BAD_STATE_E;
        }
    }


    /* Set the callback function to use for locking/unlocking mutex
     *
     * cb callback function to use
     */
    int wc_SetMutexCb(mutex_cb* cb)
    {
        compat_mutex_cb = cb;
        return 0;
    }

    /* Gets the current callback function in use for locking/unlocking mutex
     *
     */
    mutex_cb* wc_GetMutexCb(void)
    {
        return compat_mutex_cb;
    }
#endif /* defined(OPENSSL_EXTRA) || defined(HAVE_WEBSERVER) */
#ifdef SINGLE_THREADED

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        (void)m;
        return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex *m)
    {
        (void)m;
        return 0;
    }


    int wc_LockMutex(wolfSSL_Mutex *m)
    {
        (void)m;
        return 0;
    }


    int wc_UnLockMutex(wolfSSL_Mutex *m)
    {
        (void)m;
        return 0;
    }

#elif defined(FREERTOS) || defined(FREERTOS_TCP) || \
  defined(FREESCALE_FREE_RTOS)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int iReturn;

        *m = ( wolfSSL_Mutex ) xSemaphoreCreateMutex();
        if( *m != NULL )
            iReturn = 0;
        else
            iReturn = BAD_MUTEX_E;

        return iReturn;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        vSemaphoreDelete( *m );
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        /* Assume an infinite block, or should there be zero block? */
        xSemaphoreTake( *m, portMAX_DELAY );
        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        xSemaphoreGive( *m );
        return 0;
    }

#elif defined(RTTHREAD)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int iReturn;

        *m = ( wolfSSL_Mutex ) rt_mutex_create("mutex",RT_IPC_FLAG_FIFO);
        if( *m != NULL )
            iReturn = 0;
        else
            iReturn = BAD_MUTEX_E;


        return iReturn;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        rt_mutex_delete( *m );
        return 0;
    }


    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        /* Assume an infinite block, or should there be zero block? */
        return rt_mutex_take( *m, RT_WAITING_FOREVER );
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        return rt_mutex_release( *m );
    }

#elif defined(WOLFSSL_SAFERTOS)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        vSemaphoreCreateBinary(m->mutexBuffer, m->mutex);
        if (m->mutex == NULL)
            return BAD_MUTEX_E;

        return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        (void)m;
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        /* Assume an infinite block */
        xSemaphoreTake(m->mutex, portMAX_DELAY);
        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        xSemaphoreGive(m->mutex);
        return 0;
    }

#elif defined(USE_WINDOWS_API)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        InitializeCriticalSection(m);
        return 0;
    }


    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        DeleteCriticalSection(m);
        return 0;
    }


    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        EnterCriticalSection(m);
        return 0;
    }


    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        LeaveCriticalSection(m);
        return 0;
    }

#elif defined(MAXQ10XX_MUTEX)
    static pthread_mutex_t *wcCryptHwSharedMutexPtr;
    static pthread_once_t key_once_own_hw_mutex = PTHREAD_ONCE_INIT;
    static pthread_key_t key_own_hw_mutex;

    static void destruct_key(void *buf)
    {
        if (buf != NULL) {
            free(buf);
        }
    }

    static void make_key_own_hw_mutex(void)
    {
        (void)pthread_key_create(&key_own_hw_mutex, destruct_key);
    }

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int created = 0;
        void *addr = NULL;

        if (m != &wcCryptHwMutex) {
            if (pthread_mutex_init(m, 0) == 0) {
                return 0;
            }
            return BAD_MUTEX_E;
        }

        /* try to open mutex memory */
        int shm_fd = shm_open("/maxq-mutex", O_RDWR, 0666);
        if (shm_fd < 0) {
            /* create mutex memory */
            shm_fd = shm_open("/maxq-mutex", O_RDWR | O_CREAT | O_EXCL, 0666);
            created = 1;
        }

        if (shm_fd < 0) {
            WOLFSSL_MSG("wc_InitMutex: shm_open() failed");
            return BAD_MUTEX_E;
        }

        if (ftruncate(shm_fd, sizeof(pthread_mutex_t))) {
            WOLFSSL_MSG("wc_InitMutex: ftruncate() failed");
            return BAD_MUTEX_E;
        }

        addr = mmap(NULL, sizeof(pthread_mutex_t), PROT_READ | PROT_WRITE,
                    MAP_SHARED, shm_fd, 0);

        if (addr == MAP_FAILED) {
            WOLFSSL_MSG("wc_InitMutex: mmap() failed");
            return BAD_MUTEX_E;
        }

        wcCryptHwSharedMutexPtr = (pthread_mutex_t *)addr;

        if (close(shm_fd)) {
            WOLFSSL_MSG("wc_InitMutex: close() failed");
            return BAD_MUTEX_E;
        }

        if (created) {
            /* initialize mutex */
            pthread_mutexattr_t attr;
            if (pthread_mutexattr_init(&attr)) {
                WOLFSSL_MSG("wc_InitMutex: pthread_mutexattr_init() failed");
                return BAD_MUTEX_E;
            }

            if (pthread_mutexattr_setpshared(&attr,
                                             PTHREAD_PROCESS_SHARED)) {
                WOLFSSL_MSG(
                    "wc_InitMutex: pthread_mutexattr_setpshared() failed");
                return BAD_MUTEX_E;
            }

            if (pthread_mutex_init(wcCryptHwSharedMutexPtr, &attr)) {
                WOLFSSL_MSG("wc_InitMutex: pthread_mutex_init() failed");
                return BAD_MUTEX_E;
            }
        }

        if (pthread_once(&key_once_own_hw_mutex, make_key_own_hw_mutex)) {
            WOLFSSL_MSG("wc_InitMutex: pthread_once() failed");
            return BAD_MUTEX_E;
        }

        return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        void *key_ptr = NULL;
        if (m != &wcCryptHwMutex) {
            if (pthread_mutex_destroy(m) == 0) {
                return 0;
            }
            return BAD_MUTEX_E;
        }

        if (wcCryptHwSharedMutexPtr) {
            if (munmap((void *)wcCryptHwSharedMutexPtr,
                       sizeof(pthread_mutex_t))) {
                WOLFSSL_MSG("wc_FreeMutex: munmap() failed");
                return BAD_MUTEX_E;
            }

            wcCryptHwSharedMutexPtr = NULL;
        }

        key_ptr = pthread_getspecific(key_own_hw_mutex);
        if (key_ptr) {
            *((int *)key_ptr) = 0;
        }

        return 0;
    }

    static int maxq_LockMutex(wolfSSL_Mutex* m, int trylock)
    {
        void *key_ptr = NULL;
        int ret = 0;

        if (m != &wcCryptHwMutex) {
            if (pthread_mutex_lock(m) == 0) {
                return 0;
            }
            return BAD_MUTEX_E;
        }

        if (wcCryptHwSharedMutexPtr == NULL) {
            return BAD_MUTEX_E;
        }

        key_ptr = pthread_getspecific(key_own_hw_mutex);
        if (key_ptr == NULL) {
            key_ptr = malloc(sizeof(int));
            if (key_ptr == NULL) {
                return MEMORY_E;
            }

            memset(key_ptr, 0, sizeof(int));

            if (pthread_setspecific(key_own_hw_mutex, key_ptr)) {
                return THREAD_STORE_SET_E;
            }
        }
        else {
            if ((trylock == 0) && (*((int *)key_ptr) > 0)) {
                *((int *)key_ptr) = *((int *)key_ptr) + 1;
                return 0;
            }
        }

        if (trylock) {
            ret = pthread_mutex_trylock(wcCryptHwSharedMutexPtr);
        }
        else {
            ret = pthread_mutex_lock(wcCryptHwSharedMutexPtr);
        }

        if (ret != 0) {
            return BAD_MUTEX_E;
        }

        *((int *)key_ptr) = 1;
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        return maxq_LockMutex(m, 0);
    }

    int maxq_CryptHwMutexTryLock()
    {
        int ret = BAD_MUTEX_E;
        /* Make sure HW Mutex has been initialized */
        ret = wolfSSL_CryptHwMutexInit();
        if (ret == 0) {
            ret = maxq_LockMutex(&wcCryptHwMutex, 1);
        }
        return ret;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        void *key_ptr = NULL;

        if (m != &wcCryptHwMutex) {
            if (pthread_mutex_unlock(m) == 0) {
                return 0;
            }
            return BAD_MUTEX_E;
        }

        if (wcCryptHwSharedMutexPtr == NULL) {
            return BAD_MUTEX_E;
        }

        key_ptr = pthread_getspecific(key_own_hw_mutex);
        if (key_ptr) {
            if (*((int *)key_ptr) > 0) {
                *((int *)key_ptr) = *((int *)key_ptr) - 1;
                if (*((int *)key_ptr) > 0) {
                    return 0;
                }
            }
        }

        if (pthread_mutex_unlock(wcCryptHwSharedMutexPtr) != 0) {
            return BAD_MUTEX_E;
        }
        return 0;
    }

#elif defined(WOLFSSL_PTHREADS)

    #ifdef WOLFSSL_USE_RWLOCK
        int wc_InitRwLock(wolfSSL_RwLock* m)
        {
            if (pthread_rwlock_init(m, NULL) == 0)
                return 0;
            else
                return BAD_MUTEX_E;
        }

        int wc_FreeRwLock(wolfSSL_RwLock* m)
        {
            if (pthread_rwlock_destroy(m) == 0)
                return 0;
            else
                return BAD_MUTEX_E;
        }

        int wc_LockRwLock_Wr(wolfSSL_RwLock* m)
        {
            if (pthread_rwlock_wrlock(m) == 0)
                return 0;
            else
                return BAD_MUTEX_E;
        }

        int wc_LockRwLock_Rd(wolfSSL_RwLock* m)
        {
            if (pthread_rwlock_rdlock(m) == 0)
                return 0;
            else
                return BAD_MUTEX_E;
        }

        int wc_UnLockRwLock(wolfSSL_RwLock* m)
        {
            if (pthread_rwlock_unlock(m) == 0)
                return 0;
            else
                return BAD_MUTEX_E;
        }
    #endif

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        if (pthread_mutex_init(m, NULL) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }


    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        if (pthread_mutex_destroy(m) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }


    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        if (pthread_mutex_lock(m) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }


    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        if (pthread_mutex_unlock(m) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }
#elif defined(WOLFSSL_LINUXKM)

    /* Linux kernel mutex routines are voids, alas. */

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        mutex_init(m);
        return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        mutex_destroy(m);
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        mutex_lock(m);
        return 0;
    }


    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        mutex_unlock(m);
        return 0;
    }

#elif defined(WOLFSSL_VXWORKS)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        if (m) {
            if ((*m = semMCreate(0)) != SEM_ID_NULL)
                return 0;
        }
        return BAD_MUTEX_E;
    }


    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        if (m) {
            if (semDelete(*m) == OK)
                return 0;
        }
        return BAD_MUTEX_E;
    }


    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        if (m) {
            if (semTake(*m, WAIT_FOREVER) == OK)
                return 0;
        }
        return BAD_MUTEX_E;
    }


    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        if (m) {
            if (semGive(*m) == OK)
                return 0;
        }
        return BAD_MUTEX_E;
    }

#elif defined(THREADX)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        if (tx_mutex_create(m, "wolfSSL Mutex", TX_NO_INHERIT) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }


    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        if (tx_mutex_delete(m) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }


    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        if (tx_mutex_get(m, TX_WAIT_FOREVER) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        if (tx_mutex_put(m) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }

#elif defined(WOLFSSL_DEOS)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        mutexStatus mutStat;
        /*
        The empty string "" denotes an anonymous mutex, so objects do not cause name collisions.
        `protectWolfSSLTemp` in an XML configuration element template describing a mutex.
        */
        if (m) {
            mutStat = createMutex("", "protectWolfSSLTemp", m);
            if (mutStat == mutexSuccess)
                return 0;
            else{
                WOLFSSL_MSG("wc_InitMutex failed");
                return mutStat;
            }
        }
        return BAD_MUTEX_E;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        mutexStatus mutStat;
        if (m) {
            mutStat = deleteMutex(*m);
            if (mutStat == mutexSuccess)
                return 0;
            else{
                WOLFSSL_MSG("wc_FreeMutex failed");
                return mutStat;
            }
        }
        return BAD_MUTEX_E;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        mutexStatus mutStat;
        if (m) {
            mutStat = lockMutex(*m);
            if (mutStat == mutexSuccess)
                return 0;
            else{
                WOLFSSL_MSG("wc_LockMutex failed");
                return mutStat;
            }
        }
        return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        mutexStatus mutStat;
        if (m) {
            mutStat = unlockMutex(*m);
            if (mutStat== mutexSuccess)
                return 0;
            else{
                WOLFSSL_MSG("wc_UnLockMutex failed");
                return mutStat;
            }
        }
        return BAD_MUTEX_E;
    }

#elif defined(MICRIUM)
    #if (OS_VERSION < 50000)
        #define MICRIUM_ERR_TYPE OS_ERR
        #define MICRIUM_ERR_NONE OS_ERR_NONE
        #define MICRIUM_ERR_CODE(err) err
    #else
        #define MICRIUM_ERR_TYPE RTOS_ERR
        #define MICRIUM_ERR_NONE RTOS_ERR_NONE
        #define MICRIUM_ERR_CODE(err)    RTOS_ERR_CODE_GET(err)
    #endif

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        MICRIUM_ERR_TYPE err;

        OSMutexCreate(m, "wolfSSL Mutex", &err);

        if (MICRIUM_ERR_CODE(err) == MICRIUM_ERR_NONE)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        #if (OS_CFG_MUTEX_DEL_EN == DEF_ENABLED)
            MICRIUM_ERR_TYPE err;

            OSMutexDel(m, OS_OPT_DEL_ALWAYS, &err);

            if (MICRIUM_ERR_CODE(err) == MICRIUM_ERR_NONE)
                return 0;
            else
                return BAD_MUTEX_E;
        #else
            (void)m;
            return 0;
        #endif
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        MICRIUM_ERR_TYPE err;

        OSMutexPend(m, 0, OS_OPT_PEND_BLOCKING, NULL, &err);

        if (MICRIUM_ERR_CODE(err) == MICRIUM_ERR_NONE)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        MICRIUM_ERR_TYPE err;

        OSMutexPost(m, OS_OPT_POST_NONE, &err);

        if (MICRIUM_ERR_CODE(err) == MICRIUM_ERR_NONE)
            return 0;
        else
            return BAD_MUTEX_E;
    }

#elif defined(EBSNET)
    #if (defined(RTPLATFORM) && (RTPLATFORM != 0))
    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        if (rtp_sig_mutex_alloc(m, "wolfSSL Mutex") == -1)
            return BAD_MUTEX_E;
        else
            return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        rtp_sig_mutex_free(*m);
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        if (rtp_sig_mutex_claim_timed(*m, RTIP_INF) == 0)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        rtp_sig_mutex_release(*m);
        return 0;
    }

    int ebsnet_fseek(int a, long b, int c)
    {
        int retval;

        retval = vf_lseek(a, b, c);
        if (retval > 0)
            retval = 0;
        else
            retval =  -1;

        return(retval);
    }
    #else
    static int rtip_semaphore_build(wolfSSL_Mutex *m)
    {
        KS_SEMAPHORE_BUILD(m)
        return(RTP_TRUE);
    }

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        if (rtip_semaphore_build(m) == RTP_FALSE)
            return BAD_MUTEX_E;
        else
            return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        KS_SEMAPHORE_FREE(*m);
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        if (KS_SEMAPHORE_GET(*m))
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        KS_SEMAPHORE_GIVE(*m);
        return 0;
    }
    #endif
    int ebsnet_fseek(int a, long b, int c)
    {
        int retval;

        retval = (int)vf_lseek(a, b, c);
        if (retval > 0)
            retval = 0;
        else
            retval =  -1;

        return(retval);
    }

    int strcasecmp(const char *s1, const char *s2)
    {
        while (rtp_tolower(*s1) == rtp_tolower(*s2)) {
            if (*s1 == '\0' || *s2 == '\0')
                break;
            s1++;
            s2++;
        }

        return rtp_tolower(*(unsigned char *) s1) -
               rtp_tolower(*(unsigned char *) s2);
    }

#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        if (_mutex_init(m, NULL) == MQX_EOK)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        if (_mutex_destroy(m) == MQX_EOK)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        if (_mutex_lock(m) == MQX_EOK)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        if (_mutex_unlock(m) == MQX_EOK)
            return 0;
        else
            return BAD_MUTEX_E;
    }

#elif defined(WOLFSSL_TIRTOS)
    #include <xdc/runtime/Error.h>

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        Semaphore_Params params;
        Error_Block eb;

        Error_init(&eb);
        Semaphore_Params_init(&params);
        params.mode = Semaphore_Mode_BINARY;

        *m = Semaphore_create(1, &params, &eb);
        if (Error_check(&eb)) {
            Error_raise(&eb, Error_E_generic, "Failed to Create the semaphore.",
                NULL);
            return BAD_MUTEX_E;
        }
        else
            return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        Semaphore_delete(m);

        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        Semaphore_pend(*m, BIOS_WAIT_FOREVER);

        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        Semaphore_post(*m);

        return 0;
    }

#elif defined(WOLFSSL_uITRON4)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int iReturn;
        m->sem.sematr  = TA_TFIFO;
        m->sem.isemcnt = 1;
        m->sem.maxsem  = 1;
        m->sem.name    = NULL;

        m->id = acre_sem(&m->sem);
        if( m->id != E_OK )
            iReturn = 0;
        else
            iReturn = BAD_MUTEX_E;

        return iReturn;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        del_sem( m->id );
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        wai_sem(m->id);
        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        sig_sem(m->id);
        return 0;
    }

    /****  uITRON malloc/free ***/
    static ID ID_wolfssl_MPOOL = 0;
    static T_CMPL wolfssl_MPOOL = {TA_TFIFO, 0, NULL, "wolfSSL_MPOOL"};

    int uITRON4_minit(size_t poolsz) {
        ER ercd;
        wolfssl_MPOOL.mplsz = poolsz;
        ercd = acre_mpl(&wolfssl_MPOOL);
        if (ercd > 0) {
            ID_wolfssl_MPOOL = ercd;
            return 0;
        } else {
            return -1;
        }
    }

    void *uITRON4_malloc(size_t sz) {
        ER ercd;
        void *p = NULL;
        ercd = get_mpl(ID_wolfssl_MPOOL, sz, (VP)&p);
        if (ercd == E_OK) {
            return p;
        } else {
            return 0;
        }
    }

    void *uITRON4_realloc(void *p, size_t sz) {
      ER ercd;
      void *newp = NULL;
      if(p) {
          ercd = get_mpl(ID_wolfssl_MPOOL, sz, (VP)&newp);
          if ((ercd == E_OK) && (newp != NULL)) {
              XMEMCPY(newp, p, sz);
              ercd = rel_mpl(ID_wolfssl_MPOOL, (VP)p);
              if (ercd == E_OK) {
                  return newp;
              }
          }
      }
      return 0;
    }

    void uITRON4_free(void *p) {
        ER ercd;
        ercd = rel_mpl(ID_wolfssl_MPOOL, (VP)p);
        if (ercd == E_OK) {
            return;
        } else {
            return;
        }
    }

#elif defined(WOLFSSL_uTKERNEL2)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int iReturn;
        m->sem.sematr  = TA_TFIFO;
        m->sem.isemcnt = 1;
        m->sem.maxsem  = 1;

        m->id = tk_cre_sem(&m->sem);
        if( m->id != NULL )
            iReturn = 0;
        else
            iReturn = BAD_MUTEX_E;

        return iReturn;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        tk_del_sem(m->id);
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        tk_wai_sem(m->id, 1, TMO_FEVR);
        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        tk_sig_sem(m->id, 1);
        return 0;
    }

    /****  uT-Kernel malloc/free ***/
    static ID ID_wolfssl_MPOOL = 0;
    static T_CMPL wolfssl_MPOOL = {
        NULL,       /* Extended information */
        TA_TFIFO,   /* Memory pool attribute */
        0,          /* Size of whole memory pool (byte) */
        "wolfSSL"   /* Object name (max 8-char) */
    };

    int uTKernel_init_mpool(unsigned int sz) {
        ER ercd;
        wolfssl_MPOOL.mplsz = sz;
        ercd = tk_cre_mpl(&wolfssl_MPOOL);
        if (ercd > 0) {
            ID_wolfssl_MPOOL = ercd;
            return 0;
        } else {
            return (int)ercd;
        }
    }

    void *uTKernel_malloc(unsigned int sz) {
        ER ercd;
        void *p = NULL;
        ercd = tk_get_mpl(ID_wolfssl_MPOOL, sz, (VP)&p, TMO_FEVR);
        if (ercd == E_OK) {
            return p;
        } else {
            return 0;
        }
    }

    void *uTKernel_realloc(void *p, unsigned int sz) {
      ER ercd;
      void *newp = NULL;
      if (p) {
          ercd = tk_get_mpl(ID_wolfssl_MPOOL, sz, (VP)&newp, TMO_FEVR);
          if ((ercd == E_OK) && (newp != NULL)) {
              XMEMCPY(newp, p, sz);
              ercd = tk_rel_mpl(ID_wolfssl_MPOOL, (VP)p);
              if (ercd == E_OK) {
                  return newp;
              }
          }
      }
      return 0;
    }

    void uTKernel_free(void *p) {
        ER ercd;
        ercd = tk_rel_mpl(ID_wolfssl_MPOOL, (VP)p);
        if (ercd == E_OK) {
            return;
        } else {
            return;
        }
    }

#elif defined (WOLFSSL_FROSTED)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        *m = mutex_init();
        if (*m)
            return 0;
        else
            return -1;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        mutex_destroy(*m);
        return(0);
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        mutex_lock(*m);
        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        mutex_unlock(*m);
        return 0;
    }

#elif defined(WOLFSSL_CMSIS_RTOS)

    #define CMSIS_NMUTEX 10
    osMutexDef(wolfSSL_mt0);  osMutexDef(wolfSSL_mt1);  osMutexDef(wolfSSL_mt2);
    osMutexDef(wolfSSL_mt3);  osMutexDef(wolfSSL_mt4);  osMutexDef(wolfSSL_mt5);
    osMutexDef(wolfSSL_mt6);  osMutexDef(wolfSSL_mt7);  osMutexDef(wolfSSL_mt8);
    osMutexDef(wolfSSL_mt9);

    static const osMutexDef_t *CMSIS_mutex[] = { osMutex(wolfSSL_mt0),
        osMutex(wolfSSL_mt1),    osMutex(wolfSSL_mt2),   osMutex(wolfSSL_mt3),
        osMutex(wolfSSL_mt4),    osMutex(wolfSSL_mt5),   osMutex(wolfSSL_mt6),
        osMutex(wolfSSL_mt7),    osMutex(wolfSSL_mt8),   osMutex(wolfSSL_mt9) };

    static osMutexId CMSIS_mutexID[CMSIS_NMUTEX] = {0};

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int i;
        for (i=0; i<CMSIS_NMUTEX; i++) {
            if(CMSIS_mutexID[i] == 0) {
                CMSIS_mutexID[i] = osMutexCreate(CMSIS_mutex[i]);
                (*m) = CMSIS_mutexID[i];
            return 0;
            }
        }
        return -1;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        int i;
        osMutexDelete   (*m);
        for (i=0; i<CMSIS_NMUTEX; i++) {
            if(CMSIS_mutexID[i] == (*m)) {
                CMSIS_mutexID[i] = 0;
                return(0);
            }
        }
        return(-1);
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        osMutexWait(*m, osWaitForever);
        return(0);
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        osMutexRelease (*m);
        return 0;
    }

#elif defined(WOLFSSL_CMSIS_RTOSv2)
    int wc_InitMutex(wolfSSL_Mutex *m)
    {
        static const osMutexAttr_t attr = {
            "wolfSSL_mutex", osMutexRecursive, NULL, 0};

        if ((*m = osMutexNew(&attr)) != NULL)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_FreeMutex(wolfSSL_Mutex *m)
    {
        if (osMutexDelete(*m) == osOK)
            return 0;
        else
            return BAD_MUTEX_E;
    }


    int wc_LockMutex(wolfSSL_Mutex *m)
    {
        if (osMutexAcquire(*m, osWaitForever) == osOK)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex *m)
    {
        if (osMutexRelease(*m) == osOK)
            return 0;
        else
            return BAD_MUTEX_E;
    }

#elif defined(WOLFSSL_MDK_ARM)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        os_mut_init (m);
        return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        return(0);
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        os_mut_wait (m, 0xffff);
        return(0);
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        os_mut_release (m);
        return 0;
    }

#elif defined(INTIME_RTOS)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int ret = 0;

        if (m == NULL)
            return BAD_FUNC_ARG;

        *m = CreateRtSemaphore(
            1,                      /* initial unit count */
            1,                      /* maximum unit count */
            PRIORITY_QUEUING        /* creation flags: FIFO_QUEUING or PRIORITY_QUEUING */
        );
        if (*m == BAD_RTHANDLE) {
            ret = GetLastRtError();
            if (ret != E_OK)
                ret = BAD_MUTEX_E;
        }
        return ret;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        int ret = 0;
        BOOLEAN del;

        if (m == NULL)
            return BAD_FUNC_ARG;

        del = DeleteRtSemaphore(
            *m                      /* handle for RT semaphore */
        );
        if (del != TRUE)
            ret = BAD_MUTEX_E;

        return ret;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        int ret = 0;
        DWORD lck;

        if (m == NULL)
            return BAD_FUNC_ARG;

        lck = WaitForRtSemaphore(
            *m,                     /* handle for RT semaphore */
            1,                      /* number of units to wait for */
            WAIT_FOREVER            /* number of milliseconds to wait for units */
        );
        if (lck == WAIT_FAILED) {
            ret = GetLastRtError();
            if (ret != E_OK)
                ret = BAD_MUTEX_E;
        }
        return ret;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        int ret = 0;
        BOOLEAN rel;

        if (m == NULL)
            return BAD_FUNC_ARG;

        rel = ReleaseRtSemaphore(
            *m,                     /* handle for RT semaphore */
            1                       /* number of units to release to semaphore */
        );
        if (rel != TRUE)
            ret = BAD_MUTEX_E;

        return ret;
    }

#elif defined(WOLFSSL_NUCLEUS_1_2)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        /* Call the Nucleus function to create the semaphore */
        if (NU_Create_Semaphore(m, "WOLFSSL_MTX", 1,
                                NU_PRIORITY) == NU_SUCCESS) {
            return 0;
        }

        return BAD_MUTEX_E;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        if (NU_Delete_Semaphore(m) == NU_SUCCESS)
            return 0;

        return BAD_MUTEX_E;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        /* passing suspend task option */
        if (NU_Obtain_Semaphore(m, NU_SUSPEND) == NU_SUCCESS)
            return 0;

        return BAD_MUTEX_E;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        if (NU_Release_Semaphore(m) == NU_SUCCESS)
            return 0;

        return BAD_MUTEX_E;
    }

#elif defined(WOLFSSL_ZEPHYR)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        k_mutex_init(m);

        return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        int ret = 0;

        if (k_mutex_lock(m, K_FOREVER) != 0)
            ret = BAD_MUTEX_E;

        return ret;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        k_mutex_unlock(m);

        return 0;
    }

#elif defined(WOLFSSL_TELIT_M2MB)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        M2MB_OS_RESULT_E        osRes;
        M2MB_OS_MTX_ATTR_HANDLE mtxAttrHandle;
        UINT32                  inheritVal = 1;

        osRes = m2mb_os_mtx_setAttrItem(&mtxAttrHandle,
                                    CMDS_ARGS(
                                      M2MB_OS_MTX_SEL_CMD_CREATE_ATTR, NULL,
                                      M2MB_OS_MTX_SEL_CMD_NAME, "wolfMtx",
                                      M2MB_OS_MTX_SEL_CMD_INHERIT, inheritVal
                                    )
                                );
        if (osRes != M2MB_OS_SUCCESS) {
            return BAD_MUTEX_E;
        }

        osRes = m2mb_os_mtx_init(m, &mtxAttrHandle);
        if (osRes != M2MB_OS_SUCCESS) {
            return BAD_MUTEX_E;
        }

        return 0;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        M2MB_OS_RESULT_E osRes;

        if (m == NULL)
            return BAD_MUTEX_E;

        osRes = m2mb_os_mtx_deinit(*m);
        if (osRes != M2MB_OS_SUCCESS) {
            return BAD_MUTEX_E;
        }

        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        M2MB_OS_RESULT_E osRes;

        if (m == NULL)
            return BAD_MUTEX_E;

        osRes = m2mb_os_mtx_get(*m, M2MB_OS_WAIT_FOREVER);
        if (osRes != M2MB_OS_SUCCESS) {
            return BAD_MUTEX_E;
        }

        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        M2MB_OS_RESULT_E osRes;

        if (m == NULL)
            return BAD_MUTEX_E;

        osRes = m2mb_os_mtx_put(*m);
        if (osRes != M2MB_OS_SUCCESS) {
            return BAD_MUTEX_E;
        }

        return 0;
    }

#elif defined(WOLFSSL_EMBOS)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        int ret;

        OS_MUTEX_Create((OS_MUTEX*) m);
        if (m != NULL)
            ret = 0;
        else
            ret = BAD_MUTEX_E;

        return ret;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        OS_MUTEX_Delete((OS_MUTEX*) m);
        return 0;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {
        OS_MUTEX_LockBlocked((OS_MUTEX*) m);
        return 0;
    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {
        OS_MUTEX_Unlock((OS_MUTEX*) m);
        return 0;
    }

#elif defined(NETOS)

    int wc_InitMutex(wolfSSL_Mutex* m)
    {
        if (tx_mutex_create(&ready->mutex, "wolfSSL Lock", TX_INHERIT)
                == TX_SUCCESS)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_FreeMutex(wolfSSL_Mutex* m)
    {
        if (tx_mutex_delete(&ready->mutex) == TX_SUCCESS)
            return 0;
        else
            return BAD_MUTEX_E;
    }

    int wc_LockMutex(wolfSSL_Mutex* m)
    {

    }

    int wc_UnLockMutex(wolfSSL_Mutex* m)
    {

    }

#elif defined(WOLFSSL_USER_MUTEX)

    /* Use user own mutex */

    /*
    int wc_InitMutex(wolfSSL_Mutex* m) { ... }
    int wc_FreeMutex(wolfSSL_Mutex *m) { ... }
    int wc_LockMutex(wolfSSL_Mutex *m) { ... }
    int wc_UnLockMutex(wolfSSL_Mutex *m) { ... }
    */

#else
    #warning No mutex handling defined

#endif
#if !defined(WOLFSSL_USE_RWLOCK) || defined(SINGLE_THREADED)
    int wc_InitRwLock(wolfSSL_RwLock* m)
    {
        return wc_InitMutex(m);
    }

    int wc_FreeRwLock(wolfSSL_RwLock* m)
    {
        return wc_FreeMutex(m);
    }

    int wc_LockRwLock_Wr(wolfSSL_RwLock* m)
    {
        return wc_LockMutex(m);
    }

    int wc_LockRwLock_Rd(wolfSSL_RwLock* m)
    {
        return wc_LockMutex(m);
    }

    int wc_UnLockRwLock(wolfSSL_RwLock* m)
    {
        return wc_UnLockMutex(m);
    }
#endif

#ifndef NO_ASN_TIME
#if defined(_WIN32_WCE)
time_t windows_time(time_t* timer)
{
    SYSTEMTIME     sysTime;
    FILETIME       fTime;
    ULARGE_INTEGER intTime;


    GetSystemTime(&sysTime);
    SystemTimeToFileTime(&sysTime, &fTime);

    XMEMCPY(&intTime, &fTime, sizeof(FILETIME));
    /* subtract EPOCH */
    intTime.QuadPart -= 0x19db1ded53e8000;
    /* to secs */
    intTime.QuadPart /= 10000000;

    if (timer != NULL)
        *timer = (time_t)intTime.QuadPart;

    return (time_t)intTime.QuadPart;
}
#endif /*  _WIN32_WCE */

#if defined(WOLFSSL_APACHE_MYNEWT)
#include "os/os_time.h"

time_t mynewt_time(time_t* timer)
{
    time_t now;
    struct os_timeval tv;
    os_gettimeofday(&tv, NULL);
    now = (time_t)tv.tv_sec;
    if(timer != NULL) {
        *timer = now;
    }
    return now;
}
#endif /* WOLFSSL_APACHE_MYNEWT */

#if defined(WOLFSSL_GMTIME)
struct tm* gmtime(const time_t* timer)
{
    #define YEAR0          1900
    #define EPOCH_YEAR     1970
    #define SECS_DAY       (24L * 60L * 60L)
    #define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) %400)))
    #define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)

    static const int _ytab[2][12] =
    {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };

    static struct tm st_time;
    struct tm* ret = &st_time;
    time_t secs = *timer;
    unsigned long dayclock, dayno;
    int year = EPOCH_YEAR;

    dayclock = (unsigned long)secs % SECS_DAY;
    dayno    = (unsigned long)secs / SECS_DAY;

    ret->tm_sec  = (int) dayclock % 60;
    ret->tm_min  = (int)(dayclock % 3600) / 60;
    ret->tm_hour = (int) dayclock / 3600;
    ret->tm_wday = (int) (dayno + 4) % 7;        /* day 0 a Thursday */

    while(dayno >= (unsigned long)YEARSIZE(year)) {
        dayno -= YEARSIZE(year);
        year++;
    }

    ret->tm_year = year - YEAR0;
    ret->tm_yday = (int)dayno;
    ret->tm_mon  = 0;

    while(dayno >= (unsigned long)_ytab[LEAPYEAR(year)][ret->tm_mon]) {
        dayno -= _ytab[LEAPYEAR(year)][ret->tm_mon];
        ret->tm_mon++;
    }

    ret->tm_mday  = (int)++dayno;
#ifndef WOLFSSL_LINUXKM
    ret->tm_isdst = 0;
#endif

    return ret;
}
#endif /* WOLFSSL_GMTIME */


#if defined(HAVE_RTP_SYS)
#define YEAR0          1900

struct tm* rtpsys_gmtime(const time_t* timer)       /* has a gmtime() but hangs */
{
    static struct tm st_time;
    struct tm* ret = &st_time;

    DC_RTC_CALENDAR cal;
    dc_rtc_time_get(&cal, TRUE);

    ret->tm_year  = cal.year - YEAR0;       /* gm starts at 1900 */
    ret->tm_mon   = cal.month - 1;          /* gm starts at 0 */
    ret->tm_mday  = cal.day;
    ret->tm_hour  = cal.hour;
    ret->tm_min   = cal.minute;
    ret->tm_sec   = cal.second;

    return ret;
}

#endif /* HAVE_RTP_SYS */


#if defined(MICROCHIP_TCPIP_V5) || defined(MICROCHIP_TCPIP)

/*
 * time() is just a stub in Microchip libraries. We need our own
 * implementation. Use SNTP client to get seconds since epoch.
 */
time_t pic32_time(time_t* timer)
{
#ifdef MICROCHIP_TCPIP_V5
    DWORD sec = 0;
#else
    word32 sec = 0;
#endif

#ifdef MICROCHIP_MPLAB_HARMONY
    sec = TCPIP_SNTP_UTCSecondsGet();
#else
    sec = SNTPGetUTCSeconds();
#endif

    if (timer != NULL)
        *timer = (time_t)sec;

    return (time_t)sec;
}

#endif /* MICROCHIP_TCPIP || MICROCHIP_TCPIP_V5 */

#if defined(WOLFSSL_DEOS) || defined(WOLFSSL_DEOS_RTEMS)

time_t deos_time(time_t* timer)
{
    const word32 systemTickTimeInHz = 1000000 / systemTickInMicroseconds();
    const volatile word32 *systemTickPtr = systemTickPointer();

    if (timer != NULL)
        *timer = *systemTickPtr/systemTickTimeInHz;

    #if defined(CURRENT_UNIX_TIMESTAMP)
        /* CURRENT_UNIX_TIMESTAMP is seconds since Jan 01 1970. (UTC) */
        return (time_t) (*systemTickPtr/systemTickTimeInHz) + CURRENT_UNIX_TIMESTAMP;
    #else
        return (time_t) *systemTickPtr/systemTickTimeInHz;
    #endif
}
#endif /* WOLFSSL_DEOS || WOLFSSL_DEOS_RTEMS */

#if defined(FREESCALE_RTC)
#include "fsl_rtc.h"
time_t fsl_time(time_t* t)
{
    *t = RTC_GetSecondsTimerCount(RTC);
    return *t;
}
#endif

#if defined(FREESCALE_SNVS_RTC)
time_t fsl_time(time_t* t)
{
    struct tm tm_time;
    time_t ret;

    snvs_hp_rtc_datetime_t rtcDate;
    snvs_hp_rtc_config_t snvsRtcConfig;

    SNVS_HP_RTC_GetDefaultConfig(&snvsRtcConfig);
    SNVS_HP_RTC_Init(SNVS, &snvsRtcConfig);

    SNVS_HP_RTC_GetDatetime(SNVS, &rtcDate);

    tm_time.tm_year  = rtcDate.year;
    tm_time.tm_mon   = rtcDate.month;
    tm_time.tm_mday  = rtcDate.day;
    tm_time.tm_hour  = rtcDate.hour;
    tm_time.tm_min   = rtcDate.minute;
    tm_time.tm_sec   = rtcDate.second;

    ret = mktime(&tm_time);
    if (t != NULL)
        *t = ret;
    return ret;
}
#endif

#if defined(MICRIUM)

time_t micrium_time(time_t* timer)
{
    CLK_TS_SEC sec;

    Clk_GetTS_Unix(&sec);

    if (timer != NULL)
        *timer = sec;

    return (time_t) sec;
}

#endif /* MICRIUM */

#if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)

time_t mqx_time(time_t* timer)
{
    TIME_STRUCT time_s;

    _time_get(&time_s);

    if (timer != NULL)
        *timer = (time_t)time_s.SECONDS;

    return (time_t)time_s.SECONDS;
}

#endif /* FREESCALE_MQX || FREESCALE_KSDK_MQX */


#if defined(WOLFSSL_TIRTOS) && defined(USER_TIME)

time_t XTIME(time_t * timer)
{
    time_t sec = 0;

    sec = (time_t) Seconds_get();

    if (timer != NULL)
        *timer = sec;

    return sec;
}

#endif /* WOLFSSL_TIRTOS */

#if defined(WOLFSSL_XILINX)
#include "xrtcpsu.h"

time_t xilinx_time(time_t * timer)
{
    time_t sec = 0;
    XRtcPsu_Config* con;
    XRtcPsu         rtc;

    con = XRtcPsu_LookupConfig(XPAR_XRTCPSU_0_DEVICE_ID);
    if (con != NULL) {
        if (XRtcPsu_CfgInitialize(&rtc, con, con->BaseAddr) == XST_SUCCESS) {
            sec = (time_t)XRtcPsu_GetCurrentTime(&rtc);
        }
        else {
            WOLFSSL_MSG("Unable to initialize RTC");
        }
    }

    if (timer != NULL)
        *timer = sec;

    return sec;
}

#endif /* WOLFSSL_XILINX */

#if defined(WOLFSSL_ZEPHYR)

time_t z_time(time_t * timer)
{
    struct timespec ts;

    if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
        if (timer != NULL)
            *timer = ts.tv_sec;

    return ts.tv_sec;
}

#endif /* WOLFSSL_ZEPHYR */


#if defined(WOLFSSL_WICED)
    #ifndef WOLFSSL_WICED_PSEUDO_UNIX_EPOCH_TIME
        #error Please define WOLFSSL_WICED_PSEUDO_UNIX_EPOCH_TIME at build time.
    #endif /* WOLFSSL_WICED_PSEUDO_UNIX_EPOCH_TIME */

time_t wiced_pseudo_unix_epoch_time(time_t * timer)
{
    time_t epoch_time;
    /* The time() function return uptime on WICED platform. */
    epoch_time = time(NULL) + WOLFSSL_WICED_PSEUDO_UNIX_EPOCH_TIME;

    if (timer != NULL) {
        *timer = epoch_time;
    }
    return epoch_time;
}
#endif /* WOLFSSL_WICED */

#ifdef WOLFSSL_TELIT_M2MB
    time_t m2mb_xtime(time_t * timer)
    {
        time_t myTime = 0;
        INT32 fd = m2mb_rtc_open("/dev/rtc0", 0);
        if (fd != -1) {
            M2MB_RTC_TIMEVAL_T timeval;

            m2mb_rtc_ioctl(fd, M2MB_RTC_IOCTL_GET_TIMEVAL, &timeval);

            myTime = timeval.sec;

            m2mb_rtc_close(fd);
        }
        return myTime;
    }
    #ifdef WOLFSSL_TLS13
    time_t m2mb_xtime_ms(time_t * timer)
    {
        time_t myTime = 0;
        INT32 fd = m2mb_rtc_open("/dev/rtc0", 0);
        if (fd != -1) {
            M2MB_RTC_TIMEVAL_T timeval;

            m2mb_rtc_ioctl(fd, M2MB_RTC_IOCTL_GET_TIMEVAL, &timeval);

            myTime = timeval.sec + timeval.msec;

            m2mb_rtc_close(fd);
        }
        return myTime;
    }
    #endif /* WOLFSSL_TLS13 */
    #ifndef NO_CRYPT_BENCHMARK
    double m2mb_xtime_bench(int reset)
    {
        double myTime = 0;
        INT32 fd = m2mb_rtc_open("/dev/rtc0", 0);
        if (fd != -1) {
            M2MB_RTC_TIMEVAL_T timeval;

            m2mb_rtc_ioctl(fd, M2MB_RTC_IOCTL_GET_TIMEVAL, &timeval);

            myTime = (double)timeval.sec + ((double)timeval.msec / 1000);

            m2mb_rtc_close(fd);
        }
        return myTime;
    }
    #endif /* !NO_CRYPT_BENCHMARK */
#endif /* WOLFSSL_TELIT_M2MB */


#if defined(WOLFSSL_LINUXKM)
time_t time(time_t * timer)
{
    time_t ret;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 0, 0)
    struct timespec ts;
    getnstimeofday(&ts);
    ret = ts.tv_sec;
#else
    struct timespec64 ts;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 0, 0)
    ts = current_kernel_time64();
#else
    ktime_get_coarse_real_ts64(&ts);
#endif
    ret = ts.tv_sec;
#endif
    if (timer)
        *timer = ret;
    return ret;
}
#endif /* WOLFSSL_LINUXKM */

#ifdef HAL_RTC_MODULE_ENABLED
extern RTC_HandleTypeDef hrtc;
time_t stm32_hal_time(time_t *t1)
{
    struct tm tm_time;
    time_t ret;
    RTC_TimeTypeDef time;
    RTC_DateTypeDef date;

    XMEMSET(&tm_time, 0, sizeof(struct tm));

    /* order of GetTime followed by GetDate required here due to STM32 HW
     * requirement */
    HAL_RTC_GetTime(&hrtc, &time, FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &date, FORMAT_BIN);

    /* RTC year is 0-99 and "struct tm" is 1900+, so assume after year 2000 */
    tm_time.tm_year  = date.Year + 100;
    /* RTC month is 1-12 and "struct tm" is 0-12, so subtract 1 */
    tm_time.tm_mon   = date.Month - 1;
    tm_time.tm_mday  = date.Date;
    tm_time.tm_hour  = time.Hours;
    tm_time.tm_min   = time.Minutes;
    tm_time.tm_sec   = time.Seconds;

    ret = mktime(&tm_time);
    if (t1 != NULL)
        *t1 = ret;
    return ret;
}
#endif /* HAL_RTC_MODULE_ENABLED */

#endif /* !NO_ASN_TIME */

#if !defined(WOLFSSL_LEANPSK) && !defined(STRING_USER)
char* mystrnstr(const char* s1, const char* s2, unsigned int n)
{
    unsigned int s2_len = (unsigned int)XSTRLEN(s2);

    if (s2_len == 0)
        return (char*)s1;

    while (n >= s2_len && s1[0]) {
        if (s1[0] == s2[0])
            if (XMEMCMP(s1, s2, s2_len) == 0)
                return (char*)s1;
        s1++;
        n--;
    }

    return NULL;
}
#endif

/* custom memory wrappers */
#ifdef WOLFSSL_NUCLEUS_1_2

    /* system memory pool */
    extern NU_MEMORY_POOL System_Memory;

    void* nucleus_malloc(unsigned long size, void* heap, int type)
    {
        STATUS status;
        void*  stack_ptr;

        status = NU_Allocate_Memory(&System_Memory, &stack_ptr, size,
                                    NU_NO_SUSPEND);
        if (status == NU_SUCCESS) {
            return 0;
        } else {
            return stack_ptr;
        }
    }

    void* nucleus_realloc(void* ptr, unsigned long size, void* heap, int type)
    {
        DM_HEADER* old_header;
        word32     old_size, copy_size;
        void*      new_mem;

        /* if ptr is NULL, behave like malloc */
        new_mem = nucleus_malloc(size, NULL, 0);
        if (new_mem == 0 || ptr == 0) {
            return new_mem;
        }

        /* calculate old memory block size */
        /* mem pointers stored in block headers (ref dm_defs.h) */
        old_header = (DM_HEADER*) ((byte*)ptr - DM_OVERHEAD);
        old_size   = (byte*)old_header->dm_next_memory - (byte*)ptr;

        /* copy old to new */
        if (old_size < size) {
            copy_size = old_size;
        } else {
            copy_size = size;
        }
        XMEMCPY(new_mem, ptr, copy_size);

        /* free old */
        nucleus_free(ptr, NULL, 0);

        return new_mem;
    }

    void nucleus_free(void* ptr, void* heap, int type)
    {
        if (ptr != NULL)
            NU_Deallocate_Memory(ptr);
    }

#endif /* WOLFSSL_NUCLEUS_1_2 */

#if defined(WOLFSSL_TI_CRYPT) || defined(WOLFSSL_TI_HASH)
    #include <wolfcrypt/src/port/ti/ti-ccm.c>  /* initialize and Mutex for TI Crypt Engine */
    #include <wolfcrypt/src/port/ti/ti-hash.c> /* md5, sha1, sha224, sha256 */
#endif

#if defined(WOLFSSL_CRYPTOCELL)
    #define WOLFSSL_CRYPTOCELL_C
    #include <wolfcrypt/src/port/arm/cryptoCell.c> /* CC310, RTC and RNG */
    #if !defined(NO_SHA256)
        #define WOLFSSL_CRYPTOCELL_HASH_C
        #include <wolfcrypt/src/port/arm/cryptoCellHash.c> /* sha256 */
    #endif
#endif


#ifndef SINGLE_THREADED

#ifdef _MSC_VER
    int wolfSSL_NewThread(THREAD_TYPE* thread,
        THREAD_CB cb, void* arg)
    {
        if (thread == NULL || cb == NULL)
            return BAD_FUNC_ARG;

        /* Use _beginthreadex instead of _beginthread because of:
         *   _beginthreadex is safer to use than _beginthread. If the thread
         *   that's generated by _beginthread exits quickly, the handle that's
         *   returned to the caller of _beginthread might be invalid or point
         *   to another thread. However, the handle that's returned by
         *   _beginthreadex has to be closed by the caller of _beginthreadex,
         *   so it's guaranteed to be a valid handle if _beginthreadex didn't
         *   return an error.*/
        *thread = _beginthreadex(NULL, 0, cb, arg, 0, NULL);
        if (*thread == 0) {
            *thread = INVALID_THREAD_VAL;
            return MEMORY_E;
        }

        return 0;
    }

    int wolfSSL_NewThreadNoJoin(THREAD_CB_NOJOIN cb, void* arg)
    {
        THREAD_TYPE thread;

        if (cb == NULL)
            return BAD_FUNC_ARG;

        thread = _beginthread(cb, 0, arg);
        if (thread == -1L) {
            return MEMORY_E;
        }

        return 0;
    }

    int wolfSSL_JoinThread(THREAD_TYPE thread)
    {
        int ret = 0;

        if (thread == INVALID_THREAD_VAL)
            return BAD_FUNC_ARG;

        /* We still want to attempt to close the thread handle even on error */
        if (WaitForSingleObject((HANDLE)thread, INFINITE) == WAIT_FAILED)
            ret = MEMORY_E;

        if (CloseHandle((HANDLE)thread) == 0)
            ret = MEMORY_E;

        return ret;
    }

#ifdef WOLFSSL_COND
    int wolfSSL_CondInit(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        *cond = CreateEventA(NULL, FALSE, FALSE, NULL);
        if (*cond == NULL)
            return MEMORY_E;

        return 0;
    }

    int wolfSSL_CondFree(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        if (CloseHandle(*cond) == 0)
            return MEMORY_E;

        return 0;
    }

    int wolfSSL_CondSignal(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        if (SetEvent(*cond) == 0)
            return MEMORY_E;

        return 0;
    }

    int wolfSSL_CondWait(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        if (WaitForSingleObject(*cond, INFINITE) == WAIT_FAILED)
            return MEMORY_E;

        return 0;
    }
#endif /* WOLFSSL_COND */

#elif defined(WOLFSSL_TIRTOS)

    int wolfSSL_NewThread(THREAD_TYPE* thread,
        THREAD_CB cb, void* arg)
    {
        /* Initialize the defaults and set the parameters. */
        Task_Params taskParams;
        Task_Params_init(&taskParams);
        taskParams.arg0 = (UArg)arg;
        taskParams.stackSize = 65535;
        *thread = Task_create((Task_FuncPtr)cb, &taskParams, NULL);
        if (*thread == NULL) {
            return MEMORY_E;
        }
        Task_yield();
        return 0;
    }

    int wolfSSL_JoinThread(THREAD_TYPE thread)
    {
        while(1) {
            if (Task_getMode(thread) == Task_Mode_TERMINATED) {
                Task_sleep(5);
                break;
            }
            Task_yield();
        }
        return 0;
    }

#elif defined(NETOS)

    int wolfSSL_NewThread(THREAD_TYPE* thread,
        THREAD_CB cb, void* arg)
    {
        /* For backwards compatibility allow using this declaration as well. */
        #ifdef TESTSUITE_THREAD_STACK_SZ
            #define WOLFSSL_NETOS_STACK_SZ TESTSUITE_THREAD_STACK_SZ
        #endif
        /* This can be adjusted by defining in user_settings.h, will default to
         * 65k in the event it is undefined */
        #ifndef WOLFSSL_NETOS_STACK_SZ
            #define WOLFSSL_NETOS_STACK_SZ 65535
        #endif
        int result;

        if (thread == NULL || cb == NULL)
            return BAD_FUNC_ARG;

        XMEMSET(thread, 0, sizeof(*thread));

        thread->threadStack = (void *)XMALLOC(WOLFSSL_NETOS_STACK_SZ, NULL,
                DYNAMIC_TYPE_TMP_BUFFER);
        if (thread->threadStack == NULL)
            return MEMORY_E;


        /* first create the idle thread:
         * ARGS:
         * Param1: pointer to thread
         * Param2: name
         * Param3 and 4: entry function and input
         * Param5: pointer to thread stack
         * Param6: stack size
         * Param7 and 8: priority level and preempt threshold
         * Param9 and 10: time slice and auto-start indicator */
        result = tx_thread_create(&thread->tid,
                           "wolfSSL thread",
                           (entry_functionType)cb, (ULONG)arg,
                           thread->threadStack,
                           TESTSUITE_THREAD_STACK_SZ,
                           2, 2,
                           1, TX_AUTO_START);
        if (result != TX_SUCCESS) {
            free(thread->threadStack);
            thread->threadStack = NULL;
            return MEMORY_E;
        }

        return 0;
    }

    int wolfSSL_JoinThread(THREAD_TYPE thread)
    {
        /* TODO: maybe have to use tx_thread_delete? */
        free(thread.threadStack);
        thread.threadStack = NULL;
        return 0;
    }

#elif defined(WOLFSSL_ZEPHYR)

    int wolfSSL_NewThread(THREAD_TYPE* thread,
        THREAD_CB cb, void* arg)
    {
        #ifndef WOLFSSL_ZEPHYR_STACK_SZ
            #define WOLFSSL_ZEPHYR_STACK_SZ (24*1024)
        #endif

        if (thread == NULL || cb == NULL)
            return BAD_FUNC_ARG;

        XMEMSET(thread, 0, sizeof(*thread));

        /* TODO: Use the following once k_thread_stack_alloc makes it into a
         * release.
         * thread->threadStack = k_thread_stack_alloc(WOLFSSL_ZEPHYR_STACK_SZ,
         *                                            0);
         */
        thread->threadStack = (void*)XMALLOC(
                Z_KERNEL_STACK_SIZE_ADJUST(WOLFSSL_ZEPHYR_STACK_SZ), 0,
                                             DYNAMIC_TYPE_TMP_BUFFER);
        if (thread->threadStack == NULL)
            return MEMORY_E;

        /* k_thread_create does not return any error codes */
        /* Casting to k_thread_entry_t should be fine since we just ignore the
         * extra arguments being passed in */
        k_thread_create(&thread->tid, thread->threadStack,
                WOLFSSL_ZEPHYR_STACK_SZ, (k_thread_entry_t)cb, arg, NULL, NULL,
                5, 0, K_NO_WAIT);

        return 0;
    }

    int wolfSSL_JoinThread(THREAD_TYPE thread)
    {
        int ret = 0;
        int err;

        err = k_thread_join(&thread.tid, K_FOREVER);
        if (err != 0)
            ret = MEMORY_E;

        /* TODO: Use the following once k_thread_stack_free makes it into a
         * release.
         * err = k_thread_stack_free(thread.threadStack);
         * if (err != 0)
         *     ret = MEMORY_E;
         */
        XFREE(thread.threadStack, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        thread.threadStack = NULL;

        /* No thread resources to free. Everything is stored in thread.tid */

        return ret;
    }

#ifdef WOLFSSL_COND
    /* Use the pthreads translation layer for signaling */

#endif /* WOLFSSL_COND */

#elif defined(WOLFSSL_PTHREADS)

    int wolfSSL_NewThread(THREAD_TYPE* thread,
        THREAD_CB cb, void* arg)
    {
        if (thread == NULL || cb == NULL)
            return BAD_FUNC_ARG;

        if (pthread_create(thread, NULL, cb, arg) != 0)
            return MEMORY_E;

        return 0;
    }

#ifdef WOLFSSL_THREAD_NO_JOIN
    int wolfSSL_NewThreadNoJoin(THREAD_CB_NOJOIN cb, void* arg)
    {
        THREAD_TYPE thread;
        int ret;
        XMEMSET(&thread, 0, sizeof(thread));
        ret = wolfSSL_NewThread(&thread, cb, arg);
        if (ret == 0)
            ret = pthread_detach(thread);
        return ret;
    }
#endif

    int wolfSSL_JoinThread(THREAD_TYPE thread)
    {
        if (thread == INVALID_THREAD_VAL)
            return BAD_FUNC_ARG;

        if (pthread_join(thread, NULL) != 0)
            return MEMORY_E;

        return 0;
    }

#ifdef WOLFSSL_COND
    #ifndef __MACH__
    /* Generic POSIX conditional */
    int wolfSSL_CondInit(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        if (pthread_mutex_init(&cond->mutex, NULL) != 0)
            return MEMORY_E;

        if (pthread_cond_init(&cond->cond, NULL) != 0) {
            /* Keep compilers happy that we are using the return code */
            if (pthread_mutex_destroy(&cond->mutex) != 0)
                return MEMORY_E;
            return MEMORY_E;
        }

        return 0;
    }

    int wolfSSL_CondFree(COND_TYPE* cond)
    {
        int ret = 0;

        if (cond == NULL)
            return BAD_FUNC_ARG;

        if (pthread_mutex_destroy(&cond->mutex) != 0)
            ret = MEMORY_E;

        if (pthread_cond_destroy(&cond->cond) != 0)
            ret = MEMORY_E;

        return ret;
    }

    int wolfSSL_CondSignal(COND_TYPE* cond)
    {
        int ret = 0;

        if (cond == NULL)
            return BAD_FUNC_ARG;

        if (pthread_mutex_lock(&cond->mutex) != 0)
            return BAD_MUTEX_E;

        if (pthread_cond_signal(&cond->cond) != 0)
            ret = MEMORY_E;

        if (pthread_mutex_unlock(&cond->mutex) != 0)
            ret = MEMORY_E;

        return ret;
    }

    int wolfSSL_CondWait(COND_TYPE* cond)
    {
        int ret = 0;

        if (cond == NULL)
            return BAD_FUNC_ARG;

        if (pthread_mutex_lock(&cond->mutex) != 0)
            return BAD_MUTEX_E;

        if (pthread_cond_wait(&cond->cond, &cond->mutex) != 0)
            ret = MEMORY_E;

        if (pthread_mutex_unlock(&cond->mutex) != 0)
            ret = MEMORY_E;

        return ret;
    }
    #else /* __MACH__ */
    /* Apple style dispatch semaphore */
    int wolfSSL_CondInit(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        /* dispatch_release() fails hard, with Trace/BPT trap signal, if the
         * sem's internal count is less than the value passed in with
         * dispatch_semaphore_create().  work around this by initing
         * with 0, then incrementing it afterwards.
         */
        *cond = dispatch_semaphore_create(0);
        if (*cond == NULL)
            return MEMORY_E;
        return 0;
    }

    int wolfSSL_CondFree(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        dispatch_release(*cond);
        *cond = NULL;
        return 0;
    }

    int wolfSSL_CondSignal(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        dispatch_semaphore_signal(*cond);
        return 0;
    }

    int wolfSSL_CondWait(COND_TYPE* cond)
    {
        if (cond == NULL)
            return BAD_FUNC_ARG;

        dispatch_semaphore_wait(*cond, DISPATCH_TIME_FOREVER);
        return 0;
    }
    #endif /* __MACH__ */
#endif /* WOLFSSL_COND */

#endif

#endif /* SINGLE_THREADED */
