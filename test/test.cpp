/* Copyright 2018 Espressif Systems (Shanghai) PTE LTD
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
#include "serial_io.h"
#include "loader.h"
#include <string.h>
#include <stdio.h>
#include <iostream>
#include <algorithm>

using namespace std;

#define TEST_SLIP_PACKET '\xdb', 'a', 'b', 'c', '\xc0', '\xdb', \
                         'd', 'e', '\xc0', 'f', '\xdb'

#define SLIP_ENCODED_PACKET '\xdb', '\xdd', 'a', 'b', 'c', '\xdb', '\xdc', \
                            '\xdb', '\xdd', 'd', 'e', '\xdb', '\xdc', 'f', \
                            '\xdb', '\xdd'

// Wrapper around command_t for easyer
struct __attribute__((packed)) expected_response {
    expected_response(command_t cmd)
    {
        data.direction = READ_DIRECTION;
        data.command = cmd;
        data.size = 16;
        data.value = 0;
        data.status = STATUS_SUCCESS;
        data.error = 0;
        data.reserved_0 = 0;
        data.reserved_1 = 0;
    }

    response_t data;
};

static_assert(sizeof(expected_response) == sizeof(response_t), "Size NOT equel");


inline void queue_response(expected_response &response, size_t size = sizeof(expected_response))
{
    set_read_buffer(&response, size);
}

expected_response flash_begin_response(FLASH_BEGIN);
expected_response flash_data_response(FLASH_DATA);
expected_response flash_end_response(FLASH_END);
expected_response write_reg_response(WRITE_REG);
expected_response read_reg_response(READ_REG);
expected_response sync_response(SYNC);

template<size_t PAYLOAD_SIZE>
struct __attribute__((packed)) data_frame {
    int8_t delimiter_1 = '\xc0';
    data_command_t data_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_DATA,
            .size = 16,
            .checksum = 0xef,
        },
        .data_size = PAYLOAD_SIZE,
        .sequence_number = 0,
        .zero_0 = 0,
        .zero_1 = 0,
    };
    uint8_t payload[PAYLOAD_SIZE];
    int8_t delimiter_2 = '\xc0';
};

template<size_t DATA_FRAME_SIZE, size_t NUM_OF_DATA_FRAMES>
struct __attribute__((packed)) write_packet {
    int8_t delimiter_1 = '\xc0';
    begin_command_t begin_cmd  = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_BEGIN,
            .size = 16,
            .checksum = 0,
        },
        .erase_size = DATA_FRAME_SIZE * 3,
        .packet_count = 3,
        .packet_size = DATA_FRAME_SIZE,
        .offset = 0,
    };
    int8_t delimiter_2 = '\xc0';

    data_frame<DATA_FRAME_SIZE> data[NUM_OF_DATA_FRAMES];

    int8_t delimiter_3 = '\xc0';
    flash_end_command_t end_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_END,
            .size = 4,
            .checksum = 0,
        },
        .stay_in_loader = 1,
    };
    int8_t delimiter_4 = '\xc0';

    write_packet()
    {
        for (size_t i = 0; i < NUM_OF_DATA_FRAMES; i++) {
            memset(data[i].payload, 0xFF, FLASH_WRITE_SIZE); // Padding
            data[i].data_cmd.data_size = DATA_FRAME_SIZE;
            data[i].data_cmd.sequence_number = i;
        }
    }
};

int8_t large_data[2500];

TEST_CASE( "Large payload that does not fit FLASH_WRITE_SIZE is split into \
            multiple data frames. Last data frame is padded with 0xFF" )
{
    write_packet<FLASH_WRITE_SIZE, 3> expected;
    uint32_t remaining_size = sizeof(large_data) - 2 * FLASH_WRITE_SIZE;

    memset(large_data, 0x11, sizeof(large_data));
    memcpy(&expected.data[0].payload, large_data, FLASH_WRITE_SIZE);
    memcpy(&expected.data[1].payload, large_data, FLASH_WRITE_SIZE);
    memcpy(&expected.data[2].payload, large_data, remaining_size);
    expected.data[2].data_cmd.data_size = remaining_size;

    clear_buffers();
    queue_response(flash_begin_response);
    queue_response(flash_data_response);
    queue_response(flash_data_response);
    queue_response(flash_data_response);
    queue_response(flash_end_response);

    REQUIRE( loader_flash_write(large_data, sizeof(large_data), 0) == ESP_SUCCESS );

    REQUIRE( memcmp(write_buffer_data(), &expected, sizeof(expected)) == 0 );
}


TEST_CASE( "Can connect within specified time " )
{
    clear_buffers();
    queue_response(sync_response);

    loader_set_timeout(50);

    SECTION( "Can connect" ) {
        serial_set_time_delay(100);
        REQUIRE ( loader_connect() == ESP_SUCCESS );
    }

    SECTION( "Timeout error is returned when timeout expires" ) {
        serial_set_time_delay(1000);
        REQUIRE ( loader_connect() == ESP_ERROR_TIMEOUT );
    }

    serial_set_time_delay(0);
}

TEST_CASE( "Register can be read correctly" )
{
    clear_buffers();
    read_reg_response.data.value = 55;
    queue_response(read_reg_response);

    uint32_t reg_value = 0;
    loader_read_register(0, &reg_value);

    REQUIRE( reg_value == 55 );
}


const uint32_t reg_address = 0x1000;
const uint32_t reg_value = 55;

struct __attribute__((packed)) write_reg_cmd_response {
    int8_t delimiter_1 = '\xc0';
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
    int8_t delimiter_2 = '\xc0';
};


TEST_CASE( "Register can be written correctly" )
{
    write_reg_cmd_response expected;

    clear_buffers();
    queue_response(write_reg_response);

    REQUIRE( loader_write_register(reg_address, reg_value) == ESP_SUCCESS );

    REQUIRE( memcmp(write_buffer_data(), &expected, sizeof(expected)) == 0 );
}

// --------------------  Serial comm test  -----------------------

TEST_CASE ( "SLIP is enceded correctly" )
{
    flash_begin_cmd(0, 0, 0, 0); // To reset sequence number counter

    int8_t data[] = { TEST_SLIP_PACKET };

    int8_t expected[] = {
        '\xc0',       // Begin
        0x00,         // Write direction
        0x03,         // FLASH_DATA command
        16, 0,        // Number of characters to send
        0x33, 0, 0, 0,// Checksum
        sizeof(data), 0, 0, 0, // Data size
        0, 0, 0, 0,   // Sequence number
        0, 0, 0, 0,   // zero
        0, 0, 0, 0,   // zero
        SLIP_ENCODED_PACKET,
        '\xc0',       // End
    };

    // write_buffer_print();
    // print_array(expected, sizeof(expected));

    clear_buffers();
    queue_response(flash_data_response);

    REQUIRE( flash_data_cmd(data, sizeof(data), 0) == ESP_SUCCESS );

    REQUIRE( memcmp(write_buffer_data(), expected, sizeof(expected)) == 0 );
}


TEST_CASE( "Synd command is constructed correctly" )
{
    int8_t expected[] = {
        '\xc0',       // Begin
        0x00,         // Write direction
        0x08,         // SYNC command
        36, 0,        // Number of characters to send
        0, 0, 0, 0,   // Checksum is ignored for this command
        0x07, 0x07, 0x12, 0x20,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        '\xc0',       // End
    };

    clear_buffers();
    queue_response(sync_response);

    REQUIRE( sync_cmd() == ESP_SUCCESS );

    REQUIRE( memcmp(write_buffer_data(), expected, sizeof(expected)) == 0 );
}


// TEST_CASE( "Register can be read and decoded correctly" )
// {
//     clear_buffers();
//     read_reg_response.data.value = 55;
//     queue_response(read_reg_response);

//     uint32_t reg_value = 0;
//     read_reg(0, &reg_value);

//     REQUIRE( reg_value == 55 );
// }



// Test flash offset




// --------------------  Serial mock test  -----------------------

TEST_CASE( "Serial read works correctly" )
{
    uint32_t reg_value = 5;
    int8_t readout[sizeof(reg_value) + 2];
    int8_t expected[] = { '\xc0', 5, 0, 0, 0, '\xc0' };

    clear_buffers();

    set_read_buffer(&reg_value, sizeof(reg_value));

    SECTION( "Read buffer can be read" ) {
        serial_read(readout, sizeof(readout), 0);
        REQUIRE( memcmp(readout, expected, sizeof(readout)) == 0 );
    }

    SECTION ( "Read buffer can be read in smaller chunks" ) {
        serial_read(&readout[0], 3, 0);
        serial_read(&readout[3], 3, 0);
        REQUIRE( memcmp(readout, expected, sizeof(readout)) == 0 );
    }

    SECTION ( "Timeout is retuned when requested amount of data is not available" ) {
        REQUIRE( serial_read(readout, sizeof(readout) + 1, 0) == ESP_ERROR_TIMEOUT);
    }

    SECTION ( "Read buffer is correctly SLIP encoded " ) {
        int8_t data_to_encode[] = { TEST_SLIP_PACKET };
        int8_t expected[] = { '\xc0', SLIP_ENCODED_PACKET, '\xc0'};
        int8_t encoded[sizeof(expected)];

        clear_buffers();

        fill(encoded, &encoded[sizeof(encoded)], 0);
        set_read_buffer(data_to_encode, sizeof(data_to_encode));
        serial_read(encoded, sizeof(encoded), 0);

        REQUIRE( memcmp(expected, encoded, sizeof(expected)) == 0 );
    }
}






// static void arrays_match(int8_t *array_1, int8_t *array_2, size_t size)
// {
//     for (size_t i = 0; i < size; i++) {
//         if (array_1[i] != array_2[i]) {
//             printf("\nArrays do NOT match on index: %lu, with vaules %0x, %0x \n",
//                    i, array_1[i], array_2[i]);
//             return;
//         }
//     }

//     printf("Arrays Match\n");
// }


// int8_t expected[] = {
//     '\xc0',       // Begin
//     0x00,         // Write direction
//     0x02,         // FLASH_BEGIN command
//     16, 0,        // Number of characters to send
//     0, 0, 0, 0,   // Checksum is ignored for this command
//     sizeof(data), 0, 0, 0, // Size to erase
//     1, 0, 0, 0,   // Number of data packets
//     0, 4, 0, 0,   // Data size in one packet
//     0, 0, 0, 0,   // Flash offset
//     '\xc0',       // End

//     '\xc0',       // Begin
//     0x00,         // Write direction
//     0x03,         // FLASH_DATA command
//     16, 0,        // Number of characters to send
//     0x33, 0, 0, 0,// Checksum
//     sizeof(data), 0, 0, 0, // Data size
//     0, 0, 0, 0,   // Sequence number
//     0, 0, 0, 0,   // zero
//     0, 0, 0, 0,   // zero
//     SLIP_ENCODED_PACKET,
//     '\xc0',       // End

//     '\xc0',       // Begin
//     0x00,         // Write direction
//     0x04,         // FLASH_END command
//     4, 0,         // Number of characters to send
//     0, 0, 0, 0,   // Checksum is ignored for this command
//     1, 0, 0, 0,   // Do not reboot (1)
//     '\xc0',       // End
// };


// write_packet expected = {
//     // Begin command
//     .delimiter_1 = '\xc0',
//     .begin_cmd = {
//         .common = {
//             .direction = WRITE_DIRECTION,
//             .command = FLASH_BEGIN,
//             .size = 16,
//             .checksum = 0,
//         },
//         .erase_size = FLASH_WRITE_SIZE * 3,
//         .packet_count = 3,
//         .packet_size = FLASH_WRITE_SIZE,
//         .offset = 0,
//     },
//     .delimiter_2 = '\xc0',

//     // Data frames
//     .data = {
//         {
//             .delimiter_3 = '\xc0',
//             .data_cmd = {
//                 .common = {
//                     .direction = WRITE_DIRECTION,
//                     .command = FLASH_DATA,
//                     .size = 16,
//                     .checksum = 0xef,
//                 },
//                 .data_size = FLASH_WRITE_SIZE,
//                 .sequence_number = 0,
//                 .zero_0 = 0,
//                 .zero_1 = 0,
//             },
//             .payload = { 0xFF },
//             .delimiter_4 = '\xc0',
//         },

//         {
//             .delimiter_3 = '\xc0',
//             .data_cmd = {
//                 .common = {
//                     .direction = WRITE_DIRECTION,
//                     .command = FLASH_DATA,
//                     .size = 16,
//                     .checksum = 0xef,
//                 },
//                 .data_size = FLASH_WRITE_SIZE,
//                 .sequence_number = 1,
//                 .zero_0 = 0,
//                 .zero_1 = 0,
//             },
//             .payload = { 0xFF },
//             .delimiter_4 = '\xc0',
//         },

//         {
//             .delimiter_3 = '\xc0',
//             .data_cmd = {
//                 .common = {
//                     .direction = WRITE_DIRECTION,
//                     .command = FLASH_DATA,
//                     .size = 16,
//                     .checksum = 0xef,
//                 },
//                 .data_size = remaining_size,
//                 .sequence_number = 2,
//                 .zero_0 = 0,
//                 .zero_1 = 0,
//             },
//             .payload = { 0xFF },
//             .delimiter_4 = '\xc0',
//         },
//     },

//     // End command
//     .delimiter_5 = '\xc0',
//     .end_cmd = {
//         .common = {
//             .direction = WRITE_DIRECTION,
//             .command = FLASH_END,
//             .size = 4,
//             .checksum = 0,
//         },
//         .stay_in_loader = 1,

//     },
//     .delimiter_6 = '\xc0',
// };
