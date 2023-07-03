#pragma once

#include <core/string.h>

#include "nfc_protocol_support_common.h"

typedef void (*NfcProtocolSupportRenderData)(
    const NfcDeviceData* data,
    NfcProtocolFormatType format_type,
    FuriString* str);

typedef NfcCustomEvent (
    *NfcProtocolSupportPollerHandler)(NfcGenericEventData* event_data, void* context);

typedef void (*NfcProtocolSupportSceneBuilder)(NfcApp* instance);

typedef bool (*NfcProtocolSupportSceneHandler)(NfcApp* instance, uint32_t event);

typedef struct {
    const uint32_t features;

    NfcProtocolSupportRenderData render_info;

    NfcProtocolSupportPollerHandler handle_poller;

    NfcProtocolSupportSceneBuilder build_scene_saved_menu;

    NfcProtocolSupportSceneHandler handle_scene_info;
    NfcProtocolSupportSceneHandler handle_scene_read_success;
    NfcProtocolSupportSceneHandler handle_scene_saved_menu;
} NfcProtocolSupportBase;
