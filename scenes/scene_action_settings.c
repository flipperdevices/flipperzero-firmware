#include <furi.h>

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <lib/toolbox/path.h>

#include "quac.h"
#include "scenes.h"
#include "scene_action_settings.h"
#include "../actions/action.h"
#include "quac_icons.h"

// Define different settings per Action
typedef enum {
    ActionSettingsRename, // Rename file or folder
    ActionSettingsDelete, // Delete file or folder on SDcard
    ActionSettingsImport, // Copy a remote file into "current" folder
    ActionSettingsCreateGroup, // Create new empty folder in "current" folder
    ActionSettingsCreatePlaylist, // Turn this folder into a playlist
    ActionSettingsAddToPlaylist, // Append a remote file to this playlist
} ActionSettingsIndex;

// Delete the file of the currently selected item
// Update items_view list before returning so that UI is updated and correct
bool scene_action_settings_delete(App* app) {
    bool success = false;
    Item* item = ItemArray_get(app->items_view->items, app->selected_item);

    DialogMessage* dialog = dialog_message_alloc();
    dialog_message_set_header(dialog, "Delete?", 64, 0, AlignCenter, AlignTop);
    FuriString* text = furi_string_alloc();
    furi_string_printf(text, "%s\n\n%s", furi_string_get_cstr(item->name), "Are you sure?");
    dialog_message_set_text(dialog, furi_string_get_cstr(text), 64, 18, AlignCenter, AlignTop);
    dialog_message_set_buttons(dialog, "Cancel", NULL, "OK");
    DialogMessageButton button = dialog_message_show(app->dialog, dialog);

    if(button == DialogMessageButtonRight) {
        FuriString* current_path = furi_string_alloc();
        path_extract_dirname(furi_string_get_cstr(item->path), current_path);

        FS_Error fs_result = storage_common_remove(app->storage, furi_string_get_cstr(item->path));
        if(fs_result == FSE_OK) {
            success = true;
            FURI_LOG_I(TAG, "Deleted file: %s", furi_string_get_cstr(item->path));
            // ItemsView* new_items = item_get_items_view_from_path(app, current_path);
            // item_items_view_free(app->items_view);
            // app->items_view = new_items;
        } else {
            FURI_LOG_E(
                TAG, "Error deleting file! Error=%s", filesystem_api_error_get_desc(fs_result));
            FuriString* error_msg = furi_string_alloc();
            furi_string_printf(
                error_msg, "Delete failed!\nError: %s", filesystem_api_error_get_desc(fs_result));
            dialog_message_show_storage_error(app->dialog, furi_string_get_cstr(error_msg));
            furi_string_free(error_msg);
        }

        furi_string_free(current_path);
    } else {
        // FURI_LOG_I(TAG, "Used cancelled Delete");
    }

    furi_string_free(text);
    dialog_message_free(dialog);
    return success;
}

static bool scene_action_settings_import_file_browser_callback(
    FuriString* path,
    void* context,
    uint8_t** icon,
    FuriString* item_name) {
    UNUSED(context);
    UNUSED(item_name);
    char ext[MAX_EXT_LEN];
    path_extract_extension(path, ext, MAX_EXT_LEN);
    if(!strcmp(ext, ".sub")) {
        memcpy(*icon, icon_get_frame_data(&I_SubGHz_10px, 0), 32); // TODO: find the right size!
    } else if(!strcmp(ext, ".rfid")) {
        memcpy(*icon, icon_get_frame_data(&I_RFID_10px, 0), 32);
    } else if(!strcmp(ext, ".ir")) {
        memcpy(*icon, icon_get_frame_data(&I_IR_10px, 0), 32);
    } else if(!strcmp(ext, ".nfc")) {
        memcpy(*icon, icon_get_frame_data(&I_NFC_10px, 0), 32);
    } else if(!strcmp(ext, ".qpl")) {
        memcpy(*icon, icon_get_frame_data(&I_Playlist_10px, 0), 32);
    } else {
        return false;
    }
    return true;
}

