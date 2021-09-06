#include "archive_favorites.h"
#include "archive_files.h"
#include "../views/archive_main_view.h"

void archive_add_to_favorites(string_t file_path, string_t name) {
    furi_assert(file_path);
    furi_assert(name);

    string_t buffer_src;

    string_init_printf(buffer_src, "%s/%s\r\n", string_get_cstr(file_path), string_get_cstr(name));

    archive_file_append(ARCHIVE_FAV_PATH, buffer_src);

    string_clear(buffer_src);
}

bool archive_favorites_rename(string_t file_path, string_t selected, string_t dst) {
    furi_assert(file_path);
    furi_assert(selected);
    furi_assert(dst);

    FileWorker* file_worker = file_worker_alloc(false);

    string_t path;
    string_t buffer;
    string_t temp;

    string_init(buffer);
    string_init(temp);

    string_init_printf(path, "%s/%s", string_get_cstr(file_path), string_get_cstr(selected));
    bool load_result =
        file_worker_open(file_worker, ARCHIVE_FAV_PATH, FSAM_READ, FSOM_OPEN_EXISTING);

    if(load_result) {
        while(1) {
            if(!file_worker_read_until(file_worker, buffer, '\n')) {
                break;
            }
            if(!string_size(buffer)) {
                break;
            }

            string_printf(
                temp,
                "%s\r\n",
                string_search(buffer, path) ? string_get_cstr(buffer) : string_get_cstr(dst));
            archive_file_append(ARCHIVE_FAV_TEMP_PATH, temp);
            string_clean(temp);
        }
    }

    string_clear(temp);
    string_clear(buffer);
    string_clear(path);

    file_worker_close(file_worker);
    file_worker_remove(file_worker, ARCHIVE_FAV_PATH);
    file_worker_rename(file_worker, ARCHIVE_FAV_TEMP_PATH, ARCHIVE_FAV_PATH);

    file_worker_free(file_worker);

    return load_result;
}

bool archive_favorites_delete(string_t file_path, string_t selected) {
    furi_assert(file_path);
    furi_assert(selected);

    FileWorker* file_worker = file_worker_alloc(false);

    string_t path;
    string_t buffer;
    string_init(buffer);

    string_init_printf(path, "%s/%s", string_get_cstr(file_path), string_get_cstr(selected));

    bool load_result =
        file_worker_open(file_worker, ARCHIVE_FAV_PATH, FSAM_READ, FSOM_OPEN_EXISTING);
    if(load_result) {
        while(1) {
            if(!file_worker_read_until(file_worker, buffer, '\n')) {
                break;
            }
            if(!string_size(buffer)) {
                break;
            }

            if(string_search(buffer, path)) {
                string_t temp;
                string_init_printf(temp, "%s\r\n", string_get_cstr(buffer));
                archive_file_append(ARCHIVE_FAV_TEMP_PATH, temp);
                string_clear(temp);
            }
        }
    }

    string_clear(buffer);
    string_clear(path);

    file_worker_close(file_worker);
    file_worker_remove(file_worker, ARCHIVE_FAV_PATH);
    file_worker_rename(file_worker, ARCHIVE_FAV_TEMP_PATH, ARCHIVE_FAV_PATH);

    file_worker_free(file_worker);

    return load_result;
}

bool archive_is_favorite(string_t file_path, string_t selected) {
    furi_assert(file_path);
    furi_assert(selected);

    FileWorker* file_worker = file_worker_alloc(false);

    string_t path;
    string_t buffer;
    string_init(buffer);
    bool found = false;

    string_init_printf(path, "%s/%s", string_get_cstr(file_path), string_get_cstr(selected));

    bool load_result =
        file_worker_open(file_worker, ARCHIVE_FAV_PATH, FSAM_READ, FSOM_OPEN_ALWAYS);

    if(load_result) {
        while(1) {
            if(!file_worker_read_until(file_worker, buffer, '\n')) {
                break;
            }
            if(!string_size(buffer)) {
                break;
            }
            if(!string_search(buffer, path)) {
                found = true;
                break;
            }
        }
    }

    string_clear(buffer);
    string_clear(path);
    file_worker_close(file_worker);
    file_worker_free(file_worker);

    return found;
}

bool archive_favorites_read(void* context) {
    furi_assert(context);

    ArchiveMainView* archive_view = context;
    FileWorker* file_worker = file_worker_alloc(false);

    string_t buffer;
    FileInfo file_info;
    string_init(buffer);

    bool load_result =
        file_worker_open(file_worker, ARCHIVE_FAV_PATH, FSAM_READ, FSOM_OPEN_ALWAYS);

    if(load_result) {
        while(1) {
            if(!file_worker_read_until(file_worker, buffer, '\n')) {
                break;
            }
            if(!string_size(buffer)) {
                break;
            }

            archive_view_add_item(archive_view, &file_info, string_get_cstr(buffer));
            string_clean(buffer);
        }
    }
    string_clear(buffer);
    file_worker_close(file_worker);
    file_worker_free(file_worker);
    return load_result;
}
