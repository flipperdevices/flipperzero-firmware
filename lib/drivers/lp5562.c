#include "lp5562.h"
#include "lp5562_reg.h"

#include <stdio.h>

void lp5562_reset(FuriHalI2cBusHandle* handle) {
    Reg0D_Reset reg = {.value = 0xFF};
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x0D, *(uint8_t*)&reg, LP5562_I2C_TIMEOUT);
}

void lp5562_configure(FuriHalI2cBusHandle* handle) {
    Reg08_Config config = {.INT_CLK_EN = true, .PS_EN = true, .PWM_HF = true};
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x08, *(uint8_t*)&config, LP5562_I2C_TIMEOUT);

    Reg70_LedMap map = {
        .red = EngSelectI2C,
        .green = EngSelectI2C,
        .blue = EngSelectI2C,
        .white = EngSelectI2C,
    };
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x70, *(uint8_t*)&map, LP5562_I2C_TIMEOUT);
}

void lp5562_enable(FuriHalI2cBusHandle* handle) {
    Reg00_Enable reg = {.CHIP_EN = true, .LOG_EN = true};
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x00, *(uint8_t*)&reg, LP5562_I2C_TIMEOUT);
}

void lp5562_set_channel_current(FuriHalI2cBusHandle* handle, LP5562Channel channel, uint8_t value) {
    uint8_t reg_no;
    if(channel == LP5562ChannelRed) {
        reg_no = 0x07;
    } else if(channel == LP5562ChannelGreen) {
        reg_no = 0x06;
    } else if(channel == LP5562ChannelBlue) {
        reg_no = 0x05;
    } else if(channel == LP5562ChannelWhite) {
        reg_no = 0x0F;
    } else {
        return;
    }
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, reg_no, value, LP5562_I2C_TIMEOUT);
}

void lp5562_set_channel_value(FuriHalI2cBusHandle* handle, LP5562Channel channel, uint8_t value) {
    uint8_t reg_no;
    if(channel == LP5562ChannelRed) {
        reg_no = 0x04;
    } else if(channel == LP5562ChannelGreen) {
        reg_no = 0x03;
    } else if(channel == LP5562ChannelBlue) {
        reg_no = 0x02;
    } else if(channel == LP5562ChannelWhite) {
        reg_no = 0x0E;
    } else {
        return;
    }
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, reg_no, value, LP5562_I2C_TIMEOUT);
}

uint8_t lp5562_get_channel_value(FuriHalI2cBusHandle* handle, LP5562Channel channel) {
    uint8_t reg_no;
    uint8_t value;
    if(channel == LP5562ChannelRed) {
        reg_no = 0x04;
    } else if(channel == LP5562ChannelGreen) {
        reg_no = 0x03;
    } else if(channel == LP5562ChannelBlue) {
        reg_no = 0x02;
    } else if(channel == LP5562ChannelWhite) {
        reg_no = 0x0E;
    } else {
        return 0;
    }
    furi_hal_i2c_read_reg_8(handle, LP5562_ADDRESS, reg_no, &value, LP5562_I2C_TIMEOUT);
    return value;
}

static void
    lp5562_set_channel_src(FuriHalI2cBusHandle* handle, LP5562Channel channel, LP5562Engine src) {
    uint8_t reg_val = 0;
    uint8_t bit_offset = 0;

    if(channel == LP5562ChannelRed) {
        bit_offset = 4;
    } else if(channel == LP5562ChannelGreen) {
        bit_offset = 2;
    } else if(channel == LP5562ChannelBlue) {
        bit_offset = 0;
    } else if(channel == LP5562ChannelWhite) {
        bit_offset = 6;
    } else {
        return;
    }

    furi_hal_i2c_read_reg_8(handle, LP5562_ADDRESS, 0x70, &reg_val, LP5562_I2C_TIMEOUT);
    reg_val &= ~(0x3 << bit_offset);
    reg_val |= ((src & 0x03) << bit_offset);
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x70, reg_val, LP5562_I2C_TIMEOUT);
}

void lp5562_execute_program(
    FuriHalI2cBusHandle* handle,
    LP5562Engine eng,
    LP5562Channel ch,
    uint16_t* program) {
    if((eng < LP5562Engine1) || (eng > LP5562Engine3)) return;
    uint8_t reg_val = 0;
    uint8_t bit_offset = 0;

    // Engine configuration
    bit_offset = (3 - eng) * 2;
    furi_hal_i2c_read_reg_8(handle, LP5562_ADDRESS, 0x01, &reg_val, LP5562_I2C_TIMEOUT);
    reg_val &= ~(0x3 << bit_offset);
    reg_val |= (0x01 << bit_offset); // load
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x01, reg_val, LP5562_I2C_TIMEOUT);

    // Program load
    for(uint8_t i = 0; i < 16; i++) {
        // Program words are big-endian, so it's not possible to load them at once with furi_hal_i2c_write_mem
        furi_hal_i2c_write_reg_16(
            handle,
            LP5562_ADDRESS,
            0x10 + (0x20 * (eng - 1)) + i * 2,
            program[i],
            LP5562_I2C_TIMEOUT);
    }

    // Program start
    bit_offset = (3 - eng) * 2;
    furi_hal_i2c_read_reg_8(handle, LP5562_ADDRESS, 0x01, &reg_val, LP5562_I2C_TIMEOUT);
    reg_val &= ~(0x3 << bit_offset);
    reg_val |= (0x02 << bit_offset); // run
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x01, reg_val, LP5562_I2C_TIMEOUT);

    furi_hal_i2c_read_reg_8(handle, LP5562_ADDRESS, 0x00, &reg_val, LP5562_I2C_TIMEOUT);
    reg_val &= ~(0x3 << bit_offset);
    reg_val |= (0x02 << bit_offset); // run
    furi_hal_i2c_write_reg_8(handle, LP5562_ADDRESS, 0x00, reg_val, LP5562_I2C_TIMEOUT);

    // Switch output to Execution Engine
    lp5562_set_channel_src(handle, ch, eng);
}

void lp5562_execute_ramp(
    FuriHalI2cBusHandle* handle,
    LP5562Engine eng,
    LP5562Channel ch,
    uint8_t val_start,
    uint8_t val_end,
    uint16_t time) {
    if(val_start == val_end) return;

    // Temporary switch to constant value from register
    lp5562_set_channel_src(handle, ch, LP5562Direct);

    // Prepare command sequence
    uint16_t program[16];
    uint8_t diff = (val_end > val_start) ? (val_end - val_start) : (val_start - val_end);
    uint16_t time_step = time * 2 / diff;
    uint8_t prescaller = 0;
    if(time_step > 0x3F) {
        time_step /= 32;
        prescaller = 1;
    }

    if(time_step == 0) {
        time_step = 1;
    } else if(time_step > 0x3F)
        time_step = 0x3F;

    program[0] = 0x4000 | val_start; // Set PWM
    if(val_end > val_start) {
        program[1] = (prescaller << 14) | (time_step << 8) | ((diff / 2) & 0x7F); // Ramp Up
    } else {
        program[1] = (prescaller << 14) | (time_step << 8) | 0x80 |
                     ((diff / 2) & 0x7F); // Ramp Down
    }
    program[2] = 0xA001 | ((2 - 1) << 7); // Loop to step 1, repeat twice to get full 8-bit scale
    program[3] = 0xC000; // End

    // Execute program
    lp5562_execute_program(handle, eng, LP5562ChannelWhite, program);

    // Write end value to register
    lp5562_set_channel_value(handle, ch, val_end);
}
