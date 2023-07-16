#include "../mass_storage_app_i.h"
#include <mass_storage_icons.h>

typedef enum {
    SubghzCustomEventErrorBack,
} MassStorageCustomEvent;

static void
    mass_storage_scene_error_event_callback(GuiButtonType result, InputType type, void* context) {
    furi_assert(context);
    MassStorageApp* app = context;

    if((result == GuiButtonTypeLeft) && (type == InputTypeShort)) {
        view_dispatcher_send_custom_event(app->view_dispatcher, SubghzCustomEventErrorBack);
    }
}

void mass_storage_scene_error_on_enter(void* context) {
    MassStorageApp* app = context;

    widget_add_icon_element(app->widget, 0, 0, &I_SDQuestion_35x43);

    widget_add_string_multiline_element(
        app->widget,
        81,
        4,
        AlignCenter,
        AlignTop,
        FontSecondary,
        "No SD card or\napp data found.\nThis app requires a\nmass_storage/\ndirectory.");

    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Back", mass_storage_scene_error_event_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, MassStorageAppViewError);
}

bool mass_storage_scene_error_on_event(void* context, SceneManagerEvent event) {
    MassStorageApp* app = context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SubghzCustomEventErrorBack) {
            view_dispatcher_stop(app->view_dispatcher);
            consumed = true;
        }
    }
    return consumed;
}

void mass_storage_scene_error_on_exit(void* context) {
    MassStorageApp* app = context;
    widget_reset(app->widget);
}
