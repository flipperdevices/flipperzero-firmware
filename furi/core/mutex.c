#include "mutex.h"
#include "check.h"
#include "common_defines.h"

#include <semphr.h>

FuriMutex* furi_mutex_alloc(FuriMutexType type) {
    furi_assert(!FURI_IS_IRQ_MODE());

    SemaphoreHandle_t hMutex = NULL;

    if(type == FuriMutexTypeNormal) {
        hMutex = xSemaphoreCreateMutex();
    } else if(type == FuriMutexTypeRecursive) {
        hMutex = xSemaphoreCreateRecursiveMutex();
    } else {
        furi_crash("Programming error");
    }

    furi_check(hMutex != NULL);

    if(type == FuriMutexTypeRecursive) {
        /* Set LSB as 'recursive mutex flag' */
        hMutex = (SemaphoreHandle_t)((uint32_t)hMutex | 1U);
    }

    /* Return mutex ID */
    return ((FuriMutex*)hMutex);
}

void furi_mutex_free(FuriMutex* instance) {
    furi_assert(!FURI_IS_IRQ_MODE());
    vSemaphoreDelete((SemaphoreHandle_t)((uint32_t)instance & ~1U));
}

osStatus_t furi_mutex_acquire(FuriMutex* instance, uint32_t timeout) {
    SemaphoreHandle_t hMutex;
    osStatus_t stat;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)instance & 1U;

    stat = osOK;

    if(FURI_IS_IRQ_MODE() != 0U) {
        stat = osErrorISR;
    } else if(hMutex == NULL) {
        stat = osErrorParameter;
    } else {
        if(rmtx != 0U) {
            if(xSemaphoreTakeRecursive(hMutex, timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = osErrorTimeout;
                } else {
                    stat = osErrorResource;
                }
            }
        } else {
            if(xSemaphoreTake(hMutex, timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = osErrorTimeout;
                } else {
                    stat = osErrorResource;
                }
            }
        }
    }

    /* Return execution status */
    return (stat);
}

osStatus_t furi_mutex_release(FuriMutex* instance) {
    SemaphoreHandle_t hMutex;
    osStatus_t stat;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)instance & 1U;

    stat = osOK;

    if(FURI_IS_IRQ_MODE() != 0U) {
        stat = osErrorISR;
    } else if(hMutex == NULL) {
        stat = osErrorParameter;
    } else {
        if(rmtx != 0U) {
            if(xSemaphoreGiveRecursive(hMutex) != pdPASS) {
                stat = osErrorResource;
            }
        } else {
            if(xSemaphoreGive(hMutex) != pdPASS) {
                stat = osErrorResource;
            }
        }
    }

    /* Return execution status */
    return (stat);
}

/*
    Get Thread which owns a Mutex object.
*/
FuriThreadId furi_mutex_get_owner(FuriMutex* instance) {
    SemaphoreHandle_t hMutex;
    FuriThreadId owner;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    if((FURI_IS_IRQ_MODE() != 0U) || (hMutex == NULL)) {
        owner = 0;
    } else {
        owner = (FuriThreadId)xSemaphoreGetMutexHolder(hMutex);
    }

    /* Return owner thread ID */
    return (owner);
}
