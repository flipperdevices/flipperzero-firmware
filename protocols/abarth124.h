#pragma once

#include <lib/subghz/protocols/base.h>

#include <lib/subghz/blocks/const.h>
#include <lib/subghz/blocks/decoder.h>
#include <lib/subghz/blocks/encoder.h>
#include "tpms_generic.h"
#include <lib/subghz/blocks/math.h>

#define TPMS_PROTOCOL_ABARTH124_NAME "Abarth 124 Spider"

typedef struct TPMSProtocolDecoderAbarth124 TPMSProtocolDecoderAbarth124;
typedef struct TPMSProtocolEncoderAbarth124 TPMSProtocolEncoderAbarth124;

extern const SubGhzProtocolDecoder tpms_protocol_abarth124_decoder;
extern const SubGhzProtocolEncoder tpms_protocol_abarth124_encoder;
extern const SubGhzProtocol tpms_protocol_abarth124;

/**
 * Allocate TPMSProtocolDecoderAbarth124.
 * @param environment Pointer to a SubGhzEnvironment instance
 * @return TPMSProtocolDecoderAbarth124* pointer to a TPMSProtocolDecoderAbarth124 instance
 */
void* tpms_protocol_decoder_abarth124_alloc(SubGhzEnvironment* environment);

/**
 * Free TPMSProtocolDecoderAbarth124.
 * @param context Pointer to a TPMSProtocolDecoderAbarth124 instance
 */
void tpms_protocol_decoder_abarth124_free(void* context);

/**
 * Reset decoder TPMSProtocolDecoderAbarth124.
 * @param context Pointer to a TPMSProtocolDecoderAbarth124 instance
 */
void tpms_protocol_decoder_abarth124_reset(void* context);

/**
 * Parse a raw sequence of levels and durations received from the air.
 * @param context Pointer to a TPMSProtocolDecoderAbarth124 instance
 * @param level Signal level true-high false-low
 * @param duration Duration of this level in, us
 */
void tpms_protocol_decoder_abarth124_feed(void* context, bool level, uint32_t duration);

/**
 * Getting the hash sum of the last randomly received parcel.
 * @param context Pointer to a TPMSProtocolDecoderAbarth124 instance
 * @return hash Hash sum
 */
uint8_t tpms_protocol_decoder_abarth124_get_hash_data(void* context);

/**
 * Serialize data TPMSProtocolDecoderAbarth124.
 * @param context Pointer to a TPMSProtocolDecoderAbarth124 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus tpms_protocol_decoder_abarth124_serialize(
    void* context,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data TPMSProtocolDecoderAbarth124.
 * @param context Pointer to a TPMSProtocolDecoderAbarth124 instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    tpms_protocol_decoder_abarth124_deserialize(void* context, FlipperFormat* flipper_format);

/**
 * Getting a textual representation of the received data.
 * @param context Pointer to a TPMSProtocolDecoderAbarth124 instance
 * @param output Resulting text
 */
void tpms_protocol_decoder_abarth124_get_string(void* context, FuriString* output);
