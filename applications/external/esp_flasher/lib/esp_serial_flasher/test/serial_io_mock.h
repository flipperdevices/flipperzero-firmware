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

#pragma once

#include <stdint.h>
#include <stddef.h>
#include "esp_loader.h"

void clear_buffers();

void write_buffer_print();
size_t write_buffer_size();
int8_t* write_buffer_data();

void set_read_buffer(const void *data, size_t size);
void print_array(int8_t *data, uint32_t size);
void serial_set_time_delay(uint32_t miliseconds);


typedef struct {
    uint32_t dummy;
} loader_serial_config_t;

esp_loader_error_t loader_port_mock_init(const loader_serial_config_t *config);
void loader_port_mock_deinit();