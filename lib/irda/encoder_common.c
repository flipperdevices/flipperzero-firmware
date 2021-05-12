#include <stdint.h>
#include <stdbool.h>
#include <api-hal-irda.h>
#include <api-hal-delay.h>
#include "encoder_common_i.h"


void encode_mark(const EncoderTimings *timings, uint32_t duration) {
    api_hal_irda_pwm_set(timings->duty_cycle, timings->carrier_frequency);
    delay_us(duration);
}

void encode_space(const EncoderTimings *timings, uint32_t duration) {
    (void) timings;
    api_hal_irda_pwm_stop();
    delay_us(duration);
}

void encode_bit(const EncoderTimings *timings, bool bit) {
    if (bit) {
        encode_mark(timings, timings->bit1_mark);
        encode_space(timings, timings->bit1_space);
    } else {
        encode_mark(timings, timings->bit0_mark);
        encode_space(timings, timings->bit0_space);
    }
}

void encode_byte(const EncoderTimings *timings, uint8_t data) {
    for(uint8_t i = 0; i < 8; i++) {
        encode_bit(timings, !!(data & (1 << i)));
    }
}

