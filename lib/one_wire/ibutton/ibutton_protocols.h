#pragma once

#include <stdint.h>
#include <stddef.h>

#include "protocols/ibutton_protocol_common.h"

#include "ibutton_key.h"

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
 *
 */
uint32_t ibutton_protocols_get_protocol_count();

/**
 * Get maximum data size out of all protocols available
 * @return maximum data size in bytes
 */
size_t ibutton_protocols_get_max_data_size();

/**
 * Get the manufacturer name based on the protocol id
 * @param [in] id id of the protocol in question
 * @return pointer to a statically allocated string with manufacturer name
 */
const char* ibutton_protocols_get_manufacturer(iButtonProtocolId id);

/**
 * Get the protocol name based on the protocol id
 * @param [in] id id of the protocol in question
 * @return pointer to a statically allocated string with protocol name
 */
const char* ibutton_protocols_get_name(iButtonProtocolId id);

/**
 * Get protocol features bitmask by protocol id
 * @param [in] id id of the protocol in question
 */
uint32_t ibutton_protocols_get_features(iButtonProtocolId id);

/**
 * Read a physical device (a key or an emulator)
 * @param [out] key pointer to the key to read into (must be allocated before)
 * @return true on success, false on failure
 */
bool ibutton_protocols_read(iButtonKey* key);

/**
 * Write the key to a blank
 * @param [in] key pointer to the key to be written
 * @return true on success, false on failure
 */
bool ibutton_protocols_write_blank(iButtonKey* key);

/**
 * Write the key to another one of the same type
 * @param [in] key pointer to the key to be written
 * @return true on success, false on failure
 */
bool ibutton_protocols_write_copy(iButtonKey* key);

/**
 * Start emulating the key
 * @param [in] key pointer to the key to be emulated
 */
void ibutton_protocols_emulate_start(iButtonKey* key);

/**
 * Stop emulating the key
 * @param [in] key pointer to the key to be emulated
 */
void ibutton_protocols_emulate_stop(iButtonKey* key);

/**
 * Save the key data to a FFF file.
 * @param [in] key pointer to the key to be saved
 * @param [in] file_name full absolute path to the file name
 * @return true on success, false on failure
 */
bool ibutton_protocols_save(const iButtonKey* key, const char* file_name);

/**
 * Load the key from a FFF file.
 * @param [out] key pointer to the key to load into (must be allocated before)
 * @param [in] file_name full absolute path to the file name
 * @return true on success, false on failure
 */
bool ibutton_protocols_load(iButtonKey* key, const char* file_name);

/**
 * Format a string containing device full data
 * @param [in] key pointer to the key to be rendered
 * @param [out] result pointer to the FuriString instance (must be initialized)
 */
void ibutton_protocols_render_data(const iButtonKey* key, FuriString* result);

/**
 * Format a string containing device brief data
 * @param [in] key pointer to the key to be rendered
 * @param [out] result pointer to the FuriString instance (must be initialized)
 */
void ibutton_protocols_render_brief_data(const iButtonKey* key, FuriString* result);

/**
 * Format a string containing error message (for invalid keys)
 * @param [in] key pointer to the key to be rendered
 * @param [out] result pointer to the FuriString instance (must be initialized)
 */
void ibutton_protocols_render_error(const iButtonKey* key, FuriString* result);

/**
 * Check whether the key data is valid
 * @param [in] key pointer to the key to be checked
 * @return true if data is valid, false otherwise
 */
bool ibutton_protocols_is_valid(const iButtonKey* key);

/**
 * Get a pointer to the key's editable data (for in-place editing)
 * @param [in] key pointer to the key to be checked
 * @param [out] editable pointer to a structure to contain the editable data
 */
void ibutton_protocols_get_editable_data(const iButtonKey* key, iButtonEditableData* editable);

/**
 * Make all necessary internal adjustments after editing the key
 * @param [in,out] key pointer to the key to be adjusted
 */
void ibutton_protocols_apply_edits(const iButtonKey* key);

#ifdef __cplusplus
}
#endif
