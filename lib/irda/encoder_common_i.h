#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint32_t bit1_mark;
    uint32_t bit1_space;
    uint32_t bit0_mark;
    uint32_t bit0_space;
    float duty_cycle;
    uint32_t carrier_frequency;
} EncoderTimings;


void encode_byte(const EncoderTimings *timings, uint8_t data);
void encode_bit(const EncoderTimings *timings, bool bit);
void encode_space(const EncoderTimings *timings, uint32_t duration);
void encode_mark(const EncoderTimings *timings, uint32_t duration);

