#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

extern const char* DEFAULT_BACKUP_LOCATION;

bool lfs_backup_create(const char* destination);
bool lfs_backup_exists(const char* source);
bool lfs_backup_unpack(const char* source);

#ifdef __cplusplus
}
#endif