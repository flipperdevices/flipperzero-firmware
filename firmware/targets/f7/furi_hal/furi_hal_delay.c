#include "furi_hal_delay.h"

#include <furi.h>
#include <cmsis_os2.h>

#define TAG "FuriHalDelay"

static uint32_t clk_per_microsecond;
static bool osDelayInitComplete = false;

void furi_hal_delay_init(bool queuedDelay) {
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0U;
    clk_per_microsecond = SystemCoreClock / 1000000.0f;
    osDelayInitComplete = queuedDelay;
    FURI_LOG_I(TAG, "Init OK");
}

void delay_us(float microseconds) {
    uint32_t start = DWT->CYCCNT;
    uint32_t time_ticks = microseconds * clk_per_microsecond;
    while((DWT->CYCCNT - start) < time_ticks) {
    };
}

// cannot be used in ISR
// TODO add delay_ISR variant
void delay(float milliseconds) {
    if(osDelayInitComplete) {
        uint32_t ticks = milliseconds / (1000.0f / osKernelGetTickFreq());
        osStatus_t result = osDelay(ticks);
        (void)result;
        furi_assert(result == osOK);
    } else {
        delay_us(milliseconds * 1000.0f);
    }
}

uint32_t millis(void) {
    return HAL_GetTick();
}