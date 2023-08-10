/* port/ti/ti_ccm.c
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


#ifndef WOLF_CRYPT_TI_CCM_H
#define WOLF_CRYPT_TI_CCM_H

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/types.h>

#if defined(WOLFSSL_TI_CRYPT) ||  defined(WOLFSSL_TI_HASH)

int wolfSSL_TI_CCMInit(void) ;

#ifndef SINGLE_THREADED
void wolfSSL_TI_lockCCM(void) ;
void wolfSSL_TI_unlockCCM(void) ;
#else
#define wolfSSL_TI_lockCCM() WC_DO_NOTHING
#define wolfSSL_TI_unlockCCM() WC_DO_NOTHING
#endif

#endif

#endif

