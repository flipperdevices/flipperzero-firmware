#include <furi_hal_rfid.h>
#include <furi_hal_ibutton.h>
#include <furi_hal_resources.h>

#include <stm32wbxx_ll_tim.h>

#define LFRFID_READ_TIM htim1
#define LFRFID_READ_CHANNEL TIM_CHANNEL_1
#define LFRFID_EMULATE_TIM htim2
#define LFRFID_EMULATE_CHANNEL TIM_CHANNEL_3

#define LFRFID_LL_READ_TIM TIM1
#define LFRFID_LL_READ_CONFIG_CHANNEL LL_TIM_CHANNEL_CH1
#define LFRFID_LL_READ_CHANNEL LL_TIM_CHANNEL_CH1N

#define LFRFID_LL_EMULATE_TIM TIM2
#define LFRFID_LL_EMULATE_CHANNEL LL_TIM_CHANNEL_CH3

void furi_hal_rfid_init() {
    furi_hal_rfid_pins_reset();
}

void furi_hal_rfid_pins_reset() {
    // ibutton bus disable
    furi_hal_ibutton_stop();

    // pulldown rfid antenna
    hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_rfid_carrier_out, false);

    // from both sides
    hal_gpio_init(&gpio_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_rfid_pull, true);
}

void furi_hal_rfid_pins_emulate() {
    // ibutton low
    furi_hal_ibutton_start_drive();
    furi_hal_ibutton_pin_low();

    // pull pin to timer out
    hal_gpio_init_ex(
        &gpio_rfid_pull, GpioModeAltFunctionPushPull, GpioPullNo, GpioSpeedLow, GpioAltFn1TIM2);

    // pull rfid antenna from carrier side
    hal_gpio_init(&gpio_rfid_carrier_out, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_rfid_carrier_out, false);
}

