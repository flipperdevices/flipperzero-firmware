#include "minesweeper_files.h"

#include <furi.h>

bool mine_sweeper_storage_file_write(const char* format, ...) {
    furi_assert(format);

    FuriString* write_data;

    va_list args;
    va_start(args, format);

    write_data = furi_string_alloc_vprintf(format, args);

    va_end(args);

    // Open storage
    Storage* storage = furi_record_open(RECORD_STORAGE);

    // Allocate file
    File* file = storage_file_alloc(storage);

    /**
     *  Get the path to the current application data folder
     *  within /ext/apps_data/
     *
     *  Always truncate and overwrite with new data
     */

    if(!storage_file_open(file, APP_DATA_PATH("minesweeper_game_data.txt"), FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        
        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);

        return false;
    }

    if (!storage_file_write(file, furi_string_get_cstr(write_data), furi_string_size(write_data))) {

        storage_file_close(file);
        storage_file_free(file);
        furi_record_close(RECORD_STORAGE);

        return false;
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);

    return true;
}
