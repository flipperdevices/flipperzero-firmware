#include "avr_tpi.h"
#include "../lib/driver/avr_tpi_reg.h"
#include "../lib/driver/avr_isp_tpi_sw.h"

#include <furi.h>

//https://ww1.microchip.com/downloads/en/DeviceDoc/ATtiny4-5-9-10-Data-Sheet-DS40002060A.pdf

#define AVR_TPI_PROG_TX_RX_BUF_SIZE 320
#define TAG "AvrTpi"

struct AvrTpi {
    AvrIspTpiSw* tpi;
    bool pmode;
    AvrTpiCallback callback;
    void* context;
};

AvrTpi* avr_tpi_alloc(void) {
    AvrTpi* instance = malloc(sizeof(AvrTpi));
    instance->pmode = false;
    return instance;
}

void avr_tpi_free(AvrTpi* instance) {
    furi_assert(instance);

    if(instance->tpi) {
        avr_isp_tpi_sw_end_pmode(instance->tpi);
        avr_isp_tpi_sw_free(instance->tpi);
    }
    free(instance);
}

void avr_tpi_set_tx_callback(AvrTpi* instance, AvrTpiCallback callback, void* context) {
    furi_assert(instance);
    furi_assert(context);

    instance->callback = callback;
    instance->context = context;
}

bool avr_tpi_start_pmode(AvrTpi* instance) {
    furi_assert(instance);

    instance->pmode = false;
    if(instance->tpi) {
        avr_isp_tpi_sw_end_pmode(instance->tpi);
        avr_isp_tpi_sw_free(instance->tpi);
        instance->tpi = NULL;
    }

    instance->tpi = avr_isp_tpi_sw_init(AvrIspTpiSwSpeed125Khz);
    avr_isp_tpi_sw_start_pmode(instance->tpi);
    // 15.7.2 TPIPCR – tiny programming interface physical layer control register
    // the default guard time is 128 IDLE bits + 0
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SSTCS | TPI_REG_TPIPCR);
    avr_isp_tpi_sw_tx(instance->tpi, 0b00000111);

    //check tiny programming interface identification code
    //15.7.1 TPIIR – tiny programming interface identification register
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SLDCS | TPI_REG_TPIIR);
    uint8_t tpiir[1] = {0};
    avr_isp_tpi_sw_rx(instance->tpi, tpiir);
    //Table 15-12. identification code for tiny programming interface
    if(tpiir[0] != 0x80) {
        FURI_LOG_E(TAG, "TPIIR: %02x, Not TPI protocol", tpiir[0]);
        return false;
    }

    // enable NVM programming
    // 15.6 accessing the non-volatile memory controller
    // SKEY + 8 byte key to enable NVM programming
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SKEY);
    for(int i = sizeof(tpi_skey) - 1; i >= 0; i--) avr_isp_tpi_sw_tx(instance->tpi, tpi_skey[i]);

    //wait for NVMEN bit
    //15.7.3 TPISR – Tiny Programming Interface Status Register
    uint8_t nvmcsr[1] = {0};
    uint8_t timeout = 100;
    do {
        avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SLDCS | TPI_REG_TPISR);
        avr_isp_tpi_sw_rx(instance->tpi, nvmcsr);
        if(timeout-- == 0) {
            FURI_LOG_E(TAG, "Timeout waiting for NVMEN");
            return false;
        }
    } while(!(nvmcsr[0] & TPI_REG_TPISR_NVMEN));
    instance->pmode = true;
    FURI_LOG_E(TAG, "PROG OK");
    return true;
}

void avr_tpi_end_pmode(AvrTpi* instance) {
    furi_assert(instance);

    if(instance->tpi) {
        avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SLDCS | TPI_REG_TPISR);
        avr_isp_tpi_sw_tx(instance->tpi, 0x00); //TPI_REG_TPISR_NVMEN clear
        avr_isp_tpi_sw_end_pmode(instance->tpi);
        avr_isp_tpi_sw_free(instance->tpi);
        instance->tpi = NULL;
    }
    instance->pmode = false;
}

static void avr_tpi_set_pointer_reg(AvrTpi* instance, uint16_t reg) {
    furi_assert(instance);

    //15.5.3 SSTPR - Serial STore to Pointer Register
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SSTPR | 0);
    avr_isp_tpi_sw_tx(instance->tpi, reg & 0xFF);
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SSTPR | 1);
    avr_isp_tpi_sw_tx(instance->tpi, (reg >> 8) & 0xFF);
}

static void avr_tpi_nvm_cmd(AvrTpi* instance, uint8_t cmd) {
    furi_assert(instance);

    //16.7.2 NVMCMD - Non-Volatile Memory Command Register
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SOUT | TPI_SIO_ADDR(TPI_IOREG_NVMCMD));
    avr_isp_tpi_sw_tx(instance->tpi, cmd);
}

static void avr_tpi_nvm_status_pool(AvrTpi* instance) {
    furi_assert(instance);
    //16.7.1 NVMCSR - Non-Volatile Memory Control and Status Register
    uint8_t nvmcsr[1] = {0};
    uint16_t timeout = 1024;
    do {
        avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SIN | TPI_SIO_ADDR(TPI_IOREG_NVMCSR));
        avr_isp_tpi_sw_rx(instance->tpi, nvmcsr);
        if(timeout-- == 0) {
            FURI_LOG_E(TAG, "Timeout waiting for TPI_IOREG_NVMCSR");
            return;
        }
    } while((nvmcsr[0] & TPI_IOREG_NVMCSR_NVMBSY));
}

