#include <api-hal-timebase.h>
#include <api-hal-timebase-timer.h>

#include <stm32wbxx_hal.h>
#include <stm32wbxx_ll_gpio.h>
#include <FreeRTOS.h>
#include <cmsis_os.h>

#define API_HAL_TIMEBASE_LED_PORT GPIOA
#define API_HAL_TIMEBASE_LED_PIN LL_GPIO_PIN_2

#define API_HAL_TIMEBASE_CLK_FREQUENCY 32768
#define API_HAL_TIMEBASE_TICK_PER_SECOND 1024
#define API_HAL_TIMEBASE_CLK_PER_TICK (API_HAL_TIMEBASE_CLK_FREQUENCY / API_HAL_TIMEBASE_TICK_PER_SECOND)
#define API_HAL_TIMEBASE_TICK_PER_EPOCH (API_HAL_TIMEBASE_TIMER_MAX / API_HAL_TIMEBASE_CLK_PER_TICK)
#define API_HAL_TIMEBASE_MAX_SLEEP (API_HAL_TIMEBASE_TICK_PER_EPOCH - 1)

typedef struct {
    // Sleep control
    volatile uint16_t insomnia;
    // Counters
    volatile uint32_t in_sleep;
    volatile uint32_t in_awake;
    volatile uint32_t sleep_error;
    volatile uint32_t awake_error;
} ApiHalTimbase;

ApiHalTimbase api_hal_timebase = {
    .insomnia = 0,

    .in_sleep = 0,
    .in_awake = 0,
    .sleep_error = 0,
    .awake_error = 0,
};

void api_hal_timebase_init() {
    api_hal_timebase_timer_init();
    LL_DBGMCU_APB1_GRP2_FreezePeriph(LL_DBGMCU_APB1_GRP2_LPTIM2_STOP);

    LL_LPTIM_EnableIT_CMPM(API_HAL_TIMEBASE_TIMER);
    LL_LPTIM_EnableIT_ARRM(API_HAL_TIMEBASE_TIMER);

    LL_LPTIM_SetAutoReload(API_HAL_TIMEBASE_TIMER, API_HAL_TIMEBASE_TIMER_MAX);
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
    // Autoreload
    const bool arrm_flag = LL_LPTIM_IsActiveFlag_ARRM(API_HAL_TIMEBASE_TIMER);
    if(arrm_flag) {
        LL_LPTIM_ClearFLAG_ARRM(API_HAL_TIMEBASE_TIMER);
    }
    if(LL_LPTIM_IsActiveFlag_CMPM(API_HAL_TIMEBASE_TIMER)) {
        LL_LPTIM_ClearFLAG_CMPM(API_HAL_TIMEBASE_TIMER);
        // Calculate and set next stop
        uint16_t cnt = api_hal_timebase_timer_get_cnt();
        uint16_t cmp = api_hal_timebase_timer_get_cmp();
        uint16_t current_tick = cnt / API_HAL_TIMEBASE_CLK_PER_TICK;
        uint16_t compare_tick = cmp / API_HAL_TIMEBASE_CLK_PER_TICK;
        int32_t error = arrm_flag ? (int32_t)current_tick - compare_tick : (int32_t)compare_tick - current_tick;
        assert(error >= 0);
        api_hal_timebase.awake_error += error;
        
        uint16_t next_tick = current_tick + 1;
        api_hal_timebase_timer_set_cmp(next_tick * API_HAL_TIMEBASE_CLK_PER_TICK);

        // Propogate tick
        api_hal_timebase.in_awake ++;
        if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
            xPortSysTickHandler();
        }
    }
}


void vPortSuppressTicksAndSleep(TickType_t expected_idle_ticks) {
    if (expected_idle_ticks > API_HAL_TIMEBASE_MAX_SLEEP) {
        expected_idle_ticks = API_HAL_TIMEBASE_MAX_SLEEP;
    }

    if (api_hal_timebase.insomnia) {
        return;
    }

    __disable_irq();

    if (eTaskConfirmSleepModeStatus() == eAbortSleep
        || !api_hal_timebase_timer_is_safe()) {
        __enable_irq();
        return;
    }

    const uint16_t before_cnt = api_hal_timebase_timer_get_cnt();
    const uint16_t before_tick = before_cnt / API_HAL_TIMEBASE_CLK_PER_TICK;
    
    const uint16_t expected_cnt = (before_tick + expected_idle_ticks - 2) * API_HAL_TIMEBASE_CLK_PER_TICK;
    api_hal_timebase_timer_set_cmp(expected_cnt);

    // Sleep
    LL_GPIO_SetOutputPin(API_HAL_TIMEBASE_LED_PORT, API_HAL_TIMEBASE_LED_PIN);
    HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON, PWR_STOPENTRY_WFI);
    LL_GPIO_ResetOutputPin(API_HAL_TIMEBASE_LED_PORT, API_HAL_TIMEBASE_LED_PIN);

    while(!api_hal_timebase_timer_is_safe()) {}

    const uint16_t after_cnt = api_hal_timebase_timer_get_cnt();
    const uint16_t after_tick = after_cnt / API_HAL_TIMEBASE_CLK_PER_TICK;
    
    bool cmpm_flag = LL_LPTIM_IsActiveFlag_CMPM(API_HAL_TIMEBASE_TIMER);
    if (cmpm_flag) LL_LPTIM_ClearFLAG_CMPM(API_HAL_TIMEBASE_TIMER);
    bool arrm_flag = LL_LPTIM_IsActiveFlag_ARRM(API_HAL_TIMEBASE_TIMER);
    if (arrm_flag) LL_LPTIM_ClearFLAG_ARRM(API_HAL_TIMEBASE_TIMER);
    
    const uint16_t next_cmp = (after_tick + 1) * API_HAL_TIMEBASE_CLK_PER_TICK;
    api_hal_timebase_timer_set_cmp(next_cmp);

    __enable_irq();

    int32_t completed_ticks = arrm_flag ? (int32_t)before_tick - after_tick : (int32_t)after_tick - before_tick;

    assert(completed_ticks >= 0);
    if (completed_ticks > 0) {
        api_hal_timebase.in_sleep += completed_ticks;
        if (completed_ticks > expected_idle_ticks) {
            // We are late, count error
            api_hal_timebase.sleep_error += (completed_ticks - expected_idle_ticks);
             // Step expected_idle_ticks because freertos asserts this value
            vTaskStepTick(expected_idle_ticks);
        } else {
            vTaskStepTick(completed_ticks);
        }
    }
}
