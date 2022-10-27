#include "../nfc_i.h"

const char* const mfc_logging_text[] = {
    "OFF",
    "ON",
};

static void nfc_scene_settings_set_mfc_logging(VariableItem* item) {
    Nfc* nfc = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, mfc_logging_text[index]);
    nfc->settings.mfc_nonce_logging = index;
    nfc_settings_save(&nfc->settings);
}

void nfc_scene_settings_on_enter(void* context) {
    Nfc* nfc = context;
    VariableItem* item;
    uint8_t value_index;

    item = variable_item_list_add(
        nfc->variable_item_list,
        "MFC nonce log",
        COUNT_OF(mfc_logging_text),
        nfc_scene_settings_set_mfc_logging,
        nfc);
    value_index = nfc->settings.mfc_nonce_logging ? 1 : 0;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, mfc_logging_text[value_index]);

    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewSettings);
}

bool nfc_scene_settings_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    bool consumed = false;
    return consumed;
}

void nfc_scene_settings_on_exit(void* context) {
    Nfc* nfc = context;
    //variable_item_list_set_selected_item(nfc->variable_item_list, 0);
    variable_item_list_reset(nfc->variable_item_list);
}