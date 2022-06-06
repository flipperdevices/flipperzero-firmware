#include "infrared_remote.h"

#include <stdlib.h>
#include <m-string.h>
#include <furi/common_defines.h>

struct InfraredRemote {
    // Array of buttons
    string_t name;
};

InfraredRemote* infrared_remote_alloc() {
    InfraredRemote* remote = malloc(sizeof(InfraredRemote));
    string_init(remote->name);
    return remote;
}

void infrared_remote_free(InfraredRemote* remote) {
    string_clear(remote->name);
    free(remote);
}

void infrared_remote_set_name(InfraredRemote* remote, const char* name) {
    string_set_str(remote->name, name);
}

void infrared_remote_reset(InfraredRemote* remote) {
    UNUSED(remote);
}

bool infrared_remote_load(InfraredRemote* remote, const char* path) {
    UNUSED(remote);
    UNUSED(path);
    return true;
}

bool infrared_remote_save(InfraredRemote* remote, const char* path) {
    UNUSED(remote);
    UNUSED(path);
    return true;
}
