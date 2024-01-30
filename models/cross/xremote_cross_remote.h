#pragma once

#include "xremote_cross_remote_item.h"
#include "../../xremote_i.h"

#define CROSS_REMOTE_PAUSE_NAME "Pause %ds"

CrossRemote* xremote_cross_remote_alloc();

void xremote_cross_remote_free(CrossRemote* cross_remote);
bool xremote_cross_remote_load(CrossRemote* cross_remote, FuriString* path);
const char* xremote_cross_remote_get_name(CrossRemote* remote);
void xremote_cross_remote_set_transmitting(CrossRemote* remote, int status);
int xremote_cross_remote_get_transmitting(CrossRemote* remote);
bool xremote_cross_remote_add_pause(CrossRemote* remote, int time);
bool xremote_cross_remote_add_ir_item(
    CrossRemote* remote,
    const char* name,
    InfraredSignal* signal,
    uint32_t timing);
bool xremote_cross_remote_add_subghz(CrossRemote* remote, SubGhzRemote* subghz);
void xremote_cross_remote_remove_item(CrossRemote* remote, size_t index);
void xremote_cross_remote_rename_item(CrossRemote* remote, size_t index, const char* name);
size_t xremote_cross_remote_get_item_count(CrossRemote* remote);
CrossRemoteItem* xremote_cross_remote_get_item(CrossRemote* remote, size_t index);

bool xremote_cross_remote_save_new(CrossRemote* remote, const char* name);
bool xremote_cross_remote_delete(CrossRemote* remote);