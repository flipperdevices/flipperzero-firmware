#include "../nfc_app_i.h"
#include "helpers/logger_formatter.h"

enum SubmenuDebugIndex {
    SubmenuDebugIndexField,
    SubmenuDebugIndexApdu,
    SubmenuDebugIndexLogger,
    SubmenuDebugIndexFormatLogs,
};

void nfc_scene_debug_submenu_callback(void* context, uint32_t index) {
    NfcApp* nfc = context;

    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

static void nfc_scene_debug_add_format_logs_button(NfcApp* nfc) {
    if(!nfc->logger_enabled) return;
    nfc_logger_config(nfc_get_logger(nfc->nfc), nfc->logger_enabled, NFC_LOG_FOLDER);
    submenu_add_item(
        nfc->submenu,
        "Format logs",
        SubmenuDebugIndexFormatLogs,
        nfc_scene_debug_submenu_callback,
        nfc);
}

void nfc_scene_debug_on_enter(void* context) {
    NfcApp* nfc = context;
    Submenu* submenu = nfc->submenu;

    submenu_add_item(
        submenu, "Field", SubmenuDebugIndexField, nfc_scene_debug_submenu_callback, nfc);

    submenu_add_item(
        submenu,
        nfc->logger_enabled ? "Disable Logger" : "Enable Logger",
        SubmenuDebugIndexLogger,
        nfc_scene_debug_submenu_callback,
        nfc);

    nfc_scene_debug_add_format_logs_button(nfc);

    submenu_set_selected_item(
        submenu, scene_manager_get_scene_state(nfc->scene_manager, NfcSceneDebug));

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewMenu);
}

bool nfc_scene_debug_on_event(void* context, SceneManagerEvent event) {
    NfcApp* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubmenuDebugIndexField) {
            scene_manager_set_scene_state(
                nfc->scene_manager, NfcSceneDebug, SubmenuDebugIndexField);
            scene_manager_next_scene(nfc->scene_manager, NfcSceneField);
            consumed = true;
        } else if(event.event == SubmenuDebugIndexLogger) {
            nfc->logger_enabled ^= 1;
            submenu_change_item_label(
                nfc->submenu,
                SubmenuDebugIndexLogger,
                nfc->logger_enabled ? "Disable Logger" : "Enable Logger");
            nfc_scene_debug_add_format_logs_button(nfc);
            consumed = true;
        } else if(event.event == SubmenuDebugIndexFormatLogs) {
            nfc_logger_format(nfc->nfc, &nfc->logger_config);
        }
    }
    return consumed;
}

void nfc_scene_debug_on_exit(void* context) {
    NfcApp* nfc = context;

    submenu_reset(nfc->submenu);
}
