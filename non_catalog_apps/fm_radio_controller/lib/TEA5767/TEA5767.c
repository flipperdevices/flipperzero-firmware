/**
 * @file TEA5767.c
 * @author Victor Micó 
 * @brief Implementation to control the TEA5767 radio chip.
 * @version 0.1
 * @date 2023-08-24
 * 
 * @copyright GPLv3
 * 
 * Ported from:
 * https://github.com/mathertel/Radio
 */
#include <furi.h>
#include <furi_hal.h>
#include <stdint.h>
#include "TEA5767.h"

bool tea5767_is_device_ready() {
    bool result = false;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    if(furi_hal_i2c_is_device_ready(&furi_hal_i2c_handle_external, TEA5767_ADR, 5)) {
        result = true;
    }
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

bool tea5767_read_registers(uint8_t* buffer) {
    bool result = false;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    if(furi_hal_i2c_rx(&furi_hal_i2c_handle_external, TEA5767_ADR, buffer, 5, 100)) {
        result = true;
    }
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

bool tea5767_write_registers(uint8_t* buffer) {
    bool result = false;

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    if(furi_hal_i2c_tx(&furi_hal_i2c_handle_external, TEA5767_ADR, buffer, 5, 100)) {
        result = true;
    }
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);
    return result;
}

bool tea5767_init(uint8_t* buffer) {
    bool result = false;

    buffer[0] = 0x00;
    buffer[1] = 0x00;
    buffer[2] = 0xB0;
    buffer[3] = REG_4_XTAL | REG_4_SMUTE;
    buffer[4] = 0x00;

    result = tea5767_write_registers(buffer);
    return result;
}

bool tea5767_set_mute(uint8_t* buffer, bool mute) {
    bool result = false;

    if(mute) {
        buffer[REG_1] |= REG_1_MUTE;
    } else {
        buffer[REG_1] &= ~REG_1_MUTE;
    }

    result = tea5767_write_registers(buffer);
    return result;
}

bool tea5767_get_frequency(uint8_t* buffer, int* value) {
    bool result = false;
    uint16_t frequency;

    if(tea5767_read_registers(buffer)) {
        frequency = ((buffer[0] & REG_1_PLL) << 8) | buffer[1];
        *value = (frequency * QUARTZ / 4 - FILTER) / 10000;
        result = true;
    }
    return result;
}

bool tea5767_set_frequency(uint8_t* buffer, int value) {
    bool result = false;
    uint16_t frequency = 4 * (value * 10000 + FILTER) / QUARTZ;

    buffer[REG_1] = ((buffer[0] & ~REG_1_PLL) | ((frequency >> 8) & REG_1_PLL));
    buffer[REG_2] = frequency & REG_2_PLL;

    result = tea5767_write_registers(buffer);
    return result;
}

bool tea5767_get_radio_info(uint8_t* buffer, struct RADIO_INFO* info) {
    bool result = false;

    if(tea5767_read_registers(buffer)) {
        if(buffer[REG_3] & REG_3_STEREO) {
            info->stereo = true;
        }
        info->rssi = (buffer[REG_4] & REG_4_ADC) >> 4;
        result = true;
    }
    return result;
}