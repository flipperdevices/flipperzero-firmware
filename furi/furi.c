#include "furi.h"
#include <string.h>
#include "queue.h"

void furi_init() {
    furi_assert(!furi_is_irq_context());
    furi_assert(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED);

    furi_log_init();
    furi_record_init();
    furi_stdglue_init();
}

void furi_run() {
    furi_assert(!furi_is_irq_context());
    furi_assert(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED);

#if (__ARM_ARCH_7A__ == 0U)
    /* Service Call interrupt might be configured before kernel start     */
    /* and when its priority is lower or equal to BASEPRI, svc intruction */
    /* causes a Hard Fault.                                               */
    NVIC_SetPriority (SVCall_IRQn, 0U);
#endif

    /* Start the kernel scheduler */
    vTaskStartScheduler();
}

/*
    Lock the RTOS Kernel scheduler.
*/
int32_t furi_kernel_lock() {
    furi_assert(!furi_is_irq_context());

    int32_t lock;

    switch (xTaskGetSchedulerState()) {
        case taskSCHEDULER_SUSPENDED:
            lock = 1;
            break;

        case taskSCHEDULER_RUNNING:
            vTaskSuspendAll();
            lock = 0;
            break;

        case taskSCHEDULER_NOT_STARTED:
        default:
            lock = (int32_t)osError;
            break;
    }

    /* Return previous lock state */
    return (lock);
}

/*
    Unlock the RTOS Kernel scheduler.
*/
int32_t furi_kernel_unlock() {
    furi_assert(!furi_is_irq_context());

    int32_t lock;

    switch (xTaskGetSchedulerState()) {
        case taskSCHEDULER_SUSPENDED:
            lock = 1;

            if (xTaskResumeAll() != pdTRUE) {
                if (xTaskGetSchedulerState() == taskSCHEDULER_SUSPENDED) {
                    lock = (int32_t)osError;
                }
            }
            break;

        case taskSCHEDULER_RUNNING:
            lock = 0;
            break;

        case taskSCHEDULER_NOT_STARTED:
        default:
            lock = (int32_t)osError;
            break;
    }

    /* Return previous lock state */
    return (lock);
}

/*
    Restore the RTOS Kernel scheduler lock state.
*/
int32_t furi_kernel_restore_lock (int32_t lock) {
    furi_assert(!furi_is_irq_context());

    switch (xTaskGetSchedulerState()) {
        case taskSCHEDULER_SUSPENDED:
        case taskSCHEDULER_RUNNING:
            if (lock == 1) {
                vTaskSuspendAll();
            }
            else {
                if (lock != 0) {
                    lock = (int32_t)osError;
                }
                else {
                    if (xTaskResumeAll() != pdTRUE) {
                        if (xTaskGetSchedulerState() != taskSCHEDULER_RUNNING) {
                            lock = (int32_t)osError;
                        }
                    }
                }
            }
            break;

        case taskSCHEDULER_NOT_STARTED:
        default:
            lock = (int32_t)osError;
            break;
    }

    /* Return new lock state */
    return (lock);
}

/*
    Get the RTOS kernel tick frequency.
*/
uint32_t furi_kernel_get_tick_frequency() {
    /* Return frequency in hertz */
    return (configTICK_RATE_HZ);
}

osStatus_t osDelay(uint32_t ticks) {
    furi_assert(!furi_is_irq_context());
    osStatus_t stat;

    stat = osOK;

    if (ticks != 0U) {
        vTaskDelay(ticks);
    }

    /* Return execution status */
    return (stat);
}

osStatus_t osDelayUntil(uint32_t ticks) {
    furi_assert(!furi_is_irq_context());

    TickType_t tcnt, delay;
    osStatus_t stat;

    stat = osOK;
    tcnt = xTaskGetTickCount();

    /* Determine remaining number of ticks to delay */
    delay = (TickType_t)ticks - tcnt;

    /* Check if target tick has not expired */
    if((delay != 0U) && (0 == (delay >> (8 * sizeof(TickType_t) - 1)))) {
        if (xTaskDelayUntil (&tcnt, delay) == pdFALSE) {
            /* Did not delay */
            stat = osError;
        }
    }
    else
    {
        /* No delay or already expired */
        stat = osErrorParameter;
    }

    /* Return execution status */
    return (stat);
}