// Ask user for file to import from elsewhere on the SD card
FuriString* scene_action_get_file_to_import_alloc(App* app) {
    // Setup our file browser options
    DialogsFileBrowserOptions fb_options;
    dialog_file_browser_set_basic_options(&fb_options, "", NULL);
    fb_options.base_path = STORAGE_EXT_PATH_PREFIX;
    fb_options.skip_assets = true;
    furi_string_set_str(app->temp_str, fb_options.base_path);
    fb_options.item_loader_callback = scene_action_settings_import_file_browser_callback;
    fb_options.item_loader_context = app;

    FuriString* full_path = NULL;
    if(dialog_file_browser_show(app->dialog, app->temp_str, app->temp_str, &fb_options)) {
        full_path = furi_string_alloc_set(app->temp_str);
    }
    return full_path;
}

// Import a file from elsewhere on the SD card
// Update items_view list before returning so that UI is updated and correct
bool scene_action_settings_import(App* app) {
    bool success = false;
    FuriString* current_path = furi_string_alloc();
    if(app->selected_item != EMPTY_ACTION_INDEX) {
        Item* item = ItemArray_get(app->items_view->items, app->selected_item);
        path_extract_dirname(furi_string_get_cstr(item->path), current_path);
    } else {
        furi_string_set(current_path, app->items_view->path);
    }

    // Setup our file browser options
    DialogsFileBrowserOptions fb_options;
    dialog_file_browser_set_basic_options(&fb_options, "", NULL);
    fb_options.base_path = furi_string_get_cstr(current_path);
    fb_options.skip_assets = true;
    furi_string_set_str(app->temp_str, fb_options.base_path);
    fb_options.item_loader_callback = scene_action_settings_import_file_browser_callback;
    fb_options.item_loader_context = app;

    if(dialog_file_browser_show(app->dialog, app->temp_str, app->temp_str, &fb_options)) {
        // FURI_LOG_I(TAG, "Selected file is %s", furi_string_get_cstr(app->temp_str));
        // TODO: this should be a method
        FuriString* file_name = furi_string_alloc();
        path_extract_filename(app->temp_str, file_name, false);
        // FURI_LOG_I(TAG, "Importing file %s", furi_string_get_cstr(file_name));
        FuriString* full_path;
        full_path = furi_string_alloc_printf(
            "%s/%s", furi_string_get_cstr(current_path), furi_string_get_cstr(file_name));
        // FURI_LOG_I(TAG, "New path is %s", furi_string_get_cstr(full_path));

        FS_Error fs_result = storage_common_copy(
            app->storage, furi_string_get_cstr(app->temp_str), furi_string_get_cstr(full_path));
        if(fs_result == FSE_OK) {
            success = true;
            // FURI_LOG_I(TAG, "File copied / updating items view list");
            // ItemsView* new_items = item_get_items_view_from_path(app, current_path);
            // item_items_view_free(app->items_view);
            // app->items_view = new_items;
        } else {
            FURI_LOG_E(TAG, "File copy failed! %s", filesystem_api_error_get_desc(fs_result));
            FuriString* error_msg = furi_string_alloc_printf(
                "File copy failed!\nError: %s", filesystem_api_error_get_desc(fs_result));
            dialog_message_show_storage_error(app->dialog, furi_string_get_cstr(error_msg));
            furi_string_free(error_msg);
        }
        furi_string_free(file_name);
        furi_string_free(full_path);
    } else {
        // FURI_LOG_I(TAG, "User cancelled");
    }

    furi_string_free(current_path);
    return success;
}

// Prompt user for the name of the new Group
// Update items_view list before returning so that UI is updated and correct
bool scene_action_settings_create_group(App* app) {
    UNUSED(app);
    return false;
}

