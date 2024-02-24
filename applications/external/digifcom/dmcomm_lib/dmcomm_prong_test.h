// This file is part of the DMComm project by BladeSabre. License: MIT.

#ifndef DMCOMM_PRONG_TEST_H_
#define DMCOMM_PRONG_TEST_H_

#include "DMComm.h"

namespace DMComm {

class ProngTester {
public:
    ProngTester(BaseProngOutput& output, BaseProngInput& input);
    ProngTester(BaseProngOutput& output, BaseProngInput& input, AnalogProngInput& analog_input);
    void run(Print& dest);

private:
    BaseProngOutput* output_;
    BaseProngInput* input_;
    AnalogProngInput* analog_input_;
    void setActiveLevel(uint8_t level);
    void run1(Print& dest, bool analog);
    void print_level(Print& dest, bool analog);
};

} // namespace DMComm

#endif // DMCOMM_PRONG_TEST_H_
