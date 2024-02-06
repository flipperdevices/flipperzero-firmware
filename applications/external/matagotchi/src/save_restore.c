#include <toolbox/saved_struct.h>

#include "save_restore.h"
#include "constants.h"

bool save_state_to_file(struct PersistentGameState* to_persist) {
    bool result = saved_struct_save(
        GAME_STATE_STORAGE_STATE_PATH,
        to_persist,
        sizeof(struct PersistentGameState),
        GAME_STATE_HEADER_MAGIC,
        GAME_STATE_HEADER_VERSION);

    return result;
}

bool load_state_from_file(struct PersistentGameState* to_load) {
    bool result = saved_struct_load(
        GAME_STATE_STORAGE_STATE_PATH,
        to_load,
        sizeof(struct PersistentGameState),
        GAME_STATE_HEADER_MAGIC,
        GAME_STATE_HEADER_VERSION);

    return result;
}

bool save_settings_to_file(struct PersistentSettings* to_persist) {
    bool result = saved_struct_save(
        GAME_STATE_STORAGE_SETTINGS_PATH,
        to_persist,
        sizeof(struct PersistentSettings),
        GAME_STATE_HEADER_MAGIC,
        GAME_STATE_HEADER_VERSION);

    return result;
}

bool load_settings_from_file(struct PersistentSettings* to_load) {
    bool result = saved_struct_load(
        GAME_STATE_STORAGE_SETTINGS_PATH,
        to_load,
        sizeof(struct PersistentSettings),
        GAME_STATE_HEADER_MAGIC,
        GAME_STATE_HEADER_VERSION);

    return result;
}
