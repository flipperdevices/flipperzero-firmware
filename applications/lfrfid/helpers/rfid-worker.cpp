#include "rfid-worker.h"

RfidWorker::RfidWorker() {
}

RfidWorker::~RfidWorker() {
}

void RfidWorker::start_read() {
    reader.start(RfidReader::Type::Normal);
}

bool RfidWorker::read() {
    static const uint8_t data_size = LFRFID_KEY_SIZE;
    uint8_t data[data_size] = {0};
    LfrfidKeyType type;

    bool result = reader.read(&type, data, data_size);

    if(result) {
        key.set_type(type);
        key.set_data(data, data_size);
    };

    return result;
}

void RfidWorker::stop_read() {
    reader.stop();
}

void RfidWorker::start_write() {
}

bool RfidWorker::write() {
    return false;
}

void RfidWorker::stop_write() {
}
