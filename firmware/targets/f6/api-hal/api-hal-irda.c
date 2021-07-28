#include "api-hal-irda.h"
#include "furi/common_defines.h"
#include "stm32wbxx_ll_dma.h"
#include <cmsis_os2.h>
#include <api-hal-interrupt.h>
#include <api-hal-resources.h>

#include <stdint.h>
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_gpio.h>

#include <stdio.h>
#include <furi.h>
#include <math.h>
#include <main.h>
#include <api-hal-pwm.h>

static struct{
    ApiHalIrdaCaptureCallback capture_callback;
    void *capture_context;
    ApiHalIrdaTimeoutCallback timeout_callback;
    void *timeout_context;
} timer_irda;

typedef enum{
    TimerIRQSourceCCI1,
    TimerIRQSourceCCI2,
} TimerIRQSource;

static void api_hal_irda_handle_timeout(void) {
    /* Timers CNT register starts to counting from 0 to ARR, but it is
     * reseted when Channel 1 catches interrupt. It is not reseted by
     * channel 2, though, so we have to distract it's values (see TimerIRQSourceCCI1 ISR).
     * This can cause false timeout: when time is over, but we started
     * receiving new signal few microseconds ago, because CNT register
     * is reseted once per period, not per sample. */
    if (LL_GPIO_IsInputPinSet(gpio_irda_rx.port, gpio_irda_rx.pin) == 0)
        return;

    if (timer_irda.timeout_callback)
        timer_irda.timeout_callback(timer_irda.timeout_context);
}

/* High pin level is a Space state of IRDA signal. Invert level for further processing. */
static void api_hal_irda_handle_capture(TimerIRQSource source) {
    uint32_t duration = 0;
    bool level = 0;

    switch (source) {
    case TimerIRQSourceCCI1:
        duration = LL_TIM_IC_GetCaptureCH1(TIM2) - LL_TIM_IC_GetCaptureCH2(TIM2);
        level = 1;
        break;
    case TimerIRQSourceCCI2:
        duration = LL_TIM_IC_GetCaptureCH2(TIM2);
        level = 0;
        break;
    default:
        furi_check(0);
    }

    if (timer_irda.capture_callback)
        timer_irda.capture_callback(timer_irda.capture_context, level, duration);
}

static void api_hal_irda_tim_rx_isr() {
    if(LL_TIM_IsActiveFlag_CC3(TIM2)) {
        LL_TIM_ClearFlag_CC3(TIM2);
        api_hal_irda_handle_timeout();
    }
    if(LL_TIM_IsActiveFlag_CC1(TIM2)) {
        LL_TIM_ClearFlag_CC1(TIM2);

        if(READ_BIT(TIM2->CCMR1, TIM_CCMR1_CC1S)) {
            // input capture
            api_hal_irda_handle_capture(TimerIRQSourceCCI1);
        }
    }
    if(LL_TIM_IsActiveFlag_CC2(TIM2)) {
        LL_TIM_ClearFlag_CC2(TIM2);

        if(READ_BIT(TIM2->CCMR1, TIM_CCMR1_CC2S)) {
            // input capture
            api_hal_irda_handle_capture(TimerIRQSourceCCI2);
        }
    }
}

void api_hal_irda_rx_irq_init(void) {
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);

    hal_gpio_init_ex(&gpio_irda_rx, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM2);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 0x7FFFFFFE;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);

    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(TIM2);
    LL_TIM_SetTriggerInput(TIM2, LL_TIM_TS_TI1FP1);
    LL_TIM_SetSlaveMode(TIM2, LL_TIM_SLAVEMODE_RESET);
    LL_TIM_CC_DisableChannel(TIM2, LL_TIM_CHANNEL_CH2);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_DisableIT_TRIG(TIM2);
    LL_TIM_DisableDMAReq_TRIG(TIM2);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_EnableMasterSlaveMode(TIM2);
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_RISING);
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_INDIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);

    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_EnableIT_CC2(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);

    api_hal_interrupt_set_timer_isr(TIM2, api_hal_irda_tim_rx_isr);

    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
    NVIC_EnableIRQ(TIM2_IRQn);
}

