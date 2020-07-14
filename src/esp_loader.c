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

#include "loader_config.h"
#include "serial_comm_prv.h"
#include "serial_comm.h"
#include "serial_io.h"
#include "esp_loader.h"
#include "md5_hash.h"
#include <string.h>
#include <assert.h>

#ifndef MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#endif

typedef struct {
    uint32_t reg_1;
    uint32_t reg_2;
} date_registers_t;

static const date_registers_t date_regs = {
#if defined TARGET_ESP8266
    .reg_1 = 0x00062000,
    .reg_2 = 0
#elif defined TARGET_ESP32
    .reg_1 = 0x15122500,
    .reg_2 = 0
#elif defined TARGET_ESP32_S2
    .reg_1 = 0x00000500,
    .reg_2 = 0x19031400
#endif
};

static const uint32_t UART_DATE_REG_ADDR = 0x60000078;    // used to differentiate ESP8266 vs ESP32*
static const uint32_t UART_DATE_REG2_ADDR = 0x3f400074;   // used to differentiate ESP32-S2 vs other models

static const uint32_t DEFAULT_TIMEOUT = 500;
static const uint32_t DEFAULT_FLASH_TIMEOUT = 3000;       // timeout for most flash operations
static const uint32_t ERASE_REGION_TIMEOUT_PER_MB = 3000; // timeout (per megabyte) for erasing a region
static const uint8_t  PADDING_PATTERN = 0xFF;

static uint32_t s_flash_write_size = 0;

#define MEGABYTE  1024 * 1024

size_t size_id_to_flash_size[] = {
    MEGABYTE / 4,  // 256KB,
    MEGABYTE / 2,  // 512KB,
    1 * MEGABYTE,  // 1MB,
    2 * MEGABYTE,  // 2MB,
    4 * MEGABYTE,  // 4MB,
    8 * MEGABYTE,  // 8MB,
    16 * MEGABYTE  // 16MB
};

typedef enum {
    SPI_FLASH_READ_ID = 0x9F
} spi_flash_cmd_t;

#if defined TARGET_ESP8266
#define SPI_REG_BASE    0x60000200
#define SPI_CMD_REG     SPI_REG_BASE + 0x00
#define SPI_USR_REG     SPI_REG_BASE + 0x1c
#define SPI_USR1_REG    SPI_REG_BASE + 0x20
#define SPI_USR2_REG    SPI_REG_BASE + 0x24
#define SPI_W0_REG      SPI_REG_BASE + 0x40
#define SPI_MOSI_DLEN_REG 0
#define SPI_MISO_DLEN_REG 0
#elif defined TARGET_ESP32_S2
#define SPI_REG_BASE    0x3f402000
#define SPI_CMD_REG     SPI_REG_BASE + 0x00
#define SPI_USR_REG     SPI_REG_BASE + 0x18
#define SPI_USR1_REG    SPI_REG_BASE + 0x1c
#define SPI_USR2_REG    SPI_REG_BASE + 0x20
#define SPI_W0_REG      SPI_REG_BASE + 0x58
#define SPI_MOSI_DLEN_REG SPI_REG_BASE + 0x24
#define SPI_MISO_DLEN_REG SPI_REG_BASE + 0x28
#elif defined TARGET_ESP32
#define SPI_REG_BASE    0x60002000
#define SPI_CMD_REG     SPI_REG_BASE + 0x00
#define SPI_USR_REG     SPI_REG_BASE + 0x1c
#define SPI_USR1_REG    SPI_REG_BASE + 0x20
#define SPI_USR2_REG    SPI_REG_BASE + 0x24
#define SPI_W0_REG      SPI_REG_BASE + 0x80
#define SPI_MOSI_DLEN_REG SPI_REG_BASE + 0x28
#define SPI_MISO_DLEN_REG SPI_REG_BASE + 0x2c
#endif

#if MD5_ENABLED

