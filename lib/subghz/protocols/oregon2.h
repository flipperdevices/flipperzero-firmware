#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_OREGON2_NAME "Oregon2"

typedef struct SubGhzProtocolDecoderOregon2 SubGhzProtocolDecoderOregon2;

extern const SubGhzProtocolDecoder subghz_protocol_oregon2_decoder;
extern const SubGhzProtocol subghz_protocol_oregon2;

/**
 * Allocate SubGhzProtocolDecoderOregon2.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderOregon2* pointer to a SubGhzProtocolDecoderOregon2 instance
 */
void* subghz_protocol_decoder_oregon2_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderOregon2.
 * @param context Pointer to a SubGhzProtocolDecoderOregon2 instance
 */
void subghz_protocol_decoder_oregon2_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderOregon2.
 * @param context Pointer to a SubGhzProtocolDecoderOregon2 instance
 */
void subghz_protocol_decoder_oregon2_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderOregon2 instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_oregon2_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderOregon2 instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_oregon2_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderOregon2.
 * @param context Pointer to a SubGhzProtocolDecoderOregon2 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzPresetDefinition
 * @return true On success
 */
bool subghz_protocol_decoder_oregon2_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzPresetDefinition* preset);

/**
 * Deserialize data SubGhzProtocolDecoderOregon2.
 * @param context Pointer to a SubGhzProtocolDecoderOregon2 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_decoder_oregon2_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderOregon2 instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_oregon2_get_string(void* context, string_t output);