void scene_action_settings_callback(void* context, uint32_t index) {
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void scene_action_settings_on_enter(void* context) {
    App* app = context;

    Submenu* menu = app->sub_menu;
    submenu_reset(menu);

    if(app->selected_item >= 0) {
        Item* item = ItemArray_get(app->items_view->items, app->selected_item);
        submenu_set_header(menu, furi_string_get_cstr(item->name));

        submenu_add_item(
            menu, "Rename", ActionSettingsRename, scene_action_settings_callback, app);
        submenu_add_item(
            menu, "Delete", ActionSettingsDelete, scene_action_settings_callback, app);
    } else {
        submenu_set_header(menu, furi_string_get_cstr(app->items_view->name));
    }

    submenu_add_item(
        menu, "Import Here", ActionSettingsImport, scene_action_settings_callback, app);
    submenu_add_item(
        menu, "Create Group", ActionSettingsCreateGroup, scene_action_settings_callback, app);

    view_dispatcher_switch_to_view(app->view_dispatcher, QView_SubMenu);
}

bool scene_action_settings_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
        case ActionSettingsRename:
            consumed = true;
            scene_manager_next_scene(app->scene_manager, QScene_ActionRename);
            break;
        case ActionSettingsDelete:
            consumed = true;
            if(scene_action_settings_delete(app)) {
                scene_manager_previous_scene(app->scene_manager);
            }
            break;
        case ActionSettingsImport:
            consumed = true;
            // get the filename to import
            FuriString* import_file = scene_action_get_file_to_import_alloc(app);
            if(import_file) {
                FURI_LOG_I(TAG, "Importing %s", furi_string_get_cstr(import_file));
                // if it's a .ir file, switch to a scene that lets user pick the command from the file
                // only if there's more than one command in the file. then copy that relevant chunk
                // to the local directory
                char ext[MAX_EXT_LEN] = {0};

                path_extract_extension(import_file, ext, MAX_EXT_LEN);
                if(!strcmp(ext, ".ir")) {
                    FURI_LOG_I(TAG, "Loading ir file %s", furi_string_get_cstr(app->temp_str));
                    // load scene that takes filename and lists all commands
                    // the scene should write the new file, eh?
                    scene_manager_next_scene(app->scene_manager, QScene_ActionIRList);
                } else {
                    // just copy the file here
                    FuriString* current_path = furi_string_alloc();
                    if(app->selected_item != EMPTY_ACTION_INDEX) {
                        Item* item = ItemArray_get(app->items_view->items, app->selected_item);
                        path_extract_dirname(furi_string_get_cstr(item->path), current_path);
                    } else {
                        furi_string_set(current_path, app->items_view->path);
                    }
                    // TODO: this should be a method
                    FuriString* file_name = furi_string_alloc();
                    path_extract_filename(import_file, file_name, false);
                    // FURI_LOG_I(TAG, "Importing file %s", furi_string_get_cstr(file_name));
                    FuriString* full_path;
                    full_path = furi_string_alloc_printf(
                        "%s/%s",
                        furi_string_get_cstr(current_path),
                        furi_string_get_cstr(file_name));
                    // FURI_LOG_I(TAG, "New path is %s", furi_string_get_cstr(full_path));

                    FURI_LOG_I(
                        TAG,
                        "Copy: %s to %s",
                        furi_string_get_cstr(import_file),
                        furi_string_get_cstr(full_path));
                    FS_Error fs_result = storage_common_copy(
                        app->storage,
                        furi_string_get_cstr(import_file),
                        furi_string_get_cstr(full_path));
                    if(fs_result != FSE_OK) {
                        FURI_LOG_E(
                            TAG, "Copy file failed! %s", filesystem_api_error_get_desc(fs_result));
                        FuriString* error_msg = furi_string_alloc_printf(
                            "Copy failed!\nError: %s", filesystem_api_error_get_desc(fs_result));
                        dialog_message_show_storage_error(
                            app->dialog, furi_string_get_cstr(error_msg));
                        furi_string_free(error_msg);
                    }
                    furi_string_free(file_name);
                    furi_string_free(full_path);
                }
                furi_string_free(import_file);
            } else {
                scene_manager_previous_scene(app->scene_manager);
            }

            // if(scene_action_settings_import(app)) {
            //     scene_manager_previous_scene(app->scene_manager);
            // }
            break;
        case ActionSettingsCreateGroup:
            consumed = true;
            scene_manager_next_scene(app->scene_manager, QScene_ActionCreateGroup);
            break;
        }
    }

    return consumed;
}

void scene_action_settings_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->sub_menu);

    // Rebuild our list on exit, to pick up any renames
    ItemsView* new_items = item_get_items_view_from_path(app, app->items_view->path);
    item_items_view_free(app->items_view);
    app->items_view = new_items;
}