#pragma once

#include <stdbool.h>

#include "infrared_remote_button.h"

typedef struct InfraredRemote InfraredRemote;

InfraredRemote* infrared_remote_alloc();
void infrared_remote_free(InfraredRemote* remote);

void infrared_remote_set_name(InfraredRemote* remote, const char* name);
void infrared_remote_reset(InfraredRemote* remote);

bool infrared_remote_store(InfraredRemote* remote, const char* path);
bool infrared_remote_load(InfraredRemote* remote, const char* path);
