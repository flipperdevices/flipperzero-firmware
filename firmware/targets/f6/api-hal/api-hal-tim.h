#pragma once

#include <stm32wbxx_ll_tim.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Timer ISR */
typedef void (*ApiHalTimISR)();

/** Set timer ISR
 * By default ISR is serviced by HAL use this function to override it.
 * We don't clear interrupt flags for you, do it by your self.
 * @timer - timer instance
 * @isr - your interrupt service routine or use NULL to clear
 */
void api_hal_tim_set_isr(TIM_TypeDef *timer, ApiHalTimISR isr);


#ifdef __cplusplus
}
#endif
