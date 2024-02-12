#include "app_i.h"

#include <furi.h>
#include <toolbox/path.h>

#include "custom_event.h"

static void
    scene_confirm_button_callback(GuiButtonType button_type, InputType input_type, void* context) {
    furi_assert(context);
    App* app = context;

    if(input_type == InputTypeShort) {
        if(button_type == GuiButtonTypeLeft) {
            view_dispatcher_send_custom_event(app->view_dispatcher, CustomEventFileRejected);
        } else if(button_type == GuiButtonTypeRight) {
            view_dispatcher_send_custom_event(app->view_dispatcher, CustomEventFileConfirmed);
        }
    }
}

void scene_confirm_on_enter(void* context) {
    App* app = context;

    FuriString* file_name = furi_string_alloc();
    path_extract_filename(app->file_path, file_name, false);

    FuriString* label = furi_string_alloc_printf("Install %s?", furi_string_get_cstr(file_name));
    widget_add_string_element(
        app->widget, 64, 0, AlignCenter, AlignTop, FontPrimary, furi_string_get_cstr(label));

    furi_string_free(label);
    furi_string_free(file_name);

    widget_add_button_element(
        app->widget, GuiButtonTypeLeft, "Cancel", scene_confirm_button_callback, app);
    widget_add_button_element(
        app->widget, GuiButtonTypeRight, "Install", scene_confirm_button_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, ViewIdWidget);
}

bool scene_confirm_on_event(void* context, SceneManagerEvent event) {
    App* app = context;

    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == CustomEventFileConfirmed) {
            scene_manager_next_scene(app->scene_manager, SceneInstall);
        } else if(event.event == CustomEventFileRejected) {
            furi_string_reset(app->file_path);
            scene_manager_previous_scene(app->scene_manager);
        }
        consumed = true;
    } else if(event.type == SceneManagerEventTypeBack) {
        consumed = true;
    }

    return consumed;
}

void scene_confirm_on_exit(void* context) {
    App* app = context;
    widget_reset(app->widget);
}
