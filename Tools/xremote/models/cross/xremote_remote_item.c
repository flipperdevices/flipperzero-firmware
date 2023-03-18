#include "xremote_remote_item.h"

struct CrossRemoteItem {
    FuriString* name;
    int16_t type;
    int32_t time;
};

CrossRemoteItem* xremote_remote_item_alloc() {
    CrossRemoteItem* item = malloc(sizeof(CrossRemoteItem));
    item->name = furi_string_alloc();
    item->time = 0;
    item->type = 0;
    return item;
}

void xremote_remote_item_free(CrossRemoteItem* item) {
    furi_string_free(item->name);
    free(item);
}

void xremote_remote_item_set_name(CrossRemoteItem* item, const char* name) {
    furi_string_set(item->name, name);
}

const char* xremote_remote_item_get_name(CrossRemoteItem* item) {
    return furi_string_get_cstr(item->name);
}