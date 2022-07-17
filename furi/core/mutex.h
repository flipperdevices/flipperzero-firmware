#pragma once

#include "base.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FuriMutexTypeNormal,
    FuriMutexTypeRecursive,
} FuriMutexType;

/// \details Mutex ID identifies the mutex.
typedef void FuriMutex;

/// Create and Initialize a Mutex object.
/// \param[in]     attr          mutex attributes; NULL: default values.
/// \return mutex ID for reference by other functions or NULL in case of error.
FuriMutex* furi_mutex_alloc(FuriMutexType type);

/// Delete a Mutex object.
/// \param[in]     instance      mutex ID obtained by \ref furi_mutex_alloc.
void furi_mutex_free(FuriMutex* instance);

/// Acquire a Mutex or timeout if it is locked.
/// \param[in]     instance      mutex ID obtained by \ref furi_mutex_alloc.
/// \param[in]     timeout       \ref CMSIS_RTOS_TimeOutValue or 0 in case of no time-out.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_mutex_acquire(FuriMutex* instance, uint32_t timeout);

/// Release a Mutex that was acquired by \ref furi_mutex_acquire.
/// \param[in]     instance      mutex ID obtained by \ref furi_mutex_alloc.
/// \return status code that indicates the execution status of the function.
osStatus_t furi_mutex_release(FuriMutex* instance);

FuriThreadId furi_mutex_get_owner(FuriMutex* instance);

#ifdef __cplusplus
}
#endif
