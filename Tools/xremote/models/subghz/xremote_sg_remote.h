#pragma once

//#include "../../xremote_i.h"
#include "subghz_i.h"

typedef struct SubGhzRemote SubGhzRemote;

const char* xremote_sg_remote_get_name(SubGhzRemote* remote);

SubGhzRemote* xremote_sg_remote_alloc();

void xremote_sg_remote_free(SubGhzRemote* remote);

bool xremote_sg_remote_load(SubGhzRemote* remote, FuriString* path);