#include "flipper_gpio.h"

static flp_GPIO pin_a1 = {GPIOA, GPIO_PIN_1};
static flp_GPIO pin_a2 = {GPIOA, GPIO_PIN_2};

SemaphoreHandle_t xGpioSetupSemaphore = NULL;
StaticSemaphore_t xGpioSetupMutexBuffer;

// TODO: split driver and gpio library
bool initGpioDriver(void){
  xGpioSetupSemaphore = xSemaphoreCreateMutexStatic( &xGpioSetupMutexBuffer );

  if(!furi_create("/dev/gpio/a/1", (void*)&pin_a1, sizeof(pin_a1))) {
      return false;
  }

  if(!furi_create("/dev/gpio/a/2", (void*)&pin_a2, sizeof(pin_a2))) {
      return false;
  }

  return true;
}

// strangely enough, this defined in xx_hal_gpio.c, so...
#define GPIO_NUMBER           (16U)

bool digitalRead(flp_GPIO *GPIO)
{
  if ((GPIO->GPIO_Port->IDR & GPIO->allowed_GPIO_Pin) != 0x00U)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void digitalWrite(flp_GPIO *GPIO, bool PinState)
{
  // writing to BSSR is an atomic operation
  if (PinState == true)
  {
    GPIO->GPIO_Port->BSRR = GPIO->allowed_GPIO_Pin;
  }
  else
  {
    GPIO->GPIO_Port->BSRR = (uint32_t)GPIO->allowed_GPIO_Pin << GPIO_NUMBER;
  }
}

void digitalToggle(flp_GPIO *GPIO)
{
  // TODO: think about concurrency
  if ((GPIO->GPIO_Port->ODR & GPIO->allowed_GPIO_Pin) == GPIO->allowed_GPIO_Pin)
  {
    GPIO->GPIO_Port->BSRR = (uint32_t)GPIO->allowed_GPIO_Pin << GPIO_NUMBER;
  }
  else
  {
    GPIO->GPIO_Port->BSRR = GPIO->allowed_GPIO_Pin;
  }
}

void pinMode(flp_GPIO *GPIO, flp_GPIO_Mode Mode, flp_GPIO_Pullup Pullup, flp_GPIO_Speed Speed){
  // TODO: Alternate Functions

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  GPIO_InitStruct.Pin = GPIO->allowed_GPIO_Pin;
  GPIO_InitStruct.Mode = Mode;
  GPIO_InitStruct.Pull = Pullup;
  GPIO_InitStruct.Speed = Speed;

  while( xSemaphoreTake( xGpioSetupSemaphore, portMAX_DELAY ) != pdTRUE ){ };

  HAL_GPIO_Init(GPIO->GPIO_Port, &GPIO_InitStruct);
  xSemaphoreGive( xGpioSetupSemaphore );
}