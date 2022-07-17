#include "kernel.h"
#include "base.h"
#include "check.h"
#include "common_defines.h"

#include CMSIS_device_header

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

uint32_t furi_kernel_get_tick_frequency() {
    /* Return frequency in hertz */
    return (configTICK_RATE_HZ);
}

osStatus_t furi_delay_tick(uint32_t ticks) {
    furi_assert(!furi_is_irq_context());
    osStatus_t stat;

    stat = osOK;

    if (ticks != 0U) {
        vTaskDelay(ticks);
    }

    /* Return execution status */
    return (stat);
}

osStatus_t furi_delay_until_tick(uint32_t ticks) {
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

uint32_t furi_instructions_per_microsecond() {
    return SystemCoreClock / 1000000;
}

uint32_t furi_get_tick() {
    TickType_t ticks;

    if(furi_is_irq_context() != 0U) {
        ticks = xTaskGetTickCountFromISR();
    } else {
        ticks = xTaskGetTickCount();
    }

    return ticks;
}

uint32_t furi_ms_to_ticks(float milliseconds) {
    return milliseconds / (1000.0f / furi_kernel_get_tick_frequency());
}

void furi_delay_us(float microseconds) {
    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = microseconds * furi_instructions_per_microsecond();
    while((DWT->CYCCNT - start) < time_ticks) {
    };
}

void furi_delay_ms(float milliseconds) {
    if(!FURI_IS_ISR() && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        uint32_t ticks = milliseconds / (1000.0f / furi_kernel_get_tick_frequency());
        osStatus_t result = furi_delay_tick(ticks);
        (void)result;
        furi_assert(result == osOK);
    } else {
        furi_delay_us(milliseconds * 1000);
    }
}
