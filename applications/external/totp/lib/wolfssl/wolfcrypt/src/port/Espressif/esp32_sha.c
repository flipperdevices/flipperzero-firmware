/* esp32_sha.c
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
/*****************************************************************************/
/* this entire file content is excluded when NO_SHA, NO_SHA256
 * or when using WC_SHA384 or WC_SHA512
 */
#if !defined(NO_SHA) || !defined(NO_SHA256) || defined(WC_SHA384) || \
     defined(WC_SHA512)

#include "wolfssl/wolfcrypt/logging.h"


/* this entire file content is excluded if not using HW hash acceleration */
#if defined(WOLFSSL_ESP32_CRYPT) && \
   !defined(NO_WOLFSSL_ESP32_CRYPT_HASH)

/* TODO this may be chip type dependent: add support for others */
#include <hal/clk_gate_ll.h> /* ESP32-WROOM */

#include <wolfssl/wolfcrypt/sha.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/sha512.h>

#include "wolfssl/wolfcrypt/port/Espressif/esp32-crypt.h"
#include "wolfssl/wolfcrypt/error-crypt.h"

#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

static const char* TAG = "wolf_hw_sha";

#ifdef NO_SHA
    #define WC_SHA_DIGEST_SIZE 20
#endif

/* RTOS mutex or just InUse variable  */
#if defined(SINGLE_THREADED)
    static int InUse = 0;
#else
    static wolfSSL_Mutex sha_mutex = NULL;

    #if defined(DEBUG_WOLFSSL)
        /* Only when debugging, we'll keep tracking of block numbers. */
        static int this_block_num = 0;
    #endif
#endif

/* esp_sha_init
**
**   ctx: any wolfSSL ctx from any hash algo
**   hash_type: the specific wolfSSL enum for hash type
**
** Initializes ctx based on chipset capabilities and current state.
** Active HW states, such as from during a copy operation, are demoted to SW.
** For hash_type not available in HW, set SW mode.
**
** See esp_sha_init_ctx(ctx)
*/
int esp_sha_init(WC_ESP32SHA* ctx, enum wc_HashType hash_type)
{
    int ret = 0;

#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3)
    switch (hash_type) { /* check each wolfSSL hash type WC_[n] */
        case WC_HASH_TYPE_SHA:
            ctx->sha_type = SHA1; /* assign Espressif SHA HW type */
            ret = esp_sha_init_ctx(ctx);
            break;

        case WC_HASH_TYPE_SHA256:
            ctx->sha_type = SHA2_256; /* assign Espressif SHA HW type */
            ret = esp_sha_init_ctx(ctx);
            break;

    #ifdef CONFIG_IDF_TARGET_ESP32S3
        case  WC_HASH_TYPE_SHA384:
            /* TODO is SHA384 really not supported on -S3? */
            ctx->mode = ESP32_SHA_SW;
            ctx->sha_type = SHA2_384; /* Espressif type, but we won't use HW */
            break;
    #else
        case  WC_HASH_TYPE_SHA384:
            ctx->sha_type = SHA2_384; /* assign Espressif SHA HW type */
            ret = esp_sha_init_ctx(ctx);
            break;
    #endif

        case WC_HASH_TYPE_SHA512:
            ctx->sha_type = SHA2_512; /* assign Espressif SHA HW type */
            ret = esp_sha_init_ctx(ctx);
            break;

    #ifndef WOLFSSL_NOSHA512_224
        case WC_HASH_TYPE_SHA512_224:
            /* Don't call init, always SW as there's no HW. */
            ctx->mode = ESP32_SHA_SW;
            ctx->sha_type = SHA2_512; /* Espressif type, but we won't use HW */
            break;
    #endif

    #ifndef WOLFSSL_NOSHA512_256
        case WC_HASH_TYPE_SHA512_256:
            /* Don't call init, always SW as there's no HW. */
            ctx->mode = ESP32_SHA_SW;
            ctx->sha_type = SHA2_512; /* Espressif type, but we won't use HW */
            break;
    #endif

        default:
           ret = esp_sha_init_ctx(ctx);
           ESP_LOGW(TAG, "Unexpected hash_type in esp_sha_init");
           break;
    }
#else
    /* other chipsets will be implemented here */
#endif /* defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S3) */

    return ret;
}

