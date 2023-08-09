#include "../uhf_app_i.h"

void uhf_scene_delete_widget_callback(GuiButtonType result, InputType type, void* context) {
    UHFApp* uhf_app = context;
    if(type == InputTypeShort) {
        view_dispatcher_send_custom_event(uhf_app->view_dispatcher, result);
    }
}

void uhf_scene_delete_on_enter(void* context) {
    UHFApp* uhf_app = context;

    // Setup Custom Widget view
    char temp_str[64];
    snprintf(temp_str, sizeof(temp_str), "\e#Delete %s?\e#", uhf_app->uhf_device->dev_name);
    widget_add_text_box_element(
        uhf_app->widget, 0, 0, 128, 23, AlignCenter, AlignCenter, temp_str, false);
    widget_add_button_element(
        uhf_app->widget, GuiButtonTypeLeft, "Back", uhf_scene_delete_widget_callback, uhf_app);
    widget_add_button_element(
        uhf_app->widget, GuiButtonTypeRight, "Delete", uhf_scene_delete_widget_callback, uhf_app);

    view_dispatcher_switch_to_view(uhf_app->view_dispatcher, UHFViewWidget);
}

bool uhf_scene_delete_on_event(void* context, SceneManagerEvent event) {
    UHFApp* uhf_app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == GuiButtonTypeLeft) {
            return scene_manager_previous_scene(uhf_app->scene_manager);
        } else if(event.event == GuiButtonTypeRight) {
            if(uhf_device_delete(uhf_app->uhf_device, true)) {
                scene_manager_next_scene(uhf_app->scene_manager, UHFSceneDeleteSuccess);
            } else {
                scene_manager_search_and_switch_to_previous_scene(
                    uhf_app->scene_manager, UHFSceneStart);
            }
            consumed = true;
        }
    }
    return consumed;
}

void uhf_scene_delete_on_exit(void* context) {
    UHFApp* uhf_app = context;

    widget_reset(uhf_app->widget);
}
