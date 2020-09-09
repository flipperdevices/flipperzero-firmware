#include "flipper_tty_hw.h"

#define HW_UART huart1
#define UART_BUFFER_SIZE 1

extern UART_HandleTypeDef HW_UART;

static uint8_t rx_buffer[UART_BUFFER_SIZE];
static FuriRecordSubscriber* rx_tty;

void handle_hw_uart_write(const void* data, size_t size, void* ctx) 
{
	HAL_UART_Transmit(&HW_UART, (uint8_t*)data, (uint16_t)size, HAL_MAX_DELAY);
}

void handle_hw_uart_read(UART_HandleTypeDef *huart) 
{
  furi_write(rx_tty, rx_buffer, UART_BUFFER_SIZE);
	HAL_UART_Receive_IT(&HW_UART, rx_buffer, UART_BUFFER_SIZE);
}

bool initTTYHWDriver(void)
{
  if(!furi_create("/dev/tty/hw/tx", NULL, 0))
  {
    return false;
  }

  if(furi_open("/dev/tty/hw/tx", false, false, handle_hw_uart_write, NULL, NULL) == NULL) 
  {
		return false;
	}
  
  if(!furi_create("/dev/tty/hw/rx", NULL, 0))
  {
    return false;
  }

  rx_tty = furi_open("/dev/tty/hw/rx", false, false, NULL, NULL, NULL);

  if(rx_tty == NULL) 
  {
		return false;
	}
  
  HAL_UART_RegisterCallback(&HW_UART, HAL_UART_RX_COMPLETE_CB_ID, handle_hw_uart_read);
	HAL_UART_Receive_IT(&HW_UART, rx_buffer, UART_BUFFER_SIZE);

  return true;
}