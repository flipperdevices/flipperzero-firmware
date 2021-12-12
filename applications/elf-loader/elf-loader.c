#include <furi.h>
#include <storage/storage.h>
#include "elf-lib/loader.h"
#include "elf-loader-sys-api.h"
#include "elf-addr-resolver.h"

#define APP_PATH "/ext/plugin.elf"

bool elf_resolve_printf_only(const char* name, Elf32_Addr* address) {
    if(strcmp(name, "printf") == 0) {
        *address = (Elf32_Addr)printf;
        return true;
    } else {
        return false;
    }
}

int32_t elf_loader_app(void* p) {
    ELFResolver resolver = elf_resolve_printf_only;
    Storage* storage = furi_record_open("storage");
    furi_log_set_level(5);

    FURI_LOG_I("elf_loader_app", "ELF Loader start");
    int ret = loader_exec_elf(APP_PATH, resolver, storage);
    FURI_LOG_I("elf_loader_app", "ELF Loader returned: %i", ret);

    furi_log_set_level(FURI_LOG_LEVEL);
    furi_record_close("storage");
    return 0;
}