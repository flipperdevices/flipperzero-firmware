#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <m-array.h>
#include <toolbox/path.h>
#include <storage/storage.h>
#include <core/common_defines.h>
#include "xremote_remote_item.h"

//#include <infrared.h>
//#include <flipper_format/flipper_format.h>

typedef struct CrossRemote CrossRemote;

CrossRemote* cross_remote_alloc();
void cross_remote_free(CrossRemote* cross_remote);
bool cross_remote_load(CrossRemote* cross_remote, FuriString* path);