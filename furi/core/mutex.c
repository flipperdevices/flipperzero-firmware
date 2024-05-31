#include "mutex.h"
#include "check.h"
#include "common_defines.h"

#include <FreeRTOS.h>
#include <semphr.h>

struct FuriMutex {
    StaticSemaphore_t container;
};

// IMPORTANT: container MUST be the FIRST struct member
static_assert(offsetof(FuriMutex, container) == 0);

FuriMutex* furi_mutex_alloc(FuriMutexType type) {
    furi_check(!FURI_IS_IRQ_MODE());

    FuriMutex* instance = malloc(sizeof(FuriMutex));

    SemaphoreHandle_t hMutex;

    if(type == FuriMutexTypeNormal) {
        hMutex = xSemaphoreCreateMutexStatic(&instance->container);
    } else if(type == FuriMutexTypeRecursive) {
        hMutex = xSemaphoreCreateRecursiveMutexStatic(&instance->container);
    } else {
        furi_crash();
    }

    furi_check(hMutex == (SemaphoreHandle_t)instance);

    if(type == FuriMutexTypeRecursive) {
        /* Set LSB as 'recursive mutex flag' */
        instance = (FuriMutex*)((uint32_t)instance | 1UL);
    }

    return instance;
}

void furi_mutex_free(FuriMutex* instance) {
    furi_check(!FURI_IS_IRQ_MODE());
    furi_check(instance);

    FuriMutex* _instance = (FuriMutex*)((uint32_t)instance & ~1UL);

    vSemaphoreDelete((SemaphoreHandle_t)_instance);
    free(_instance);
}

FuriStatus furi_mutex_acquire(FuriMutex* instance, uint32_t timeout) {
    furi_check(instance);

    SemaphoreHandle_t hMutex;
    FuriStatus stat;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)instance & 1U;

    stat = FuriStatusOk;

    if(FURI_IS_IRQ_MODE()) {
        stat = FuriStatusErrorISR;
    } else if(hMutex == NULL) {
        stat = FuriStatusErrorParameter;
    } else {
        if(rmtx != 0U) {
            if(xSemaphoreTakeRecursive(hMutex, timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = FuriStatusErrorTimeout;
                } else {
                    stat = FuriStatusErrorResource;
                }
            }
        } else {
            if(xSemaphoreTake(hMutex, timeout) != pdPASS) {
                if(timeout != 0U) {
                    stat = FuriStatusErrorTimeout;
                } else {
                    stat = FuriStatusErrorResource;
                }
            }
        }
    }

    /* Return execution status */
    return (stat);
}

FuriStatus furi_mutex_release(FuriMutex* instance) {
    furi_check(instance);

    SemaphoreHandle_t hMutex;
    FuriStatus stat;
    uint32_t rmtx;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    /* Extract recursive mutex flag */
    rmtx = (uint32_t)instance & 1U;

    stat = FuriStatusOk;

    if(FURI_IS_IRQ_MODE()) {
        stat = FuriStatusErrorISR;
    } else if(hMutex == NULL) {
        stat = FuriStatusErrorParameter;
    } else {
        if(rmtx != 0U) {
            if(xSemaphoreGiveRecursive(hMutex) != pdPASS) {
                stat = FuriStatusErrorResource;
            }
        } else {
            if(xSemaphoreGive(hMutex) != pdPASS) {
                stat = FuriStatusErrorResource;
            }
        }
    }

    /* Return execution status */
    return (stat);
}

FuriThreadId furi_mutex_get_owner(FuriMutex* instance) {
    furi_check(instance);

    SemaphoreHandle_t hMutex;
    FuriThreadId owner;

    hMutex = (SemaphoreHandle_t)((uint32_t)instance & ~1U);

    if(hMutex == NULL) {
        owner = 0;
    } else if(FURI_IS_IRQ_MODE()) {
        owner = (FuriThreadId)xSemaphoreGetMutexHolderFromISR(hMutex);
    } else {
        owner = (FuriThreadId)xSemaphoreGetMutexHolder(hMutex);
    }

    /* Return owner thread ID */
    return (owner);
}
