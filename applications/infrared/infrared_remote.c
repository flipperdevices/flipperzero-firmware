#include "infrared_remote.h"

#include <stdlib.h>
#include <m-string.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <furi/common_defines.h>

#define TAG "InfraredRemote"

ARRAY_DEF(infrared_button_array, InfraredRemoteButton*, M_PTR_OPLIST);

struct InfraredRemote {
    infrared_button_array_t buttons;
    string_t name;
};

static void infrared_remote_clear_buttons(InfraredRemote* remote) {
    infrared_button_array_it_t it;
    for(infrared_button_array_it(it, remote->buttons); !infrared_button_array_end_p(it);
        infrared_button_array_next(it)) {
        infrared_remote_button_free(*infrared_button_array_cref(it));
    }
    infrared_button_array_reset(remote->buttons);
}

InfraredRemote* infrared_remote_alloc() {
    InfraredRemote* remote = malloc(sizeof(InfraredRemote));
    infrared_button_array_init(remote->buttons);
    string_init(remote->name);
    return remote;
}

void infrared_remote_free(InfraredRemote* remote) {
    infrared_remote_clear_buttons(remote);
    infrared_button_array_clear(remote->buttons);
    string_clear(remote->name);
    free(remote);
}

void infrared_remote_set_name(InfraredRemote* remote, const char* name) {
    string_set_str(remote->name, name);
}

const char* infrared_remote_get_name(InfraredRemote* remote) {
    return string_get_cstr(remote->name);
}

size_t infrared_remote_get_button_count(InfraredRemote* remote) {
    return infrared_button_array_size(remote->buttons);
}

InfraredRemoteButton* infrared_remote_get_button(InfraredRemote* remote, size_t index) {
    furi_assert(index < infrared_button_array_size(remote->buttons));
    return *infrared_button_array_get(remote->buttons, index);
}

bool infrared_remote_store(InfraredRemote* remote, const char* path) {
    Storage* storage = furi_record_open("storage");
    FlipperFormat* ff = flipper_format_file_alloc(storage);
    FURI_LOG_I(TAG, "store file: \'%s\'", path);

    bool success = flipper_format_file_open_always(ff, path) &&
                   flipper_format_write_header_cstr(ff, "IR signals file", 1);
    if(success) {
        infrared_button_array_it_t it;
        for(infrared_button_array_it(it, remote->buttons); !infrared_button_array_end_p(it);
            infrared_button_array_next(it)) {
            InfraredRemoteButton* button = *infrared_button_array_cref(it);
            success = infrared_signal_save(
                infrared_remote_button_get_signal(button),
                ff,
                infrared_remote_button_get_name(button));
            if(!success) {
                break;
            }
        }
    }

    flipper_format_free(ff);
    furi_record_close("storage");
    return success;
}

bool infrared_remote_load(InfraredRemote* remote, const char* path) {
    Storage* storage = furi_record_open("storage");
    FlipperFormat* ff = flipper_format_file_alloc(storage);

    string_t buf;
    string_init(buf);

    FURI_LOG_I(TAG, "load file: \'%s\'", path);
    bool success = flipper_format_file_open_existing(ff, path);

    if(success) {
        uint32_t version;
        success = flipper_format_read_header(ff, buf, &version) &&
                  !string_cmp_str(buf, "IR signals file") && (version == 1);
    }

    if(success) {
        //TODO: Find a better way to pass string_t
        string_t path_str;
        string_init_set_str(path_str, path);
        path_extract_filename(path_str, buf, true);
        string_clear(path_str);

        infrared_remote_clear_buttons(remote);
        infrared_remote_set_name(remote, string_get_cstr(buf));

        for(bool can_read = true; can_read;) {
            InfraredRemoteButton* button = infrared_remote_button_alloc();
            can_read = infrared_signal_read(infrared_remote_button_get_signal(button), ff, buf);
            if(can_read) {
                infrared_remote_button_set_name(button, string_get_cstr(buf));
                infrared_button_array_push_back(remote->buttons, button);
            } else {
                infrared_remote_button_free(button);
            }
        }
    }

    string_clear(buf);
    flipper_format_free(ff);
    furi_record_close("storage");
    return success;
}
