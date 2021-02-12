#pragma once
#include <stdint.h>
#include <atomic>

class CyfralDecoder {
public:
    bool read(uint8_t* data, uint8_t data_size);
    void process_front(bool polarity, uint32_t time);

    CyfralDecoder();

private:
    enum class BitState : uint8_t {
        WAIT_FRONT_HIGH,
        WAIT_FRONT_LOW,
    };

    enum class State : uint8_t {
        WAIT_START_NIBBLE,
        READ_NIBBLE,
        READ_STOP_NIBBLE,
    };

    State state;
    BitState bit_state;

    bool process_bit(bool polarity, uint32_t time, bool* readed, bool* readed_value);
    void reset_state();
    bool nibble_valid(uint8_t data);

    uint32_t period_time;
    std::atomic<bool> ready;
    uint8_t data[4];
    uint8_t sync_data;
    uint8_t index;
    uint8_t bit_index;

    uint32_t max_cyfral_period;
};
