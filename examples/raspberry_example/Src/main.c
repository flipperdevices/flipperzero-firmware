/* Flash multiple partitions example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/param.h>
#include "serial_io.h"
#include "esp_loader.h"

#define TARGET_RST_Pin 2
#define TARGET_IO0_Pin 3

#define DEFAULT_BAUD_RATE 115200
#define HIGHER_BAUD_RATE  460800
#define SERIAL_DEVICE     "/dev/ttyS0"

esp_loader_error_t loader_port_rpi_init(const char *device,
                                        uint32_t baudrate,
                                        uint32_t reset_trigger_pin,
                                        uint32_t gpio0_trigger_pin);

extern const unsigned char bootloader_bin[];
extern const unsigned bootloader_bin_size;
extern const unsigned char hello_world_bin[];
extern const unsigned hello_world_bin_size;
extern const unsigned char partition_table_bin[];
extern const unsigned partition_table_bin_size;

const uint32_t BOOTLOADER_ADDRESS  = 0x1000;
const uint32_t PARTITION_ADDRESS   = 0x8000;
const uint32_t APPLICATION_ADDRESS = 0x10000;


esp_loader_error_t flash_binary(const unsigned char *bin, size_t size, size_t address)
{
    esp_loader_error_t err;
    int32_t packet_number = 0;
    static uint8_t payload[1024];
    const unsigned char *bin_addr = bin;

    printf("Erasing flash...\n");
    err = esp_loader_flash_start(address, size, sizeof(payload));
    if (err != ESP_LOADER_SUCCESS) {
        printf("Erasing flash failed with error %d.\n", err);
        return err;
    }
    printf("Start programming\n");

    while (size > 0) {
        size_t to_read = MIN(size, sizeof(payload));
        memcpy(payload, bin_addr, to_read);

        err = esp_loader_flash_write(payload, to_read);
        if (err != ESP_LOADER_SUCCESS) {
            printf("Packet could not be written\n");
            return err;
        }

        printf("packet: %ld  written: %u B\n", packet_number++, to_read);

        size -= to_read;
        bin_addr += to_read;
    };

    printf("Finished programming\n");

#if MD5_ENABLED
    err = esp_loader_flash_verify();
    if (err != ESP_LOADER_SUCCESS) {
        printf("MD5 does not match. err: %d\n", err);
        return err;
    }
    printf("Flash verified\n");
#endif

    return ESP_LOADER_SUCCESS;
}

esp_loader_error_t connect_to_target()
{
    loader_port_rpi_init(SERIAL_DEVICE, DEFAULT_BAUD_RATE, TARGET_RST_Pin, TARGET_IO0_Pin);

    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    esp_loader_error_t err = esp_loader_connect(&connect_config);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Cannot connect to target. Error: %u\n", err);
        return err;
    }
    printf("Connected to target\n");

    err = esp_loader_change_baudrate(HIGHER_BAUD_RATE);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Unable to change baud rate on target.\n");
        return err;
    }

    err = loader_port_change_baudrate(HIGHER_BAUD_RATE);
    if (err != ESP_LOADER_SUCCESS) {
        printf("Unable to change baud rate.\n");
        return err;
    }
    printf("Baudrate changed\n");

    return ESP_LOADER_SUCCESS;
}

int main(void)
{
    if (connect_to_target() == ESP_LOADER_SUCCESS) {
        flash_binary(bootloader_bin, bootloader_bin_size, BOOTLOADER_ADDRESS);
        flash_binary(partition_table_bin, partition_table_bin_size, PARTITION_ADDRESS);
        flash_binary(hello_world_bin, hello_world_bin_size, APPLICATION_ADDRESS);
    }

    loader_port_reset_target();
}