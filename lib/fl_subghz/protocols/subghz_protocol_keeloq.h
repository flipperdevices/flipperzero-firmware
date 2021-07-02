#pragma once

#include "subghz_protocol_common.h"

typedef struct SubGhzProtocolKeeloq SubGhzProtocolKeeloq;

/**
 * @brief Allocate SubGhzProtocolKeeloq
 * 
 * @return SubGhzProtocolKeeloq* 
 */
SubGhzProtocolKeeloq* subghz_protocol_keeloq_alloc();

/**
 * @brief Free SubGhzProtocolKeeloq
 * 
 * @param instance 
 */
void subghz_protocol_keeloq_free(SubGhzProtocolKeeloq* instance);

/**
 * @brief Loading of manufacture keys
 * 
 * @param instance - SubGhzProtocolKeeloq instance
 * @param name - key name
 * @param key - manufacture (64bit)
 * @param type - type manufacture key 
 */
void subghz_protocol_keeloq_add_manafacture_key(SubGhzProtocolKeeloq* instance, const char* name, uint64_t key, uint16_t type);

/**
 * @brief Sends the key on the air
 * 
 * @param instance - SubGhzProtocolKeeloq instance
 * @param key - key send
 * @param bit - count bit key
 * @param repeat - repeat send key
 */
void subghz_protocol_keeloq_send_key(SubGhzProtocolKeeloq* instance, uint64_t key, uint8_t bit, uint8_t repeat);

/**
 * @brief Parse accepted duration
 * 
 * @param instance - SubGhzProtocolKeeloq instance
 * @param data - LevelPair data
 */
void subghz_protocol_keeloq_parse(SubGhzProtocolKeeloq* instance, LevelPair data);

void subghz_protocol_keeloq_to_str(SubGhzProtocolKeeloq* instance, string_t output);