/* we'll call a separate init as there's only 1 HW acceleration */
int esp_sha_init_ctx(WC_ESP32SHA* ctx)
{
    if (ctx->initializer == NULL) {
        ESP_LOGV(TAG, "regular init of blank WC_ESP32SHA ctx");

        /* we'll keep track of who initialized this */
        ctx->initializer = ctx; /* save our address in the initializer */
        ctx->mode = ESP32_SHA_INIT;
    }
    else {
        /* things may be more interesting when previously initialized */
        if (ctx->initializer == ctx) {
            /* We're likely re-using an existing object previously initialized.
            ** There's of course a non-zero probability that garbage data is
            ** the same pointer value, but that's highly unlikely; We'd need
            ** to discard, then re-init to same memory location for a matching
            ** initializer. */
            ESP_LOGV(TAG, "re-using existing WC_ESP32SHA ctx");

            /* we should never have an unexpected mode in a known ctx */
            switch (ctx->mode) {
                case ESP32_SHA_INIT:
                case ESP32_SHA_SW:
                    /* nothing interesting here */
                    break;

                case ESP32_SHA_HW:
                    /* This will be dealt with below: likely demote to SW */
                    break;

                case ESP32_SHA_HW_COPY:
                    /* This is an interesting mode, caller gave HW mode hint */
                    ESP_LOGI(TAG, "ALERT: ESP32_SHA_HW_COPY?");
                    break;

                default:
                    /* This should almost occur. We'd need to have an
                    ** uninitialized ctx that just happens to include the
                    ** breadcrumb initializer with the same address. */
                    ESP_LOGW(TAG, "ALERT: unexpected WC_ESP32SHA ctx mode: "
                                  "%d. ", ctx->mode);
                    ctx->mode = ESP32_SHA_INIT;
                    break;
            }
            /* We don't need to do anything here,
            ** this section for diagnostics only.
            ** May need to unlock HW, below. */
        } /* ctx->initializer == ctx */
        else {
            /* We may end up here with either dirty memory
            ** or copied SHA ctx.
            **
            ** Any copy function should have already set mode = ESP32_SHA_INIT.
            **
            ** In either case, initialize: */
            ctx->initializer = ctx; /* set a new address */

            /* Always set to ESP32_SHA_INIT, but give debug info as to why: */
            switch (ctx->mode) {
                case ESP32_SHA_INIT:
                    /* if we are already in init mode, nothing to do. */
                    break;

                case ESP32_SHA_SW:
                    /* this should rarely, if ever occur */
                    ESP_LOGW(TAG, "ALERT: unexpected SW WC_ESP32SHA ctx mode. "
                                  "Copied? Revert to ESP32_SHA_INIT.");
                    ctx->mode = ESP32_SHA_INIT;
                    break;

                case ESP32_SHA_HW:
                    /* this should rarely, if ever occur. */
                    ESP_LOGW(TAG, "ALERT: unexpected HW WC_ESP32SHA ctx mode. "
                                  "Copied?");
                    ctx->mode = ESP32_SHA_INIT;
                    break;

                case ESP32_SHA_HW_COPY:
                    /* This is an interesting but acceptable situation:
                    ** an anticipated active HW copy that will demote to SW. */
                    ESP_LOGV(TAG, "HW WC_ESP32SHA ctx mode = ESP32_SHA_HW_COPY.");
                    break;

                default:
                    /* this will frequently occur during new init */
                    ESP_LOGV(TAG, "ALERT: unexpected WC_ESP32SHA ctx mode. "
                                  "Uninitialized?");
                    ctx->mode = ESP32_SHA_INIT;
                    break;
            } /* switch */
        } /* ctx->initializer != ctx */
    } /* ctx->initializer != NULL */

    /*
    ** After possibly changing the mode (above) handle current mode:
    */
    switch (ctx->mode) {
        case ESP32_SHA_INIT:
            /* Likely a fresh, new SHA, as desired. */
            ESP_LOGV(TAG, "Normal ESP32_SHA_INIT");
            break;

        case ESP32_SHA_HW:
            /* We're already in hardware mode, so release. */
            /* Interesting, but normal. */
            ESP_LOGV(TAG, ">> HW unlock.");

            /* During init is the ONLY TIME we call unlock.
            ** If there's a problem, likely some undesired operation
            ** outside of wolfSSL.
            */
            esp_sha_hw_unlock(ctx);
            ctx->mode = ESP32_SHA_INIT;
            break;

        case ESP32_SHA_HW_COPY:
            /* When we init during a known active HW copy, revert to SW. */
            ESP_LOGV(TAG, "Planned revert to SW during copy.");
            ctx->mode = ESP32_SHA_SW;
            break;

        case ESP32_SHA_SW:
            /* This is an interesting situation: likely a call when
            ** another SHA in progress, but copied. */
            ESP_LOGV(TAG, ">> SW Set to init.");
            ctx->mode = ESP32_SHA_INIT;
            break;

        case ESP32_SHA_FAIL_NEED_UNROLL:
            /* Oh, how did we get here? likely uninitialized SHA memory.
            ** User code logic may need attention. */
            ESP_LOGW(TAG, "ALERT: \nESP32_SHA_FAIL_NEED_UNROLL\n");
            ctx->mode = ESP32_SHA_INIT;
            break;

        default:
            /* Most likely corrupted memory. */
            ESP_LOGW(TAG, "ALERT: \nunexpected mode value: "
                          "%d \n", ctx->mode);
            ctx->mode = ESP32_SHA_INIT;
            break;
    } /* switch (ctx->mode)  */

    /* reminder: always start isfirstblock = 1 (true) when using HW engine */
    /* we're always on the first block at init time (not zero-based!) */
    ctx->isfirstblock = true;
    ctx->lockDepth = 0; /* new objects will always start with lock depth = 0 */

    return 0; /* Always return success. We assume all issues handled, above. */
} /* esp_sha_init_ctx */

/*
** internal SHA ctx copy for ESP HW
*/
int esp_sha_ctx_copy(struct wc_Sha* src, struct wc_Sha* dst)
{
    int ret;
    if (src->ctx.mode == ESP32_SHA_HW) {
        /* this is an interesting situation to copy HW digest to SW */
        ESP_LOGV(TAG, "esp_sha_ctx_copy esp_sha_digest_process");

        /* Get a copy of the HW digest, but don't process it. */
        ret = esp_sha_digest_process(dst, 0);
        if (ret == 0) {
            /* note we arrived here only because the src is already in HW mode */
            dst->ctx.mode = ESP32_SHA_HW_COPY; /* provide init hint to SW revert */

            /* initializer will be set during init */
            ret = esp_sha_init(&(dst->ctx), WC_HASH_TYPE_SHA);
            if (ret != 0) {
                ESP_LOGE(TAG, "Error during esp_sha_ctx_copy in esp_sha_init.");
            }
        }
        else {
            ESP_LOGE(TAG, "Error during esp_sha_ctx_copy in esp_sha_digest_process.");
        }

        if (dst->ctx.mode == ESP32_SHA_SW) {
            /* The normal revert to SW in copy is expected */
            ESP_LOGV(TAG, "Confirmed SHA Copy set to SW");
        }
        else {
            /* However NOT reverting to SW is not right.
            ** This should never happen. */
            ESP_LOGW(TAG, "SHA Copy NOT set to SW");
        }
    } /* (src->ctx.mode == ESP32_SHA_HW */
    else { /* src not in HW mode, ok to copy. */
        /*
        ** reminder XMEMCOPY, above: dst->ctx = src->ctx;
        ** No special HW init needed in SW mode.
        ** but we need to set our initializer breadcrumb: */
        dst->ctx.initializer = &(dst->ctx); /* assign new breadcrumb to dst */
        ret = 0;
    }

    return ret;
} /* esp_sha_ctx_copy */

