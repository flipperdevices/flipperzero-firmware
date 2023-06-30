#pragma once

#include "nfc_protocol_support_common.h"
#include "../../nfc_app.h"

NfcProtocolFeature nfc_protocol_support_get_features(const NfcDevice* device);

void nfc_protocol_support_render_info(
    const NfcDevice* device,
    NfcProtocolFormatType format_type,
    FuriString* str);

// Poller handler
NfcCustomEvent nfc_protocol_support_handle_poller(NfcGenericEvent event, void* context);

// Listener handler
// TODO

// Scene builders
void nfc_protocol_support_build_scene_saved_menu(NfcApp* instance);

// Scene handlers
bool nfc_protocol_support_handle_scene_saved_menu(NfcApp* instance, uint32_t event);
