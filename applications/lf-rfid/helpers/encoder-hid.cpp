#include "encoder-hid.h"
#include <furi.h>

void EncoderHID::init(const uint8_t* data, const uint8_t data_size) {
    card_data[0] = 0b00011101010101010101100101010101;
    card_data[1] = 0b01010101011010101010011010011010;
    card_data[2] = 0b01010101101010011010100101010110;

    card_data_index = 0;
}

void EncoderHID::get_next(bool* polarity, uint16_t* period, uint16_t* pulse) {
    // hid 0 is 6 cycles by 8 clocks
    // hid 1 is 5 cycles by 10 clocks
    const uint8_t hid_0_period = 8;
    const uint8_t hid_0_count = 6;
    const uint8_t hid_1_period = 10;
    const uint8_t hid_1_count = 5;

    bool bit = (card_data[card_data_index / 32] >> (31 - (card_data_index % 32))) & 1;

    *polarity = true;
    if(bit) {
        *period = hid_1_period;
        *pulse = hid_1_period / 2;

        bit_index++;
        if(bit_index >= hid_1_count) {
            bit_index = 0;
            card_data_index++;
            if(card_data_index >= (32 * card_data_max)) {
                card_data_index = 0;
            }
        }
    } else {
        *period = hid_0_period;
        *pulse = hid_0_period / 2;

        bit_index++;
        if(bit_index >= hid_0_count) {
            bit_index = 0;
            card_data_index++;
            if(card_data_index >= (32 * card_data_max)) {
                card_data_index = 0;
            }
        }
    }
}
