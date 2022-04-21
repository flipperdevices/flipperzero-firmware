#include <furi_hal_os.h>
#include <furi_hal_usb.h>
#include <furi_hal_uart.h>
#include <furi_hal_clock.h>
#include <furi_hal_power.h>
#include <furi_hal_gpio.h>
#include <furi_hal_resources.h>
#include <furi_hal_delay.h>
#include <furi_hal_idle_timer.h>

#include <stm32wbxx_ll_cortex.h>

#include <furi.h>

#define TAG "FuriHalOs"

#define FURI_HAL_IDLE_TIMER_CLK_HZ 32768
#define FURI_HAL_OS_TICK_HZ configTICK_RATE_HZ

#define FURI_HAL_OS_IDLE_CNT_TO_TICKS(x) ((x * FURI_HAL_OS_TICK_HZ) / FURI_HAL_IDLE_TIMER_CLK_HZ)
#define FURI_HAL_OS_TICKS_TO_IDLE_CNT(x) ((x * FURI_HAL_IDLE_TIMER_CLK_HZ) / FURI_HAL_OS_TICK_HZ)

#define FURI_HAL_IDLE_TIMER_TICK_PER_EPOCH (FURI_HAL_OS_IDLE_CNT_TO_TICKS(FURI_HAL_IDLE_TIMER_MAX))
#define FURI_HAL_OS_MAX_SLEEP (FURI_HAL_IDLE_TIMER_TICK_PER_EPOCH - 1)

#define FURI_HAL_OS_EXTI_IRQ_MASK_0                                                  \
    ((1 << EXTI0_IRQn) | (1 << EXTI1_IRQn) | (1 << EXTI2_IRQn) | (1 << EXTI3_IRQn) | \
     (1 << EXTI4_IRQn) | (1 << EXTI9_5_IRQn))
#define FURI_HAL_OS_EXTI_IRQ_MASK_1 ((1 << (EXTI15_10_IRQn - 32)))
#define FURI_HAL_OS_IPCC_IRQ_MASK_1 ((1 << (IPCC_C1_TX_IRQn - 32)) | (1 << (IPCC_C1_TX_IRQn - 32)))

#ifdef FURI_HAL_OS_DEBUG
#include <stm32wbxx_ll_gpio.h>

void furi_hal_os_timer_callback() {
    furi_hal_gpio_write(&gpio_ext_pa4, !furi_hal_gpio_read(&gpio_ext_pa4));
}
#endif

extern void xPortSysTickHandler();

static volatile uint32_t furi_hal_os_skew = 0;

void furi_hal_os_init() {
    furi_hal_idle_timer_init();

#ifdef FURI_HAL_OS_DEBUG
    furi_hal_gpio_init_simple(&gpio_ext_pa7, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pa6, GpioModeOutputPushPull);
    furi_hal_gpio_init_simple(&gpio_ext_pa4, GpioModeOutputPushPull);
    osTimerId_t second_timer = osTimerNew(furi_hal_os_timer_callback, osTimerPeriodic, NULL, NULL);
    osTimerStart(second_timer, FURI_HAL_OS_TICK_HZ);
#endif

    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_os_tick() {
    if(xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
#ifdef FURI_HAL_OS_DEBUG
        furi_hal_gpio_write(&gpio_ext_pa6, !furi_hal_gpio_read(&gpio_ext_pa6));
#endif
        xPortSysTickHandler();
    }
}

static inline void furi_hal_os_suspend_aux_periphs() {
    // Disable USART
    furi_hal_uart_suspend(FuriHalUartIdUSART1);
    furi_hal_uart_suspend(FuriHalUartIdLPUART1);
    // Disable USB
}

static inline void furi_hal_os_resume_aux_periphs() {
    // Re-enable USART
    furi_hal_uart_resume(FuriHalUartIdUSART1);
    furi_hal_uart_resume(FuriHalUartIdLPUART1);
    // Re-enable USB
}

static inline bool furi_hal_os_is_bad_interrupt_pending() {
    // Only EXTI and IPCC interrupts are allowed in sleep mode
    return (NVIC->ICPR[0] & ~FURI_HAL_OS_EXTI_IRQ_MASK_0) ||
           (NVIC->ICPR[1] & ~FURI_HAL_OS_EXTI_IRQ_MASK_1) ||
           (NVIC->ICPR[1] & ~FURI_HAL_OS_IPCC_IRQ_MASK_1);
}

static inline uint32_t furi_hal_os_sleep(TickType_t expected_idle_ticks) {
    // Stop ticks
    furi_hal_clock_suspend_tick();

    // Start wakeup timer
    furi_hal_idle_timer_start(FURI_HAL_OS_TICKS_TO_IDLE_CNT(expected_idle_ticks));

#ifdef FURI_HAL_OS_DEBUG
    furi_hal_gpio_write(&gpio_ext_pa7, 0);
#endif

    // Go to sleep mode
    furi_hal_power_sleep();

#ifdef FURI_HAL_OS_DEBUG
    furi_hal_gpio_write(&gpio_ext_pa7, 1);
#endif

    // Calculate how much time we spent in the sleep
    uint32_t after_cnt = furi_hal_idle_timer_get_cnt() + furi_hal_os_skew;
    uint32_t after_tick = FURI_HAL_OS_IDLE_CNT_TO_TICKS(after_cnt);
    furi_hal_os_skew = after_cnt - (after_cnt / after_tick);

    bool cmpm = LL_LPTIM_IsActiveFlag_CMPM(FURI_HAL_IDLE_TIMER);
    bool arrm = LL_LPTIM_IsActiveFlag_ARRM(FURI_HAL_IDLE_TIMER);
    if(cmpm && arrm) after_tick += expected_idle_ticks;

    // Prepare tick timer for new round
    furi_hal_idle_timer_reset();

    // Resume ticks
    furi_hal_clock_resume_tick();
    return after_tick;
}

void vPortSuppressTicksAndSleep(TickType_t expected_idle_ticks) {
    if(!furi_hal_power_sleep_available()) {
        __WFI();
        return;
    }

    // Limit amount of ticks to maximum that timer can count
    if(expected_idle_ticks > FURI_HAL_OS_MAX_SLEEP) {
        expected_idle_ticks = FURI_HAL_OS_MAX_SLEEP;
    }

    furi_hal_os_suspend_aux_periphs();
    // Stop IRQ handling, no one should disturb us till we finish
    __disable_irq();

    // Confirm OS that sleep is still possible
    if(eTaskConfirmSleepModeStatus() == eAbortSleep) {
        furi_hal_os_resume_aux_periphs();
        __enable_irq();
        return;
    }

    // if(furi_hal_os_is_bad_interrupt_pending()) {
    //     furi_crash("Bad interrupt pending before sleep");
    // }

    // Sleep and track how much ticks we spent sleeping
    uint32_t completed_ticks = furi_hal_os_sleep(expected_idle_ticks);
    // Notify system about time spent in sleep
    if(completed_ticks > 0) {
        vTaskStepTick(MIN(completed_ticks, expected_idle_ticks));
    }

    furi_hal_os_resume_aux_periphs();
    // Reenable IRQ
    __enable_irq();
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char* pcTaskName) {
    furi_crash("StackOverflow");
}
