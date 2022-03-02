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

#include "serial_comm_prv.h"
#include "serial_comm.h"
#include "serial_io.h"
#include <stddef.h>
#include <string.h>

#define CMD_SIZE(cmd) ( sizeof(cmd) - sizeof(command_common_t) )

static uint32_t s_sequence_number = 0;

static const uint8_t DELIMITER = 0xC0;
static const uint8_t C0_REPLACEMENT[2] = {0xDB, 0xDC};
static const uint8_t DB_REPLACEMENT[2] = {0xDB, 0xDD};

static esp_loader_error_t check_response(command_t cmd, uint32_t *reg_value, void* resp, uint32_t resp_size);


static inline esp_loader_error_t serial_read(uint8_t *buff, size_t size)
{
    return loader_port_serial_read(buff, size, loader_port_remaining_time());
}

static inline esp_loader_error_t serial_write(const uint8_t *buff, size_t size)
{
    return loader_port_serial_write(buff, size, loader_port_remaining_time());
}

static uint8_t compute_checksum(const uint8_t *data, uint32_t size)
{
    uint8_t checksum = 0xEF;

    while (size--) {
        checksum ^= *data++;
    }

    return checksum;
}

static esp_loader_error_t SLIP_receive_data(uint8_t *buff, uint32_t size)
{
    uint8_t ch;

    for (uint32_t i = 0; i < size; i++) {
        RETURN_ON_ERROR( serial_read(&ch, 1) );

        if (ch == 0xDB) {
            RETURN_ON_ERROR( serial_read(&ch, 1) );
            if (ch == 0xDC) {
                buff[i] = 0xC0;
            } else if (ch == 0xDD) {
                buff[i] = 0xDB;
            } else {
                return ESP_LOADER_ERROR_INVALID_RESPONSE;
            }
        } else {
            buff[i] = ch;
        }
    }

    return ESP_LOADER_SUCCESS;
}


static esp_loader_error_t SLIP_receive_packet(uint8_t *buff, uint32_t size)
{
    uint8_t ch;

    // Wait for delimiter
    do {
        RETURN_ON_ERROR( serial_read(&ch, 1) );
    } while (ch != DELIMITER);

    // Workaround: bootloader sends two dummy(0xC0) bytes after response when baud rate is changed.
    do {
        RETURN_ON_ERROR( serial_read(&ch, 1) );
    } while (ch == DELIMITER);

    buff[0] = ch;

    RETURN_ON_ERROR( SLIP_receive_data(&buff[1], size - 1) );

    // Wait for delimiter
    do {
        RETURN_ON_ERROR( serial_read(&ch, 1) );
    } while (ch != DELIMITER);

    return ESP_LOADER_SUCCESS;
}


static esp_loader_error_t SLIP_send(const uint8_t *data, uint32_t size)
{
    uint32_t to_write = 0;  // Bytes ready to write as they are
    uint32_t written = 0;   // Bytes already written

    for (uint32_t i = 0; i < size; i++) {
        if (data[i] != 0xC0 && data[i] != 0xDB) {
            to_write++; // Queue this byte for writing
            continue;
        }

        // We have a byte that needs encoding, write the queue first
        if (to_write > 0) {
            RETURN_ON_ERROR( serial_write(&data[written], to_write) );
        }

        // Write the encoded byte
        if (data[i] == 0xC0) {
            RETURN_ON_ERROR( serial_write(C0_REPLACEMENT, 2) );
        } else {
            RETURN_ON_ERROR( serial_write(DB_REPLACEMENT, 2) );
        }

        // Update to start again after the encoded byte
        written = i + 1;
        to_write = 0;
    }

    // Write the rest of the bytes that didn't need encoding
    if (to_write > 0) {
        RETURN_ON_ERROR( serial_write(&data[written], to_write) );
    }

    return ESP_LOADER_SUCCESS;
}


static esp_loader_error_t SLIP_send_delimiter(void)
{
    return serial_write(&DELIMITER, 1);
}


