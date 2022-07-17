#pragma once

#include <stdlib.h>

#include <core/check.h>
#include <core/common_defines.h>
#include <core/event_flags.h>
#include <core/log.h>
#include <core/memmgr.h>
#include <core/memmgr_heap.h>
#include <core/message_queue.h>
#include <core/mutex.h>
#include <core/pubsub.h>
#include <core/record.h>
#include <core/semaphore.h>
#include <core/stdglue.h>
#include <core/thread.h>
#include <core/timer.h>
#include <core/valuemutex.h>

#include <furi_hal_gpio.h>

// FreeRTOS timer, REMOVE AFTER REFACTORING
#include <timers.h>

#ifdef __cplusplus
extern "C" {
#endif

void furi_init();

void furi_run();

/// Lock the RTOS Kernel scheduler.
/// \return previous lock state (1 - locked, 0 - not locked, error code if negative).
int32_t furi_kernel_lock();

/// Unlock the RTOS Kernel scheduler.
/// \return previous lock state (1 - locked, 0 - not locked, error code if negative).
int32_t furi_kernel_unlock();

/// Restore the RTOS Kernel scheduler lock state.
/// \param[in]     lock          lock state obtained by \ref furi_kernel_lock or \ref furi_kernel_unlock.
/// \return new lock state (1 - locked, 0 - not locked, error code if negative).
int32_t furi_kernel_restore_lock(int32_t lock);

/// Get the RTOS kernel tick frequency.
/// \return frequency of the kernel tick in hertz, i.e. kernel ticks per second.
uint32_t furi_kernel_get_tick_frequency();

//  ==== Generic Wait Functions ====

/// Wait for Timeout (Time Delay).
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value
/// \return status code that indicates the execution status of the function.
osStatus_t osDelay(uint32_t ticks);

/// Wait until specified time.
/// \param[in]     ticks         absolute time in ticks
/// \return status code that indicates the execution status of the function.
osStatus_t osDelayUntil(uint32_t ticks);

#ifdef __cplusplus
}
#endif
