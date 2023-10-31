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


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_TI_CRYPT) ||  defined(WOLFSSL_TI_HASH)

#include "wolfssl/wolfcrypt/port/ti/ti-ccm.h"
#include <stdbool.h>
#include <stdint.h>

#ifndef TI_DUMMY_BUILD
#include "driverlib/sysctl.h"
#include "driverlib/rom_map.h"
#include "driverlib/rom.h"

#ifndef SINGLE_THREADED
#include <wolfssl/wolfcrypt/wc_port.h>
    static wolfSSL_Mutex TI_CCM_Mutex;
#endif
#endif /* TI_DUMMY_BUILD */

#define TIMEOUT  500000
#define WAIT(stat) { volatile int i; for(i=0; i<TIMEOUT; i++)if(stat)break; if(i==TIMEOUT)return(false); }

static bool ccm_init = false;
int wolfSSL_TI_CCMInit(void)
{
    if (ccm_init)
        return true;
    ccm_init = true;

#ifndef TI_DUMMY_BUILD
    SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |
                                       SYSCTL_CFG_VCO_480), 120000000);

    if (!ROM_SysCtlPeripheralPresent(SYSCTL_PERIPH_CCM0))
        return false;

         ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_CCM0);
    WAIT(ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_CCM0));
         ROM_SysCtlPeripheralReset(SYSCTL_PERIPH_CCM0);
    WAIT(ROM_SysCtlPeripheralReady(SYSCTL_PERIPH_CCM0));

#ifndef SINGLE_THREADED
    if (wc_InitMutex(&TI_CCM_Mutex))
        return false;
#endif
#endif /* !TI_DUMMY_BUILD */

    return true;
}

#ifndef SINGLE_THREADED
void wolfSSL_TI_lockCCM(void)
{
#ifndef TI_DUMMY_BUILD
    wc_LockMutex(&TI_CCM_Mutex);
#endif
}

void wolfSSL_TI_unlockCCM(void){
#ifndef TI_DUMMY_BUILD
    wc_UnLockMutex(&TI_CCM_Mutex);
#endif
}
#endif /* !SINGLE_THREADED */

#endif /* WOLFSSL_TI_CRYPT || WOLFSSL_TI_HASH */
