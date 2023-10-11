#pragma once

#include <gui/scene_manager.h>

#include "nfc_protocol_support_common.h"

void nfc_protocol_support_on_enter(NfcProtocolSupportScene scene, void* context);

bool nfc_protocol_support_on_event(
    NfcProtocolSupportScene scene,
    void* context,
    SceneManagerEvent event);

void nfc_protocol_support_on_exit(NfcProtocolSupportScene scene, void* context);
