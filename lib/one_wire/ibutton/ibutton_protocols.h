#pragma once

#include "protocols/ibutton_protocol_defs.h"

/**
 * Get protocol id from iButton family code
 * @param [in] family_code
 * @return protocol id or iButtonProtocolMax if protocol not found
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

/**
 * Read the device ROM
 * @param [in] host pointer to a OneWire host instance
 * @return true on success, false on failure
 */
bool ibutton_protocols_read(OneWireHost* host, iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Save the key data to a FFF file. The header must be written beforehand.
 * @param [out] ff pointer to a FlipperFormat instance
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_save(FlipperFormat* ff, const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Save the key data to a FFF file. The header must be read beforehand.
 * @param [in] ff pointer to a FlipperFormat instance
 * @param [in] version file format version to use when loading
 * @param [out] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_load(FlipperFormat* ff, uint32_t version, iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Format a string containing device full data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_render_data(FuriString* result, const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Format a string containing device brief data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 */
void ibutton_protocols_render_brief_data(FuriString* result, const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Check whether the data is valid according to the selected protocol
 * @param [in] protocol_data pointer to protocol-specific data
 * @param [in] protocol_id id of the protocol in question
 * @return true if data is valid, false otherwise
 */
bool ibutton_protocols_is_valid(const iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);
