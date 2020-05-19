/* Serial flasher Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "esp_loader.h"
#include "serial_io.h"
#include "driver/gpio.h"
#include "driver/uart.h"
#include <sys/param.h>


static const char *TAG = "example";

#ifndef TARGET_ESP8266
const uint32_t BOOTLOADER_ADDRESS = 0x1000;
#else
const uint32_t BOOTLOADER_ADDRESS = 0x0;
#endif
const uint32_t PARTITION_ADDRESS = 0x8000;
const uint32_t APPLICATION_ADDRESS = 0x10000;

const uint32_t HIGHER_BAUD_RATE = 230400;
static uint8_t payload[1024];

#if defined TARGET_ESP8266
#define PARTITION_FILE "/spiffs/ESP8266/partition-table.bin"
#define BOOTLOADER_FILE "/spiffs/ESP8266/bootloader.bin"
#define APPLICATION_FILE "/spiffs/ESP8266/hello-world.bin"
#elif defined TARGET_ESP32
#define PARTITION_FILE "/spiffs/ESP32/partition-table.bin"
#define BOOTLOADER_FILE "/spiffs/ESP32/bootloader.bin"
#define APPLICATION_FILE "/spiffs/ESP32/hello-world.bin"
#elif defined TARGET_ESP32_S2
#define PARTITION_FILE "/spiffs/ESP32_S2/partition-table.bin"
#define BOOTLOADER_FILE "/spiffs/ESP32_S2/bootloader.bin"
#define APPLICATION_FILE "/spiffs/ESP32_S2/hello-world.bin"
#endif


static void flash_binary(FILE *image, size_t image_size, size_t address)
{
    esp_loader_error_t err;
    int32_t packet_number = 0;

    err = esp_loader_flash_start(address, image_size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "Flash start operation failed with error %d.", err);
        return;
    }
    ESP_LOGI(TAG, "Start programming");

    while (image_size > 0) {
        size_t to_read = MIN(image_size, sizeof(payload));

        size_t read = fread(payload, 1, to_read, image);
        if (read != to_read) {
            ESP_LOGE(TAG, "Error occurred while reading file.");
            return;
        }

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            ESP_LOGE(TAG, "Packet could not be written");
            return;
        }

        ESP_LOGI(TAG, "packet: %d  written: %u B", packet_number++, to_read);

        image_size -= to_read;
    };

    ESP_LOGI(TAG, "Finished programming");

#ifndef TARGET_ESP8266
    err = esp_loader_flash_verify();
    if (err != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "MD5 does not match. err: %d", err);
        return;
    }
#endif
    ESP_LOGI(TAG, "Flash verified");
}

static FILE *get_image_and_its_size(const char *path, size_t *image_size)
{
    FILE *image = fopen(path, "r");
    if (image == NULL) {
        ESP_LOGE(TAG, "Failed to open file %s", path);
        esp_vfs_spiffs_unregister(NULL);
        return NULL;
    }

    fseek(image, 0L, SEEK_END);
    *image_size = ftell(image);
    rewind(image);

    ESP_LOGW(TAG, "File %s opened. Size: %u bytes", path, *image_size);

    return image;
}

static void upload_file(const char *path, size_t address)
{
    size_t image_size;
    FILE *image = get_image_and_its_size(path, &image_size);

    if (image != NULL) {
        flash_binary(image, image_size, address);
        fclose(image);
    }
}

static esp_err_t connect_to_target()
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
            upload_file(PARTITION_FILE, PARTITION_ADDRESS);
            upload_file(BOOTLOADER_FILE, BOOTLOADER_ADDRESS);
            upload_file(APPLICATION_FILE, APPLICATION_ADDRESS);
        }
        esp_vfs_spiffs_unregister(NULL);
    }
}