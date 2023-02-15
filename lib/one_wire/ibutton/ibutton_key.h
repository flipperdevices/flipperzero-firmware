/**
 * @file ibutton_key.h
 * 
 * iButton key data holder
 */

#pragma once

#include <core/string.h>

#include <one_wire/one_wire_host.h>
#include <one_wire/one_wire_slave.h>

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

bool ibutton_key_is_valid(iButtonKey* key);

const char* ibutton_key_get_manufacturer_name(iButtonKey* key);

const char* ibutton_key_get_protocol_name(iButtonKey* key);

void ibutton_key_get_rendered_data(iButtonKey* key, FuriString* result);

void ibutton_key_get_rendered_brief_data(iButtonKey* key, FuriString* result);

void ibutton_key_get_rendered_error(iButtonKey* key, FuriString* result);

uint32_t ibutton_key_get_features(iButtonKey* key);

bool ibutton_key_read(iButtonKey* key, OneWireHost* host);

bool ibutton_key_write_blank(iButtonKey* key, OneWireHost* host);

bool ibutton_key_write_copy(iButtonKey* key, OneWireHost* host);

bool ibutton_key_emulate(iButtonKey* key, OneWireSlave* bus);

bool ibutton_key_save(iButtonKey* key, const char* file_name);

bool ibutton_key_load(iButtonKey* key, const char* file_name);

#ifdef __cplusplus
}
#endif
