/*
 * BH1750.c
 *
 *	The MIT License.
 *  Created on: 06.11.2022
 *      Author: Oleksii Kutuzov
 *
 *	Ported from:
 *	https://github.com/lamik/Light_Sensors_STM32
 */

#include "BH1750.h"

BH1750* bh1750_alloc() {
    BH1750* bh1750 = malloc(sizeof(BH1750));
    bh1750->mode = BH1750_DEFAULT_MODE;
    bh1750->mt_reg = BH1750_DEFAULT_MTREG;
    return bh1750;
}

void bh1750_free(BH1750* bh1750) {
    furi_assert(bh1750);
    free(bh1750);
}

//
//	Initialization.
//
BH1750_STATUS bh1750_init(BH1750* bh1750) {
    if(BH1750_OK == bh1750_reset(bh1750)) {
        if(BH1750_OK == bh1750_set_mt_reg(bh1750, BH1750_DEFAULT_MTREG)) // Set default value;
            return BH1750_OK;
    }
    return BH1750_ERROR;
}

//
//	Reset all registers to default value.
//
BH1750_STATUS bh1750_reset(BH1750* bh1750) {
    UNUSED(bh1750);
    uint8_t command = 0x07;
    bool status;

    furi_hal_i2c_acquire(I2C_BUS);
    status = furi_hal_i2c_tx(I2C_BUS, BH1750_ADDRESS, &command, 1, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);

    if(status) {
        return BH1750_OK;
    }

    return BH1750_ERROR;
}

//
//	Set the power state.
//	0 - sleep, low power.
//	1 - running.
//
BH1750_STATUS bh1750_set_power_state(BH1750* bh1750, uint8_t PowerOn) {
    UNUSED(bh1750);
    PowerOn = (PowerOn ? 1 : 0);
    bool status;

    furi_hal_i2c_acquire(I2C_BUS);
    status = furi_hal_i2c_tx(I2C_BUS, BH1750_ADDRESS, &PowerOn, 1, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);

    if(status) {
        return BH1750_OK;
    }

    return BH1750_ERROR;
}

//
//	Set the mode of converting. Look into bh1750_mode enum.
//
BH1750_STATUS bh1750_set_mode(BH1750* bh1750, BH1750_mode mode) {
    if(!((mode >> 4) || (mode >> 5))) {
        return BH1750_ERROR;
    }

    if((mode & 0x0F) > 3) {
        return BH1750_ERROR;
    }

    bool status;

    bh1750->mode = mode;

    furi_hal_i2c_acquire(I2C_BUS);
    status = furi_hal_i2c_tx(I2C_BUS, BH1750_ADDRESS, &mode, 1, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);

    if(status) {
        return BH1750_OK;
    }

    return BH1750_ERROR;
}

//
//	Set the Measurement Time register. It allows to increase or decrease the sensitivity.
//
BH1750_STATUS bh1750_set_mt_reg(BH1750* bh1750, uint8_t mt_reg) {
    if(mt_reg < 31 || mt_reg > 254) {
        return BH1750_ERROR;
    }

    bh1750->mt_reg = mt_reg;

    uint8_t tmp[2];
    bool status;

    tmp[0] = (0x40 | (mt_reg >> 5));
    tmp[1] = (0x60 | (mt_reg & 0x1F));

    furi_hal_i2c_acquire(I2C_BUS);
    status = furi_hal_i2c_tx(I2C_BUS, BH1750_ADDRESS, &tmp[0], 1, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);
    if(!status) {
        return BH1750_ERROR;
    }

    furi_hal_i2c_acquire(I2C_BUS);
    status = furi_hal_i2c_tx(I2C_BUS, BH1750_ADDRESS, &tmp[1], 1, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);
    if(status) {
        return BH1750_OK;
    }

    return BH1750_ERROR;
}

//
//	Trigger the conversion in manual modes.
//	For low resolution conversion time is typical 16 ms,
//	for high resolution 120 ms. You need to wait until read the measurement value.
//	There is no need to exit low power mode for manual conversion. It makes automatically.
//
BH1750_STATUS bh1750_trigger_manual_conversion(BH1750* bh1750) {
    if(BH1750_OK == bh1750_set_mode(bh1750, bh1750->mode)) {
        return BH1750_OK;
    }
    return BH1750_ERROR;
}

//
//	Read the converted value and calculate the result.
//
BH1750_STATUS bh1750_read_light(BH1750* bh1750, float* result) {
    uint8_t mt_reg = bh1750->mt_reg;
    BH1750_mode mode = bh1750->mode;
    float result_tmp;
    uint8_t rcv[2];
    bool status;

    furi_hal_i2c_acquire(I2C_BUS);
    status = furi_hal_i2c_rx(I2C_BUS, BH1750_ADDRESS, rcv, 2, I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);

    if(status) {
        result_tmp = (rcv[0] << 8) | (rcv[1]);

        if(mt_reg != BH1750_DEFAULT_MTREG) {
            result_tmp *= (float)((uint8_t)BH1750_DEFAULT_MTREG / (float)mt_reg);
        }

        if(mode == ONETIME_HIGH_RES_MODE_2 || mode == CONTINUOUS_HIGH_RES_MODE_2) {
            result_tmp /= 2.0;
        }

        *result = result_tmp / BH1750_CONVERSION_FACTOR;

        return BH1750_OK;
    }
    return BH1750_ERROR;
}
