#include "app_i.h"

#include <furi.h>
#include <notification/notification_messages.h>

#include "custom_event.h"
#include "video_game_module_tool_icons.h"

static void
    scene_success_button_callback(GuiButtonType button_type, InputType input_type, void* context) {
    App* app = context;
    if(input_type == InputTypeShort && button_type == GuiButtonTypeCenter) {
        view_dispatcher_send_custom_event(app->view_dispatcher, CustomEventSuccessDismissed);
    }
}

void scene_success_on_enter(void* context) {
    App* app = context;

    widget_add_icon_element(app->widget, 11, 24, &I_Module_60x26);
    widget_add_icon_element(app->widget, 77, 10, &I_Checkmark_44x40);
    widget_add_button_element(
        app->widget, GuiButtonTypeCenter, "OK", scene_success_button_callback, app);
    widget_add_string_multiline_element(
        app->widget, 64, 0, AlignCenter, AlignTop, FontPrimary, "Video Game Module\nUpdated");

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewIdWidget);

    notification_message(app->notification, &sequence_success);
    notification_message(app->notification, &sequence_set_green_255);
}

bool scene_success_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == CustomEventSuccessDismissed) {
            scene_manager_search_and_switch_to_previous_scene(app->scene_manager, SceneProbe);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void scene_success_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
    furi_string_reset(app->file_path);
    notification_message(app->notification, &sequence_reset_green);
}
