#include "key-worker.h"
#include <callback-connector.h>
#include <maxim_crc.h>

extern COMP_HandleTypeDef hcomp1;

KeyWorker::Error KeyWorker::read(iButtonKey* key) {
    KeyWorker::Error result = KeyWorker::Error::EMPTY;

    KeyReader::Error key_reader_result = key_reader.read(key);

    switch(key_reader_result) {
    case KeyReader::Error::EMPTY:
        result = KeyWorker::Error::EMPTY;
        break;
    case KeyReader::Error::CRC_ERROR:
        result = KeyWorker::Error::CRC_ERROR;
        break;
    case KeyReader::Error::NOT_ARE_KEY:
        result = KeyWorker::Error::NOT_ARE_KEY;
        break;
    case KeyReader::Error::OK:
        result = KeyWorker::Error::OK;
        break;
    }

    return result;
}

void KeyWorker::start_read() {
    key_reader.start();
}

void KeyWorker::stop_read() {
    key_reader.stop();
}

bool KeyWorker::emulated() {
    return key_emulator.emulated();
}

void KeyWorker::start_emulate(iButtonKey* key) {
    key_emulator.start(key);
}

void KeyWorker::stop_emulate() {
    key_emulator.stop();
}

KeyWorker::KeyWorker(const GpioPin* one_wire_gpio)
    : onewire_master{one_wire_gpio}
    , onewire_slave{one_wire_gpio}
    , key_reader{&onewire_master}
    , key_emulator{&onewire_slave} {
}
