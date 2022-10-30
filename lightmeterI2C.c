#include "lightmeterI2C.h"

void i2c_send(lightmeterI2CSender* i2c_sender) {
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t address = 0x23 << 1;
    i2c_sender->error = furi_hal_i2c_trx(
        I2C_BUS,
        address,
        &i2c_sender->value,
        sizeof(i2c_sender->value),
        i2c_sender->recv,
        sizeof(i2c_sender->recv),
        I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);
    i2c_sender->must_send = false;
    i2c_sender->sended = true;
}

lightmeterI2CSender* lightmeter_sender_alloc() {
    lightmeterI2CSender* i2c_sender = malloc(sizeof(lightmeterI2CSender));
    i2c_sender->must_send = false;
    i2c_sender->sended = false;
    return i2c_sender;
}

void lightmeter_sender_free(lightmeterI2CSender* i2c_sender) {
    furi_assert(i2c_sender);
    free(i2c_sender);
}
