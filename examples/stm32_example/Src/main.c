/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

#include "main.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "serial_io.h"
#include "esp_loader.h"

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART2_UART_Init(void);

void flash_images(void);
void loader_port_stm32_init(UART_HandleTypeDef *huart, 
                            GPIO_TypeDef* port_io0, 
                            uint16_t pin_num_io0, 
                            GPIO_TypeDef* port_rst, 
                            uint16_t pin_num_rst);

extern const unsigned char bootloader_bin[];
extern const unsigned bootloader_bin_size;
extern const unsigned char hello_world_bin[];
extern const unsigned hello_world_bin_size;
extern const unsigned char partition_table_bin[];
extern const unsigned partition_table_bin_size;

const uint32_t BOOTLOADER_ADDRESS  = 0x1000;
const uint32_t PARTITION_ADDRESS   = 0x8000;
const uint32_t APPLICATION_ADDRESS = 0x10000;


#define HIGHER_BAUD_RATE 230400

esp_loader_error_t flash_binary(const unsigned char *bin, size_t size, size_t address)
{
    esp_loader_error_t err;
    int32_t packet_number = 0;
    static uint8_t payload[1024];
    const unsigned char *bin_addr = bin;

    printf("Erasing flash...");
    err = esp_loader_flash_start(address, size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS) {
        printf("Erasing flash failed with error %d.\n", err);
        return err;
    }
    printf("Start programming");

    while (size > 0) {
        size_t to_read = MIN(size, sizeof(payload));
        memcpy(payload, bin_addr, to_read);

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            printf("Packet could not be written\n");
            return err;
        }

        printf("packet: %ld  written: %u B\n", packet_number++, to_read);

        size -= to_read;
        bin_addr += to_read;
    };

    printf("Finished programming\n");

#if MD5_ENABLED
    err = esp_loader_flash_verify();
    if (err != ESP_LOADER_SUCCESS) {
        printf("MD5 does not match. err: %d\n", err);
        return err;
    }
    printf("Flash verified\n");
#endif
    
    return ESP_LOADER_SUCCESS;
}

HAL_StatusTypeDef connect_to_target()
{
    loader_port_stm32_init(&huart1, GPIOB, TARGET_IO0_Pin, GPIOB, TARGET_RST_Pin);

    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    esp_loader_error_t err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Cannot connect to target. Error: %u\n", err);
        return err;
    }
    printf("Connected to target\n");

    err = esp_loader_change_baudrate(HIGHER_BAUD_RATE);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Unable to change baud rate on target.");
        return err;
    }

    err = loader_port_change_baudrate(HIGHER_BAUD_RATE);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Unable to change baud rate.");
        return err;
    }
    printf("Baudrate changed\n");

    return HAL_OK;
}

void flash_images()
{
    if(connect_to_target() == HAL_OK) {
        flash_binary(bootloader_bin, bootloader_bin_size, BOOTLOADER_ADDRESS);
        flash_binary(partition_table_bin, partition_table_bin_size, PARTITION_ADDRESS);
        flash_binary(hello_world_bin, hello_world_bin_size, APPLICATION_ADDRESS);
    }
}


int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  flash_images();
  while (1) { }
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
}

static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GREEN_LED_Pin|ORANGE_LED_Pin|RED_LED_Pin|BLUE_LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TARGET_RST_Pin|TARGET_IO0_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : GREEN_LED_Pin ORANGE_LED_Pin RED_LED_Pin BLUE_LED_Pin */
  GPIO_InitStruct.Pin = GREEN_LED_Pin|ORANGE_LED_Pin|RED_LED_Pin|BLUE_LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pins : PB4 PB5 */
  GPIO_InitStruct.Pin = TARGET_RST_Pin|TARGET_IO0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* User can add his own implementation to report the HAL error return state */
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
