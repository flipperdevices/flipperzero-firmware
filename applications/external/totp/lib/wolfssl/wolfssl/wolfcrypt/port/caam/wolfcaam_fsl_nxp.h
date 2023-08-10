/* wolfcaam_fsl_nxp.h
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


/* This file is for interacting with the driver code */
#ifndef WOLFCAAM_FSL_NXP_H
#define WOLFCAAM_FSL_NXP_H

#include <wolfssl/wolfcrypt/types.h>

#ifdef WOLFSSL_IMXRT1170_CAAM

#include "fsl_device_registers.h"
#include "fsl_caam.h"

/* check for patched version of fsl_caam */
#ifdef CAAM_ECC_EXPANSION
    #define WOLFSSL_CAAM_ECC
#endif
#ifdef CAAM_BLOB_EXPANSION
    #define WOLFSSL_CAAM_BLOB
#endif

#define Error int
#define Value int
#define Boolean int
#define Success 1
#define Failure 0
#define INTERRUPT_Panic() WC_DO_NOTHING
#define MemoryMapMayNotBeEmpty -1
#define CAAM_WAITING -2
#define NoActivityReady -1
#define MemoryOperationNotPerformed -1

#define CAAM_ADDRESS uintptr_t
#ifndef WOLFSSL_CAAM_BUFFER
#define WOLFSSL_CAAM_BUFFER
    typedef struct CAAM_BUFFER {
        int BufferType;
        CAAM_ADDRESS TheAddress;
        int Length;
    } CAAM_BUFFER;
#endif

#define DataBuffer 0
#define LastBuffer 0
#define Success 1

/* unique devId for CAAM use on crypto callbacks */
#define WOLFSSL_CAAM_DEVID 7

#include <wolfssl/wolfcrypt/port/caam/wolfcaam_ecdsa.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_aes.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam_hash.h>
#include <wolfssl/wolfcrypt/cryptocb.h>

#define ResourceNotAvailable -3
#define CAAM_WAITING -2

WOLFSSL_LOCAL int SynchronousSendRequest(int type, unsigned int args[4],
        CAAM_BUFFER *buf, int sz);
WOLFSSL_LOCAL int wc_CAAMInitInterface(void);
WOLFSSL_LOCAL void wc_CAAMFreeInterface(void);

#define CAAM_SEND_REQUEST(type, sz, arg, buf) \
        SynchronousSendRequest((type), (arg), (buf), (sz))
#define CAAM_INIT_INTERFACE wc_CAAMInitInterface
#define CAAM_FREE_INTERFACE wc_CAAMFreeInterface
#endif

#define CAAM_BUFFER_ALIGN 16

#endif /* WOLFCAAM_QNX_H */
