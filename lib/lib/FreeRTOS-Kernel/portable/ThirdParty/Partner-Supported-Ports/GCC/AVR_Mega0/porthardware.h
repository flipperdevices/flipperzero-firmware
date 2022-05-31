/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef PORTHARDWARE_H
#define PORTHARDWARE_H

#include "FreeRTOSConfig.h"

/*-----------------------------------------------------------*/

#define CLR_INT( FLAG_REG, FLAG_MASK )                                         \
    asm volatile (                                                             \
        "push r16\n\t"                                                         \
        "ldi r16, %1\n\t"                                                      \
        "sts %0, r16\n\t"                                                      \
        "pop r16\n\t"                                                          \
        :                                                                      \
        : "i" ( _SFR_MEM_ADDR( FLAG_REG ) ), "i" ( ( uint8_t ) ( FLAG_MASK ) ) \
        );

#if ( configUSE_TIMER_INSTANCE == 0 )

    #define TICK_INT_vect    TCB0_INT_vect
    #define INT_FLAGS        TCB0_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB0.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB0.INTCTRL = TCB_CAPT_bm;                          \
        TCB0.CTRLA = TCB_ENABLE_bm;                          \
    }
    
    #define TICK_TMR_STOP()   TCB0.CTRLA = 0x00;
    #define TICK_TMR_START()                                 \
    {                                                        \
        TCB0.INTFLAGS = TCB_CAPT_bm;                         \
        TCB0.CTRLA = TCB_ENABLE_bm;                          \
    }
    #define TICK_TMR_READ()     TCB0.CNT
    #define TICK_INT_READY()    (TCB0.INTCTRL & TCB_CAPT_bm)

#elif ( configUSE_TIMER_INSTANCE == 1 )

    #define TICK_INT_vect    TCB1_INT_vect
    #define INT_FLAGS        TCB1_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB1.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB1.INTCTRL = TCB_CAPT_bm;                          \
        TCB1.CTRLA = TCB_ENABLE_bm;                          \
    }
    
    #define TICK_TMR_STOP()   TCB1.CTRLA = 0x00;
    #define TICK_TMR_START()                                 \
    {                                                        \
        TCB1.INTFLAGS = TCB_CAPT_bm;                         \
        TCB1.CTRLA = TCB_ENABLE_bm;                          \
    }
    #define TICK_TMR_READ()     TCB1.CNT
    #define TICK_INT_READY()    (TCB1.INTCTRL & TCB_CAPT_bm)

#elif ( configUSE_TIMER_INSTANCE == 2 )

    #define TICK_INT_vect    TCB2_INT_vect
    #define INT_FLAGS        TCB2_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB2.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB2.INTCTRL = TCB_CAPT_bm;                          \
        TCB2.CTRLA = TCB_ENABLE_bm;                          \
    }
    
    #define TICK_TMR_STOP()   TCB2.CTRLA = 0x00;
    #define TICK_TMR_START()                                 \
    {                                                        \
        TCB2.INTFLAGS = TCB_CAPT_bm;                         \
        TCB2.CTRLA = TCB_ENABLE_bm;                          \
    }
    #define TICK_TMR_READ()     TCB2.CNT
    #define TICK_INT_READY()    (TCB2.INTCTRL & TCB_CAPT_bm)

#elif ( configUSE_TIMER_INSTANCE == 3 )

    #define TICK_INT_vect    TCB3_INT_vect
    #define INT_FLAGS        TCB3_INTFLAGS
    #define INT_MASK         TCB_CAPT_bm

    #define TICK_init()                                      \
    {                                                        \
        TCB3.CCMP = configCPU_CLOCK_HZ / configTICK_RATE_HZ; \
        TCB3.INTCTRL = TCB_CAPT_bm;                          \
        TCB3.CTRLA = TCB_ENABLE_bm;                          \
    }
    
    #define TICK_TMR_STOP()   TCB3.CTRLA = 0x00;
    #define TICK_TMR_START()                                 \
    {                                                        \
        TCB3.INTFLAGS = TCB_CAPT_bm;                         \
        TCB3.CTRLA = TCB_ENABLE_bm;                          \
    }
    #define TICK_TMR_READ()     TCB3.CNT
    #define TICK_INT_READY()    (TCB3.INTCTRL & TCB_CAPT_bm)

#elif ( configUSE_TIMER_INSTANCE == 4 )

    #define TICK_INT_vect    RTC_CNT_vect
    #define INT_FLAGS        RTC_INTFLAGS
    #define INT_MASK         RTC_OVF_bm

/* Hertz to period for RTC setup */
    #define RTC_PERIOD_HZ( x )    ( 32768 * ( ( 1.0 / x ) ) )
    #define TICK_init()                                        \
    {                                                          \
        while( RTC.STATUS > 0 ) {; }                           \
        RTC.CTRLA = RTC_PRESCALER_DIV1_gc | 1 << RTC_RTCEN_bp; \
        RTC.PER = RTC_PERIOD_HZ( configTICK_RATE_HZ );         \
        RTC.INTCTRL |= 1 << RTC_OVF_bp;                        \
    }

#else /* if ( configUSE_TIMER_INSTANCE == 0 ) */
    #undef TICK_INT_vect
    #undef INT_FLAGS
    #undef INT_MASK
    #undef TICK_init()
    #error Invalid timer setting.
#endif /* if ( configUSE_TIMER_INSTANCE == 0 ) */


#if ( configUSE_TICKLESS_IDLE == 1 )

#define LOW_POWER_CLOCK     (32768UL)

#define RTC_TICK_PERIOD_MS            ( ( TickType_t ) 1000 / configTICK_RATE_HZ )
#define RTC_TICKS_TO_COUNTS(tick_cnt) (uint32_t)(((float)(tick_cnt * LOW_POWER_CLOCK)/configTICK_RATE_HZ) - 0.5)
#define RTC_COUNTS_TO_TICKS(counts)   (uint32_t)((float)((counts * 1.0) * configTICK_RATE_HZ)/LOW_POWER_CLOCK  )


#define RTC_INIT()                                                          \
{                                                                           \
	while( RTC.STATUS > 0 ) {; }                                            \
	RTC.PER = 0xFFFF;                                                       \
	RTC.CMP = 0x3FFF;                                                       \
	RTC.CNT = 0;                                                            \
	RTC.INTFLAGS = RTC_OVF_bm | RTC_CMP_bm;                                 \
	RTC.CTRLA = RTC_RUNSTDBY_bm | RTC_PRESCALER_DIV1_gc | RTC_RTCEN_bm ;    \
	RTC.INTCTRL = RTC_OVF_bm | RTC_CMP_bm;                                  \
}

#endif

/*-----------------------------------------------------------*/

#endif /* PORTHARDWARE_H */
