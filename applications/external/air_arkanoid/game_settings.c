#include <storage/storage.h>
#include "game_settings.h"
#include <lib/toolbox/saved_struct.h>

#define SETTINGS_PATH APP_DATA_PATH("settings.bin")
#define SETTINGS_VERSION (0)
#define SETTINGS_MAGIC (0x69)

bool game_settings_load(Settings* settings) {
    furi_assert(settings);

    return saved_struct_load(
        SETTINGS_PATH, settings, sizeof(Settings), SETTINGS_MAGIC, SETTINGS_VERSION);
}

bool game_settings_save(Settings* settings) {
    furi_assert(settings);

    return saved_struct_save(
        SETTINGS_PATH, settings, sizeof(Settings), SETTINGS_MAGIC, SETTINGS_VERSION);
}