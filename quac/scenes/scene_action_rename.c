#include <furi.h>

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_input.h>

#include "quac.h"
#include "scenes.h"
#include "scene_action_rename.h"
#include "../actions/action.h"

#include <lib/toolbox/path.h>

enum {
    SceneActionRenameEvent,
};

void scene_action_rename_callback(void* context) {
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SceneActionRenameEvent);
}

void scene_action_rename_on_enter(void* context) {
    App* app = context;
    TextInput* text = app->text_input;

    Item* item = ItemArray_get(app->items_view->items, app->selected_item);

    text_input_set_header_text(text, "Enter new name:");

    FuriString* file_name = furi_string_alloc();
    path_extract_filename_no_ext(furi_string_get_cstr(item->path), file_name);
    strncpy(app->temp_cstr, furi_string_get_cstr(file_name), MAX_NAME_LEN);

    text_input_set_result_callback(
        text, scene_action_rename_callback, app, app->temp_cstr, MAX_NAME_LEN, false);

    furi_string_free(file_name);
    view_dispatcher_switch_to_view(app->view_dispatcher, QView_TextInput);
}

bool scene_action_rename_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == SceneActionRenameEvent) {
            // FURI_LOG_I(TAG, "Attempting rename to %s", app->temp_cstr);
            if(!strcmp(app->temp_cstr, "")) {
                return false;
            }
            Item* item = ItemArray_get(app->items_view->items, app->selected_item);
            const char* old_path = furi_string_get_cstr(item->path);

            FuriString* file_name = furi_string_alloc();
            path_extract_filename(item->path, file_name, true);
            // FURI_LOG_I(TAG, "Original name is %s", furi_string_get_cstr(file_name));
            if(!furi_string_cmp_str(file_name, app->temp_cstr)) {
                FURI_LOG_W(TAG, "Rename: File names are the same!");
                furi_string_free(file_name);
                return false;
            }

            // build the new name full path, with extension
            FuriString* dir_name = furi_string_alloc();
            path_extract_dirname(old_path, dir_name);
            FuriString* new_path = furi_string_alloc_printf(
                "%s/%s%s", furi_string_get_cstr(dir_name), app->temp_cstr, item->ext);

            FURI_LOG_I(TAG, "Rename: %s to %s", old_path, furi_string_get_cstr(new_path));
            FS_Error fs_result =
                storage_common_rename(app->storage, old_path, furi_string_get_cstr(new_path));
            if(fs_result == FSE_OK) {
                ItemsView* new_items = item_get_items_view_from_path(app, dir_name);
                item_items_view_free(app->items_view);
                app->items_view = new_items;
                // furi_string_swap(item->path, new_path);
                // furi_string_set_str(item->name, app->temp_cstr);
                // item_prettify_name(item->name);
            } else {
                FURI_LOG_E(
                    TAG, "Rename file failed! %s", filesystem_api_error_get_desc(fs_result));
                FuriString* error_msg = furi_string_alloc_printf(
                    "Rename failed!\nError: %s", filesystem_api_error_get_desc(fs_result));
                dialog_message_show_storage_error(app->dialog, furi_string_get_cstr(error_msg));
                furi_string_free(error_msg);
            }

            scene_manager_search_and_switch_to_previous_scene(app->scene_manager, QScene_Items);

            furi_string_free(dir_name);
            furi_string_free(file_name);
            furi_string_free(new_path);

            consumed = true;
        }
    }

    return consumed;
}

void scene_action_rename_on_exit(void* context) {
    App* app = context;
    text_input_reset(app->text_input);
}