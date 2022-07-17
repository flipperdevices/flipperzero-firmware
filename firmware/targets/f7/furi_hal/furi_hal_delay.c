#include "furi_hal_delay.h"

#include <furi.h>
#include <stm32wbxx_ll_utils.h>

#define TAG "FuriHalDelay"

void furi_hal_delay_init() {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0U;
}

uint32_t furi_hal_delay_instructions_per_microsecond() {
    return SystemCoreClock / 1000000;
}

uint32_t furi_hal_get_tick(void) {
    TickType_t ticks;

    if(furi_is_irq_context() != 0U) {
        ticks = xTaskGetTickCountFromISR();
    } else {
        ticks = xTaskGetTickCount();
    }

    return ticks;
}

uint32_t furi_hal_ms_to_ticks(float milliseconds) {
    return milliseconds / (1000.0f / furi_kernel_get_tick_frequency());
}

void furi_hal_delay_us(float microseconds) {
    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = microseconds * furi_hal_delay_instructions_per_microsecond();
    while((DWT->CYCCNT - start) < time_ticks) {
    };
}

void furi_hal_delay_ms(float milliseconds) {
    if(!FURI_IS_ISR() && xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) {
        uint32_t ticks = milliseconds / (1000.0f / furi_kernel_get_tick_frequency());
        osStatus_t result = osDelay(ticks);
        (void)result;
        furi_assert(result == osOK);
    } else {
        furi_hal_delay_us(milliseconds * 1000);
    }
}
