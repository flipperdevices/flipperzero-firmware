#include <bq25896.h>
#include <bq25896_reg.h>
#include <i2c.h>

uint8_t bit_reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

bool bq25896_read_reg(uint8_t address, void *value) {
    if (HAL_I2C_Master_Transmit(&POWER_I2C, BQ25896_ADDRESS, &address, 1, 2000) != HAL_OK) {
        return false;
    }

    uint8_t data;
    if (HAL_I2C_Master_Receive(&POWER_I2C, BQ25896_ADDRESS, &data, 1, 2000) != HAL_OK) {
        return false;
    }

    (*(uint8_t*)value) = data;

    return true;
}

bool bq25896_write_reg(uint8_t address, void *value) {
    uint8_t data[2] = { address, *(uint8_t*)value };

    if (HAL_I2C_Master_Transmit(&POWER_I2C, BQ25896_ADDRESS, data, 2, 2000) != HAL_OK) {
        return false;
    }
    return true;
}

REG14 bq25896_info;

void bq25896_init() {
    bq25896_read_reg(0x14, &bq25896_info);
}

void bq25896_poweroff() {
    REG09 data;
    data.BATFET_DIS=1;
    bq25896_write_reg(0x09, &data);
}

bool bq25896_is_charging() {
    REG0B data;
    bq25896_read_reg(0x0B, &data);
    return data.CHRG_STAT != CHRG_STAT_NO;
}