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

#define BIN_FIRST_SEGMENT_OFFSET    0x18
// Maximum block sized for RAM and Flash writes, respectively.
#define ESP_RAM_BLOCK               0x1800

typedef struct {
    const uint8_t *data;
    uint32_t size;
    uint32_t addr;
} partition_attr_t;

typedef struct {
    partition_attr_t boot;
    partition_attr_t part;
    partition_attr_t app;
} example_binaries_t;

typedef struct {
    partition_attr_t ram_app;
} example_ram_app_binary_t;

/**
 * @brief esptool portable bin header format
 */
typedef struct example_bin_header {
    uint8_t magic;
    uint8_t segments;
    uint8_t flash_mode;
    uint8_t flash_size_freq;
    uint32_t entrypoint;
} example_bin_header_t;

/**
 * @brief esptool portable bin segment format
 */
typedef struct example_bin_segment {
    uint32_t addr;
    uint32_t size;
    uint8_t *data;
} example_bin_segment_t;


void get_example_binaries(target_chip_t target, example_binaries_t *binaries);
void get_example_ram_app_binary(target_chip_t target, example_ram_app_binary_t *bin);
esp_loader_error_t connect_to_target(uint32_t higher_transmission_rate);
esp_loader_error_t flash_binary(const uint8_t *bin, size_t size, size_t address);
esp_loader_error_t load_ram_binary(const uint8_t *bin);
