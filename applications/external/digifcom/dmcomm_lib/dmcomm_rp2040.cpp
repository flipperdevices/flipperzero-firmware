// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

#ifdef ARDUINO_ARCH_MBED_RP2040

#include <hardware/structs/sio.h>

namespace DMComm {

PComOutput::PComOutput(uint8_t pin_drive_signal, uint8_t pin_drive_low, uint8_t pin_weak_pull)
    : pin_drive_signal_(pin_drive_signal)
    , pin_drive_low_(pin_drive_low)
    , pin_weak_pull_(pin_weak_pull) {
    pinmask_drive_signal_ = 1 << pin_drive_signal;
    pinmask_drive_both_ = pinmask_drive_signal_ | (1 << pin_drive_low);
    setActiveLevel(LOW);
}

PComOutput::~PComOutput() {
    end();
}

void PComOutput::begin() {
    pinMode(pin_drive_signal_, INPUT);
    pinMode(pin_drive_low_, INPUT);
    pinMode(pin_weak_pull_, OUTPUT);
    release();
}

void PComOutput::end() {
}

void PComOutput::driveActive() {
    if(active_level_ == LOW) {
        sio_hw->gpio_clr = pinmask_drive_signal_; // drive_signal pin -> low
    } else {
        sio_hw->gpio_set = pinmask_drive_signal_; // drive_signal pin -> high
    }
    sio_hw->gpio_oe_set = pinmask_drive_both_; // both drive pins -> output (at the same time)
}

void PComOutput::driveIdle() {
    if(active_level_ == LOW) {
        sio_hw->gpio_set = pinmask_drive_signal_; // drive_signal pin -> high
    } else {
        sio_hw->gpio_clr = pinmask_drive_signal_; // drive_signal pin -> low
    }
    sio_hw->gpio_oe_set = pinmask_drive_both_; // both drive pins -> output (at the same time)
}

void PComOutput::release() {
    digitalWrite(pin_weak_pull_, idle_level_); // configure weak pull
    sio_hw->gpio_oe_clr = pinmask_drive_both_; // both drive pins -> input (at the same time)
}

} // namespace DMComm

#endif // RP2040
