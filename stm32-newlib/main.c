#if defined STM32F1
# include <stm32f1xx_hal.h>
#elif defined STM32F2
# include <stm32f2xx_hal.h>
#elif defined STM32F4
# include <stm32f4xx_hal.h>
#elif defined STM32G0
#include <stm32g0xx_hal.h>
#endif

#include <stdio.h>
#include <time.h>

UART_HandleTypeDef UART_Handle;
uint64_t virtualTimer;

void SysTick_Handler(void)
{
    HAL_IncTick();
    virtualTimer++;
}

void initGPIO()
{
    GPIO_InitTypeDef GPIO_Config;
#if defined STM32F1
    __GPIOA_CLK_ENABLE();

    /* USART1 */
    GPIO_Config.Mode = GPIO_MODE_AF_PP;
    GPIO_Config.Pin = GPIO_PIN_9;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_HIGH;

    HAL_GPIO_Init(GPIOA, &GPIO_Config);

    GPIO_Config.Mode = GPIO_MODE_INPUT;
    GPIO_Config.Pin = GPIO_PIN_10;

    HAL_GPIO_Init(GPIOA, &GPIO_Config);
#elif defined STM32F2
    __GPIOB_CLK_ENABLE();

    /* USART1 */
    GPIO_Config.Alternate = GPIO_AF7_USART1;
    GPIO_Config.Mode = GPIO_MODE_AF_PP;
    GPIO_Config.Pin = GPIO_PIN_6 | GPIO_PIN_7;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOB, &GPIO_Config);
#elif defined STM32F4
    __GPIOC_CLK_ENABLE();

    /* USART3 */
    GPIO_Config.Alternate = GPIO_AF7_USART3;
    GPIO_Config.Mode = GPIO_MODE_AF_PP;
    GPIO_Config.Pin = GPIO_PIN_10 | GPIO_PIN_11;
    GPIO_Config.Pull = GPIO_NOPULL;
    GPIO_Config.Speed = GPIO_SPEED_FAST;

    HAL_GPIO_Init(GPIOC, &GPIO_Config);
#elif defined STM32G0
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_Config.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_Config.Mode = GPIO_MODE_AF_PP;
    GPIO_Config.Pull = GPIO_PULLUP;
    GPIO_Config.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_Config.Alternate = GPIO_AF1_USART2;

    HAL_GPIO_Init(GPIOA, &GPIO_Config);
#endif
}

void initUART()
{
#if defined STM32F1
    __USART1_CLK_ENABLE();
    UART_Handle.Instance = USART1;
#elif defined STM32F2
    __USART1_CLK_ENABLE();
    UART_Handle.Instance = USART1;
#elif defined STM32F4
    __USART3_CLK_ENABLE();
    UART_Handle.Instance = USART3;
#elif defined STM32G0
    __HAL_RCC_USART1_CLK_ENABLE();
    UART_Handle.Instance = USART1;

#endif

    UART_Handle.Init.BaudRate = 115200;
    UART_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    UART_Handle.Init.Mode = UART_MODE_TX_RX;
    UART_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
    UART_Handle.Init.Parity = UART_PARITY_NONE;
    UART_Handle.Init.StopBits = UART_STOPBITS_1;
    UART_Handle.Init.WordLength = UART_WORDLENGTH_8B;

    HAL_UART_Init(&UART_Handle);
}

void initClock(void)
{
    RCC_ClkInitTypeDef RCC_ClkInitStruct;
    RCC_OscInitTypeDef RCC_OscInitStruct;

#if defined STM32F1
    __HAL_RCC_PWR_CLK_ENABLE();
    uint8_t fLatency;

    RCC_OscInitStruct.OscillatorType  = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;
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

#elif defined STM32F2
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = 25;
    RCC_OscInitStruct.PLL.PLLN = 240;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 5;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3);

#elif defined STM32F4
    __HAL_RCC_PWR_CLK_ENABLE();
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
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);

    if (HAL_GetREVID() == 0x1001)
    {
        __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
    }
#elif defined STM32G0
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0);

    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
    PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

#endif
}

void setTime(uint32_t time)
{
    virtualTimer = (uint64_t)time * 1000;
}

void initAll(void)
{
    HAL_Init();

    initClock();
    initGPIO();
    initUART();
}

int main(void)
{
    initAll();

    for (;;)
    {
        char c = 0;
        time_t t;
        scanf("%c", &c);
        switch (c)
        {
        case 's':
            scanf("%lld", &t);
            setTime(t);
            printf("Current time changed: %d - %s\r", t, ctime(&t));
            break;
        default:
            t = time(0);
            printf("Current time: %d - %s\r", t, ctime(&t));
            break;
        }
    }
    return 0;
}
