#pragma once

//#include "../../xremote_i.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
//#include <m-array.h>
#include <storage/storage.h>
#include <core/common_defines.h>
#include <flipper_format/flipper_format.h>
#include <flipper_format/flipper_format_i.h>

typedef struct SubGhzRemote SubGhzRemote;

const char* xremote_sg_remote_get_name(SubGhzRemote* remote);
const char* xremote_sg_remote_get_filename(SubGhzRemote* remote);

SubGhzRemote* xremote_sg_remote_alloc();

void xremote_sg_remote_free(SubGhzRemote* remote);

bool xremote_sg_remote_load(SubGhzRemote* remote, FuriString* path);