/**
 * @file ibutton_key.h
 * 
 * iButton key data holder
 */

#pragma once

#include <core/string.h>

#include "protocols/protocol_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iButtonKey iButtonKey;

/**
 * Allocate key
 * @return iButtonKey* 
 */
iButtonKey* ibutton_key_alloc(size_t data_size);

/**
 * Free key
 * @param [in] key
 */
void ibutton_key_free(iButtonKey* key);

/**
 * TODO: description comment
 */
iButtonProtocolId ibutton_key_get_protocol_id(const iButtonKey* key);

/**
 * TODO: description comment
 */
void ibutton_key_set_protocol_id(iButtonKey* key, iButtonProtocolId protocol_id);

/**
 * TODO: description comment
 */
void ibutton_key_reset(iButtonKey* key);

#ifdef __cplusplus
}
#endif
