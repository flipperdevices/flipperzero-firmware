#pragma once
#include "flipper.h"
#include "flipper_v2.h"

enum class CyfralReaderGpioError : uint8_t {
    NO_ERROR = 0,
    UNABLE_TO_DETECT = 1,
    RAW_DATA_SIZE_ERROR = 2,
    UNKNOWN_NIBBLE_VALUE = 3,
    NO_START_NIBBLE = 4,
};

class CyfralReaderGpio {
private:
    void capture_data(bool* data, uint16_t capture_size);
    bool parse_data(bool* raw_data, uint16_t capture_size, uint8_t* data, uint8_t count);
    uint32_t search_array_in_array(
        const bool* haystack,
        const uint32_t haystack_size,
        const bool* needle,
        const uint32_t needle_size);

    // key is 9 nibbles
    static const uint16_t bits_in_nibble = 4;
    static const uint16_t key_length = 9;
    static const uint32_t capture_size = key_length * bits_in_nibble * 2;
    CyfralReaderGpioError error;
    const GpioPin* pin_record;

public:
    CyfralReaderGpio(const GpioPin* emulate_pin);
    ~CyfralReaderGpio();
    void start(void);
    void stop(void);
    bool read(uint8_t* data, uint8_t count);
};

void CyfralReaderGpio::capture_data(bool* data, uint16_t capture_size) {
}

uint32_t CyfralReaderGpio::search_array_in_array(
    const bool* haystack,
    const uint32_t haystack_size,
    const bool* needle,
    const uint32_t needle_size) {
    uint32_t haystack_index = 0, needle_index = 0;

    while(haystack_index < haystack_size && needle_index < needle_size) {
        if(haystack[haystack_index] == needle[needle_index]) {
            haystack_index++;
            needle_index++;
            if(needle_index == needle_size) {
                return (haystack_index - needle_size);
            };
        } else {
            haystack_index = haystack_index - needle_index + 1;
            needle_index = 0;
        }
    }

    return haystack_index;
}

bool CyfralReaderGpio::parse_data(
    bool* raw_data,
    uint16_t capture_size,
    uint8_t* data,
    uint8_t count) {
    const bool start_nibble[bits_in_nibble] = {1, 1, 1, 0};
    uint32_t start_position =
        search_array_in_array(raw_data, capture_size, start_nibble, bits_in_nibble);
    uint32_t end_position = 0;

    memset(data, 0, count);

    if(start_position < capture_size) {
        start_position = start_position + bits_in_nibble;
        end_position = start_position + count * 2 * bits_in_nibble;

        if(end_position >= capture_size) {
            error = CyfralReaderGpioError::RAW_DATA_SIZE_ERROR;
            return false;
        }

        bool first_nibble = true;
        uint8_t data_position = 0;
        uint8_t nibble_value = 0;

        while(data_position < count) {
            nibble_value = !raw_data[start_position] << 3 | !raw_data[start_position + 1] << 2 |
                           !raw_data[start_position + 2] << 1 | !raw_data[start_position + 3];

            switch(nibble_value) {
            case(0x7):
            case(0xB):
            case(0xD):
            case(0xE):
                break;
            default:
                error = CyfralReaderGpioError::UNKNOWN_NIBBLE_VALUE;
                return false;
                break;
            }

            if(first_nibble) {
                data[data_position] |= nibble_value << 4;
            } else {
                data[data_position] |= nibble_value;
            }

            first_nibble = !first_nibble;

            if(first_nibble) {
                data_position++;
            }

            start_position = start_position + bits_in_nibble;
        }

        error = CyfralReaderGpioError::NO_ERROR;
        return true;
    }

    error = CyfralReaderGpioError::NO_START_NIBBLE;
    return false;
}

CyfralReaderGpio::CyfralReaderGpio(const GpioPin* gpio_pin) {
    pin_record = gpio_pin;
}

CyfralReaderGpio::~CyfralReaderGpio() {
}

void CyfralReaderGpio::start(void) {
    // pulldown lf-rfid pins to prevent interference
    // TODO open record
    GpioPin rfid_pull_pin = {.port = RFID_PULL_GPIO_Port, .pin = RFID_PULL_Pin};
    gpio_init((GpioPin*)&rfid_pull_pin, GpioModeOutputOpenDrain);
    gpio_write((GpioPin*)&rfid_pull_pin, false);

    // TODO open record
    GpioPin rfid_out_pin = {.port = RFID_OUT_GPIO_Port, .pin = RFID_OUT_Pin};
    gpio_init((GpioPin*)&rfid_out_pin, GpioModeOutputOpenDrain);
    gpio_write((GpioPin*)&rfid_out_pin, false);
}

void CyfralReaderGpio::stop(void) {
}

bool CyfralReaderGpio::read(uint8_t* data, uint8_t count) {
    bool raw_data[capture_size];
    bool result = false;
    error = CyfralReaderGpioError::NO_ERROR;

    // TODO think about other detection method
    // key not on line
    /*if(line_level_max > 2000) {
        error = CyfralReaderGpioError::UNABLE_TO_DETECT;
        return false;
    }*/

    // capturing raw data consisting of bits
    capture_data(raw_data, capture_size);

    // parse captured data
    if(parse_data(raw_data, capture_size, data, count)) {
        result = true;
    }

    return result;
}