void furi_hal_rfid_pins_read() {
    // ibutton low
    furi_hal_ibutton_start_drive();
    furi_hal_ibutton_pin_low();

    // dont pull rfid antenna
    hal_gpio_init(&gpio_rfid_pull, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    hal_gpio_write(&gpio_rfid_pull, false);

    // carrier pin to timer out
    hal_gpio_init_ex(
        &gpio_rfid_carrier_out,
        GpioModeAltFunctionPushPull,
        GpioPullNo,
        GpioSpeedLow,
        GpioAltFn1TIM1);

    // comparator in
    hal_gpio_init(&gpio_rfid_data_in, GpioModeAnalog, GpioPullNo, GpioSpeedLow);
}

void furi_hal_rfid_pin_pull_release() {
    hal_gpio_write(&gpio_rfid_pull, true);
}

void furi_hal_rfid_pin_pull_pulldown() {
    hal_gpio_write(&gpio_rfid_pull, false);
}

void furi_hal_rfid_tim_read(float freq, float duty_cycle) {
    uint32_t period = (uint32_t)((SystemCoreClock) / freq) - 1;

    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

    LL_TIM_InitTypeDef TIM_InitStruct = {0};
    TIM_InitStruct.Prescaler = 0;
    TIM_InitStruct.CounterMode = LL_TIM_COUNTERMODE_UP;
    TIM_InitStruct.Autoreload = period;
    TIM_InitStruct.ClockDivision = LL_TIM_CLOCKDIVISION_DIV1;
    TIM_InitStruct.RepetitionCounter = 0;
    LL_TIM_Init(LFRFID_LL_READ_TIM, &TIM_InitStruct);

    LL_TIM_SetClockSource(LFRFID_LL_READ_TIM, LL_TIM_CLOCKSOURCE_INTERNAL);
    LL_TIM_DisableARRPreload(LFRFID_LL_READ_TIM);
    LL_TIM_OC_EnablePreload(TIM1, LL_TIM_CHANNEL_CH1);

    LL_TIM_OC_InitTypeDef TIM_OC_InitStruct = {0};
    TIM_OC_InitStruct.OCMode = LL_TIM_OCMODE_PWM1;
    TIM_OC_InitStruct.OCState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.OCNState = LL_TIM_OCSTATE_DISABLE;
    TIM_OC_InitStruct.CompareValue = (uint32_t)(period * duty_cycle);
    TIM_OC_InitStruct.OCPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCNPolarity = LL_TIM_OCPOLARITY_HIGH;
    TIM_OC_InitStruct.OCIdleState = LL_TIM_OCIDLESTATE_LOW;
    TIM_OC_InitStruct.OCNIdleState = LL_TIM_OCIDLESTATE_LOW;
    LL_TIM_OC_Init(LFRFID_LL_READ_TIM, LL_TIM_CHANNEL_CH1, &TIM_OC_InitStruct);

    LL_TIM_OC_DisableFast(LFRFID_LL_READ_TIM, LL_TIM_CHANNEL_CH1);
    LL_TIM_SetTriggerOutput(LFRFID_LL_READ_TIM, LL_TIM_TRGO_RESET);
    LL_TIM_SetTriggerOutput2(LFRFID_LL_READ_TIM, LL_TIM_TRGO2_RESET);
    LL_TIM_DisableMasterSlaveMode(LFRFID_LL_READ_TIM);

    LL_TIM_BDTR_InitTypeDef TIM_BDTRInitStruct = {0};
    TIM_BDTRInitStruct.OSSRState = LL_TIM_OSSR_DISABLE;
    TIM_BDTRInitStruct.OSSIState = LL_TIM_OSSI_DISABLE;
    TIM_BDTRInitStruct.LockLevel = LL_TIM_LOCKLEVEL_OFF;
    TIM_BDTRInitStruct.DeadTime = 0;
    TIM_BDTRInitStruct.BreakState = LL_TIM_BREAK_DISABLE;
    TIM_BDTRInitStruct.BreakPolarity = LL_TIM_BREAK_POLARITY_HIGH;
    TIM_BDTRInitStruct.BreakFilter = LL_TIM_BREAK_FILTER_FDIV1;
    TIM_BDTRInitStruct.BreakAFMode = LL_TIM_BREAK_AFMODE_INPUT;
    TIM_BDTRInitStruct.Break2State = LL_TIM_BREAK2_DISABLE;
    TIM_BDTRInitStruct.Break2Polarity = LL_TIM_BREAK2_POLARITY_HIGH;
    TIM_BDTRInitStruct.Break2Filter = LL_TIM_BREAK2_FILTER_FDIV1;
    TIM_BDTRInitStruct.Break2AFMode = LL_TIM_BREAK_AFMODE_INPUT;
    TIM_BDTRInitStruct.AutomaticOutput = LL_TIM_AUTOMATICOUTPUT_DISABLE;
    LL_TIM_BDTR_Init(LFRFID_LL_READ_TIM, &TIM_BDTRInitStruct);
}

void furi_hal_rfid_tim_read_start() {
    LL_TIM_CC_EnableChannel(LFRFID_LL_READ_TIM, LFRFID_LL_READ_CHANNEL);
    LL_TIM_EnableAllOutputs(LFRFID_LL_READ_TIM);
    LL_TIM_EnableCounter(LFRFID_LL_READ_TIM);
}

void furi_hal_rfid_tim_read_stop() {
    LL_TIM_DisableCounter(LFRFID_LL_READ_TIM);
    LL_TIM_DisableAllOutputs(LFRFID_LL_READ_TIM);
    LL_TIM_CC_DisableChannel(LFRFID_LL_READ_TIM, LFRFID_LL_READ_CHANNEL);
}

void furi_hal_rfid_tim_emulate(float freq) {
    // TODO LL init
    uint32_t prescaler = (uint32_t)((SystemCoreClock) / freq) - 1;

    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    // basic PWM setup with needed freq and internal clock
    LFRFID_EMULATE_TIM.Init.Prescaler = prescaler;
    LFRFID_EMULATE_TIM.Init.CounterMode = TIM_COUNTERMODE_UP;
    LFRFID_EMULATE_TIM.Init.Period = 1;
    LFRFID_EMULATE_TIM.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    LFRFID_EMULATE_TIM.Init.RepetitionCounter = 0;
    LFRFID_EMULATE_TIM.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if(HAL_TIM_Base_Init(&LFRFID_EMULATE_TIM) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if(HAL_TIM_ConfigClockSource(&LFRFID_EMULATE_TIM, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_TIM_PWM_Init(&LFRFID_EMULATE_TIM) != HAL_OK) {
        Error_Handler();
    }

    // no master-slave mode
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&LFRFID_EMULATE_TIM, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    // pwm config
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if(HAL_TIM_PWM_ConfigChannel(&LFRFID_EMULATE_TIM, &sConfigOC, LFRFID_EMULATE_CHANNEL) !=
       HAL_OK) {
        Error_Handler();
    }
}

void furi_hal_rfid_tim_emulate_start() {
    // TODO make api for interrupts priority
    /*for(size_t i = WWDG_IRQn; i <= DMAMUX1_OVR_IRQn; i++) {
        HAL_NVIC_SetPriority(i, 15, 0);
    }

    HAL_NVIC_SetPriority(TIM2_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);*/

    HAL_TIM_PWM_Start_IT(&LFRFID_EMULATE_TIM, LFRFID_EMULATE_CHANNEL);
    HAL_TIM_Base_Start_IT(&LFRFID_EMULATE_TIM);
}

void furi_hal_rfid_tim_emulate_stop() {
    HAL_TIM_Base_Stop(&LFRFID_EMULATE_TIM);
    HAL_TIM_PWM_Stop(&LFRFID_EMULATE_TIM, LFRFID_EMULATE_CHANNEL);
}

void furi_hal_rfid_tim_reset() {
    // HAL_TIM_Base_DeInit(&LFRFID_READ_TIM);
    LL_TIM_DeInit(TIM1);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_TIM1);

    // HAL_TIM_Base_DeInit(&LFRFID_EMULATE_TIM);
    LL_TIM_DeInit(TIM2);
    LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_TIM2);
}

bool furi_hal_rfid_is_tim_emulate(TIM_HandleTypeDef* hw) {
    return (hw == &LFRFID_EMULATE_TIM);
}

void furi_hal_rfid_set_emulate_period(uint32_t period) {
    LFRFID_EMULATE_TIM.Instance->ARR = period;
}

void furi_hal_rfid_set_emulate_pulse(uint32_t pulse) {
    switch(LFRFID_EMULATE_CHANNEL) {
    case TIM_CHANNEL_1:
        LFRFID_EMULATE_TIM.Instance->CCR1 = pulse;
        break;
    case TIM_CHANNEL_2:
        LFRFID_EMULATE_TIM.Instance->CCR2 = pulse;
        break;
    case TIM_CHANNEL_3:
        LFRFID_EMULATE_TIM.Instance->CCR3 = pulse;
        break;
    case TIM_CHANNEL_4:
        LFRFID_EMULATE_TIM.Instance->CCR4 = pulse;
        break;
    default:
        furi_crash(NULL);
        break;
    }
}

void furi_hal_rfid_set_read_period(uint32_t period) {
    LL_TIM_SetAutoReload(LFRFID_LL_READ_TIM, period);
}

void furi_hal_rfid_set_read_pulse(uint32_t pulse) {
    switch(LFRFID_LL_READ_CONFIG_CHANNEL) {
    case LL_TIM_CHANNEL_CH1:
        LL_TIM_OC_SetCompareCH1(LFRFID_LL_READ_TIM, pulse);
        break;
    case LL_TIM_CHANNEL_CH2:
        LL_TIM_OC_SetCompareCH2(LFRFID_LL_READ_TIM, pulse);
        break;
    case LL_TIM_CHANNEL_CH3:
        LL_TIM_OC_SetCompareCH3(LFRFID_LL_READ_TIM, pulse);
        break;
    case LL_TIM_CHANNEL_CH4:
        LL_TIM_OC_SetCompareCH4(LFRFID_LL_READ_TIM, pulse);
        break;
    default:
        furi_crash(NULL);
        break;
    }
}

void furi_hal_rfid_change_read_config(float freq, float duty_cycle) {
    uint32_t period = (uint32_t)((SystemCoreClock) / freq) - 1;
    furi_hal_rfid_set_read_period(period);
    furi_hal_rfid_set_read_pulse(period * duty_cycle);
}
