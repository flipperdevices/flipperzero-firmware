#include "passport_settings.h"

#include "passport_settings_filename.h"

#include <furi.h>
#include <lib/toolbox/saved_struct.h>
#include <storage/storage.h>

#define PASSPORT_SETTINGS_PATH EXT_PATH(PASSPORT_SETTINGS_FILE_NAME)
#define PASSPORT_SETTINGS_VERSION (4)
#define PASSPORT_SETTINGS_MAGIC (0x20)

bool passport_settings_load(PassportSettings* passport_settings) {
    furi_assert(passport_settings);

    return saved_struct_load(
        PASSPORT_SETTINGS_PATH,
        passport_settings,
        sizeof(PassportSettings),
        PASSPORT_SETTINGS_MAGIC,
        PASSPORT_SETTINGS_VERSION);
}

bool passport_settings_save(PassportSettings* passport_settings) {
    furi_assert(passport_settings);

    return saved_struct_save(
        PASSPORT_SETTINGS_PATH,
        passport_settings,
        sizeof(PassportSettings),
        PASSPORT_SETTINGS_MAGIC,
        PASSPORT_SETTINGS_VERSION);
}
