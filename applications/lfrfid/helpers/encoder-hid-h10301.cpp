#include "encoder-hid-h10301.h"
#include "protocols/protocol-hid-h10301.h"
#include <furi.h>

void EncoderHID_H10301::init(const uint8_t* data, const uint8_t data_size) {
    ProtocolHID10301 hid;
    hid.encode(data, data_size, reinterpret_cast<uint8_t*>(&card_data), sizeof(card_data) * 3);

    card_data_index = 0;
    bit_index = 0;
    add_zero_pulse = true;
}

void EncoderHID_H10301::write_bit(bool bit, uint8_t position) {
    write_raw_bit(bit, position + 0);
    write_raw_bit(!bit, position + 1);
}

void EncoderHID_H10301::write_raw_bit(bool bit, uint8_t position) {
    if(bit) {
        card_data[position / 32] |= 1UL << (31 - (position % 32));
    } else {
        card_data[position / 32] &= ~(1UL << (31 - (position % 32)));
    }
}

void EncoderHID_H10301::increase_index() {
    card_data_index++;
    if(card_data_index >= (32 * card_data_max)) {
        card_data_index = 0;
    }
}

typedef struct {
    const uint8_t period;
    const uint8_t count;
} FSKInfo;

void EncoderHID_H10301::get_next(bool* polarity, uint16_t* period, uint16_t* pulse) {
    const FSKInfo hid_fsk[2] = {
        {.period = 8, .count = 6},
        {.period = 10, .count = 5},
    };

    uint8_t bit = (card_data[card_data_index / 32] >> (31 - (card_data_index % 32))) & 1;
    *polarity = true;

    *period = hid_fsk[bit].period;

    bit_index++;
    if(bit_index >= hid_fsk[bit].count) {
        // add zero pulse every 4 bits
        if(card_data_index % 8 == 0 && add_zero_pulse) {
            add_zero_pulse = false;
            *period = hid_fsk[0].period;
        } else {
            bit_index = 0;
            add_zero_pulse = true;
            increase_index();
        }
    }

    *pulse = *period / 2;
}
