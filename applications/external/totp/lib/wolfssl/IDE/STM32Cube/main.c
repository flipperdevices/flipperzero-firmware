/* main.c
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */

/* Example for main.c with STM32Cube/wolfssl_example.c */

#if 0 /* EXAMPLE main.c */

/* Includes ------------------------------------------------------------------*/
#include "wolfssl_example.h"
#include "wolfssl/wolfcrypt/settings.h"

/* Private variables ---------------------------------------------------------*/
CRYP_HandleTypeDef hcryp;
__ALIGN_BEGIN static const uint32_t pKeyCRYP[6] __ALIGN_END = {
            0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000};
HASH_HandleTypeDef hhash;
RNG_HandleTypeDef hrng;
RTC_HandleTypeDef hrtc;
UART_HandleTypeDef huart4;

/* Definitions for defaultTask */
#ifndef SINGLE_THREADED
    #ifdef CMSIS_OS2_H_
        osThreadId_t defaultTaskHandle;
        const osThreadAttr_t wolfCryptDemo_attributes = {
          .name = "wolfCryptDemo",
          .priority = (osPriority_t) osPriorityNormal,
          .stack_size = WOLF_EXAMPLES_STACK
        };
    #else
        osThreadId defaultTaskHandle;
    #endif
#endif

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_CRYP_Init(void);
static void MX_HASH_Init(void);
static void MX_RNG_Init(void);
static void MX_UART4_Init(void);
static void MX_RTC_Init(void);

/* Retargets the C library printf function to the USART. */
#include <stdio.h>
#ifdef __GNUC__
int __io_putchar(int ch)
#else
int fputc(int ch, FILE *f)
#endif
{
    HAL_UART_Transmit(&HAL_CONSOLE_UART, (uint8_t *)&ch, 1, 0xFFFF);

    return ch;
}
#ifdef __GNUC__
int _write(int file,char *ptr, int len)
{
    int DataIdx;
    for (DataIdx= 0; DataIdx< len; DataIdx++) {
        __io_putchar(*ptr++);
    }
    return len;
}
#endif

int main(void)
{
    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Turn off buffers, so I/O occurs immediately */
    setvbuf(stdin, NULL, _IONBF, 0);
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_CRYP_Init();
    MX_HASH_Init();
    MX_RNG_Init();
    MX_UART4_Init();
    MX_RTC_Init();
    MX_SPI1_Init();
    MX_UART4_Init();

#ifdef SINGLE_THREADED
    wolfCryptDemo(NULL);
#else
    /* Init scheduler */
    osKernelInitialize();

    /* Create the thread(s) */
    /* definition and creation of defaultTask */
    #ifdef CMSIS_OS2_H_
    defaultTaskHandle = osThreadNew(wolfCryptDemo, NULL, &wolfCryptDemo_attributes);
    #else
    osThreadDef(defaultTask, wolfCryptDemo, osPriorityNormal, 0, WOLF_EXAMPLES_STACK);
    defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);
    #endif

    /* Start scheduler */
    osKernelStart();

    /* We should never get here as control is now taken by the scheduler */

    /* Infinite loop */
    while (1) {}
#endif /* SINGLE_THREADED */
}

/** System Clock Configuration
*/
static void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

    /** Configure the main internal regulator output voltage 
    */
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
    /** Initializes the CPU, AHB and APB buses clocks 
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 8;
    RCC_OscInitStruct.PLL.PLLN = 160;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = 7;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB buses clocks 
    */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK) {
        Error_Handler();
    }
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief CRYP Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRYP_Init(void)
{
    hcryp.Instance = CRYP;
    hcryp.Init.DataType = CRYP_DATATYPE_32B;
    hcryp.Init.pKey = (uint32_t *)pKeyCRYP;
    hcryp.Init.Algorithm = CRYP_TDES_ECB;
    hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
    if (HAL_CRYP_Init(&hcryp) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief HASH Initialization Function
  * @param None
  * @retval None
  */
static void MX_HASH_Init(void)
{
    hhash.Init.DataType = HASH_DATATYPE_32B;
    if (HAL_HASH_Init(&hhash) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{
    hrng.Instance = RNG;
    if (HAL_RNG_Init(&hrng) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    /* Initialize RTC Only */
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    if (HAL_RTC_Init(&hrtc) != HAL_OK) {
        Error_Handler();
    }
    /* Initialize RTC and set the Time and Date */
    sTime.Hours = 0x0;
    sTime.Minutes = 0x0;
    sTime.Seconds = 0x0;
    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;
    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }
    sDate.WeekDay = RTC_WEEKDAY_MONDAY;
    sDate.Month = RTC_MONTH_JANUARY;
    sDate.Date = 0x1;
    sDate.Year = 0x0;

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief UART4 Initialization Function
  * @param None
  * @retval None
  */
static void MX_UART4_Init(void)
{
    huart4.Instance = UART4;
    huart4.Init.BaudRate = 115200;
    huart4.Init.WordLength = UART_WORDLENGTH_8B;
    huart4.Init.StopBits = UART_STOPBITS_1;
    huart4.Init.Parity = UART_PARITY_NONE;
    huart4.Init.Mode = UART_MODE_TX_RX;
    huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart4.Init.OverSampling = UART_OVERSAMPLING_16;
    if (HAL_UART_Init(&huart4) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOC_CLK_ENABLE();
}

 /**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1) {
        HAL_IncTick();
    }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
    /* User can add his own implementation to report the HAL error return state */
    while(1)
    {
    }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
    /* User can add his own implementation to report the file name and line number,
      tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */


#if 0
/* Working _sbrk example for .ld based libC malloc/free */
/* Replace this with one in Core/Src/sysmem.c */
/* Symbols defined in the linker script */
extern uint8_t _end; 
extern uint8_t _estack;
extern uint32_t _Min_Stack_Size;
void* _sbrk(ptrdiff_t incr)
{
    static uint8_t* __sbrk_heap_end = NULL;
    const uint32_t stack_limit = (uint32_t)&_estack - (uint32_t)&_Min_Stack_Size;
    const uint8_t* max_heap = (uint8_t *)stack_limit;
    uint8_t* prev_heap_end;

    /* Initialize heap end at first call */
    if (__sbrk_heap_end == NULL) {
        __sbrk_heap_end = &_end;
    }

    /* Protect heap from growing into the reserved MSP stack */
    if (__sbrk_heap_end + incr > max_heap) {
        errno = ENOMEM;
        return (void *)-1;
    }

    prev_heap_end = __sbrk_heap_end;
    __sbrk_heap_end += incr;

    return (void*)prev_heap_end;
}
#endif

#endif /* EXAMPLE main.c */
