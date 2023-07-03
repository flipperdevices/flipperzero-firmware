#pragma once

#include <nfc/protocols/iso14443_3a/iso14443_3a.h>

#include "iso14443_3a.h"

bool nfc_protocol_support_handle_scene_saved_menu_iso14443_3a_common(
    NfcApp* instance,
    uint32_t event);
