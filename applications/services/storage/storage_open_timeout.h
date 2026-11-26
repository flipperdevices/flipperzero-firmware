#pragma once

#include <storage/storage.h>

#ifdef __cplusplus
extern "C" {
#endif

#define STORAGE_OPEN_TIMEOUT_MS 1000

bool storage_file_open_timeout(
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode,
    uint32_t timeout_ms);

bool storage_file_md5_timeout(
    File* file,
    const char* path,
    FuriString* output,
    FS_Error* file_error,
    uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