/*
** internal sha224 ctx copy (no ESP HW)
*/
int esp_sha224_ctx_copy(struct wc_Sha256* src, struct wc_Sha256* dst)
{
    /* There's no 224 hardware on ESP32 */
    dst->ctx.initializer = &dst->ctx; /* assign the initializer to dst */

    /* always set to SW, as there's no ESP32 HW for SHA224.
    ** TODO: add support for ESP32-S2. ESP32-S3, ESP32-C3 here.
    */
    dst->ctx.mode = ESP32_SHA_SW;
    return 0;
} /* esp_sha224_ctx_copy */

/*
** internal sha256 ctx copy for ESP HW
*/
int esp_sha256_ctx_copy(struct wc_Sha256* src, struct wc_Sha256* dst)
{
    int ret;
    if (src->ctx.mode == ESP32_SHA_HW) {
        /* Get a copy of the HW digest, but don't process it. */
        ESP_LOGI(TAG, "esp_sha256_ctx_copy esp_sha512_digest_process");
        ret = esp_sha256_digest_process(dst, 0);

        if (ret == 0) {
            /* provide init hint to possibly SW revert */
            dst->ctx.mode = ESP32_SHA_HW_COPY;

            /* initializer breadcrumb will be set during init */
            ret = esp_sha_init(&(dst->ctx), WC_HASH_TYPE_SHA256 );
        }

        if (dst->ctx.mode == ESP32_SHA_SW) {
            ESP_LOGV(TAG, "Confirmed wc_Sha256 Copy set to SW");
        }
        else {
            ESP_LOGW(TAG, "wc_Sha256 Copy NOT set to SW");
        }
    } /* (src->ctx.mode == ESP32_SHA_HW) */
    else {
        ret = 0;
        /*
        ** reminder this happened in XMEMCOPY: dst->ctx = src->ctx;
        ** No special HW init needed in SW mode.
        ** but we need to set our initializer: */
        dst->ctx.initializer = &dst->ctx; /* assign the initializer to dst */
    } /* not (src->ctx.mode == ESP32_SHA_HW) */

    return ret;
} /* esp_sha256_ctx_copy */

/*
** internal sha384 ctx copy for ESP HW
*/
int esp_sha384_ctx_copy(struct wc_Sha512* src, struct wc_Sha512* dst)
{
    int ret;
    if (src->ctx.mode == ESP32_SHA_HW) {
        /* Get a copy of the HW digest, but don't process it. */
        ESP_LOGI(TAG, "esp_sha384_ctx_copy esp_sha512_digest_process");
        ret = esp_sha512_digest_process(dst, 0);
        if (ret == 0) {
            /* provide init hint to SW revert */
            dst->ctx.mode = ESP32_SHA_HW_COPY;

            /* initializer will be set during init */
            ret = esp_sha_init(&(dst->ctx), WC_HASH_TYPE_SHA384);
            if (ret != 0) {
                ESP_LOGE(TAG, "Error during esp_sha384_ctx_copy in esp_sha_init.");
            }
        }
        else {
            ESP_LOGE(TAG, "Error during esp_sha384_ctx_copy in esp_sha512_digest_process.");
        }

        /* just some diagnostic runtime info */
        if (dst->ctx.mode == ESP32_SHA_SW) {
            ESP_LOGV(TAG, "Confirmed wc_Sha512 Copy set to SW");
        }
        else {
            ESP_LOGW(TAG, "wc_Sha512 Copy NOT set to SW");
        }
    } /* src->ctx.mode == ESP32_SHA_HW */
    else {
        ret = 0;
        /*
        ** reminder this happened in XMEMCOPY, above: dst->ctx = src->ctx;
        ** No special HW init needed in SW mode.
        ** but we need to set our initializer: */
        dst->ctx.initializer = &dst->ctx; /* assign the initializer to dst */
    } /* not (src->ctx.mode == ESP32_SHA_HW) */

    return ret;
} /* esp_sha384_ctx_copy */

/*
** Internal sha512 ctx copy for ESP HW.
** If HW already active, fall back to SW for this ctx.
*/
int esp_sha512_ctx_copy(struct wc_Sha512* src, struct wc_Sha512* dst)
{
    int ret;
    if (src->ctx.mode == ESP32_SHA_HW) {
        /* Get a copy of the HW digest, but don't process it. */
        ESP_LOGI(TAG, "esp_sha512_ctx_copy esp_sha512_digest_process");
        ret = esp_sha512_digest_process(dst, 0);

        if (ret == 0) {
            /* provide init hint to SW revert */
            dst->ctx.mode = ESP32_SHA_HW_COPY;

            /* initializer will be set during init
            ** reminder we should never arrive here for
            ** ESP32 SHA512/224 or SHA512/224, as there's no HW */
            ret = esp_sha_init(&(dst->ctx), WC_HASH_TYPE_SHA512);
        }

        if (dst->ctx.mode == ESP32_SHA_SW) {
            ESP_LOGV(TAG, "Confirmed wc_Sha512 Copy set to SW");
        }
        else {
            ESP_LOGW(TAG, "wc_Sha512 Copy NOT set to SW");
        }
    } /* src->ctx.mode == ESP32_SHA_HW */
    else {
        ret = 0;
        /* reminder this happened in XMEMCOPY, above: dst->ctx = src->ctx;
        ** No special HW init needed when not in active HW mode.
        ** but we need to set our initializer breadcrumb: */
        dst->ctx.initializer = &dst->ctx; /*breadcrumb is this ctx address */
    }

    return ret;
} /* esp_sha512_ctx_copy */

