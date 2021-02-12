#include "cyfral-decoder.h"
#include <furi.h>

extern UART_HandleTypeDef DEBUG_UART;

void low_level_printf(const char* s, ...) {
    char buf[129];
    va_list attr;
    va_start(attr, s);
    vsnprintf(buf, 128, s, attr);
    HAL_UART_Transmit(&DEBUG_UART, (uint8_t*)buf, (uint16_t)129, HAL_MAX_DELAY);
    va_end(attr);
}

void CyfralDecoder::reset_state() {
    state = CyfralDecoder::State::WAIT_START_NIBBLE;
    bit_state = CyfralDecoder::BitState::WAIT_FRONT_LOW;

    period_time = 0;
    bit_index = 0;
    ready = false;
    index = 0;

    sync_data = 0;
    data[1] = 0;
    data[2] = 0;
    data[3] = 0;
    sync_data = 0;
}

bool CyfralDecoder::nibble_valid(uint8_t data) {
    uint8_t data_value = data & 0x0F;

    switch(data_value) {
    case 0b1110:
    case 0b1101:
    case 0b1011:
    case 0b0111:
        return true;
        break;
    default:
        return false;
    }
}

CyfralDecoder::CyfralDecoder() {
    reset_state();
    max_cyfral_period = 0;
}

void CyfralDecoder::process_front(bool polarity, uint32_t time) {
    bool readed;
    bool value;

    if(max_cyfral_period == 0) {
        max_cyfral_period = 230 * (SystemCoreClock / 1000000.0f);
    }

    if(ready) return;

    switch(state) {
    case CyfralDecoder::State::WAIT_START_NIBBLE:
        // wait for start sync
        if(process_bit(polarity, time, &readed, &value)) {
            if(readed) {
                sync_data = ((sync_data << 1) | value) & 0x0F;
                if(sync_data == 0b0001) {
                    state = CyfralDecoder::State::READ_NIBBLE;
                }
            }
        } else {
            reset_state();
        }

        break;
    case CyfralDecoder::State::READ_NIBBLE:
        if(process_bit(polarity, time, &readed, &value)) {
            if(readed) {
                data[index] = (data[index] << 1) | value;

                bit_index++;

                if(bit_index == 8) {
                    bit_index = 0;
                    index++;
                }

                if(index == 4) {
                    state = CyfralDecoder::State::READ_STOP_NIBBLE;
                }
            }
        } else {
            reset_state();
        }
        break;
    case CyfralDecoder::State::READ_STOP_NIBBLE:
        // wait for stop sync
        if(process_bit(polarity, time, &readed, &value)) {
            if(readed) {
                sync_data = ((sync_data << 1) | value) & 0x0F;
                bit_index++;

                switch(bit_index) {
                case 0:
                case 1:
                case 2:
                case 3:
                    break;
                case 4:
                    if(sync_data == 0b0001) {
                        bool data_valid = true;

                        for(uint8_t i = 0; i < 4; i++) {
                            if(!nibble_valid(data[i])) {
                                data_valid = false;
                                break;
                            } else if(!nibble_valid(data[i] >> 4)) {
                                data_valid = false;
                                break;
                            }
                        }

                        if(data_valid) {
                            ready = true;
                        } else {
                            reset_state();
                        }
                    } else {
                        reset_state();
                    }
                    break;
                default:
                    reset_state();
                    break;
                }
            }
        } else {
            reset_state();
        }
        break;
    }
}

bool CyfralDecoder::process_bit(bool polarity, uint32_t time, bool* readed, bool* readed_value) {
    bool result = true;
    *readed = false;

    // bit start from low
    switch(bit_state) {
    case CyfralDecoder::BitState::WAIT_FRONT_LOW:
        if(polarity == true) {
            period_time += time;

            *readed = true;
            if(period_time <= max_cyfral_period) {
                if((period_time / 2) > time) {
                    *readed_value = false;
                } else {
                    *readed_value = true;
                }
            } else {
                result = false;
            }

            bit_state = CyfralDecoder::BitState::WAIT_FRONT_HIGH;
        } else {
            result = false;
        }
        break;
    case CyfralDecoder::BitState::WAIT_FRONT_HIGH:
        if(polarity == false) {
            period_time = time;
            bit_state = CyfralDecoder::BitState::WAIT_FRONT_LOW;
        } else {
            result = false;
        }
        break;
    }

    return result;
}

bool CyfralDecoder::read(uint8_t* _data, uint8_t data_size) {
    furi_check(data_size <= 4);
    bool result = false;

    if(ready) {
        memcpy(_data, data, data_size);
        reset_state();
        result = true;
    }

    return result;
}