static const uint32_t MD5_TIMEOUT_PER_MB = 800;
static struct MD5Context s_md5_context;
static uint32_t s_start_address;
static uint32_t s_image_size;

static inline void init_md5(uint32_t address, uint32_t size)
{
    s_start_address = address;
    s_image_size = size;
    MD5Init(&s_md5_context);
}

static inline void md5_update(const uint8_t *data, uint32_t size)
{
    MD5Update(&s_md5_context, data, size);
}

static inline void md5_final(uint8_t digets[16])
{
    MD5Final(digets, &s_md5_context);
}

#else

static inline void init_md5(uint32_t address, uint32_t size) { }
static inline void md5_update(const uint8_t *data, uint32_t size) { }
static inline void md5_final(uint8_t digets[16]) { }

#endif


static uint32_t timeout_per_mb(uint32_t size_bytes, uint32_t time_per_mb)
{
    uint32_t timeout = ERASE_REGION_TIMEOUT_PER_MB * (size_bytes / 1e6);
    return MAX(timeout, DEFAULT_FLASH_TIMEOUT);
}

static esp_loader_error_t detect_chip(void)
{
    uint32_t reg_1, reg_2;

    RETURN_ON_ERROR( esp_loader_read_register(UART_DATE_REG_ADDR,  &reg_1) );
    RETURN_ON_ERROR( esp_loader_read_register(UART_DATE_REG2_ADDR, &reg_2) );

    if (date_regs.reg_1 == reg_1 && (date_regs.reg_2 == 0 || date_regs.reg_2 == reg_2)) {
        return ESP_LOADER_SUCCESS;
    } else {
        return ESP_LOADER_ERROR_INVALID_TARGET;
    }
}

esp_loader_error_t esp_loader_connect(esp_loader_connect_args_t *connect_args)
{
    esp_loader_error_t err;
    int32_t trials = connect_args->trials;

    loader_port_enter_bootloader();

    do {
        loader_port_start_timer(connect_args->sync_timeout);
        err = loader_sync_cmd();
        if (err == ESP_LOADER_ERROR_TIMEOUT) {
            if (--trials == 0) {
                return ESP_LOADER_ERROR_TIMEOUT;
            }
            loader_port_delay_ms(100);
        } else if (err != ESP_LOADER_SUCCESS) {
            return err;
        }
    } while (err != ESP_LOADER_SUCCESS);

    RETURN_ON_ERROR( detect_chip() );

#ifndef TARGET_ESP8266
    uint32_t SPI_PIN_CONFIG_DEFAULT = 0;
    loader_port_start_timer(DEFAULT_TIMEOUT);
    err = loader_spi_attach_cmd(SPI_PIN_CONFIG_DEFAULT);
#endif

    return err;
}

#ifndef TARGET_ESP8266

static esp_loader_error_t spi_set_data_lengths(size_t mosi_bits, size_t miso_bits)
{
    if (mosi_bits > 0) {
        RETURN_ON_ERROR( esp_loader_write_register(SPI_MOSI_DLEN_REG, mosi_bits - 1) );
    }
    if (miso_bits > 0) {
        RETURN_ON_ERROR( esp_loader_write_register(SPI_MISO_DLEN_REG, miso_bits - 1) );
    }

    return ESP_LOADER_SUCCESS;
}

#else

static esp_loader_error_t spi_set_data_lengths(size_t mosi_bits, size_t miso_bits)
{
    uint32_t mosi_bitlen_shift = 17;
    uint32_t miso_bitlen_shift = 8;
    uint32_t mosi_mask = (mosi_bits == 0) ? 0 : mosi_bits - 1;
    uint32_t miso_mask = (miso_bits == 0) ? 0 : miso_bits - 1;
    uint32_t usr_reg = (miso_mask << miso_bitlen_shift) | (mosi_mask << mosi_bitlen_shift);

    return esp_loader_write_register(SPI_USR1_REG, usr_reg);
}