/*
** determine the digest size, depending on SHA type.
**
** See FIPS PUB 180-4, Instruction Section 1.
**
** see ESP32 shah.h for values:
**
**  enum SHA_TYPE {
**      SHA1 = 0,
**      SHA2_256,
**      SHA2_384,
**      SHA2_512,
**      SHA_INVALID = -1,
**  };
**
** given the SHA_TYPE (see Espressif sha.h) return WC digest size.
**
** Returns zero for bad digest size type request.
**
*/
static word32 wc_esp_sha_digest_size(enum SHA_TYPE type)
{
    int ret = 0;
    ESP_LOGV(TAG, "  esp_sha_digest_size");

    switch (type) {
    #ifndef NO_SHA
        case SHA1: /* typically 20 bytes */
            ret = WC_SHA_DIGEST_SIZE;
            break;
#endif
    #ifdef WOLFSSL_SHA224
    /*
        no SHA224 HW at this time.
        case SHA2_224:
            ret = WC_SHA224_DIGEST_SIZE;
            break;
    */
    #endif
    #ifndef NO_SHA256
        case SHA2_256: /* typically 32 bytes */
            ret = WC_SHA256_DIGEST_SIZE;
            break;
#endif
    #ifdef WOLFSSL_SHA384
        case SHA2_384:
            ret =  WC_SHA384_DIGEST_SIZE;
            break;
#endif
    #ifdef WOLFSSL_SHA512
        case SHA2_512: /* typically 64 bytes */
            ret = WC_SHA512_DIGEST_SIZE;
            break;
#endif
        default:
            ESP_LOGE(TAG, "Bad SHA type in wc_esp_sha_digest_size");
            ret = 0;
            break;
    }

    return ret; /* Return value is a size, not an error code. */
} /* wc_esp_sha_digest_size */

/*
** wait until all engines becomes idle
*/
static int wc_esp_wait_until_idle(void)
{
    int ret = 0; /* assume success */

#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
    /* ESP32-C3 RISC-V TODO */
#elif defined(CONFIG_IDF_TARGET_ESP32S3)
    while (REG_READ(SHA_BUSY_REG)) {
      /* do nothing while waiting. */
    }
#else
    while ((DPORT_REG_READ(SHA_1_BUSY_REG)   != 0) ||
           (DPORT_REG_READ(SHA_256_BUSY_REG) != 0) ||
           (DPORT_REG_READ(SHA_384_BUSY_REG) != 0) ||
           (DPORT_REG_READ(SHA_512_BUSY_REG) != 0)) {
        /* do nothing while waiting. */
    }
#endif

    return ret;
} /* wc_esp_wait_until_idle */

