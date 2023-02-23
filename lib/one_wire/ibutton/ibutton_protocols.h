#pragma once

#include "protocols/ibutton_protocol_base.h"
#include "protocols/ibutton_protocol_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise the iButton protocols system and allocate necessary objects
 */
void ibutton_protocols_init();

/**
 * Shutdown the iButton protocols system and free unused resources
 */
void ibutton_protocols_shutdown();

/**
 * Get the manufacturer name based on the protocol id
 * @param [in] protocol_id id of the protocol in question
 * @return pointer to a statically allocated string with manufacturer name
 */
const char* ibutton_protocols_get_manufacturer(iButtonProtocol protocol_id);

/**
 * Get the protocol name based on the protocol id
 * @param [in] protocol_id id of the protocol in question
 * @return pointer to a statically allocated string with protocol name
 */
const char* ibutton_protocols_get_name(iButtonProtocol protocol_id);

#ifdef __cplusplus
}
#endif
