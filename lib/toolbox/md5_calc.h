#pragma once

#include <stdint.h>
#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

bool md5_calc_file(File* file, const char* path, unsigned char output[16]);

bool md5_string_calc_file(File* file, const char* path, FuriString* output);

#ifdef __cplusplus
}
#endif