/*
** hack alert. there really should have been something implemented
** in Espressif periph_ctrl.c to detect ref_counts[periph] depth.
**
** since there is not at this time, we have this brute-force method.
**
** when trying to unwrap an arbitrary depth of peripheral-enable(s),
** we'll check the register upon *enable* to see if we actually did.
**
** Note that enable / disable only occurs when ref_counts[periph] == 0
**
** TODO: check if this works with other ESP32 platforms ESP32-C3,
** ESP32-S3, etc.  (A: generally, no. RISC-V has different HW accelerator.)
*/
int esp_unroll_sha_module_enable(WC_ESP32SHA* ctx)
{
    /* if we end up here, there was a prior unexpected fail and
     * we need to unroll enables */
    int ret = 0; /* assume success unless proven otherwise */
    int actual_unroll_count = 0;
    int max_unroll_count = 1000; /* never get stuck in a hardware wait loop */

#if defined(CONFIG_IDF_TARGET_ESP32)
    uint32_t this_sha_mask; /* this is the bit-mask for our SHA CLK_EN_REG */
#endif

    if (ctx == NULL) {
        ESP_LOGE(TAG, "esp_unroll_sha_module_enable called with null ctx.");
        return BAD_FUNC_ARG;
    }

#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
    /*  RISC-V Architecture: TODO */
#else
    /* Xtensa Architecture */

    /* unwind prior calls to THIS ctx. decrement ref_counts[periph] */
    /* only when ref_counts[periph] == 0 does something actually happen */

    /* once the value we read is a 0 in the DPORT_PERI_CLK_EN_REG bit
     * then we have fully unrolled the enables via ref_counts[periph]==0 */
#if CONFIG_IDF_TARGET_ESP32S3
    /* once the value we read is a 0 in the DPORT_PERI_CLK_EN_REG bit
     * then we have fully unrolled the enables via ref_counts[periph]==0 */
    while (periph_ll_periph_enabled(PERIPH_SHA_MODULE)) {
#else
    /* this is the bit-mask for our SHA CLK_EN_REG */
    this_sha_mask = periph_ll_get_clk_en_mask(PERIPH_SHA_MODULE);
    asm volatile("memw");
    while ((this_sha_mask & *(uint32_t*)DPORT_PERI_CLK_EN_REG) != 0) {
#endif /* CONFIG_IDF_TARGET_ESP32S3 */
        periph_module_disable(PERIPH_SHA_MODULE);
        asm volatile("memw");
        actual_unroll_count++;
        ESP_LOGI(TAG, "unroll not yet successful. try #%d",
                 actual_unroll_count);

        /* we'll only try this some unreasonable number of times
         * before giving up */
        if (actual_unroll_count > max_unroll_count) {
            ret = -1; /* failed to unroll */
            break;
        }
    }
#endif /* else; not RISC-V */
    if (ret == 0) {
        if (ctx->lockDepth != actual_unroll_count) {
            /* this could be a warning of wonkiness in RTOS environment.
            ** we were successful, but not expected depth count.
            **
            ** This should never happen unless someone else called
            ** periph_module_disable() or threading not working properly.
            **/
            ESP_LOGW(TAG, "warning lockDepth mismatch.");
        }
        ctx->lockDepth = 0;
        ctx->mode = ESP32_SHA_INIT;
    }
    else {
        /* This should never occur. Something must have gone seriously
        ** wrong. Check for non-wolfSSL outside calls that may have enabled HW.
        */
        ESP_LOGE(TAG, "Failed to unroll after %d attempts.",
                      actual_unroll_count);
        ESP_LOGI(TAG, "Setting ctx->mode = ESP32_SHA_SW");
        ctx->mode = ESP32_SHA_SW;
    }
    return ret;
} /* esp_unroll_sha_module_enable */

/*
** lock HW engine.
** this should be called before using engine.
*/
int esp_sha_try_hw_lock(WC_ESP32SHA* ctx)
{
    int ret = 0;

    ESP_LOGV(TAG, "enter esp_sha_hw_lock %x", (int)ctx->initializer);

    if (ctx == NULL) {
        ESP_LOGE(TAG, " esp_sha_try_hw_lock called with NULL ctx");
        return BAD_FUNC_ARG;
    }

    /* Init mutex
     *
     * Note that even single thread mode may calculate hashes
     * concurrently, so we still need to keep track of the
     * engine being busy or not.
     **/
#if defined(SINGLE_THREADED)
    if (ctx->mode == ESP32_SHA_INIT) {
        if (!InUse) {
            ctx->mode = ESP32_SHA_HW;
            InUse = 1;
        }
        else {
            ctx->mode = ESP32_SHA_SW;
        }
    }
    else {
         /* this should not happens */
        ESP_LOGE(TAG, "unexpected error in esp_sha_try_hw_lock.");
        return -1;
    }
#else /* not defined(SINGLE_THREADED) */
    /*
    ** there's only one SHA engine for all the hash types
    ** so when any hash is in use, no others can use it.
    ** fall back to SW.
    **
    ** here is some sample code to test the unrolling of SHA enables:
    **

    periph_module_enable(PERIPH_SHA_MODULE);
    ctx->lockDepth++;
    periph_module_enable(PERIPH_SHA_MODULE);
    ctx->lockDepth++;
    ctx->mode = ESP32_FAIL_NEED_INIT;

    **
    */

    if (sha_mutex == NULL) {
        ESP_LOGV(TAG, "Initializing sha_mutex");

        /* created, but not yet locked */
        ret = esp_CryptHwMutexInit(&sha_mutex);
        if (ret == 0) {
            ESP_LOGV(TAG, "esp_CryptHwMutexInit sha_mutex init success.");
        }
        else {
            ESP_LOGE(TAG, "esp_CryptHwMutexInit sha_mutex failed.");
            sha_mutex = 0;

            ESP_LOGI(TAG, "Revert to ctx->mode = ESP32_SHA_SW.");
            ctx->mode = ESP32_SHA_SW;
            return 0; /* success, just not using HW */
        }
    }

    /* check if this SHA has been operated as SW or HW, or not yet init */
    if (ctx->mode == ESP32_SHA_INIT) {
        /* try to lock the HW engine */
        ESP_LOGV(TAG, "ESP32_SHA_INIT\n");

        /* we don't wait:
        ** either the engine is free, or we fall back to SW
        **/
        if (esp_CryptHwMutexLock(&sha_mutex, (TickType_t)0) == 0) {
            /* check to see if we had a prior fail and need to unroll enables */
            ret = esp_unroll_sha_module_enable(ctx);
            ESP_LOGV(TAG, "Hardware Mode, lock depth = %d,  %x",
                          ctx->lockDepth, (int)ctx->initializer);

            if (ctx->lockDepth > 0) {
                /* it is unlikely that this would ever occur,
                ** as the mutex should be gate keeping */
                ESP_LOGW(TAG, "WARNING: Hardware Mode "
                              "interesting lock depth = %d,  %x",
                              ctx->lockDepth, (int)ctx->initializer);
            }
        }
        else {
            /* We should have otherwise anticipated this; how did we get here?
            ** This code should rarely, ideally never be reached. */
            ESP_LOGI(TAG, "\nHardware in use; Mode REVERT to ESP32_SHA_SW\n");
            ctx->mode = ESP32_SHA_SW;
            return 0; /* success, but revert to SW */
        }
    } /* (ctx->mode == ESP32_SHA_INIT) */
    else {
        /* this should not happen: called during mode != ESP32_SHA_INIT  */
        ESP_LOGE(TAG, "unexpected error in esp_sha_try_hw_lock.");
        return -1;
    }
#endif /* not defined(SINGLE_THREADED) */

#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
    /* ESP32-C3 RISC-V TODO */
#else
    if (ret == 0) {
        ctx->lockDepth++; /* depth for THIS ctx (there could be others!) */
        periph_module_enable(PERIPH_SHA_MODULE);
        ctx->mode = ESP32_SHA_HW;
    }
    else {
        ESP_LOGW(TAG, ">>>> Other problem; Mode REVERT to ESP32_SHA_SW");
        ctx->mode = ESP32_SHA_SW;
    }
#endif
    ESP_LOGV(TAG, "leave esp_sha_hw_lock");

    return ret;
} /* esp_sha_try_hw_lock */

/*
** release HW engine. when we don't have it locked, SHA module is DISABLED
*/
int esp_sha_hw_unlock(WC_ESP32SHA* ctx)
{
    ESP_LOGV(TAG, "enter esp_sha_hw_unlock");

#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
    /* ESP32-C3 RISC-V TODO */
#else
    /* Disable AES hardware */
    periph_module_disable(PERIPH_SHA_MODULE);
#endif
    /* we'll keep track of our lock depth.
     * in case of unexpected results, all the periph_module_disable() calls
     * and periph_module_disable() need to be unwound.
     *
     * see ref_counts[periph] in file: periph_ctrl.c */
    if (ctx->lockDepth > 0) {
        ctx->lockDepth--;
    }
    else {
        ctx->lockDepth = 0;
    }

#if defined(SINGLE_THREADED)
    InUse = 0;
#else
    /* unlock HW engine for next use */
    esp_CryptHwMutexUnLock(&sha_mutex);
#endif
    ESP_LOGV(TAG, "leave esp_sha_hw_unlock, %x", (int)ctx->initializer);
    return 0;
} /* esp_sha_hw_unlock */

/*
* Start SHA process by using HW engine.
* Assumes register already loaded.
* Returns a negative value error code upon failure.
*/
static int esp_sha_start_process(WC_ESP32SHA* sha)
{
    int ret = 0;
#if defined(CONFIG_IDF_TARGET_ESP32S3)
    uint8_t HardwareAlgorithm;
#endif

    if (sha == NULL) {
        return BAD_FUNC_ARG;
    }

    ESP_LOGV(TAG, "    enter esp_sha_start_process");

    #if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
        /* ESP32-C3 RISC-V TODO */
	#elif defined(CONFIG_IDF_TARGET_ESP32S3)

    /* Translate from Wolf SHA type to hardware algorithm. */
    HardwareAlgorithm = 0;
    switch (sha->sha_type) {
        case SHA1:
            HardwareAlgorithm = 0;
            break;
        case SHA2_256:
            HardwareAlgorithm = 2;
            break;
    #if defined(WOLFSSL_SHA384)
        case SHA2_384:
            HardwareAlgorithm = 3;
            break;
    #endif
    #if defined(WOLFSSL_SHA512)
        case SHA2_512:
            HardwareAlgorithm = 4;
            break;
    #endif
        default:
            /* Unsupported SHA mode. */
            sha->mode = ESP32_SHA_FAIL_NEED_UNROLL;
            return -1;
    }

    REG_WRITE(SHA_MODE_REG, HardwareAlgorithm);

    if (sha->isfirstblock) {
        REG_WRITE(SHA_START_REG, 1);
        sha->isfirstblock = false;

        ESP_LOGV(TAG, "      set sha->isfirstblock = 0");

    #if defined(DEBUG_WOLFSSL)
        this_block_num = 1; /* one-based counter, just for debug info */
    #endif
    } /* first block */
    else {
        REG_WRITE(SHA_CONTINUE_REG, 1);

    #if defined(DEBUG_WOLFSSL)
        this_block_num++; /* one-based counter */
        ESP_LOGV(TAG, "      continue block #%d", this_block_num);
    #endif
    } /* not first block */

#else /* not ESP32S3 */
    if (sha->isfirstblock) {
        /* start registers for first message block
         * we don't make any relational memory position assumptions.
         */
        switch (sha->sha_type) {
            case SHA1:
                DPORT_REG_WRITE(SHA_1_START_REG, 1);
                break;

            case SHA2_256:
                DPORT_REG_WRITE(SHA_256_START_REG, 1);
                break;

        #if defined(WOLFSSL_SHA384)
            case SHA2_384:
                DPORT_REG_WRITE(SHA_384_START_REG, 1);
                break;
        #endif

        #if defined(WOLFSSL_SHA512)
            case SHA2_512:
                DPORT_REG_WRITE(SHA_512_START_REG, 1);
                break;
        #endif

            default:
                sha->mode = ESP32_SHA_FAIL_NEED_UNROLL;
                ret = -1;
                break;
        }

        sha->isfirstblock = false;
        ESP_LOGV(TAG, "      set sha->isfirstblock = 0");

    #if defined(DEBUG_WOLFSSL)
        this_block_num = 1; /* one-based counter, just for debug info */
    #endif

    }
    else {

        /* continue registers for next message block.
         * we don't make any relational memory position assumptions
         * for future chip architecture changes.
         */
        switch (sha->sha_type) {
            case SHA1:
                DPORT_REG_WRITE(SHA_1_CONTINUE_REG, 1);
                break;

            case SHA2_256:
                DPORT_REG_WRITE(SHA_256_CONTINUE_REG, 1);
                break;

        #if defined(WOLFSSL_SHA384)
            case SHA2_384:
                DPORT_REG_WRITE(SHA_384_CONTINUE_REG, 1);
                break;
        #endif

        #if defined(WOLFSSL_SHA512)
            case SHA2_512:
                DPORT_REG_WRITE(SHA_512_CONTINUE_REG, 1);
                break;
        #endif

            default:
                /* error for unsupported other values */
                sha->mode = ESP32_SHA_FAIL_NEED_UNROLL;
                ret = -1;
                break;
        }
    }
    #endif

        #if defined(DEBUG_WOLFSSL)
            this_block_num++; /* one-based counter */
            ESP_LOGV(TAG, "      continue block #%d", this_block_num);
        #endif

   ESP_LOGV(TAG, "    leave esp_sha_start_process");

   return ret;
}

/*
** process message block
*/
static int wc_esp_process_block(WC_ESP32SHA* ctx, /* see ctx->sha_type */
                                 const word32* data,
                                 word32 len)
{
    int ret = 0; /* assume success */
    word32 word32_to_save = (len) / (sizeof(word32));
#ifdef CONFIG_IDF_TARGET_ESP32S3
    uint32_t* MessageSource;
    uint32_t* AcceleratorMessage;
#else
    int i;
#endif
    ESP_LOGV(TAG, "  enter esp_process_block");
    if (word32_to_save > 0x31) {
        word32_to_save = 0x31;
        ESP_LOGE(TAG, "  ERROR esp_process_block length exceeds 0x31 words.");
    }

    /* wait until the engine is available */
    ret = wc_esp_wait_until_idle();

#if CONFIG_IDF_TARGET_ESP32S3
    MessageSource = (uint32_t*)data;
    AcceleratorMessage = (uint32_t*)(SHA_TEXT_BASE);
    while (word32_to_save--) {
      /* Must swap endianness of data loaded into hardware accelerator to produce
       * correct result. Using DPORT_REG_WRITE doesn't avoid this for ESP32s3.
       * Note: data sheet claims we also need to swap endianness across 64 byte words
       * when doing SHA-512, but the SHA-512 result is not correct if you do that. */
      DPORT_REG_WRITE(AcceleratorMessage, __builtin_bswap32(*MessageSource));
      ++AcceleratorMessage;
      ++MessageSource;
    } /*  (word32_to_save--) */

#else
    /* load [len] words of message data into HW */
    for (i = 0; i < word32_to_save; i++) {
        /* by using DPORT_REG_WRITE, we avoid the need
         * to call __builtin_bswap32 to address endianness.
         *
         * a useful watch array cast to watch at runtime:
         *   ((uint32_t[32])  (*(volatile uint32_t *)(SHA_TEXT_BASE)))
         *
         * Write value to DPORT register (does not require protecting)
         */
    #if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
        /* ESP32-C3 RISC-V TODO */
    #else
        DPORT_REG_WRITE(SHA_TEXT_BASE + (i*sizeof(word32)), *(data + i));
    #endif
        /* memw confirmed auto inserted by compiler here */
    }
#endif

    /* notify HW to start process
     * see ctx->sha_type
     * reg data does not change until we are ready to read */
    ret = esp_sha_start_process(ctx);

    ESP_LOGV(TAG, "  leave esp_process_block");
    return ret;
} /* wc_esp_process_block */

/*
** retrieve SHA digest from memory
*/
int wc_esp_digest_state(WC_ESP32SHA* ctx, byte* hash)
{
    word32 digestSz;

#if CONFIG_IDF_TARGET_ESP32S3
    uint64_t* pHash64Buffer;
    uint32_t* pHashDestination;
    size_t szHashWords;
    size_t szHash64Words;
# endif

    ESP_LOGV(TAG, "enter esp_digest_state");

    if (ctx == NULL) {
        return BAD_FUNC_ARG;
    }

    /* sanity check */
    digestSz = wc_esp_sha_digest_size(ctx->sha_type);
    if (digestSz == 0) {
        ctx->mode = ESP32_SHA_FAIL_NEED_UNROLL;
        ESP_LOGE(TAG, "unexpected error. sha_type is invalid.");
        return -1;
    }
#if CONFIG_IDF_TARGET_ESP32S3
    if (ctx->isfirstblock == true) {
        /* no hardware use yet. Nothing to do yet */
        return 0;
    }

    /* wait until idle */
    wc_esp_wait_until_idle();

    /* read hash result into buffer & flip endianness */
    pHashDestination = (uint32_t*)hash;
    szHashWords = wc_esp_sha_digest_size(ctx->sha_type) / sizeof(uint32_t);
    esp_dport_access_read_buffer(pHashDestination, SHA_H_BASE, szHashWords);

    if (ctx->sha_type == SHA2_512) {
        /* Although we don't have to swap endianness on 64-bit words
        ** at the input, we do for the output. */
        szHash64Words = szHashWords / 2;
        pHash64Buffer = (uint64_t*)pHashDestination;
        while (szHash64Words--) {
            *pHash64Buffer = __builtin_bswap64(*pHash64Buffer);
            ++pHash64Buffer;
        }
    } /*  (ctx->sha_type == SHA2_512) */
    else {
        while (szHashWords--) {
            *pHashDestination = __builtin_bswap32(*pHashDestination);
            ++pHashDestination;
        }
    } /* not (ctx->sha_type == SHA2_512) */

    /* end if CONFIG_IDF_TARGET_ESP32S3 */
#else
    /* not CONFIG_IDF_TARGET_ESP32S3 */
    /* wait until idle */
    wc_esp_wait_until_idle();

    /* each sha_type register is at a different location  */
#if defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32C6)
    /* ESP32-C3 RISC-V TODO */
#else
    switch (ctx->sha_type) {
        case SHA1:
            DPORT_REG_WRITE(SHA_1_LOAD_REG, 1);
            break;

        case SHA2_256:
            DPORT_REG_WRITE(SHA_256_LOAD_REG, 1);
            break;

    #if defined(WOLFSSL_SHA384)
        case SHA2_384:
            DPORT_REG_WRITE(SHA_384_LOAD_REG, 1);
            break;
    #endif

    #if defined(WOLFSSL_SHA512)
        case SHA2_512:
            DPORT_REG_WRITE(SHA_512_LOAD_REG, 1);
            break;
    #endif

        default:
            ctx->mode = ESP32_SHA_FAIL_NEED_UNROLL;
            return -1;
    }

    if (ctx->isfirstblock == true) {
        /* no hardware use yet. Nothing to do yet */
        return 0;
    }

    /* LOAD final digest */

    wc_esp_wait_until_idle();

    /* MEMW instructions before volatile memory references to guarantee
     * sequential consistency. At least one MEMW should be executed in
     * between every load or store to a volatile variable
     */
    asm volatile("memw");

    /* put result in hash variable.
     *
     * ALERT - hardware specific. See esp_hw_support\port\esp32\dport_access.c
     *
     * note we read 4-byte word32's here via DPORT_SEQUENCE_REG_READ
     *
     *  example:
     *    DPORT_SEQUENCE_REG_READ(address + i * 4);
     */

    esp_dport_access_read_buffer(
#if ESP_IDF_VERSION_MAJOR >= 4
        (uint32_t*)(hash), /* the result will be found in hash upon exit */
#else
        (word32*)(hash), /* the result will be found in hash upon exit */
#endif
        SHA_TEXT_BASE,   /* there's a fixed reg addr for all SHA */
        digestSz / sizeof(word32) /* # 4-byte */
    );
#endif

#if defined(WOLFSSL_SHA512) || defined(WOLFSSL_SHA384)
    if (ctx->sha_type == SHA2_384 || ctx->sha_type == SHA2_512) {
        word32  i;
        word32* pwrd1 = (word32*)(hash);
        /* swap 32 bit words in 64 bit values */
        for (i = 0; i < WC_SHA512_DIGEST_SIZE / 4; i += 2) {
            pwrd1[i]     ^= pwrd1[i + 1];
            pwrd1[i + 1] ^= pwrd1[i];
            pwrd1[i]     ^= pwrd1[i + 1];
        }
    }
#endif
#endif /* not CONFIG_IDF_TARGET_ESP32S3 */

    ESP_LOGV(TAG, "leave esp_digest_state");
    return 0;
} /* wc_esp_digest_state */

#ifndef NO_SHA
/*
** sha1 process
*/
int esp_sha_process(struct wc_Sha* sha, const byte* data)
{
    int ret = 0;

    ESP_LOGV(TAG, "enter esp_sha_process");

    wc_esp_process_block(&sha->ctx, (const word32*)data, WC_SHA_BLOCK_SIZE);

    ESP_LOGV(TAG, "leave esp_sha_process");

    return ret;
} /* esp_sha_process */

/*
** retrieve sha1 digest
*/
int esp_sha_digest_process(struct wc_Sha* sha, byte blockprocess)
{
    int ret = 0;

    ESP_LOGV(TAG, "enter esp_sha_digest_process");

    if (blockprocess) {
        wc_esp_process_block(&sha->ctx, sha->buffer, WC_SHA_BLOCK_SIZE);
    }

    ret = wc_esp_digest_state(&sha->ctx, (byte*)sha->digest);

    ESP_LOGV(TAG, "leave esp_sha_digest_process");

    return ret;
} /* esp_sha_digest_process */
#endif /* NO_SHA */


#ifndef NO_SHA256
/*
** sha256 process
**
** repeatedly call this for [N] blocks of [WC_SHA256_BLOCK_SIZE] bytes of data
*/
int esp_sha256_process(struct wc_Sha256* sha, const byte* data)
{
    int ret = 0;

    ESP_LOGV(TAG, "  enter esp_sha256_process");

    if ((&sha->ctx)->sha_type == SHA2_256) {
#if defined(DEBUG_WOLFSSL_VERBOSE)
        ESP_LOGV(TAG, "    confirmed SHA type call match");
#endif
    }
    else {
        ret = -1;
        ESP_LOGE(TAG, "    ERROR SHA type call mismatch");
    }

    wc_esp_process_block(&sha->ctx, (const word32*)data, WC_SHA256_BLOCK_SIZE);

    ESP_LOGV(TAG, "  leave esp_sha256_process");

    return ret;
} /* esp_sha256_process */

/*
** retrieve sha256 digest
**
** note that wc_Sha256Final() in sha256.c expects to need to reverse byte
** order, even though we could have returned them in the right order.
*/
int esp_sha256_digest_process(struct wc_Sha256* sha, byte blockprocess)
{
    int ret = 0;

    ESP_LOGV(TAG, "enter esp_sha256_digest_process");

    if (blockprocess) {
        wc_esp_process_block(&sha->ctx, sha->buffer, WC_SHA256_BLOCK_SIZE);
    }

    wc_esp_digest_state(&sha->ctx, (byte*)sha->digest);

    ESP_LOGV(TAG, "leave esp_sha256_digest_process");
    return ret;
} /* esp_sha256_digest_process */


#endif /* NO_SHA256 */

#if defined(WOLFSSL_SHA512) || defined(WOLFSSL_SHA384)
/*
** sha512 process. this is used for sha384 too.
*/
int esp_sha512_block(struct wc_Sha512* sha, const word32* data, byte isfinal)
{
    int ret = 0; /* assume success */
    ESP_LOGV(TAG, "enter esp_sha512_block");
    /* start register offset */

    /* note that in SW mode, wolfSSL uses 64 bit words */
    if (sha->ctx.mode == ESP32_SHA_SW) {
        ByteReverseWords64(sha->buffer,
                           sha->buffer,
                           WC_SHA512_BLOCK_SIZE);
        if (isfinal) {
            sha->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2] =
                                        sha->hiLen;
            sha->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 1] =
                                        sha->loLen;
        }
    }
    else {
        /* when we are in HW mode, Espressif uses 32 bit words */
        ByteReverseWords((word32*)sha->buffer,
                         (word32*)sha->buffer,
                         WC_SHA512_BLOCK_SIZE);

        if (isfinal) {
            sha->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 2] =
                                        rotlFixed64(sha->hiLen, 32U);
            sha->buffer[WC_SHA512_BLOCK_SIZE / sizeof(word64) - 1] =
                                        rotlFixed64(sha->loLen, 32U);
        }

        ret = wc_esp_process_block(&sha->ctx, data, WC_SHA512_BLOCK_SIZE);
    }
    ESP_LOGV(TAG, "leave esp_sha512_block");
    return ret;
} /* esp_sha512_block */

