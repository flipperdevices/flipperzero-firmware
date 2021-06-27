#pragma once
#include "key-info.h"
#include "rfid-reader.h"
#include "rfid-writer.h"
#include "rfid-timer-emulator.h"

class RfidWorker {
public:
    RfidWorker();
    ~RfidWorker();

    bool read();
    void stop_read();

    bool write();
    void stop_write();

    bool emulate();
    void stop_emulate();

private:
    RfidWriter writer;
    RfidReader reader;
    RfidTimerEmulator emulator;
};