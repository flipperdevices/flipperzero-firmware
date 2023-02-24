#pragma once

#include <core/string.h>

#include <flipper_format.h>

#include "../ibutton_protocol_common.h"

typedef struct iButtonProtocolsMisc iButtonProtocolsMisc;

iButtonProtocolsMisc* ibutton_protocols_misc_alloc();

void ibutton_protocols_misc_free(iButtonProtocolsMisc* protocols);

size_t ibutton_protocols_misc_get_max_data_size(iButtonProtocolsMisc* protocols);

bool ibutton_protocols_misc_get_id_by_name(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolId* protocol_id,
    const char* protocol_name);

const char* ibutton_protocols_misc_get_manufacturer(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolId protocol_id);

const char*
    ibutton_protocols_misc_get_name(iButtonProtocolsMisc* protocols, iButtonProtocolId protocol_id);

bool ibutton_protocols_misc_read(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId* protocol_id);

void ibutton_protocols_misc_emulate_start(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id);

void ibutton_protocols_misc_emulate_stop(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolId protocol_id);

bool ibutton_protocols_misc_save(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FlipperFormat* ff);

bool ibutton_protocols_misc_load(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    uint32_t format_version,
    FlipperFormat* ff);

void ibutton_protocols_misc_render_data(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result);

void ibutton_protocols_misc_render_brief_data(
    iButtonProtocolsMisc* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result);

void ibutton_protocols_misc_get_editable_data(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    iButtonProtocolEditableData* data);

void ibutton_protocols_misc_apply_edits(
    iButtonProtocolsMisc* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id);
