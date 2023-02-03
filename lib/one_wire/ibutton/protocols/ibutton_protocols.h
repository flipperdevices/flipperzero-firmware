#pragma once

#include <core/string.h>

#include <one_wire/one_wire_host.h>

typedef enum {
    iButtonProtocolDS1990,
    iButtonProtocolDS1992,
    iButtonProtocolMax
} iButtonProtocol;

/**
 * Get protocol index from iButton family code
 * @param [in] family_code
 * @return protocol index or iButtonProtocolMax if protocol not found
 */
size_t ibutton_protocols_get_index_by_family_code(uint8_t family_code);

/**
 * Get ROM size based on the protocol index
 * @param [in] protocol_index index of the protocol in question
 * @return ROM size in bytes
 */
size_t ibutton_protocols_get_rom_size(size_t protocol_index);

/**
 * Get ROM size based on the protocol index
 * @return maximum ROM size in bytes
 */
size_t ibutton_protocols_get_max_rom_size();

/**
 * Get user data size based on the protocol index
 * @param [in] protocol_index index of the protocol in question
 * @return user data size in bytes
 */
size_t ibutton_protocols_get_user_data_size(size_t protocol_index);

/**
 * Get maximum user data size based on the protocol index
 * @return maximum user data size in bytes
 */
size_t ibutton_protocols_get_max_user_data_size();

/**
 * Read the device ROM
 * @param [in] host pointer to a OneWire host instance
 * @param [out] data pointer to a read buffer. Must be of appropriate size.
 * @param [in] protocol_index index of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_read_rom(OneWireHost* host, uint8_t* rom_data, size_t protocol_index);

/**
 * Read device-specific user data (SRAM, EEPROM or another special function)
 * @param [in] host pointer to a OneWire host instance
 * @param [out] data pointer to a read buffer. Must be of appropriate size.
 * @param [in] protocol_index index of the protocol in question
 * @return true on success, false on failure
 */
bool ibutton_protocols_read_user_data(OneWireHost* host, void* user_data, size_t protocol_index);

/**
 * Format a string containing device brief data
 * @param [in] rom_data pointer to the ROM data
 * @param [in] user_data pointer to the device-specific user data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_index index of the protocol in question
 */
void ibutton_protocols_render_brief_data(const uint8_t* rom_data, const void* user_data, FuriString* result, size_t protocol_index);

/**
 * Format a string containing device full data
 * @param [in] rom_data pointer to the ROM data
 * @param [in] user_data pointer to the device-specific user data
 * @param [out] result pointer to the FuriString instance (must be initialized)
 * @param [in] protocol_index index of the protocol in question
 */
void ibutton_protocols_render_data(const uint8_t* rom_data, const void* user_data, FuriString* result, size_t protocol_index);
