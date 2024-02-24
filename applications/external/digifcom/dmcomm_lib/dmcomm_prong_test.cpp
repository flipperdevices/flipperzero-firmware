// This file is part of the DMComm project by BladeSabre. License: MIT.

#include "DMComm.h"

namespace DMComm {

ProngTester::ProngTester(BaseProngOutput& output, BaseProngInput& input)
    : output_(&output)
    , input_(&input)
    , analog_input_(nullptr) {
}

ProngTester::ProngTester(
    BaseProngOutput& output,
    BaseProngInput& input,
    AnalogProngInput& analog_input)
    : output_(&output)
    , input_(&input)
    , analog_input_(&analog_input) {
}

void ProngTester::run(Print& dest) {
    output_->begin();
    input_->begin();
    dest.println(F("[digital]"));
    run1(dest, false);
    if(analog_input_ != nullptr) {
        analog_input_->begin();
        dest.println(F("[analog]"));
        run1(dest, true);
    }
}

void ProngTester::setActiveLevel(uint8_t level) {
    output_->setActiveLevel(level);
    input_->setActiveLevel(level);
    if(analog_input_ != nullptr) {
        analog_input_->setActiveLevel(level);
    }
}

void ProngTester::run1(Print& dest, bool analog) {
    setActiveLevel(LOW);
    output_->release();
    dest.print(F("active_low_release = "));
    print_level(dest, analog);

    output_->driveActive();
    dest.print(F("active_low_drive_active = "));
    print_level(dest, analog);

    output_->driveIdle();
    dest.print(F("active_low_drive_idle = "));
    print_level(dest, analog);

    setActiveLevel(HIGH);
    output_->release();
    dest.print(F("active_high_release = "));
    print_level(dest, analog);

    output_->driveActive();
    dest.print(F("active_high_drive_active = "));
    print_level(dest, analog);

    output_->driveIdle();
    dest.print(F("active_high_drive_idle = "));
    print_level(dest, analog);
}

void ProngTester::print_level(Print& dest, bool analog) {
    delay(10);
    if(analog) {
        dest.println(analog_input_->voltage());
    } else if(input_->isActive()) {
        dest.println(F("\"active\""));
    } else {
        dest.println(F("\"idle\""));
    }
}

} // namespace DMComm
