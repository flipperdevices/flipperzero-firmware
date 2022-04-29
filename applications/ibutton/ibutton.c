#include "ibutton.h"
#include "ibutton_i.h"
#include "ibutton/scenes/ibutton_scene.h"

#include <toolbox/path.h>
#include <flipper_format/flipper_format.h>

#define IBUTTON_APP_FOLDER "/any/ibutton"
#define IBUTTON_APP_EXTENSION ".ibtn"
#define IBUTTON_APP_FILE_TYPE "Flipper iButton key"

bool ibutton_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    iButton* ibutton = context;
    return scene_manager_handle_custom_event(ibutton->scene_manager, event);
}

bool ibutton_back_event_callback(void* context) {
    furi_assert(context);
    iButton* ibutton = context;
    return scene_manager_handle_back_event(ibutton->scene_manager);
}

void ibutton_tick_event_callback(void* context) {
    furi_assert(context);
    iButton* ibutton = context;
    scene_manager_handle_tick_event(ibutton->scene_manager);
}

iButton *ibutton_alloc() {
    iButton* ibutton = malloc(sizeof(iButton));

    ibutton->scene_manager = scene_manager_alloc(&ibutton_scene_handlers, ibutton);

    ibutton->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(ibutton->view_dispatcher);
    view_dispatcher_set_event_callback_context(ibutton->view_dispatcher, ibutton);
    view_dispatcher_set_custom_event_callback(ibutton->view_dispatcher, ibutton_custom_event_callback);
    view_dispatcher_set_navigation_event_callback(ibutton->view_dispatcher, ibutton_back_event_callback);
    view_dispatcher_set_tick_event_callback(ibutton->view_dispatcher, ibutton_tick_event_callback, 100);

    ibutton->gui = furi_record_open("gui");
    view_dispatcher_attach_to_gui(ibutton->view_dispatcher, ibutton->gui, ViewDispatcherTypeFullscreen);

    ibutton->storage = furi_record_open("storage");
    ibutton->dialogs = furi_record_open("dialogs");
    ibutton->notifications = furi_record_open("notification");

    ibutton->key = ibutton_key_alloc();
    ibutton->key_worker = ibutton_worker_alloc();
    ibutton_worker_start_thread(ibutton->key_worker);

    ibutton->submenu = submenu_alloc();
    view_dispatcher_add_view(ibutton->view_dispatcher, iButtonViewSubmenu, submenu_get_view(ibutton->submenu));

    ibutton->byte_input = byte_input_alloc();
    view_dispatcher_add_view(ibutton->view_dispatcher, iButtonViewByteInput, byte_input_get_view(ibutton->byte_input));

    return ibutton;
}

void ibutton_free(iButton* ibutton) {
    furi_assert(ibutton);

    view_dispatcher_remove_view(ibutton->view_dispatcher, iButtonViewByteInput);
    byte_input_free(ibutton->byte_input);

    view_dispatcher_remove_view(ibutton->view_dispatcher, iButtonViewSubmenu);
    submenu_free(ibutton->submenu);

    view_dispatcher_free(ibutton->view_dispatcher);
    scene_manager_free(ibutton->scene_manager);

    furi_record_close("storage");
    ibutton->storage = NULL;

    furi_record_close("notification");
    ibutton->notifications = NULL;

    furi_record_close("dialogs");
    ibutton->dialogs = NULL;

    furi_record_close("gui");
    ibutton->gui = NULL;

    ibutton_worker_stop_thread(ibutton->key_worker);
    ibutton_worker_free(ibutton->key_worker);
    ibutton_key_free(ibutton->key);

    free(ibutton);
}

bool ibutton_file_select(iButton* ibutton) {
    bool success = dialog_file_select_show(
        ibutton->dialogs, IBUTTON_APP_FOLDER, IBUTTON_APP_EXTENSION,
        ibutton->file_name, IBUTTON_FILE_NAME_SIZE, ibutton_key_get_name_p(ibutton->key));

    if(success) {
        string_t key_str;
        string_init_printf(key_str, "%s/%s%s", IBUTTON_APP_FOLDER, ibutton->file_name, IBUTTON_APP_EXTENSION);
        success = ibutton_load_key_data(ibutton, key_str);

        if(success) {
            ibutton_key_set_name(ibutton->key, ibutton->file_name);
        }

        string_clear(key_str);
    }

    return success;
}

bool ibutton_load_key_data(iButton* ibutton, string_t key_path) {
    FlipperFormat* file = flipper_format_file_alloc(ibutton->storage);
    bool result = false;
    string_t data;
    string_init(data);

    do {
        if(!flipper_format_file_open_existing(file, string_get_cstr(key_path))) break;

        // header
        uint32_t version;
        if(!flipper_format_read_header(file, data, &version)) break;
        if(string_cmp_str(data, IBUTTON_APP_FILE_TYPE) != 0) break;
        if(version != 1) break;

        // key type
        iButtonKeyType type;
        if(!flipper_format_read_string(file, "Key type", data)) break;
        if(!ibutton_key_get_type_by_string(string_get_cstr(data), &type)) break;

        // key data
        uint8_t key_data[IBUTTON_KEY_DATA_SIZE] = {0};
        if(!flipper_format_read_hex(file, "Data", key_data, ibutton_key_get_size_by_type(type)))
            break;

        ibutton_key_set_type(ibutton->key, type);
        ibutton_key_set_data(ibutton->key, key_data, IBUTTON_KEY_DATA_SIZE);

        result = true;
    } while(false);

    flipper_format_free(file);
    string_clear(data);

    if(!result) {
        dialog_message_show_storage_error(ibutton->dialogs, "Cannot load\nkey file");
    }

    return result;
}

int32_t ibutton_app(void *p) {
    iButton *ibutton = ibutton_alloc();

    scene_manager_next_scene(ibutton->scene_manager, iButtonSceneStart);
    view_dispatcher_run(ibutton->view_dispatcher);

    ibutton_free(ibutton);
    return 0;
}
