#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "elf_cpp/elf_hashtable.h"
#include <flipper_application/flipper_application.h>

#define TAG "elf_loader_app"

int32_t elf_loader_app(void* p) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    DialogsApp* dialogs = furi_record_open(RECORD_DIALOGS);
    Gui* gui = furi_record_open(RECORD_GUI);

    string_t elf_name, error_message;

    FlipperApplication* app = flipper_application_alloc(storage, &hashtable_api_interface);

    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    Loading* loading = loading_alloc();

    view_dispatcher_enable_queue(view_dispatcher);
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(view_dispatcher, 0, loading_get_view(loading));

    string_init_set(error_message, "unknown error");

    bool load_success = false;
    do {
        if(p) {
            string_init_set(elf_name, (const char*)p);
        } else {
            string_init_set(elf_name, "/ext/apps");
            if(!dialog_file_browser_show(dialogs, elf_name, elf_name, ".fap", true, NULL, false)) {
                load_success = true;
                break;
            }
        }

        view_dispatcher_switch_to_view(view_dispatcher, 0);

        FURI_LOG_I(TAG, "ELF Loader is loading %s", string_get_cstr(elf_name));

        FlipperApplicationPreloadStatus preload_res =
            flipper_application_preload(app, string_get_cstr(elf_name));
        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            string_printf(error_message, "preload failed: %d", preload_res);
            FURI_LOG_E(
                TAG,
                "ELF Loader failed to preload %s (code %d)",
                string_get_cstr(elf_name),
                preload_res);
            break;
        }

        FURI_LOG_I(TAG, "ELF Loader is mapping");
        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            string_printf(error_message, "load failed: %d", load_status);
            FURI_LOG_E(
                TAG,
                "ELF Loader failed to map to memory %s (code %d)",
                string_get_cstr(elf_name),
                load_status);
            break;
        }

        FURI_LOG_I(TAG, "ELF Loader is staring app");

        FuriThread* thread = flipper_application_spawn(app, NULL);
        furi_thread_join(thread);

        load_success = true;
        int ret = furi_thread_get_return_code(thread);

        FURI_LOG_I(TAG, "ELF app returned: %i", ret);
    } while(0);

    if(!load_success) {
        DialogMessage* message = dialog_message_alloc();
        dialog_message_set_header(message, "ELF Loader", 64, 0, AlignCenter, AlignTop);
        dialog_message_set_buttons(message, NULL, NULL, NULL);

        string_t buffer;
        string_init(buffer);
        string_printf(buffer, "Error: %s\n", string_get_cstr(error_message));
        dialog_message_set_text(
            message, string_get_cstr(buffer), 64, 32, AlignCenter, AlignCenter);

        dialog_message_show(dialogs, message);
        dialog_message_free(message);
        string_clear(buffer);
    }

    string_clear(error_message);

    flipper_application_free(app);
    view_dispatcher_remove_view(view_dispatcher, 0);
    loading_free(loading);
    view_dispatcher_free(view_dispatcher);

    string_clear(elf_name);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_DIALOGS);
    furi_record_close(RECORD_STORAGE);
    return 0;
}