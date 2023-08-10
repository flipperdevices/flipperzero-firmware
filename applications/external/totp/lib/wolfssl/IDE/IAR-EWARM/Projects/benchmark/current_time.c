/* current-time.c
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

#ifdef WOLFSSL_TI_CURRTIME
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_timer.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"

void InitTimer(void) {
    uint32_t ui32SysClock = ROM_SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                       SYSCTL_OSC_MAIN |
                                       SYSCTL_USE_PLL |
                                       SYSCTL_CFG_VCO_480), 120000000);

    printf("Clock=%dMHz\n", (int)(ui32SysClock/1000000));
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    ROM_TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
    ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, -1);
    ROM_TimerEnable(TIMER0_BASE, TIMER_A);
}

static int initFlag = false ;
double current_time(int reset)
{
    if(!initFlag)InitTimer() ;
    initFlag = true ;
    if(reset)ROM_TimerLoadSet(TIMER0_BASE, TIMER_A, -1);
    return (double)(-(int)ROM_TimerValueGet(TIMER0_BASE, TIMER_A ))/120000000.0 ;
}

#else

/* dummy */
double current_time(int reset) {
    static double t;
    t += 1.0; /* for avoid infinite loop of waiting time */
    if(reset)t = 0.0;
    return t ;
}

#endif
