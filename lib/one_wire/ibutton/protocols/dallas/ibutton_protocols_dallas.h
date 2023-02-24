#pragma once

#include <core/string.h>

#include <flipper_format.h>

#include "../ibutton_protocol_base.h"

typedef struct iButtonProtocolsDallas iButtonProtocolsDallas;

iButtonProtocolsDallas* ibutton_protocols_dallas_alloc();

void ibutton_protocols_dallas_free(iButtonProtocolsDallas* protocols);

size_t ibutton_protocols_dallas_get_max_data_size(iButtonProtocolsDallas* protocols);

bool ibutton_protocols_dallas_get_id_by_name(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId* protocol_id,
    const char* protocol_name);

uint32_t ibutton_protocols_dallas_get_features(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id);

const char* ibutton_protocols_dallas_get_manufacturer(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id);

const char* ibutton_protocols_dallas_get_name(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id);

bool ibutton_protocols_dallas_read(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId* protocol_id);

bool ibutton_protocols_dallas_write_blank(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id);

bool ibutton_protocols_dallas_write_copy(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id);

void ibutton_protocols_dallas_emulate_start(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id);

void ibutton_protocols_dallas_emulate_stop(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolId protocol_id);

bool ibutton_protocols_dallas_save(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FlipperFormat* ff);

bool ibutton_protocols_dallas_load(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    uint32_t version,
    FlipperFormat* ff);

void ibutton_protocols_dallas_render_data(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result);

void ibutton_protocols_dallas_render_brief_data(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result);

void ibutton_protocols_dallas_render_error(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    FuriString* result);

bool ibutton_protocols_dallas_is_valid(
    iButtonProtocolsDallas* protocols,
    const iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id);

void ibutton_protocols_dallas_get_editable_data(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id,
    iButtonProtocolEditableData* data);

void ibutton_protocols_dallas_apply_edits(
    iButtonProtocolsDallas* protocols,
    iButtonProtocolData* protocol_data,
    iButtonProtocolId protocol_id);
