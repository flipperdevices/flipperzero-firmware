#include "../mass_storage_app_i.h"

void mass_storage_scene_usb_locked_on_enter(void* context) {
    MassStorageApp* app = context;

    widget_add_icon_element(app->widget, 78, 0, &I_ActiveConnection_50x64);
    widget_add_string_multiline_element(
        app->widget, 3, 2, AlignLeft, AlignTop, FontPrimary, "Connection\nis active!");
    widget_add_string_multiline_element(
        app->widget,
        3,
        30,
        AlignLeft,
        AlignTop,
        FontSecondary,
        "Disconnect from\nPC or phone to\nuse this function.");

    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewWidget);
}

bool mass_storage_scene_usb_locked_on_event(void* context, SceneManagerEvent event) {
    MassStorageApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeBack) {
        consumed = scene_manager_search_and_switch_to_previous_scene(
            app->scene_manager, MassStorageSceneFileSelect);
        if(!consumed) {
            consumed = scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, MassStorageSceneStart);
        }
    }

    return consumed;
}

void mass_storage_scene_usb_locked_on_exit(void* context) {
    MassStorageApp* app = context;
    widget_reset(app->widget);
}
