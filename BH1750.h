/*
 * BH1750.h
 *
 *	The MIT License.
 *  Created on: 06.11.2022
 *      Author: Oleksii Kutuzov
 *
 *	Ported from:
 *	https://github.com/lamik/Light_Sensors_STM32
 */

#include <furi.h>
#include <furi_hal.h>

#ifndef BH1750_H_
#define BH1750_H_

// I2C BUS
#define I2C_BUS &furi_hal_i2c_handle_external
#define I2C_TIMEOUT 10

#define BH1750_ADDRESS (0x23 << 1)

#define BH1750_POWER_DOWN 0x00
#define BH1750_POWER_ON 0x01
#define BH1750_RESET 0x07
#define BH1750_DEFAULT_MTREG 69
#define BH1750_DEFAULT_MODE ONETIME_HIGH_RES_MODE

#define BH1750_CONVERSION_FACTOR 1.2

typedef enum { BH1750_OK = 0, BH1750_ERROR = 1 } BH1750_STATUS;

typedef enum {
    CONTINUOUS_HIGH_RES_MODE = 0x10,
    CONTINUOUS_HIGH_RES_MODE_2 = 0x11,
    CONTINUOUS_LOW_RES_MODE = 0x13,
    ONETIME_HIGH_RES_MODE = 0x20,
    ONETIME_HIGH_RES_MODE_2 = 0x21,
    ONETIME_LOW_RES_MODE = 0x23
} BH1750_mode;

typedef struct {
    BH1750_mode mode;
    uint8_t mt_reg;
} BH1750;

BH1750* bh1750_alloc();
void bh1750_free(BH1750* bh1750);

BH1750_STATUS bh1750_init(BH1750* bh1750);
BH1750_STATUS bh1750_reset(BH1750* bh1750);
BH1750_STATUS bh1750_set_power_state(BH1750* bh1750, uint8_t PowerOn);
BH1750_STATUS bh1750_set_mt_reg(BH1750* bh1750, uint8_t MTreg);
BH1750_STATUS bh1750_set_mode(BH1750* bh1750, BH1750_mode Mode);
BH1750_STATUS bh1750_trigger_manual_conversion(BH1750* bh1750);
BH1750_STATUS bh1750_read_light(BH1750* bh1750, float* Result);

#endif /* BH1750_H_ */
