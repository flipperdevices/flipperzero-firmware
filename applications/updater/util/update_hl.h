#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool update_hl_prepare(const char* manifest_path);

bool update_hl_is_armed();

void update_hl_disarm();

#ifdef __cplusplus
}
#endif