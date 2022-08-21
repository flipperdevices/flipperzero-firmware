#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_NERO_PARADOX_WIRELESS_NAME "Paradox Wireless"

typedef struct SubGhzProtocolDecoderParadoxWireless SubGhzProtocolDecoderParadoxWireless;
typedef struct SubGhzProtocolEncoderParadoxWireless SubGhzProtocolEncoderParadoxWireless;

extern const SubGhzProtocolDecoder subghz_protocol_paradox_wireless_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_paradox_wireless_encoder;
extern const SubGhzProtocol subghz_protocol_paradox_wireless;

/**
 * Allocate SubGhzProtocolEncoderParadoxWireless.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolEncoderParadoxWireless* pointer to a SubGhzProtocolEncoderParadoxWireless instance
 */
void* subghz_protocol_encoder_paradox_wireless_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolEncoderParadoxWireless.
 * @param context Pointer to a SubGhzProtocolEncoderParadoxWireless instance
 */
void subghz_protocol_encoder_paradox_wireless_free(void* context);

/**
 * Deserialize and generating an upload to send.
 * @param context Pointer to a SubGhzProtocolEncoderParadoxWireless instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_encoder_paradox_wireless_deserialize(
    void* context,
    FlipperFormat* flipper_format);

/**
 * Forced transmission stop.
 * @param context Pointer to a SubGhzProtocolEncoderParadoxWireless instance
 */
void subghz_protocol_encoder_paradox_wireless_stop(void* context);

/**
 * Getting the level and duration of the upload to be loaded into DMA.
 * @param context Pointer to a SubGhzProtocolEncoderParadoxWireless instance
 * @return LevelDuration 
 */
LevelDuration subghz_protocol_encoder_paradox_wireless_yield(void* context);

/**
 * Allocate SubGhzProtocolDecoderParadoxWireless.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderParadoxWireless* pointer to a SubGhzProtocolDecoderParadoxWireless instance
 */
void* subghz_protocol_decoder_paradox_wireless_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderParadoxWireless.
 * @param context Pointer to a SubGhzProtocolDecoderParadoxWireless instance
 */
void subghz_protocol_decoder_paradox_wireless_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderParadoxWireless.
 * @param context Pointer to a SubGhzProtocolDecoderParadoxWireless instance
 */
void subghz_protocol_decoder_paradox_wireless_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderParadoxWireless instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_paradox_wireless_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderParadoxWireless instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_paradox_wireless_get_hash_data(void* context);

/**
 * Serialize data SubGhzProtocolDecoderParadoxWireless.
 * @param context Pointer to a SubGhzProtocolDecoderParadoxWireless instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzPresetDefinition
 * @return true On success
 */
bool subghz_protocol_decoder_paradox_wireless_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzPresetDefinition* preset);

/**
 * Deserialize data SubGhzProtocolDecoderParadoxWireless.
 * @param context Pointer to a SubGhzProtocolDecoderParadoxWireless instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_decoder_paradox_wireless_deserialize(
    void* context,
    FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderParadoxWireless instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_paradox_wireless_get_string(void* context, string_t output);
