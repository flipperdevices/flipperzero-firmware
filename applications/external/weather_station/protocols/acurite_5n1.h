#pragma once

#include <lib/subghz/protocols/base.h>

#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include "ws_generic.h"
#include <lib/subghz/blocks/math.h>

#define WS_PROTOCOL_ACURITE_5N1_NAME "Acurite 5n1"

typedef struct WSProtocolDecoderAcurite_5n1 WSProtocolDecoderAcurite_5n1;
typedef struct WSProtocolEncoderAcurite_5n1 WSProtocolEncoderAcurite_5n1;

extern const SubGhzProtocolDecoder ws_protocol_acurite_5n1_decoder;
extern const SubGhzProtocolEncoder ws_protocol_acurite_5n1_encoder;
extern const SubGhzProtocol ws_protocol_acurite_5n1;

/**
 * Allocate WSProtocolDecoderAcurite_5n1.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return WSProtocolDecoderAcurite_5n1* pointer to a WSProtocolDecoderAcurite_5n1 instance
 */
void* ws_protocol_decoder_acurite_5n1_alloc(SubGhzEnvironment* environment);

/**
 * Free WSProtocolDecoderAcurite_5n1.
 * @param context Pointer to a WSProtocolDecoderAcurite_5n1 instance
 */
void ws_protocol_decoder_acurite_5n1_free(void* context);

/**
 * Reset decoder WSProtocolDecoderAcurite_5n1.
 * @param context Pointer to a WSProtocolDecoderAcurite_5n1 instance
 */
void ws_protocol_decoder_acurite_5n1_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a WSProtocolDecoderAcurite_5n1 instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void ws_protocol_decoder_acurite_5n1_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a WSProtocolDecoderAcurite_5n1 instance
 * @return hash Hash sum
 */
uint32_t ws_protocol_decoder_acurite_5n1_get_hash_data(void* context);

/**
 * Serialize data WSProtocolDecoderAcurite_5n1.
 * @param context Pointer to a WSProtocolDecoderAcurite_5n1 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus ws_protocol_decoder_acurite_5n1_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data WSProtocolDecoderAcurite_5n1.
 * @param context Pointer to a WSProtocolDecoderAcurite_5n1 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    ws_protocol_decoder_acurite_5n1_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a WSProtocolDecoderAcurite_5n1 instance
 * @param output Resulting text
 */
void ws_protocol_decoder_acurite_5n1_get_string(void* context, FuriString* output);
