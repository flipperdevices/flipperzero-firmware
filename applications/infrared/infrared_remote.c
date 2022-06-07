#include "infrared_remote.h"

#include <stdlib.h>
#include <m-string.h>
#include <m-array.h>

#include <furi/common_defines.h>

ARRAY_DEF(infrared_button_array, InfraredRemoteButton*, M_PTR_OPLIST);

struct InfraredRemote {
    infrared_button_array_t buttons;
    string_t name;
};

InfraredRemote* infrared_remote_alloc() {
    InfraredRemote* remote = malloc(sizeof(InfraredRemote));
    infrared_button_array_init(remote->buttons);
    string_init(remote->name);
    return remote;
}

void infrared_remote_free(InfraredRemote* remote) {
    infrared_button_array_it_t it;

    for(infrared_button_array_it(it, remote->buttons); !infrared_button_array_end_p(it);
        infrared_button_array_next(it)) {
        infrared_remote_button_free(*infrared_button_array_cref(it));
    }

    infrared_button_array_clear(remote->buttons);
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
