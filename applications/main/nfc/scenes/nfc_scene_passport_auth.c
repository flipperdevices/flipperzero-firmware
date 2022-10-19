#include "../nfc_i.h"

#define TAG "PassportAuth"

#define MRTD_AUTH_METHOD_COUNT 4
// Indexes must match MrtdAuthMethod (lib/nfc/protocols/mrtd_helpers.h)
const char* const mrtd_auth_method_text[MRTD_AUTH_METHOD_COUNT] = {
    "None",
    "Any",
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

void nfc_scene_passport_auth_var_list_enter_callback(void* context, uint32_t index) {
    Nfc* nfc = context;
    view_dispatcher_send_custom_event(nfc->view_dispatcher, index);
}

void nfc_scene_passport_auth_method_changed(VariableItem* item) {
    Nfc* nfc = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    nfc->dev->dev_data.mrtd_data.auth.method = index;
    variable_item_set_current_value_text(item, mrtd_auth_method_text[index]);
}

void nfc_scene_passport_auth_on_enter(void* context) {
    Nfc* nfc = context;
    MrtdData* mrtd_data = &nfc->dev->dev_data.mrtd_data;

    // By entering the Auth menu, we default to Auth: Any
    MrtdAuthMethod* auth_method = &mrtd_data->auth.method;
    if(*auth_method == MrtdAuthMethodNone) {
        *auth_method = MrtdAuthMethodAny;
    }

    VariableItemList* variable_item_list = nfc->variable_item_list;

    VariableItem* item;
    uint8_t value_index;

    const size_t temp_str_size = 15;
    char temp_str[temp_str_size];
    snprintf(
        temp_str,
        temp_str_size,
        "%02u%02u%02u",
        mrtd_data->auth.birth_date.year,
        mrtd_data->auth.birth_date.month,
        mrtd_data->auth.birth_date.day);

    item = variable_item_list_add(variable_item_list, "Birth Date", 1, NULL, NULL);
    variable_item_set_current_value_text(item, temp_str);

    snprintf(
        temp_str,
        temp_str_size,
        "%02u%02u%02u",
        mrtd_data->auth.expiry_date.year,
        mrtd_data->auth.expiry_date.month,
        mrtd_data->auth.expiry_date.day);

    item = variable_item_list_add(variable_item_list, "Expiry Date", 1, NULL, NULL);
    variable_item_set_current_value_text(item, temp_str);

    item = variable_item_list_add(variable_item_list, "Document Nr.", 1, NULL, NULL);

    strncpy(temp_str, mrtd_data->auth.doc_number, temp_str_size);
    temp_str[temp_str_size] = '\x00';
    if(strlen(temp_str) > 8) {
        temp_str[8] = '.';
        temp_str[9] = '.';
        temp_str[10] = '.';
        temp_str[11] = '\x00';
    }
    variable_item_set_current_value_text(item, temp_str);

    item = variable_item_list_add(
        variable_item_list,
        "Method",
        MRTD_AUTH_METHOD_COUNT,
        nfc_scene_passport_auth_method_changed,
        nfc);

    value_index = *auth_method;
    variable_item_set_current_value_index(item, value_index);
    variable_item_set_current_value_text(item, mrtd_auth_method_text[value_index]);

    variable_item_list_add(variable_item_list, "Authenticate and read", 1, NULL, NULL);

    variable_item_list_set_enter_callback(
        variable_item_list, nfc_scene_passport_auth_var_list_enter_callback, nfc);
    view_dispatcher_switch_to_view(nfc->view_dispatcher, NfcViewVarItemList);
}

bool nfc_scene_passport_auth_on_event(void* context, SceneManagerEvent event) {
    Nfc* nfc = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        FURI_LOG_D(TAG, "event.event: %ld", event.event);
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
            scene_manager_next_scene(nfc->scene_manager, NfcScenePassportDocNr);
            consumed = true;
            break;
        case NfcScenePassportAuthSelectMethod:
            consumed = true;
            break;
        case NfcScenePassportAuthSelectAuth:
            if(nfc->dev->dev_data.mrtd_data.auth.method == MrtdAuthMethodPace) {
                scene_manager_next_scene(nfc->scene_manager, NfcScenePassportPaceTodo);
            } else {
                nfc_device_clear(nfc->dev);
                scene_manager_next_scene(nfc->scene_manager, NfcSceneRead);
            }
            consumed = true;
            break;
        }
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_previous_scene(nfc->scene_manager);
    }

    return consumed;
}

void nfc_scene_passport_auth_on_exit(void* context) {
    Nfc* nfc = context;

    // Clear view
    variable_item_list_reset(nfc->variable_item_list);
}