static esp_loader_error_t send_cmd(const void *cmd_data, uint32_t size, uint32_t *reg_value)
{
    response_t response;
    command_t command = ((command_common_t *)cmd_data)->command;

    RETURN_ON_ERROR( SLIP_send_delimiter() );
    RETURN_ON_ERROR( SLIP_send((const uint8_t *)cmd_data, size) );
    RETURN_ON_ERROR( SLIP_send_delimiter() );

    return check_response(command, reg_value, &response, sizeof(response));
}


static esp_loader_error_t send_cmd_with_data(const void *cmd_data, size_t cmd_size,
                                             const void *data, size_t data_size)
{
    response_t response;
    command_t command = ((command_common_t *)cmd_data)->command;

    RETURN_ON_ERROR( SLIP_send_delimiter() );
    RETURN_ON_ERROR( SLIP_send((const uint8_t *)cmd_data, cmd_size) );
    RETURN_ON_ERROR( SLIP_send(data, data_size) );
    RETURN_ON_ERROR( SLIP_send_delimiter() );

    return check_response(command, NULL, &response, sizeof(response));
}


static esp_loader_error_t send_cmd_md5(const void *cmd_data, size_t cmd_size, uint8_t md5_out[MD5_SIZE])
{
    rom_md5_response_t response;
    command_t command = ((command_common_t *)cmd_data)->command;

    RETURN_ON_ERROR( SLIP_send_delimiter() );
    RETURN_ON_ERROR( SLIP_send((const uint8_t *)cmd_data, cmd_size) );
    RETURN_ON_ERROR( SLIP_send_delimiter() );

    RETURN_ON_ERROR( check_response(command, NULL, &response, sizeof(response)) );

    memcpy(md5_out, response.md5, MD5_SIZE);

    return ESP_LOADER_SUCCESS;
}


static void log_loader_internal_error(error_code_t error)
{
    loader_port_debug_print("Error: ");

    switch (error) {
        case INVALID_CRC:     loader_port_debug_print("INVALID_CRC"); break;
        case INVALID_COMMAND: loader_port_debug_print("INVALID_COMMAND"); break;
        case COMMAND_FAILED:  loader_port_debug_print("COMMAND_FAILED"); break;
        case FLASH_WRITE_ERR: loader_port_debug_print("FLASH_WRITE_ERR"); break;
        case FLASH_READ_ERR:  loader_port_debug_print("FLASH_READ_ERR"); break;
        case READ_LENGTH_ERR: loader_port_debug_print("READ_LENGTH_ERR"); break;
        case DEFLATE_ERROR:   loader_port_debug_print("DEFLATE_ERROR"); break;
        default:              loader_port_debug_print("UNKNOWN ERROR"); break;
    }
    
    loader_port_debug_print("\n");
}


static esp_loader_error_t check_response(command_t cmd, uint32_t *reg_value, void* resp, uint32_t resp_size)
{
    esp_loader_error_t err;
    common_response_t *response = (common_response_t *)resp;

    do {
        err = SLIP_receive_packet(resp, resp_size);
        if (err != ESP_LOADER_SUCCESS) {
            return err;
        }
    } while ((response->direction != READ_DIRECTION) || (response->command != cmd));

    response_status_t *status = (response_status_t *)((uint8_t *)resp + resp_size - sizeof(response_status_t));

    if (status->failed) {
        log_loader_internal_error(status->error);
        return ESP_LOADER_ERROR_INVALID_RESPONSE;
    }

    if (reg_value != NULL) {
        *reg_value = response->value;
    }

    return ESP_LOADER_SUCCESS;
}

esp_loader_error_t loader_flash_begin_cmd(uint32_t offset,
                                          uint32_t erase_size,
                                          uint32_t block_size,
                                          uint32_t blocks_to_write,
                                          bool encryption)
{
    uint32_t encryption_size = encryption ? sizeof(uint32_t) : 0;

    begin_command_t begin_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_BEGIN,
            .size = CMD_SIZE(begin_cmd) - encryption_size,
            .checksum = 0
        },
        .erase_size = erase_size,
        .packet_count = blocks_to_write,
        .packet_size = block_size,
        .offset = offset,
        .encrypted = 0
    };

    s_sequence_number = 0;

    return send_cmd(&begin_cmd, sizeof(begin_cmd) - encryption_size, NULL);
}


