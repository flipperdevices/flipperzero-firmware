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

#include "serial_io.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_log.h"
#include <unistd.h>

// #define SERIAL_DEBUG_ENABLE

#define UART_TXD_PIN        GPIO_NUM_4
#define UART_RXD_PIN        GPIO_NUM_5
#define UART_PORT           UART_NUM_1

#define RESET_TRIGER_PIN    GPIO_NUM_25
#define GPIO0_TRIGER_PIN    GPIO_NUM_26

#ifdef SERIAL_DEBUG_ENABLE
void serial_debug_print(const uint8_t *data, uint16_t size)
{
    ESP_LOG_BUFFER_HEX("LOADER ESP32 PORT", data, size);
}
#else
void serial_debug_print(const uint8_t *data, uint16_t size) { }
#endif

static int64_t s_time_end;


esp_loader_error_t loader_port_serial_init(uint32_t baud_rate)
{
    // Initialize UART
    uart_config_t uart_config = {
        .baud_rate = baud_rate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };

    const int rx_buffer_size = 2 * 200;
    const int tx_buffer_size = 2 * 200;

    if ( uart_param_config(UART_PORT, &uart_config) != ESP_OK ) {
        return ESP_LOADER_ERROR_FAIL;
    }
    if ( uart_set_pin(UART_PORT, UART_TXD_PIN, UART_RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE) != ESP_OK ) {
        return ESP_LOADER_ERROR_FAIL;
    }
    if ( uart_driver_install(UART_PORT, rx_buffer_size, tx_buffer_size, 0, NULL, 0) != ESP_OK ) {
        return ESP_LOADER_ERROR_FAIL;
    }

    // Initialize boot pin selection pins
    gpio_pad_select_gpio(RESET_TRIGER_PIN);
    gpio_set_pull_mode(RESET_TRIGER_PIN, GPIO_PULLUP_ONLY);
    gpio_set_direction(RESET_TRIGER_PIN, GPIO_MODE_OUTPUT);

    gpio_pad_select_gpio(GPIO0_TRIGER_PIN);
    gpio_set_pull_mode(GPIO0_TRIGER_PIN, GPIO_PULLUP_ONLY);
    gpio_set_direction(GPIO0_TRIGER_PIN, GPIO_MODE_OUTPUT);

    return ESP_LOADER_SUCCESS;
}


esp_loader_error_t loader_port_serial_write(const uint8_t *data, uint16_t size, uint32_t timeout)
{
    serial_debug_print(data, size);

    uart_write_bytes(UART_PORT, (const char *)data, size);
    esp_err_t err = uart_wait_tx_done(UART_PORT, pdMS_TO_TICKS(timeout));
    
    if (err == ESP_OK) {
        return ESP_LOADER_SUCCESS;
    } else if (err == ESP_ERR_TIMEOUT) {
        return ESP_LOADER_ERROR_TIMEOUT;
    } else {
        return ESP_LOADER_ERROR_FAIL;
    }
}


esp_loader_error_t loader_port_serial_read(uint8_t *data, uint16_t size, uint32_t timeout)
{
    int read = uart_read_bytes(UART_PORT, data, size, pdMS_TO_TICKS(timeout));
    
    if (read < 0) {
        return ESP_LOADER_ERROR_FAIL;
    } else if (read < size) {
        return ESP_LOADER_ERROR_TIMEOUT;
    } else {
        return ESP_LOADER_SUCCESS;
    }
}


// Set GPIO0 LOW, then
// assert reset pin for 50 milliseconds.
void loader_port_enter_bootloader(void)
{
    gpio_set_level(GPIO0_TRIGER_PIN, 0);
    gpio_set_level(RESET_TRIGER_PIN, 0);
    gpio_set_level(RESET_TRIGER_PIN, 1);
    loader_port_delay_ms(50);
    gpio_set_level(GPIO0_TRIGER_PIN, 1);
}


void loader_port_reset_target(void)
{
    gpio_set_level(RESET_TRIGER_PIN, 0);
    loader_port_delay_ms(50);
    gpio_set_level(RESET_TRIGER_PIN, 1);
}


void loader_port_delay_ms(uint32_t ms)
{
    usleep(ms * 1000);
}


void loader_port_start_timer(uint32_t ms)
{
    s_time_end = esp_timer_get_time() + ms * 1000;
}


uint32_t loader_port_remaining_time(void)
{
    int64_t remaining = (s_time_end - esp_timer_get_time()) / 1000;
    return (remaining > 0) ? (uint32_t)remaining : 0;
}


void loader_port_debug_print(const char *str)
{
    printf("DEBUG: %s\n", str);
}

esp_loader_error_t loader_port_change_baudrate(uint32_t baudrate)
{
    esp_err_t err = uart_set_baudrate(UART_PORT, baudrate);
    return (err == ESP_OK) ? ESP_LOADER_SUCCESS : ESP_LOADER_ERROR_FAIL;
}