
#include "api-hal-tim.h"

#include <furi.h>
#include <main.h>
#include <stm32wbxx_ll_tim.h>

volatile ApiHalTimISR api_hal_tim_tim2_isr = NULL;

void TIM2_IRQHandler(void) {
    if (api_hal_tim_tim2_isr) {
        api_hal_tim_tim2_isr();
    } else {
        HAL_TIM_IRQHandler(&htim2);
    }
}

void api_hal_tim_set_isr(TIM_TypeDef *timer, ApiHalTimISR isr) {
    if (timer == TIM2) {
        api_hal_tim_tim2_isr = isr;
    } else {
        furi_check(0);
    }
}
