/**
 * @file ibutton_key.h
 * 
 * iButton key data holder
 */

#pragma once

#include "protocols/ibutton_protocol_defs.h"

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
iButtonProtocol ibutton_key_get_protocol_id(const iButtonKey* key);

/**
 * Set protocol id
 * @param [in] key
 * @param [in] protocol_id
 */
void ibutton_key_set_protocol_id(iButtonKey* key, iButtonProtocol protocol_id);

/**
 * Get protocol data
 * @param [in] key
 * @return protocol data
 */
iButtonProtocolData* ibutton_key_get_protocol_data(const iButtonKey* key);

/**
 * Set protocol data
 * @param [in] key
 * @param [in] protocol_data
 */
void ibutton_key_set_protocol_data(iButtonKey* key, iButtonProtocolData* protocol_data);

#ifdef __cplusplus
}
#endif
