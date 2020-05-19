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

#include "catch.hpp"
#include "serial_comm.h"
#include "serial_comm_prv.h"
#include "serial_io_mock.h"
#include "esp_loader.h"
#include "serial_io.h"
#include <string.h>
#include <stdio.h>
#include <array>
#include <iostream>
#include <algorithm>

using namespace std;


#define TEST_SLIP_PACKET 0xdb, 'a', 'b', 'c', 0xc0, 0xdb, 'd', 'e', 0xc0, 'f', 0xdb

#define SLIP_ENCODED_PACKET 0xdb, 0xdd, 'a', 'b', 'c', 0xdb, 0xdc, 0xdb, \
                            0xdd, 'd', 'e', 0xdb, 0xdc, 'f', 0xdb, 0xdd


// Helper function for debugging.  
__attribute__((unused))
static void arrays_match(int8_t *array_1, int8_t *array_2, size_t size)
{
    for (size_t i = 0; i < size; i++) {
        if (array_1[i] != array_2[i]) {
            printf("\nArrays do NOT match on index: %lu, with values %0x, %0x \n",
                   i, array_1[i], array_2[i]);
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
        data.status.reserved_0 = 0;
        data.status.reserved_1 = 0;
    }

    response_t data;
};

static_assert(sizeof(expected_response) == sizeof(response_t), "Size NOT equal");

inline void queue_response(expected_response &response, size_t size = sizeof(expected_response))
{
    set_read_buffer(&response, size);
}

expected_response flash_begin_response(FLASH_BEGIN);
expected_response flash_data_response(FLASH_DATA);
expected_response flash_end_response(FLASH_END);
expected_response write_reg_response(WRITE_REG);
expected_response read_reg_response(READ_REG);
expected_response attach_response(SPI_ATTACH);
expected_response sync_response(SYNC);


struct __attribute__((packed)) flash_start_frame {
    uint8_t delimiter_3 = 0xc0;
    begin_command_t begin_cmd  = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_BEGIN,
            .size = 16,
            .checksum = 0,
        },
        .erase_size = 0,
        .packet_count = 0,
        .packet_size = 0,
        .offset = 0,
    };
    uint8_t delimiter_4 = 0xc0;

    flash_start_frame(uint32_t offset, uint32_t image_size, uint32_t block_size)
    {
        uint32_t blocks_to_write = (image_size + block_size - 1) / block_size;

        begin_cmd.packet_count = blocks_to_write;
        begin_cmd.erase_size = blocks_to_write * block_size;
        begin_cmd.packet_size = block_size;
        begin_cmd.offset = offset;
    }
};


struct __attribute__((packed)) flash_finish_frame {
    uint8_t delimiter_1 = 0xc0;
    flash_end_command_t end_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_END,
            .size = 4,
            .checksum = 0,
        },
        .stay_in_loader = 1,
    };
    uint8_t delimiter_2 = 0xc0;
};


template<size_t PAYLOAD_SIZE>
struct __attribute__((packed)) flash_write_frame {
    uint8_t delimiter_1 = 0xc0;
    data_command_t data_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_DATA,
            .size = 16 + PAYLOAD_SIZE,
            .checksum = 0xef,
        },
        .data_size = PAYLOAD_SIZE,
        .sequence_number = 0,
        .zero_0 = 0,
        .zero_1 = 0,
    };
    array<uint8_t, PAYLOAD_SIZE> payload;
    uint8_t delimiter_2 = 0xc0;

    flash_write_frame()
    {
        payload.fill(0xFF);
        data_cmd.sequence_number = seq_num++;
    }

    ~flash_write_frame()
    {
        seq_num--;
    }

    void fill(uint8_t data, size_t size = PAYLOAD_SIZE)
    {
        fill_n(payload.data(), size, data);
    }

    static uint32_t seq_num;
};


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


template<size_t PAYLOAD_SIZE>
uint32_t flash_write_frame<PAYLOAD_SIZE>::seq_num = 0;



TEST_CASE( "Large payload that does not fit BLOCK_SIZE is split into \
            multiple data frames. Last data frame is padded with 0xFF" )
{
    const uint32_t BLOCK_SIZE = 1024;

    uint8_t data[BLOCK_SIZE];
    memset(data, 0x11, BLOCK_SIZE);

    flash_write_frame<BLOCK_SIZE> expected_data[3];
    expected_data[0].fill(0x11);
    expected_data[1].fill(0x11);
    expected_data[2].fill(0x11, 200);

    flash_start_frame expected_start(0, sizeof(data) * 3, BLOCK_SIZE);

    // Check flash start operation 
    clear_buffers();
    queue_response(flash_begin_response);

    REQUIRE ( esp_loader_flash_start(0, sizeof(data) * 3, BLOCK_SIZE) == ESP_LOADER_SUCCESS );
    
    REQUIRE( memcmp(write_buffer_data(), &expected_start, sizeof(expected_start)) == 0 );


    // Check flash write operation 
    clear_buffers();
    queue_response(flash_data_response);
    queue_response(flash_data_response);
    queue_response(flash_data_response);

    REQUIRE( esp_loader_flash_write(data, sizeof(data)) == ESP_LOADER_SUCCESS );
    REQUIRE( esp_loader_flash_write(data, sizeof(data)) == ESP_LOADER_SUCCESS );
    REQUIRE( esp_loader_flash_write(data, 200) == ESP_LOADER_SUCCESS );

    REQUIRE( memcmp(write_buffer_data(), &expected_data, sizeof(expected_data)) == 0 );

    // queue_response(flash_end_response);
}


