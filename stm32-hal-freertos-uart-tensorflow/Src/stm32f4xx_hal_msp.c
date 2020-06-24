#include "main.h"

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
	GPIO_InitTypeDef GPIO_init;

	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	//UART
	USARTx_TX_GPIO_CLK_ENABLE();
	USARTx_RX_GPIO_CLK_ENABLE();

	USARTx_CLK_ENABLE();

	GPIO_init.Pin = USARTx_TX_PIN;
	GPIO_init.Mode = GPIO_MODE_AF_PP;
	GPIO_init.Pull = GPIO_PULLUP;
	GPIO_init.Speed = GPIO_SPEED_FAST;
	GPIO_init.Alternate = USARTx_TX_AF;

	HAL_GPIO_Init(USARTx_TX_GPIO_PORT, &GPIO_init);

	GPIO_init.Pin = USARTx_RX_PIN;
	GPIO_init.Alternate = USARTx_RX_AF;

	HAL_GPIO_Init(USARTx_RX_GPIO_PORT, &GPIO_init);

	/* System interrupt init*/
	/* PendSV_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}
