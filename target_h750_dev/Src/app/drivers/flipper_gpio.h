#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "../core/furi.h"

typedef enum {
  Input = GPIO_MODE_INPUT, 
  OutputPushPull = GPIO_MODE_OUTPUT_PP,
  OutputOpenDrain = GPIO_MODE_OUTPUT_OD,
  AltFunctionPushPull = GPIO_MODE_AF_PP,
  AltFunctionOpenDrain = GPIO_MODE_AF_OD,
  Analog = GPIO_MODE_ANALOG,
  Interrupt_Rise = GPIO_MODE_IT_RISING,
  Interrupt_Fall = GPIO_MODE_IT_FALLING, 
  Interrupt_RiseFall = GPIO_MODE_IT_RISING_FALLING,
  Event_Rise = GPIO_MODE_EVT_RISING,
  Event_Fall = GPIO_MODE_EVT_FALLING,
  Event_RiseFall = GPIO_MODE_EVT_RISING_FALLING,     
} flp_GPIO_Mode;

typedef enum {
  LowSpeed = GPIO_SPEED_FREQ_LOW, 
  MediumSpeed = GPIO_SPEED_FREQ_MEDIUM, 
  HighSpeed = GPIO_SPEED_FREQ_HIGH, 
  VeryHighSpeed = GPIO_SPEED_FREQ_VERY_HIGH,
} flp_GPIO_Speed;

typedef enum flp_GPIO_Pullup_Enum {
  NoPullup = GPIO_NOPULL, 
  PullUp = GPIO_PULLUP, 
  PullDown = GPIO_PULLDOWN,
} flp_GPIO_Pullup;

typedef struct{
    GPIO_TypeDef *GPIO_Port;
    uint8_t allowed_GPIO_Pin;
} flp_GPIO;

void pinMode(flp_GPIO *GPIO, flp_GPIO_Mode Mode, flp_GPIO_Pullup Pullup, flp_GPIO_Speed Speed);
void digitalWrite(flp_GPIO *GPIO, bool PinState);
bool digitalRead(flp_GPIO *GPIO);

bool initGpioDriver(void);

#ifdef __cplusplus
}
#endif

