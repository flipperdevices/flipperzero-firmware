/* xil-versal-trng.c
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

#if defined(WOLFSSL_XILINX_CRYPT_VERSAL)

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/port/xilinx/xil-versal-glue.h>

#include <xtrngpsv.h>

#if !defined(HAVE_HASHDRBG)
/** How often shall the DRBG part be re-seeded */
#ifndef WC_RESEED_INTERVAL
#define WC_RESEED_INTERVAL (1000000)
#endif

/** How big shall the seed be */
#ifndef WC_XILINX_TRNG_DF_LEN_MUL
#ifdef DRBG_SEED_LEN
#define WC_XILINX_TRNG_DF_LEN_MUL (DRBG_SEED_LEN / BYTES_PER_BLOCK)
#else
/* Use the next best value to the default `DRBG_SEED_LEN` (= 440bits)
 * WC_XILINX_TRNG_DF_LEN_MUL * BYTES_PER_BLOCK * BITS_PER_BYTE
 * 4U * 16 * 8 = 512
 */
#define WC_XILINX_TRNG_DF_LEN_MUL 4U
#endif
#endif
#endif /* !defined(HAVE_HASHDRBG) */

static XTrngpsv trng;

static sword32 versal_trng_selftest(void)
{
    sword32 ret = XTrngpsv_RunKAT(&trng);
    if (ret == XTRNGPSV_SUCCESS)
        ret = XTrngpsv_RunHealthTest(&trng);
    return ret;
}

#if !defined(HAVE_HASHDRBG)
static WC_INLINE void array_add_one(byte* data, word32 dataSz)
{
    int i;

    for (i = dataSz - 1; i >= 0; i--) {
        data[i]++;
        if (data[i] != 0)
            break;
    }
}

static WC_INLINE void usercfg_add_nonce(XTrngpsv_UsrCfg* user_cfg,
                                        byte* nonce,
                                        word32 nonceSz)
{
    word32 nonce_written = 0;
    byte *pers_string = (byte*)user_cfg->PersString;
    byte *nonce_rd = nonce;
    do {
        word32 to_write =
                nonce_written + nonceSz <= sizeof(user_cfg->PersString) ?
                        nonceSz : sizeof(user_cfg->PersString) - nonce_written;
        XMEMCPY(pers_string, nonce_rd, to_write);
        if (nonce != nonce_rd)
            array_add_one(pers_string, to_write);
        nonce_rd = pers_string;
        pers_string += to_write;
        nonce_written += to_write;
    } while (nonce_written < sizeof(user_cfg->PersString));
    user_cfg->PersStrPresent = XTRNGPSV_TRUE;
}
#endif

/**
   Initialize the Versal TRNG.

   nonce    Pointer to user-supplied nonce.
   nonceSz  Size of the user-supplied nonce.

   Returns 0 on success
 */
int wc_VersalTrngInit(byte* nonce, word32 nonceSz)
{
#if !defined(HAVE_HASHDRBG)
    /* Use the HRNG mode (with DF) which lets us extract FIPS compliant
     * random data from the TRNG.
     */
    XTrngpsv_UsrCfg user_cfg = {
            .Mode = XTRNGPSV_HRNG,
            .SeedLife = WC_RESEED_INTERVAL,
            .PredResistanceEn = XTRNGPSV_FALSE,
            .DFDisable = XTRNGPSV_FALSE,
            .DFLenMul = WC_XILINX_TRNG_DF_LEN_MUL,
            .InitSeedPresent = XTRNGPSV_FALSE,
            .PersStrPresent = XTRNGPSV_FALSE
    };
#else
    /* Use the PTRNG mode (without DF) which lets us extract raw entropy
     * to be used as a seed to the internal DRBG.
     */
    const XTrngpsv_UsrCfg user_cfg = {
            .Mode = XTRNGPSV_PTRNG,
            .SeedLife = 0U,
            .PredResistanceEn = XTRNGPSV_FALSE,
            .DFDisable = XTRNGPSV_TRUE,
            .DFLenMul = 0U,
            .InitSeedPresent =  XTRNGPSV_FALSE,
            .PersStrPresent = XTRNGPSV_FALSE
    };
#endif
    int ret = WC_HW_E;
    XTrngpsv_Config *cfg;
    sword32 xret = 0;
    if (trng.State == XTRNGPSV_HEALTHY) {
        return 0;
    }
    cfg = XTrngpsv_LookupConfig(WOLFSSL_PSV_TRNG_DEV_ID);
    if (!cfg) {
        WOLFSSL_MSG("Could not lookup TRNG config");
        goto out;
    }
    xret = XTrngpsv_CfgInitialize(&trng, cfg, cfg->BaseAddress);
    if (xret)
        goto out;
    xret = versal_trng_selftest();
    if (xret)
        goto out;
#if !defined(HAVE_HASHDRBG)
    if (nonce)
        usercfg_add_nonce(&user_cfg, nonce, nonceSz);
#endif
    xret = XTrngpsv_Instantiate(&trng, &user_cfg);
    if (xret)
        goto out;

    ret = 0;

out:
    if (xret) {
        WOLFSSL_MSG_EX("Xilinx API returned 0x%08x", xret);
    }
    return ret;
}

/**
   Reset the Versal TRNG.

   Returns 0 on success
 */
int wc_VersalTrngReset(void)
{
    sword32 xret = XTrngpsv_Uninstantiate(&trng);
    if (xret) {
        WOLFSSL_MSG_EX("XTrngpsv_Uninstantiate() returned 0x%08x", xret);
        return WC_HW_E;
    }
    return 0;
}

/**
   Run the Versal TRNG self-test.

   Returns 0 on success
 */
int wc_VersalTrngSelftest(void)
{
    return versal_trng_selftest() == XTRNGPSV_SUCCESS ? 0 : -1;
}

/**
   Read from the Versal TRNG.

   output   Destination
   sz       Length of output

   Returns 0 on success
 */
int wc_VersalTrngGenerate(byte* output, word32 sz)
{
    /* The TRNG always generates exactly 32bytes of output */
    byte buf[XTRNGPSV_SEC_STRENGTH_BYTES];
    word32 bytes_generated = 0;
    do {
        word32 bytes_left = sz - bytes_generated;
        word32 bytes_required =
                bytes_left > XTRNGPSV_SEC_STRENGTH_BYTES ?
                        XTRNGPSV_SEC_STRENGTH_BYTES : bytes_left;
        sword32 xret = XTrngpsv_Generate(&trng, buf,
                                         XTRNGPSV_SEC_STRENGTH_BYTES,
                                         XTRNGPSV_FALSE);
        if (xret) {
            WOLFSSL_MSG_EX("XTrngpsv_Generate() returned 0x%08x", xret);
            return WC_HW_E;
        }
        XMEMCPY(&output[bytes_generated], buf, bytes_required);
        bytes_generated += bytes_required;
    } while (bytes_generated < sz);
    return 0;
}

#endif
