#pragma once

#include <lib/subghz/protocols/base.h>

#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include "ws_generic.h"
#include <lib/subghz/blocks/math.h>

#define WS_PROTOCOL_AURIOL_AHFL_NAME "Auriol AHFL" //Auriol AHFL 433B2 IPX4

typedef struct WSProtocolDecoderAuriol_AHFL WSProtocolDecoderAuriol_AHFL;
typedef struct WSProtocolEncoderAuriol_AHFL WSProtocolEncoderAuriol_AHFL;

extern const SubGhzProtocolDecoder ws_protocol_auriol_ahfl_decoder;
extern const SubGhzProtocolEncoder ws_protocol_auriol_ahfl_encoder;
extern const SubGhzProtocol ws_protocol_auriol_ahfl;

/**
 * Allocate WSProtocolDecoderAuriol_AHFL.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return WSProtocolDecoderAuriol_AHFL* pointer to a WSProtocolDecoderAuriol_AHFL instance
 */
void* ws_protocol_decoder_auriol_ahfl_alloc(SubGhzEnvironment* environment);

/**
 * Free WSProtocolDecoderAuriol_AHFL.
 * @param context Pointer to a WSProtocolDecoderAuriol_AHFL instance
 */
void ws_protocol_decoder_auriol_ahfl_free(void* context);

/**
 * Reset decoder WSProtocolDecoderAuriol_AHFL.
 * @param context Pointer to a WSProtocolDecoderAuriol_AHFL instance
 */
void ws_protocol_decoder_auriol_ahfl_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a WSProtocolDecoderAuriol_AHFL instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void ws_protocol_decoder_auriol_ahfl_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a WSProtocolDecoderAuriol_AHFL instance
 * @return hash Hash sum
 */
uint8_t ws_protocol_decoder_auriol_ahfl_get_hash_data(void* context);

/**
 * Serialize data WSProtocolDecoderAuriol_AHFL.
 * @param context Pointer to a WSProtocolDecoderAuriol_AHFL instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus ws_protocol_decoder_auriol_ahfl_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data WSProtocolDecoderAuriol_AHFL.
 * @param context Pointer to a WSProtocolDecoderAuriol_AHFL instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    ws_protocol_decoder_auriol_ahfl_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a WSProtocolDecoderAuriol_AHFL instance
 * @param output Resulting text
 */
void ws_protocol_decoder_auriol_ahfl_get_string(void* context, FuriString* output);
