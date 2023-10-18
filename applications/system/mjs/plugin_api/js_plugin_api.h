#pragma once

#include <furi.h>
#include <mjs_core_public.h>

#ifdef __cplusplus
extern "C" {
#endif

bool js_delay_with_flags(struct mjs* mjs, uint32_t time);

#ifdef __cplusplus
}
#endif