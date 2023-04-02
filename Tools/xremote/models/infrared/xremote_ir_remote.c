#include "xremote_ir_remote.h"

#define TAG "XremoteInfraredRemote"

ARRAY_DEF(InfraredButtonArray, InfraredRemoteButton*, M_PTR_OPLIST);

struct InfraredRemote {
    InfraredButtonArray_t buttons;
    FuriString* name;
    FuriString* path;
};

InfraredRemote* xremote_ir_remote_alloc() {
    InfraredRemote* remote = malloc(sizeof(InfraredRemote));
    InfraredButtonArray_init(remote->buttons);
    remote->name = furi_string_alloc();
    remote->path = furi_string_alloc();
    return remote;
}

const char* xremote_ir_remote_get_name(InfraredRemote* remote) {
    return furi_string_get_cstr(remote->name);
}

static void xremote_ir_remote_clear_buttons(InfraredRemote* remote) {
    InfraredButtonArray_it_t it;
    for(InfraredButtonArray_it(it, remote->buttons); !InfraredButtonArray_end_p(it);
        InfraredButtonArray_next(it)) {
        xremote_ir_remote_button_free(*InfraredButtonArray_cref(it));
    }
    InfraredButtonArray_reset(remote->buttons);
}

void xremote_ir_remote_free(InfraredRemote* remote) {
    furi_string_free(remote->path);
    furi_string_free(remote->name);
    free(remote);
}

InfraredRemoteButton* xremote_ir_get_button(InfraredRemote* remote, size_t index) {
    furi_assert(index < InfraredButtonArray_size(remote->buttons));
    return *InfraredButtonArray_get(remote->buttons, index);
}

bool xremote_ir_remote_load(InfraredRemote* remote, FuriString* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    FuriString* buf;
    buf = furi_string_alloc();

    FURI_LOG_I(TAG, "loading IR Remote: \'%s\'", furi_string_get_cstr(path));
    bool success = false;

    do {
        if(!flipper_format_buffered_file_open_existing(ff, furi_string_get_cstr(path))) break;
        uint32_t version;
        if(!flipper_format_read_header(ff, buf, &version)) break;
        if(!furi_string_equal(buf, "IR signals file") || (version != 1)) break;

        path_extract_filename(path, buf, true);
        xremote_ir_remote_clear_buttons(remote);

        for(bool can_read = true; can_read;) {
            InfraredRemoteButton* button = xremote_ir_remote_button_alloc();
            can_read = xremote_ir_signal_read(xremote_ir_remote_button_get_signal(button), ff, buf);
            if (can_read) {
                xremote_ir_remote_button_set_name(button, furi_string_get_cstr(buf));
                InfraredButtonArray_push_back(remote->buttons, button);
            } else {
                xremote_ir_remote_button_free(button);
            }
        }
        success = true;
    } while(false);

    UNUSED(remote);

    furi_string_free(buf);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}

size_t xremote_ir_remote_get_button_count(InfraredRemote* remote) {
    return InfraredButtonArray_size(remote->buttons);
}

InfraredRemoteButton* xremote_ir_remote_get_button(InfraredRemote* remote, size_t index) {
    furi_assert(index < InfraredButtonArray_size(remote->buttons));
    return *InfraredButtonArray_get(remote->buttons, index);
}