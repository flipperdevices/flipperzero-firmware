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

} i2cSender;

void i2c_send();

i2cSender* i2c_sender_alloc();
void i2c_sender_free(i2cSender* i2c_sender);
