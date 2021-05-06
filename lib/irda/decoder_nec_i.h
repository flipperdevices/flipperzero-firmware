#pragma once

#include <stdbool.h>
#include <stdint.h>

const IrdaMessage* decode_nec(void *decoder, bool level, uint32_t duration);
void fini_nec(void* d);
void* init_nec(void);

