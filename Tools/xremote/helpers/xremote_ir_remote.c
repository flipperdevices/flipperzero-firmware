#include "xremote_ir_remote.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>

#define TAG "XremoteInfraredRemote"

typedef struct InfraredRemote InfraredRemote;

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

void xremote_ir_remote_free(InfraredRemote* remote) {
    furi_string_free(remote->path);
    furi_string_free(remote->name);
    free(remote);
}

bool xremote_ir_remote_load(InfraredRemote* remote, FuriString* path) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    FlipperFormat* ff = flipper_format_buffered_file_alloc(storage);

    FuriString* buf;
    buf = furi_string_alloc();

    FURI_LOG_I(TAG, "loading IR Remote: \'%s\'", furi_string_get_cstr(path));
    bool success = false;

    UNUSED(remote);

    furi_string_free(buf);
    flipper_format_free(ff);
    furi_record_close(RECORD_STORAGE);
    return success;
}