void api_hal_irda_rx_irq_deinit(void) {
    LL_TIM_DeInit(TIM2);
    api_hal_interrupt_set_timer_isr(TIM2, NULL);
}

void api_hal_irda_rx_timeout_irq_init(uint32_t timeout_ms) {
    LL_TIM_OC_SetCompareCH3(TIM2, timeout_ms * 1000);
    LL_TIM_OC_SetMode(TIM2, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_ACTIVE);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH3);
    LL_TIM_EnableIT_CC3(TIM2);
}

bool api_hal_irda_rx_irq_is_busy(void) {
    return (LL_TIM_IsEnabledIT_CC1(TIM2) || LL_TIM_IsEnabledIT_CC2(TIM2));
}

void api_hal_irda_rx_irq_set_callback(ApiHalIrdaCaptureCallback callback, void *ctx) {
    timer_irda.capture_callback = callback;
    timer_irda.capture_context = ctx;
}

void api_hal_irda_rx_timeout_irq_set_callback(ApiHalIrdaTimeoutCallback callback, void *ctx) {
    timer_irda.timeout_callback = callback;
    timer_irda.timeout_context = ctx;
}

void api_hal_irda_pwm_set(float value, float freq) {
    hal_pwmn_set(value, freq, &IRDA_TX_TIM, IRDA_TX_CH);
}

void api_hal_irda_pwm_stop() {
    hal_pwmn_stop(&IRDA_TX_TIM, IRDA_TX_CH);
}

///////////////////////////////////////////////////////////////////////////////

static uint32_t irda_update_events_till_end = 0;

void api_hal_irda_tx_dma_isr() {
    if (LL_DMA_IsActiveFlag_TC2(DMA1)) {
        LL_DMA_ClearFlag_TC2(DMA1);
        LL_DMA_DisableIT_TC(DMA1, LL_DMA_CHANNEL_2);
        irda_update_events_till_end = 3;
        LL_TIM_EnableIT_UPDATE(TIM1);
    }
}

static void api_hal_irda_tim_tx_isr(void) {
    if(LL_TIM_IsActiveFlag_UPDATE(TIM1)) {
        LL_TIM_ClearFlag_UPDATE(TIM1);
        if (--irda_update_events_till_end == 0) {
            LL_TIM_DisableIT_UPDATE(TIM1);
            LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
            LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_1);
            LL_TIM_DisableCounter(TIM1);
            hal_gpio_init_ex(&gpio_irda_tx, GpioModeOutputOpenDrain, GpioPullDown, GpioSpeedLow, 0);
            api_hal_interrupt_set_timer_isr(TIM1, NULL);
        }
    }
}

static void api_hal_irda_configure_timer_tx(uint32_t freq, float duty_cycle)
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);
//    LL_DBGMCU_APB2_GRP1_FreezePeriph(LL_DBGMCU_APB2_GRP1_TIM1_STOP);

    LL_TIM_SetCounterMode(TIM1, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetPrescaler(TIM1, 0);
    LL_TIM_EnableARRPreload(TIM1);
    LL_TIM_SetAutoReload(TIM1, __LL_TIM_CALC_ARR(SystemCoreClock, LL_TIM_GetPrescaler(TIM1), freq));
    LL_TIM_OC_SetCompareCH3(TIM1, ( (LL_TIM_GetAutoReload(TIM1) + 1 ) * (1 - duty_cycle)));
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_OC_SetMode(TIM1, LL_TIM_CHANNEL_CH3, LL_TIM_OCMODE_FORCED_INACTIVE);     /* LL_TIM_OCMODE_PWM2 set by DMA */
    LL_TIM_OC_SetPolarity(TIM1, LL_TIM_CHANNEL_CH3N, LL_TIM_OCPOLARITY_HIGH);
    LL_TIM_DisableMasterSlaveMode(TIM1);
    LL_TIM_OC_DisableFast(TIM1, LL_TIM_CHANNEL_CH3);
    LL_TIM_CC_EnableChannel(TIM1, LL_TIM_CHANNEL_CH3N);
    LL_TIM_EnableAllOutputs(TIM1);
    LL_TIM_SetCounter(TIM1, 0);
    LL_TIM_DisableIT_CC3(TIM1);
    LL_TIM_DisableIT_UPDATE(TIM1);
    LL_TIM_EnableDMAReq_UPDATE(TIM1);

    api_hal_interrupt_set_timer_isr(TIM1, api_hal_irda_tim_tx_isr);

    NVIC_SetPriority(TIM1_UP_TIM16_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
    NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);
}

