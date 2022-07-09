#include <furi.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "elf_lib/elf_loader.h"
#include "elf_cpp/elf_hashtable.h"

#define APP_PATH "/ext/plugin.elf"

int32_t elf_loader_app(void* p) {
    UNUSED(p);
    ELFResolver resolver = elf_resolve_from_hashtable;
    Storage* storage = furi_record_open("storage");
    DialogsApp* dialogs = furi_record_open("dialogs");

    string_t elf_name;
    string_init_set(elf_name, "/ext/");

    if(dialog_file_browser_show(dialogs, elf_name, elf_name, ".elf", true, NULL, false)) {
        FURI_LOG_I("elf_loader_app", "ELF Loader start");
        int ret = loader_exec_elf(string_get_cstr(elf_name), resolver, storage);
        FURI_LOG_I("elf_loader_app", "ELF Loader returned: %i", ret);
    }

    string_clear(elf_name);
    furi_record_close("dialogs");
    furi_record_close("storage");
    return 0;
}