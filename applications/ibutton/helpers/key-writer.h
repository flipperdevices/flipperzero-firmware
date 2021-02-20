#pragma once
#include "../ibutton-key.h"
#include <one_wire_master.h>

class KeyWriter {
public:
    KeyWriter(OneWireMaster* onewire_master);
    ~KeyWriter();

    void write(iButtonKey* key);
    void start();
    void stop();

private:
    OneWireMaster* onewire_master;
};