esp_loader_error_t loader_flash_data_cmd(const uint8_t *data, uint32_t size)
{
    data_command_t data_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_DATA,
            .size = CMD_SIZE(data_cmd) + size,
            .checksum = compute_checksum(data, size)
        },
        .data_size = size,
        .sequence_number = s_sequence_number++,
    };

    return send_cmd_with_data(&data_cmd, sizeof(data_cmd), data, size);
}


esp_loader_error_t loader_flash_end_cmd(bool stay_in_loader)
{
    flash_end_command_t end_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = FLASH_END,
            .size = CMD_SIZE(end_cmd),
            .checksum = 0
        },
        .stay_in_loader = stay_in_loader
    };

    return send_cmd(&end_cmd, sizeof(end_cmd), NULL);
}


esp_loader_error_t loader_sync_cmd(void)
{
    sync_command_t sync_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = SYNC,
            .size = CMD_SIZE(sync_cmd),
            .checksum = 0
        },
        .sync_sequence = {
            0x07, 0x07, 0x12, 0x20,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
            0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55,
        }
    };

    return send_cmd(&sync_cmd, sizeof(sync_cmd), NULL);
}


esp_loader_error_t loader_write_reg_cmd(uint32_t address, uint32_t value,
                                        uint32_t mask, uint32_t delay_us)
{
    write_reg_command_t write_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = WRITE_REG,
            .size = CMD_SIZE(write_cmd),
            .checksum = 0
        },
        .address = address,
        .value = value,
        .mask = mask,
        .delay_us = delay_us
    };

    return send_cmd(&write_cmd, sizeof(write_cmd), NULL);
}


esp_loader_error_t loader_read_reg_cmd(uint32_t address, uint32_t *reg)
{
    read_reg_command_t read_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = READ_REG,
            .size = CMD_SIZE(read_cmd),
            .checksum = 0
        },
        .address = address,
    };

    return send_cmd(&read_cmd, sizeof(read_cmd), reg);
}


esp_loader_error_t loader_spi_attach_cmd(uint32_t config)
{
    spi_attach_command_t attach_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = SPI_ATTACH,
            .size = CMD_SIZE(attach_cmd),
            .checksum = 0
        },
        .configuration = config,
        .zero = 0
    };

    return send_cmd(&attach_cmd, sizeof(attach_cmd), NULL);
}

esp_loader_error_t loader_change_baudrate_cmd(uint32_t baudrate)
{
    change_baudrate_command_t baudrate_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = CHANGE_BAUDRATE,
            .size = CMD_SIZE(baudrate_cmd),
            .checksum = 0
        },
        .new_baudrate = baudrate,
        .old_baudrate = 0 // ESP32 ROM only
    };

    return send_cmd(&baudrate_cmd, sizeof(baudrate_cmd), NULL);
}

esp_loader_error_t loader_md5_cmd(uint32_t address, uint32_t size, uint8_t *md5_out)
{
    spi_flash_md5_command_t md5_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = SPI_FLASH_MD5,
            .size = CMD_SIZE(md5_cmd),
            .checksum = 0
        },
        .address = address,
        .size = size,
        .reserved_0 = 0,
        .reserved_1 = 0
    };

    return send_cmd_md5(&md5_cmd, sizeof(md5_cmd), md5_out);
}

esp_loader_error_t loader_spi_parameters(uint32_t total_size)
{
    write_spi_command_t spi_cmd = {
        .common = {
            .direction = WRITE_DIRECTION,
            .command = SPI_SET_PARAMS,
            .size = 24,
            .checksum = 0
        },
        .id = 0,
        .total_size = total_size,
        .block_size = 64 * 1024,
        .sector_size = 4 * 1024,
        .page_size = 0x100,
        .status_mask = 0xFFFF,
    };

    return send_cmd(&spi_cmd, sizeof(spi_cmd), NULL);
}

__attribute__ ((weak)) void loader_port_debug_print(const char *str)
{

}
