#include "infrared_remote.h"

#include <m-list.h>

#include <toolbox/path.h>
#include <storage/storage.h>

#define TAG "InfraredRemote"

#define INFRARED_FILE_HEADER "IR signals file"
#define INFRARED_FILE_VERSION (1)

LIST_DEF(InfraredButtonNameList, FuriString*, FURI_STRING_OPLIST);

struct InfraredRemote {
    InfraredButtonNameList_t button_names;
    InfraredSignal* current_signal;
    FuriString* name;
    FuriString* path;
};

InfraredRemote* infrared_remote_alloc() {
    InfraredRemote* remote = malloc(sizeof(InfraredRemote));
    InfraredButtonNameList_init(remote->button_names);
    remote->current_signal = infrared_signal_alloc();
    remote->name = furi_string_alloc();
    remote->path = furi_string_alloc();
    return remote;
}

void infrared_remote_free(InfraredRemote* remote) {
    InfraredButtonNameList_clear(remote->button_names);
    infrared_signal_free(remote->current_signal);
    furi_string_free(remote->path);
    furi_string_free(remote->name);
    free(remote);
}

void infrared_remote_reset(InfraredRemote* remote) {
    InfraredButtonNameList_reset(remote->button_names);
    furi_string_reset(remote->name);
    furi_string_reset(remote->path);
}

const char* infrared_remote_get_name(const InfraredRemote* remote) {
    return furi_string_get_cstr(remote->name);
}

static void infrared_remote_set_path(InfraredRemote* remote, const char* path) {
    furi_string_set(remote->path, path);
    path_extract_filename(remote->path, remote->name, true);
}

const char* infrared_remote_get_path(const InfraredRemote* remote) {
    return furi_string_get_cstr(remote->path);
}

size_t infrared_remote_get_button_count(const InfraredRemote* remote) {
    return InfraredButtonNameList_size(remote->button_names);
}

InfraredRemoteButton* infrared_remote_get_button(InfraredRemote* remote, size_t index) {
    UNUSED(remote);
    UNUSED(index);
    furi_crash("infrared_remote_get_button() not implemented");
}

const char* infrared_remote_get_button_name(const InfraredRemote* remote, size_t index) {
    furi_assert(index < infrared_remote_get_button_count(remote));
    return furi_string_get_cstr(*InfraredButtonNameList_cget(remote->button_names, index));
}

const InfraredSignal*
    infrared_remote_get_button_signal(const InfraredRemote* remote, size_t index) {
    furi_assert(index < infrared_remote_get_button_count(remote));

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    do {
        const char* path = furi_string_get_cstr(remote->path);
        if(!flipper_format_buffered_file_open_existing(ff, path)) break;

        const char* name = infrared_remote_get_button_name(remote, index);
        if(!infrared_signal_search_and_read(remote->current_signal, ff, name)) {
            FURI_LOG_E(TAG, "Failed to load signal '%s' from file '%s'", name, path);
        }

        if(!flipper_format_buffered_file_close(ff)) break;
    } while(false);

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return remote->current_signal;
}

bool infrared_remote_find_button_by_name(
    const InfraredRemote* remote,
    const char* name,
    size_t* index) {
    uint32_t i = 0;
    InfraredButtonNameList_it_t it;

    for(InfraredButtonNameList_it(it, remote->button_names); !InfraredButtonNameList_end_p(it);
        InfraredButtonNameList_next(it), ++i) {
        if(furi_string_equal(*InfraredButtonNameList_cref(it), name)) {
            *index = i;
            return true;
        }
    }

    return false;
}

bool infrared_remote_add_button(InfraredRemote* remote, const char* name, InfraredSignal* signal) {
    UNUSED(remote);
    UNUSED(name);
    UNUSED(signal);
    furi_crash("infrared_remote_add_button() not implemented");
    // InfraredRemoteButton* button = infrared_remote_button_alloc();
    // infrared_remote_button_set_name(button, name);
    // infrared_remote_button_set_signal(button, signal);
    // InfraredButtonArray_push_back(remote->buttons, button);
    // return infrared_remote_store(remote);
}

bool infrared_remote_rename_button(InfraredRemote* remote, const char* new_name, size_t index) {
    UNUSED(remote);
    UNUSED(new_name);
    UNUSED(index);
    furi_crash("infrared_remote_rename_button() not implemented");
    // furi_assert(index < InfraredButtonArray_size(remote->buttons));
    // InfraredRemoteButton* button = *InfraredButtonArray_get(remote->buttons, index);
    // infrared_remote_button_set_name(button, new_name);
    // return infrared_remote_store(remote);
}

bool infrared_remote_delete_button(InfraredRemote* remote, size_t index) {
    UNUSED(remote);
    UNUSED(index);
    furi_crash("infrared_remote_delete_button() not implemented");
    // furi_assert(index < InfraredButtonArray_size(remote->buttons));
    // InfraredRemoteButton* button;
    // InfraredButtonArray_pop_at(&button, remote->buttons, index);
    // infrared_remote_button_free(button);
    // return infrared_remote_store(remote);
}

void infrared_remote_move_button(InfraredRemote* remote, size_t index_orig, size_t index_dest) {
    UNUSED(remote);
    UNUSED(index_orig);
    UNUSED(index_dest);
    furi_crash("infrared_remote_move_button() not implemented");
    // furi_assert(index_orig < InfraredButtonArray_size(remote->buttons));
    // furi_assert(index_dest < InfraredButtonArray_size(remote->buttons));
    //
    // InfraredRemoteButton* button;
    // InfraredButtonArray_pop_at(&button, remote->buttons, index_orig);
    // InfraredButtonArray_push_at(remote->buttons, index_dest, button);
}

bool infrared_remote_load(InfraredRemote* remote, const char* path) {
    FURI_LOG_I(TAG, "Loading file: '%s'", path);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    FuriString* tmp = furi_string_alloc();
    bool success = false;

    do {
        if(!flipper_format_buffered_file_open_existing(ff, path)) break;

        uint32_t version;
        if(!flipper_format_read_header(ff, tmp, &version)) break;

        if(!furi_string_equal(tmp, INFRARED_FILE_HEADER) || (version != INFRARED_FILE_VERSION))
            break;

        infrared_remote_set_path(remote, path);
        InfraredButtonNameList_reset(remote->button_names);

        while(flipper_format_read_string(ff, "name", tmp)) {
            InfraredButtonNameList_push_back(remote->button_names, tmp);
        }

        success = true;
    } while(false);

    furi_string_free(tmp);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);

    return success;
}

bool infrared_remote_rename(InfraredRemote* remote, const char* new_path) {
    const char* old_path = infrared_remote_get_path(remote);

    Storage* storage = furi_record_open(RECORD_STORAGE);
    const FS_Error status = storage_common_rename(storage, old_path, new_path);
    furi_record_close(RECORD_STORAGE);

    const bool success = (status == FSE_OK || status == FSE_EXIST);

    if(success) {
        infrared_remote_set_path(remote, new_path);
    }

    return success;
}

bool infrared_remote_remove(InfraredRemote* remote) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    const FS_Error status = storage_common_remove(storage, infrared_remote_get_path(remote));
    furi_record_close(RECORD_STORAGE);

    const bool success = (status == FSE_OK || status == FSE_NOT_EXIST);

    if(success) {
        infrared_remote_reset(remote);
    }

    return success;
}
