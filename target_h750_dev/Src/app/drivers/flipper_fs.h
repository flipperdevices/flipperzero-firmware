#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "../core/furi.h"
#include "fatfs.h"

bool initFsDriver(void);

#ifdef __cplusplus
}
#endif