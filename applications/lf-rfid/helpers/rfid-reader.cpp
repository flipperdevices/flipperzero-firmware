#include "rfid-reader.h"
#include <furi.h>
#include <api-hal.h>
#include <stm32wbxx_ll_cortex.h>
#include <tim.h>

extern COMP_HandleTypeDef hcomp1;

/**
 * @brief private violation assistant for RfidReader
 */
struct RfidReaderAccessor {
    static void decode(RfidReader& rfid_reader, bool polarity) {
        rfid_reader.decode(polarity);
    }
};

void RfidReader::decode(bool polarity) {
    uint32_t current_dwt_value = DWT->CYCCNT;

    decoder_em.process_front(polarity, current_dwt_value - last_dwt_value);
    decoder_hid26.process_front(polarity, current_dwt_value - last_dwt_value);
    //decoder_indala.process_front(polarity, current_dwt_value - last_dwt_value);
    //decoder_analyzer.process_front(polarity, current_dwt_value - last_dwt_value);

    last_dwt_value = current_dwt_value;
}

static void comparator_trigger_callback(void* hcomp, void* comp_ctx) {
    COMP_HandleTypeDef* _hcomp = static_cast<COMP_HandleTypeDef*>(hcomp);
    RfidReader* _this = static_cast<RfidReader*>(comp_ctx);

    if(hcomp == &hcomp1) {
        RfidReaderAccessor::decode(
            *_this, (HAL_COMP_GetOutputLevel(_hcomp) == COMP_OUTPUT_LEVEL_HIGH));
    }
}

RfidReader::RfidReader() {
}

void RfidReader::start() {
    start_gpio();
    start_timer();
    start_comparator();
}

void RfidReader::stop() {
    stop_gpio();
    stop_timer();
    stop_comparator();
}

bool RfidReader::read(LfrfidKeyType* type, uint8_t* data, uint8_t data_size) {
    bool result = false;

    if(decoder_em.read(data, data_size)) {
        *type = LfrfidKeyType::KeyEmarine;
        result = true;
    }

    if(decoder_hid26.read(data, data_size)) {
        *type = LfrfidKeyType::KeyHID;
        result = true;
    }

    //decoder_indala.read(NULL, 0);
    //decoder_analyzer.read(NULL, 0);

    return result;
}

void RfidReader::start_comparator(void) {
    api_interrupt_add(comparator_trigger_callback, InterruptTypeComparatorTrigger, this);
    last_dwt_value = DWT->CYCCNT;

    hcomp1.Init.InputMinus = COMP_INPUT_MINUS_VREFINT;
    hcomp1.Init.InputPlus = COMP_INPUT_PLUS_IO1;
    hcomp1.Init.OutputPol = COMP_OUTPUTPOL_NONINVERTED;
    hcomp1.Init.Hysteresis = COMP_HYSTERESIS_LOW;
    hcomp1.Init.BlankingSrce = COMP_BLANKINGSRC_NONE;
    hcomp1.Init.Mode = COMP_POWERMODE_MEDIUMSPEED;
    hcomp1.Init.WindowMode = COMP_WINDOWMODE_DISABLE;
    hcomp1.Init.TriggerMode = COMP_TRIGGERMODE_IT_RISING_FALLING;
    if(HAL_COMP_Init(&hcomp1) != HAL_OK) {
        Error_Handler();
    }

    HAL_COMP_Start(&hcomp1);
}

void RfidReader::start_timer(void) {
    // currently we dont use subcarrier timer and gpio
    /*TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};
    TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
    TIM_SlaveConfigTypeDef sSlaveConfig = {0};

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_ENABLE;
    sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 65535;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.RepetitionCounter = 0;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if(HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    if(HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_TIM_OC_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sSlaveConfig.SlaveMode = TIM_SLAVEMODE_GATED;
    sSlaveConfig.InputTrigger = TIM_TS_ITR0;
    if(HAL_TIM_SlaveConfigSynchro(&htim2, &sSlaveConfig) != HAL_OK) {
        Error_Handler();
    }

    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }

    sConfigOC.OCMode = TIM_OCMODE_TIMING;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
    sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
    if(HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    if(HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_3) != HAL_OK) {
        Error_Handler();
    }
    sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
    sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
    sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
    sBreakDeadTimeConfig.DeadTime = 0;
    sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
    sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
    sBreakDeadTimeConfig.BreakFilter = 0;
    sBreakDeadTimeConfig.BreakAFMode = TIM_BREAK_AFMODE_INPUT;
    sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
    sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
    sBreakDeadTimeConfig.Break2Filter = 0;
    sBreakDeadTimeConfig.Break2AFMode = TIM_BREAK_AFMODE_INPUT;
    sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
    if(HAL_TIMEx_ConfigBreakDeadTime(&htim2, &sBreakDeadTimeConfig) != HAL_OK) {
        Error_Handler();
    }

    hal_pwm_set(0.5, 125000 / 2, &IRDA_RX_TIM, IRDA_RX_FALLING_CH);
    HAL_TIM_PWM_Stop(&IRDA_RX_TIM, IRDA_RX_FALLING_CH);
    IRDA_RX_TIM.Instance->ARR = 1023;
    */

    hal_pwmn_set(0.5, 125000, &LFRFID_TIM, LFRFID_CH);
    //LFRFID_TIM.Instance->ARR = 511;
}

void RfidReader::start_gpio(void) {
    // TODO open record
    GpioPin pull_pin = {.port = RFID_PULL_GPIO_Port, .pin = RFID_PULL_Pin};
    GpioPin* pull_pin_record = &pull_pin;
    gpio_init(pull_pin_record, GpioModeOutputPushPull);

    // debug gpio's
    //gpio_init(&ext_pa7_gpio, GpioModeOutputPushPull);
    //gpio_init(&ext_pa6_gpio, GpioModeOutputPushPull);

    // pulldown ibutton_gpio to prevent interference from ibutton
    gpio_init(&ibutton_gpio, GpioModeOutputOpenDrain);
    gpio_write(&ibutton_gpio, false);

    /*
    // currently we dont use subcarrier timer and gpio
    // TODO move to HAL
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = IR_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(IR_RX_GPIO_Port, &GPIO_InitStruct);
    */
}

void RfidReader::stop_comparator(void) {
    HAL_COMP_Stop(&hcomp1);
    api_interrupt_remove(comparator_trigger_callback, InterruptTypeComparatorTrigger);
}

void RfidReader::stop_timer(void) {
    // reset IRDA timer
    hal_pwm_stop(&IRDA_RX_TIM, IRDA_RX_FALLING_CH);
    MX_TIM2_Init();

    hal_pwmn_stop(&LFRFID_TIM, LFRFID_CH);
}

void RfidReader::stop_gpio(void) {
    // TODO open record
    GpioPin pull_pin = {.port = RFID_PULL_GPIO_Port, .pin = RFID_PULL_Pin};
    GpioPin* pull_pin_record = &pull_pin;
    gpio_init(pull_pin_record, GpioModeInterruptRise);
    gpio_init(&ibutton_gpio, GpioModeAnalog);

    /*
    // currently we dont use subcarrier timer and gpio
    // TODO move to HAL
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = IR_RX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(IR_RX_GPIO_Port, &GPIO_InitStruct);
    */
}