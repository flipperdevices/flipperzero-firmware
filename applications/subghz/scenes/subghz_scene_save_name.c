#include "../subghz_i.h"
#include <lib/toolbox/random_name.h>

#define SCENE_SAVE_NAME_CUSTOM_EVENT (0UL)

void subghz_scene_save_name_text_input_callback(void* context) {
    SubGhz* subghz = context;

    view_dispatcher_send_custom_event(subghz->view_dispatcher, SCENE_SAVE_NAME_CUSTOM_EVENT);
}

const void subghz_scene_save_name_on_enter(void* context) {
    SubGhz* subghz = context;

    // Setup view
    TextInput* text_input = subghz->text_input;
    bool dev_name_empty = false;
    //if(!strcmp(nfc->dev.dev_name, "")) {
        set_random_name(subghz->text_store, sizeof(subghz->text_store));
        dev_name_empty = true;
    //} else {
    //    nfc_text_store_set(nfc, nfc->dev.dev_name);
    //}
    text_input_set_header_text(text_input, "Name the card");
    text_input_set_result_callback(
        text_input,
        subghz_scene_save_name_text_input_callback,
        subghz,
        subghz->text_store,
        22, // максимальная длинна  имени NFC_DEV_NAME_MAX_LEN,
        dev_name_empty); 
    view_dispatcher_switch_to_view(subghz->view_dispatcher, SubGhzViewTextInput);
}

const bool subghz_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    SubGhz* subghz = context;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SCENE_SAVE_NAME_CUSTOM_EVENT) {
            // if(nfc->dev.dev_name) {
            //     nfc_device_delete(&nfc->dev);
            // }
            // memcpy(&nfc->dev.dev_name, nfc->text_store, strlen(nfc->text_store));
            // if(nfc_device_save(&nfc->dev, nfc->text_store)) {
            //     scene_manager_next_scene(nfc->scene_manager, NfcSceneSaveSuccess);
            //     return true;
            // } else {
            //     return scene_manager_search_and_switch_to_previous_scene(
            //         nfc->scene_manager, NfcSceneStart);
            // }
        }
    }
    return false;
}

const void subghz_scene_save_name_on_exit(void* context) {
    SubGhz* subghz = context;

    // Clear view
    text_input_set_header_text(subghz->text_input, NULL);
    text_input_set_result_callback(subghz->text_input, NULL, NULL, NULL, 0, false);
}
