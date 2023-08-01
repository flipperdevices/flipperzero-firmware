#include "evil_portal_storage.h"

void evil_portal_replace_index_html(Storage* storage, FuriString* path) {
    FS_Error error;
    error = storage_common_remove(storage, EVIL_PORTAL_INDEX_SAVE_PATH);
    if(error != FSE_OK) {
        FURI_LOG_D("EVIL PORTAL", "Error removing file");
    } else {
        FURI_LOG_D("EVIL PORTAL", "Error removed file");
    }
    error = storage_common_copy(storage, furi_string_get_cstr(path), EVIL_PORTAL_INDEX_SAVE_PATH);
    if(error != FSE_OK) {
        FURI_LOG_D("EVIL PORTAL", "Error copying file");
    }
}

void evil_portal_create_html_folder_if_not_exists(Storage* storage) {
    if(storage_common_stat(storage, HTML_FOLDER, NULL) == FSE_NOT_EXIST) {
        FURI_LOG_D("Evil Portal", "Directory %s doesn't exist. Will create new.", HTML_FOLDER);
        if(!storage_simply_mkdir(storage, HTML_FOLDER)) {
            FURI_LOG_E("Evil Portal", "Error creating directory %s", HTML_FOLDER);
        }
    }
}

void evil_portal_read_ap_name(void* context) {
    Evil_PortalApp* app = context;
    Storage* storage = app->storage;
    FileInfo fi;

    if(storage_common_stat(storage, EVIL_PORTAL_AP_SAVE_PATH, &fi) == FSE_OK) {
        File* ap_name = storage_file_alloc(storage);
        if(storage_file_open(ap_name, EVIL_PORTAL_AP_SAVE_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint16_t now_read =
                storage_file_read(ap_name, app->ap_name, (uint16_t)sizeof(app->ap_name) - 1);
            app->ap_name[now_read] = '\0';
        }
        storage_file_close(ap_name);
        storage_file_free(ap_name);
    } else {
        snprintf(app->ap_name, sizeof(app->ap_name), "Evil Portal");
    }
}

void evil_portal_write_ap_name(void* context) {
    Evil_PortalApp* app = context;
    Storage* storage = app->storage;

    File* ap_name = storage_file_alloc(storage);
    if(storage_file_open(ap_name, EVIL_PORTAL_AP_SAVE_PATH, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(ap_name, app->ap_name, strlen(app->ap_name));
    }
    storage_file_close(ap_name);
    storage_file_free(ap_name);
}

static char* sequential_file_resolve_path(
    Storage* storage,
    const char* dir,
    const char* prefix,
    const char* extension) {
    if(storage == NULL || dir == NULL || prefix == NULL || extension == NULL) {
        return NULL;
    }

    char file_path[256];
    int file_index = 0;

    do {
        if(snprintf(
               file_path, sizeof(file_path), "%s/%s_%d.%s", dir, prefix, file_index, extension) <
           0) {
            return NULL;
        }
        file_index++;
    } while(storage_file_exists(storage, file_path));

    return strdup(file_path);
}

void write_logs(Storage* storage, FuriString* portal_logs) {
    if(!storage_file_exists(storage, EVIL_PORTAL_LOG_SAVE_PATH)) {
        storage_simply_mkdir(storage, EVIL_PORTAL_LOG_SAVE_PATH);
    }

    char* seq_file_path =
        sequential_file_resolve_path(storage, EVIL_PORTAL_LOG_SAVE_PATH, "log", "txt");

    File* file = storage_file_alloc(storage);

    if(storage_file_open(file, seq_file_path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(
            file, furi_string_get_cstr(portal_logs), furi_string_utf8_length(portal_logs));
    }
    storage_file_close(file);
    storage_file_free(file);
}