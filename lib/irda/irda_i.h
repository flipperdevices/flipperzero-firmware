#pragma once
#include "irda.h"
#include <stddef.h>
#include "irda_encoder_i.h"
#include "irda_common_decoder_i.h"
#include "irda_protocol_defs_i.h"

typedef IrdaMessage* (*IrdaDecode) (void* ctx, bool level, uint32_t duration);
typedef void* (*IrdaInit) (void);
typedef void (*IrdaFini) (void*);

typedef void (*IrdaEncode)(uint32_t address, uint32_t command, bool repeat);

