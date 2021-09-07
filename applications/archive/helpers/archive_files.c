#include "archive_files.h"
#include "archive_favorites.h"
#include "../archive_i.h"

void archive_trim_file_ext(char* name) {
    size_t str_len = strlen(name);
    char* end = name + str_len;
    while(end > name && *end != '.' && *end != '\\' && *end != '/') {
        --end;
    }
    if((end > name && *end == '.') && (*(end - 1) != '\\' && *(end - 1) != '/')) {
        *end = '\0';
    }
}

bool archive_get_filenames(void* context, uint8_t tab_id, const char* path) {
    furi_assert(context);

    ArchiveMainView* main_view = context;
    archive_file_array_clean(main_view);

    if(tab_id != ArchiveTabFavorites) {
        archive_read_dir(main_view, path);
    } else {
        archive_favorites_read(main_view);
    }
    return true;
}

bool archive_read_dir(void* context, const char* path) {
    furi_assert(context);

    ArchiveMainView* main_view = context;
    FileInfo file_info;
    Storage* fs_api = furi_record_open("storage");
    File* directory = storage_file_alloc(fs_api);
    char name[MAX_NAME_LEN];

    if(!storage_dir_open(directory, path)) {
        storage_dir_close(directory);
        storage_file_free(directory);
        return false;
    }

    while(1) {
        if(!storage_dir_read(directory, &file_info, name, MAX_NAME_LEN)) {
            break;
        }

        uint16_t files_cnt = archive_file_array_size(main_view);

        if(files_cnt > MAX_FILES) {
            break;
        } else if(storage_file_get_error(directory) == FSE_OK) {
            archive_view_add_item(main_view, &file_info, name);
        } else {
            storage_dir_close(directory);
            storage_file_free(directory);
            return false;
        }
    }
    storage_dir_close(directory);
    storage_file_free(directory);

    furi_record_close("storage");

    return true;
}

void archive_file_append(const char* path, string_t string) {
    furi_assert(path);
    furi_assert(string);

    FileWorker* file_worker = file_worker_alloc(false);

    if(!file_worker_open(file_worker, path, FSAM_WRITE, FSOM_OPEN_APPEND)) {
        FURI_LOG_E("Archive", "Append open error");
    }

    if(!file_worker_write(file_worker, string_get_cstr(string), string_size(string))) {
        FURI_LOG_E("Archive", "Append write error");
    }

    file_worker_close(file_worker);
    file_worker_free(file_worker);
}

void archive_delete_file(void* context, string_t path, string_t name) {
    furi_assert(context);
    furi_assert(path);
    furi_assert(name);
    ArchiveMainView* main_view = context;
    FileWorker* file_worker = file_worker_alloc(false);

    string_t full_path;
    string_init(full_path);
    string_printf(full_path, "%s/%s", string_get_cstr(path), string_get_cstr(name));
    file_worker_remove(file_worker, string_get_cstr(full_path));
    file_worker_free(file_worker);
    string_clear(full_path);

    if(archive_is_favorite(string_get_cstr(path), string_get_cstr(name))) {
        archive_favorites_delete(string_get_cstr(path), string_get_cstr(name));
    }

    archive_file_array_remove_selected(main_view);
}
