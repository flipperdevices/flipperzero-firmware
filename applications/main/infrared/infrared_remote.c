#include "infrared_remote.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-list.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>

#define TAG "InfraredRemote"

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

void infrared_remote_set_name(InfraredRemote* remote, const char* name) {
    furi_string_set(remote->name, name);
}

const char* infrared_remote_get_name(InfraredRemote* remote) {
    return furi_string_get_cstr(remote->name);
}

void infrared_remote_set_path(InfraredRemote* remote, const char* path) {
    furi_string_set(remote->path, path);
}

const char* infrared_remote_get_path(InfraredRemote* remote) {
    return furi_string_get_cstr(remote->path);
}

size_t infrared_remote_get_button_count(const InfraredRemote* remote) {
    return InfraredButtonNameList_size(remote->button_names);
}

InfraredRemoteButton* infrared_remote_get_button(InfraredRemote* remote, size_t index) {
    UNUSED(remote);
    UNUSED(index);
    // furi_assert(index < InfraredButtonArray_size(remote->buttons));
    // return *InfraredButtonArray_get(remote->buttons, index);
    return NULL;
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

bool infrared_remote_find_button_by_name(InfraredRemote* remote, const char* name, size_t* index) {
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
    // InfraredRemoteButton* button = infrared_remote_button_alloc();
    // infrared_remote_button_set_name(button, name);
    // infrared_remote_button_set_signal(button, signal);
    // InfraredButtonArray_push_back(remote->buttons, button);
    // return infrared_remote_store(remote);
    return false;
}

bool infrared_remote_rename_button(InfraredRemote* remote, const char* new_name, size_t index) {
    UNUSED(remote);
    UNUSED(new_name);
    UNUSED(index);
    // furi_assert(index < InfraredButtonArray_size(remote->buttons));
    // InfraredRemoteButton* button = *InfraredButtonArray_get(remote->buttons, index);
    // infrared_remote_button_set_name(button, new_name);
    // return infrared_remote_store(remote);
    return false;
}

bool infrared_remote_delete_button(InfraredRemote* remote, size_t index) {
    UNUSED(remote);
    UNUSED(index);
    // furi_assert(index < InfraredButtonArray_size(remote->buttons));
    // InfraredRemoteButton* button;
    // InfraredButtonArray_pop_at(&button, remote->buttons, index);
    // infrared_remote_button_free(button);
    // return infrared_remote_store(remote);
    return false;
}

void infrared_remote_move_button(InfraredRemote* remote, size_t index_orig, size_t index_dest) {
    UNUSED(remote);
    UNUSED(index_orig);
    UNUSED(index_dest);
    // furi_assert(index_orig < InfraredButtonArray_size(remote->buttons));
    // furi_assert(index_dest < InfraredButtonArray_size(remote->buttons));
    //
    // InfraredRemoteButton* button;
    // InfraredButtonArray_pop_at(&button, remote->buttons, index_orig);
    // InfraredButtonArray_push_at(remote->buttons, index_dest, button);
}

bool infrared_remote_store(InfraredRemote* remote) {
    furi_crash("No touchy");
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    const char* path = furi_string_get_cstr(remote->path);

    FURI_LOG_I(TAG, "store file: \'%s\'", path);

    bool success = flipper_format_file_open_always(ff, path) &&
                   flipper_format_write_header_cstr(ff, "IR signals file", 1);
    if(success) {
        // InfraredButtonArray_it_t it;
        // for(InfraredButtonArray_it(it, remote->buttons); !InfraredButtonArray_end_p(it);
        //     InfraredButtonArray_next(it)) {
        //     InfraredRemoteButton* button = *InfraredButtonArray_cref(it);
        //     success = infrared_signal_save(
        //         infrared_remote_button_get_signal(button),
        //         ff,
        //         infrared_remote_button_get_name(button));
        //     if(!success) {
        //         break;
        //     }
        // }
    }

    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool infrared_remote_load(InfraredRemote* remote, FuriString* path) {
    const char* path_cstr = furi_string_get_cstr(path);
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    FuriString* buf;
    buf = furi_string_alloc();

    FURI_LOG_I(TAG, "load file: \'%s\'", path_cstr);
    bool success = false;

    do {
        if(!flipper_format_buffered_file_open_existing(ff, path_cstr)) break;

        uint32_t version;
        if(!flipper_format_read_header(ff, buf, &version)) break;

        if(!furi_string_equal(buf, "IR signals file") || (version != 1)) break;

        path_extract_filename(path, buf, true);

        infrared_remote_set_name(remote, furi_string_get_cstr(buf));
        infrared_remote_set_path(remote, path_cstr);

        InfraredButtonNameList_reset(remote->button_names);

        while(flipper_format_read_string(ff, "name", buf)) {
            InfraredButtonNameList_push_back(remote->button_names, buf);
        }

        success = true;
    } while(false);

    furi_string_free(buf);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

bool infrared_remote_remove(InfraredRemote* remote) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    FS_Error status = storage_common_remove(storage, furi_string_get_cstr(remote->path));
    infrared_remote_reset(remote);

    furi_record_close(RECORD_STORAGE);
    return (status == FSE_OK || status == FSE_NOT_EXIST);
}
