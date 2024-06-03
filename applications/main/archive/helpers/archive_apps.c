#include "archive_apps.h"
#include "archive_browser.h"
#include <applications/main/u2f/u2f_data.h>

static const char* known_apps[] = {
    [ArchiveAppTypeU2f] = "u2f",
    [ArchiveAppTypeSearch] = "search",
};

ArchiveAppTypeEnum archive_get_app_type(const char* path) {
    const char* app_name = strchr(path, ':');
    if(app_name == NULL) {
        return ArchiveAppTypeUnknown;
    }
    app_name++;

    for(size_t i = 0; i < COUNT_OF(known_apps); i++) { //-V1008
        if(strncmp(app_name, known_apps[i], strlen(known_apps[i])) == 0) {
            return i;
        }
    }
    return ArchiveAppTypeUnknown;
}

bool archive_app_is_available(void* context, const char* path) {
    UNUSED(context);
    furi_assert(path);

    ArchiveAppTypeEnum app = archive_get_app_type(path);
    bool res = false;

    Storage* storage = furi_record_open(RECORD_STORAGE);
    switch(app) {
    case ArchiveAppTypeU2f:
        res = storage_file_exists(storage, U2F_KEY_FILE) &&
              storage_file_exists(storage, U2F_CNT_FILE);
        break;
    case ArchiveAppTypeSearch:
        res = true;
        break;
    default:
        break;
    }
    furi_record_close(RECORD_STORAGE);

    return res;
}

bool archive_app_read_dir(void* context, const char* path) {
    furi_assert(context);
    furi_assert(path);
    ArchiveBrowserView* browser = context;

    ArchiveAppTypeEnum app = archive_get_app_type(path);

    switch(app) {
    case ArchiveAppTypeU2f:
        archive_file_array_rm_all(browser);
        archive_add_app_item(browser, "/app:u2f/U2F Token");
        return true;
    case ArchiveAppTypeSearch:
        return true;
    default:
        archive_file_array_rm_all(browser);
        return false;
    }
}

void archive_app_delete_file(void* context, const char* path) {
    furi_assert(context);
    furi_assert(path);
    ArchiveBrowserView* browser = context;

    ArchiveAppTypeEnum app = archive_get_app_type(path);
    bool res = false;

    Storage* fs_api = furi_record_open(RECORD_STORAGE);
    switch(app) {
    case ArchiveAppTypeU2f:
        res = (storage_common_remove(fs_api, U2F_KEY_FILE) == FSE_OK);
        res |= (storage_common_remove(fs_api, U2F_CNT_FILE) == FSE_OK);
        if(archive_is_favorite("/app:u2f/U2F Token")) {
            archive_favorites_delete("/app:u2f/U2F Token");
        }
        break;
    case ArchiveAppTypeSearch:
        break;
    default:
        break;
    }
    furi_record_close(RECORD_STORAGE);

    if(res) {
        archive_file_array_rm_selected(browser);
    }
}
