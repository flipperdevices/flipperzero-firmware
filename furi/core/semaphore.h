#pragma once

#include "base.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void FuriSemaphore;

/// Create and Initialize a Semaphore object.
/// \param[in]     max_count     maximum number of available tokens.
/// \param[in]     initial_count initial number of available tokens.
/// \param[in]     attr          semaphore attributes; NULL: default values.
/// \return semaphore ID for reference by other functions or NULL in case of error.
FuriSemaphore* furi_semaphore_alloc(uint32_t max_count, uint32_t initial_count);

/// Acquire a Semaphore token or timeout if no tokens are available.
/// \param[in]     instance  semaphore ID obtained by \ref furi_semaphore_alloc.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_semaphore_acquire(FuriSemaphore* instance, uint32_t timeout);

/// Release a Semaphore token up to the initial maximum count.
/// \param[in]     instance  semaphore ID obtained by \ref furi_semaphore_alloc.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_semaphore_release(FuriSemaphore* instance);

/// Get current Semaphore token count.
/// \param[in]     instance  semaphore ID obtained by \ref furi_semaphore_alloc.
/// \return number of tokens available.
uint32_t furi_semaphore_get_count(FuriSemaphore* instance);

/// Delete a Semaphore object.
/// \param[in]     instance  semaphore ID obtained by \ref furi_semaphore_alloc.
void furi_semaphore_free(FuriSemaphore* instance);

#ifdef __cplusplus
}
#endif
