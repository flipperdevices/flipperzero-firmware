#include "power_settings.h"

bool SAVE_POWER_SETTINGS(uint32_t* x) {
    return saved_struct_save(
        POWER_SETTINGS_PATH, x, sizeof(uint32_t), POWER_SETTINGS_MAGIC, POWER_SETTINGS_VER);
}

bool LOAD_POWER_SETTINGS(uint32_t* x) {
    return saved_struct_load(
        POWER_SETTINGS_PATH, x, sizeof(uint32_t), POWER_SETTINGS_MAGIC, POWER_SETTINGS_VER);
}