static void api_hal_irda_configure_cmgr2_dma_tx(void) {
    /* RCR is preload, so start from Space time */
    static uint8_t ccmr2_reg[] = {
        0x48,   /* Space time - force low */
        0x78,   /* Mark time - enable PWM2 mode */
    };

    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.PeriphOrM2MSrcAddress = (uint32_t)&(TIM1->CCMR2);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)ccmr2_reg;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_CIRCULAR;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_BYTE;
    dma_config.NbData = COUNT_OF(ccmr2_reg);
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM1_UP;
    dma_config.Priority = LL_DMA_PRIORITY_VERYHIGH;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma_config);
}

static void api_hal_irda_configure_rcr_dma_tx(uint32_t* buffer, size_t buffer_size) {
    LL_DMA_InitTypeDef dma_config = {0};
    dma_config.PeriphOrM2MSrcAddress = (uint32_t)&(TIM1->RCR);
    dma_config.MemoryOrM2MDstAddress = (uint32_t)buffer;
    dma_config.Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH;
    dma_config.Mode = LL_DMA_MODE_NORMAL;
    dma_config.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_config.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_config.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD;
    dma_config.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_WORD;
    dma_config.NbData = buffer_size;
    dma_config.PeriphRequest = LL_DMAMUX_REQ_TIM1_UP;
    dma_config.Priority = LL_DMA_PRIORITY_MEDIUM;
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_2, &dma_config);
    api_hal_interrupt_set_dma_channel_isr(DMA1, LL_DMA_CHANNEL_2, api_hal_irda_tx_dma_isr);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_2);

    NVIC_SetPriority(DMA1_Channel2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
    NVIC_EnableIRQ(DMA1_Channel2_IRQn);
}

bool api_hal_irda_start_async_tx(uint32_t* buffer, size_t buffer_size, uint32_t freq, float duty_cycle) {
    if (!buffer || (duty_cycle > 1) || (duty_cycle < 0) || (freq > 40000) || (freq < 32000)) {
        furi_assert(0);
        return false;
    }

    float cycle_time = 1000000.0 / freq;

    for (int i = 0; i < buffer_size; ++i) {
#if 0
        float free_time = (1 - duty_cycle) * cycle_time;
        float num_of_impulses = roundf((buffer[i] + (i%2 ? -free_time : free_time)) / cycle_time);
#else
        float num_of_impulses = roundf(buffer[i] / cycle_time);
#endif
        if ((num_of_impulses <= 0) || (num_of_impulses > 0x10000)) {
            furi_assert(0);     // tmp
            return false;
        }

        buffer[i] = num_of_impulses - 1;
        printf("%s: timing %ld, pulses: %f\r\n", i % 2 ? "space" : "mark", buffer[i], num_of_impulses); // tmp
    }

    LL_TIM_SetRepetitionCounter(TIM1, 0);
    api_hal_irda_configure_cmgr2_dma_tx();
    api_hal_irda_configure_rcr_dma_tx(buffer, buffer_size);
    api_hal_irda_configure_timer_tx(freq, duty_cycle);
    hal_gpio_init_ex(&gpio_irda_tx, GpioModeAltFunctionPushPull, GpioPullUp, GpioSpeedHigh, GpioAltFn1TIM1);

    /* Force update generation */
    __disable_irq();
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
    LL_TIM_GenerateEvent_UPDATE(TIM1);
    LL_TIM_EnableCounter(TIM1);
    __enable_irq();

    return false;
}

void api_hal_irda_stop_async_tx(void) {

}

