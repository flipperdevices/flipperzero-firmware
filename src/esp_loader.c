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

#include "serial_comm.h"
#include "serial_io.h"
#include "esp_loader.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#endif

static const uint32_t DEFAULT_TIMEOUT = 500;
static const uint32_t SPI_PIN_CONFIG_DEFAULT = 0;
static const uint32_t DEFAULT_FLASH_TIMEOUT = 3000;       // timeout for most flash operations
static const uint32_t ERASE_REGION_TIMEOUT_PER_MB = 3000; // timeout (per megabyte) for erasing a region
static const uint8_t  PADDING_PATTERN = 0xFF;


static uint32_t s_flash_write_size = 0;


static uint32_t timeout_per_mb(uint32_t size_bytes)
{
    uint32_t timeout = ERASE_REGION_TIMEOUT_PER_MB * (size_bytes / 1e6);
    return MAX(timeout, DEFAULT_FLASH_TIMEOUT);
}


esp_loader_error_t esp_loader_connect(esp_loader_connect_args_t *connect_args)
{
    int32_t trials = connect_args->trials;

    loader_port_enter_bootloader();

    do {
        loader_port_start_timer(connect_args->sync_timeout);
        esp_loader_error_t err = loader_sync_cmd();
        if (err != ESP_LOADER_ERROR_TIMEOUT) {
            return err;
        }
        if (--trials > 0) {
            loader_port_delay_ms(100);
        }
    } while (trials > 0);

    return ESP_LOADER_ERROR_TIMEOUT;
}


esp_loader_error_t esp_loader_flash_start(uint32_t offset, uint32_t image_size, uint32_t block_size)
{
    uint32_t blocks_to_write = (image_size + block_size - 1) / block_size;
    uint32_t erase_size = block_size * blocks_to_write;
    s_flash_write_size = block_size;

    loader_port_start_timer(DEFAULT_TIMEOUT);

    RETURN_ON_ERROR( loader_spi_attach_cmd(SPI_PIN_CONFIG_DEFAULT) );

    loader_port_start_timer(timeout_per_mb(erase_size));

    return loader_flash_begin_cmd(offset, erase_size, block_size, blocks_to_write);
}


esp_loader_error_t esp_loader_flash_write(void *payload, uint32_t size)
{
    uint32_t padding_bytes = s_flash_write_size - size;
    uint8_t *data = (uint8_t *)payload;

    while (padding_bytes--) {
        data[size++] = PADDING_PATTERN;
    }

    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_flash_data_cmd(data, s_flash_write_size);
}


esp_loader_error_t esp_loader_flash_finish(bool reboot)
{
    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_flash_end_cmd(!reboot);
}


esp_loader_error_t esp_loader_read_register(uint32_t address, uint32_t *reg_value)
{
    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_read_reg_cmd(address, reg_value);
}


esp_loader_error_t esp_loader_write_register(uint32_t address, uint32_t reg_value)
{
    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_write_reg_cmd(address, reg_value, 0xFFFFFFFF, 0);
}

void esp_loader_reset_target(void)
{
    loader_port_reset_target();
}

