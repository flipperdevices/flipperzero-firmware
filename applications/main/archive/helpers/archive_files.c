#include "archive_files.h"
#include "archive_apps.h"
#include "archive_browser.h"
#include <applications/external/subghz_playlist/playlist_file.h>
#include <applications/external/subghz_remote/subghz_remote_app_i.h>
#include <applications/external/ir_remote/infrared_remote.h>

#define TAG "Archive"

#define ASSETS_DIR "assets"

void archive_set_file_type(ArchiveFile_t* file, const char* path, bool is_folder, bool is_app) {
    furi_assert(file);

    file->is_app = is_app;
    if(is_app) {
        file->type = archive_get_app_filetype(archive_get_app_type(path));
    } else {
        for(size_t i = 0; i < COUNT_OF(known_ext); i++) {
            if((known_ext[i][0] == '?') || (known_ext[i][0] == '*')) continue;
            if(furi_string_end_with(file->path, known_ext[i])) {
                // Check for .txt containing folder
                if(strcmp(known_ext[i], ".txt") == 0) {
                    const char* txt_path = NULL;
                    switch(i) {
                    case ArchiveFileTypeSubghzPlaylist:
                        txt_path = PLAYLIST_FOLDER;
                        break;
                    case ArchiveFileTypeSubghzRemote:
                        txt_path = SUBREM_APP_FOLDER;
                        break;
                    case ArchiveFileTypeInfraredRemote:
                        txt_path = IR_REMOTE_PATH;
                        break;
                    case ArchiveFileTypeBadKb:
                        txt_path = archive_get_default_path(ArchiveTabBadKb);
                        break;
                    }
                    if(txt_path != NULL) {
                        size_t len = strlen(txt_path);
                        if(furi_string_size(file->path) < len) continue;
                        // Compare but ignore /ext or /any, continue if different (memcmp() != 0)
                        if(memcmp(furi_string_get_cstr(file->path) + 4, txt_path + 4, len - 4)) {
                            continue;
                        }
                        file->type = i;
                        return;
                    }
                } else {
                    file->type = i;
                    return;
                }
            }
        }

        if(is_folder) {
            file->type = ArchiveFileTypeFolder;
        } else {
            file->type = ArchiveFileTypeUnknown;
        }
    }
}

bool archive_get_items(void* context, const char* path) {
    furi_assert(context);

    bool res = false;
    ArchiveBrowserView* browser = context;

    if(archive_get_tab(browser) == ArchiveTabFavorites) {
        res = archive_favorites_read(browser);
    } else if(strncmp(path, "/app:", 5) == 0) {
        res = archive_app_read_dir(browser, path);
    }
    return res;
}

void archive_file_append(const char* path, const char* format, ...) {
    furi_assert(path);

    FuriString* string;
    va_list args;
    va_start(args, format);
    string = furi_string_alloc_vprintf(format, args);
    va_end(args);

    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    File* file = storage_file_alloc(fs_api);

    bool res = storage_file_open(file, path, FSAM_WRITE, FSOM_OPEN_APPEND);

    if(res) {
        storage_file_write(file, furi_string_get_cstr(string), furi_string_size(string));
    }

    storage_file_close(file);
    storage_file_free(file);
    furi_record_close(RECORD_STORAGE);
}

void archive_delete_file(void* context, const char* format, ...) {
    furi_assert(context);

    FuriString* filename;
    va_list args;
    va_start(args, format);
    filename = furi_string_alloc_vprintf(format, args);
    va_end(args);

    ArchiveBrowserView* browser = context;
    Storage* fs_api = furi_record_open(RECORD_STORAGE);

    FileInfo fileinfo;
    storage_common_stat(fs_api, furi_string_get_cstr(filename), &fileinfo);

    bool res = false;

    if(file_info_is_dir(&fileinfo)) {
        res = storage_simply_remove_recursive(fs_api, furi_string_get_cstr(filename));
    } else {
        res = (storage_common_remove(fs_api, furi_string_get_cstr(filename)) == FSE_OK);
    }

    furi_record_close(RECORD_STORAGE);

    if(archive_is_favorite("%s", furi_string_get_cstr(filename))) {
        archive_favorites_delete("%s", furi_string_get_cstr(filename));
    }

    if(res) {
        archive_file_array_rm_selected(browser);
    }

    furi_string_free(filename);
}

FS_Error archive_copy_rename_file_or_dir(
    void* context,
    const char* src_path,
    FuriString* dst_path,
    bool copy,
    bool find_name) {
    furi_assert(context);
    FURI_LOG_I(
        TAG, "%s from %s to %s", copy ? "Copy" : "Move", src_path, furi_string_get_cstr(dst_path));

    Storage* fs_api = furi_record_open(RECORD_STORAGE);

    FileInfo fileinfo;
    storage_common_stat(fs_api, src_path, &fileinfo);

    FS_Error error = FSE_OK;

    if(!path_contains_only_ascii(furi_string_get_cstr(dst_path))) {
        error = FSE_INVALID_NAME;
    } else if(!copy && !strcmp(src_path, furi_string_get_cstr(dst_path))) {
        error = FSE_EXIST;
    } else {
        if(find_name && storage_common_exists(fs_api, furi_string_get_cstr(dst_path))) {
            FuriString* dir_path = furi_string_alloc();
            FuriString* filename = furi_string_alloc();
            FuriString* file_ext = furi_string_alloc();

            path_extract_dirname(furi_string_get_cstr(dst_path), dir_path);
            path_extract_filename(dst_path, filename, true);
            path_extract_ext_str(dst_path, file_ext);

            storage_get_next_filename(
                fs_api,
                furi_string_get_cstr(dir_path),
                furi_string_get_cstr(filename),
                furi_string_get_cstr(file_ext),
                dst_path,
                255);
            furi_string_cat_printf(
                dir_path, "/%s%s", furi_string_get_cstr(dst_path), furi_string_get_cstr(file_ext));
            furi_string_set(dst_path, dir_path);

            furi_string_free(dir_path);
            furi_string_free(filename);
            furi_string_free(file_ext);
        }

        if(copy) {
            error = storage_common_copy(fs_api, src_path, furi_string_get_cstr(dst_path));
        } else {
            error = storage_common_rename(fs_api, src_path, furi_string_get_cstr(dst_path));
        }
    }
    furi_record_close(RECORD_STORAGE);

    if(!copy && archive_is_favorite("%s", src_path)) {
        archive_favorites_rename(src_path, furi_string_get_cstr(dst_path));
    }

    if(error == FSE_OK) {
        FURI_LOG_I(
            TAG,
            "%s from %s to %s is DONE",
            copy ? "Copy" : "Move",
            src_path,
            furi_string_get_cstr(dst_path));
    } else {
        FURI_LOG_E(
            TAG,
            "%s from %s to %s failed: %s, Code: %d",
            copy ? "Copy" : "Move",
            src_path,
            furi_string_get_cstr(dst_path),
            filesystem_api_error_get_desc(error),
            error);
    }

    return error;
}