#endif

static esp_loader_error_t spi_flash_command(spi_flash_cmd_t cmd, void *data_tx, size_t tx_size, void *data_rx, size_t rx_size)
{
    assert(rx_size <= 32); // Reading more than 32 bits back from a SPI flash operation is unsupported
    assert(tx_size <= 64); // Writing more than 64 bytes of data with one SPI command is unsupported

    uint32_t SPI_USR_CMD  = (1 << 31);
    uint32_t SPI_USR_MISO = (1 << 28);
    uint32_t SPI_USR_MOSI = (1 << 27);
    uint32_t SPI_CMD_USR  = (1 << 18);
    uint32_t CMD_LEN_SHIFT = 28;

    // Save SPI configuration
    uint32_t old_spi_usr;
    uint32_t old_spi_usr2;
    RETURN_ON_ERROR( esp_loader_read_register(SPI_USR_REG, &old_spi_usr) );
    RETURN_ON_ERROR( esp_loader_read_register(SPI_USR2_REG, &old_spi_usr2) );

    RETURN_ON_ERROR( spi_set_data_lengths(tx_size, rx_size) );

    uint32_t usr_reg_2 = (7 << CMD_LEN_SHIFT) | cmd;
    uint32_t usr_reg = SPI_USR_CMD;
    if (rx_size > 0) {
        usr_reg |= SPI_USR_MISO;
    }
    if (tx_size > 0) {
        usr_reg |= SPI_USR_MOSI;
    }

    RETURN_ON_ERROR( esp_loader_write_register(SPI_USR_REG, usr_reg) );
    RETURN_ON_ERROR( esp_loader_write_register(SPI_USR2_REG, usr_reg_2 ) );

    if (tx_size == 0) {
        // clear data register before we read it
        RETURN_ON_ERROR( esp_loader_write_register(SPI_W0_REG, 0) );
    } else {
        uint32_t *data = (uint32_t *)data_tx;
        uint32_t words_to_write = MIN((tx_size + 31) / 8 * 4, 1);
        uint32_t data_reg_addr = SPI_W0_REG;

        while (words_to_write--) {
            uint32_t word = *data++;
            RETURN_ON_ERROR( esp_loader_write_register(data_reg_addr, word) );
            data_reg_addr += 4;
        }
    }

    RETURN_ON_ERROR( esp_loader_write_register(SPI_CMD_REG, SPI_CMD_USR) );

    uint32_t trials = 10;
    while (trials--) {
        uint32_t cmd_reg;
        RETURN_ON_ERROR( esp_loader_read_register(SPI_CMD_REG, &cmd_reg) );
        if ((cmd_reg & SPI_CMD_USR) == 0) {
            break;
        }
    }

    if (trials == 0) {
        return ESP_LOADER_ERROR_TIMEOUT;
    }

    RETURN_ON_ERROR( esp_loader_read_register(SPI_W0_REG, data_rx) );

    // Restore SPI configuration
    RETURN_ON_ERROR( esp_loader_write_register(SPI_USR_REG, old_spi_usr) );
    RETURN_ON_ERROR( esp_loader_write_register(SPI_USR2_REG, old_spi_usr2) );

    return ESP_LOADER_SUCCESS;
}

static esp_loader_error_t detect_flash_size(size_t *flash_size)
{
    uint32_t flash_id = 0;

    RETURN_ON_ERROR( spi_flash_command(SPI_FLASH_READ_ID, NULL, 0, &flash_id, 24) );
    uint32_t size_id = flash_id >> 16;

    if (size_id < 0x12 || size_id > 0x18) {
        return ESP_LOADER_ERROR_UNSUPPORTED_CHIP;
    }

    *flash_size = size_id_to_flash_size[size_id - 0x12];

    return ESP_LOADER_SUCCESS;
}

