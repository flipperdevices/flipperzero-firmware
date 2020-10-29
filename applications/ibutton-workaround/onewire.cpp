#include <stdio.h>

extern "C" {
    #include "main.h"
    #include "cmsis_os.h"
    #include "gpio.h"
}

#include "onewire/onewire.h"

uint8_t onewire_reset(GpioPin pin) {
    uint8_t r;
    uint8_t retries = 125;

    // noInterrupts();
    // app_gpio_init(pin, GpioModeInput);
    // interrupts();
    // wait until the wire is high... just in case
    app_gpio_write(pin, true);
    do {
        if (--retries == 0) return 0;
        delay_us(2);
    } while (!app_gpio_read(pin));

    // noInterrupts();
    app_gpio_write(pin, false); // drive output low
    // interrupts();
    delay_us(480);
    // noInterrupts();
    app_gpio_write(pin, true); // allow it to float
    delay_us(70);

    r = !app_gpio_read(pin);

    // interrupts();

    delay_us(410);

    return r;
}

uint8_t onewire_read_bit(GpioPin pin) {
    uint8_t r;

    // noInterrupts();
    app_gpio_write(pin, false); // drive output low
    delay_us(3);
    app_gpio_write(pin, true); // allow it to float
    delay_us(10);
    r = app_gpio_read(pin);
    // interrupts();
    delay_us(53);
    return r;
}

uint8_t onewire_read(GpioPin pin) {
    uint8_t r = 0;

    for (uint8_t bitMask = 0x01; bitMask; bitMask <<= 1) {
        if (onewire_read_bit(pin)) {
            r |= bitMask;
        }
    }

    return r;
}

void onewire_read_bytes(GpioPin pin, uint8_t *buf, uint16_t count) {
    for (uint16_t i = 0 ; i < count ; i++) {
        buf[i] = onewire_read(pin);
    }
}

void onewire_write_bit(GpioPin pin, uint8_t v) {
    if (v & 1) {
        // noInterrupts();
        app_gpio_write(pin, false); // drive output low
        delay_us(10);
        app_gpio_write(pin, true); // allow it to float
        // DIRECT_WRITE_HIGH(reg, mask);   // drive output high
        // interrupts();
        delay_us(55);
    } else {
        // noInterrupts();
        app_gpio_write(pin, false); // drive output low
        delay_us(65);
        app_gpio_write(pin, true); // allow it to float
        // DIRECT_WRITE_HIGH(reg, mask);   // drive output high
        // interrupts();
        delay_us(5);
    }
}

void onewire_write(GpioPin pin, uint8_t v) {
    uint8_t bitMask;

    for (bitMask = 0x01; bitMask; bitMask <<= 1) {
        onewire_write_bit(pin, (bitMask & v) ? 1 : 0);
    }
}
