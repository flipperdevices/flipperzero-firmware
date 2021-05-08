#include <stdint.h>
#include <irda_i.h>
#include <api-hal-irda.h>
#include <api-hal-delay.h>
#include "protocol_defs_i.h"
#include "encoder_common_defs_i.h"
DEF_TIMINGS(IRDA_SAMSUNG_BIT1_MARK,
            IRDA_SAMSUNG_BIT1_SPACE,
            IRDA_SAMSUNG_BIT0_MARK,
            IRDA_SAMSUNG_BIT0_SPACE,
            IRDA_SAMSUNG_DUTY_CYCLE,
            IRDA_SAMSUNG_CARRIER_FREQUENCY)
#include "encoder_common_i.h"


static void encode_preamble(void) {
    encode_mark(IRDA_SAMSUNG_PREAMBULE_MARK);
    encode_space(IRDA_SAMSUNG_PREAMBULE_SPACE);
}

static void encode_repeat(void) {
    encode_space(IRDA_SAMSUNG_REPEAT_PAUSE);
    encode_mark(IRDA_SAMSUNG_REPEAT_MARK);
    encode_space(IRDA_SAMSUNG_REPEAT_SPACE);
    encode_bit(1);
    encode_bit(1);
}

void encode_samsung32(uint32_t addr, uint32_t cmd, bool repeat) {
    uint8_t address = addr & 0xFF;
    uint8_t command = cmd & 0xFF;
    uint8_t command_inverse = (uint8_t) ~command;

    if (!repeat) {
        encode_preamble();
        encode_byte(address);
        encode_byte(address);
        encode_byte(command);
        encode_byte(command_inverse);
        encode_bit(1);
    } else {
        encode_repeat();
    }
}

