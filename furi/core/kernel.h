#pragma once

#include <core/base.h>

#ifdef __cplusplus
extern "C" {
#endif

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
osStatus_t furi_delay_tick(uint32_t ticks);

/// Wait until specified tick.
/// \param[in]     ticks         absolute time in ticks
/// \return status code that indicates the execution status of the function.
osStatus_t furi_delay_until_tick(uint32_t ticks);

/** Get instructions per microsecond count */
uint32_t furi_instructions_per_microsecond();

/** Get current tick counter
 *
 * System uptime, may overflow.
 *
 * @return     Current ticks in milliseconds
 */
uint32_t furi_get_tick(void);

/** Convert milliseconds to ticks
 *
 * @param[in]   milliseconds    time in milliseconds
 * @return      time in ticks
 */
uint32_t furi_ms_to_ticks(float milliseconds);

/** Delay in milliseconds
 * @warning    Cannot be used from ISR
 *
 * @param[in]  milliseconds  milliseconds to wait
 */
void furi_delay_ms(float milliseconds);

/** Delay in microseconds
 *
 * @param[in]  microseconds  microseconds to wait
 */
void furi_delay_us(float microseconds);

#ifdef __cplusplus
}
#endif
