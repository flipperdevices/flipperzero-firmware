/*
Flipper devices inc.

GPIO and HAL implementations
*/

#include "main.h"
#include "flipper_hal.h"
#include <stdio.h>

void pwm_set(float value, float freq, TIM_HandleTypeDef* tim, uint32_t channel) {
    printf("[TIM] set pwm %d:%d %f Hz, %f%%\n", *tim, channel, freq, value * 100.);
}

void HAL_GPIO_WritePin(const char* port, uint32_t pin, HAL_GPIO_PIN_STATE state) {
    printf("[GPIO] set pin %s:%d = %d\n", port, pin, state);
}

HAL_StatusTypeDef
HAL_SPI_Transmit(SPI_HandleTypeDef* hspi, uint8_t* pData, uint16_t size, uint32_t Timeout) {
    printf("[SPI] write %d to %s: ", size, *hspi);
    for(size_t i = 0; i < size; i++) {
        printf("%02X ", pData[i]);
    }
    printf("\n");

    return 0;
}

SPI_HandleTypeDef hspi1 = "spi1";
