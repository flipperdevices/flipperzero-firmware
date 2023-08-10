/* lpc_18xx_port.c
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


#include "board.h"
#include "otp_18xx_43xx.h" /* For RNG */
#include "timer_18xx_43xx.h"

static uint32_t mTimeInit = 0;
#define TIMER_SCALER    1000000
static void init_time(void)
{
    if(mTimeInit == 0) {
        uint32_t timerFreq;

        /* Set current time for RTC 2:00:00PM, 2012-10-05 */
        RTC_TIME_T FullTime;

        Chip_RTC_Init(LPC_RTC);

        FullTime.time[RTC_TIMETYPE_SECOND]  = 0;
        FullTime.time[RTC_TIMETYPE_MINUTE]  = 0;
        FullTime.time[RTC_TIMETYPE_HOUR]    = 14;
        FullTime.time[RTC_TIMETYPE_DAYOFMONTH]  = 5;
        FullTime.time[RTC_TIMETYPE_DAYOFWEEK]   = 5;
        FullTime.time[RTC_TIMETYPE_DAYOFYEAR]   = 279;
        FullTime.time[RTC_TIMETYPE_MONTH]   = 10;
        FullTime.time[RTC_TIMETYPE_YEAR]    = 2012;

        Chip_RTC_SetFullTime(LPC_RTC, &FullTime);

        /* Enable RTC (starts increase the tick counter and second counter register) */
        Chip_RTC_Enable(LPC_RTC, ENABLE);

        /* Enable timer 1 clock and reset it */
        Chip_TIMER_Init(LPC_TIMER2);
        Chip_RGU_TriggerReset(RGU_TIMER2_RST);
        while (Chip_RGU_InReset(RGU_TIMER2_RST)) {}

        /* Get timer peripheral clock rate */
        timerFreq = Chip_Clock_GetRate(CLK_MX_TIMER2);

        /* Timer setup */
        Chip_TIMER_Reset(LPC_TIMER2);
        Chip_TIMER_PrescaleSet(LPC_TIMER2, timerFreq/TIMER_SCALER);
        Chip_TIMER_Enable(LPC_TIMER2);

        mTimeInit = 1;
    }
}

double current_time()
{
    //RTC_TIME_T FullTime;
    uint32_t timerMs;

    init_time();
    timerMs = Chip_TIMER_ReadCount(LPC_TIMER2);

    //Chip_RTC_GetFullTime(LPC_RTC, &FullTime);
    //(double)FullTime.time[RTC_TIMETYPE_SECOND]

    return (double)timerMs/TIMER_SCALER;
}

/* Memory location of the generated random numbers (for total of 128 bits) */
static volatile uint32_t* mRandData = (uint32_t*)0x40045050;
static uint32_t mRandInit = 0;
static uint32_t mRandIndex = 0;
uint32_t rand_gen(void)
{
    uint32_t rand = 0;
    uint32_t status = LPC_OK;
    if(mRandIndex == 0) {
        if(mRandInit == 0) {
            Chip_OTP_Init();
            mRandInit = 1;
        }
        status = Chip_OTP_GenRand();
    }
    if(status == LPC_OK) {
        rand = mRandData[mRandIndex];
    }
    else {
        printf("GenRand Failed 0x%x\n", status);
    }
    if(++mRandIndex > 4) {
        mRandIndex = 0;
    }
    return rand;
}