/*
** sha512 process. this is used for sha384 too.
*/
int esp_sha512_process(struct wc_Sha512* sha)
{
    int ret = 0; /* assume success */
    word32 *data = (word32*)sha->buffer;

    ESP_LOGV(TAG, "enter esp_sha512_process");

    esp_sha512_block(sha, data, 0);

    ESP_LOGV(TAG, "leave esp_sha512_process");
    return ret;
}

/*
** retrieve sha512 digest. this is used for sha384, sha512-224, sha512-256 too.
*/
int esp_sha512_digest_process(struct wc_Sha512* sha, byte blockproc)
{
    int ret = 0;
    ESP_LOGV(TAG, "enter esp_sha512_digest_process");

    if (blockproc) {
        word32* data = (word32*)sha->buffer;

        ret = esp_sha512_block(sha, data, 1);
    }
    if (sha->ctx.mode == ESP32_SHA_HW) {
        ret = wc_esp_digest_state(&sha->ctx, (byte*)sha->digest);
    }
    else {
        ESP_LOGW(TAG, "Call esp_sha512_digest_process in non-HW mode?");
    }

    ESP_LOGV(TAG, "leave esp_sha512_digest_process");
    return ret;
} /* esp_sha512_digest_process */
#endif /* WOLFSSL_SHA512 || WOLFSSL_SHA384 */
#endif /* WOLFSSL_ESP32_CRYPT */
#endif /* !defined(NO_SHA) ||... */
