#pragma once
#include "flipper.h"
#include "flipper_v2.h"
#include "one_wire_timings.h"

class OneWireGpio {
private:
    const GpioPin* gpio;

public:
    OneWireGpio(const GpioPin* one_wire_gpio);
    ~OneWireGpio();
    bool reset(void);
    bool read_bit(void);
    uint8_t read(void);
    void read_bytes(uint8_t* buf, uint16_t count);
    void write_bit(bool value);
    void write(uint8_t value);
    void start(void);
    void stop(void);
};