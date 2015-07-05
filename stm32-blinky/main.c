#if defined STM32F1
# include <stm32f1xx_hal.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#endif

void initGPIO()
{
    GPIO_InitTypeDef GPIO_Config;

    GPIO_Config.Mode = GPIO_MODE_AF_PP;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_HIGH;

#if defined STM32F1
    __GPIOC_CLK_ENABLE();
    __AFIO_CLK_ENABLE();

    GPIO_Config.Pin = GPIO_PIN_8;
    HAL_GPIO_Init(GPIOC, &GPIO_Config);
#elif defined STM32F4
    __GPIOA_CLK_ENABLE();

    GPIO_Config.Alternate = GPIO_AF2_TIM3;
    GPIO_Config.Pin = GPIO_PIN_6;
    HAL_GPIO_Init(GPIOA, &GPIO_Config);
#endif
}

void initTimers()
{
    TIM_HandleTypeDef TIM_Handle;

    // 10 kHz timer.
#if defined STM32F1
    __TIM8_CLK_ENABLE();
    TIM_Handle.Instance = TIM8;
    TIM_Handle.Init.Prescaler = (uint16_t)(HAL_RCC_GetPCLK2Freq() / 10000) - 1;
#elif defined STM32F4
    __TIM3_CLK_ENABLE();
    TIM_Handle.Instance = TIM3;
    // TIM3 Clocked from SYSCLK = 168 MHz
    TIM_Handle.Init.Prescaler = (uint16_t)(HAL_RCC_GetSysClockFreq() / 10000) - 1;
#endif
    // 1 Hz blinking
    TIM_Handle.Init.Period = 10000;
    TIM_Handle.Init.ClockDivision = 0;
    TIM_Handle.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_Base_Init(&TIM_Handle);
    HAL_TIM_PWM_Init(&TIM_Handle);

    TIM_OC_InitTypeDef TIM_OCConfig;

    TIM_OCConfig.OCMode = TIM_OCMODE_PWM1;
    // 5000 / 10000 = 50% duty cycle.
    TIM_OCConfig.Pulse = 4999;
    TIM_OCConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    TIM_OCConfig.OCFastMode = TIM_OCFAST_DISABLE;

#if defined STM32F1
    HAL_TIM_PWM_ConfigChannel(&TIM_Handle, &TIM_OCConfig, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_3);
#elif defined STM32F4
    HAL_TIM_PWM_ConfigChannel(&TIM_Handle, &TIM_OCConfig, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&TIM_Handle, TIM_CHANNEL_1);
#endif
}

static void initClock(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();

#if defined STM32F1
    uint8_t fLatency;

    RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_OFF;
    RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
    RCC_OscInitStruct.HSICalibrationValue = 0;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

# if (defined STM32F100xB) || (defined STM32F100xE)
    // 8 MHz * 3 = 24 MHz SYSCLK
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL3;
    fLatency = FLASH_LATENCY_0;
# elif (defined STM32F101x6) || (defined STM32F101xB) || (defined STM32F101xE) || (defined STM32F101xG)
    // 8 MHz / 2 * 9 = 36 MHz SYSCLK
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    fLatency = FLASH_LATENCY_1;
# elif (defined STM32F102x6) || (defined STM32F102xB)
    // 8 MHz * 6 = 48 MHz SYSCLK
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    fLatency = FLASH_LATENCY_1;
# elif (defined STM32F103x6) || (defined STM32F103xB) || (defined STM32F103xE) || (defined STM32F103xG)
    // 8 MHz * 9 = 72 MHz SYSCLK
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    fLatency = FLASH_LATENCY_2;
# elif (defined STM32F105xC) || (defined STM32F107xC)
    // 8 MHz * 9 = 72 MHz SYSCLK
    RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
    fLatency = FLASH_LATENCY_2;
# endif

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, fLatency);

#elif defined STM32F4
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    // 8 MHz * 336 / 8 / 2 = 168 MHz SYSCLK
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
#endif
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
