#pragma once
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct EncoderMetakom EncoderMetakom;

EncoderMetakom* encoder_metakom_alloc();
void encoder_metakom_free(EncoderMetakom* metakom);
void encoder_metakom_reset(EncoderMetakom* metakom);
void encoder_metakom_set_data(EncoderMetakom* metakom, const uint8_t* data, size_t data_size);
void encoder_metakom_get_pulse(EncoderMetakom* metakom, bool* polarity, uint32_t* length);

#ifdef __cplusplus
}
#endif