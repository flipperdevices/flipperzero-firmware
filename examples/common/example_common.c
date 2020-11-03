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

#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include "serial_io.h"
#include "esp_loader.h"
#include "example_common.h"

#ifndef SINGLE_TARGET_SUPPORT

#define BOOTLOADER_ADDRESS_8266  0x1000
#define BOOTLOADER_ADDRESS  0x1000
#define PARTITION_ADDRESS   0x8000
#define APPLICATION_ADDRESS 0x10000

extern const uint8_t  ESP32_bootloader_bin[];
extern const uint32_t ESP32_bootloader_bin_size;
extern const uint8_t  ESP32_hello_world_bin[];
extern const uint32_t ESP32_hello_world_bin_size;
extern const uint8_t  ESP32_partition_table_bin[];
extern const uint32_t ESP32_partition_table_bin_size;

extern const uint8_t  ESP32_S2_bootloader_bin[];
extern const uint32_t ESP32_S2_bootloader_bin_size;
extern const uint8_t  ESP32_S2_hello_world_bin[];
extern const uint32_t ESP32_S2_hello_world_bin_size;
extern const uint8_t  ESP32_S2_partition_table_bin[];
extern const uint32_t ESP32_S2_partition_table_bin_size;

extern const uint8_t  ESP8266_bootloader_bin[];
extern const uint32_t ESP8266_bootloader_bin_size;
extern const uint8_t  ESP8266_hello_world_bin[];
extern const uint32_t ESP8266_hello_world_bin_size;
extern const uint8_t  ESP8266_partition_table_bin[];
extern const uint32_t ESP8266_partition_table_bin_size;

void get_example_binaries(target_chip_t target, example_binaries_t *bins)
{
    if (target == ESP8266_CHIP) {
        bins->boot.data = ESP8266_bootloader_bin;
        bins->boot.size = ESP8266_bootloader_bin_size;
        bins->boot.addr = BOOTLOADER_ADDRESS_8266;
        bins->part.data = ESP8266_partition_table_bin;
        bins->part.size = ESP8266_partition_table_bin_size;
        bins->part.addr = PARTITION_ADDRESS;
        bins->app.data  = ESP8266_hello_world_bin;
        bins->app.size  = ESP8266_hello_world_bin_size;
        bins->app.addr  = APPLICATION_ADDRESS;
    } else if (target == ESP32_CHIP) {
        bins->boot.data = ESP32_bootloader_bin;
        bins->boot.size = ESP32_bootloader_bin_size;
        bins->boot.addr = BOOTLOADER_ADDRESS;
        bins->part.data = ESP32_partition_table_bin;
        bins->part.size = ESP32_partition_table_bin_size;
        bins->part.addr = PARTITION_ADDRESS;
        bins->app.data  = ESP32_hello_world_bin;
        bins->app.size  = ESP32_hello_world_bin_size;
        bins->app.addr  = APPLICATION_ADDRESS;
    } else {
        bins->boot.data = ESP32_S2_bootloader_bin;
        bins->boot.size = ESP32_S2_bootloader_bin_size;
        bins->boot.addr = BOOTLOADER_ADDRESS;
        bins->part.data = ESP32_S2_partition_table_bin;
        bins->part.size = ESP32_S2_partition_table_bin_size;
        bins->part.addr = PARTITION_ADDRESS;
        bins->app.data  = ESP32_S2_hello_world_bin;
        bins->app.size  = ESP32_S2_hello_world_bin_size;
        bins->app.addr  = APPLICATION_ADDRESS;
    }
}

#endif

esp_loader_error_t connect_to_target(uint32_t higrer_baudrate)
{
    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    esp_loader_error_t err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Cannot connect to target. Error: %u\n", err);
        return err;
    }
    printf("Connected to target\n");

    if (higrer_baudrate && esp_loader_get_target() != ESP8266_CHIP) {
        err = esp_loader_change_baudrate(higrer_baudrate);
        if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC) {
            printf("ESP8266 does not support change baudrate command.");
            return err;
        } else if (err != ESP_LOADER_SUCCESS) {
            printf("Unable to change baud rate on target.");
            return err;
        } else {
            err = loader_port_change_baudrate(higrer_baudrate);
            if (err != ESP_LOADER_SUCCESS) {
                printf("Unable to change baud rate.");
                return err;
            }
            printf("Baudrate changed\n");
        }
    }

    return ESP_LOADER_SUCCESS;
}


esp_loader_error_t flash_binary(const uint8_t *bin, size_t size, size_t address)
{
    esp_loader_error_t err;
    static uint8_t payload[1024];
    const uint8_t *bin_addr = bin;

    printf("Erasing flash (this may take a while)...\n");
    err = esp_loader_flash_start(address, size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS) {
        printf("Erasing flash failed with error %d.\n", err);
        return err;
    }
    printf("Start programming\n");

    size_t binary_size = size;
    size_t written = 0;

    while (size > 0) {
        size_t to_read = MIN(size, sizeof(payload));
        memcpy(payload, bin_addr, to_read);

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            printf("\nPacket could not be written! Error %d.\n", err);
            return err;
        }

        size -= to_read;
        bin_addr += to_read;
        written += to_read;

        int progress = (int)(((float)written / binary_size) * 100);
        printf("\rProgress: %d %%", progress);
        fflush(stdout);
    };

    printf("\nFinished programming\n");

#if MD5_ENABLED
    err = esp_loader_flash_verify();
    if (err == ESP_LOADER_ERROR_UNSUPPORTED_FUNC) {
        printf("ESP8266 does not support flash verify command.");
        return err;
    } else if (err != ESP_LOADER_SUCCESS) {
        printf("MD5 does not match. err: %d\n", err);
        return err;
    }
    printf("Flash verified\n");
#endif

    return ESP_LOADER_SUCCESS;
}