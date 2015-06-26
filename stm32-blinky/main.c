#if defined STM32F1
# include <stm32f10x.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#endif

void initGPIO()
{
    GPIO_InitTypeDef GPIO_Config;
    __GPIOA_CLK_ENABLE();

    GPIO_Config.Alternate = GPIO_AF2_TIM3;
    GPIO_Config.Mode = GPIO_MODE_AF_PP;
    GPIO_Config.Pin = GPIO_PIN_6;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOA, &GPIO_Config);
}

void initTimers()
{
    __TIM3_CLK_ENABLE();

    TIM_HandleTypeDef TIM_Handle;

    TIM_Handle.Instance = TIM3;
    // 10 kHz timer.
    TIM_Handle.Init.Prescaler = (uint16_t)(2 * HAL_RCC_GetSysClockFreq() / 10000) - 1;
    // 10000 / 5000 = 1 Hz blinking.
    TIM_Handle.Init.Period = 5000;
    TIM_Handle.Init.ClockDivision = 0;
    TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_Base_Init(&TIM_Handle);
    HAL_TIM_PWM_Init(&TIM_Handle);

    TIM_OC_InitTypeDef TIM_OCConfig;

    TIM_OCConfig.OCMode = TIM_OCMODE_PWM1;
    // 2500 / 5000 = 50% duty cycle.
    TIM_OCConfig.Pulse = 2499;
    TIM_OCConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    TIM_OCConfig.OCFastMode = TIM_OCFAST_DISABLE;

    HAL_TIM_PWM_ConfigChannel(&TIM_Handle, &TIM_OCConfig, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_1);
}

static void initClock(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    }
}

void initAll(void)
{
    HAL_Init();

    initClock();
    initGPIO();
    initTimers();
}

int main(void)
{
    initAll();
    for (;;);
    return 0;
}
