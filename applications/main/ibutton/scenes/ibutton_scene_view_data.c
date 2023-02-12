#include "../ibutton_i.h"

void ibutton_scene_view_data_on_enter(void* context) {
    iButton* ibutton = context;
    iButtonKey* key = ibutton->key;
    TextBox* text_box = ibutton->text_box;

    FuriString* tmp = furi_string_alloc();

    ibutton_key_get_rendered_data(key, tmp);

    text_box_set_font(text_box, TextBoxFontHex);
    //TODO: make TextBox own the text
    text_box_set_text(text_box, furi_string_get_cstr(tmp));

    view_dispatcher_switch_to_view(ibutton->view_dispatcher, iButtonViewTextBox);
    scene_manager_set_scene_state(ibutton->scene_manager, iButtonSceneViewData, (uint32_t)tmp);
}

bool ibutton_scene_view_data_on_event(void* context, SceneManagerEvent event) {
    UNUSED(context);
    UNUSED(event);
    return false;
}

void ibutton_scene_view_data_on_exit(void* context) {
    iButton* ibutton = context;
    text_box_reset(ibutton->text_box);

    FuriString* tmp =
        (FuriString*)scene_manager_get_scene_state(ibutton->scene_manager, iButtonSceneViewData);
    furi_string_free(tmp);
}
