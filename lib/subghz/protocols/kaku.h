#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_KAKU_NAME "KaKu"

typedef struct SubGhzProtocolDecoderKaku SubGhzProtocolDecoderKaku;
typedef struct SubGhzProtocolEncoderKaku SubGhzProtocolEncoderKaku;

extern const SubGhzProtocolDecoder subghz_protocol_kaku_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_kaku_encoder;
extern const SubGhzProtocol subghz_protocol_kaku;

/**
 * Allocate SubGhzProtocolDecoderKaku.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderKaku* pointer to a SubGhzProtocolDecoderKaku instance
 */
void* subghz_protocol_decoder_kaku_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderKaku.
 * @param context Pointer to a SubGhzProtocolDecoderKaku instance
 */
void subghz_protocol_decoder_kaku_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderKaku.
 * @param context Pointer to a SubGhzProtocolDecoderKaku instance
 */
void subghz_protocol_decoder_kaku_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderKaku instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_kaku_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderKaku instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_kaku_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderKaku.
 * @param context Pointer to a SubGhzProtocolDecoderKaku instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzPresetDefinition
 * @return true On success
 */
bool subghz_protocol_decoder_kaku_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzPresetDefinition* preset);

/**
 * Deserialize data SubGhzProtocolDecoderKaku.
 * @param context Pointer to a SubGhzProtocolDecoderKaku instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_decoder_kaku_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderKaku instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_kaku_get_string(void* context, string_t output);

void* subghz_protocol_encoder_kaku_alloc(SubGhzEnvironment* environment);

void subghz_protocol_encoder_kaku_free(void* context);

bool subghz_protocol_encoder_kaku_deserialize(void* context, FlipperFormat* flipper_format);

void subghz_protocol_encoder_kaku_stop(void* context);

LevelDuration subghz_protocol_encoder_kaku_yield(void* context);

void subghz_protocol_endcoder_kaku_upload_pulse(
    LevelDuration* data,
    size_t* index,
    size_t on_time,
    size_t off_time);
