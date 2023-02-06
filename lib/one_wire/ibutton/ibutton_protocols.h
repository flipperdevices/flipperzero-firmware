#pragma once

#include <core/string.h>

#include <one_wire/one_wire_host.h>

#include "protocols/ibutton_protocol_defs.h"

/**
 * Get protocol id from iButton family code
 * @param [in] family_code
 * @return protocol id or iButtonProtocolMax if protocol not found
 */
iButtonProtocol ibutton_protocols_get_id_by_family_code(uint8_t family_code);

/**
 * Get data size based on the protocol index
 * @param [in] protocol_index index of the protocol in question
 * @return data size in bytes
 */
size_t ibutton_protocols_get_data_size(iButtonProtocol protocol_id);

/**
 * Get maximum data size out of all protocols available
 * @return maximum data size in bytes
 */
size_t ibutton_protocols_get_max_data_size();

/**
 * Read the device ROM
 * @param [in] host pointer to a OneWire host instance
 * @return true on success, false on failure
 */
bool ibutton_protocols_read(OneWireHost* host, iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Format a string containing device full data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 */
void ibutton_protocols_render_data(FuriString* result, iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

/**
 * Format a string containing device brief data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 */
void ibutton_protocols_render_brief_data(FuriString* result, iButtonProtocolData* protocol_data, iButtonProtocol protocol_id);

