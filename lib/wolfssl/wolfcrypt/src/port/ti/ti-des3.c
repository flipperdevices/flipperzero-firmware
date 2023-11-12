/* port/ti/ti-des.c
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

#ifndef NO_DES3

#if defined(WOLFSSL_TI_CRYPT)
#include <stdbool.h>
#include <stdint.h>

#include <wolfssl/wolfcrypt/des3.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/ti/ti-ccm.h>

#include "inc/hw_des.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/des.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"

static int  DesSetIV(Des* des, const byte* iv, int tri)
{
    if (des == NULL)
        return BAD_FUNC_ARG;

    if (iv)
        XMEMCPY(des->reg, iv, tri == DES_CFG_TRIPLE ? DES3_IVLEN : DES_IVLEN);
    else
        XMEMSET(des->reg,  0, tri == DES_CFG_TRIPLE ? DES3_IVLEN : DES_IVLEN);

    return 0;
}

static int  DesSetKey(Des* des, const byte* key, const byte* iv,int dir, int tri)
{
    if(!wolfSSL_TI_CCMInit())return 1 ;
    if ((des == NULL) || (key == NULL) || (iv == NULL))
        return BAD_FUNC_ARG;
    if(!((dir == DES_ENCRYPTION) || (dir == DES_DECRYPTION)))
        return BAD_FUNC_ARG;

    XMEMCPY(des->key, key, tri == DES_CFG_SINGLE ? DES_KEYLEN : DES3_KEYLEN) ;
    return DesSetIV(des, iv, tri);
}

static int  DesCbcAlign16(Des* des, byte* out, const byte* in, word32 sz, word32 dir, word32 tri)
{

    wolfSSL_TI_lockCCM() ;
    ROM_DESReset(DES_BASE);
    ROM_DESConfigSet(DES_BASE, (dir | DES_CFG_MODE_CBC | tri));
    ROM_DESIVSet(DES_BASE, (uint32_t*)des->reg);
    ROM_DESKeySet(DES_BASE,(uint32_t*)des->key);
    if(dir == DES_CFG_DIR_DECRYPT)
        /* if input and output same will overwrite input iv */
        XMEMCPY(des->tmp, in + sz - DES_BLOCK_SIZE, DES_BLOCK_SIZE);
    ROM_DESDataProcess(DES_BASE, (uint32_t *)in, (uint32_t *)out, sz);
    wolfSSL_TI_unlockCCM() ;

    /* store iv for next call */
    if(dir == DES_CFG_DIR_ENCRYPT)
        XMEMCPY(des->reg, out + sz - DES_BLOCK_SIZE, DES_BLOCK_SIZE);
    else
        XMEMCPY(des->reg, des->tmp, DES_BLOCK_SIZE);

    return 0 ;
}

#define IS_ALIGN16(p) (((unsigned int)(p)&0xf) == 0)

static int  DesCbc(Des* des, byte* out, const byte* in, word32 sz, word32 dir, word32 tri)
{
    const byte * in_p ; byte * out_p ;
    word32 size ;
    #define TI_BUFFSIZE 1024
    byte buff[TI_BUFFSIZE] ;
    if ((des == NULL) || (in == NULL) || (out == NULL))
        return BAD_FUNC_ARG;
    if(sz % DES_BLOCK_SIZE)
        return BAD_FUNC_ARG;

    while(sz > 0) {
        size = sz ; in_p = in ; out_p = out ;
        if(!IS_ALIGN16(in)){
            size = sz>TI_BUFFSIZE ? TI_BUFFSIZE : sz ;
            XMEMCPY(buff, in, size) ;
            in_p = (const byte *)buff ;
        }
        if(!IS_ALIGN16(out)){
            size = sz>TI_BUFFSIZE ? TI_BUFFSIZE : sz ;
            out_p = (byte *)buff ;
        }

        DesCbcAlign16(des, out_p, in_p, size, dir, tri) ;

        if(!IS_ALIGN16(out)){
            XMEMCPY(out, buff, size) ;
        }
        sz -= size ; in += size ; out += size ;
    }
    return 0 ;
}

WOLFSSL_API int  wc_Des_SetKey(Des* des, const byte* key, const byte* iv,int dir)
{
    return DesSetKey(des, key, iv, dir, DES_CFG_SINGLE) ;
}

WOLFSSL_API void  wc_Des_SetIV(Des* des, const byte* iv)
{
    DesSetIV(des, iv, DES_CFG_SINGLE) ;
}

WOLFSSL_API int  wc_Des3_SetKey(Des3* des, const byte* key, const byte* iv,int dir)
{
    return DesSetKey((Des *)des, key, iv, dir, DES_CFG_TRIPLE) ;
}

WOLFSSL_API int  wc_Des3_SetIV(Des3* des, const byte* iv)
{
    return DesSetIV((Des *)des, iv, DES_CFG_TRIPLE) ;
}


WOLFSSL_API int  wc_Des_CbcEncrypt(Des* des, byte* out, const byte* in, word32 sz)
{
    return DesCbc(des, out, in, sz, DES_CFG_DIR_ENCRYPT, DES_CFG_SINGLE) ;
}

WOLFSSL_API int  wc_Des_CbcDecrypt(Des* des, byte* out, const byte* in, word32 sz)
{
    return DesCbc(des, out, in, sz, DES_CFG_DIR_DECRYPT, DES_CFG_SINGLE) ;
}

WOLFSSL_API int  wc_Des_CbcDecryptWithKey(byte* out, const byte* in, word32 sz,
                                          const byte* key, const byte* iv)
{
    (void)out; (void)in; (void)sz; (void)key; (void)iv ;
    return -1 ;
}

WOLFSSL_API int  wc_Des3_CbcEncrypt(Des3* des, byte* out, const byte* in, word32 sz)
{
    return DesCbc((Des *)des, out, in, sz, DES_CFG_DIR_ENCRYPT, DES_CFG_TRIPLE) ;
}

WOLFSSL_API int  wc_Des3_CbcDecrypt(Des3* des, byte* out, const byte* in, word32 sz)
{
    return DesCbc((Des *)des, out, in, sz, DES_CFG_DIR_DECRYPT, DES_CFG_TRIPLE) ;
}

WOLFSSL_API int  wc_Des3_CbcDecryptWithKey(byte* out, const byte* in, word32 sz,
                                               const byte* key, const byte* iv)
{
     (void)out; (void)in; (void)sz; (void)key; (void)iv ;
     return -1 ;
 }

WOLFSSL_API int wc_Des3Init(Des3* des, void* heap, int devId)
{
    if (des == NULL)
        return BAD_FUNC_ARG;

    des->heap = heap;
    (void)devId;

    return 0;
}

WOLFSSL_API void wc_Des3Free(Des3* des)
{
    (void)des;
}


#endif /* WOLFSSL_TI_CRYPT */

#endif /* !NO_DES3 */
