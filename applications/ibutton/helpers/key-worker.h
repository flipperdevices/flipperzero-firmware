#pragma once
#include <stdint.h>
#include <furi.h>
#include "cyfral-decoder.h"
#include "metakom-decoder.h"
#include "key-info.h"
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
    KeyWorker(const GpioPin* one_wire_gpio);

private:
    bool read_key(iButtonKeyType* key_type, uint8_t* data, uint8_t data_size);
    bool verify_key(iButtonKeyType key_type, const uint8_t* const data, uint8_t data_size);

    // cyfral and metakom readers data
    void comparator_trigger_callback(void* hcomp, void* comp_ctx);
    void start_comaparator(void);
    void stop_comaparator(void);
    uint32_t last_dwt_value;

    CyfralDecoder cyfral_decoder;
    MetakomDecoder metakom_decoder;

    // mode
    uint32_t read_mode_switch_time;
    enum class ReadMode : uint8_t {
        CYFRAL_METAKOM,
        DALLAS,
    };
    ReadMode read_mode;

    // one wire
    OneWireMaster onewire_master;
    OneWireSlave onewire_slave;

    //
    void pause_os();
    void resume_os();

    void switch_to(ReadMode mode);
    void switch_mode_if_needed();

    // reader state data
    enum class State : uint8_t {
        STATE_INIT,
        STATE_ACQUIRE,
        STATE_READ,
    };

    State reader_state;

    struct ReadedTypes {
        bool cyfral;
        bool metakom;
        bool dallas;
    };

    ReadedTypes readed_types;

    uint32_t reader_start_time;
};