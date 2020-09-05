/*
Flipper devices inc.

GPIO and HAL implementations
*/

#include "main.h"
#include "flipper_hal.h"
#include <stdio.h>
#include <zephyr.h>

void app_gpio_init(GpioPin gpio, GpioMode mode) {
    struct device *port;
    port = device_get_binding(gpio.label);
    gpio_pin_configure(port, gpio.pin, mode);
}

void app_gpio_write(GpioPin gpio, bool state) {
    struct device *port;
    port = device_get_binding(gpio.label);
    gpio_pin_set(port, gpio.pin, state);
}

bool app_gpio_read(GpioPin gpio) {
    struct device *port;
    port = device_get_binding(gpio.label);
    return gpio_pin_get(port, gpio.pin);
}

void app_delay_us(uint32_t time) {
    printf("[DELAY] %d us\n", time);
}

void app_delay(uint32_t time) {
    k_msleep(time);
}

void pwm_set(float value, float freq, TIM_HandleTypeDef* tim, uint32_t channel) {
    printf("[TIM] set pwm: %d %f Hz, %f%%\n", channel, freq, value * 100.);
}


void app_tim_ic_init(bool both) {
    printf("[TIM] init\n");
}

void app_tim_pulse(uint32_t width) {
    printf("[TIM] pulse %d\n", width);
}

void app_tim_stop() {
    printf("[TIM] stop\n");
}
