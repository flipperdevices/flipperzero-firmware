#include "rp2040.h"

#include <furi.h>

#include "target.h"

// Most of the below code is heavily inspired by or taken directly from:
// Blackmagic: https://github.com/blackmagic-debug/blackmagic
// Pico-bootrom: https://github.com/raspberrypi/pico-bootrom

#define RP_REG_ACCESS_NORMAL 0x0000U
#define RP_REG_ACCESS_WRITE_XOR 0x1000U
#define RP_REG_ACCESS_WRITE_ATOMIC_BITSET 0x2000U
#define RP_REG_ACCESS_WRITE_ATOMIC_BITCLR 0x3000U

#define RP_CLOCKS_BASE_ADDR 0x40008000U
#define RP_CLOCKS_WAKE_EN0 (RP_CLOCKS_BASE_ADDR + 0xa0U)
#define RP_CLOCKS_WAKE_EN1 (RP_CLOCKS_BASE_ADDR + 0xa4U)
#define RP_CLOCKS_WAKE_EN0_MASK 0xff0c0f19U
#define RP_CLOCKS_WAKE_EN1_MASK 0x00002007U

#define RP_GPIO_QSPI_BASE_ADDR 0x40018000U
#define RP_GPIO_QSPI_SCLK_CTRL (RP_GPIO_QSPI_BASE_ADDR + 0x04U)
#define RP_GPIO_QSPI_CS_CTRL (RP_GPIO_QSPI_BASE_ADDR + 0x0cU)
#define RP_GPIO_QSPI_SD0_CTRL (RP_GPIO_QSPI_BASE_ADDR + 0x14U)
#define RP_GPIO_QSPI_SD1_CTRL (RP_GPIO_QSPI_BASE_ADDR + 0x1cU)
#define RP_GPIO_QSPI_SD2_CTRL (RP_GPIO_QSPI_BASE_ADDR + 0x24U)
#define RP_GPIO_QSPI_SD3_CTRL (RP_GPIO_QSPI_BASE_ADDR + 0x2cU)
#define RP_GPIO_QSPI_CS_DRIVE_NORMAL (0U << 8U)
#define RP_GPIO_QSPI_CS_DRIVE_INVERT (1U << 8U)
#define RP_GPIO_QSPI_CS_DRIVE_LOW (2U << 8U)
#define RP_GPIO_QSPI_CS_DRIVE_HIGH (3U << 8U)
#define RP_GPIO_QSPI_CS_DRIVE_MASK 0x00000300U
#define RP_GPIO_QSPI_SD1_CTRL_INOVER_BITS 0x00030000U
#define RP_GPIO_QSPI_SCLK_POR 0x0000001fU

#define RP_SSI_BASE_ADDR 0x18000000U
#define RP_SSI_CTRL0 (RP_SSI_BASE_ADDR + 0x00U)
#define RP_SSI_CTRL1 (RP_SSI_BASE_ADDR + 0x04U)
#define RP_SSI_ENABLE (RP_SSI_BASE_ADDR + 0x08U)
#define RP_SSI_SER (RP_SSI_BASE_ADDR + 0x10U)
#define RP_SSI_BAUD (RP_SSI_BASE_ADDR + 0x14U)
#define RP_SSI_TXFLR (RP_SSI_BASE_ADDR + 0x20U)
#define RP_SSI_RXFLR (RP_SSI_BASE_ADDR + 0x24U)
#define RP_SSI_SR (RP_SSI_BASE_ADDR + 0x28U)
#define RP_SSI_ICR (RP_SSI_BASE_ADDR + 0x48U)
#define RP_SSI_DR0 (RP_SSI_BASE_ADDR + 0x60U)
#define RP_SSI_XIP_SPI_CTRL0 (RP_SSI_BASE_ADDR + 0xf4U)
#define RP_SSI_CTRL0_FRF_MASK 0x00600000U
#define RP_SSI_CTRL0_FRF_SERIAL (0U << 21U)
#define RP_SSI_CTRL0_FRF_DUAL (1U << 21U)
#define RP_SSI_CTRL0_FRF_QUAD (2U << 21U)
#define RP_SSI_CTRL0_TMOD_MASK 0x00000300U
#define RP_SSI_CTRL0_TMOD_BIDI (0U << 8U)
#define RP_SSI_CTRL0_TMOD_TX_ONLY (1U << 8U)
#define RP_SSI_CTRL0_TMOD_RX_ONLY (2U << 8U)
#define RP_SSI_CTRL0_TMOD_EEPROM (3U << 8U)
#define RP_SSI_CTRL0_DATA_BIT_MASK 0x001f0000U
#define RP_SSI_CTRL0_DATA_BIT_SHIFT 16U
#define RP_SSI_CTRL0_DATA_BITS(x) (((x)-1U) << RP_SSI_CTRL0_DATA_BIT_SHIFT)
#define RP_SSI_CTRL0_MASK \
    (RP_SSI_CTRL0_FRF_MASK | RP_SSI_CTRL0_TMOD_MASK | RP_SSI_CTRL0_DATA_BIT_MASK)
