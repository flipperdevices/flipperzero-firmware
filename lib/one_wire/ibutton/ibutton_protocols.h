#pragma once

#include <stdint.h>
#include <stddef.h>

#include "protocols/ibutton_protocol_base.h"

#include "ibutton_key.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t* ptr;
    size_t size;
} iButtonEditableData;

/**
 * Initialise the iButton protocols system and allocate necessary objects
 */
void ibutton_protocols_init();

/**
 * Shutdown the iButton protocols system and free unused resources
 */
void ibutton_protocols_shutdown();

/**
 * Get maximum data size out of all protocols available
 * @return maximum data size in bytes
 */
size_t ibutton_protocols_get_max_data_size();

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

// /**
//  * Get protocol id from iButton family code
//  * @param [in] family_code
//  * @return protocol id or iButtonProtocolGeneric if protocol not found
//  */
// iButtonProtocol ibutton_protocols_get_id_by_family_code(uint8_t family_code);
//
// /**
//  * Get protocol id from iButton protocol name
//  * @param [in] protocol_name
//  * @return protocol id or iButtonProtocolMax if protocol not found
//  */
// iButtonProtocol ibutton_protocols_get_id_by_name(const char* protocol_name);

/**
 * Get protocol features bitmask by protocol id
 * @param [in] protocol_id id of the protocol in question
 */
uint32_t ibutton_protocols_get_features(iButtonProtocol protocol_id);

// /**
//  * Get data size based on the protocol id
//  * @param [in] protocol_id id of the protocol in question
//  * @return data size in bytes
//  */
// size_t ibutton_protocols_get_data_size(iButtonProtocol protocol_id);

/**
 * Read the device using the selected protocol
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [out] protocol_id id of the detected protocol or iButtonProtocolMax on failure
 * @return true on success, false on failure
 */
bool ibutton_protocols_read(iButtonKey* key);

/**
 * Write the key to a blank using the selected protocol
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_write_blank(iButtonKey* key);

/**
 * Write the key to another one of the same type using the selected protocol
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_write_copy(iButtonKey* key);

/**
 * Start emulating the key using the selected protocol
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_emulate_start(iButtonKey* key);

/**
 * Stop emulating the key using the selected protocol
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_emulate_stop(iButtonKey* key);

/**
 * Save the key data to a FFF file.
 * @param [in] file_name full absolute path to the file name
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_save(const iButtonKey* key, const char* file_name);

/**
 * Save the key data to a FFF file.
 * @param [in] file_name full absolute path to the file name
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [out] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_load(iButtonKey* key, const char* file_name);

/**
 * Format a string containing device full data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the detected protocol or iButtonProtocolMax on failure
 */
void ibutton_protocols_render_data(const iButtonKey* key, FuriString* result);

/**
 * Format a string containing device brief data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_render_brief_data(const iButtonKey* key, FuriString* result);

/**
 * Format a string containing error message (for invalid keys)
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_render_error(const iButtonKey* key, FuriString* result);

/**
 * Check whether the data is valid according to the selected protocol
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true if data is valid, false otherwise
 */
bool ibutton_protocols_is_valid(const iButtonKey* key);

/**
 * Get a pointer to the protocol's editable data and its size
 * @param [out] data pointer to a pointer to contain the start of data
 * @param [out] data_size pointer to a variable to contain the data size
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_get_editable_data(const iButtonKey* key, iButtonEditableData* data);

/**
 * Make all necessary internal adjustments after editing the data
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_apply_edits(const iButtonKey* key);

#ifdef __cplusplus
}
#endif
