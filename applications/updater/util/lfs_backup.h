#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

bool lfs_backup_create();
bool lfs_backup_exists();
bool lfs_backup_unpack();

#ifdef __cplusplus
}
#endif