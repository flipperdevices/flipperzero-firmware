#include <bq27220.h>
#include <bq27220_reg.h>
#include <i2c.h>

uint16_t bq27220_read_word(uint8_t address) {
    uint8_t data[2] = { address };
    if (HAL_I2C_Master_Transmit(&POWER_I2C, BQ27220_ADDRESS, data, 1, 2000) != HAL_OK) {
        return 0;
    }

    if (HAL_I2C_Master_Receive(&POWER_I2C, BQ27220_ADDRESS, data, 2, 2000) != HAL_OK) {
        return 0;
    }
    return *(uint16_t*)data;
}

void bq27220_init() {}

uint16_t bq27220_get_voltage() {
    return bq27220_read_word(CommandVoltage);
}

int16_t bq27220_get_current() {
    return (int16_t)bq27220_read_word(CommandCurrent);
}

uint16_t bq27220_get_full_charge_capacity() {
    return bq27220_read_word(CommandFullChargeCapacity);
}

uint16_t bq27220_get_remaining_capacity() {
    return bq27220_read_word(CommandRemainingCapacity);
}

uint16_t bq27220_get_state_of_charge() {
    return bq27220_read_word(CommandStateOfCharge);
}
