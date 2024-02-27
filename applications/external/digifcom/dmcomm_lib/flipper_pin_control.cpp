#include "flipper_pin_control.h"
#include "DMComm.h"

namespace DMComm {

FComOutput::FComOutput(const GpioPin* pin_out)
    : pin_out_(pin_out) {
    setActiveLevel(LOW);
}

FComOutput::~FComOutput() {
}

void FComOutput::begin() {
    furi_hal_gpio_init(pin_out_, GpioModeOutputPushPull, GpioPullNo, GpioSpeedLow);
    release();
}

void FComOutput::end() {
}

void FComOutput::driveActive() {
    furi_hal_gpio_write(pin_out_, active_level_);
}

void FComOutput::driveIdle() {
    furi_hal_gpio_write(pin_out_, idle_level_);
}

void FComOutput::release() {
    furi_hal_gpio_write(pin_out_, idle_level_);
}

FComInput::FComInput(const GpioPin* pin_in)
    : pin_in_(pin_in) {
    setActiveLevel(LOW);
}

FComInput::~FComInput() {
}

void FComInput::begin() {
    furi_hal_gpio_init(pin_in_, GpioModeInput, GpioPullNo, GpioSpeedLow);
}

void FComInput::end() {
}

void FComInput::setThreshold(uint16_t threshold_mV) {
    UNUSED(threshold_mV);
}

bool FComInput::isActive() {
    uint8_t level = furi_hal_gpio_read(pin_in_) ? HIGH : LOW;
    return level == active_level_;
}

/*
NOTE: We override this because if we loop without delays on the
flipper, we starve the process scheduler thread... which means
the UI locks up. This isn't an issue for _most_ operations because
they are all 13ms or less. But in listen mode or go-second mode
we are waiting for _FIVE SECONDS_ which is really annoying for the
user being unable to back out of the app during that time.

SO. If the delay is really big, we're going to add a 1ms delay
every loop to allow the UI to process events and stuff so the 
user isn't left in the dust.

For tight timeouts though, we will keep the original code.

Technically, this should be done using rising/falling edge
interrupts... However, that's pretty complicated to implement
because we need to determine which state is active, what current
state we are in, call into C code and manage waiting on a lock.
And even then lock timeouts aren't microsecond accurate, so it
still wouldn't be something we could use for everything...

This is "good enough".
*/
uint32_t FComInput::waitFor(bool active, uint32_t timeout) {
    if(timeout < 250000) {
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
    } else {
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
            delay(1);
        }
    }
}

}