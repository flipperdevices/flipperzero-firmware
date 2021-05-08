#include <stdint.h>
#include "protocol_defs_i.h"
#include "encoder_common_defs_i.h"
DEF_TIMINGS(IRDA_NEC_BIT1_MARK,
            IRDA_NEC_BIT1_SPACE,
            IRDA_NEC_BIT0_MARK,
            IRDA_NEC_BIT0_SPACE,
            IRDA_NEC_DUTY_CYCLE,
            IRDA_NEC_CARRIER_FREQUENCY)
#include "encoder_common_i.h"


static void encode_preamble(void) {
    encode_mark(IRDA_NEC_PREAMBULE_MARK);
    encode_space(IRDA_NEC_PREAMBULE_SPACE);
}

static void encode_repeat(void) {
    encode_space(IRDA_NEC_REPEAT_PAUSE);
    encode_mark(IRDA_NEC_REPEAT_MARK);
    encode_space(IRDA_NEC_REPEAT_SPACE);
    encode_bit(1);
}

void encode_nec(uint32_t addr, uint32_t cmd, bool repeat) {
    uint8_t address = addr & 0xFF;
    uint8_t command = cmd & 0xFF;
    uint8_t address_inverse = (uint8_t) ~address;
    uint8_t command_inverse = (uint8_t) ~command;

    if (!repeat) {
        encode_preamble();
        encode_byte(address);
        encode_byte(address_inverse);
        encode_byte(command);
        encode_byte(command_inverse);
        encode_bit(1);
    } else {
        encode_repeat();
    }
}
