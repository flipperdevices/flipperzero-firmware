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

#ifndef TARGET_ESP8266
    #define BOOTLOADER_ADDRESS 0x1000
#else
    #define BOOTLOADER_ADDRESS 0x0
#endif

#define PARTITION_ADDRESS   0x8000
#define APPLICATION_ADDRESS 0x10000

extern const unsigned char bootloader_bin[];
extern const unsigned bootloader_bin_size;
extern const unsigned char hello_world_bin[];
extern const unsigned hello_world_bin_size;
extern const unsigned char partition_table_bin[];
extern const unsigned partition_table_bin_size;

esp_loader_error_t connect_to_target(uint32_t higrer_baudrate);
esp_loader_error_t flash_binary(const unsigned char *bin, size_t size, size_t address);
