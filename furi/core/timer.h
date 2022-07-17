#pragma once

#include "core/base.h"

#ifdef __cplusplus
extern "C" {
#endif

/// Timer callback function.
typedef void (*osTimerFunc_t) (void *argument);

/// Timer type.
typedef enum {
  osTimerOnce               = 0,          ///< One-shot timer.
  osTimerPeriodic           = 1           ///< Repeating timer.
} osTimerType_t;

/// \details Timer ID identifies the timer.
typedef void *osTimerId_t;

/* Timer callback information structure definition */
typedef struct {
    osTimerFunc_t func;
    void         *arg;
} TimerCallback_t;

/// Attributes structure for timer.
typedef struct {
  const char                   *name;   ///< name of the timer
  uint32_t                 attr_bits;   ///< attribute bits
  void                      *cb_mem;    ///< memory for control block
  uint32_t                   cb_size;   ///< size of provided memory for control block
} osTimerAttr_t;

/// Create and Initialize a timer.
/// \param[in]     func          function pointer to callback function.
/// \param[in]     type          \ref osTimerOnce for one-shot or \ref osTimerPeriodic for periodic behavior.
/// \param[in]     argument      argument to the timer callback function.
/// \param[in]     attr          timer attributes; NULL: default values.
/// \return timer ID for reference by other functions or NULL in case of error.
osTimerId_t osTimerNew(osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr);

/// Start or restart a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \param[in]     ticks         \ref CMSIS_RTOS_TimeOutValue "time ticks" value of the timer.
/// \return status code that indicates the execution status of the function.
osStatus_t osTimerStart(osTimerId_t timer_id, uint32_t ticks);

/// Stop a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osTimerStop(osTimerId_t timer_id);

/// Check if a timer is running.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return 0 not running, 1 running.
uint32_t osTimerIsRunning(osTimerId_t timer_id);

/// Delete a timer.
/// \param[in]     timer_id      timer ID obtained by \ref osTimerNew.
/// \return status code that indicates the execution status of the function.
osStatus_t osTimerDelete(osTimerId_t timer_id);

#ifdef __cplusplus
}
#endif
