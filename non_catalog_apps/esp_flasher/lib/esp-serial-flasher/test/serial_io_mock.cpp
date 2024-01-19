/* Copyright 2018-2023 Espressif Systems (Shanghai) CO LTD
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

#include <limits>
#include <vector>
#include <iterator>
#include <algorithm>
#include <iostream>
#include <stdio.h>
#include "esp_loader_io.h"
#include "serial_io_mock.h"

using namespace std;

static vector<int8_t> write_buffer;
static vector<int8_t> read_buffer;
static uint32_t receive_delay = 0;
static int32_t timer = 0;


esp_loader_error_t loader_port_mock_init(const loader_serial_config_t *config)
{
    return ESP_LOADER_SUCCESS;
}

void loader_port_mock_deinit()
{

}

esp_loader_error_t loader_port_write(const uint8_t *data, uint16_t size, uint32_t timeout)
{
    copy(&data[0], &data[size], back_inserter(write_buffer));

    return ESP_LOADER_SUCCESS;
}

esp_loader_error_t loader_port_read(uint8_t *data, uint16_t size, uint32_t timeout)
{
    if (read_buffer.size() < size) {
        return ESP_LOADER_ERROR_TIMEOUT;
    }

    if (receive_delay != 0 && timeout != 0) {
        if (receive_delay > timeout) {
            receive_delay -= timeout;
            return ESP_LOADER_ERROR_TIMEOUT;
        }
        receive_delay = 0;
    }

    copy_n(read_buffer.begin(), size, data);
    read_buffer.erase(read_buffer.begin(), read_buffer.begin() + size);

    return ESP_LOADER_SUCCESS;
}

void loader_port_enter_bootloader()
{
    // GPIO0 and GPIO2 must be LOW
    // Then Reset
}

void loader_port_reset_target()
{

}

void loader_port_delay_ms(uint32_t ms)
{

}


void loader_port_start_timer(uint32_t ms)
{
    timer = (int32_t)ms;
}


uint32_t loader_port_remaining_time(void)
{
    return (timer > 0) ? timer : 0;
}



// ----------  For testing purposes only  ----------

static void SLIP_encode(const int8_t *in_buff, size_t size, vector<int8_t> &encoded_buff)
{
    encoded_buff.push_back('\xc0');

    for (uint32_t i = 0; i < size; i++) {
        if (in_buff[i] == '\xc0') {
            encoded_buff.push_back('\xdb');
            encoded_buff.push_back('\xdc');
        } else if (in_buff[i] == '\xdb') {
            encoded_buff.push_back('\xdb');
            encoded_buff.push_back('\xdd');
        } else {
            encoded_buff.push_back(in_buff[i]);
        }
    }

    encoded_buff.push_back('\xc0');
}

void clear_buffers()
{
    write_buffer.clear();
    read_buffer.clear();
}

int8_t *write_buffer_data()
{
    return write_buffer.data();
}

size_t write_buffer_size()
{
    return write_buffer.size();
}

void set_read_buffer(const void *data, size_t size)
{
    SLIP_encode((const int8_t *)data, size, read_buffer);
}

void print_array(int8_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < size; i++) {
        printf("%02x, ", (uint8_t)data[i]);
    }
    printf("\n");
}

void write_buffer_print()
{
    print_array(write_buffer.data(), write_buffer.size());
}

void serial_set_time_delay(uint32_t miliseconds)
{
    receive_delay = miliseconds;
}