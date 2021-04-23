#pragma once
#include <api-hal.h>
#include "key-info.h"

class RfidEmulator {
public:
    RfidEmulator();
    void start();
    void stop();
    void emulate(LfrfidKeyType type, uint8_t* data, uint8_t data_size);

private:
    void do_gpio(bool data, float time);
    void do_emarine(void);
    void do_emarine_old(void);

    void do_hid(void);
    void hid_0(void);
    void hid_1(void);

    GpioPin pull_pin = {.port = RFID_PULL_GPIO_Port, .pin = RFID_PULL_Pin};
    GpioPin* pull_pin_record = &pull_pin;

    GpioPin out_pin = {.port = RFID_OUT_GPIO_Port, .pin = RFID_OUT_Pin};
    GpioPin* out_pin_record = &out_pin;
};
