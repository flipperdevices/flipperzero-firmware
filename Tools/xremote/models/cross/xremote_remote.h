#pragma once

#include "xremote_remote_item.h"
#include "../../xremote_i.h"

#define CROSS_REMOTE_PAUSE_NAME "Pause %ds"

CrossRemote* cross_remote_alloc();

void cross_remote_free(CrossRemote* cross_remote);
bool cross_remote_load(CrossRemote* cross_remote, FuriString* path);
const char* cross_remote_get_name(CrossRemote* remote);
void cross_remote_set_transmitting(CrossRemote* remote, int status);
int cross_remote_get_transmitting(CrossRemote* remote);
bool cross_remote_add_pause(CrossRemote* remote, int time);
bool cross_remote_add_ir_item(CrossRemote* remote, const char* name, InfraredSignal* signal);
bool cross_remote_add_subghz(CrossRemote* remote, SubGhzRemote* subghz);
void cross_remote_remove_item(CrossRemote* remote, size_t index);
void cross_remote_rename_item(CrossRemote* remote, size_t index, const char* name);
size_t cross_remote_get_item_count(CrossRemote* remote);
CrossRemoteItem* cross_remote_get_item(CrossRemote* remote, size_t index);

void cross_remote_set_name(CrossRemote* remote, const char* name);
void cross_remote_set_path(CrossRemote* remote, const char* path);
bool cross_remote_save_new(CrossRemote* remote, const char* name);
bool cross_remote_store(CrossRemote* remote);
