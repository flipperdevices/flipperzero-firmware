#pragma once
#include "m-string.h"

#ifdef __cplusplus
extern "C" {
#endif

void path_extract_filename_no_ext(const char* path, string_t filename);

#ifdef __cplusplus
}
#endif