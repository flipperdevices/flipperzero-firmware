/*
   This file was taken from the project: 
   https://github.com/DarkFlippers/unleashed-firmware

   The original project is licensed under the GNU GPLv3

   Modifications made:
   - Added function infrared_remote_get_button_by_name()
   - Added function infrared_remote_delete_button_by_name()
   - Added function infrared_remote_push_button()
*/

#pragma once

#include <stdbool.h>

#include "infrared_remote_button.h"

typedef struct InfraredRemote InfraredRemote;

InfraredRemote* infrared_remote_alloc();
void infrared_remote_free(InfraredRemote* remote);
void infrared_remote_reset(InfraredRemote* remote);

void infrared_remote_set_name(InfraredRemote* remote, const char* name);
const char* infrared_remote_get_name(InfraredRemote* remote);

void infrared_remote_set_path(InfraredRemote* remote, const char* path);
const char* infrared_remote_get_path(InfraredRemote* remote);

size_t infrared_remote_get_button_count(InfraredRemote* remote);
InfraredRemoteButton* infrared_remote_get_button(InfraredRemote* remote, size_t index);
bool infrared_remote_find_button_by_name(InfraredRemote* remote, const char* name, size_t* index);
InfraredRemoteButton* infrared_remote_get_button_by_name(InfraredRemote* remote, const char* name);

bool infrared_remote_add_button(InfraredRemote* remote, const char* name, InfraredSignal* signal);
void infrared_remote_push_button(InfraredRemote* remote, const char* name, InfraredSignal* signal);
bool infrared_remote_rename_button(InfraredRemote* remote, const char* new_name, size_t index);
bool infrared_remote_delete_button(InfraredRemote* remote, size_t index);
bool infrared_remote_delete_button_by_name(InfraredRemote* remote, const char* name);
void infrared_remote_move_button(InfraredRemote* remote, size_t index_orig, size_t index_dest);

bool infrared_remote_store(InfraredRemote* remote);
bool infrared_remote_load(InfraredRemote* remote, FuriString* path);
bool infrared_remote_remove(InfraredRemote* remote);
