#pragma once

#include <lib/subghz/protocols/base.h>

#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include "ws_generic.h"
#include <lib/subghz/blocks/math.h>

#define WS_PROTOCOL_KEDSUM_TH_NAME "Kedsum-TH"

typedef struct WSProtocolDecoderKedsumTH WSProtocolDecoderKedsumTH;
typedef struct WSProtocolEncoderKedsumTH WSProtocolEncoderKedsumTH;

extern const SubGhzProtocolDecoder ws_protocol_kedsum_th_decoder;
extern const SubGhzProtocolEncoder ws_protocol_kedsum_th_encoder;
extern const SubGhzProtocol ws_protocol_kedsum_th;

/**
 * Allocate WSProtocolDecoderKedsumTH.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return WSProtocolDecoderKedsumTH* pointer to a WSProtocolDecoderKedsumTH instance
 */
void* ws_protocol_decoder_kedsum_th_alloc(SubGhzEnvironment* environment);

/**
 * Free WSProtocolDecoderKedsumTH.
 * @param context Pointer to a WSProtocolDecoderKedsumTH instance
 */
void ws_protocol_decoder_kedsum_th_free(void* context);

/**
 * Reset decoder WSProtocolDecoderKedsumTH.
 * @param context Pointer to a WSProtocolDecoderKedsumTH instance
 */
void ws_protocol_decoder_kedsum_th_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a WSProtocolDecoderKedsumTH instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void ws_protocol_decoder_kedsum_th_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a WSProtocolDecoderKedsumTH instance
 * @return hash Hash sum
 */
uint32_t ws_protocol_decoder_kedsum_th_get_hash_data(void* context);

/**
 * Serialize data WSProtocolDecoderKedsumTH.
 * @param context Pointer to a WSProtocolDecoderKedsumTH instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus ws_protocol_decoder_kedsum_th_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data WSProtocolDecoderKedsumTH.
 * @param context Pointer to a WSProtocolDecoderKedsumTH instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    ws_protocol_decoder_kedsum_th_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a WSProtocolDecoderKedsumTH instance
 * @param output Resulting text
 */
void ws_protocol_decoder_kedsum_th_get_string(void* context, FuriString* output);
