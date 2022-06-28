/**
 * @file lfrfid_key.h
 * 
 * LF RFID key data holder
 */

#pragma once
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LFRIFID_KEY_DATA_SIZE 8
#define LFRIFID_KEY_NAME_SIZE 22

typedef enum {
    LFRFIDKeyEM4100,
    LFRFIDKeyH10301,
    LFRFIDKeyI40134,
    LFRFIDKeyKeyIOProxXSF,
} LFRFIDKeyType;

typedef struct LFRFIDKey LFRFIDKey;

/**
 * Allocate key
 * @return LFRFIDKey* 
 */
LFRFIDKey* lfrfid_key_alloc();

/**
 * Free key
 * @param key 
 */
void lfrfid_key_free(LFRFIDKey* key);

/**
 * Copy key
 * @param to 
 * @param from 
 */
void lfrfid_key_set(LFRFIDKey* to, const LFRFIDKey* from);

/**
 * Set key data
 * @param key 
 * @param data 
 * @param data_count 
 */
void lfrfid_key_set_data(LFRFIDKey* key, uint8_t* data, uint8_t data_count);

/**
 * Clear key data
 * @param key 
 */
void lfrfid_key_clear_data(LFRFIDKey* key);

/**
 * Get pointer to key data
 * @param key 
 * @return const uint8_t* 
 */
const uint8_t* lfrfid_key_get_data_p(LFRFIDKey* key);

/**
 * Get key data size
 * @param key 
 * @return uint8_t 
 */
uint8_t lfrfid_key_get_data_size(LFRFIDKey* key);

/**
 * Set key name
 * @param key 
 * @param name 
 */
void lfrfid_key_set_name(LFRFIDKey* key, const char* name);

/**
 * Get pointer to key name
 * @param key 
 * @return const char* 
 */
const char* lfrfid_key_get_name_p(LFRFIDKey* key);

/**
 * Set key type
 * @param key 
 * @param key_type 
 */
void lfrfid_key_set_type(LFRFIDKey* key, LFRFIDKeyType key_type);

/**
 * Get key type
 * @param key 
 * @return LFRFIDKeyType 
 */
LFRFIDKeyType lfrfid_key_get_type(LFRFIDKey* key);

/**
 * Get type string from key type
 * @param key_type 
 * @return const char* 
 */
const char* lfrfid_key_get_string_by_type(LFRFIDKeyType key_type);

/**
 * Get manufacturer string from key type
 * @param key_type 
 * @return const char* 
 */
const char* lfrfid_key_get_manufacturer_string(LFRFIDKeyType key_type);

/**
 * Get key type from string
 * @param type_string 
 * @param key_type 
 * @return bool 
 */
bool lfrfid_key_get_type_by_string(const char* type_string, LFRFIDKeyType* key_type);

/**
 * Get key data size from type
 * @param key_type 
 * @return uint8_t 
 */
uint8_t lfrfid_key_get_size_by_type(LFRFIDKeyType key_type);

/**
 * Get max key size
 * @return uint8_t 
 */
uint8_t lfrfid_key_get_max_size();

#ifdef __cplusplus
}
#endif
