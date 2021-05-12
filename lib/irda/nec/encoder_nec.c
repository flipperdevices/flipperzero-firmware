#include <stdint.h>
#include "../protocol_defs_i.h"
#include "../encoder_common_i.h"


static const EncoderTimings encoder_timings = {
    .bit1_mark = IRDA_NEC_BIT1_MARK,
    .bit1_space = IRDA_NEC_BIT1_SPACE,
    .bit0_mark =IRDA_NEC_BIT0_MARK,
    .bit0_space = IRDA_NEC_BIT0_SPACE,
    .duty_cycle = IRDA_NEC_DUTY_CYCLE,
    .carrier_frequency = IRDA_NEC_CARRIER_FREQUENCY,
};


static void encode_preamble(void) {
    encode_mark(&encoder_timings, IRDA_NEC_PREAMBULE_MARK);
    encode_space(&encoder_timings, IRDA_NEC_PREAMBULE_SPACE);
}

static void encode_repeat(void) {
    encode_space(&encoder_timings, IRDA_NEC_REPEAT_PAUSE);
    encode_mark(&encoder_timings, IRDA_NEC_REPEAT_MARK);
    encode_space(&encoder_timings, IRDA_NEC_REPEAT_SPACE);
    encode_bit(&encoder_timings, 1);
}

void encode_nec(uint32_t addr, uint32_t cmd, bool repeat) {
    uint8_t address = addr & 0xFF;
    uint8_t command = cmd & 0xFF;
    uint8_t address_inverse = (uint8_t) ~address;
    uint8_t command_inverse = (uint8_t) ~command;

    if (!repeat) {
        encode_preamble();
        encode_byte(&encoder_timings, address);
        encode_byte(&encoder_timings, address_inverse);
        encode_byte(&encoder_timings, command);
        encode_byte(&encoder_timings, command_inverse);
        encode_bit(&encoder_timings, 1);
    } else {
        encode_repeat();
    }
}

