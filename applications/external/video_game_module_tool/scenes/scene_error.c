#include "app_i.h"

#include <furi.h>
#include <notification/notification_messages.h>

#include "custom_event.h"
#include "video_game_module_tool_icons.h"

static void
    scene_error_button_callback(GuiButtonType button_type, InputType input_type, void* context) {
    App* app = context;
    if(input_type == InputTypeShort && button_type == GuiButtonTypeLeft) {
        view_dispatcher_send_custom_event(app->view_dispatcher, CustomEventRetryRequested);
    }
}

void scene_error_on_enter(void* context) {
    App* app = context;

    widget_add_icon_element(app->widget, 83, 22, &I_WarningDolphinFlip_45x42);
    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Retry", scene_error_button_callback, app);
    widget_add_string_element(
        app->widget, 64, 0, AlignCenter, AlignTop, FontPrimary, "Installation Failed!");

    const char* error_msg;
    if(app->flasher_error == FlasherErrorBadFile) {
        error_msg = "This file is\ncorrupted or\nunsupported";
    } else if(app->flasher_error == FlasherErrorDisconnect) {
        error_msg = "The module was\ndisconnected\nduring the update";
    } else if(app->flasher_error == FlasherErrorUnknown) {
        error_msg = "An unknown error\nhas occurred";
    } else {
        furi_crash();
    }

    widget_add_string_multiline_element(
        app->widget, 0, 28, AlignLeft, AlignCenter, FontSecondary, error_msg);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewIdWidget);

    notification_message(app->notification, &sequence_error);
    notification_message(app->notification, &sequence_set_red_255);
}

bool scene_error_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == CustomEventRetryRequested) {
            scene_manager_search_and_switch_to_previous_scene(app->scene_manager, SceneProbe);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        furi_string_reset(app->file_path);
        scene_manager_search_and_switch_to_previous_scene(app->scene_manager, SceneProbe);
        consumed = true;
    }

    return consumed;
}

void scene_error_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
    notification_message(app->notification, &sequence_reset_red);
}
