/* Example of loading the program into RAM through SPI

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
#include "driver/spi_master.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp32_spi_port.h"
#include "esp_loader.h"
#include "example_common.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "spi_ram_loader";

// Max line size
#define BUF_LEN 128
static uint8_t buf[BUF_LEN] = {0};

void slave_monitor(void *arg)
{
    // Initialize UART
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 0, 0)
        .source_clk = UART_SCLK_DEFAULT,
#endif
    };

    ESP_ERROR_CHECK(uart_param_config(UART_NUM_2, &uart_config));

    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_2, GPIO_NUM_6, GPIO_NUM_7,
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_2, BUF_LEN * 4, BUF_LEN * 4, 0, NULL, 0));

    while (1) {
        int rxBytes = uart_read_bytes(UART_NUM_2, buf, BUF_LEN, 100 / portTICK_PERIOD_MS);
        buf[rxBytes] = '\0';
        printf("%s", buf);
    }
}

void app_main(void)
{
    example_ram_app_binary_t bin;

    const loader_esp32_spi_config_t config = {
        .spi_bus = SPI2_HOST,
        .frequency = 20 * 1000000,
        .reset_trigger_pin = GPIO_NUM_5,
        .spi_clk_pin = GPIO_NUM_12,
        .spi_cs_pin = GPIO_NUM_10,
        .spi_miso_pin = GPIO_NUM_13,
        .spi_mosi_pin = GPIO_NUM_11,
        .spi_quadwp_pin = GPIO_NUM_14,
        .spi_quadhd_pin = GPIO_NUM_9,
        .strap_bit0_pin = GPIO_NUM_13,
        .strap_bit1_pin = GPIO_NUM_2,
        .strap_bit2_pin = GPIO_NUM_3,
        .strap_bit3_pin = GPIO_NUM_4,
    };

    if (loader_port_esp32_spi_init(&config) != ESP_LOADER_SUCCESS) {
        ESP_LOGE(TAG, "SPI initialization failed.");
        abort();
    }

    if (connect_to_target(0) == ESP_LOADER_SUCCESS) {
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
            ESP_LOGE(TAG, "Loading to RAM failed ...");
        }
    }
    vTaskDelete(NULL);
}