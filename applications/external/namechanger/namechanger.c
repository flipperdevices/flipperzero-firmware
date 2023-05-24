#include "namechanger.h"
#include "scenes/namechanger_scene.h"

#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>

bool namechanger_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    NameChanger* namechanger = context;
    return scene_manager_handle_custom_event(namechanger->scene_manager, event);
}

bool namechanger_back_event_callback(void* context) {
    furi_assert(context);
    NameChanger* namechanger = context;
    return scene_manager_handle_back_event(namechanger->scene_manager);
}

NameChanger* namechanger_alloc() {
    NameChanger* namechanger = malloc(sizeof(NameChanger));

    namechanger->scene_manager = scene_manager_alloc(&namechanger_scene_handlers, namechanger);

    namechanger->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(namechanger->view_dispatcher);
    view_dispatcher_set_event_callback_context(namechanger->view_dispatcher, namechanger);
    view_dispatcher_set_custom_event_callback(
        namechanger->view_dispatcher, namechanger_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        namechanger->view_dispatcher, namechanger_back_event_callback);

    namechanger->gui = furi_record_open(RECORD_GUI);
    namechanger->storage = furi_record_open(RECORD_STORAGE);

    namechanger->submenu = submenu_alloc();
    view_dispatcher_add_view(
        namechanger->view_dispatcher,
        NameChangerViewSubmenu,
        submenu_get_view(namechanger->submenu));

    namechanger->text_input = text_input_alloc();
    view_dispatcher_add_view(
        namechanger->view_dispatcher,
        NameChangerViewTextInput,
        text_input_get_view(namechanger->text_input));

    namechanger->popup = popup_alloc();
    view_dispatcher_add_view(
        namechanger->view_dispatcher, NameChangerViewPopup, popup_get_view(namechanger->popup));

    namechanger->widget = widget_alloc();
    view_dispatcher_add_view(
        namechanger->view_dispatcher, NameChangerViewWidget, widget_get_view(namechanger->widget));

    return namechanger;
}

void namechanger_free(NameChanger* namechanger) {
    furi_assert(namechanger);

    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewWidget);
    widget_free(namechanger->widget);
    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewPopup);
    popup_free(namechanger->popup);

    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewTextInput);
    text_input_free(namechanger->text_input);

    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewSubmenu);
    submenu_free(namechanger->submenu);

    view_dispatcher_free(namechanger->view_dispatcher);
    scene_manager_free(namechanger->scene_manager);

    furi_record_close(RECORD_STORAGE);

    furi_record_close(RECORD_GUI);

    free(namechanger);
}

void namechanger_text_store_set(NameChanger* namechanger, const char* text, ...) {
    va_list args;
    va_start(args, text);

    vsnprintf(namechanger->text_store, NAMECHANGER_TEXT_STORE_SIZE, text, args);

    va_end(args);
}

int32_t namechanger_app(void* p) {
    UNUSED(p);
    NameChanger* namechanger = namechanger_alloc();

    view_dispatcher_attach_to_gui(
        namechanger->view_dispatcher, namechanger->gui, ViewDispatcherTypeFullscreen);
    scene_manager_next_scene(namechanger->scene_manager, NameChangerSceneStart);

    view_dispatcher_run(namechanger->view_dispatcher);

    namechanger_free(namechanger);
    return 0;
}

bool namechanger_name_write(NameChanger* namechanger, char* name) {
    FuriString* file_path = furi_string_alloc();
    furi_string_set(file_path, "/ext/dolphin/name.txt");

    bool result = false;

    //If name is not "eraseerase" (set by Revert) then write name to file
    //otherwise, remove name.txt

    if(strcmp(name, "eraseerase") != 0) {
        //save
        FlipperFormat* file = flipper_format_file_alloc(namechanger->storage);

        do {
            // Open file for write
            if(!flipper_format_file_open_always(file, furi_string_get_cstr(file_path))) {
                break;
            }

            // Write header
            if(!flipper_format_write_header_cstr(file, NAMECHANGER_HEADER, 1)) {
                break;
            }

            // Write comments
            if(!flipper_format_write_comment_cstr(
                   file, "Changing the value below will change your FlipperZero device name.")) {
                break;
            }

            if(!flipper_format_write_comment_cstr(
                   file,
                   "Note: This is limited to 8 characters using the following: a-z, A-Z, 0-9, and _")) {
                break;
            }

            if(!flipper_format_write_comment_cstr(
                   file, "It cannot contain any other characters.")) {
                break;
            }

            if(!flipper_format_write_string_cstr(file, "Name", name)) {
                break;
            }

            result = true;
        } while(false);

        flipper_format_free(file);

        if(!result) {
            FURI_LOG_E(TAG, "Cannot save name file.");
        }
    } else {
        result = storage_simply_remove(namechanger->storage, furi_string_get_cstr(file_path));
    }

    return result;
}