esp_loader_error_t esp_loader_flash_start(uint32_t offset, uint32_t image_size, uint32_t block_size)
{
    uint32_t blocks_to_write = (image_size + block_size - 1) / block_size;
    uint32_t erase_size = block_size * blocks_to_write;
    s_flash_write_size = block_size;
    size_t flash_size = 0;

    if (detect_flash_size(&flash_size) == ESP_LOADER_SUCCESS) {
        if (image_size > flash_size) {
            return ESP_LOADER_ERROR_IMAGE_SIZE;
        }
        loader_port_start_timer(DEFAULT_TIMEOUT);
        RETURN_ON_ERROR( loader_spi_parameters(flash_size) );
    } else {
        loader_port_debug_print("Flash size detection failed, falling back to default");
    }

    init_md5(offset, image_size);

    loader_port_start_timer(timeout_per_mb(erase_size, ERASE_REGION_TIMEOUT_PER_MB));
    return loader_flash_begin_cmd(offset, erase_size, block_size, blocks_to_write);
}


esp_loader_error_t esp_loader_flash_write(void *payload, uint32_t size)
{
    uint32_t padding_bytes = s_flash_write_size - size;
    uint8_t *data = (uint8_t *)payload;
    uint32_t padding_index = size;

    while (padding_bytes--) {
        data[padding_index++] = PADDING_PATTERN;
    }

    md5_update(payload, (size + 3) & ~3);

    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_flash_data_cmd(data, s_flash_write_size);
}


esp_loader_error_t esp_loader_flash_finish(bool reboot)
{
    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_flash_end_cmd(!reboot);
}


esp_loader_error_t esp_loader_read_register(uint32_t address, uint32_t *reg_value)
{
    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_read_reg_cmd(address, reg_value);
}


esp_loader_error_t esp_loader_write_register(uint32_t address, uint32_t reg_value)
{
    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_write_reg_cmd(address, reg_value, 0xFFFFFFFF, 0);
}

#ifndef TARGET_ESP8266
esp_loader_error_t esp_loader_change_baudrate(uint32_t baudrate)
{
    loader_port_start_timer(DEFAULT_TIMEOUT);

    return loader_change_baudrate_cmd(baudrate);
}
#endif

#if MD5_ENABLED

static void hexify(const uint8_t raw_md5[16], uint8_t hex_md5_out[32])
{
    uint8_t high_nibble, low_nibble;

    static const uint8_t dec_to_hex[] = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    for (int i = 0; i < 16; i++) {
        high_nibble = (raw_md5[i] / 16);
        low_nibble = (raw_md5[i] - (high_nibble * 16));
        *hex_md5_out++ = dec_to_hex[high_nibble];
        *hex_md5_out++ = dec_to_hex[low_nibble];
    }
}


esp_loader_error_t esp_loader_flash_verify(void)
{
    uint8_t raw_md5[16];
    uint8_t hex_md5[MD5_SIZE + 1];
    uint8_t received_md5[MD5_SIZE + 1];

    md5_final(raw_md5);
    hexify(raw_md5, hex_md5);

    loader_port_start_timer(timeout_per_mb(s_image_size, MD5_TIMEOUT_PER_MB));

    RETURN_ON_ERROR( loader_md5_cmd(s_start_address, s_image_size, received_md5) );

    bool md5_match = memcmp(hex_md5, received_md5, MD5_SIZE) == 0;

    if (!md5_match) {
        hex_md5[MD5_SIZE] = '\n';
        received_md5[MD5_SIZE] = '\n';

        loader_port_debug_print("Error: MD5 checksum does not match:\n");
        loader_port_debug_print("Expected:\n");
        loader_port_debug_print((char *)received_md5);
        loader_port_debug_print("Actual:\n");
        loader_port_debug_print((char *)hex_md5);

        return ESP_LOADER_ERROR_INVALID_MD5;
    }

    return ESP_LOADER_SUCCESS;
}

#endif

void esp_loader_reset_target(void)
{
    loader_port_reset_target();
}