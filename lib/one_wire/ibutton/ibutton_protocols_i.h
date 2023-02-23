#pragma once

#include "ibutton_protocols.h"

#include "protocols/ibutton_protocol_defs_i.h"

/**
 * Get protocol id from iButton family code
 * @param [in] family_code
 * @return protocol id or iButtonProtocolGeneric if protocol not found
 */
iButtonProtocol ibutton_protocols_get_id_by_family_code(uint8_t family_code);

/**
 * Get protocol id from iButton protocol name
 * @param [in] protocol_name
 * @return protocol id or iButtonProtocolMax if protocol not found
 */
iButtonProtocol ibutton_protocols_get_id_by_name(const char* protocol_name);

/**
 * Get protocol features bitmask by protocol id
 * @param [in] protocol_id id of the protocol in question
 */
uint32_t ibutton_protocols_get_features(iButtonProtocol protocol_id);

/**
 * Get data size based on the protocol id
 * @param [in] protocol_id id of the protocol in question
 * @return data size in bytes
 */
size_t ibutton_protocols_get_data_size(iButtonProtocol protocol_id);

/**
 * Get maximum data size out of all protocols available
 * @return maximum data size in bytes
 */
size_t ibutton_protocols_get_max_data_size();

/**
 * Read the device using the selected protocol
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [out] protocol_id id of the detected protocol or iButtonProtocolMax on failure
 * @return true on success, false on failure
 */
bool ibutton_protocols_read(iButtonProtocolData* protocol_data, iButtonProtocol* protocol_id);

/**
 * Write the key to a blank using the selected protocol
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_write_blank(iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Write the key to another one of the same type using the selected protocol
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_write_copy(iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Start emulating the key using the selected protocol
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_emulate_start(
    iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id);

/**
 * Stop emulating the key using the selected protocol
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_emulate_stop(iButtonProtocol protocol_id);

/**
 * Save the key data to a FFF file.
 * @param [in] file_name full absolute path to the file name
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_save(
    const char* file_name,
    const iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id);

/**
 * Save the key data to a FFF file.
 * @param [in] file_name full absolute path to the file name
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [out] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_load(
    const char* file_name,
    iButtonProtocolData* protocol_data,
    iButtonProtocol* protocol_id);

/**
 * Format a string containing device full data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the detected protocol or iButtonProtocolMax on failure
 */
void ibutton_protocols_render_data(
    FuriString* result,
    const iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id);

/**
 * Format a string containing device brief data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_render_brief_data(
    FuriString* result,
    const iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id);

/**
 * Format a string containing error message (for invalid keys)
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_render_error(
    FuriString* result,
    const iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id);

/**
 * Check whether the data is valid according to the selected protocol
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true if data is valid, false otherwise
 */
bool ibutton_protocols_is_valid(
    const iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id);

/**
 * Get a pointer to the protocol's editable data and its size
 * @param [out] data pointer to a pointer to contain the start of data
 * @param [out] data_size pointer to a variable to contain the data size
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_get_editable_data(
    uint8_t** data,
    size_t* data_size,
    iButtonProtocolData* protocol_data,
    iButtonProtocol protocol_id);

/**
 * Make all necessary internal adjustments after editing the data
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_apply_edits(iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);
