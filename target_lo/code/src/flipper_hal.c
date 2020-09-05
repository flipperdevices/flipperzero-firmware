/*
Flipper devices inc.

GPIO and HAL implementations
*/

#include "main.h"
#include "flipper_hal.h"
#include <stdio.h>
#include <zephyr.h>

void app_gpio_init(GpioPin gpio, GpioMode mode) {
    if(gpio.pin != 0) {

        switch(mode) {
            case GpioModeInput:
                printf("[GPIO] %s%d input\n", gpio.port, gpio.pin);
            break;

            case GpioModeOutput:
                printf("[GPIO] %s%d push pull\n", gpio.port, gpio.pin);
            break;

            case GpioModeOpenDrain:
                printf("[GPIO] %s%d open drain\n", gpio.port, gpio.pin);
            break;
        }

        gpio.mode = mode;
    } else {
        printf("[GPIO] no pin\n");
    }
}

void app_gpio_write(GpioPin gpio, bool state) {
    if(gpio.pin != 0) {
        if(state) {
            printf("[GPIO] %s%d on\n", gpio.port, gpio.pin);
        } else {
            printf("[GPIO] %s%d off\n", gpio.port, gpio.pin);
        }
    } else {
        printf("[GPIO] no pin\n");
    }
}

bool app_gpio_read(GpioPin gpio) {
    // TODO emulate pin state?

    return false;
}

void app_delay_us(uint32_t time) {
    // How to deal with it
    printf("[DELAY] %d us\n", time);
}

void app_delay(uint32_t time) {
    k_msleep(time);
}

void pwm_set(float value, float freq, TIM_HandleTypeDef* tim, uint32_t channel) {
    printf("[TIM] set pwm %d:%d %f Hz, %f%%\n", *tim, channel, freq, value * 100.);
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
