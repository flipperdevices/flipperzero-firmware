/* xil-versal-glue.c
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

#include <xil_cache.h>

/**
   Initialize versal peripherals

   xsec  Xsecure structure to initialize

   Returns 0 on success
 */
int wc_InitXsecure(wc_Xsecure* xsec)
{
#ifdef XSECURE_CACHE_DISABLE
    static int cache_disabled = 0;
    if (cache_disabled == 0) {
        Xil_DCacheDisable();
        cache_disabled = 1;
    }
#endif

    if (xsec == NULL) {
        return BAD_FUNC_ARG;
    }

    if (XMailbox_Initialize(&(xsec->mb), 0U) != XST_SUCCESS) {
        WOLFSSL_MSG("Can't initialize mailbox");
        return WC_HW_E;
    }

    if (XSecure_ClientInit(&(xsec->cinst), &(xsec->mb)) != XST_SUCCESS) {
        WOLFSSL_MSG("Can't initialize secure client");
        return WC_HW_E;
    }

    /* Set shared memory */
    if (XMailbox_SetSharedMem(&(xsec->mb), XIL_CAST_U64(xsec->mem),
                              sizeof(xsec->mem)) != XST_SUCCESS) {
        WOLFSSL_MSG("Can't set shared mem");
        return WC_HW_E;
    }

    return 0;
}

/**
   Convert Xilinx specific error to string

   err	The error to convert

   Returns a pointer to a string (always, never returns NULL).
 */
const char *wc_XsecureErrorToString(int err)
{
    if (err == 0) {
        return "Success";
    }
    if (err == 1) {
        return "Generic \"Failure\".";
    }
    switch (err & 0xff) {
    case 0x02:
        return "Error when SHA3 init fails.";
    case 0x03:
        return "Error when SHA3 last update fails.";
    case 0x04:
        return "Error when DMA driver fails to update the data to SHA3.";
    case 0x05:
        return "Error when timeout.";
    case 0x06:
        return "Error when SHA3 hash not matched with expected hash.";
    case 0x07:
        return "Error when SHA3 finish fails.";
    case 0x08:
        return "SHA3 Invalid Argument.";
    case 0x09:
        return "SSS Invalid Argument.";
    case 0x0A:
        return "SHA3 State mismatch.";
    case 0x40:
        return "user provided GCM tag does not match calculated tag.";
    case 0x41:
        return "AES key clear error.";
    case 0x42:
        return "AES DPA CM is not supported on device.";
    case 0x43:
        return "Error when AES key write fails.";
    case 0x44:
        return "Error when AES decrypt init fails.";
    case 0x45:
        return "Error when GCM tag not matched with user provided tag.";
    case 0x46:
        return "Error when AES data not matched with expected data.";
    case 0x47:
        return "AES KAT fails.";
    case 0x48:
        return "Error when AESDPACM key write fails.";
    case 0x49:
        return "Error when AESDPACM key load fails.";
    case 0x4A:
        return "Error when AESDPACM sss config fails.";
    case 0x4B:
        return "AESDPACM KAT fails.";
    case 0x4C:
        return "Error 1 when AESDPACM data not matched with expected data.";
    case 0x4D:
        return "Error 2 when AESDPACM data not matched with expected data.";
    case 0x4E:
        return "Error 3 when AESDPACM data not matched with expected data.";
    case 0x4F:
        return "Error 4 when AESDPACM data not matched with expected data.";
    case 0x50:
        return "Error 5 when AESDPACM data not matched with expected data.";
    case 0x51:
        return "AES Invalid Argument.";
    case 0x52:
        return "AES KAT Invalid Argument.";
    case 0x53:
        return "AES State mismatch.";
    case 0x54:
        return "AES - Access to device keys is not allowed for IPI for any operations.";
    case 0x80:
        return "RSA KAT fails.";
    case 0x81:
        return "Error when RSA data not matched with expected data.";
    case 0x82:
        return "RSA Invalid Argument.";
    case 0x83:
        return "RSA KAT Invalid Argument.";
    case 0x84:
        return "RSA State mismatch.";
    case 0xC0:
        return "ECC key is not valid.";
    case 0xC1:
        return "Elliptic KAT fails.";
    case 0xC2:
        return "Elliptic Curve not supported.";
    case 0xC3:
        return "Public key is zero.";
    case 0xC4:
        return "Wrong order of Public key.";
    case 0xC5:
        return "Key not found on curve.";
    case 0xC6:
        return "Signature provided for verification is bad.";
    case 0xC7:
        return "Incorrect hash length for sign generation.";
    case 0xC8:
        return "Incorrect hash length for sign verification.";
    case 0xC9:
        return "Bad random number used for sign generation.";
    case 0xCA:
        return "Error in generating Public key.";
    case 0xCB:
        return "Invalid argument.";
    case 0xCC:
        return "R set to zero.";
    case 0xCD:
        return "S set to zero.";
    case 0xCE:
        return "R is not within ECC order.";
    case 0xCF:
        return "S is not within ECC order.";
    case 0xD0:
        return "Curve not supported for KAT.";
    case 0xF0:
        return "No tamper response when tamper interrupt is detected.";
    case 0xF1:
        return "Access to Xilsecure IPIs is disabled if IPI request is non secure.";
    case 0xF2:
        return "SHA3 busy with earlier operation, Kat can't be executed.";
    case 0xF3:
        return "AES busy with earlier operation, Kat can't be executed.";
    default:
        return "Unknown error code.";
    }
}

#endif
