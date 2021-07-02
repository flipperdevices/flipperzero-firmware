#pragma once

#include "subghz_protocol_common.h"

typedef void (*SubGhzProtocolTextCallback)(string_t text, void* context);

typedef struct SubGhzProtocol SubGhzProtocol;

/**
 * @brief Allocate SubGhzProtocol
 * 
 * @return SubGhzProtocol* 
 */
SubGhzProtocol* subghz_protocol_alloc();

/**
 * @brief Free SubGhzProtocol
 * 
 * @param instance 
 */
void subghz_protocol_free(SubGhzProtocol* instance);

/**
 * @brief Outputting data from all parsers
 * 
 * @param instance - SubGhzProtocol instance
 * @param callback - SubGhzProtocolTextCallback callback
 * @param context
 */
void subghz_protocol_enable_dump(
    SubGhzProtocol* instance,
    SubGhzProtocolTextCallback callback,
    void* context);

/**
 * @brief File name rainbow table Nice Flor-S
 * 
 * @param instance - SubGhzProtocol instance
 * @param file_name - "path/file_name"
 */
void subghz_protocol_load_nice_flor_s_file(SubGhzProtocol* instance, const char* file_name);

/**
 * @brief File upload manufacture keys
 * 
 * @param instance - SubGhzProtocol instance
 * @param file_name - "path/file_name"
 */
void subghz_protocol_load_keeloq_file(SubGhzProtocol* instance, const char* file_name);

/**
 * @brief Restarting all parsers
 * 
 * @param instance - SubGhzProtocol instance
 */
void subghz_protocol_reset(SubGhzProtocol* instance);

/**
 * @brief Loading data into all parsers
 * 
 * @param instance - SubGhzProtocol instance
 * @param data - LevelPair data
 */
void subghz_protocol_parse(SubGhzProtocol* instance, LevelPair data);