#define RP_SSI_ENABLE_SSI (1U << 0U)
#define RP_SSI_XIP_SPI_CTRL0_FORMAT_STD_SPI (0U << 0U)
#define RP_SSI_XIP_SPI_CTRL0_FORMAT_SPLIT (1U << 0U)
#define RP_SSI_XIP_SPI_CTRL0_FORMAT_FRF (2U << 0U)
#define RP_SSI_XIP_SPI_CTRL0_ADDRESS_LENGTH(x) (((x)*2U) << 2U)
#define RP_SSI_XIP_SPI_CTRL0_INSTR_LENGTH_8b (2U << 8U)
#define RP_SSI_XIP_SPI_CTRL0_WAIT_CYCLES(x) (((x)*8U) << 11U)
#define RP_SSI_XIP_SPI_CTRL0_XIP_CMD_SHIFT 24U
#define RP_SSI_XIP_SPI_CTRL0_XIP_CMD(x) ((x) << RP_SSI_XIP_SPI_CTRL0_XIP_CMD_SHIFT)
#define RP_SSI_XIP_SPI_CTRL0_TRANS_1C1A (0U << 0U)
#define RP_SSI_XIP_SPI_CTRL0_TRANS_1C2A (1U << 0U)
#define RP_SSI_XIP_SPI_CTRL0_TRANS_2C2A (2U << 0U)

#define RP_PADS_QSPI_BASE_ADDR 0x40020000U
#define RP_PADS_QSPI_GPIO_SCLK (RP_PADS_QSPI_BASE_ADDR + 0x04U)
#define RP_PADS_QSPI_GPIO_SD0 (RP_PADS_QSPI_BASE_ADDR + 0x08U)
#define RP_PADS_QSPI_GPIO_SD1 (RP_PADS_QSPI_BASE_ADDR + 0x0cU)
#define RP_PADS_QSPI_GPIO_SD2 (RP_PADS_QSPI_BASE_ADDR + 0x10U)
#define RP_PADS_QSPI_GPIO_SD3 (RP_PADS_QSPI_BASE_ADDR + 0x14U)
#define RP_PADS_QSPI_GPIO_SCLK_FAST_SLEW 0x00000001U
#define RP_PADS_QSPI_GPIO_SCLK_8mA_DRIVE 0x00000020U
#define RP_PADS_QSPI_GPIO_SCLK_IE 0x00000040U
#define RP_PADS_QSPI_GPIO_SD0_OD_BITS 0x00000080U
#define RP_PADS_QSPI_GPIO_SD0_PUE_BITS 0x00000008U
#define RP_PADS_QSPI_GPIO_SD0_PDE_BITS 0x00000004U

#define RP_RESETS_BASE_ADDR 0x4000c000U
#define RP_RESETS_RESET (RP_RESETS_BASE_ADDR + 0x00U)
#define RP_RESETS_RESET_DONE (RP_RESETS_BASE_ADDR + 0x08U)
#define RP_RESETS_RESET_IO_QSPI_BITS 0x00000040U
#define RP_RESETS_RESET_PADS_QSPI_BITS 0x00000200U

// SPI Flash defines
#define SPI_FLASH_OPCODE_MASK 0x00ffU
#define SPI_FLASH_OPCODE(x) ((x)&SPI_FLASH_OPCODE_MASK)
#define SPI_FLASH_DUMMY_MASK 0x0700U
#define SPI_FLASH_DUMMY_SHIFT 8U
#define SPI_FLASH_DUMMY_LEN(x) (((x) << SPI_FLASH_DUMMY_SHIFT) & SPI_FLASH_DUMMY_MASK)
#define SPI_FLASH_OPCODE_MODE_MASK 0x0800U
#define SPI_FLASH_OPCODE_ONLY (0U << 11U)
#define SPI_FLASH_OPCODE_3B_ADDR (1U << 11U)
#define SPI_FLASH_DATA_MASK 0x1000U
#define SPI_FLASH_DATA_SHIFT 12U
#define SPI_FLASH_DATA_IN (0U << SPI_FLASH_DATA_SHIFT)
#define SPI_FLASH_DATA_OUT (1U << SPI_FLASH_DATA_SHIFT)

