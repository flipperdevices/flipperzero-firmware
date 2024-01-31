#pragma once

#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

bool storage_printf(File* file, const char* format, ...);
bool storage_write_file(File* file, const char* str, size_t str_len);
void save_log_and_write(char* str, size_t len);

#ifdef __cplusplus
}
#endif