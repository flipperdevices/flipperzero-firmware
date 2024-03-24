
#include "storage.h"
#include <storage/storage.h>
#include <flipper_format/flipper_format_i.h>
static Storage* flipper_hero_open_storage() {
    return furi_record_open(RECORD_STORAGE);
}

static void flipper_hero_close_storage() {
    furi_record_close(RECORD_STORAGE);
}
static void flipper_hero_close_config_file(FlipperFormat* file) {
    if(file == NULL) return;
    flipper_format_file_close(file);
    flipper_format_free(file);
}

void save_game_records(PluginState* plugin_state) {
    Storage* storage = flipper_hero_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    // Overwrite won't work, so delete first
    if(storage_file_exists(storage, GAME_RECORDS_FILE_PATH)) {
        storage_simply_remove(storage, GAME_RECORDS_FILE_PATH);
    }

    if(!flipper_format_file_open_new(fff_file, GAME_RECORDS_FILE_PATH)) {
        flipper_hero_close_storage();
        return;
    }

    // Store score and round
    flipper_format_write_int32(fff_file, "score", &plugin_state->record_score, 1);
    flipper_format_write_int32(fff_file, "round", &plugin_state->record_round, 1);

    flipper_hero_close_config_file(fff_file);
    flipper_hero_close_storage();
}

void load_game_records(PluginState* plugin_state) {
    Storage* storage = flipper_hero_open_storage();
    FlipperFormat* fff_file = flipper_format_file_alloc(storage);

    if(storage_common_stat(storage, GAME_RECORDS_FILE_PATH, NULL) != FSE_OK) {
        flipper_hero_close_config_file(fff_file);
        flipper_hero_close_storage();
        // Initialize default values if file does not exist
        plugin_state->record_score = 0;
        plugin_state->record_round = 1;
        return;
    }

    if(!flipper_format_file_open_existing(fff_file, GAME_RECORDS_FILE_PATH)) {
        flipper_hero_close_config_file(fff_file);
        flipper_hero_close_storage();
        return;
    }

    // Assuming the use of a uint32_t for score and round
    flipper_format_read_int32(fff_file, "score", &plugin_state->record_score, 1);
    flipper_format_read_int32(fff_file, "round", &plugin_state->record_round, 1);

    flipper_hero_close_config_file(fff_file);
    flipper_hero_close_storage();
}
