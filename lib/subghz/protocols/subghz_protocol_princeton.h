#pragma once

#include "subghz_protocol_common.h"


/** SubGhzEncoderPrinceton anonymous type */
typedef struct SubGhzEncoderPrinceton SubGhzEncoderPrinceton;

/** Allocate SubGhzEncoderPrinceton
 * @return pointer to SubGhzEncoderPrinceton instance
 */
SubGhzEncoderPrinceton* subghz_encoder_princeton_alloc();

/** Free SubGhzEncoderPrinceton instance
 * @param instance - SubGhzEncoderPrinceton instance
 */
void subghz_encoder_princeton_free(SubGhzEncoderPrinceton* instance);

/** Reset encoder with new params
 * @param instance - SubGhzEncoderPrinceton instance
 * @param key - 24bit key
 * @param repeat - how many times to repeat 
 */
void subghz_encoder_princeton_reset(SubGhzEncoderPrinceton* instance, uint32_t key, size_t repeat);

/** Get repeat count left
 * @param instance - SubGhzEncoderPrinceton instance
 * @return repeat count left
 */
size_t subghz_encoder_princeton_get_repeat_left(SubGhzEncoderPrinceton* instance);

/** Get level duration
 * @param instance - SubGhzEncoderPrinceton instance
 * @return level duration
 */
LevelDuration subghz_encoder_princeton_yield(void* context);


/** SubGhzDecoderPrinceton anonymous type */
typedef struct SubGhzDecoderPrinceton SubGhzDecoderPrinceton;

/** Allocate SubGhzDecoderPrinceton
 * 
 * @return SubGhzDecoderPrinceton* 
 */
SubGhzDecoderPrinceton* subghz_decoder_princeton_alloc();

/** Free SubGhzDecoderPrinceton
 * 
 * @param instance 
 */
void subghz_decoder_princeton_free(SubGhzDecoderPrinceton* instance);

/** Sends the key on the air
 * 
 * @param instance - SubGhzDecoderPrinceton instance
 * @param key - key send
 * @param bit - count bit key
 * @param repeat - repeat send key
 */
void subghz_decoder_princeton_send_key(SubGhzDecoderPrinceton* instance, uint64_t key, uint8_t bit, uint8_t repeat);

/** Reset internal state
 * @param instance - SubGhzDecoderPrinceton instance
 */
void subghz_decoder_princeton_reset(SubGhzDecoderPrinceton* instance);

/** Parse accepted duration
 * 
 * @param instance - SubGhzDecoderPrinceton instance
 * @param data - LevelDuration level_duration
 */
void subghz_decoder_princeton_parse(SubGhzDecoderPrinceton* instance, bool level, uint32_t duration);

