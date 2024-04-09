#include <furi.h>

#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <lib/toolbox/path.h>

#include "quac.h"
#include "scenes.h"
#include "scene_action_ir_list.h"
#include "../actions/action_ir_utils.h"

#include <flipper_format/flipper_format.h>

void scene_action_ir_list_callback(void* context, uint32_t index) {
    App* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, index);
}

void scene_action_ir_list_on_enter(void* context) {
    App* app = context;

    Submenu* menu = app->sub_menu;
    submenu_reset(menu);

    // Our selected IR File is app->temp_str
    submenu_set_header(menu, "Select IR Command");

    // read the IR file and load the names of all of the commands
    FuriString* name = furi_string_alloc();

    uint32_t index = 0;
    FlipperFormat* fff_data_file = flipper_format_file_alloc(app->storage);
    if(flipper_format_file_open_existing(fff_data_file, furi_string_get_cstr(app->temp_str))) {
        while(flipper_format_read_string(fff_data_file, "name", name)) {
            submenu_add_item(
                menu, furi_string_get_cstr(name), index, scene_action_ir_list_callback, app);
            index++;
        }
    }

    if(index == 0) {
        FURI_LOG_E(TAG, "Failed to get commands from %s", furi_string_get_cstr(app->temp_str));
    }

    flipper_format_file_close(fff_data_file);
    flipper_format_free(fff_data_file);
    furi_string_free(name);

    view_dispatcher_switch_to_view(app->view_dispatcher, QView_SubMenu);
}

bool scene_action_ir_list_on_event(void* context, SceneManagerEvent event) {
    App* app = context;
    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom) {
        consumed = true;
        uint32_t index = event.event;
        InfraredSignal* signal = infrared_utils_signal_alloc();

        // extract that item as it's own file and place it "here", as defined by
        // the currently selected_item
        FuriString* name = furi_string_alloc(); // IR command name
        FlipperFormat* fff_data_file = flipper_format_file_alloc(app->storage);
        FuriString* file_name = furi_string_alloc(); // new IR file name

        do {
            if(!flipper_format_file_open_existing(
                   fff_data_file, furi_string_get_cstr(app->temp_str))) {
                FURI_LOG_E(TAG, "Failed to open %s", furi_string_get_cstr(app->temp_str));
                break;
            }
            if(!infrared_utils_read_signal_at_index(fff_data_file, index, signal, name)) {
                FURI_LOG_E(TAG, "Failed to read signal at %lu", index);
                break;
            }
            FURI_LOG_I(TAG, "Read IR signal: %s", furi_string_get_cstr(name));
            flipper_format_file_close(fff_data_file);

            // generate the new path, based on current item's dir and new command name
            Item* item = ItemArray_get(app->items_view->items, app->selected_item);
            path_extract_dirname(furi_string_get_cstr(item->path), file_name);
            furi_string_cat_printf(file_name, "/%s.ir", furi_string_get_cstr(name));

            FURI_LOG_I(TAG, "Writing new IR file: %s", furi_string_get_cstr(file_name));
            if(!flipper_format_file_open_new(fff_data_file, furi_string_get_cstr(file_name))) {
                FURI_LOG_E(TAG, "Error creating new file: %s", furi_string_get_cstr(file_name));
                break;
            }
            if(!infrared_utils_write_signal(fff_data_file, signal, name)) {
                FURI_LOG_E(TAG, "Failed to write signal!");
                break;
            }

            // Import successful!
            // Leave the user on this scene, in case they want to import
            // more commands from this IR file
            notification_message(app->notifications, &sequence_success);

        } while(false);

        // cleanup
        flipper_format_file_close(fff_data_file);
        flipper_format_free(fff_data_file);
        furi_string_free(name);
        furi_string_free(file_name);
        infrared_utils_signal_free(signal);
    }
    return consumed;
}

void scene_action_ir_list_on_exit(void* context) {
    App* app = context;
    submenu_reset(app->sub_menu);
}