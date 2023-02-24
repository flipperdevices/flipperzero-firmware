#pragma once

#include "../ibutton_protocol_common.h"

#include <flipper_format.h>

#include <one_wire/one_wire_host.h>
#include <one_wire/one_wire_slave.h>

typedef bool (*iButtonProtocolReadWriteFunc)(OneWireHost*, iButtonProtocolData*);
typedef void (*iButtonProtocolEmulateFunc)(OneWireSlave*, iButtonProtocolData*);
typedef bool (*iButtonProtocolSaveFunc)(FlipperFormat*, const iButtonProtocolData*);
typedef bool (*iButtonProtocolLoadFunc)(FlipperFormat*, uint32_t, iButtonProtocolData*);
typedef void (*iButtonProtocolRenderDataFunc)(FuriString*, const iButtonProtocolData*);
typedef bool (*iButtonProtocolIsValidFunc)(const iButtonProtocolData*);
typedef void (
    *iButtonProtocolGetEditableDataFunc)(iButtonProtocolEditableData*, iButtonProtocolData*);
typedef void (*iButtonProtocolApplyEditsFunc)(iButtonProtocolData*);

typedef struct {
    const uint8_t family_code;
    const uint32_t features;
    const size_t data_size;
    const char* manufacturer;
    const char* name;

    iButtonProtocolReadWriteFunc read;
    iButtonProtocolReadWriteFunc write_blank;
    iButtonProtocolReadWriteFunc write_copy;
    iButtonProtocolEmulateFunc emulate;
    iButtonProtocolSaveFunc save;
    iButtonProtocolLoadFunc load;
    iButtonProtocolRenderDataFunc render_data;
    iButtonProtocolRenderDataFunc render_brief_data;
    iButtonProtocolRenderDataFunc render_error;
    iButtonProtocolIsValidFunc is_valid;
    iButtonProtocolGetEditableDataFunc get_editable_data;
    iButtonProtocolApplyEditsFunc apply_edits;
} iButtonProtocolDallasBase;
