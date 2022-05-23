/*
 * FreeRTOS Kernel V10.4.3
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 * Copyright (c) 2021 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
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
 */

#ifndef FREERTOS_SDK_CONFIG_H
#define FREERTOS_SDK_CONFIG_H

#ifndef __ASSEMBLER__
    #include "FreeRTOSConfig.h"
    #include "rp2040_config.h"

    #if ( configSUPPORT_PICO_SYNC_INTEROP == 1 )
        // increase the amount of time it may reasonably take to wake us up
        #ifndef PICO_TIME_SLEEP_OVERHEAD_ADJUST_US
        #define PICO_TIME_SLEEP_OVERHEAD_ADJUST_US 150
        #endif

        #define lock_owner_id_t uint32_t
        extern uint32_t ulPortLockGetCurrentOwnerId(void);
        #define lock_get_caller_owner_id() ulPortLockGetCurrentOwnerId()
        #define LOCK_INVALID_OWNER_ID ((uint32_t)-1)

        struct lock_core;
        #ifndef lock_internal_spin_unlock_with_wait
        extern void vPortLockInternalSpinUnlockWithWait( struct lock_core *pxLock, uint32_t ulSave);
        #define lock_internal_spin_unlock_with_wait(lock, save) vPortLockInternalSpinUnlockWithWait(lock, save)
        #endif

        #ifndef lock_internal_spin_unlock_with_notify
        extern void vPortLockInternalSpinUnlockWithNotify( struct lock_core *pxLock, uint32_t save);
        #define lock_internal_spin_unlock_with_notify(lock, save) vPortLockInternalSpinUnlockWithNotify(lock, save);
        #endif

        #ifndef lock_internal_spin_unlock_with_best_effort_wait_or_timeout
        extern bool xPortLockInternalSpinUnlockWithBestEffortWaitOrTimeout( struct lock_core *pxLock, uint32_t ulSave, absolute_time_t uxUntil);
        #define lock_internal_spin_unlock_with_best_effort_wait_or_timeout(lock, save, until) \
            xPortLockInternalSpinUnlockWithBestEffortWaitOrTimeout(lock, save, until)
        #endif
    #endif /* configSUPPORT_PICO_SYNC_INTEROP */

    #if ( configSUPPORT_PICO_TIME_INTEROP == 1 )
        extern void xPortSyncInternalYieldUntilBefore(absolute_time_t t);
        #define sync_internal_yield_until_before(t) xPortSyncInternalYieldUntilBefore(t)
    #endif /* configSUPPORT_PICO_TIME_INTEROP */
#endif /* __ASSEMBLER__ */
#endif