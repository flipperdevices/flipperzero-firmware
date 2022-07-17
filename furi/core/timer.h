#pragma once

#include "core/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Timer callback function.
typedef void (*FuriTimerCallback)(void *context);

/// Timer type.
typedef enum {
  osTimerOnce               = 0,          ///< One-shot timer.
  osTimerPeriodic           = 1           ///< Repeating timer.
} FuriTimerType;

/// \details Timer ID identifies the timer.
typedef void FuriTimer;

/// Create and Initialize a timer.
/// \param[in]     func          function pointer to callback function.
/// \param[in]     type          \ref osTimerOnce for one-shot or \ref osTimerPeriodic for periodic behavior.
/// \param[in]     context      context to the timer callback function.
/// \param[in]     attr          timer attributes; NULL: default values.
/// \return timer ID for reference by other functions or NULL in case of error.
FuriTimer* furi_timer_alloc(FuriTimerCallback func, FuriTimerType type, void *context);

/// Start or restart a timer.
/// \param[in]     instance      timer ID obtained by \ref furi_timer_alloc.
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_timer_start(FuriTimer* instance, uint32_t ticks);

/// Stop a timer.
/// \param[in]     instance      timer ID obtained by \ref furi_timer_alloc.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_timer_stop(FuriTimer* instance);

/// Check if a timer is running.
/// \param[in]     instance      timer ID obtained by \ref furi_timer_alloc.
/// \return 0 not running, 1 running.
uint32_t furi_timer_is_running(FuriTimer* instance);

/// Delete a timer.
/// \param[in]     instance      timer ID obtained by \ref furi_timer_alloc.
/// \return status code that indicates the execution status of the function.
void furi_timer_free(FuriTimer* instance);

#ifdef __cplusplus
}
#endif
