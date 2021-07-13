#include "api-hal-interrupt.h"

#include <furi.h>
#include <main.h>
#include <stm32wbxx_ll_tim.h>

volatile ApiHalInterruptISR api_hal_tim_tim2_isr = NULL;

void TIM2_IRQHandler(void) {
    if (api_hal_tim_tim2_isr) {
        api_hal_tim_tim2_isr();
    } else {
        HAL_TIM_IRQHandler(&htim2);
    }
}

void DMAMUX1_OVR_IRQHandler(void) {
}

void DMA1_Channel1_IRQHandler(void) {
}

void api_hal_interrupt_init() {
    NVIC_SetPriority(DMA1_Channel1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    NVIC_SetPriority(DMAMUX1_OVR_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(), 5, 0));
    NVIC_EnableIRQ(DMAMUX1_OVR_IRQn);
}

void api_hal_interrupt_set_timer_isr(TIM_TypeDef *timer, ApiHalInterruptISR isr) {
    if (timer == TIM2) {
        if (isr) {
            furi_assert(api_hal_tim_tim2_isr == NULL);
        } else {
            furi_assert(api_hal_tim_tim2_isr != NULL);
        }
        api_hal_tim_tim2_isr = isr;
    } else {
        furi_check(0);
    }
}

extern void api_interrupt_call(InterruptType type, void* hw);

/* ST HAL symbols */

/* Comparator trigger event */
void HAL_COMP_TriggerCallback(COMP_HandleTypeDef* hcomp) {
    api_interrupt_call(InterruptTypeComparatorTrigger, hcomp);
}

/* Timer update event */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    api_interrupt_call(InterruptTypeTimerUpdate, htim);
}
