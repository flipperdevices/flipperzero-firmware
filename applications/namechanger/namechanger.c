#include "namechanger.h"
#include "namechanger_i.h"
#include "namechanger/scenes/namechanger_scene.h"
#include "m-string.h"
#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>

bool namechanger_make_app_folder(NameChanger* namechanger) {
    bool created = false;

    string_t folderpath;
    string_init_set_str(folderpath, "/ext/dolphin");

    //Make dir if doesn't exist
    if(!storage_simply_mkdir(namechanger->storage, string_get_cstr(folderpath))) {
        dialog_message_show_storage_error(namechanger->dialogs, "Cannot create\napp folder.");
    } else {
        created = true;
    }

    string_clear(folderpath);
    return created;
}

bool namechanger_name_read_write(NameChanger* namechanger, char* name, uint8_t mode) {
    FlipperFormat* file = flipper_format_file_alloc(namechanger->storage);

    string_t file_path;
    string_init_set_str(file_path, "/ext/dolphin/name.txt");

    if(namechanger_make_app_folder(namechanger)) {
        if(mode == 2) {
            //read
            bool result = false;

            string_t data;
            string_init(data);

            do {
                if(!flipper_format_file_open_existing(file, string_get_cstr(file_path))) {
                    break;
                }

                // header
                uint32_t version;

                if(!flipper_format_read_header(file, data, &version)) {
                    break;
                }

                if(string_cmp_str(data, NAMECHANGER_HEADER) != 0) {
                    break;
                }

                if(version != 1) {
                    break;
                }

                // get Name
                if(!flipper_format_read_string(file, "Name", data)) {
                    break;
                }

                result = true;
            } while(false);

            flipper_format_free(file);

            if(!result) {
                FURI_LOG_I(TAG, "Cannot load file.");
            } else {
                string_strim(data);
                FURI_LOG_I(TAG, "data: %s", data);

                namechanger_text_store_set(namechanger, "%s", string_get_cstr(data));
            }

            string_clear(data);

            return result;
        } else if(mode == 3) {
            //save
            FlipperFormat* file = flipper_format_file_alloc(namechanger->storage);

            bool result = false;

            do {
                // Open file for write
                if(!flipper_format_file_open_always(file, string_get_cstr(file_path))) {
                    break;
                }

                // Write header
                if(!flipper_format_write_header_cstr(file, NAMECHANGER_HEADER, 1)) {
                    break;
                }

                // Write comments
                if(!flipper_format_write_comment_cstr(
                       file,
                       "Changing the value below will change your FlipperZero device name.")) {
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

                //Write name
                if(!flipper_format_write_string_cstr(file, "Name", name)) {
                    break;
                }

                result = true;
            } while(false);

            flipper_format_free(file);

            if(!result) {
                //dialog_message_show_storage_error(namechanger->dialogs, "Cannot save\nname file");
                FURI_LOG_I(TAG, "Cannot save name file.");
            } else {
                //set name
                furi_hal_version_set_custom_name(name);
            }

            return result;
        } else {
            FURI_LOG_I(TAG, "Something broke.");
            return false;
        }
    } else {
        dialog_message_show_storage_error(namechanger->dialogs, "Cannot create\napp folder.");
        return false;
    }
}

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

void namechanger_tick_event_callback(void* context) {
    furi_assert(context);
    NameChanger* namechanger = context;
    scene_manager_handle_tick_event(namechanger->scene_manager);
}

NameChanger* namechanger_alloc() {
    NameChanger* namechanger = malloc(sizeof(namechanger));

    namechanger->scene_manager = scene_manager_alloc(&namechanger_scene_handlers, namechanger);

    namechanger->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(namechanger->view_dispatcher);
    view_dispatcher_set_event_callback_context(namechanger->view_dispatcher, namechanger);
    view_dispatcher_set_custom_event_callback(
        namechanger->view_dispatcher, namechanger_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(
        namechanger->view_dispatcher, namechanger_back_event_callback);
    view_dispatcher_set_tick_event_callback(
        namechanger->view_dispatcher, namechanger_tick_event_callback, 500);

    namechanger->gui = furi_record_open(RECORD_GUI);
    namechanger->storage = furi_record_open(RECORD_STORAGE);
    namechanger->dialogs = furi_record_open(RECORD_DIALOGS);

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

    namechanger->dialog_ex = dialog_ex_alloc();
    view_dispatcher_add_view(
        namechanger->view_dispatcher,
        NameChangerViewDialogEx,
        dialog_ex_get_view(namechanger->dialog_ex));

    return namechanger;
}

void namechanger_free(NameChanger* namechanger) {
    furi_assert(namechanger);

    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewDialogEx);
    dialog_ex_free(namechanger->dialog_ex);

    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewWidget);
    widget_free(namechanger->widget);

    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewPopup);
    popup_free(namechanger->popup);

    view_dispatcher_remove_view(namechanger->view_dispatcher, NameChangerViewTextInput);
    text_input_free(namechanger->text_input);

    view_dispatcher_free(namechanger->view_dispatcher);
    scene_manager_free(namechanger->scene_manager);

    furi_record_close(RECORD_STORAGE);
    namechanger->storage = NULL;

    furi_record_close(RECORD_DIALOGS);
    namechanger->dialogs = NULL;

    furi_record_close(RECORD_GUI);
    namechanger->gui = NULL;

    free(namechanger);
}

