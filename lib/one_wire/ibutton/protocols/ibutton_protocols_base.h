#pragma once

#include <stdbool.h>
#include <flipper_format.h>

#include "ibutton_protocol_common_i.h"

typedef void iButtonProtocols;
typedef uint32_t iButtonGroupId;

typedef iButtonProtocols* (*iButtonProtocolsAllocFunc)(void);
typedef void (*iButtonProtocolsFreeFunc)(iButtonProtocols*);

typedef void (*iButtonProtocolsRenderFunc)(
    iButtonProtocols*,
    const iButtonProtocolData*,
    iButtonProtocolLocalId,
    FuriString*);

typedef bool (*iButtonProtocolsIsValidFunc)(
    iButtonProtocols*,
    const iButtonProtocolData*,
    iButtonProtocolLocalId);

typedef void (*iButtonProtocolsGetDataFunc)(
    iButtonProtocols*,
    iButtonProtocolData*,
    iButtonProtocolLocalId,
    iButtonEditableData*);

typedef void (
    *iButtonProtocolsApplyFunc)(iButtonProtocols*, iButtonProtocolData*, iButtonProtocolLocalId);

typedef size_t (*iButtonProtocolsGetSizeFunc)(iButtonProtocols*);
typedef uint32_t (*iButtonProtocolsGetFeaturesFunc)(iButtonProtocols*, iButtonProtocolLocalId);
typedef const char* (*iButtonProtocolsGetStringFunc)(iButtonProtocols*, iButtonProtocolLocalId);

typedef bool (*iButtonProtocolsGetIdFunc)(iButtonProtocols*, iButtonProtocolLocalId*, const char*);

typedef bool (
    *iButtonProtocolsReadFunc)(iButtonProtocols*, iButtonProtocolData*, iButtonProtocolLocalId*);

typedef bool (
    *iButtonProtocolsWriteFunc)(iButtonProtocols*, iButtonProtocolData*, iButtonProtocolLocalId);

typedef bool (*iButtonProtocolsSaveFunc)(
    iButtonProtocols*,
    const iButtonProtocolData*,
    iButtonProtocolLocalId,
    FlipperFormat*);

typedef bool (*iButtonProtocolsLoadFunc)(
    iButtonProtocols*,
    iButtonProtocolData*,
    iButtonProtocolLocalId,
    uint32_t,
    FlipperFormat*);

typedef struct {
    const uint32_t protocol_count;

    iButtonProtocolsAllocFunc alloc;
    iButtonProtocolsFreeFunc free;

    iButtonProtocolsGetSizeFunc get_max_data_size;
    iButtonProtocolsGetIdFunc get_id_by_name;
    iButtonProtocolsGetFeaturesFunc get_features;

    iButtonProtocolsGetStringFunc get_manufacturer;
    iButtonProtocolsGetStringFunc get_name;

    iButtonProtocolsReadFunc read;
    iButtonProtocolsWriteFunc write_blank;
    iButtonProtocolsWriteFunc write_copy;

    iButtonProtocolsApplyFunc emulate_start;
    iButtonProtocolsApplyFunc emulate_stop;

    iButtonProtocolsSaveFunc save;
    iButtonProtocolsLoadFunc load;

    iButtonProtocolsRenderFunc render_data;
    iButtonProtocolsRenderFunc render_brief_data;
    iButtonProtocolsRenderFunc render_error;

    iButtonProtocolsIsValidFunc is_valid;
    iButtonProtocolsGetDataFunc get_editable_data;

    iButtonProtocolsApplyFunc apply_edits;
} iButtonProtocolsBase;
