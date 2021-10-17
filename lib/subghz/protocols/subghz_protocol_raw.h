#pragma once

#include "subghz_protocol_common.h"

typedef struct SubGhzProtocolRAW SubGhzProtocolRAW;

/** Allocate SubGhzProtocolRAW
 * 
 * @return SubGhzProtocolRAW* 
 */
SubGhzProtocolRAW* subghz_protocol_raw_alloc();

/** Free SubGhzProtocolRAW
 * 
 * @param instance 
 */
void subghz_protocol_raw_free(SubGhzProtocolRAW* instance);

/** Reset internal state
 * @param instance - SubGhzProtocolRAW instance
 */
void subghz_protocol_raw_reset(SubGhzProtocolRAW* instance);

// /** Analysis of received data
//  * 
//  * @param instance SubGhzProtocolRAW instance
//  */
// void subghz_protocol_raw_check_remote_controller(SubGhzProtocolRAW* instance);

/** Parse accepted duration
 * 
 * @param instance - SubGhzProtocolRAW instance
 * @param data - LevelDuration level_duration
 */
void subghz_protocol_raw_parse(SubGhzProtocolRAW* instance, bool level, uint32_t duration);

// /** Outputting information from the parser
//  * 
//  * @param instance - SubGhzProtocolRAW* instance
//  * @param output   - output string
//  */
// void subghz_protocol_raw_to_str(SubGhzProtocolRAW* instance, string_t output);

// /** Get a string to save the protocol
//  * 
//  * @param instance  - SubGhzProtocolRAW instance
//  * @param output    - the resulting string
//  */
// void subghz_protocol_raw_to_save_str(SubGhzProtocolRAW* instance, string_t output);

// /** Loading protocol from bin data
//  * 
//  * @param instance - SubGhzProtocolRAW instance
//  * @param context - SubGhzProtocolCommonLoad context
//  */
// void subghz_decoder_raw_to_load_protocol(SubGhzProtocolRAW* instance, void* context);

void subghz_protocol_raw_print(SubGhzProtocolRAW* instance);

bool subghz_protocol_save_raw_to_file_init(
    SubGhzProtocolRAW* instance,
    const char* dev_name,
    uint32_t frequency,
    FuriHalSubGhzPreset preset);
void subghz_protocol_save_raw_to_file_stop(SubGhzProtocolRAW* instance);
bool subghz_protocol_save_raw_to_file_write(SubGhzProtocolRAW* instance);
size_t subghz_save_protocol_raw_get_sample_write(SubGhzProtocolRAW* instance);

bool subghz_save_protocol_raw_to_file(SubGhzProtocolRAW* instance, const char* dev_name);