/*
Flipper devices inc.

GPIO and HAL implementations
*/

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include <device.h>
#include <devicetree.h>
#include <drivers/gpio.h>

typedef enum {
    GpioModeInput = GPIO_INPUT,
    GpioModeOutput = GPIO_OUTPUT,
    GpioModeOpenDrain = GPIO_OUTPUT
} GpioMode;

typedef struct {
    char* label;
    gpio_pin_t pin;
} GpioPin;

#define LED0_NODE DT_ALIAS(led0)
#if DT_NODE_HAS_STATUS(LED0_NODE, okay)
  #define LED0_LABEL DT_GPIO_LABEL(LED0_NODE, gpios)
  #define LED0_PIN DT_GPIO_PIN(LED0_NODE, gpios)
#else
  #error "Unsupported board: led0 devicetree alias is not defined"
  #define LED0_LABEL ""
  #define LED0_PIN 0
#endif

#define LED_RED {.label = LED0_LABEL, .pin = LED0_PIN} 

void app_gpio_init(GpioPin gpio, GpioMode mode);

void app_gpio_write(GpioPin gpio, bool state);

bool app_gpio_read(GpioPin gpio);

void app_delay_us(uint32_t time);
void app_delay(uint32_t time);

void pwm_set(float value, float freq, TIM_HandleTypeDef* tim, uint32_t channel);

void app_tim_ic_init(bool both);

void app_tim_pulse(uint32_t width);

void app_tim_stop();
