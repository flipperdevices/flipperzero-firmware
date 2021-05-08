#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <api-hal-irda.h>
#include <api-hal-delay.h>
#include "protocol_defs_i.h"


static inline void encode_mark(uint32_t duration) {
    api_hal_irda_pwm_set(i_encoder_timings.duty_cycle, i_encoder_timings.carrier_frequency);
    delay_us(duration);
}

static inline void encode_space(uint32_t duration) {
    api_hal_irda_pwm_stop();
    delay_us(duration);
}

static inline void encode_bit(bool bit) {
    if (bit) {
        encode_mark(i_encoder_timings.bit1_mark);
        encode_space(i_encoder_timings.bit1_space);
    } else {
        encode_mark(i_encoder_timings.bit0_mark);
        encode_space(i_encoder_timings.bit0_space);
    }
}

static inline void encode_byte(uint8_t data) {
    for(uint8_t i = 0; i < 8; i++) {
        encode_bit(!!(data & (1 << i)));
    }
}