static uint8_t avr_tpi_read_data(AvrTpi* instance) {
    furi_assert(instance);

    uint8_t data[1] = {0};
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SLD_PI);
    avr_isp_tpi_sw_rx(instance->tpi, data);
    return data[0];
}

static void avr_tpi_write_data(AvrTpi* instance, uint8_t data) {
    furi_assert(instance);

    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SST_PI);
    avr_isp_tpi_sw_tx(instance->tpi, data);
}

void avr_tpi_erase_chip(AvrTpi* instance) {
    furi_assert(instance);

    //Figure 6-1. Data Memory Map (Byte Addressing)
    avr_tpi_set_pointer_reg(instance, 0x4001); //need the +1 for chip erase
    //16.4.3.1 Chip Erase
    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_CHIP_ERASE);
    avr_isp_tpi_sw_tx(instance->tpi, TPI_CMD_SST);
    avr_isp_tpi_sw_tx(instance->tpi, 0xFF);
    avr_tpi_nvm_status_pool(instance);

    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_NO_OPERATION);
    avr_tpi_nvm_status_pool(instance);
}

AvrTpiSignature avr_tpi_get_signature(AvrTpi* instance) {
    furi_assert(instance);

    AvrTpiSignature signature = {0};
    //16.3.4 Signature Section
    avr_tpi_set_pointer_reg(instance, 0x3FC0);
    FURI_LOG_E("TAG", "read signature!!!");
    signature.vendor = avr_tpi_read_data(instance);
    signature.part_family = avr_tpi_read_data(instance);
    signature.part_number = avr_tpi_read_data(instance);

    FURI_LOG_E(
        TAG,
        "Signature: %02x %02x %02x",
        signature.vendor,
        signature.part_family,
        signature.part_number);

    return signature;
}

uint8_t avr_tpi_get_nwm_lock_bit(AvrTpi* instance) {
    furi_assert(instance);

    //16.3.1 Non-Volatile Memory Lock Bits
    //16.4.4 Reading NVM Lock Bits
    avr_tpi_set_pointer_reg(instance, 0x3F00);
    return avr_tpi_read_data(instance);
}

void avr_tpi_set_nwm_lock_bit(AvrTpi* instance, uint8_t value) {
    furi_assert(instance);

    avr_tpi_set_pointer_reg(instance, 0x3F00);
    //16.4.5 Writing NVM Lock Bits
    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_WORD_WRITE);
    avr_tpi_write_data(instance, value);
    avr_tpi_write_data(instance, 0xFF);
    avr_tpi_nvm_status_pool(instance);

    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_NO_OPERATION);
    avr_tpi_nvm_status_pool(instance);
}

uint8_t avr_tpi_get_configuration_bit(AvrTpi* instance) {
    furi_assert(instance);

    //16.3.3 Configuration Section
    avr_tpi_set_pointer_reg(instance, 0x3F40);
    return avr_tpi_read_data(instance);
}

void avr_tpi_set_configuration_bit(AvrTpi* instance, uint8_t value) {
    furi_assert(instance);

    avr_tpi_set_pointer_reg(instance, 0x3F40);

    //16.4.3.4 Erasing the Configuration Section
    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_SECTION_ERASE);
    avr_tpi_write_data(instance, 0xFF);
    avr_tpi_write_data(instance, 0xFF);
    avr_tpi_nvm_status_pool(instance);

    if(value != 0xFF) {
        //16.4.3.5 Writing a Configuration Word
        avr_tpi_set_pointer_reg(instance, 0x3F40);
        avr_tpi_nvm_cmd(instance, TPI_NVMCMD_WORD_WRITE);
        avr_tpi_write_data(instance, value);
        avr_tpi_write_data(instance, 0xFF);
        avr_tpi_nvm_status_pool(instance);
    }

    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_NO_OPERATION);
    avr_tpi_nvm_status_pool(instance);
}

uint8_t avr_tpi_get_calibration_bit(AvrTpi* instance) {
    furi_assert(instance);

    //16.3.5 Calibration Section
    avr_tpi_set_pointer_reg(instance, 0x3F80);
    return avr_tpi_read_data(instance);
}

void avr_tpi_read_block(AvrTpi* instance, uint16_t address, uint8_t* data, uint16_t size) {
    furi_assert(instance);
    furi_assert(data);

    //Figure 6-1. Data Memory Map (Byte Addressing)
    avr_tpi_set_pointer_reg(instance, address);
    for(uint16_t i = 0; i < size; i++) {
        data[i] = avr_tpi_read_data(instance);
    }
}

void avr_tpi_write_block(AvrTpi* instance, uint16_t address, uint8_t* data, uint16_t size) {
    furi_assert(instance);
    furi_assert(data);

    //16.4.3.3 Writing a Code Word
    avr_tpi_set_pointer_reg(instance, address);
    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_WORD_WRITE);

    for(uint16_t i = 0; i < size; i += 2) {
        avr_tpi_write_data(instance, data[i]);
        avr_tpi_write_data(instance, data[i + 1]);
        avr_tpi_nvm_status_pool(instance);
    }

    avr_tpi_nvm_cmd(instance, TPI_NVMCMD_NO_OPERATION);
    avr_tpi_nvm_status_pool(instance);
}
