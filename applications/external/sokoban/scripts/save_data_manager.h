#include "app.h"
#include <storage/storage.h>

LevelsDatabase* levels_database_load();
void levels_database_free(LevelsDatabase* levelsMetadata);
void levels_database_load_player_progress(LevelsDatabase* database);
void levels_database_save_player_progress(LevelsDatabase* database);