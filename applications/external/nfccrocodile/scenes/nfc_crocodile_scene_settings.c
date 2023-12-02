#include "../nfc_crocodile_i.h"
#include "nfc_crocodile_scene.h"

enum NfcCrocodileSettingsIndex { NfcCrocodileSettingsStorageMode };

#define NFC_CROCODILE_STORAGE_COUNT 2
const char* nfc_crocodile_settings_storage_text[NFC_CROCODILE_STORAGE_COUNT] = {
    "Text",
    "URL",
};

const uint32_t nfc_crocodile_settings_storage_value[NFC_CROCODILE_STORAGE_COUNT] = {
    NfcCrocodileStorageText,
    NfcCrocodileStorageURL,
};

static void nfc_crocodile_scene_settings_set_storage(VariableItem* item) {
    NfcCrocodile* nfc_crocodile = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);

    variable_item_set_current_value_text(item, nfc_crocodile_settings_storage_text[index]);
    nfc_crocodile->storage_type = nfc_crocodile_settings_storage_value[index];
}

void nfc_crocodile_scene_settings_on_enter(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    VariableItem* item;

    item = variable_item_list_add(
        nfc_crocodile->variable_item_list,
        "Storage Type",
        NFC_CROCODILE_STORAGE_COUNT,
        nfc_crocodile_scene_settings_set_storage,
        context);
    uint8_t index = nfc_crocodile->storage_type;
    variable_item_set_current_value_index(item, index);
    variable_item_set_current_value_text(item, nfc_crocodile_settings_storage_text[index]);

    view_dispatcher_switch_to_view(
        nfc_crocodile->view_dispatcher, NfcCrocodileViewVariableItemList);
}

bool nfc_crocodile_scene_settings_on_event(void* context, SceneManagerEvent event) {
    NfcCrocodile* nfc_crocodile = context;
    bool consumed = false;

    UNUSED(event);
    UNUSED(nfc_crocodile);

    return consumed;
}

void nfc_crocodile_scene_settings_on_exit(void* context) {
    NfcCrocodile* nfc_crocodile = context;
    variable_item_list_set_selected_item(nfc_crocodile->variable_item_list, 0);
    variable_item_list_reset(nfc_crocodile->variable_item_list);
}