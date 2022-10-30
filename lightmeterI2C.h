#pragma once

#include <furi.h>
#include <furi_hal.h>

// I2C BUS
#define I2C_BUS &furi_hal_i2c_handle_external
#define I2C_TIMEOUT 3

typedef struct {
    uint8_t address_idx;
    uint8_t value;
    uint8_t recv[2];
    bool must_send;
    bool sended;
    bool error;

} lightmeterI2CSender;

void i2c_send();

lightmeterI2CSender* lightmeter_sender_alloc();
void lightmeter_sender_free(lightmeterI2CSender* i2c_sender);
