#pragma once

//#include "subghz_protocol_common.h"
#include "../subghz_protocol_blocks.h"

#define SUBGHZ_PROTOCOL_CAME_NAME "CAME"

//typedef struct SubGhzProtocolCame SubGhzProtocolCame;
typedef struct SubGhzProtocolCameDecoder SubGhzProtocolCameDecoder;
typedef struct SubGhzProtocolCameEncoder SubGhzProtocolCameEncoder;

void* subghz_protocol_came_encoder_alloc();
void subghz_protocol_came_encoder_free(void* context);
bool subghz_protocol_came_encoder_load(
    void* context,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat);
void subghz_protocol_came_encoder_stop(void* context);
LevelDuration subghz_protocol_came_encoder_yield(void* context);


/** Allocate SubGhzProtocolCame
 * 
 * @return SubGhzProtocolCame* 
 */
void* subghz_protocol_came_decoder_alloc();

/** Free SubGhzProtocolCame
 * 
 * @param instance 
 */
void subghz_protocol_came_decoder_free(void* context);

// /** Get upload protocol
//  *
//  * @param instance - SubGhzProtocolCame instance
//  * @param encoder - SubGhzProtocolCommonEncoder encoder
//  * @return bool
//  */
// bool subghz_protocol_came_send_key(
//     SubGhzProtocolCame* instance,
//     SubGhzProtocolCommonEncoder* encoder);

/** Reset internal state
 * @param instance - SubGhzProtocolCame instance
 */
void subghz_protocol_came_decoder_reset(void* context);

/** Parse accepted duration
 * 
 * @param instance - SubGhzProtocolCame instance
 * @param data - LevelDuration level_duration
 */
void subghz_protocol_came_decoder_feed(void* context, bool level, uint32_t duration);

/** Outputting information from the parser
 * 
 * @param instance - SubGhzProtocolCame* instance
 * @param output   - output string
 */
void subghz_protocol_came_decoder_serialization(void* context, string_t output);

/** Adding data to a file
 * 
 * @param instance  - SubGhzProtocolCame instance
 * @param flipper_file - FlipperFile 
 * @return bool
 */
bool subghz_protocol_came_save_file(void* context, FlipperFile* flipper_file);

/** Loading protocol from file
 * 
 * @param flipper_file - FlipperFile 
 * @param instance - SubGhzProtocolCame instance
 * @param file_path - file path
 * @return bool
 */
bool subghz_protocol_came_load_file(
    void* context,
    FlipperFile* flipper_file,
    const char* file_path) ;

// /** Loading protocol from bin data
//  * 
//  * @param instance - SubGhzProtocolCame instance
//  * @param context - SubGhzProtocolCommonLoad context
//  */
// void subghz_decoder_came_to_load_protocol(SubGhzProtocolCame* instance, void* context);
