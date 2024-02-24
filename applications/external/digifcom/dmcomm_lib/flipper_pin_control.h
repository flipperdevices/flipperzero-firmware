#pragma once
#include "dmcomm_lib/dmcomm_pin_control.h"
#include "flipper.h"

namespace DMComm {

class FComOutput : public BaseProngOutput {
public:
    FComOutput(const GpioPin* pin_out);
    ~FComOutput();
    void begin();
    void end();
    void driveActive();
    void driveIdle();
    void release();

private:
    const GpioPin* pin_out_;
};

class FComInput : public BaseProngInput {
public:
    FComInput(const GpioPin* pin_in);
    ~FComInput();
    void begin();
    void end();
    bool isActive();
    void setThreshold(uint16_t threshold_mV);
    uint16_t voltage();

private:
    const GpioPin* pin_in_;
};

}