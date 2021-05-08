#pragma once
#include "irda.h"
#include <stddef.h>

typedef IrdaMessage* (*Decode) (void* ctx, bool level, uint32_t duration);
typedef void* (*Init) (void);
typedef void (*Fini) (void*);

typedef void (*Encode)(uint32_t address, uint32_t command, bool repeat);

