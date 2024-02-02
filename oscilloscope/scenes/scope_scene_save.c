#include <float.h>
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_resources.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <gui/elements.h>
#include <notification/notification_messages.h>
#include <flipper_format/flipper_format.h>
#include "../scope_app_i.h"

void scope_scene_save_text_input_callback(void* context) {
    ScopeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, ScopeCustomEventTextInputDone);
}

void scope_scene_save_on_enter(void* context) {
    ScopeApp* app = context;
    // Setup view
    TextInput* text_input = app->text_input;
    FuriString* file_name = furi_string_alloc();
    FuriString* dir_name = furi_string_alloc();

    text_input_set_header_text(text_input, "Name signal");
    text_input_set_result_callback(
        text_input,
        scope_scene_save_text_input_callback,
        app,
        app->file_name_tmp,
        MAX_LEN_NAME,
        false);

    furi_string_free(file_name);
    furi_string_free(dir_name);

    view_dispatcher_switch_to_view(app->view_dispatcher, ScopeViewSave);
}

bool scope_scene_save_on_event(void* context, SceneManagerEvent event) {
    ScopeApp* app = context;
    UNUSED(app);
    UNUSED(event);

    bool consumed = false;
    if(event.type == SceneManagerEventTypeCustom && event.event == ScopeCustomEventTextInputDone) {
        if(strcmp(app->file_name_tmp, "") != 0) {
            FuriString* temp_str = furi_string_alloc();
            furi_string_printf(
                temp_str,
                "%s/%s%s",
                APP_DATA_PATH(""),
                app->file_name_tmp,
                FLIPPERSCOPE_APP_EXTENSION);
            Storage* storage = furi_record_open(RECORD_STORAGE);
            File* file = storage_file_alloc(storage);
            if(!storage_file_open(
                   file, furi_string_get_cstr(temp_str), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
                // Todo: Display error
            }
            if(!storage_file_write(
                   file, app->data, sizeof(uint16_t) * ADC_CONVERTED_DATA_BUFFER_SIZE)) {
                // Todo: Display error
            }
            storage_file_close(file);
            storage_file_free(file);
            furi_record_close(RECORD_STORAGE);
            free(app->data);
            app->data = NULL;
            scene_manager_previous_scene(app->scene_manager);
            consumed = true;
        }
    }

    return consumed;
}

void scope_scene_save_on_exit(void* context) {
    ScopeApp* app = context;
    // Clear views
    text_input_reset(app->text_input);
}