#define SPI_FLASH_CMD_WRITE_ENABLE \
    (SPI_FLASH_OPCODE_ONLY | SPI_FLASH_DUMMY_LEN(0) | SPI_FLASH_OPCODE(0x06U))
#define SPI_FLASH_CMD_PAGE_PROGRAM                                            \
    (SPI_FLASH_OPCODE_3B_ADDR | SPI_FLASH_DATA_OUT | SPI_FLASH_DUMMY_LEN(0) | \
     SPI_FLASH_OPCODE(0x02))
#define SPI_FLASH_CMD_SECTOR_ERASE \
    (SPI_FLASH_OPCODE_3B_ADDR | SPI_FLASH_DUMMY_LEN(0) | SPI_FLASH_OPCODE(0x20U))
#define SPI_FLASH_CMD_CHIP_ERASE \
    (SPI_FLASH_OPCODE_ONLY | SPI_FLASH_DUMMY_LEN(0) | SPI_FLASH_OPCODE(0x60U))
#define SPI_FLASH_CMD_READ_STATUS \
    (SPI_FLASH_OPCODE_ONLY | SPI_FLASH_DATA_IN | SPI_FLASH_DUMMY_LEN(0) | SPI_FLASH_OPCODE(0x05U))
#define SPI_FLASH_CMD_READ_JEDEC_ID \
    (SPI_FLASH_OPCODE_ONLY | SPI_FLASH_DATA_IN | SPI_FLASH_DUMMY_LEN(0) | SPI_FLASH_OPCODE(0x9FU))
#define SPI_FLASH_CMD_READ_SFDP                                              \
    (SPI_FLASH_OPCODE_3B_ADDR | SPI_FLASH_DATA_IN | SPI_FLASH_DUMMY_LEN(1) | \
     SPI_FLASH_OPCODE(0x5AU))
#define SPI_FLASH_CMD_WAKE_UP \
    (SPI_FLASH_OPCODE_ONLY | SPI_FLASH_DUMMY_LEN(0) | SPI_FLASH_OPCODE(0xABU))
#define SPI_FLASH_CMD_READ_DATA                                              \
    (SPI_FLASH_OPCODE_3B_ADDR | SPI_FLASH_DATA_IN | SPI_FLASH_DUMMY_LEN(0) | \
     SPI_FLASH_OPCODE(0x03U))

#define SPI_FLASH_STATUS_BUSY 0x01U
#define SPI_FLASH_STATUS_WRITE_ENABLED 0x02U

#define RP2040_IO_PADS_BITS (RP_RESETS_RESET_IO_QSPI_BITS | RP_RESETS_RESET_PADS_QSPI_BITS)

#define W25X_CMD_RESET_ENABLE (0x66U)
#define W25X_CMD_RESET (0x99U)

#define TAG "VgmRp2040"

static bool rp2040_spi_gpio_init(void) {
    bool success = false;

    do {
        if(!target_write_memory_32(
               RP_RESETS_RESET | RP_REG_ACCESS_WRITE_ATOMIC_BITSET, RP2040_IO_PADS_BITS))
            break;
        if(!target_write_memory_32(
               RP_RESETS_RESET | RP_REG_ACCESS_WRITE_ATOMIC_BITCLR, RP2040_IO_PADS_BITS))
            break;

        uint32_t reset_done = 0;
        while((reset_done & RP2040_IO_PADS_BITS) != RP2040_IO_PADS_BITS) {
            if(!target_read_memory_32(RP_RESETS_RESET_DONE, &reset_done)) break;
        }

        if(reset_done == 0) break;

        if(!target_write_memory_32(RP_GPIO_QSPI_SCLK_CTRL, 0)) break;
        if(!target_write_memory_32(RP_GPIO_QSPI_CS_CTRL, 0)) break;
        if(!target_write_memory_32(RP_GPIO_QSPI_SD0_CTRL, 0)) break;
        if(!target_write_memory_32(RP_GPIO_QSPI_SD1_CTRL, 0)) break;
        if(!target_write_memory_32(RP_GPIO_QSPI_SD2_CTRL, 0)) break;
        if(!target_write_memory_32(RP_GPIO_QSPI_SD3_CTRL, 0)) break;

        success = true;
    } while(false);

    return success;
}

