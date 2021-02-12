#include "key-worker.h"
#include <callback-connector.h>
#include <maxim_crc.h>

extern COMP_HandleTypeDef hcomp1;

KeyWorker::Error KeyWorker::read(iButtonKey* key) {
    uint8_t tmp_key_data[8] = {0, 0, 0, 0, 0, 0, 0, 0};
    iButtonKeyType key_type;

    KeyWorker::Error result = KeyWorker::Error::EMPTY;

    if(read_key(&key_type, tmp_key_data, 8)) {
        switch(key_type) {
        case iButtonKeyType::KeyDallas:
            if(verify_key(key_type, tmp_key_data, 8)) {
                if(maxim_crc8(tmp_key_data, 8) == 0) {
                    if(tmp_key_data[0] == 0x01) {
                        result = KeyWorker::Error::OK;
                    } else {
                        result = KeyWorker::Error::NOT_ARE_KEY;
                    }
                } else {
                    result = KeyWorker::Error::CRC_ERROR;
                }
            }

            break;
        case iButtonKeyType::KeyCyfral:
            result = KeyWorker::Error::OK;
            break;
        case iButtonKeyType::KeyMetakom:
            result = KeyWorker::Error::OK;
            break;
        }

        if(result != KeyWorker::Error::EMPTY) {
            key->set_type(key_type);
            key->set_data(tmp_key_data, 8);
        }
    }

    switch_mode_if_needed();

    return result;
}

KeyWorker::KeyWorker(const GpioPin* one_wire_gpio)
    : onewire_master{one_wire_gpio}
    , onewire_slave{one_wire_gpio} {
    read_mode_switch_time = 0;
    read_mode = ReadMode::DALLAS;
    reader_state = State::STATE_INIT;
}

bool KeyWorker::read_key(iButtonKeyType* key_type, uint8_t* data, uint8_t data_size) {
    bool readed = false;

    switch(read_mode) {
    case ReadMode::DALLAS:
        pause_os();
        if(onewire_master.search(data)) {
            onewire_master.reset_search();
            readed = true;
            *key_type = iButtonKeyType::KeyDallas;
        } else {
            onewire_master.reset_search();
        }
        resume_os();
        break;
    case ReadMode::CYFRAL_METAKOM:
        if(cyfral_decoder.read(data, 2)) {
            readed = true;
            *key_type = iButtonKeyType::KeyCyfral;
        } else if(metakom_decoder.read(data, 4)) {
            readed = true;
            *key_type = iButtonKeyType::KeyMetakom;
        }
        break;
    }

    return readed;
}

bool KeyWorker::verify_key(iButtonKeyType key_type, const uint8_t* const data, uint8_t data_size) {
    bool result = true;
    uint8_t tmp_data[4];

    switch(key_type) {
    case iButtonKeyType::KeyDallas:
        switch_to(ReadMode::DALLAS);

        if(onewire_master.reset()) {
            if(onewire_master.reset()) {
                onewire_master.write(0x33);
                for(uint8_t i = 0; i < data_size; i++) {
                    if(onewire_master.read() != data[i]) {
                        result = false;
                        break;
                    }
                }
            } else {
                result = false;
                break;
            }
        }
        break;

    default:
        result = false;
        break;
    }

    return result;
}

void KeyWorker::start_comaparator(void) {
    // pulldown lf-rfid pins to prevent interference
    // TODO open record
    GpioPin rfid_pull_pin = {.port = RFID_PULL_GPIO_Port, .pin = RFID_PULL_Pin};
    gpio_init(&rfid_pull_pin, GpioModeOutputOpenDrain);
    gpio_write(&rfid_pull_pin, false);

    // TODO open record
    GpioPin rfid_out_pin = {.port = RFID_OUT_GPIO_Port, .pin = RFID_OUT_Pin};
    gpio_init(&rfid_out_pin, GpioModeOutputOpenDrain);
    gpio_write(&rfid_out_pin, false);

    auto cmp_cb = cbc::obtain_connector(this, &KeyWorker::comparator_trigger_callback);
    api_interrupt_add(cmp_cb, InterruptTypeComparatorTrigger, this);
    last_dwt_value = DWT->CYCCNT;
    HAL_COMP_Start(&hcomp1);
}

void KeyWorker::stop_comaparator(void) {
    HAL_COMP_Stop(&hcomp1);
    auto cmp_cb = cbc::obtain_connector(this, &KeyWorker::comparator_trigger_callback);
    api_interrupt_remove(cmp_cb, InterruptTypeComparatorTrigger);
}

void KeyWorker::comparator_trigger_callback(void* hcomp, void* comp_ctx) {
    COMP_HandleTypeDef* _hcomp = static_cast<COMP_HandleTypeDef*>(hcomp);
    KeyWorker* _this = static_cast<KeyWorker*>(comp_ctx);

    if(hcomp == &hcomp1) {
        _this->cyfral_decoder.process_front(
            (HAL_COMP_GetOutputLevel(_hcomp) == COMP_OUTPUT_LEVEL_HIGH),
            DWT->CYCCNT - last_dwt_value);

        _this->metakom_decoder.process_front(
            (HAL_COMP_GetOutputLevel(_hcomp) == COMP_OUTPUT_LEVEL_HIGH),
            DWT->CYCCNT - last_dwt_value);

        last_dwt_value = DWT->CYCCNT;
    }
}

void KeyWorker::pause_os() {
    __disable_irq();
}

void KeyWorker::resume_os() {
    __enable_irq();
}

void KeyWorker::switch_to(ReadMode mode) {
    switch(mode) {
    case ReadMode::DALLAS:
        onewire_master.start();
        stop_comaparator();
        break;
    case ReadMode::CYFRAL_METAKOM:
        onewire_master.stop();
        start_comaparator();
        break;
    }

    read_mode = mode;
}

void KeyWorker::switch_mode_if_needed() {
    if(osKernelGetTickCount() - read_mode_switch_time > (osKernelGetTickFreq() / 50)) {
        read_mode_switch_time = osKernelGetTickCount();
        switch(read_mode) {
        case ReadMode::DALLAS:
            switch_to(ReadMode::CYFRAL_METAKOM);
            break;
        case ReadMode::CYFRAL_METAKOM:
            switch_to(ReadMode::DALLAS);
            break;
        }
    }
}
