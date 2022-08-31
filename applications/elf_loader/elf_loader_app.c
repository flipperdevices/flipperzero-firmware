#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "elf_cpp/elf_hashtable.h"
#include <flipper_application/flipper_application.h>

#define TAG "elf_loader_app"

typedef struct {
    FlipperApplication* app;
    Storage* storage;
    uint8_t icon_temp[32];
} ElfLoader;

static uint8_t* elf_loader_icon_callback(string_t path, void* context) {
    ElfLoader* loader = context;
    furi_assert(loader);

    loader->app = flipper_application_alloc(loader->storage, &hashtable_api_interface);

    FlipperApplicationPreloadStatus preload_res =
        flipper_application_preload(loader->app, string_get_cstr(path));

    uint8_t* ret = NULL;

    if(preload_res == FlipperApplicationPreloadStatusSuccess) {
        const FlipperApplicationManifest* manifest = flipper_application_get_manifest(loader->app);
        if(manifest->has_icon) {
            memcpy(loader->icon_temp, manifest->icon, FAP_MANIFEST_MAX_ICON_SIZE);
            ret = loader->icon_temp;
        }
    } else {
        FURI_LOG_E(TAG, "ELF Loader failed to preload %s", string_get_cstr(path));
    }

    flipper_application_free(loader->app);
    return ret;
}

int32_t elf_loader_app(void* p) {
    ElfLoader* loader = malloc(sizeof(ElfLoader));
    loader->storage = furi_record_open("storage");
    DialogsApp* dialogs = furi_record_open("dialogs");
    Gui* gui = furi_record_open("gui");

    string_t elf_name, error_message;

    ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
    Loading* loading = loading_alloc();

    view_dispatcher_enable_queue(view_dispatcher);
    view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(view_dispatcher, 0, loading_get_view(loading));

    string_init_set(error_message, "unknown error");

    bool file_selected = false;
    bool show_error = true;
    do {
        if(p) {
            string_init_set(elf_name, (const char*)p);
        } else {
            string_init_set(elf_name, EXT_PATH("apps"));
            if(!dialog_file_browser_show(
                   dialogs,
                   elf_name,
                   elf_name,
                   ".fap",
                   true,
                   NULL,
                   false,
                   elf_loader_icon_callback,
                   loader)) {
                show_error = false;
                break;
            }
        }
        file_selected = true;
        loader->app = flipper_application_alloc(loader->storage, &hashtable_api_interface);

        view_dispatcher_switch_to_view(view_dispatcher, 0);

        FURI_LOG_I(TAG, "ELF Loader is loading %s", string_get_cstr(elf_name));

        FlipperApplicationPreloadStatus preload_res =
            flipper_application_preload(loader->app, string_get_cstr(elf_name));
        if(preload_res != FlipperApplicationPreloadStatusSuccess) {
            const char* err_msg = flipper_application_preload_status_to_string(preload_res);
            string_printf(error_message, "Preload failed: %s", err_msg);
            FURI_LOG_E(
                TAG, "ELF Loader failed to preload %s: %s", string_get_cstr(elf_name), err_msg);
            break;
        }

        FURI_LOG_I(TAG, "ELF Loader is mapping");
        FlipperApplicationLoadStatus load_status = flipper_application_map_to_memory(loader->app);
        if(load_status != FlipperApplicationLoadStatusSuccess) {
            const char* err_msg = flipper_application_load_status_to_string(load_status);
            string_printf(error_message, "Load failed: %s", err_msg);
            FURI_LOG_E(
                TAG,
                "ELF Loader failed to map to memory %s: %s",
                string_get_cstr(elf_name),
                err_msg);
            break;
        }

        FURI_LOG_I(TAG, "ELF Loader is staring app");

        FuriThread* thread = flipper_application_spawn(loader->app, NULL);
        furi_thread_start(thread);
        furi_thread_join(thread);

        show_error = false;
        int ret = furi_thread_get_return_code(thread);

        FURI_LOG_I(TAG, "ELF app returned: %i", ret);
    } while(0);

    if(show_error) {
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

    if(file_selected) {
        flipper_application_free(loader->app);
    }
    view_dispatcher_remove_view(view_dispatcher, 0);
    loading_free(loading);
    view_dispatcher_free(view_dispatcher);

    string_clear(elf_name);
    furi_record_close("gui");
    furi_record_close("dialogs");
    furi_record_close("storage");
    free(loader);
    return 0;
}