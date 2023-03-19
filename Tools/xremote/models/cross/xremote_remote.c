#include "../infrared/xremote_ir_signal.h"
#include "xremote_remote.h"

#define TAG "XremoteCrossRemote"

ARRAY_DEF(CrossRemoteItemArray, CrossRemoteItem*, M_PTR_OPLIST);

struct CrossRemote {
    FuriString* name;
    FuriString* path;
    CrossRemoteItemArray_t items;
};

static void cross_remote_clear_items(CrossRemote* remote) {
    CrossRemoteItemArray_it_t it;
    for(CrossRemoteItemArray_it(it, remote->items); !CrossRemoteItemArray_end_p(it);
        CrossRemoteItemArray_next(it)) {
        xremote_remote_item_free(*CrossRemoteItemArray_cref(it));
    }
    CrossRemoteItemArray_reset(remote->items);
}

static void cross_remote_find_vacant_remote_name(FuriString* name, const char* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FuriString* base_path;
    base_path = furi_string_alloc_set(path);

    if(furi_string_end_with(base_path, XREMOTE_APP_EXTENSION)) {
        size_t filename_start = furi_string_search_rchar(base_path, '/');
        furi_string_left(base_path, filename_start);
    }

    furi_string_printf(
        base_path, "%s/%s%s", path, furi_string_get_cstr(name), XREMOTE_APP_EXTENSION);

    FS_Error status = storage_common_stat(storage, furi_string_get_cstr(base_path), NULL);

    if(status == FSE_OK) {
        // If name is taken, try another name2, name3 etc
        size_t dot = furi_string_search_rchar(base_path, '.');
        furi_string_left(base_path, dot);

        FuriString* path_temp;
        path_temp = furi_string_alloc();

        uint32_t i = 1;
        do {
            furi_string_printf(
                path_temp, "%s%lu%s", furi_string_get_cstr(base_path), ++i, XREMOTE_APP_EXTENSION);
            status = storage_common_stat(storage, furi_string_get_cstr(path_temp), NULL);
        } while(status == FSE_OK);

        furi_string_free(path_temp);

        if(status == FSE_NOT_EXIST) {
            furi_string_cat_printf(name, "%lu", i);
        }
    }

    furi_string_free(base_path);
    furi_record_close(RECORD_STORAGE);
}

CrossRemote* cross_remote_alloc() {
    CrossRemote* remote = malloc(sizeof(CrossRemote));
    CrossRemoteItemArray_init(remote->items);
    remote->name = furi_string_alloc();
    remote->path = furi_string_alloc();
    return remote;
}

void cross_remote_free(CrossRemote* remote) {
    furi_string_free(remote->name);
    furi_string_free(remote->path);
    cross_remote_clear_items(remote);
    CrossRemoteItemArray_clear(remote->items);
    free(remote);
}

const char* cross_remote_get_name(CrossRemote* remote) {
    return furi_string_get_cstr(remote->name);
}

bool cross_remote_add_ir_item(CrossRemote* remote, const char* name, InfraredSignal* signal) {
    CrossRemoteItem* item = xremote_remote_item_alloc();
    xremote_remote_item_set_name(item, name);
    xremote_remote_item_set_ir_signal(item, signal);
    CrossRemoteItemArray_push_back(remote->items, item);
    return true;
    //return cross_remote_store(remote);
}

size_t cross_remtoe_get_item_count(CrossRemote* remote) {
    return CrossRemoteItemArray_size(remote->items);
}

CrossRemoteItem* cross_remote_get_item(CrossRemote* remote, size_t index) {
    furi_assert(index < CrossRemoteItemArray_size(remote->items));
    return *CrossRemoteItemArray_get(remote->items, index);
}


bool cross_remote_load(CrossRemote* remote, FuriString* path) {
    UNUSED(remote);
    UNUSED(path);
    return true;
}

void cross_remote_set_name(CrossRemote* remote, const char* name) {
    furi_string_set(remote->name, name);
}

void cross_remote_set_path(CrossRemote* remote, const char* path) {
    furi_string_set(remote->path, path);
}

bool cross_remote_save_new(CrossRemote* remote) {
    FuriString *new_name, *new_path;
    new_name = furi_string_alloc_set(XREMOTE_DEFAULT_REMOTE_NAME);
    new_path = furi_string_alloc_set(XREMOTE_APP_FOLDER);

    cross_remote_find_vacant_remote_name(new_name, furi_string_get_cstr(new_path));
    furi_string_cat_printf(
        new_path, "/%s%s", furi_string_get_cstr(new_name), XREMOTE_APP_EXTENSION);

    cross_remote_set_name(remote, furi_string_get_cstr(new_name));
    cross_remote_set_path(remote, furi_string_get_cstr(new_path));

    furi_string_free(new_name);
    furi_string_free(new_path);
    return cross_remote_store(remote);
}

bool cross_remote_store(CrossRemote* remote) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    const char* path = furi_string_get_cstr(remote->path);

    FURI_LOG_I(TAG, "Storing file: \'%s\'", path);

    bool success = flipper_format_file_open_always(ff, path) &&
                   flipper_format_write_header_cstr(ff, "Cross Remote File", 1);

    //TODO save Items

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}