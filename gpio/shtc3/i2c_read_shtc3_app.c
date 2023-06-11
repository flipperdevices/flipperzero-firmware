#include <furi.h>
#include <furi_hal.h>

uint8_t address = 0x70 << 1;
uint32_t timeout = 1000;
FuriHalI2cBusHandle* i2c = &furi_hal_i2c_handle_external;

void wake_up() {
    uint8_t data_wake_up[2] = {0x35, 0x17};
    if(furi_hal_i2c_tx(i2c, address, data_wake_up, 2, timeout)) {
        FURI_LOG_I("I2C", "Wake up success");
    } else {
        FURI_LOG_I("I2C", "Wake up failed");
    }
}

void read_id() {
    uint8_t data_read_id[2] = {0xef, 0xc8};
    uint8_t data_response[3] = {0};
    if(furi_hal_i2c_trx(i2c, address, data_read_id, 2, data_response, 3, timeout)) {
        uint16_t value = (data_response[0] << 8) | data_response[1];

        // put binary value into buffer and null terminate
        char buffer[16];
        for(uint8_t i = 0; i < 16; i++) {
            buffer[15 - i] = (value & (1 << i)) ? '1' : '0';
        }
        buffer[16] = 0;

        FURI_LOG_I("I2C", "Read ID:     (%s) %04x", buffer, value);
        FURI_LOG_I("I2C", "Expected ID: (xxxx1xxxxx000111)");
        FURI_LOG_I("I2C", "CRC 8: %02x", data_response[2]);
    } else {
        FURI_LOG_I("I2C", "Read ID failed");
    }
}

int32_t i2c_read_shtc3_app(void* p) {
    UNUSED(p);

    furi_hal_i2c_acquire(&furi_hal_i2c_handle_external);
    wake_up();
    read_id();
    furi_hal_i2c_release(&furi_hal_i2c_handle_external);

    return 0;
}