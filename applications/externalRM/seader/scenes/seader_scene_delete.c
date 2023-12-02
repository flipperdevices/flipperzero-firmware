#include "../seader_i.h"

void seader_scene_delete_widget_callback(GuiButtonType result, InputType type, void* context) {
    Seader* seader = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(seader->view_dispatcher, result);
    }
}

void seader_scene_delete_on_enter(void* context) {
    Seader* seader = context;

    // Setup Custom Widget view
    char temp_str[64];
    snprintf(temp_str, sizeof(temp_str), "\e#Delete %s?\e#", seader->credential->name);
    widget_add_text_box_element(
        seader->widget, 0, 0, 128, 23, AlignCenter, AlignCenter, temp_str, false);
    widget_add_button_element(
        seader->widget, GuiButtonTypeLeft, "Back", seader_scene_delete_widget_callback, seader);
    widget_add_button_element(
        seader->widget, GuiButtonTypeRight, "Delete", seader_scene_delete_widget_callback, seader);

    view_dispatcher_switch_to_view(seader->view_dispatcher, SeaderViewWidget);
}

bool seader_scene_delete_on_event(void* context, SceneManagerEvent event) {
    Seader* seader = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            return scene_manager_previous_scene(seader->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            if(seader_credential_delete(seader->credential, true)) {
                scene_manager_next_scene(seader->scene_manager, SeaderSceneDeleteSuccess);
            } else {
                scene_manager_search_and_switch_to_previous_scene(
                    seader->scene_manager, SeaderSceneStart);
            }
            consumed = true;
        }
    }
    return consumed;
}

void seader_scene_delete_on_exit(void* context) {
    Seader* seader = context;

    widget_reset(seader->widget);
}
