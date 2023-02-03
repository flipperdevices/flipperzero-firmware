/**
 * @file ibutton_key.h
 * 
 * iButton key data holder
 */

#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iButtonKey iButtonKey;

/**
 * Allocate key
 * @return iButtonKey* 
 */
iButtonKey* ibutton_key_alloc();

/**
 * Free key
 * @param [in] key
 */
void ibutton_key_free(iButtonKey* key);

/**
 * Get protocol id
 * @param [in] key
 * @return protocol id
 */
size_t ibutton_key_get_protocol_id(const iButtonKey* key);

/**
 * Set protocol id
 * @param [in] key
 * @param [in] protocol_id
 */
void ibutton_key_set_protocol_id(iButtonKey* key, size_t protocol_id);

/**
 * Set ROM data. The data size is defined by the current protocol.
 * @param [in] key
 * @param [in] data
 */
void ibutton_key_set_rom_data(iButtonKey* key, const uint8_t* rom_data, size_t data_size);

/**
 * Set user data. The data size is defined by the current protocol.
 * @param [in] key
 * @param [in] data
 */
void ibutton_key_set_user_data(iButtonKey* key, const void* user_data, size_t data_size);

#ifdef __cplusplus
}
#endif
