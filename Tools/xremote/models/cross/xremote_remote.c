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

bool cross_remote_load(CrossRemote* remote, FuriString* path) {
    UNUSED(remote);
    UNUSED(path);
    return true;
}