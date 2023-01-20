#pragma once

#include "base.h"

#define SUBGHZ_PROTOCOL_DATA_RAW_NAME "DataRAW"

typedef struct SubGhzProtocolDecoderDataRAW SubGhzProtocolDecoderDataRAW;
typedef struct SubGhzProtocolEncoderDataRAW SubGhzProtocolEncoderDataRAW;

extern const SubGhzProtocolDecoder subghz_protocol_data_raw_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_data_raw_encoder;
extern const SubGhzProtocol subghz_protocol_data_raw;

/**
 * Allocate SubGhzProtocolEncoderDataRAW.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolEncoderDataRAW* pointer to a SubGhzProtocolEncoderDataRAW instance
 */
void* subghz_protocol_encoder_data_raw_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolEncoderDataRAW.
 * @param context Pointer to a SubGhzProtocolEncoderDataRAW instance
 */
void subghz_protocol_encoder_data_raw_free(void* context);

/**
 * Deserialize and generating an upload to send.
 * @param context Pointer to a SubGhzProtocolEncoderDataRAW instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_encoder_data_raw_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Forced transmission stop.
 * @param context Pointer to a SubGhzProtocolEncoderDataRAW instance
 */
void subghz_protocol_encoder_data_raw_stop(void* context);

/**
 * Getting the level and duration of the upload to be loaded into DMA.
 * @param context Pointer to a SubGhzProtocolEncoderDataRAW instance
 * @return LevelDuration 
 */
LevelDuration subghz_protocol_encoder_data_raw_yield(void* context);

/**
 * Allocate SubGhzProtocolDecoderDataRAW.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return SubGhzProtocolDecoderDataRAW* pointer to a SubGhzProtocolDecoderDataRAW instance
 */
void* subghz_protocol_decoder_data_raw_alloc(SubGhzEnvironment* environment);

/**
 * Free SubGhzProtocolDecoderDataRAW.
 * @param context Pointer to a SubGhzProtocolDecoderDataRAW instance
 */
void subghz_protocol_decoder_data_raw_free(void* context);

/**
 * Reset decoder SubGhzProtocolDecoderDataRAW.
 * @param context Pointer to a SubGhzProtocolDecoderDataRAW instance
 */
void subghz_protocol_decoder_data_raw_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a SubGhzProtocolDecoderDataRAW instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void subghz_protocol_decoder_data_raw_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a SubGhzProtocolDecoderDataRAW instance
 * @return hash Hash sum
 */
uint8_t subghz_protocol_decoder_data_raw_get_hash_data(void* context);


void subghz_protocol_decoder_data_input_rssi(SubGhzProtocolDecoderDataRAW* instance, float rssi);

/**
 * Serialize data SubGhzProtocolDecoderDataRAW.
 * @param context Pointer to a SubGhzProtocolDecoderDataRAW instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return true On success
 */
bool subghz_protocol_decoder_data_raw_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data SubGhzProtocolDecoderDataRAW.
 * @param context Pointer to a SubGhzProtocolDecoderDataRAW instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return true On success
 */
bool subghz_protocol_decoder_data_raw_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a SubGhzProtocolDecoderDataRAW instance
 * @param output Resulting text
 */
void subghz_protocol_decoder_data_raw_get_string(void* context, FuriString* output);
