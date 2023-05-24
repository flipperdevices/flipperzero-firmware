#include "../namechanger.h"

static void
    namechanger_scene_revert_widget_callback(GuiButtonType result, InputType type, void* context) {
    NameChanger* namechanger = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(namechanger->view_dispatcher, result);
    }
}

void namechanger_scene_revert_on_enter(void* context) {
    NameChanger* namechanger = context;
    Widget* widget = namechanger->widget;
    widget_add_text_box_element(
        widget, 0, 0, 128, 25, AlignCenter, AlignCenter, "\e#Revert Name?\e#", false);
    widget_add_icon_element(widget, 48, 20, &I_MarioBlock);
    widget_add_button_element(
        widget, GuiButtonTypeLeft, "Cancel", namechanger_scene_revert_widget_callback, namechanger);
    widget_add_button_element(
        widget,
        GuiButtonTypeRight,
        "Revert",
        namechanger_scene_revert_widget_callback,
        namechanger);
    view_dispatcher_switch_to_view(namechanger->view_dispatcher, NameChangerViewWidget);
}

bool namechanger_scene_revert_on_event(void* context, SceneManagerEvent event) {
    NameChanger* namechanger = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    } else if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        if(event.event == GuiButtonTypeRight) {
            if(namechanger_name_write(namechanger, "eraseerase")) {
                scene_manager_next_scene(
                    namechanger->scene_manager, NameChangerSceneRevertSuccess);
            } else {
                scene_manager_search_and_switch_to_previous_scene(
                    namechanger->scene_manager, NameChangerSceneStart);
            }
        } else if(event.event == GuiButtonTypeLeft) {
            scene_manager_search_and_switch_to_previous_scene(
                namechanger->scene_manager, NameChangerSceneStart);
        }
    }
    return consumed;
}

void namechanger_scene_revert_on_exit(void* context) {
    NameChanger* namechanger = context;
    widget_reset(namechanger->widget);
}
