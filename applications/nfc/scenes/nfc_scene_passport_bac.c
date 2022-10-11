#include "../nfc_i.h"

#define TAG "PassportBac"

#define MRTD_AUTH_METHOD_COUNT 2
// Indexes must match MrtdAuthMethod (lib/nfc/protocols/mrtd.h)
const char* const mrtd_auth_method_text[MRTD_AUTH_METHOD_COUNT] = {
    "BAC",
    "PACE",
};

typedef enum {
    NfcScenePassportAuthSelectDob,
    NfcScenePassportAuthSelectDoe,
    NfcScenePassportAuthSelectDocNr,
    NfcScenePassportAuthSelectMethod,
    NfcScenePassportAuthSelectAuth,
} NfcScenePassportAuthSelect;

void nfc_scene_passport_bac_var_list_enter_callback(void* context, uint32_t index) {
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_passport_bac_auth_method_changed(VariableItem* item) {
    Nfc* nfc = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    nfc->dev->dev_data.mrtd_data.auth.method = index;
    variable_item_set_current_value_text(item, mrtd_auth_method_text[index]);
}

void nfc_scene_passport_bac_on_enter(void* context) {
    Nfc* nfc = context;

    VariableItemList* variable_item_list = nfc->variable_item_list;

    VariableItem* item;
    uint8_t value_index;

    const size_t temp_str_size = 10;
    char temp_str[temp_str_size];
    snprintf(temp_str, temp_str_size, "%02u%02u%02u",
        nfc->dev->dev_data.mrtd_data.auth.bac.birth_date.year,
        nfc->dev->dev_data.mrtd_data.auth.bac.birth_date.month,
        nfc->dev->dev_data.mrtd_data.auth.bac.birth_date.day);

    item = variable_item_list_add(variable_item_list, "Birth Date", 1, NULL, NULL);
    variable_item_set_current_value_text(item, temp_str);

    snprintf(temp_str, temp_str_size, "%02u%02u%02u",
        nfc->dev->dev_data.mrtd_data.auth.bac.expiry_date.year,
        nfc->dev->dev_data.mrtd_data.auth.bac.expiry_date.month,
        nfc->dev->dev_data.mrtd_data.auth.bac.expiry_date.day);

    item = variable_item_list_add(variable_item_list, "Expiry Date", 1, NULL, NULL);
    variable_item_set_current_value_text(item, temp_str);

    variable_item_list_add(variable_item_list, "Document Nr.", 1, NULL, NULL);
    //TODO: add scene to enter docnr, based on nfc_scene_passport_date.c

    item = variable_item_list_add(
        variable_item_list,
        "Method",
        MRTD_AUTH_METHOD_COUNT,
        nfc_scene_passport_bac_auth_method_changed,
        nfc);

    value_index = nfc->dev->dev_data.mrtd_data.auth.method;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, mrtd_auth_method_text[value_index]);

    variable_item_list_add(variable_item_list, "Authenticate and read", 1, NULL, NULL);

    variable_item_list_set_enter_callback(
        variable_item_list, nfc_scene_passport_bac_var_list_enter_callback, nfc);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewVarItemList);
}

bool nfc_scene_passport_bac_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        FURI_LOG_D(TAG, "event.event: %d", event.event);
        switch(event.event) {
        case NfcScenePassportAuthSelectDob:
            scene_manager_set_scene_state(nfc->scene_manager, NfcScenePassportDate, 0);
            scene_manager_next_scene(nfc->scene_manager, NfcScenePassportDate);
            consumed = true;
            break;
        case NfcScenePassportAuthSelectDoe:
            scene_manager_set_scene_state(nfc->scene_manager, NfcScenePassportDate, 1);
            scene_manager_next_scene(nfc->scene_manager, NfcScenePassportDate);
            consumed = true;
            break;
        case NfcScenePassportAuthSelectDocNr:
            consumed = true;
            break;
        case NfcScenePassportAuthSelectMethod:
            consumed = true;
            break;
        case NfcScenePassportAuthSelectAuth:
            if(nfc->dev->dev_data.mrtd_data.auth.method == MrtdAuthMethodPace) {
                scene_manager_next_scene(nfc->scene_manager, NfcScenePassportPaceTodo);
            }
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    }

    return consumed;
}

void nfc_scene_passport_bac_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    variable_item_list_reset(nfc->variable_item_list);
}
