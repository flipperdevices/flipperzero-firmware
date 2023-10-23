#pragma once

#include "infrared_signal.h"

typedef struct InfraredRemote InfraredRemote;

InfraredRemote* infrared_remote_alloc();
void infrared_remote_free(InfraredRemote* remote);
void infrared_remote_reset(InfraredRemote* remote);

const char* infrared_remote_get_name(const InfraredRemote* remote);
const char* infrared_remote_get_path(const InfraredRemote* remote);

size_t infrared_remote_get_signal_count(const InfraredRemote* remote);
const char* infrared_remote_get_signal_name(const InfraredRemote* remote, size_t index);
bool infrared_remote_get_signal_index(
    const InfraredRemote* remote,
    const char* name,
    size_t* index);

bool infrared_remote_load_signal(
    const InfraredRemote* remote,
    InfraredSignal* signal,
    size_t index);

bool infrared_remote_append_signal(
    InfraredRemote* remote,
    const InfraredSignal* signal,
    const char* name);

bool infrared_remote_insert_signal(
    InfraredRemote* remote,
    const InfraredSignal* signal,
    const char* name,
    size_t index);

bool infrared_remote_rename_signal(InfraredRemote* remote, size_t index, const char* new_name);
bool infrared_remote_move_signal(InfraredRemote* remote, size_t index, size_t new_index);
bool infrared_remote_delete_signal(InfraredRemote* remote, size_t index);

bool infrared_remote_create(InfraredRemote* remote, const char* path);
bool infrared_remote_load(InfraredRemote* remote, const char* path);
bool infrared_remote_rename(InfraredRemote* remote, const char* new_path);
bool infrared_remote_remove(InfraredRemote* remote);
