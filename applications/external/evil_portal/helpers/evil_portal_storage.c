#include "evil_portal_storage.h"

static Storage* evil_portal_open_storage() {
    return furi_record_open(RECORD_STORAGE);
}

static void evil_portal_close_storage() {
    furi_record_close(RECORD_STORAGE);
}

void evil_portal_read_index_html(void* context) {
    Evil_PortalApp* app = context;
    Storage* storage = evil_portal_open_storage();
    FileInfo fi;

    if(storage_common_stat(storage, EVIL_PORTAL_INDEX_SAVE_PATH, &fi) == FSE_OK) {
        File* index_html = storage_file_alloc(storage);
        if(storage_file_open(
               index_html, EVIL_PORTAL_INDEX_SAVE_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
            app->index_html = malloc((size_t)fi.size);
            uint8_t* buf_ptr = app->index_html;
            size_t read = 0;
            while(read < fi.size) {
                size_t to_read = fi.size - read;
                if(to_read > UINT16_MAX) to_read = UINT16_MAX;
                uint16_t now_read = storage_file_read(index_html, buf_ptr, (uint16_t)to_read);
                read += now_read;
                buf_ptr += now_read;
            }
            free(buf_ptr);
        }
        storage_file_close(index_html);
        storage_file_free(index_html);
    } else {
        char* html_error = "<b>Evil portal</b><br>Unable to read the html file.<br>"
                           "Is the SD Card set up correctly? <br>See instructions @ "
                           "github.com/bigbrodude6119/flipper-zero-evil-portal<br>"
                           "Under the 'Install pre-built app on the flipper' section.";
        app->index_html = (uint8_t*)html_error;
    }

    evil_portal_close_storage();
}

void evil_portal_read_ap_name(void* context) {
    Evil_PortalApp* app = context;
    Storage* storage = evil_portal_open_storage();
    FileInfo fi;

    if(storage_common_stat(storage, EVIL_PORTAL_AP_SAVE_PATH, &fi) == FSE_OK) {
        File* ap_name = storage_file_alloc(storage);
        if(storage_file_open(ap_name, EVIL_PORTAL_AP_SAVE_PATH, FSAM_READ, FSOM_OPEN_EXISTING)) {
            app->ap_name = malloc((size_t)fi.size);
            uint8_t* buf_ptr = app->ap_name;
            size_t read = 0;
            while(read < fi.size) {
                size_t to_read = fi.size - read;
                if(to_read > UINT16_MAX) to_read = UINT16_MAX;
                uint16_t now_read = storage_file_read(ap_name, buf_ptr, (uint16_t)to_read);
                read += now_read;
                buf_ptr += now_read;
            }
            free(buf_ptr);
        }
        storage_file_close(ap_name);
        storage_file_free(ap_name);
    } else {
        char* app_default = "Evil Portal";
        app->ap_name = (uint8_t*)app_default;
    }
    evil_portal_close_storage();
}

char* sequential_file_resolve_path(
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

void write_logs(char* portal_logs) {
    Storage* storage = evil_portal_open_storage();

    if(!storage_file_exists(storage, EVIL_PORTAL_LOG_SAVE_PATH)) {
        storage_simply_mkdir(storage, EVIL_PORTAL_LOG_SAVE_PATH);
    }

    char* seq_file_path =
        sequential_file_resolve_path(storage, EVIL_PORTAL_LOG_SAVE_PATH, "log", "txt");

    File* file = storage_file_alloc(storage);

    if(storage_file_open(file, seq_file_path, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, portal_logs, strlen(portal_logs));
    }
    storage_file_close(file);
    storage_file_free(file);
    evil_portal_close_storage();

    portal_logs = "";
}