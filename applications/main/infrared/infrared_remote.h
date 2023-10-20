#pragma once

#include "infrared_signal.h"
#include "infrared_remote_button.h"

typedef struct InfraredRemote InfraredRemote;

InfraredRemote* infrared_remote_alloc();
void infrared_remote_free(InfraredRemote* remote);
void infrared_remote_reset(InfraredRemote* remote);

const char* infrared_remote_get_name(const InfraredRemote* remote);
const char* infrared_remote_get_path(const InfraredRemote* remote);

size_t infrared_remote_get_button_count(const InfraredRemote* remote);
const char* infrared_remote_get_button_name(const InfraredRemote* remote, size_t index);
const InfraredSignal*
    infrared_remote_get_button_signal(const InfraredRemote* remote, size_t index);

bool infrared_remote_find_button_by_name(
    const InfraredRemote* remote,
    const char* name,
    size_t* index);

bool infrared_remote_add_button(
    InfraredRemote* remote,
    const InfraredSignal* signal,
    const char* name);
bool infrared_remote_rename_button(InfraredRemote* remote, size_t index, const char* new_name);
bool infrared_remote_delete_button(InfraredRemote* remote, size_t index);
void infrared_remote_move_button(InfraredRemote* remote, size_t index, size_t new_index);

bool infrared_remote_load(InfraredRemote* remote, const char* path);
bool infrared_remote_rename(InfraredRemote* remote, const char* new_path);
bool infrared_remote_remove(InfraredRemote* remote);

// DEPRECATED
InfraredRemoteButton* infrared_remote_get_button(InfraredRemote* remote, size_t index);
