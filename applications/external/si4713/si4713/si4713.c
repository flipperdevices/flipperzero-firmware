#include <furi.h>
#include <furi_hal_i2c.h>

#include "si4713.h"
#include "si4713_constants.h"

#define TAG "SI4713"

void si4713_send_command(SI4713Device* device,
                         const uint8_t* commands,
                         uint8_t length) {
    furi_assert(device, "si4713_send_command: Device is null");

    furi_hal_i2c_acquire(I2C_BUS);
    furi_check(furi_hal_i2c_is_device_ready(I2C_BUS, device->address << 1, 50),
               "device not ready");
    FURI_LOG_D(TAG, "Sending %d command(s)", length);
    furi_hal_i2c_trx(
        I2C_BUS,
        device->address << 1,
        commands,
        length,
        device->result,
        sizeof(device->result),
        I2C_TIMEOUT);
    furi_delay_ms(10);
    furi_hal_i2c_release(I2C_BUS);
    FURI_LOG_D(TAG, "2 byte response: %d %d", device->result[0], device->result[1]);
}

void si4713_trx_command(SI4713Device* device,
                            const uint8_t* commands,
                            uint8_t length,
                            uint8_t* rx,
                            uint8_t rx_length) {
    furi_assert(device);
    FURI_LOG_D(TAG, "Sending %d command(s)", length);

    furi_hal_i2c_acquire(I2C_BUS);
    furi_hal_i2c_trx(
        I2C_BUS,
        device->address << 1,
        commands,
        length,
        rx,
        rx_length,
        I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);
    FURI_LOG_D(TAG, "Received %d bytes:", rx_length);
    for (uint8_t* p = rx; p < rx + rx_length; p++)
        FURI_LOG_D(TAG, "%d", *p);

}

void si4713_set_property(SI4713Device* device,
                         uint16_t property, uint16_t value) {
    furi_assert(device);
    device->buffer[0] = SI4710_CMD_SET_PROPERTY;
    device->buffer[1] = 0;
    device->buffer[2] = property >> 8;
    device->buffer[3] = property & 0xFF;
    device->buffer[4] = value >> 8;
    device->buffer[5] = value & 0xFF;
    si4713_send_command(device,
                        device->buffer,
                        6);
}

uint8_t si4713_get_status(SI4713Device* device) {
    furi_assert(device);
    device->buffer[0] = SI4710_CMD_GET_INT_STATUS;
    si4713_send_command(device,
                        device->buffer,
                        1);
    return device->result[0];
}

void si4713_update_info(SI4713Device* device) {
    furi_assert(device);
    FURI_LOG_D(TAG, "Updating info");
    device->buffer[0] = SI4710_CMD_GET_REV;
    device->buffer[1] = 0x00;
    uint8_t resp[9];
    si4713_trx_command(device,device->buffer,
                       1, resp,9);

    device->info = (SI4713DeviceInfo){
        .firmware = (((uint16_t)resp[2]) << 8) | resp[3],
        .patch = (((uint16_t)resp[4]) << 8) | resp[5],
        .cmp = (((uint16_t)resp[6]) << 8) | resp[7],
        .product_num = resp[1],
        .revision = resp[8]
    };
}

void si4713_update_status(SI4713Device* device) {
    furi_assert(device);
    device->buffer[0] = SI4710_CMD_GET_INT_STATUS;
    device->buffer[1] = 0x00;
    uint8_t resp[8];
    si4713_trx_command(device,device->buffer,
                       2, resp,8);
    device->status = (SI4713TuneStatus){
        .freq = (((uint16_t)resp[2]) << 8) | resp[3],
        .dBuV = resp[5],
        .ant_cap = resp[6],
        .noise_level = resp[7],
    };
}

