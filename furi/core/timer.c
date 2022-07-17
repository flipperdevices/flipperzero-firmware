#include "timer.h"
#include "check.h"

#include "core/common_defines.h"
#include <FreeRTOS.h>
#include <timers.h>

/* Timer callback information structure definition */
typedef struct {
    FuriTimerCallback func;
    void         *context;
} TimerCallback_t;

static void TimerCallback(TimerHandle_t hTimer) {
    TimerCallback_t *callb;

    /* Retrieve pointer to callback function and context */
    callb = (TimerCallback_t *)pvTimerGetTimerID (hTimer);

    /* Remove dynamic allocation flag */
    callb = (TimerCallback_t *)((uint32_t)callb & ~1U);

    if (callb != NULL) {
        callb->func (callb->context);
    }
}

/*
    Create and Initialize a timer.
*/
FuriTimer* furi_timer_alloc (FuriTimerCallback func, FuriTimerType type, void *context) {
    furi_assert((furi_is_irq_context() == 0U) && (func != NULL));

    TimerHandle_t hTimer;
    TimerCallback_t *callb;
    UBaseType_t reload;
    uint32_t callb_dyn;

    hTimer    = NULL;
    callb     = NULL;
    callb_dyn = 0U;

    /* Dynamic memory allocation is available: if memory for callback and */
    /* its context is not provided, allocate it from dynamic memory pool */
    if (callb == NULL) {
        callb = (TimerCallback_t *)pvPortMalloc (sizeof(TimerCallback_t));

        if (callb != NULL) {
            /* Callback memory was allocated from dynamic pool, set flag */
            callb_dyn = 1U;
        }
    }

    if (callb != NULL) {
        callb->func = func;
        callb->context  = context;

        if (type == osTimerOnce) {
            reload = pdFALSE;
        } else {
            reload = pdTRUE;
        }

        /* Store callback memory dynamic allocation flag */
        callb = (TimerCallback_t *)((uint32_t)callb | callb_dyn);
        /*
            TimerCallback function is always provided as a callback and is used to call application
            specified function with its context both stored in structure callb.
        */
        hTimer = xTimerCreate (NULL, 1, reload, callb, TimerCallback);
        if ((hTimer == NULL) && (callb != NULL) && (callb_dyn == 1U)) {
            /* Failed to create a timer, release allocated resources */
            callb = (TimerCallback_t *)((uint32_t)callb & ~1U);
            vPortFree (callb);
        }
    }

    /* Return timer ID */
    return ((FuriTimer*)hTimer);
}

/*
    Start or restart a timer.
*/
osStatus_t furi_timer_start (FuriTimer* instance, uint32_t ticks) {
    furi_assert(instance);

    TimerHandle_t hTimer = (TimerHandle_t)instance;
    osStatus_t stat;

    if (furi_is_irq_context() != 0U) {
        stat = osErrorISR;
    }
    else {
        if (xTimerChangePeriod (hTimer, ticks, portMAX_DELAY) == pdPASS) {
            stat = osOK;
        } else {
            stat = osErrorResource;
        }
    }

    /* Return execution status */
    return (stat);
}

/*
    Stop a timer.
*/
osStatus_t furi_timer_stop (FuriTimer* instance) {
    furi_assert(instance);

    TimerHandle_t hTimer = (TimerHandle_t)instance;
    osStatus_t stat;

    if (furi_is_irq_context() != 0U) {
        stat = osErrorISR;
    }
    else {
        if (xTimerIsTimerActive (hTimer) == pdFALSE) {
            stat = osErrorResource;
        }
        else {
            if (xTimerStop (hTimer, portMAX_DELAY) == pdPASS) {
                stat = osOK;
            } else {
                stat = osError;
            }
        }
    }

    /* Return execution status */
    return (stat);
}

/*
    Check if a timer is running.
*/
uint32_t furi_timer_is_running (FuriTimer* instance) {
    furi_assert(instance);

    TimerHandle_t hTimer = (TimerHandle_t)instance;
    uint32_t running;

    if ((furi_is_irq_context() != 0U)) {
        running = 0U;
    } else {
        running = (uint32_t)xTimerIsTimerActive (hTimer);
    }

    /* Return 0: not running, 1: running */
    return (running);
}

/*
    Delete a timer.
*/
void furi_timer_free (FuriTimer* instance) {
    furi_assert(!furi_is_irq_context());
    furi_assert(instance);

    TimerHandle_t hTimer = (TimerHandle_t)instance;
    TimerCallback_t *callb;

    callb = (TimerCallback_t *)pvTimerGetTimerID (hTimer);

    if (xTimerDelete (hTimer, portMAX_DELAY) == pdPASS) {
        if ((uint32_t)callb & 1U) {
            /* Callback memory was allocated from dynamic pool, clear flag */
            callb = (TimerCallback_t *)((uint32_t)callb & ~1U);

            /* Return allocated memory to dynamic pool */
            vPortFree (callb);
        }
    }
}

