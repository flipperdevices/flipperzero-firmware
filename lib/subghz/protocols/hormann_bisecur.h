#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_HORMANN_BISECUR_NAME "Hormann BiSecur"

typedef struct SubGhzProtocolDecoderHormannBiSecur SubGhzProtocolDecoderHormannBiSecur;
typedef struct SubGhzProtocolEncoderHormannBiSecur SubGhzProtocolEncoderHormannBiSecur;

extern const SubGhzProtocolDecoder subghz_protocol_hormann_bisecur_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_hormann_bisecur_encoder;
extern const SubGhzProtocol subghz_protocol_hormann_bisecur;

/**
 * Allocate SubGhzProtocolEncoderHormannBiSecur.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolEncoderHormannBiSecur* pointer to a SubGhzProtocolEncoderHormannBiSecur instance
 */
void* subghz_protocol_encoder_hormann_bisecur_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolEncoderHormannBiSecur.
 * @param context Pointer to a SubGhzProtocolEncoderHormannBiSecur instance
 */
void subghz_protocol_encoder_hormann_bisecur_free(void* context);

/**
 * Deserialize and generating an upload to send.
 * @param context Pointer to a SubGhzProtocolEncoderHormannBiSecur instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_encoder_hormann_bisecur_deserialize(
    void* context,
    FlipperFormat* flipper_format);

/**
 * Forced transmission stop.
 * @param context Pointer to a SubGhzProtocolEncoderHormannBiSecur instance
 */
void subghz_protocol_encoder_hormann_bisecur_stop(void* context);

/**
 * Getting the level and duration of the upload to be loaded into DMA.
 * @param context Pointer to a SubGhzProtocolEncoderHormannBiSecur instance
 * @return LevelDuration 
 */
LevelDuration subghz_protocol_encoder_hormann_bisecur_yield(void* context);

/**
 * Allocate SubGhzProtocolDecoderHormannBiSecur.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderHormannBiSecur* pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 */
void* subghz_protocol_decoder_hormann_bisecur_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderHormannBiSecur.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 */
void subghz_protocol_decoder_hormann_bisecur_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderHormannBiSecur.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 */
void subghz_protocol_decoder_hormann_bisecur_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_hormann_bisecur_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @return hash Hash sum
 */
uint32_t subghz_protocol_decoder_hormann_bisecur_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderHormannBiSecur.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_hormann_bisecur_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data SubGhzProtocolDecoderHormannBiSecur.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus subghz_protocol_decoder_hormann_bisecur_deserialize(
    void* context,
    FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_hormann_bisecur_get_string(void* context, FuriString* output);

/**
 * Getting a one-line textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderHormannBiSecur instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_hormann_bisecur_get_string_brief(void* context, FuriString* output);
