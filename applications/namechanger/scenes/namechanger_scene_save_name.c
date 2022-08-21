#include "../namechanger_i.h"
#include "m-string.h"
#include <toolbox/path.h>

static void namechanger_scene_save_name_text_input_callback(void* context) {
    NameChanger* namechanger = context;
    view_dispatcher_send_custom_event(namechanger->view_dispatcher, NameChangerCustomEventTextEditResult);
}

void namechanger_scene_save_name_on_enter(void* context) {
    NameChanger* namechanger = context;
    TextInput* text_input = namechanger->text_input;
	
	bool file_exists = namechanger_name_read_write(namechanger, NULL, 2);

    text_input_set_header_text(text_input, "Set Flipper Name");
    text_input_set_result_callback(
		text_input,
		namechanger_scene_save_name_text_input_callback,
		namechanger,
        namechanger->text_store,
		NAMECHANGER_TEXT_STORE_SIZE,
        file_exists);

    view_dispatcher_switch_to_view(namechanger->view_dispatcher, NameChangerViewTextInput);
}

bool namechanger_scene_save_name_on_event(void* context, SceneManagerEvent event) {
    NameChanger* namechanger = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == NameChangerCustomEventTextEditResult) {
            if(namechanger_name_read_write(namechanger, namechanger->text_store, 3)) {
                scene_manager_next_scene(namechanger->scene_manager, NameChangerSceneSaveSuccess);
            } else {
                scene_manager_search_and_switch_to_previous_scene(
                    namechanger->scene_manager, NameChangerSceneSaveFailed);
            }
        }
    }

    return consumed;
}

void namechanger_scene_save_name_on_exit(void* context) {
    NameChanger* namechanger = context;
    TextInput* text_input = namechanger->text_input;

    text_input_reset(text_input);
}
