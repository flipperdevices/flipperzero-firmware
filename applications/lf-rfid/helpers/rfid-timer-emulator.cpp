#include "rfid-timer-emulator.h"

extern TIM_HandleTypeDef htim1;

/*
static void init_timer_external_etr(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 7;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if(HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_ETRMODE2;
    sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
    sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
    sClockSourceConfig.ClockFilter = 0;
    if(HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 4;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if(HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}

static void init_timer_internal_125k(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim2.Instance = TIM2;
    htim2.Init.Prescaler = 512 - 1;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.Period = 7;
    htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if(HAL_TIM_Base_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
    sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
    sClockSourceConfig.ClockFilter = 0;
    if(HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 4;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if(HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}
*/

static void init_timer_internal_125k_tim1(void) {
    TIM_ClockConfigTypeDef sClockSourceConfig = {0};
    TIM_MasterConfigTypeDef sMasterConfig = {0};
    TIM_OC_InitTypeDef sConfigOC = {0};

    htim1.Instance = TIM1;
    htim1.Init.Prescaler = 512 - 1;
    htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim1.Init.Period = 7;
    htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
    if(HAL_TIM_Base_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }
    sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
    sClockSourceConfig.ClockPolarity = TIM_CLOCKPOLARITY_NONINVERTED;
    sClockSourceConfig.ClockPrescaler = TIM_CLOCKPRESCALER_DIV1;
    sClockSourceConfig.ClockFilter = 0;
    if(HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
        Error_Handler();
    }
    if(HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
        Error_Handler();
    }
    sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
    sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
    if(HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
        Error_Handler();
    }
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 4;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if(HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1) != HAL_OK) {
        Error_Handler();
    }
}

static void init_gpio(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = RFID_PULL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_TIM1;
    HAL_GPIO_Init(RFID_PULL_GPIO_Port, &GPIO_InitStruct);
}

static uint16_t times_index = 0;
//constexpr uint8_t times_count = 2;
//constexpr uint8_t times[times_count] = {4, 2};

constexpr uint16_t hid_237_34672_count = 528;
constexpr uint8_t hid_237_34672[hid_237_34672_count] = {
    8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 10,
    10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,
    8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,
    10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10,
    10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,
    8,  8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,
    8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,
    8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,
    8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10,
    10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,  8,  8,
    8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,
    10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10,
    10, 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 8,
    8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  10, 10,
    10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10,
    10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10,
    10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  8,  8,  8,
    8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10,
    8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  10,
    10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10,
    10, 10, 8,  8,  8,  8,  8,  8,  10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 8,  8,  8,  8,  8,  8,
};

static void callback_hid(void* _hw, void* ctx) {
    //RfidTimerEmulator* _this = static_cast<RfidTimerEmulator*>(ctx);
    TIM_HandleTypeDef* hw = static_cast<TIM_HandleTypeDef*>(_hw);

    if(hw == &htim1) {
        hw->Instance->ARR = hid_237_34672[times_index] - 1;
        hw->Instance->CCR1 = hid_237_34672[times_index] / 2; // - 1

        hal_gpio_write(&ext_pa7_gpio, true);
        hal_gpio_write(&ext_pa7_gpio, false);

        times_index++;
        if(times_index >= hid_237_34672_count) {
            times_index = 0;
        }
    }
}

typedef struct {
    uint8_t arr;
    uint8_t ccr;
} TimerTick;

constexpr TimerTick indala_data[] = {
    {.arr = 3, .ccr = 2}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 2},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 2}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 2}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 3, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 3, .ccr = 2}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 2},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 2}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 2}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 3, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 3, .ccr = 2}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 2},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 3, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1}, {.arr = 2, .ccr = 1},
    {.arr = 2, .ccr = 1},
};

constexpr uint16_t indala_size = sizeof(indala_data) / sizeof(TimerTick);

static void callback_indala(void* _hw, void* ctx) {
    //RfidTimerEmulator* _this = static_cast<RfidTimerEmulator*>(ctx);
    TIM_HandleTypeDef* hw = static_cast<TIM_HandleTypeDef*>(_hw);

    if(hw == &htim1) {
        hw->Instance->ARR = indala_data[times_index].arr - 1;
        hw->Instance->CCR1 = indala_data[times_index].ccr;

        //hal_gpio_write(&ext_pa7_gpio, true);
        //hal_gpio_write(&ext_pa7_gpio, false);

        times_index++;
        if(times_index >= indala_size) {
            times_index = 0;
        }
    }
}

RfidTimerEmulator::RfidTimerEmulator() {
}

void RfidTimerEmulator::start(Type type) {
    hal_gpio_init(&ext_pa7_gpio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh);

    GpioPin out_pin = {.port = RFID_OUT_GPIO_Port, .pin = RFID_OUT_Pin};
    GpioPin* out_pin_record = &out_pin;

    gpio_init(out_pin_record, GpioModeOutputPushPull);
    gpio_write(out_pin_record, false);

    // pulldown iBtn pin to prevent interference from ibutton
    gpio_init(&ibutton_gpio, GpioModeOutputOpenDrain);
    gpio_write(&ibutton_gpio, false);

    init_timer_internal_125k_tim1();
    init_gpio();

    switch(type) {
    case Type::HID:
        api_interrupt_add(callback_hid, InterruptTypeTimerUpdate, this);
        break;

    case Type::Indala:
        api_interrupt_add(callback_indala, InterruptTypeTimerUpdate, this);
        break;
    }

    for(size_t i = WWDG_IRQn; i <= DMAMUX1_OVR_IRQn; i++) {
        HAL_NVIC_SetPriority(static_cast<IRQn_Type>(i), 15, 0);
    }

    HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

    HAL_TIM_PWM_Start_IT(&htim1, TIM_CHANNEL_1);
    HAL_TIM_Base_Start_IT(&htim1);
}

void RfidTimerEmulator::stop() {
    HAL_TIM_Base_Stop(&htim1);
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);

    api_interrupt_remove(callback_hid, InterruptTypeTimerUpdate);
    api_interrupt_remove(callback_indala, InterruptTypeTimerUpdate);
}

void RfidTimerEmulator::emulate() {
}
