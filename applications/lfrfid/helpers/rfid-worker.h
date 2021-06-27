#pragma once
#include "key-info.h"
#include "rfid-reader.h"
#include "rfid-writer.h"
#include "rfid-timer-emulator.h"
#include "rfid-key.h"

class RfidWorker {
public:
    RfidWorker();
    ~RfidWorker();

    void start_read();
    bool read();
    void stop_read();

    void start_write();
    bool write();
    void stop_write();

    bool start_emulate();
    void stop_emulate();

    RfidKey key;

private:
    RfidWriter writer;
    RfidReader reader;
    RfidTimerEmulator emulator;
};