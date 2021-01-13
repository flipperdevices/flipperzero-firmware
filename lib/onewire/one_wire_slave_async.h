#pragma once
#include "flipper.h"
#include "flipper_v2.h"
#include "one_wire_timings.h"

class OneWireAsyncSlave {
private:
public:
    void start(void);
    void stop(void);
    OneWireAsyncSlave();
    ~OneWireAsyncSlave();
};