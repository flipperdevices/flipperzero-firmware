#pragma once
#include <furi.h>
#include "key-info.h"
#include "key-reader.h"
#include "key-emulator.h"
#include "../ibutton-key.h"
#include <one_wire_master.h>
#include <one_wire_slave.h>

class KeyWorker {
public:
    enum class Error : uint8_t {
        EMPTY,
        CRC_ERROR,
        NOT_ARE_KEY,
        OK,
    };

    KeyWorker::Error read(iButtonKey* key);
    void start_read();
    void stop_read();

    bool emulated();
    void start_emulate(iButtonKey* key);
    void stop_emulate();

    KeyWorker(const GpioPin* one_wire_gpio);

private:
    // one wire
    OneWireMaster onewire_master;
    OneWireSlave onewire_slave;
    KeyReader key_reader;
    KeyEmulator key_emulator;
};