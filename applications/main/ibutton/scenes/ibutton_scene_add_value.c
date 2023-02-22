#include "../ibutton_i.h"

static void ibutton_scene_add_type_byte_input_callback(void* context) {
    iButton* ibutton = context;
    view_dispatcher_send_custom_event(ibutton->view_dispatcher, iButtonCustomEventByteEditResult);
}

static void ibutton_scene_add_type_byte_changed_callback(void* context) {
    iButton* ibutton = context;
    view_dispatcher_send_custom_event(ibutton->view_dispatcher, iButtonCustomEventByteEditChanged);
}

void ibutton_scene_add_value_on_enter(void* context) {
    iButton* ibutton = context;
    byte_input_set_header_text(ibutton->byte_input, "Enter the key");

    iButtonKeyEditableData editable_data;
    ibutton_key_get_editable_data(ibutton->key, &editable_data);

    byte_input_set_result_callback(
        ibutton->byte_input,
        ibutton_scene_add_type_byte_input_callback,
        ibutton_scene_add_type_byte_changed_callback,
        context,
        editable_data.ptr,
        editable_data.size);

    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewByteInput);
}

bool ibutton_scene_add_value_on_event(void* context, SceneManagerEvent event) {
    iButton* ibutton = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == iButtonCustomEventByteEditResult) {
            scene_manager_next_scene(ibutton->scene_manager, iButtonSceneSaveName);
        } else if(event.event == iButtonCustomEventByteEditChanged) {
            ibutton_key_apply_edits(ibutton->key);
        }
    }

    return consumed;
}

void ibutton_scene_add_value_on_exit(void* context) {
    iButton* ibutton = context;

    byte_input_set_result_callback(ibutton->byte_input, NULL, NULL, NULL, NULL, 0);
    byte_input_set_header_text(ibutton->byte_input, NULL);
}
