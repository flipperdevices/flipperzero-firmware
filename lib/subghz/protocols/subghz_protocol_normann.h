#pragma once

#include "subghz_protocol_common.h"

typedef struct SubGhzProtocolNormann SubGhzProtocolNormann;

/** Allocate SubGhzProtocolNormann
 * 
 * @return SubGhzProtocolNormann* 
 */
SubGhzProtocolNormann* subghz_protocol_normann_alloc();

/** Free SubGhzProtocolNormann
 * 
 * @param instance 
 */
void subghz_protocol_normann_free(SubGhzProtocolNormann* instance);

/** Get upload protocol
 * 
 * @param instance - SubGhzProtocolNormann instance
 * @param encoder - SubGhzProtocolCommonEncoder encoder
 * @return bool
 */
bool subghz_protocol_normann_send_key(
    SubGhzProtocolNormann* instance,
    SubGhzProtocolCommonEncoder* encoder);

/** Reset internal state
 * @param instance - SubGhzProtocolNormann instance
 */
void subghz_protocol_normann_reset(SubGhzProtocolNormann* instance);

/** Parse accepted duration
 * 
 * @param instance - SubGhzProtocolNormann instance
 * @param data - LevelDuration level_duration
 */
void subghz_protocol_normann_parse(SubGhzProtocolNormann* instance, bool level, uint32_t duration);

/** Outputting information from the parser
 * 
 * @param instance - SubGhzProtocolNormann* instance
 * @param output   - output string
 */
void subghz_protocol_normann_to_str(SubGhzProtocolNormann* instance, string_t output);

/** Get a string to save the protocol
 * 
 * @param instance  - SubGhzProtocolNormann instance
 * @param output    - the resulting string
 */
void subghz_protocol_normann_to_save_str(SubGhzProtocolNormann* instance, string_t output);

/** Loading protocol from file
 * 
 * @param file_worker - FileWorker file_worker
 * @param instance - SubGhzProtocolNormann instance
 * @param file_path - file path
 * @return bool
 */
bool subghz_protocol_normann_to_load_protocol_from_file(
    FileWorker* file_worker,
    SubGhzProtocolNormann* instance,
    const char* file_path);

/** Loading protocol from bin data
 * 
 * @param instance - SubGhzProtocolNormann instance
 * @param context - SubGhzProtocolCommonLoad context
 */
void subghz_decoder_normann_to_load_protocol(SubGhzProtocolNormann* instance, void* context);
