#pragma once

#include <stdbool.h>
#include <flipper_format.h>

#include "ibutton_protocol_common.h"

typedef void iButtonProtocols;
typedef uint32_t iButtonGroupId;
typedef int32_t iButtonLocalProtocolId;

typedef iButtonProtocols* (*iButtonProtocolsAllocFunc)(void);
typedef void (*iButtonProtocolsFreeFunc)(iButtonProtocols*);

typedef void (*iButtonProtocolsRenderFunc)(
    iButtonProtocols*,
    const iButtonProtocolData*,
    iButtonProtocolId,
    FuriString*);

typedef bool (*iButtonProtocolsIsValidFunc)(
    iButtonProtocols*,
    const iButtonProtocolData*,
    iButtonProtocolId);

typedef void (*iButtonProtocolsGetDataFunc)(
    iButtonProtocols*,
    iButtonProtocolData*,
    iButtonProtocolId,
    iButtonEditableData*);

typedef void (
    *iButtonProtocolsApplyFunc)(iButtonProtocols*, iButtonProtocolData*, iButtonProtocolId);

typedef size_t (*iButtonProtocolsGetSizeFunc)(iButtonProtocols*);
typedef uint32_t (*iButtonProtocolsGetFeaturesFunc)(iButtonProtocols*, iButtonProtocolId);
typedef const char* (*iButtonProtocolsGetStringFunc)(iButtonProtocols*, iButtonProtocolId);

typedef bool (*iButtonProtocolsGetIdFunc)(iButtonProtocols*, iButtonProtocolId*, const char*);

typedef bool (
    *iButtonProtocolsReadFunc)(iButtonProtocols*, iButtonProtocolData*, iButtonProtocolId*);

typedef bool (
    *iButtonProtocolsWriteFunc)(iButtonProtocols*, iButtonProtocolData*, iButtonProtocolId);

typedef bool (*iButtonProtocolsSaveFunc)(
    iButtonProtocols*,
    const iButtonProtocolData*,
    iButtonProtocolId,
    FlipperFormat*);

typedef bool (*iButtonProtocolsLoadFunc)(
    iButtonProtocols*,
    iButtonProtocolData*,
    iButtonProtocolId,
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
