#include <toolbox/saved_struct.h>

#include "save_restore.h"
#include "constants.h"

bool save_to_file(struct PersistentGameState *to_persist) {
    bool result = saved_struct_save(
        GAME_STATE_STORAGE_PATH,
        to_persist,
        sizeof(struct PersistentGameState),
        GAME_STATE_HEADER_MAGIC,
        GAME_STATE_HEADER_VERSION);

    return result;
}

bool load_from_file(struct PersistentGameState *to_load) {
    bool result = saved_struct_load(
        GAME_STATE_STORAGE_PATH,
        to_load,
        sizeof(struct PersistentGameState),
        GAME_STATE_HEADER_MAGIC,
        GAME_STATE_HEADER_VERSION);

    return result;
}