TEST_CASE( "Can connect within specified time " )
{
    // Set date registers used for detection of attached chip
    auto uart_date_reg_1 = read_reg_response;
    auto uart_date_reg_2 = read_reg_response;
    uart_date_reg_1.data.common.value = 0x15122500;
    uart_date_reg_2.data.common.value = 0;

    clear_buffers();
    queue_response(sync_response);
    queue_response(uart_date_reg_1);
    queue_response(uart_date_reg_2);
    queue_response(attach_response);

    esp_loader_connect_args_t connect_config = {
        .sync_timeout = 10,
        .trials = 1
    };

    SECTION( "Can connect" ) {
        serial_set_time_delay(5);
        REQUIRE ( esp_loader_connect(&connect_config) == ESP_LOADER_SUCCESS );
    }

    SECTION( "Timeout error is returned when timeout expires" ) {
        serial_set_time_delay(20);
        REQUIRE ( esp_loader_connect(&connect_config) == ESP_LOADER_ERROR_TIMEOUT );
    }

    SECTION( "Can connect after several trials within specified time" ) {
        connect_config.trials = 5;
        serial_set_time_delay(40);
        REQUIRE ( esp_loader_connect(&connect_config) == ESP_LOADER_SUCCESS );

        serial_set_time_delay(60);
        REQUIRE ( esp_loader_connect(&connect_config) == ESP_LOADER_ERROR_TIMEOUT );
    }

    serial_set_time_delay(0);
}

TEST_CASE( "Register can be read correctly" )
{
    clear_buffers();
    uint32_t reg_value = 0;
    read_reg_response.data.common.value = 55;

    queue_response(read_reg_response);

    REQUIRE( esp_loader_read_register(0, &reg_value) == ESP_LOADER_SUCCESS );

    REQUIRE( reg_value == 55 );
}


TEST_CASE( "Register can be written correctly" )
{
    write_reg_cmd_response expected;
    write_reg_response.data.common.value = 55;

    clear_buffers();
    queue_response(write_reg_response);

    REQUIRE( esp_loader_write_register(reg_address, reg_value) == ESP_LOADER_SUCCESS );

    REQUIRE( memcmp(write_buffer_data(), &expected, sizeof(expected)) == 0 );
}

// --------------------  Serial comm test  -----------------------

TEST_CASE ( "SLIP is encoded correctly" )
{
    loader_flash_begin_cmd(0, 0, 0, 0); // To reset sequence number counter

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

    REQUIRE( loader_flash_data_cmd(data, sizeof(data)) == ESP_LOADER_SUCCESS );

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

    REQUIRE( loader_sync_cmd() == ESP_LOADER_SUCCESS );

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
        loader_port_serial_read(readout, sizeof(readout), 0);
        REQUIRE( memcmp(readout, expected, sizeof(readout)) == 0 );
    }

    SECTION ( "Read buffer can be read in smaller chunks" ) {
        loader_port_serial_read(&readout[0], 3, 0);
        loader_port_serial_read(&readout[3], 3, 0);
        REQUIRE( memcmp(readout, expected, sizeof(readout)) == 0 );
    }

    SECTION ( "Timeout is returned when requested amount of data is not available" ) {
        REQUIRE( loader_port_serial_read(readout, sizeof(readout) + 1, 0) == ESP_LOADER_ERROR_TIMEOUT);
    }

    SECTION ( "Read buffer is correctly SLIP encoded " ) {
        uint8_t data_to_encode[] = { TEST_SLIP_PACKET };
        uint8_t expected[] = { 0xc0, SLIP_ENCODED_PACKET, 0xc0};
        uint8_t encoded[sizeof(expected)];

        clear_buffers();

        fill(encoded, &encoded[sizeof(encoded)], 0);
        set_read_buffer(data_to_encode, sizeof(data_to_encode));
        loader_port_serial_read(encoded, sizeof(encoded), 0);

        REQUIRE( memcmp(expected, encoded, sizeof(expected)) == 0 );
    }
}