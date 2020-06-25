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

#include <sys/param.h>
#include "esp_err.h"
#include "esp_log.h"
#include "serial_io.h"
#include "esp_loader.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "example_common.h"

#define HIGHER_BAUD_RATE 230400

void flash_binary(FILE *image, size_t image_size, size_t address)
{
    esp_loader_error_t err;
    int32_t packet_number = 0;
    static uint8_t payload[1024];

    ESP_LOGI(TAG, "Erasing flash...");
    err = esp_loader_flash_start(address, image_size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "Erasing flash failed with error %d.", err);
        return;
    }
    ESP_LOGI(TAG, "Start programming");

    while (image_size > 0) {
        size_t to_read = MIN(image_size, sizeof(payload));

        size_t read = fread(payload, 1, to_read, image);
        if (read != to_read) {
            ESP_LOGE(TAG, "Error occurred while reading file. to_read %u, read %u", to_read, read);
            return;
        }

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            ESP_LOGE(TAG, "Packet could not be written");
            return;
        }

        printf("packet: %d  written: %u B\n", packet_number++, to_read);

        image_size -= to_read;
    };

    ESP_LOGI(TAG, "Finished programming");

#if MD5_ENABLED
    err = esp_loader_flash_verify();
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "MD5 does not match. err: %d", err);
        return;
    }
    ESP_LOGI(TAG, "Flash verified");
#endif
}

FILE *get_image_and_its_size(const char *path, size_t *image_size)
{
    FILE *image = fopen(path, "r");
    if (image == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s", path);
        return NULL;
    }

    fseek(image, 0L, SEEK_END);
    *image_size = ftell(image);
    rewind(image);

    ESP_LOGW(TAG, "File %s opened. Size: %u bytes", path, *image_size);

    return image;
}

void upload_file(const char *path, size_t address)
{
    size_t image_size;
    FILE *image = get_image_and_its_size(path, &image_size);

    if (image != NULL) {
        flash_binary(image, image_size, address);
        fclose(image);
    }
}

esp_err_t connect_to_target()
{
    const loader_serial_config_t config = {
        .baud_rate = 115200,
        .uart_port = UART_NUM_1,
        .uart_rx_pin = GPIO_NUM_5,
        .uart_tx_pin = GPIO_NUM_4,
        .reset_trigger_pin = GPIO_NUM_25,
        .gpio0_trigger_pin = GPIO_NUM_26,
    };

    // Initialize UART
    esp_loader_error_t err = loader_port_serial_init(&config);
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "serial initialization failed.");
        return err;
    }

    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "Cannot connect to target. Error: %u", err);
        return err;
    }
    ESP_LOGI(TAG, "Connected to target");

#ifndef TARGET_ESP8266
    err = esp_loader_change_baudrate(HIGHER_BAUD_RATE);
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "Unable to change baud rate on target.");
        return err;
    }

    err = loader_port_change_baudrate(HIGHER_BAUD_RATE);
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "Unable to change baud rate.");
        return err;
    }
#endif

    return ESP_OK;
}