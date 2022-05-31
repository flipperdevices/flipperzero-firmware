/*
 * FreeRTOS Kernel <DEVELOPMENT BRANCH>
 * Copyright (C) 2003-2015 Cadence Design Systems, Inc.
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

/*******************************************************************************
*
*       XTENSA INITIALIZATION ROUTINES CODED IN C
*
*  This file contains miscellaneous Xtensa RTOS-generic initialization functions
*  that are implemented in C.
*
*******************************************************************************/


#ifdef XT_BOARD
    #include    <xtensa/xtbsp.h>
#endif

#include    "xtensa_rtos.h"
#include "esp_idf_version.h"
#if (ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0))
#include    "esp_clk.h"
#else
#if CONFIG_IDF_TARGET_ESP32S2
#include    "esp32s2/clk.h"
#elif CONFIG_IDF_TARGET_ESP32
#include    "esp32/clk.h"
#endif
#endif /* ESP_IDF_VERSION < ESP_IDF_VERSION_VAL(4, 2, 0) */

#ifdef XT_RTOS_TIMER_INT

    unsigned _xt_tick_divisor = 0; /* cached number of cycles per tick */

    void _xt_tick_divisor_init( void )
    {
        _xt_tick_divisor = esp_clk_cpu_freq() / XT_TICK_PER_SEC;
    }

/* Deprecated, to be removed */
    int xt_clock_freq( void )
    {
        return esp_clk_cpu_freq();
    }

#endif /* XT_RTOS_TIMER_INT */
