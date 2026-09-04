#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_HONEYWELL_5834_NAME "Honeywell5834"

typedef struct SubGhzProtocolDecoderHoneywell5834 SubGhzProtocolDecoderHoneywell5834;
typedef struct SubGhzProtocolEncoderHoneywell5834 SubGhzProtocolEncoderHoneywell5834;

extern const SubGhzProtocolDecoder subghz_protocol_honeywell_5834_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_honeywell_5834_encoder;
extern const SubGhzProtocol subghz_protocol_honeywell_5834;

/**
 * Allocate SubGhzProtocolEncoderHoneywell5834.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolEncoderHoneywell5834* pointer to instance
 */
void* subghz_protocol_encoder_honeywell_5834_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolEncoderHoneywell5834.
 * @param context Pointer to a SubGhzProtocolEncoderHoneywell5834 instance
 */
void subghz_protocol_encoder_honeywell_5834_free(void* context);

/**
 * Deserialize and generate an upload to send.
 * @param context Pointer to a SubGhzProtocolEncoderHoneywell5834 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_encoder_honeywell_5834_deserialize(
    void* context,
    FlipperFormat* flipper_format);

/**
 * Forced transmission stop.
 * @param context Pointer to a SubGhzProtocolEncoderHoneywell5834 instance
 */
void subghz_protocol_encoder_honeywell_5834_stop(void* context);

/**
 * Getting the level and duration of the upload to be loaded into DMA.
 * @param context Pointer to a SubGhzProtocolEncoderHoneywell5834 instance
 * @return LevelDuration
 */
LevelDuration subghz_protocol_encoder_honeywell_5834_yield(void* context);

/**
 * Allocate SubGhzProtocolDecoderHoneywell5834.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderHoneywell5834* pointer to instance
 */
void* subghz_protocol_decoder_honeywell_5834_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderHoneywell5834.
 * @param context Pointer to a SubGhzProtocolDecoderHoneywell5834 instance
 */
void subghz_protocol_decoder_honeywell_5834_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderHoneywell5834.
 * @param context Pointer to a SubGhzProtocolDecoderHoneywell5834 instance
 */
void subghz_protocol_decoder_honeywell_5834_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderHoneywell5834 instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_honeywell_5834_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderHoneywell5834 instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_honeywell_5834_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderHoneywell5834.
 * @param context Pointer to a SubGhzProtocolDecoderHoneywell5834 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_honeywell_5834_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data SubGhzProtocolDecoderHoneywell5834.
 * @param context Pointer to a SubGhzProtocolDecoderHoneywell5834 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_honeywell_5834_deserialize(
    void* context,
    FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderHoneywell5834 instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_honeywell_5834_get_string(void* context, FuriString* output);
