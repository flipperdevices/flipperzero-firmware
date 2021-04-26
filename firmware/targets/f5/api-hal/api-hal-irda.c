#include "cmsis_os.h"
#include "api-hal-tim_i.h"
#include "api-hal-irda.h"
#include <stm32wbxx_ll_tim.h>
#include <stm32wbxx_ll_gpio.h>
#include <stdio.h>
#include <furi.h>
#include "main.h"


static struct{
    TimerISRCallback callback;
    void *ctx;
} timerIrda;


void api_hal_irda_tim_isr(TimerIRQSource source)
{
    uint32_t duration;
    uint8_t level;

    switch (source) {
    case TimerIRQSourceCCI1:
        duration = LL_TIM_OC_GetCompareCH1(TIM2);
        level = 1;
        break;
    case TimerIRQSourceCCI2:
        duration = LL_TIM_OC_GetCompareCH2(TIM2);
        level = 0;
        break;
    default:
        furi_check(0);
    }

    if (timerIrda.callback)
        timerIrda.callback(timerIrda.ctx, duration, level);

    LL_TIM_SetCounter(TIM2, 0);
}

void api_hal_irda_rx_irq_init(void)
{
    LL_TIM_InitTypeDef TIM_InitStruct = {0};

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Peripheral clock enable */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM2);

    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    /**TIM2 GPIO Configuration
    PA0   ------> TIM2_CH1
    */
    GPIO_InitStruct.Pin = LL_GPIO_PIN_0;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    GPIO_InitStruct.Alternate = LL_GPIO_AF_1;
    LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    TIM_InitStruct.Prescaler = 64 - 1;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = 4294967295;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    LL_TIM_Init(TIM2, &TIM_InitStruct);
    LL_TIM_SetClockSource(TIM2, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_EnableARRPreload(TIM2);
    LL_TIM_SetTriggerOutput(TIM2, LL_TIM_TRGO_RESET);
    LL_TIM_DisableMasterSlaveMode(TIM2);
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ACTIVEINPUT_DIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH1, LL_TIM_IC_POLARITY_FALLING);
    LL_TIM_IC_SetActiveInput(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ACTIVEINPUT_INDIRECTTI);
    LL_TIM_IC_SetPrescaler(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_ICPSC_DIV1);
    LL_TIM_IC_SetFilter(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_FILTER_FDIV1);
    LL_TIM_IC_SetPolarity(TIM2, LL_TIM_CHANNEL_CH2, LL_TIM_IC_POLARITY_RISING);

    LL_TIM_EnableIT_CC1(TIM2);
    LL_TIM_EnableIT_CC2(TIM2);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_EnableChannel(TIM2, LL_TIM_CHANNEL_CH2);

    LL_TIM_SetCounter(TIM2, 0);
    LL_TIM_EnableCounter(TIM2);

    NVIC_SetPriority(TIM2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),5, 0));
    NVIC_EnableIRQ(TIM2_IRQn);
}

void api_hal_irda_rx_irq_deinit(void) {
    LL_TIM_DisableIT_CC1(TIM2);
    LL_TIM_DisableIT_CC2(TIM2);
    LL_TIM_CC_DisableChannel(TIM2, LL_TIM_CHANNEL_CH1);
    LL_TIM_CC_DisableChannel(TIM2, LL_TIM_CHANNEL_CH2);
}

void api_hal_irda_rx_irq_set_callback(TimerISRCallback callback, void *ctx) {
    furi_check(callback);

    timerIrda.callback = callback;
    timerIrda.ctx = ctx;
}

void api_hal_irda_pwm_start(float value, float freq) {
    api_hal_pwm_start(value, freq, &IRDA_TX_TIM, IRDA_TX_CH);
}

void api_hal_irda_pwm_stop() {
    api_hal_pwm_stop(&IRDA_TX_TIM, IRDA_TX_CH);
}




///////////////////////

uint32_t cnt = 0;
struct {
    uint32_t last;
    uint8_t edge;
} m[500];

void api_hal_irda_tim_isr_DBG(TimerIRQSource source)
{
    switch (source) {
    case TimerIRQSourceCCI1:
        m[cnt].last = LL_TIM_OC_GetCompareCH1(TIM2);
        m[cnt].edge = 1;
        ++cnt;
        break;
    case TimerIRQSourceCCI2:
        m[cnt].last = LL_TIM_OC_GetCompareCH2(TIM2);
        m[cnt].edge = 0;
        ++cnt;
        break;
    }

    LL_TIM_SetCounter(TIM2, 0);
}

void print_m(void) {
    if (cnt == 0) {
        printf(".");
        return;
    }

    printf("\r\n================ %lu ============\r\n", cnt);
    for (int i = 0; i < cnt; i++) {
        printf("%8lu  -  %s\r\n", m[i].last, m[i].edge ? "rising" : "falling" );
    }
    cnt = 0;
}

