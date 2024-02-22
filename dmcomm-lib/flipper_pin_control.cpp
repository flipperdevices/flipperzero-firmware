#include "flipper_pin_control.h"

namespace DMComm {

FComOutput::FComOutput(const GpioPin* pin_out) :
        pin_out_(pin_out) {
    setActiveLevel(LOW);
}

FComOutput::~FComOutput() {
    
}

void FComOutput::begin() {
    furi_hal_gpio_init(pin_out_, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    release();
}

void FComOutput::end() {}

void FComOutput::driveActive() {
    furi_hal_gpio_write(pin_out_, active_level_);
}

void FComOutput::driveIdle() {
    furi_hal_gpio_write(pin_out_, idle_level_);
}

void FComOutput::release() {
    furi_hal_gpio_write(pin_out_, idle_level_);
}

FComInput::FComInput(const GpioPin* pin_in) : pin_in_(pin_in) {
    setActiveLevel(LOW);
}

FComInput::~FComInput() {
}

void FComInput::begin() {
    furi_hal_gpio_init(pin_in_, GpioModeInput, GpioPullNo, GpioSpeedLow);
}

void FComInput::end() {}

void FComInput::setThreshold(uint16_t threshold_mV) {
    UNUSED(threshold_mV);
}

bool FComInput::isActive() {
    uint8_t level = furi_hal_gpio_read(pin_in_) ? HIGH : LOW;
    return level == active_level_;
}

}