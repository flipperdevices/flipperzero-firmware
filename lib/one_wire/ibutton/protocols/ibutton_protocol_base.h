#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include <core/string.h>
#include <one_wire/one_wire_host.h>

typedef void iButtonProtocolData;

typedef bool (*iButtonProtocolReadFunc)(OneWireHost*, iButtonProtocolData*);
typedef void (*iButtonProtocolRenderDataFunc)(FuriString*, const iButtonProtocolData*);

typedef struct {
    const uint8_t family_code;
    const size_t data_size;

    iButtonProtocolReadFunc read;
    iButtonProtocolRenderDataFunc render_data;
    iButtonProtocolRenderDataFunc render_brief_data;
} iButtonProtocolBase;