// Configure SSI in regular SPI mode
static bool rp2040_spi_init(void) {
    bool success = false;

    do {
        // Disable SSI
        if(!target_write_memory_32(RP_SSI_ENABLE, 0)) break;
        // Clear error all flags
        if(!target_read_memory_32(RP_SSI_SR, NULL)) break;
        // Clear all pending interrupts
        if(!target_read_memory_32(RP_SSI_ICR, NULL)) break;
        // Set SPI clock divisor (Fclk_out = Fssi_clk / RP_SSI_BAUD)
        if(!target_write_memory_32(RP_SSI_BAUD, 6UL)) break;
        // Set SPI configuration:
        // - Regular 1-bit SPI frame format,
        // - Frame size = 8 bit,
        // - Both transmit and receive
        if(!target_write_memory_32(
               RP_SSI_CTRL0,
               RP_SSI_CTRL0_FRF_SERIAL | RP_SSI_CTRL0_DATA_BITS(8) | RP_SSI_CTRL0_TMOD_BIDI))
            break;
        if(!target_write_memory_32(RP_SSI_SER, 1)) break;
        // Enable SSI
        if(!target_write_memory_32(RP_SSI_ENABLE, 1)) break;
        success = true;
    } while(false);

    return success;
}

// Force CS pin to a chosen state
static bool rp2040_spi_chip_select(uint32_t state) {
    bool success = false;

    do {
        uint32_t cs_value;
        // Read GPIO control register
        if(!target_read_memory_32(RP_GPIO_QSPI_CS_CTRL, &cs_value)) break;
        // Modify GPIO control register
        if(!target_write_memory_32(
               RP_GPIO_QSPI_CS_CTRL, (cs_value & (~RP_GPIO_QSPI_CS_DRIVE_MASK)) | state))
            break;
        success = true;
    } while(false);

    return success;
}

// Perform an SPI transaction (transmit one byte, receive one byte at the same time)
static bool rp2040_spi_txrx(uint8_t tx_data, uint8_t* rx_data) {
    bool success = false;

    do {
        // Write to SSI data register 0
        if(!target_write_memory_32(RP_SSI_DR0, tx_data)) break;
        uint32_t value;
        // Read from SSI data register 0
        if(!target_read_memory_32(RP_SSI_DR0, &value)) break;
        if(rx_data) {
            *rx_data = value;
        }
        success = true;
    } while(false);

    return success;
}

// Prepare SPI flash operation
static bool rp2040_spi_setup_txrx(uint16_t command, uint32_t address, size_t data_size) {
    bool success = false;

    do {
        // Number of data frames = data_size
        if(!target_write_memory_32(RP_SSI_CTRL1, data_size)) break;
        // Select flash chip
        if(!rp2040_spi_chip_select(RP_GPIO_QSPI_CS_DRIVE_LOW)) break;
        // Transmit command
        const uint8_t opcode = command & SPI_FLASH_OPCODE_MASK;
        if(!rp2040_spi_txrx(opcode, NULL)) break;

        // Transmit 24-bit address for commands that require it
        if((command & SPI_FLASH_OPCODE_MODE_MASK) == SPI_FLASH_OPCODE_3B_ADDR) {
            if(!rp2040_spi_txrx((address >> 16U) & 0xFFUL, NULL)) break;
            if(!rp2040_spi_txrx((address >> 8U) & 0xFFUL, NULL)) break;
            if(!rp2040_spi_txrx(address & 0xFFUL, NULL)) break;
        }

        const size_t inter_length = (command & SPI_FLASH_DUMMY_MASK) >> SPI_FLASH_DUMMY_SHIFT;

        size_t i;
        for(i = 0; i < inter_length; ++i) {
            if(!rp2040_spi_txrx(0, NULL)) break;
        }
        if(i < inter_length) break;

        success = true;
    } while(false);

    return success;
}

static bool rp2040_spi_read(uint16_t command, uint32_t address, void* data, size_t data_size) {
    bool success = false;

    do {
        if(!rp2040_spi_setup_txrx(command, address, data_size)) break;
        uint8_t* rx_data = data;
        size_t rx_data_size;
        for(rx_data_size = 0; rx_data_size < data_size; ++rx_data_size) {
            if(!rp2040_spi_txrx(0, &rx_data[rx_data_size])) break;
        }
        if(rx_data_size < data_size) break;
        rp2040_spi_chip_select(RP_GPIO_QSPI_CS_DRIVE_HIGH);
        success = true;
    } while(false);

    return success;
}

static bool
    rp2040_spi_write(uint16_t command, uint32_t address, const void* data, const size_t data_size) {
    bool success = false;

    do {
        if(!rp2040_spi_setup_txrx(command, address, data_size)) break;
        const uint8_t* tx_data = data;
        size_t tx_data_size;
        for(tx_data_size = 0; tx_data_size < data_size; ++tx_data_size) {
            if(!rp2040_spi_txrx(tx_data[tx_data_size], NULL)) break;
        }
        if(tx_data_size < data_size) break;
        if(!rp2040_spi_chip_select(RP_GPIO_QSPI_CS_DRIVE_HIGH)) break;
        success = true;
    } while(false);

    return success;
}

