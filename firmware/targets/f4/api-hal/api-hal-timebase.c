#include <api-hal-timebase.h>
#include <api-hal-timebase-timer.h>

#include <stm32wbxx_hal.h>

#define API_HAL_TIMEBASE_TICK_FREQUENCY 32768

typedef struct {
    uint32_t tick;
    uint16_t insomnia;
} ApiHatTimbase;

ApiHatTimbase api_hal_timebase = {
    .tick = 0,
    .insomnia = 0,
};

void LPTIM2_IRQHandler(void) {
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        xPortSysTickHandler();
    }
}

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    if (api_hal_timebase.insomnia) {
        return;
    }

    HAL_SuspendTick();

    __disable_irq();
    xPortSysTickHandler();
    LL_LPTIM_SetAutoReload(API_HAL_TIMER, xExpectedIdleTime);

    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    uint32_t counter = LL_LPTIM_GetCounter(API_HAL_TIMER);
    vTaskStepTick(counter > xExpectedIdleTime ? xExpectedIdleTime : counter);

    __enable_irq();
    HAL_ResumeTick();
}
