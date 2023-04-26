/* Example of loading the program into RAM

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <sys/param.h>
#include <string.h>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_task_wdt.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp32_port.h"
#include "esp_loader.h"
#include "example_common.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "serial_ram_loader";

// This can be set to a higher baud rate, but because it takes some time to
// switch the uart baud rate in slave_monitor task, the log at slave starup
// time will be lost or garbled.
#define HIGHER_BAUDRATE 115200

// Max line size
#define BUF_LEN 128
static uint8_t buf[BUF_LEN] = {0};

void slave_monitor(void *arg)
{
#if (HIGHER_BAUDRATE != 115200)
    uart_flush_input(UART_NUM_1);
    uart_flush(UART_NUM_1);
    uart_set_baudrate(UART_NUM_1, 115200);
#endif
    while (1) {
        int rxBytes = uart_read_bytes(UART_NUM_1, buf, BUF_LEN, 100 / portTICK_PERIOD_MS);
        buf[rxBytes] = '\0';
        printf("%s", buf);
    }
}

void app_main(void)
{
    example_ram_app_binary_t bin;

    const loader_esp32_config_t config = {
        .baud_rate = 115200,
        .uart_port = UART_NUM_1,
        .uart_rx_pin = GPIO_NUM_5,
        .uart_tx_pin = GPIO_NUM_4,
        .reset_trigger_pin = GPIO_NUM_25,
        .gpio0_trigger_pin = GPIO_NUM_26,
    };

    if (loader_port_esp32_init(&config) != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "serial initialization failed.");
        abort();
    }

    if (connect_to_target(HIGHER_BAUDRATE) == ESP_LOADER_SUCCESS) {
        get_example_ram_app_binary(esp_loader_get_target(), &bin);
        ESP_LOGI(TAG, "Loading app to RAM ...");
        esp_loader_error_t err = load_ram_binary(bin.ram_app.data);
        if (err == ESP_LOADER_SUCCESS) {
            // Forward slave's serial output
            ESP_LOGI(TAG, "********************************************");
            ESP_LOGI(TAG, "*** Logs below are print from slave .... ***");
            ESP_LOGI(TAG, "********************************************");
            xTaskCreate(slave_monitor, "slave_monitor", 2048, NULL, configMAX_PRIORITIES, NULL);
        } else {
            ESP_LOGE(TAG, "Loading to ram failed ...");
        }
    }
    vTaskDelete(NULL);
}