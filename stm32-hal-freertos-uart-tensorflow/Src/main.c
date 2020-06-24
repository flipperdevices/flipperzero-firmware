#include <string.h>
#include "main.h"
#include "cmsis_os.h"
#include "data_types.h"
#include "main_functions.h"
#include "constants.h"
#include "color.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
void inferenceTask(void const *argument);
void blinkTask(void const *argument);
void UARTTask(void const *argument);

osThreadId defaultTaskHandle;
osThreadId UARTTaskHandle;
osThreadId blinkTaskHandle;

UART_HandleTypeDef UartHandle;

void UARTTask(void const *argument)
{
	char *hello_string = "Hello World\n";
	volatile HAL_StatusTypeDef err;

	while (1) {
		HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);
		HAL_Delay(500);
		err = HAL_UART_Transmit(&UartHandle, (uint8_t *)hello_string,
					sizeof(hello_string) + 1,
					HAL_MAX_DELAY);
		if (err == HAL_OK)
			HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin,
					  GPIO_PIN_RESET);
		HAL_Delay(500);
	}
}

void inferenceTask(void const *argument)
{
	circle_t *tmp_circle;
	int count = 0;
	uint16_t screen_height = BSP_LCD_GetYSize();
	uint16_t screen_width = BSP_LCD_GetXSize();
	uint16_t x_pos, y_pos;
	setup();

	for (;;) {
		tmp_circle = loop();
		if (tmp_circle) {
			x_pos = (uint16_t)(tmp_circle->x * screen_width /
					   (2 * PI));
			y_pos = (uint16_t)((screen_height / 2) +
					   tmp_circle->y * screen_height / 2);
			BSP_LCD_FillCircle(x_pos, y_pos, tmp_circle->size);
		}
		count++;
		if (count == 40)
			vTaskSuspend(NULL);
		else
			HAL_Delay(100);
	}
}

void blinkTask(void const *argument)
{
	int count = 0;

	HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);

	for (;;) {
		count %= 10;

		switch (count++) {
		case 0:
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,
					  GPIO_PIN_SET);
			break;
		case 1:
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin,
					  GPIO_PIN_SET);
			break;
		case 2:
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin,
					  GPIO_PIN_SET);
			break;
		case 3:
			break;
		case 5:
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin,
					  GPIO_PIN_RESET);
			break;
		case 6:
			HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin,
					  GPIO_PIN_RESET);
			break;
		case 7:
			HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin,
					  GPIO_PIN_RESET);
			break;
		case 8:
			break;
		default:
			break;
		}

		HAL_Delay(100);
	}
}

int main(void)
{
	HAL_Init();

	BSP_LCD_Init();
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LCD_COLOR_WHITE);

	BSP_LCD_Clear(LCD_COLOR_WHITE);

	SystemClock_Config();

	UartHandle.Instance = USARTx;
	UartHandle.Init.BaudRate = 9600;
	UartHandle.Init.WordLength = UART_WORDLENGTH_8B;
	UartHandle.Init.StopBits = UART_STOPBITS_1;
	UartHandle.Init.Parity = UART_PARITY_ODD;
	UartHandle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UartHandle.Init.Mode = UART_MODE_TX_RX;
	UartHandle.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&UartHandle) != HAL_OK) {
		Error_Handler();
	}

	MX_GPIO_Init();

	osThreadDef(defaultTask, inferenceTask, osPriorityNormal, 0, 256);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	osThreadDef(UARTTaskHandle, UARTTask, osPriorityNormal, 0, 256);
	UARTTaskHandle = osThreadCreate(osThread(UARTTaskHandle), NULL);

	osThreadDef(blinkTaskHandle, blinkTask, osPriorityNormal, 0, 256);
	blinkTaskHandle = osThreadCreate(osThread(blinkTaskHandle), NULL);

	/* Start scheduler */
	osKernelStart();

	while (1) {
	}
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage 
           *   */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
	/** Initializes the CPU, AHB and APB busses clocks 
               *   */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 168;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 7;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks 
                                   *   */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK |
				      RCC_CLOCKTYPE_SYSCLK |
				      RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) !=
	    HAL_OK) {
		Error_Handler();
	}
}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOE_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOI_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOG, LED2_Pin | LED1_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED3_GPIO_Port, LED3_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : FSMC_NE1_Pin FSMC_NWAIT_Pin D2_Pin FSMC_NWE_Pin 
                           D3_Pin FSMC_NOE_Pin FSMC_CLK_Pin D1_Pin 
                           D0_Pin A18_Pin A17_Pin A16_Pin 
                           D15_Pin D14_Pin D13_Pin */
	GPIO_InitStruct.Pin = FSMC_NE1_Pin | FSMC_NWAIT_Pin | D2_Pin |
			      FSMC_NWE_Pin | D3_Pin | FSMC_NOE_Pin |
			      FSMC_CLK_Pin | D1_Pin | D0_Pin | A18_Pin |
			      A17_Pin | A16_Pin | D15_Pin | D14_Pin | D13_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

	/*Configure GPIO pin : FSMC_NL_Pin */
	GPIO_InitStruct.Pin = FSMC_NL_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
	HAL_GPIO_Init(FSMC_NL_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : User_Button_Pin */
	GPIO_InitStruct.Pin = User_Button_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(User_Button_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : LED2_Pin LED1_Pin */
	GPIO_InitStruct.Pin = LED2_Pin | LED1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pins : FSMC_NE3_Pin FSMC_NE2_Pin A15_Pin A14_Pin 
                           A13_Pin A12_Pin A11_Pin A10_Pin */
	GPIO_InitStruct.Pin = FSMC_NE3_Pin | FSMC_NE2_Pin | A15_Pin | A14_Pin |
			      A13_Pin | A12_Pin | A11_Pin | A10_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF12_FSMC;
	HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

	/*Configure GPIO pin : LED3_Pin */
	GPIO_InitStruct.Pin = LED3_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED3_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : LED4_Pin */
	GPIO_InitStruct.Pin = LED4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED4_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : SW1_Pin */
	GPIO_InitStruct.Pin = SW1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(SW1_GPIO_Port, &GPIO_InitStruct);
}

void Error_Handler(void)
{
	while (1) {
	}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
