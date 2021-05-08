#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "irda.h"

const IrdaMessage* decode_nec(void* decoder, bool level, uint32_t duration);
void fini_nec(void* decoder);
void* init_nec(void);

