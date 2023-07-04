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

typedef void (*NfcProtocolSupportOnEnter)(NfcApp* instance);
typedef bool (*NfcProtocolSupportOnEvent)(NfcApp* instance, uint32_t event);
typedef void (*NfcProtocolSupportOnExit)(NfcApp* instance);

typedef struct {
    NfcProtocolSupportOnEnter on_enter;
    NfcProtocolSupportOnEvent on_event;
    NfcProtocolSupportOnExit on_exit;
} NfcProtocolSupportSceneBase;

typedef struct {
    const uint32_t features;

    NfcProtocolSupportRenderData render_info;

    NfcProtocolSupportPollerHandler handle_poller;

    NfcProtocolSupportSceneBuilder build_scene_read_menu;
    NfcProtocolSupportSceneBuilder build_scene_saved_menu;

    NfcProtocolSupportSceneHandler handle_scene_info;
    NfcProtocolSupportSceneHandler handle_scene_read_menu;
    NfcProtocolSupportSceneHandler handle_scene_saved_menu;

    NfcProtocolSupportSceneBase scene_info;
    NfcProtocolSupportSceneBase scene_read;
    NfcProtocolSupportSceneBase scene_read_menu;
    NfcProtocolSupportSceneBase scene_read_success;
    NfcProtocolSupportSceneBase scene_saved_menu;
} NfcProtocolSupportBase;
