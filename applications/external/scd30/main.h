#pragma once

#include <memory>
#include <stdint.h>

#include "i2c.h"

struct SCD30;

extern "C" int32_t scd30_main();

enum class EventID {
    None,
    Unknown,
    Key,
    Tick,
};

struct EventMessage {
    EventID id;
    InputEvent event;
};

struct State {
    FuriTimer* timer = nullptr;
    I2C* i2c = nullptr;
    uint32_t timerFrequency = 1;
    std::unique_ptr<SCD30> scd30;
    bool lastReadResult = false;
    bool hasRead = false;

    bool init(FuriMessageQueue*);
    bool readData();
};