bool namechanger_delete_file(NameChanger* namechanger, char* file_path) {
    bool result = false;
    result = storage_simply_remove(namechanger->storage, file_path);

    return result;
}

void namechanger_text_store_set(NameChanger* namechanger, const char* text, ...) {
    va_list args;
    va_start(args, text);

    vsnprintf(namechanger->text_store, NAMECHANGER_TEXT_STORE_SIZE, text, args);

    va_end(args);
}

void namechanger_text_store_clear(NameChanger* namechanger) {
    memset(namechanger->text_store, 0, NAMECHANGER_TEXT_STORE_SIZE);
}

int32_t namechanger_app(void* p) {
    UNUSED(p);
    FURI_LOG_I(TAG, "app1");
    NameChanger* namechanger = namechanger_alloc();

    FURI_LOG_I(TAG, "app2");
    view_dispatcher_attach_to_gui(
        namechanger->view_dispatcher, namechanger->gui, ViewDispatcherTypeFullscreen);

    scene_manager_next_scene(namechanger->scene_manager, NameChangerSceneSaveName);

    view_dispatcher_run(namechanger->view_dispatcher);

    namechanger_free(namechanger);
    return 0;
}

void namechanger_on_system_start() {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* file = flipper_format_file_alloc(storage);

    string_t NAMEHEADER;
    string_init_set_str(NAMEHEADER, "Flipper Name File");

    string_t folderpath;
    string_init_set_str(folderpath, "/ext/dolphin");

    string_t filepath;
    string_init_set_str(filepath, "/ext/dolphin/name.txt");

    //Make dir if doesn't exist
    if(storage_simply_mkdir(storage, string_get_cstr(folderpath))) {
        bool result = false;

        string_t data;
        string_init(data);

        do {
            if(!flipper_format_file_open_existing(file, string_get_cstr(filepath))) {
                break;
            }

            // header
            uint32_t version;

            if(!flipper_format_read_header(file, data, &version)) {
                break;
            }

            if(string_cmp_str(data, string_get_cstr(NAMEHEADER)) != 0) {
                break;
            }

            if(version != 1) {
                break;
            }

            // get Name
            if(!flipper_format_read_string(file, "Name", data)) {
                break;
            }

            result = true;
        } while(false);

        flipper_format_free(file);

        if(!result) {
            //file not good - write new one
            FlipperFormat* file = flipper_format_file_alloc(storage);

            bool res = false;

            string_t name;
            string_init_set_str(name, furi_hal_version_get_name_ptr());

            do {
                // Open file for write
                if(!flipper_format_file_open_always(file, string_get_cstr(filepath))) {
                    break;
                }

                // Write header
                if(!flipper_format_write_header_cstr(file, string_get_cstr(NAMEHEADER), 1)) {
                    break;
                }

                // Write comments
                if(!flipper_format_write_comment_cstr(
                       file,
                       "Changing the value below will change your FlipperZero device name.")) {
                    break;
                }

                if(!flipper_format_write_comment_cstr(
                       file,
                       "Note: This is limited to 8 characters using the following: a-z, A-Z, 0-9, and _")) {
                    break;
                }

                if(!flipper_format_write_comment_cstr(
                       file, "It can contain other characters but use at your own risk.")) {
                    break;
                }

                //Write name
                if(!flipper_format_write_string_cstr(file, "Name", string_get_cstr(name))) {
                    break;
                }

                res = true;
            } while(false);

            flipper_format_free(file);

            if(!res) {
                FURI_LOG_E(TAG, "Save failed.");
            }

            string_clear(name);
        } else {
            string_strim(data);
            FURI_LOG_I(TAG, "data: %s", data);

            if(!string_size(data)) {
                //Empty file - get default name and write to file.
                FlipperFormat* file = flipper_format_file_alloc(storage);

                bool res = false;

                string_t name;
                string_init_set_str(name, furi_hal_version_get_name_ptr());

                do {
                    // Open file for write
                    if(!flipper_format_file_open_always(file, string_get_cstr(filepath))) {
                        break;
                    }

                    // Write header
                    if(!flipper_format_write_header_cstr(file, string_get_cstr(NAMEHEADER), 1)) {
                        break;
                    }

                    // Write comments
                    if(!flipper_format_write_comment_cstr(
                           file,
                           "Changing the value below will change your FlipperZero device name.")) {
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

                    //Write name
                    if(!flipper_format_write_string_cstr(file, "Name", string_get_cstr(name))) {
                        break;
                    }

                    res = true;
                } while(false);

                flipper_format_free(file);

                if(!res) {
                    FURI_LOG_E(TAG, "Save failed.");
                }

                string_clear(name);
            } else {
                //set name from file
                furi_hal_version_set_custom_name(string_get_cstr(data));
            }
        }

        string_clear(data);
    }

    string_clear(filepath);
    string_clear(folderpath);
    furi_record_close(RECORD_STORAGE);
}