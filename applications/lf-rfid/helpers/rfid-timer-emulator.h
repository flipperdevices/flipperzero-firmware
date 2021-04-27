#pragma once
#include <api-hal.h>
#include "key-info.h"

class RfidTimerEmulator {
public:
    enum class Type : uint8_t {
        HID,
        Indala,
    };

    RfidTimerEmulator();
    void start(Type type);
    void stop();
    void emulate();

private:
};