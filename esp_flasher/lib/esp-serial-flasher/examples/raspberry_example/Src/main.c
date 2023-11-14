/* Flash multiple partitions example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include "esp_loader.h"
#include "example_common.h"
#include "raspberry_port.h"

#define TARGET_RST_Pin 2
#define TARGET_IO0_Pin 3

#define DEFAULT_BAUD_RATE 115200
#define HIGHER_BAUD_RATE  460800
#define SERIAL_DEVICE     "/dev/ttyS0"

#define BINARY_PATH       "../../binaries/ESP32_AT_Firmware/Firmware.bin"


static void upload_file(const char *path, size_t address)
{
    char *buffer = NULL;

    FILE *image = fopen(path, "r");
    if (image == NULL) {
        printf("Error:Failed to open file %s\n", path);
        return;
    }

    fseek(image, 0L, SEEK_END);
    size_t size = ftell(image);
    rewind(image);

    printf("File %s opened. Size: %u bytes\n", path, size);

    buffer = (char *)malloc(size);
    if (buffer == NULL) {
        printf("Error: Failed allocate memory\n");
        goto cleanup;
    }

    // copy file content to buffer
    size_t bytes_read = fread(buffer, 1, size, image);
    if (bytes_read != size) {
        printf("Error occurred while reading file");
        goto cleanup;
    }

    flash_binary(buffer, size, address);

cleanup:
    fclose(image);
    free(buffer);
}

int main(void)
{
    const loader_raspberry_config_t config = {
        .device = SERIAL_DEVICE,
        .baudrate = DEFAULT_BAUD_RATE,
        .reset_trigger_pin = TARGET_RST_Pin,
        .gpio0_trigger_pin = TARGET_IO0_Pin,
    };

    loader_port_raspberry_init(&config);

    if (connect_to_target(HIGHER_BAUD_RATE) == ESP_LOADER_SUCCESS) {
        upload_file(BINARY_PATH, 0);
    }

    loader_port_reset_target();
}