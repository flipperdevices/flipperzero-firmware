/* Copyright 2020-2023 Espressif Systems (Shanghai) CO LTD
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

#include "catch.hpp"
#include "protocol.h"
#include "serial_io_mock.h"
#include "esp_loader.h"
#include "esp_loader_io.h"
#include <string.h>
#include <stdio.h>
#include <array>
#include <map>
#include <iostream>
#include <algorithm>

using namespace std;


#define TEST_SLIP_PACKET 0xdb, 'a', 'b', 'c', 0xc0, 0xdb, 'd', 'e', 0xc0, 'f', 0xdb

#define SLIP_ENCODED_PACKET 0xdb, 0xdd, 'a', 'b', 'c', 0xdb, 0xdc, 0xdb, \
                            0xdd, 'd', 'e', 0xdb, 0xdc, 'f', 0xdb, 0xdd

#define REQUIRE_SUCCESS(exp) REQUIRE( (exp) == ESP_LOADER_SUCCESS )

// Helper function for debugging.
__attribute__((unused))
static void arrays_match(void *array_1, void *array_2, size_t size)
{
    int8_t *arr_1 = (int8_t *)array_1;
    int8_t *arr_2 = (int8_t *)array_2;

    for (size_t i = 0; i < size; i++) {
        if (arr_1[i] != arr_2[i]) {
            printf("\nArrays do NOT match on index: %lu, with values %02hhx, %02hhx \n",
                   i, arr_1[i], arr_2[i]);

            printf("\nExpected: ");
            for (uint32_t j = 0; j < size; j++) { printf("%02hhx ", arr_1[j]); }
            printf("\nActual:   ");
            for (uint32_t j = 0; j < size; j++) { printf("%02hhx ", arr_2[j]); }

            return;
        }
    }
    printf("Arrays Match\n");
}


// Wrapper around command_t for convenience
struct __attribute__((packed)) expected_response {
    expected_response(command_t cmd)
    {
        data.common.direction = READ_DIRECTION;
        data.common.command = cmd;
        data.common.size = 16;
        data.common.value = 0;
        data.status.failed = STATUS_SUCCESS;
        data.status.error = 0;
    }

    response_t data;
};

static_assert(sizeof(expected_response) == sizeof(response_t), "Size NOT equal");

inline void queue_response(expected_response &response, size_t size = sizeof(expected_response))
{
    set_read_buffer(&response, size);
}

expected_response set_params_response(SPI_SET_PARAMS);
expected_response flash_begin_response(FLASH_BEGIN);
expected_response flash_data_response(FLASH_DATA);
expected_response flash_end_response(FLASH_END);
expected_response write_reg_response(WRITE_REG);
expected_response read_reg_response(READ_REG);
expected_response attach_response(SPI_ATTACH);
expected_response sync_response(SYNC);

const uint32_t reg_address = 0x1000;
const uint32_t reg_value = 55;

struct __attribute__((packed)) write_reg_cmd_response {
    uint8_t delimiter_1 = 0xc0;
    write_reg_command_t write_reg_cmd  = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = WRITE_REG,
            .size = 16,
            .checksum = 0
        },
        .address = reg_address,
        .value = reg_value,
        .mask = 0xFFFFFFFF,
        .delay_us = 0
    };
    uint8_t delimiter_2 = 0xc0;
};

map<target_chip_t, uint32_t> chip_magic_value = {
    {ESP8266_CHIP,  0xfff0c101},
    {ESP32_CHIP,    0x00f01d83},
    {ESP32S2_CHIP,  0x000007c6},
    {ESP32C2_CHIP,  0x6f51306f},
    {ESP32C3_CHIP,  0x6921506f},
    {ESP32S3_CHIP,  0x00000009},
    {ESP32H4_CHIP,  0xca26cc22},
    {ESP32H2_CHIP,  0xd7b73e80},
};

void queue_connect_response(target_chip_t target = ESP32_CHIP, uint32_t magic_value = 0)
{
    // Set magic value register used for detection of attached chip
    auto magic_value_response = read_reg_response;
    magic_value_response.data.common.value = magic_value ? magic_value : chip_magic_value[target];
    clear_buffers();
    queue_response(sync_response);
    queue_response(magic_value_response);

    if (target == ESP8266_CHIP) {
        queue_response(flash_begin_response);
    } else {
        auto efuse_pin_config_reg_1 = read_reg_response;
        auto efuse_pin_config_reg_2 = read_reg_response;

        queue_response(efuse_pin_config_reg_1);
        queue_response(efuse_pin_config_reg_2);
        queue_response(attach_response);
    }

}

TEST_CASE( "chip_magic_value contains all supported chips " )
{
    REQUIRE( chip_magic_value.size() ==  ESP_MAX_CHIP );
}

TEST_CASE( "Can connect within specified time " )
{
    queue_connect_response();

    esp_loader_connect_args_t connect_config = {
        .sync_timeout = 10,
        .trials = 1
    };

    SECTION( "Can connect" ) {
        serial_set_time_delay(5);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
    }

    SECTION( "Timeout error is returned when timeout expires" ) {
        serial_set_time_delay(20);
        REQUIRE ( esp_loader_connect(&connect_config) == ESP_LOADER_ERROR_TIMEOUT );
    }

    SECTION( "Can connect after several trials within specified time" ) {
        connect_config.trials = 5;
        serial_set_time_delay(40);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );

        serial_set_time_delay(60);
        REQUIRE( esp_loader_connect(&connect_config) == ESP_LOADER_ERROR_TIMEOUT );
    }

    serial_set_time_delay(0);
}


TEST_CASE( "Can detect attached target" )
{
    esp_loader_connect_args_t connect_config = ESP_LOADER_CONNECT_DEFAULT();

    SECTION( "Can detect ESP32" ) {
        queue_connect_response(ESP32_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32_CHIP );
    }

    SECTION( "Can detect ESP32S2" ) {
        queue_connect_response(ESP32S2_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32S2_CHIP );
    }

    SECTION( "Can detect ESP32C2" ) {
        queue_connect_response(ESP32C2_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32C2_CHIP );
    }

    SECTION( "Can detect ESP32H4" ) {
        queue_connect_response(ESP32H4_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32H4_CHIP );
    }

    SECTION( "Can detect ESP32H2" ) {
        queue_connect_response(ESP32H2_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32H2_CHIP );
    }

    SECTION( "Can detect ESP32C3" ) {
        queue_connect_response(ESP32C3_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32C3_CHIP );
    }

    SECTION( "Can detect ESP32C3 rev3" ) {
        queue_connect_response(ESP32C3_CHIP, 0x1b31506f);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32C3_CHIP );
    }

    SECTION( "Can detect ESP32S3" ) {
        queue_connect_response(ESP32S3_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP32S3_CHIP );
    }

    SECTION( "Can detect ESP8266" ) {
        queue_connect_response(ESP8266_CHIP);
        REQUIRE_SUCCESS( esp_loader_connect(&connect_config) );
        REQUIRE( esp_loader_get_target() == ESP8266_CHIP );
    }

    SECTION( "Can detect unknown chip" ) {
        queue_connect_response(ESP_UNKNOWN_CHIP, 0xaaaaaaaa);
        REQUIRE( esp_loader_connect(&connect_config) == ESP_LOADER_ERROR_INVALID_TARGET);
    }
}

TEST_CASE( "Register can be read correctly" )
{
    clear_buffers();
    uint32_t reg_value = 0;
    read_reg_response.data.common.value = 55;

    queue_response(read_reg_response);

    REQUIRE_SUCCESS( esp_loader_read_register(0, &reg_value) );

    REQUIRE( reg_value == 55 );
}


TEST_CASE( "Register can be written correctly" )
{
    write_reg_cmd_response expected;
    write_reg_response.data.common.value = 55;

    clear_buffers();
    queue_response(write_reg_response);

    REQUIRE_SUCCESS( esp_loader_write_register(reg_address, reg_value) );

    REQUIRE( memcmp(write_buffer_data(), &expected, sizeof(expected)) == 0 );
}

// --------------------  Serial comm test  -----------------------

TEST_CASE ( "SLIP is encoded correctly" )
{
    loader_flash_begin_cmd(0, 0, 0, 0, ESP32_CHIP); // To reset sequence number counter

    uint8_t data[] = { TEST_SLIP_PACKET };

    uint8_t expected[] = {
        0xc0,       // Begin
        0x00,         // Write direction
        0x03,         // FLASH_DATA command
        16 + sizeof(data), 0, // Number of characters to send
        0x33, 0, 0, 0,// Checksum
        sizeof(data), 0, 0, 0, // Data size
        0, 0, 0, 0,   // Sequence number
        0, 0, 0, 0,   // zero
        0, 0, 0, 0,   // zero
        SLIP_ENCODED_PACKET,
        0xc0,       // End
    };

    // write_buffer_print();
    // print_array(expected, sizeof(expected));

    clear_buffers();
    queue_response(flash_data_response);

    REQUIRE_SUCCESS( loader_flash_data_cmd(data, sizeof(data)) );

    REQUIRE( memcmp(write_buffer_data(), expected, sizeof(expected)) == 0 );
}


TEST_CASE( "Sync command is constructed correctly" )
{
    uint8_t expected[] = {
        0xc0,         // Begin
        0x00,         // Write direction
        0x08,         // SYNC command
        36, 0,        // Number of characters to send
        0, 0, 0, 0,   // Checksum is ignored for this command
        0x07, 0x07, 0x12, 0x20,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0xc0,         // End
    };

    clear_buffers();
    queue_response(sync_response);

    REQUIRE_SUCCESS( loader_sync_cmd() );

    REQUIRE( memcmp(write_buffer_data(), expected, sizeof(expected)) == 0 );
}

TEST_CASE( "Register can be read and decoded correctly" )
{
    clear_buffers();
    read_reg_response.data.common.value = 55;
    queue_response(read_reg_response);

    uint32_t reg_value = 0;
    esp_loader_read_register(0, &reg_value);

    REQUIRE( reg_value == 55 );
}

TEST_CASE( "Received response (in SLIP format) is decoded correctly" )
{
    clear_buffers();
    read_reg_response.data.common.value = 0xC0BD; // C0, BD has to be replaced
    queue_response(read_reg_response);

    uint32_t reg_value = 0;
    esp_loader_read_register(0, &reg_value);

    REQUIRE( reg_value == 0xC0BD );
}


// --------------------  Serial mock test  -----------------------

TEST_CASE( "Serial read works correctly" )
{
    uint32_t reg_value = 5;
    uint8_t readout[sizeof(reg_value) + 2];
    uint8_t expected[] = { 0xc0, 5, 0, 0, 0, 0xc0 };

    clear_buffers();

    set_read_buffer(&reg_value, sizeof(reg_value));

    SECTION( "Read buffer can be read" ) {
        loader_port_read(readout, sizeof(readout), 0);
        REQUIRE( memcmp(readout, expected, sizeof(readout)) == 0 );
    }

    SECTION ( "Read buffer can be read in smaller chunks" ) {
        loader_port_read(&readout[0], 3, 0);
        loader_port_read(&readout[3], 3, 0);
        REQUIRE( memcmp(readout, expected, sizeof(readout)) == 0 );
    }

    SECTION ( "Timeout is returned when requested amount of data is not available" ) {
        REQUIRE( loader_port_read(readout, sizeof(readout) + 1, 0) == ESP_LOADER_ERROR_TIMEOUT);
    }

    SECTION ( "Read buffer is correctly SLIP encoded " ) {
        uint8_t data_to_encode[] = { TEST_SLIP_PACKET };
        uint8_t expected[] = { 0xc0, SLIP_ENCODED_PACKET, 0xc0};
        uint8_t encoded[sizeof(expected)];

        clear_buffers();

        fill(encoded, &encoded[sizeof(encoded)], 0);
        set_read_buffer(data_to_encode, sizeof(data_to_encode));
        loader_port_read(encoded, sizeof(encoded), 0);

        REQUIRE( memcmp(expected, encoded, sizeof(expected)) == 0 );
    }
}
