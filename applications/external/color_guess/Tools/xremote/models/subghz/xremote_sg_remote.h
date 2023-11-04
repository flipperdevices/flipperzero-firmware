#pragma once

//#include "../../xremote_i.h"
#include "subghz_i.h"
//#include <lib/subghz/protocols/protocol_items.h>

//extern const SubGhzProtocolRegistry subghz_protocol_registry;

typedef struct SubGhzRemote SubGhzRemote;

const char* xremote_sg_remote_get_name(SubGhzRemote* remote);

SubGhzRemote* xremote_sg_remote_alloc();

void xremote_sg_remote_free(SubGhzRemote* remote);

bool xremote_sg_remote_load(SubGhzRemote* remote, FuriString* path);

uint32_t xremote_sg_remote_get_frequency(SubGhzRemote* remote);

const char* xremote_sg_remote_get_preset(SubGhzRemote* remote);