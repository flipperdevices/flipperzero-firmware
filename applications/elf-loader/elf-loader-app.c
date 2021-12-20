#include <furi.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>
#include "elf-lib/elf-loader.h"
#include "elf-loader-sys-api.h"
#include "elf-cpp/elf-hashtable.h"

#define APP_PATH "/ext/plugin.elf"

int32_t elf_loader_app(void* p) {
    ELFResolver resolver = elf_resolve_from_hashtable;
    Storage* storage = furi_record_open("storage");
    DialogsApp* dialogs = furi_record_open("dialogs");
    const uint8_t app_name_size = 128;
    char* app_name = furi_alloc(app_name_size + 1);

    if(dialog_file_select_show(dialogs, "/ext", ".elf", app_name, app_name_size, NULL)) {
        FURI_LOG_I("elf_loader_app", "ELF Loader start");
        int ret = loader_exec_elf(APP_PATH, resolver, storage);
        FURI_LOG_I("elf_loader_app", "ELF Loader returned: %i", ret);
    }

    furi_record_close("dialogs");
    furi_record_close("storage");
    return 0;
}