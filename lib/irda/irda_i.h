#pragma once
#include "irda.h"
#include <stddef.h>

typedef IrdaMessage* (*IrdaDecode) (void* ctx, bool level, uint32_t duration);
typedef void* (*IrdaInit) (void);
typedef void (*IrdaFini) (void*);

typedef void (*IrdaEncode)(uint32_t address, uint32_t command, bool repeat);

