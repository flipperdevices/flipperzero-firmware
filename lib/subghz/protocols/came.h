#pragma once
#include "base.h"

#define SUBGHZ_PROTOCOL_CAME_NAME "CAME"

typedef struct SubGhzProtocolDecoderCame SubGhzProtocolDecoderCame;
typedef struct SubGhzProtocolEncoderCame SubGhzProtocolEncoderCame;

extern const SubGhzProtocolDecoder subghz_protocol_came_decoder;
extern const SubGhzProtocolEncoder subghz_protocol_came_encoder;
extern const SubGhzProtocol subghz_protocol_came;

void* subghz_protocol_encoder_came_alloc(SubGhzEnvironment* environment);

void subghz_protocol_encoder_came_free(void* context);

bool subghz_protocol_encoder_came_load(
    void* context,
    uint64_t key,
    uint8_t count_bit,
    size_t repeat);

void subghz_protocol_encoder_came_stop(void* context);

LevelDuration subghz_protocol_encoder_came_yield(void* context);

/** Allocate SubGhzProtocolCame
 * 
 * @return SubGhzProtocolCame* 
 */
void* subghz_protocol_decoder_came_alloc(SubGhzEnvironment* environment);

/** Free SubGhzProtocolCame
 * 
 * @param instance 
 */
void subghz_protocol_decoder_came_free(void* context);

/** Reset internal state
 * @param instance - SubGhzProtocolCame instance
 */
void subghz_protocol_decoder_came_reset(void* context);

/** Parse accepted duration
 * 
 * @param instance - SubGhzProtocolCame instance
 * @param data - LevelDuration level_duration
 */
void subghz_protocol_decoder_came_feed(void* context, bool level, uint32_t duration);

/** Outputting information from the parser
 * 
 * @param instance - SubGhzProtocolCame* instance
 * @param output   - output string
 */
void subghz_protocol_decoder_came_serialization(void* context, string_t output);

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
    const char* file_path);

// /** Loading protocol from bin data
//  *
//  * @param instance - SubGhzProtocolCame instance
//  * @param context - SubGhzProtocolCommonLoad context
//  */
// void subghz_decoder_came_to_load_protocol(SubGhzProtocolCame* instance, void* context);