void si4713_reset(SI4713Device* device) {
    furi_assert(device);
    FURI_LOG_D(TAG, "Resetting device");
    // exactly translated from the arduino library:
    // https://github.com/adafruit/Adafruit-Si4713-Library/blob/master/Adafruit_Si4713.cpp#L71
    furi_hal_gpio_write(device->reset_pin, true);
    furi_delay_ms(10);
    furi_hal_gpio_write(device->reset_pin, false);
    furi_delay_ms(10);
    furi_hal_gpio_write(device->reset_pin, true);
}

void si4713_powerup(SI4713Device* device) {
    furi_assert(device);
    FURI_LOG_D(TAG, "Sending powerup command to device");
    // https://github.com/adafruit/Adafruit-Si4713-Library/blob/master/Adafruit_Si4713.cpp#L319
    device->buffer[0] = SI4710_CMD_POWER_UP;
    device->buffer[1] = 0x12;
    device->buffer[2] = 0x50; // analog input mode (headphone jack)
    si4713_send_command(device, device->buffer, 3);

    // explained here
    // https://github.com/adafruit/Adafruit-Si4713-Library/blob/master/Adafruit_Si4713.cpp#L329
    si4713_set_property(device, SI4713_PROP_REFCLK_FREQ, 0x8000);
    si4713_set_property(device, SI4713_PROP_TX_PREEMPHASIS, 0x0000);
    si4713_set_property(device, SI4713_PROP_TX_ACOMP_GAIN, 0x000a);
    si4713_set_property(device, SI4713_PROP_TX_ACOMP_ENABLE, 0x0000);
}

void si4713_set_tx_power(SI4713Device* device,
                         uint8_t power,
                         uint8_t antcap) {
    furi_assert(device);
    device->buffer[0] = SI4710_CMD_TX_TUNE_POWER;
    device->buffer[1] = 0x00;
    device->buffer[2] = 0x00;
    device->buffer[3] = power;
    device->buffer[4] = antcap;
    si4713_send_command(device,
                        device->buffer,
                        5);
}

void si4713_tune_fm(SI4713Device* device,
                    uint16_t freq) {
    furi_assert(device);
    FURI_LOG_D(TAG, "Tuning frequency to %d", freq);
    device->buffer[0] = SI4710_CMD_TX_TUNE_FREQ;
    device->buffer[1] = 0x00;
    device->buffer[2] = freq >> 8;
    device->buffer[3] = freq & 0xFF;
    si4713_send_command(device,
                        device->buffer,
                        4);
    while ((si4713_get_status(device) & 0x81) != 0x81)
        furi_delay_ms(10);
}

// C0 (pin 16) -> SCL
// C1 (pin 15) -> SDA
// RX (pin 14) -> RST
// i've found that pin 14 works for me
// but is reconfigurable if you'd like to
// use the uart interface while using
// the board (for whatever reason)
SI4713Device* si4713_alloc(uint8_t addr,
                           const GpioPin* reset,
                           bool initialize) {
    FURI_LOG_D(TAG, "Allocating device");
    SI4713Device* device = malloc(sizeof(SI4713Device));

    device->address = addr;

    FURI_LOG_D(TAG, "Initializing GPIO reset pin");
    furi_hal_gpio_init(reset, GpioModeOutputPushPull, GpioPullNo, GpioSpeedHigh);
    device->reset_pin = reset;
    FURI_LOG_D(TAG, "Successfully initialized reset pin");

    if (initialize) {
        si4713_reset(device);
        furi_delay_ms(10); // wait a litte before sending commands
        si4713_powerup(device);

        si4713_update_info(device);

        if(device->info.product_num != 13)
            // 13 is the number adafruit wants (and what this library is based off of)
            FURI_LOG_W(
                TAG, "Unexpected product number (expected 13, got %d)", device->info.product_num);
    }

    return device;
}

void si4713_free(SI4713Device* device, bool reset_low) {
    furi_assert(device);
    if (reset_low) furi_hal_gpio_write(device->reset_pin, false);
    free(device);
}