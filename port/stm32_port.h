/* Copyright 2020-2023 Espressif Systems (Shanghai) CO LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include "esp_loader_io.h"

#if defined STM32C0
#include "stm32c0xx_hal.h"
#elif defined STM32F0
#include "stm32f0xx_hal.h"
#elif defined STM32F1
#include "stm32f1xx_hal.h"
#elif defined STM32F2
#include "stm32f2xx_hal.h"
#elif defined STM32F3
#include "stm32f3xx_hal.h"
#elif defined STM32F4
#include "stm32f4xx_hal.h"
#elif defined STM32F7
#include "stm32f7xx_hal.h"
#elif defined STM32G0
#include "stm32g0xx_hal.h"
#elif defined STM32G4
#include "stm32g4xx_hal.h"
#elif defined STM32H5
#include "stm32h5xx_hal.h"
#elif defined STM32H7
#include "stm32h7xx_hal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    UART_HandleTypeDef *huart;
    GPIO_TypeDef *port_io0;
    uint16_t pin_num_io0;
    GPIO_TypeDef *port_rst;
    uint16_t pin_num_rst;
} loader_stm32_config_t;

void loader_port_stm32_init(loader_stm32_config_t *config);

#ifdef __cplusplus
}
#endif
