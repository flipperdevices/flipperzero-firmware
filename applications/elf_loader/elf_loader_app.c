#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/loading.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "elf_lib/elf_loader.h"
#include "elf_cpp/elf_hashtable.h"

#define APP_PATH "/ext/plugin.elf"

int32_t elf_loader_app(void* p) {
    Storage* storage = furi_record_open("storage");
    DialogsApp* dialogs = furi_record_open("dialogs");
    string_t elf_name;

    do {
        if(p) {
            string_init_set(elf_name, (const char*)p);
        } else {
            string_init_set(elf_name, "/ext/elf");
            if(!dialog_file_browser_show(dialogs, elf_name, elf_name, ".elf", true, NULL, false)) {
                break;
            }
        }

        // alloc
        Gui* gui = furi_record_open("gui");
        ViewDispatcher* view_dispatcher = view_dispatcher_alloc();
        Loading* loading = loading_alloc();
        view_dispatcher = view_dispatcher_alloc();
        view_dispatcher_enable_queue(view_dispatcher);
        view_dispatcher_attach_to_gui(view_dispatcher, gui, ViewDispatcherTypeFullscreen);
        view_dispatcher_add_view(view_dispatcher, 0, loading_get_view(loading));
        view_dispatcher_switch_to_view(view_dispatcher, 0);

        FURI_LOG_I("elf_loader_app", "ELF Loader is loading %s", string_get_cstr(elf_name));
        int ret = loader_exec_elf(string_get_cstr(elf_name), elf_resolve_from_hashtable, storage);
        FURI_LOG_I("elf_loader_app", "ELF Loader returned: %i", ret);

        // free
        view_dispatcher_remove_view(view_dispatcher, 0);
        loading_free(loading);
        view_dispatcher_free(view_dispatcher);
        furi_record_close("gui");
    } while(0);

    string_clear(elf_name);
    furi_record_close("dialogs");
    furi_record_close("storage");
    return 0;
}