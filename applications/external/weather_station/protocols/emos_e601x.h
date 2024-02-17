#pragma once

#include <lib/subghz/protocols/base.h>

#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include "ws_generic.h"
#include <lib/subghz/blocks/math.h>

#define WS_PROTOCOL_EMOSE601X_NAME "EMOS E601x"

typedef struct WSProtocolDecoderEmosE601x WSProtocolDecoderEmosE601x;
typedef struct WSProtocolEncoderEmosE601x WSProtocolEncoderEmosE601x;

extern const SubGhzProtocolDecoder ws_protocol_emose601x_decoder;
extern const SubGhzProtocolEncoder ws_protocol_emose601x_encoder;
extern const SubGhzProtocol ws_protocol_emose601x;

/**
 * Allocate WSProtocolDecoderEmosE601x.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return WSProtocolDecoderEmosE601x* pointer to a WSProtocolDecoderEmosE601x instance
 */
void* ws_protocol_decoder_emose601x_alloc(SubGhzEnvironment* environment);

/**
 * Free WSProtocolDecoderEmosE601x.
 * @param context Pointer to a WSProtocolDecoderEmosE601x instance
 */
void ws_protocol_decoder_emose601x_free(void* context);

/**
 * Reset decoder WSProtocolDecoderEmosE601x.
 * @param context Pointer to a WSProtocolDecoderEmosE601x instance
 */
void ws_protocol_decoder_emose601x_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a WSProtocolDecoderEmosE601x instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void ws_protocol_decoder_emose601x_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a WSProtocolDecoderEmosE601x instance
 * @return hash Hash sum
 */
uint32_t ws_protocol_decoder_emose601x_get_hash_data(void* context);

/**
 * Serialize data WSProtocolDecoderEmosE601x.
 * @param context Pointer to a WSProtocolDecoderEmosE601x instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus ws_protocol_decoder_emose601x_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data WSProtocolDecoderEmosE601x.
 * @param context Pointer to a WSProtocolDecoderEmosE601x instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    ws_protocol_decoder_emose601x_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a WSProtocolDecoderEmosE601x instance
 * @param output Resulting text
 */
void ws_protocol_decoder_emose601x_get_string(void* context, FuriString* output);
