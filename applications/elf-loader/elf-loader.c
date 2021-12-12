#include <furi.h>
#include <storage/storage.h>
#include "elf-lib/loader.h"
#include "elf-loader-sys-api.h"

#define APP_PATH "/ext/plugin.elf"
#define APP_NAME "plugin.elf"
#define APP_STACK_SIZE 2048

static const sysent_t sysentries = {printf};
static const ELFSymbol_t exports[] = {{"syscalls", (void*)&sysentries}};
static const ELFEnv_t env = {exports, sizeof(exports) / sizeof(*exports)};

int32_t elf_loader_app(void* p) {
    Storage* storage = furi_record_open("storage");
    furi_log_set_level(5);

    FURI_LOG_I("elf_loader_app", "Loader start");
    int ret = loader_exec_elf(APP_PATH, &env, storage);
    FURI_LOG_I("elf_loader_app", "Loader returned: %i", ret);

    furi_log_set_level(FURI_LOG_LEVEL);
    furi_record_close("storage");
    return 0;
}