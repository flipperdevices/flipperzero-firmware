#include <api-hal-timebase.h>
#include <api-hal-timebase-timer.h>

#include <stm32wbxx_hal.h>
#include <FreeRTOS.h>
#include <cmsis_os.h>

#define API_HAL_TIMEBASE_CLK_FREQUENCY 32768
#define API_HAL_TIMEBASE_TICK_PER_SECOND 1024
#define API_HAL_TIMEBASE_CLK_PER_TICK (API_HAL_TIMEBASE_CLK_FREQUENCY / API_HAL_TIMEBASE_TICK_PER_SECOND)

typedef struct {
    uint16_t insomnia;
    uint32_t slept;
    uint32_t error;
} ApiHatTimbase;

ApiHatTimbase api_hal_timebase = {
    .insomnia = 0,
    .slept = 0,
    .error = 0,
};

void api_hal_timebase_init() {
    api_hal_timebase_timer_init();

    // LL_LPTIM_EnableIT_ARRM(API_HAL_TIMEBASE_TIMER);
    LL_LPTIM_EnableIT_CMPM(API_HAL_TIMEBASE_TIMER);
    LL_LPTIM_Enable(API_HAL_TIMEBASE_TIMER);

    LL_LPTIM_EnableResetAfterRead(API_HAL_TIMEBASE_TIMER);
    LL_LPTIM_SetAutoReload(API_HAL_TIMEBASE_TIMER, 0xFFFF);
    LL_LPTIM_SetCompare(API_HAL_TIMEBASE_TIMER, API_HAL_TIMEBASE_CLK_PER_TICK);
    LL_LPTIM_StartCounter(API_HAL_TIMEBASE_TIMER, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
}

uint16_t api_hal_timebase_insomnia_level() {
    return api_hal_timebase.insomnia;
}

void api_hal_timebase_insomnia_enter() {
    api_hal_timebase.insomnia++;
}

void api_hal_timebase_insomnia_exit() {
    api_hal_timebase.insomnia--;
}

void LPTIM2_IRQHandler(void) {
    // Compare event
    if(LL_LPTIM_IsActiveFlag_CMPM(API_HAL_TIMEBASE_TIMER)) {
        LL_LPTIM_ClearFLAG_CMPM(API_HAL_TIMEBASE_TIMER);
        LL_LPTIM_SetCompare(API_HAL_TIMEBASE_TIMER, (uint16_t)LL_LPTIM_GetCompare(API_HAL_TIMEBASE_TIMER) + API_HAL_TIMEBASE_CLK_PER_TICK);
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
            xPortSysTickHandler();
        }
    }
    // Autoreload
    if(LL_LPTIM_IsActiveFlag_ARRM(API_HAL_TIMEBASE_TIMER)) {
        LL_LPTIM_ClearFLAG_ARRM(API_HAL_TIMEBASE_TIMER);
        // Not ok
    } 
}

void vPortSuppressTicksAndSleep( TickType_t xExpectedIdleTime )
{
    if (api_hal_timebase.insomnia) {
        return;
    }

    __disable_irq();

    uint32_t cnt = LL_LPTIM_GetCounter(API_HAL_TIMEBASE_TIMER);
    LL_LPTIM_SetCompare(API_HAL_TIMEBASE_TIMER, (xExpectedIdleTime * API_HAL_TIMEBASE_CLK_PER_TICK));

    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);

    uint32_t counter = (LL_LPTIM_GetCounter(API_HAL_TIMEBASE_TIMER) + 3)/ API_HAL_TIMEBASE_CLK_PER_TICK;
    api_hal_timebase.slept += counter;
    api_hal_timebase.error += counter - xExpectedIdleTime;
    vTaskStepTick(counter > xExpectedIdleTime ? xExpectedIdleTime : counter);

    LL_LPTIM_SetCompare(API_HAL_TIMEBASE_TIMER, API_HAL_TIMEBASE_CLK_PER_TICK);

    __enable_irq();
}
