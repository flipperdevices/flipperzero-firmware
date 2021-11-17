#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EncoderCyfral EncoderCyfral;

EncoderCyfral* encoder_cyfral_alloc();
void encoder_cyfral_free(EncoderCyfral* cyfral);
void encoder_cyfral_reset(EncoderCyfral* cyfral);
void encoder_cyfral_set_data(EncoderCyfral* cyfral, const uint8_t* data, size_t data_size);
void encoder_cyfral_get_pulse(EncoderCyfral* cyfral, bool* polarity, uint32_t* length);

#ifdef __cplusplus
}
#endif