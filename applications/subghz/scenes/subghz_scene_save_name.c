#include "../subghz_i.h"
#include <lib/toolbox/random_name.h>
#include "../helpers/subghz_custom_event.h"
#include <lib/subghz/protocols/subghz_protocol_raw.h>

void subghz_scene_save_name_text_input_callback(void* context) {
    SubGhz* subghz = context;
    view_dispatcher_send_custom_event(subghz->view_dispatcher, SubghzCustomEventSceneSaveName);
}

bool subghz_scene_save_name_validator_callback(char* file_name) {
    bool ret = true;
    string_t path;
    string_init_printf(path, "%s/%s%s", SUBGHZ_APP_PATH_FOLDER, file_name, SUBGHZ_APP_EXTENSION);
    Storage* storage = furi_record_open("storage");
    if(storage_common_stat(storage, string_get_cstr(path), NULL) == FSE_OK) {
        ret = false;
    } else {
        ret = true;
    }
    string_clear(path);
    furi_record_close("storage");
    return ret;
}

void subghz_scene_save_name_on_enter(void* context) {
    SubGhz* subghz = context;

    // Setup view
    TextInput* text_input = subghz->text_input;
    bool dev_name_empty = false;

    if(!strcmp(subghz->file_name, "")) {
        set_random_name(subghz->file_name, sizeof(subghz->file_name));

    } else {
        strcpy(subghz->file_name_tmp, subghz->file_name);
        if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReadRAW) ==
           SubghzCustomEventManagerSet) {
            subghz_get_next_name_file(subghz);
        }
    }
    //highlighting the entire filename by default
    dev_name_empty = true;

    text_input_set_header_text(text_input, "Name signal");
    text_input_set_result_callback(
        text_input,
        subghz_scene_save_name_text_input_callback,
        subghz,
        subghz->file_name,
        22, //Max len name
        dev_name_empty);

    text_input_set_validator_callback(
        text_input,
        subghz_scene_save_name_validator_callback,
        subghz->file_name,
        "This name\nexists!\nChoose\nanother one.");

    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewTextInput);
}

bool subghz_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;
    if(event.type == SceneManagerEventTypeBack) {
        strcpy(subghz->file_name, subghz->file_name_tmp);
        scene_manager_previous_scene(subghz->scene_manager);
        return true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubghzCustomEventSceneSaveName) {
            if(strcmp(subghz->file_name, "")) {
                if(strcmp(subghz->file_name_tmp, "")) {
                    if(!subghz_rename_file(subghz)) {
                        return false;
                    }
                } else {
                    subghz_save_protocol_to_file(subghz, subghz->file_name);
                }

                if(scene_manager_get_scene_state(subghz->scene_manager, SubGhzSceneReadRAW) ==
                   SubghzCustomEventManagerSet) {
                    subghz_protocol_raw_set_last_file_name(
                        (SubGhzProtocolRAW*)subghz->txrx->protocol_result, subghz->file_name);
                } else {
                    subghz_file_name_clear(subghz);
                }

                scene_manager_next_scene(subghz->scene_manager, SubGhzSceneSaveSuccess);
                return true;
            } else {
                string_set(subghz->error_str, "No name file");
                scene_manager_next_scene(subghz->scene_manager, SubGhzSceneShowErrorSub);
                return true;
            }
        }
    }
    return false;
}

void subghz_scene_save_name_on_exit(void* context) {
    SubGhz* subghz = context;

    // Clear view
    text_input_clean(subghz->text_input);
    scene_manager_set_scene_state(
        subghz->scene_manager, SubGhzSceneReadRAW, SubghzCustomEventManagerNoSet);
}