static bool rp2040_spi_run_command(uint16_t command, uint32_t address) {
    return rp2040_spi_write(command, address, NULL, 0);
}

// Custom procedure to reset the W25X SPI flash
static bool rp2040_w25xx_flash_reset(void) {
    bool success = false;
    do {
        if(!rp2040_spi_txrx(W25X_CMD_RESET_ENABLE, NULL)) break;
        if(!rp2040_spi_txrx(W25X_CMD_RESET, NULL)) break;
        furi_delay_us(50);
        success = true;
    } while(false);

    return success;
}

bool rp2040_init(void) {
    bool success = false;

    do {
        if(!rp2040_spi_gpio_init()) {
            FURI_LOG_E(TAG, "Failed to initialize SPI pins");
            break;
        }
        if(!rp2040_spi_init()) {
            FURI_LOG_E(TAG, "Failed to configure SPI hardware");
            break;
        }
        if(!rp2040_w25xx_flash_reset()) {
            FURI_LOG_E(TAG, "Failed to reset SPI flash");
            break;
        }
        success = true;
    } while(false);

    return success;
}

bool rp2040_flash_read_data(uint32_t address, void* data, size_t data_size) {
    bool success = false;

    do {
        if(!rp2040_spi_read(SPI_FLASH_CMD_READ_DATA, address, data, data_size)) {
            FURI_LOG_E(TAG, "Failed to read data");
            break;
        }
        success = true;
    } while(false);

    return success;
}

bool rp2040_flash_erase_sector(uint32_t address) {
    bool success = false;

    do {
        if(!rp2040_spi_run_command(SPI_FLASH_CMD_WRITE_ENABLE, 0)) {
            FURI_LOG_E(TAG, "Failed to issue WRITE_ENABLE command");
            break;
        }
        uint8_t status;
        if(!rp2040_spi_read(SPI_FLASH_CMD_READ_STATUS, 0U, &status, sizeof(status))) {
            FURI_LOG_E(TAG, "Failed to issue READ_STATUS command");
            break;
        }
        if((status & SPI_FLASH_STATUS_WRITE_ENABLED) == 0) {
            FURI_LOG_E(TAG, "Failed to enable write mode, status byte: 0x%02X", status);
            break;
        }
        if(!rp2040_spi_run_command(SPI_FLASH_CMD_SECTOR_ERASE, address)) {
            FURI_LOG_E(TAG, "Failed to issue SECTOR_ERASE command");
            break;
        }
        do {
            if(!rp2040_spi_read(SPI_FLASH_CMD_READ_STATUS, 0U, &status, sizeof(status))) {
                FURI_LOG_E(TAG, "Failed to issue READ_STATUS command");
                break;
            }
        } while(status & SPI_FLASH_STATUS_BUSY);

        if(status & SPI_FLASH_STATUS_BUSY) break;

        success = true;
    } while(false);

    return success;
}

bool rp2040_flash_program_page(uint32_t address, const void* data, size_t data_size) {
    bool success = false;

    do {
        if(!rp2040_spi_run_command(SPI_FLASH_CMD_WRITE_ENABLE, 0)) {
            FURI_LOG_E(TAG, "Failed to issue WRITE_ENABLE command");
            break;
        }
        uint8_t status;
        if(!rp2040_spi_read(SPI_FLASH_CMD_READ_STATUS, 0U, &status, sizeof(status))) {
            FURI_LOG_E(TAG, "Failed to issue READ_STATUS command");
            break;
        }
        if((status & SPI_FLASH_STATUS_WRITE_ENABLED) == 0) {
            FURI_LOG_E(TAG, "Failed to enable write mode, status byte: 0x%02X", status);
            break;
        }
        if(!rp2040_spi_write(SPI_FLASH_CMD_PAGE_PROGRAM, address, data, data_size)) {
            FURI_LOG_E(TAG, "Failed to issue PAGE_PROGRAM command");
            break;
        }
        do {
            if(!rp2040_spi_read(SPI_FLASH_CMD_READ_STATUS, 0U, &status, sizeof(status))) {
                FURI_LOG_E(TAG, "Failed to issue READ_STATUS command");
                break;
            }
        } while(status & SPI_FLASH_STATUS_BUSY);

        if(status & SPI_FLASH_STATUS_BUSY) break;

        success = true;
    } while(false);

    return success;
}
