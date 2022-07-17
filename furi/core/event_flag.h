#pragma once

#include "base.h"

#ifdef __cplusplus
extern "C" {
#endif

/// \details Event Flags ID identifies the event flags.
typedef void FuriEventFlag;

/// Create and Initialize an Event Flags object.
/// \param[in]     attr          event flags attributes; NULL: default values.
/// \return event flags ID for reference by other functions or NULL in case of error.
FuriEventFlag* furi_event_flag_alloc();

/// Set the specified Event Flags.
/// \param[in]     instance         event flags ID obtained by \ref furi_event_flag_alloc.
/// \param[in]     flags         specifies the flags that shall be set.
/// \return event flags after setting or error code if highest bit set.
uint32_t furi_event_flag_set(FuriEventFlag* instance, uint32_t flags);

/// Clear the specified Event Flags.
/// \param[in]     instance         event flags ID obtained by \ref furi_event_flag_alloc.
/// \param[in]     flags         specifies the flags that shall be cleared.
/// \return event flags before clearing or error code if highest bit set.
uint32_t furi_event_flag_clear(FuriEventFlag* instance, uint32_t flags);

/// Get the current Event Flags.
/// \param[in]     instance         event flags ID obtained by \ref furi_event_flag_alloc.
/// \return current event flags.
uint32_t furi_event_flag_get(FuriEventFlag* instance);

/// Wait for one or more Event Flags to become signaled.
/// \param[in]     instance         event flags ID obtained by \ref furi_event_flag_alloc.
/// \param[in]     flags         specifies the flags to wait for.
/// \param[in]     options       specifies flags options (osFlagsXxxx).
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return event flags before clearing or error code if highest bit set.
uint32_t
    furi_event_flag_wait(FuriEventFlag* instance, uint32_t flags, uint32_t options, uint32_t timeout);

/// Delete an Event Flags object.
/// \param[in]     instance         event flags ID obtained by \ref furi_event_flag_alloc.
void furi_event_flag_free(FuriEventFlag* instance);

#ifdef __cplusplus
}
#endif
