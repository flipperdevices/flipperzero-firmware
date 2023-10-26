#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_GENIE_NAME "Genie"

typedef struct SubGhzProtocolDecoderGenie SubGhzProtocolDecoderGenie;
typedef struct SubGhzProtocolEncoderGenie SubGhzProtocolEncoderGenie;

extern const SubGhzProtocolDecoder subghz_protocol_genie_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_genie_encoder;
extern const SubGhzProtocol subghz_protocol_genie;

/**
 * Allocate SubGhzProtocolEncoderGenie.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolEncoderGenie* pointer to a SubGhzProtocolEncoderGenie instance
 */
void* subghz_protocol_encoder_genie_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolEncoderGenie.
 * @param context Pointer to a SubGhzProtocolEncoderGenie instance
 */
void subghz_protocol_encoder_genie_free(void* context);

/**
 * Deserialize and generating an upload to send.
 * @param context Pointer to a SubGhzProtocolEncoderGenie instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    subghz_protocol_encoder_genie_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Forced transmission stop.
 * @param context Pointer to a SubGhzProtocolEncoderGenie instance
 */
void subghz_protocol_encoder_genie_stop(void* context);

/**
 * Getting the level and duration of the upload to be loaded into DMA.
 * @param context Pointer to a SubGhzProtocolEncoderGenie instance
 * @return LevelDuration 
 */
LevelDuration subghz_protocol_encoder_genie_yield(void* context);

/**
 * Allocate SubGhzProtocolDecoderGenie.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderGenie* pointer to a SubGhzProtocolDecoderGenie instance
 */
void* subghz_protocol_decoder_genie_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderGenie.
 * @param context Pointer to a SubGhzProtocolDecoderGenie instance
 */
void subghz_protocol_decoder_genie_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderGenie.
 * @param context Pointer to a SubGhzProtocolDecoderGenie instance
 */
void subghz_protocol_decoder_genie_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderGenie instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_genie_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderGenie instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_genie_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderGenie.
 * @param context Pointer to a SubGhzProtocolDecoderGenie instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return SubGhzProtocolStatus
 */
SubGhzProtocolStatus subghz_protocol_decoder_genie_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data SubGhzProtocolDecoderGenie.
 * @param context Pointer to a SubGhzProtocolDecoderGenie instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return SubGhzProtocolStatus
 */
SubGhzProtocolStatus
    subghz_protocol_decoder_genie_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderGenie instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_genie_get_string(void* context, FuriString* output);
