#include <furi.h>

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_input.h>

#include "quac.h"
#include "scenes.h"
#include "scene_action_create_group.h"
#include "../actions/action.h"

#include <lib/toolbox/path.h>

enum {
    SceneActionCreateGroupEvent,
};

void scene_action_create_group_callback(void* context) {
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SceneActionCreateGroupEvent);
}

void scene_action_create_group_on_enter(void* context) {
    App* app = context;
    TextInput* text = app->text_input;

    text_input_set_header_text(text, "Enter new group name:");

    app->temp_cstr[0] = 0;
    text_input_set_result_callback(
        text, scene_action_create_group_callback, app, app->temp_cstr, MAX_NAME_LEN, false);

    // TextInputValidatorCallback
    // text_input_set_validator(text, validator_callback, context)

    view_dispatcher_switch_to_view(app->view_dispatcher, QView_TextInput);
}

bool scene_action_create_group_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SceneActionCreateGroupEvent) {
            // FURI_LOG_I(TAG, "Attempting to create group %s", app->temp_cstr);
            if(!strcmp(app->temp_cstr, "")) {
                return false;
            }

            FuriString* current_path = furi_string_alloc();
            if(app->selected_item != EMPTY_ACTION_INDEX) {
                Item* item = ItemArray_get(app->items_view->items, app->selected_item);
                path_extract_dirname(furi_string_get_cstr(item->path), current_path);
            } else {
                furi_string_set(current_path, app->items_view->path);
            }

            FuriString* new_group_path = furi_string_alloc();
            furi_string_printf(
                new_group_path, "%s/%s", furi_string_get_cstr(current_path), app->temp_cstr);
            // FURI_LOG_I(TAG, "Full new path: %s", furi_string_get_cstr(new_group_path));

            FS_Error fs_result =
                storage_common_mkdir(app->storage, furi_string_get_cstr(new_group_path));
            if(fs_result == FSE_OK) {
                ItemsView* new_items = item_get_items_view_from_path(app, current_path);
                item_items_view_free(app->items_view);
                app->items_view = new_items;
            } else {
                FURI_LOG_E(
                    TAG, "Create Group failed! %s", filesystem_api_error_get_desc(fs_result));
                FuriString* error_msg = furi_string_alloc_printf(
                    "Create Group failed!\nError: %s", filesystem_api_error_get_desc(fs_result));
                dialog_message_show_storage_error(app->dialog, furi_string_get_cstr(error_msg));
                furi_string_free(error_msg);
            }

            furi_string_free(current_path);
            furi_string_free(new_group_path);

            scene_manager_search_and_switch_to_previous_scene(app->scene_manager, QScene_Items);
            consumed = true;
        }
    }

    return consumed;
}

void scene_action_create_group_on_exit(void* context) {
    App* app = context;
    text_input_reset(app->text_input);
}