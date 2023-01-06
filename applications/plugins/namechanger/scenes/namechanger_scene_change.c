#include "../namechanger.h"

static void namechanger_scene_change_text_input_callback(void* context) {
    NameChanger* namechanger = context;

    view_dispatcher_send_custom_event(
        namechanger->view_dispatcher, NameChangerCustomEventTextEditResult);
}

void namechanger_scene_change_on_enter(void* context) {
    NameChanger* namechanger = context;
    TextInput* text_input = namechanger->text_input;

    namechanger_text_store_set(namechanger, "%s", furi_hal_version_get_name_ptr());

    text_input_set_header_text(text_input, "Set Flipper Name");

    text_input_set_result_callback(
        text_input,
        namechanger_scene_change_text_input_callback,
        namechanger,
        namechanger->text_store,
        NAMECHANGER_TEXT_STORE_SIZE,
        true);

    view_dispatcher_switch_to_view(namechanger->view_dispatcher, NameChangerViewTextInput);
}

bool namechanger_scene_change_on_event(void* context, SceneManagerEvent event) {
    NameChanger* namechanger = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == NameChangerCustomEventTextEditResult) {
            if(namechanger_name_write(namechanger, namechanger->text_store)) {
                scene_manager_next_scene(
                    namechanger->scene_manager, NameChangerSceneChangeSuccess);
            } else {
                scene_manager_search_and_switch_to_previous_scene(
                    namechanger->scene_manager, NameChangerSceneStart);
            }
        } else {
            scene_manager_search_and_switch_to_previous_scene(
                namechanger->scene_manager, NameChangerSceneStart);
        }
    }
    return consumed;
}

void namechanger_scene_change_on_exit(void* context) {
    NameChanger* namechanger = context;
    TextInput* text_input = namechanger->text_input;

    text_input_reset(text_input);
}
