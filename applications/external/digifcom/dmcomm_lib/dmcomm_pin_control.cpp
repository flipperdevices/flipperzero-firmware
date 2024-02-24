// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

namespace DMComm {

void BaseProngOutput::setActiveLevel(uint8_t level) {
    active_level_ = level;
    idle_level_ = level == HIGH ? LOW : HIGH;
}

#ifdef NOT_FLIPPER
DComOutput::DComOutput(uint8_t pin_out, uint8_t pin_notOE)
    : pin_out_(pin_out)
    , pin_notOE_(pin_notOE) {
    setActiveLevel(LOW);
}

DComOutput::~DComOutput() {
    end();
}

void DComOutput::begin() {
    pinModeMaybe(pin_out_, OUTPUT);
    pinModeMaybe(pin_notOE_, OUTPUT);
    release();
}

void DComOutput::end() {
}

void DComOutput::driveActive() {
    digitalWriteMaybe(pin_out_, active_level_);
    digitalWriteMaybe(pin_notOE_, LOW);
}

void DComOutput::driveIdle() {
    digitalWriteMaybe(pin_out_, idle_level_);
    digitalWriteMaybe(pin_notOE_, LOW);
}

void DComOutput::release() {
    digitalWriteMaybe(pin_notOE_, HIGH);
    digitalWriteMaybe(pin_out_, idle_level_); // "don't care" on D-Com, but matters for A-Com
}
#endif
void BaseProngInput::setActiveLevel(uint8_t level) {
    active_level_ = level;
    idle_level_ = level == HIGH ? LOW : HIGH;
}

uint32_t BaseProngInput::waitForActive(uint32_t timeout) {
    return waitFor(true, timeout);
}
uint32_t BaseProngInput::waitForIdle(uint32_t timeout) {
    return waitFor(false, timeout);
}

uint32_t BaseProngInput::waitFor(bool active, uint32_t timeout) {
    uint32_t start_time = micros();
    uint32_t duration;
    while(true) {
        duration = micros() - start_time;
        if(duration > timeout) {
            return DMCOMM_SIGNAL_TIMED_OUT;
        }
        if(active == isActive()) {
            return duration;
        }
    }
}

ReceiveOutcome
    BaseProngInput::waitFrom(bool active, uint32_t dur_min, uint32_t dur_max, int16_t current_bit) {
    ReceiveOutcome outcome = {};
    outcome.current_bit = current_bit;
    outcome.current_bit_active = active;
    outcome.last_duration = waitFor(!active, dur_max);
    if(outcome.last_duration == DMCOMM_SIGNAL_TIMED_OUT) {
        outcome.status = kErrorTimeout;
        return outcome;
    }
    if(outcome.last_duration < dur_min) {
        outcome.status = kErrorTooShort;
        return outcome;
    }
    outcome.status = kStatusReceived;
    return outcome;
}

#ifdef NOT_FLIPPER
DigitalProngInput::DigitalProngInput(uint8_t pin_in)
    : pin_in_(pin_in) {
    setActiveLevel(LOW);
}

DigitalProngInput::~DigitalProngInput() {
    end();
}

void DigitalProngInput::begin() {
    pinMode(pin_in_, INPUT);
}

void DigitalProngInput::end() {
}

void DigitalProngInput::setThreshold(uint16_t threshold_mV) {
    UNUSED(threshold_mV);
}

bool DigitalProngInput::isActive() {
    return digitalRead(pin_in_) == active_level_;
}
#endif

AnalogProngInput::AnalogProngInput(
    uint8_t pin_in,
    uint16_t reference_voltage_mV,
    uint8_t read_resolution)
    : pin_in_(pin_in)
    , reference_voltage_mV_(reference_voltage_mV)
    , read_resolution_(read_resolution) {
    setActiveLevel(LOW);
    setThreshold(reference_voltage_mV / 2);
}

AnalogProngInput::~AnalogProngInput() {
    end();
}

void AnalogProngInput::begin() {
    //pinMode(pin_in_, INPUT);
}

void AnalogProngInput::end() {
}

void AnalogProngInput::setThreshold(uint16_t threshold_mV) {
    uint32_t max_input = (1 << read_resolution_) - 1;
    threshold_mV_ = threshold_mV;
    threshold_units_ = (uint16_t)(max_input * threshold_mV / reference_voltage_mV_);
}

bool AnalogProngInput::isActive() {
    //uint16_t read = analogRead(pin_in_);
    //return (read >= threshold_units_) != (active_level_ == LOW);  // != as XOR
    return true;
}

uint16_t AnalogProngInput::voltage() {
    //uint32_t max_input = (1 << read_resolution_) - 1;
    //uint32_t read = analogRead(pin_in_);
    //return (uint16_t) (read * reference_voltage_mV_ / max_input);
    return 0;
}

} // namespace DMComm
