#pragma once

#include <core/string.h>

#include "../../nfc_app.h"

typedef void (*NfcProtocolSupportOnEnter)(NfcApp* instance);

typedef bool (*NfcProtocolSupportOnEvent)(NfcApp* instance, uint32_t event);

typedef void (*NfcProtocolSupportOnExit)(NfcApp* instance);

typedef struct {
    NfcProtocolSupportOnEnter on_enter;
    NfcProtocolSupportOnEvent on_event;
    /*NfcProtocolSupportOnExit on_exit; is not necessary */
} NfcProtocolSupportSceneBase;

typedef struct {
    const uint32_t features;

    NfcProtocolSupportSceneBase scene_info;
    NfcProtocolSupportSceneBase scene_more_info;
    NfcProtocolSupportSceneBase scene_read;
    NfcProtocolSupportSceneBase scene_read_menu;
    NfcProtocolSupportSceneBase scene_read_success;
    NfcProtocolSupportSceneBase scene_saved_menu;
    NfcProtocolSupportSceneBase scene_save_name;
    NfcProtocolSupportSceneBase scene_emulate;
} NfcProtocolSupportBase;
