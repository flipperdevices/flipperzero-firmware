#pragma once
#include "flipper.h"
#include "flipper_v2.h"
#include "one_wire_timings.h"

class OneWireAsyncSlave {
private:
    enum OWState {
        OW_WAIT_RESET_FALL,
        OW_WAIT_RESET_RISE,
        OW_SEND_PRESENCE,
    };

public:
    void start(void);
    void stop(void);
    OneWireAsyncSlave();
    ~OneWireAsyncSlave();
};