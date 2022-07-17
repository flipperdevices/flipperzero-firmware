#include "timer.h"

#include "core/common_defines.h"
#include <FreeRTOS.h>
#include <timers.h>


/* ==== Timer Management Functions ==== */

#if (configUSE_OS2_TIMER == 1)

static void TimerCallback (TimerHandle_t hTimer) {
    TimerCallback_t *callb;

    /* Retrieve pointer to callback function and argument */
    callb = (TimerCallback_t *)pvTimerGetTimerID (hTimer);

    /* Remove dynamic allocation flag */
    callb = (TimerCallback_t *)((uint32_t)callb & ~1U);

    if (callb != NULL) {
        callb->func (callb->arg);
    }
}

/*
    Create and Initialize a timer.
*/
osTimerId_t osTimerNew (osTimerFunc_t func, osTimerType_t type, void *argument, const osTimerAttr_t *attr) {
    const char *name;
    TimerHandle_t hTimer;
    TimerCallback_t *callb;
    UBaseType_t reload;
    int32_t mem;
    uint32_t callb_dyn;

    hTimer = NULL;

    if ((furi_is_irq_context() == 0U) && (func != NULL)) {
        callb     = NULL;
        callb_dyn = 0U;

        #if (configSUPPORT_STATIC_ALLOCATION == 1)
            /* Static memory allocation is available: check if memory for control block */
            /* is provided and if it also contains space for callback and its argument  */
            if ((attr != NULL) && (attr->cb_mem != NULL)) {
                if (attr->cb_size >= (sizeof(StaticTimer_t) + sizeof(TimerCallback_t))) {
                    callb = (TimerCallback_t *)((uint32_t)attr->cb_mem + sizeof(StaticTimer_t));
                }
            }
        #endif

        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
            /* Dynamic memory allocation is available: if memory for callback and */
            /* its argument is not provided, allocate it from dynamic memory pool */
            if (callb == NULL) {
                callb = (TimerCallback_t *)pvPortMalloc (sizeof(TimerCallback_t));

                if (callb != NULL) {
                    /* Callback memory was allocated from dynamic pool, set flag */
                    callb_dyn = 1U;
                }
            }
        #endif

        if (callb != NULL) {
            callb->func = func;
            callb->arg  = argument;

            if (type == osTimerOnce) {
                reload = pdFALSE;
            } else {
                reload = pdTRUE;
            }

            mem  = -1;
            name = NULL;

            if (attr != NULL) {
                if (attr->name != NULL) {
                    name = attr->name;
                }

                if ((attr->cb_mem != NULL) && (attr->cb_size >= sizeof(StaticTimer_t))) {
                    /* The memory for control block is provided, use static object */
                    mem = 1;
                }
                else {
                    if ((attr->cb_mem == NULL) && (attr->cb_size == 0U)) {
                        /* Control block will be allocated from the dynamic pool */
                        mem = 0;
                    }
                }
            }
            else {
                mem = 0;
            }
            /* Store callback memory dynamic allocation flag */
            callb = (TimerCallback_t *)((uint32_t)callb | callb_dyn);
            /*
                TimerCallback function is always provided as a callback and is used to call application
                specified function with its argument both stored in structure callb.
            */
            if (mem == 1) {
                #if (configSUPPORT_STATIC_ALLOCATION == 1)
                    hTimer = xTimerCreateStatic (name, 1, reload, callb, TimerCallback, (StaticTimer_t *)attr->cb_mem);
                #endif
            }
            else {
                if (mem == 0) {
                    #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
                        hTimer = xTimerCreate (name, 1, reload, callb, TimerCallback);
                    #endif
                }
            }

            #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
            if ((hTimer == NULL) && (callb != NULL) && (callb_dyn == 1U)) {
                /* Failed to create a timer, release allocated resources */
                callb = (TimerCallback_t *)((uint32_t)callb & ~1U);

                vPortFree (callb);
            }
            #endif
        }
    }

    /* Return timer ID */
    return ((osTimerId_t)hTimer);
}

/*
    Start or restart a timer.
*/
osStatus_t osTimerStart (osTimerId_t timer_id, uint32_t ticks) {
    TimerHandle_t hTimer = (TimerHandle_t)timer_id;
    osStatus_t stat;

    if (furi_is_irq_context() != 0U) {
        stat = osErrorISR;
    }
    else if (hTimer == NULL) {
        stat = osErrorParameter;
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
osStatus_t osTimerStop (osTimerId_t timer_id) {
    TimerHandle_t hTimer = (TimerHandle_t)timer_id;
    osStatus_t stat;

    if (furi_is_irq_context() != 0U) {
        stat = osErrorISR;
    }
    else if (hTimer == NULL) {
        stat = osErrorParameter;
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
uint32_t osTimerIsRunning (osTimerId_t timer_id) {
    TimerHandle_t hTimer = (TimerHandle_t)timer_id;
    uint32_t running;

    if ((furi_is_irq_context() != 0U) || (hTimer == NULL)) {
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
osStatus_t osTimerDelete (osTimerId_t timer_id) {
    TimerHandle_t hTimer = (TimerHandle_t)timer_id;
    osStatus_t stat;
#ifndef USE_FreeRTOS_HEAP_1
#if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
    TimerCallback_t *callb;
#endif

    if (furi_is_irq_context() != 0U) {
        stat = osErrorISR;
    }
    else if (hTimer == NULL) {
        stat = osErrorParameter;
    }
    else {
        #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
        callb = (TimerCallback_t *)pvTimerGetTimerID (hTimer);
        #endif

        if (xTimerDelete (hTimer, portMAX_DELAY) == pdPASS) {
            #if (configSUPPORT_DYNAMIC_ALLOCATION == 1)
                if ((uint32_t)callb & 1U) {
                    /* Callback memory was allocated from dynamic pool, clear flag */
                    callb = (TimerCallback_t *)((uint32_t)callb & ~1U);

                    /* Return allocated memory to dynamic pool */
                    vPortFree (callb);
                }
            #endif
            stat = osOK;
        } else {
            stat = osErrorResource;
        }
    }
#else
    stat = osError;
#endif

    /* Return execution status */
    return (stat);
}
#endif /* (configUSE_OS2_TIMER == 1) */

