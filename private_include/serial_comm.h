/* Copyright 2020 Espressif Systems (Shanghai) PTE LTD
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
#include <stdbool.h>
#include "esp_loader.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Macro which can be used to check the error code,
 * and return in case the code is not ESP_LOADER_SUCCESS.
 */
#define RETURN_ON_ERROR(x) do {         \
    esp_loader_error_t _err_ = (x);     \
    if (_err_ != ESP_LOADER_SUCCESS) {  \
        return _err_;                   \
    }                                   \
} while(0)

esp_loader_error_t loader_flash_begin_cmd(uint32_t offset, uint32_t erase_size, uint32_t block_size, uint32_t blocks_to_write);

esp_loader_error_t loader_flash_data_cmd(const uint8_t *data, uint32_t size);

esp_loader_error_t loader_flash_end_cmd(bool stay_in_loader);

esp_loader_error_t loader_write_reg_cmd(uint32_t address, uint32_t value, uint32_t mask, uint32_t delay_us);

esp_loader_error_t loader_read_reg_cmd(uint32_t address, uint32_t *reg);

esp_loader_error_t loader_sync_cmd(void);

esp_loader_error_t loader_spi_attach_cmd(uint32_t config);


// TODO implement the rest of supported commands
// esp_loader_error_t set_spi_params(uint32_t *params);
// esp_loader_error_t change_baudrate(uint32_t *baud);
// esp_loader_error_t flash_write_defl();
// esp_loader_error_t spi_flash_md5_is(uint8_t data[16]);


#ifdef __cplusplus
}
#endif