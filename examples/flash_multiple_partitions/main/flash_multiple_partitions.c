/* Flash multiple partitions example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sys/param.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "loader_config.h"
#include "example_common.h"


#ifndef TARGET_ESP8266
const uint32_t BOOTLOADER_ADDRESS = 0x1000;
#else
const uint32_t BOOTLOADER_ADDRESS = 0x0;
#endif
const uint32_t PARTITION_ADDRESS = 0x8000;
const uint32_t APPLICATION_ADDRESS = 0x10000;

#if defined TARGET_ESP8266
#define BINARY_PATH "/spiffs/ESP8266/"
#elif defined TARGET_ESP32
#define BINARY_PATH "/spiffs/ESP32/"
#elif defined TARGET_ESP32_S2
#define BINARY_PATH "/spiffs/ESP32_S2/"
#endif

static esp_err_t register_vfs()
{
    ESP_LOGI(TAG, "Initializing SPIFFS");

    esp_vfs_spiffs_conf_t conf = {
        .base_path = "/spiffs",
        .partition_label = NULL,
        .max_files = 5,
        .format_if_mount_failed = false
    };

    // Use settings defined above to initialize and mount SPIFFS filesystem.
    esp_err_t ret = esp_vfs_spiffs_register(&conf);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(TAG, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
    }

    return ret;
}

void app_main(void)
{
    if ( register_vfs() == ESP_OK ) {
        if ( connect_to_target() == ESP_OK) {
            upload_file(BINARY_PATH"partition-table.bin", PARTITION_ADDRESS);
            upload_file(BINARY_PATH"bootloader.bin", BOOTLOADER_ADDRESS);
            upload_file(BINARY_PATH"hello-world.bin", APPLICATION_ADDRESS);
        }
        esp_vfs_spiffs_unregister(NULL);
    }
}