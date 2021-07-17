#pragma once
#include <furi.h>
#include "filesystem-api-v2.h"
#include "storage-sd-api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StorageApp StorageApp;

bool storage_file_open(
    StorageApp* app,
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode);
bool storage_file_close(StorageApp* app, File* file);
uint16_t storage_file_read(StorageApp* app, File* file, void* buff, uint16_t bytes_to_read);
uint16_t
    storage_file_write(StorageApp* app, File* file, const void* buff, uint16_t bytes_to_write);
bool storage_file_seek(StorageApp* app, File* file, uint32_t offset, bool from_start);
uint64_t storage_file_tell(StorageApp* app, File* file);
bool storage_file_truncate(StorageApp* app, File* file);
uint64_t storage_file_size(StorageApp* app, File* file);
bool storage_file_sync(StorageApp* app, File* file);
bool storage_file_eof(StorageApp* app, File* file);

bool storage_dir_open(StorageApp* app, File* file, const char* path);
bool storage_dir_close(StorageApp* app, File* file);
bool storage_dir_read(
    StorageApp* app,
    File* file,
    FileInfo* fileinfo,
    char* name,
    uint16_t name_length);
bool storage_dir_rewind(StorageApp* app, File* file);

FS_Error storage_common_stat(StorageApp* app, const char* path, FileInfo* fileinfo);
FS_Error storage_common_remove(StorageApp* app, const char* path);
FS_Error storage_common_rename(StorageApp* app, const char* old_path, const char* new_path);
FS_Error storage_common_mkdir(StorageApp* app, const char* path);
FS_Error storage_common_fs_info(
    StorageApp* app,
    const char* fs_path,
    uint64_t* total_space,
    uint64_t* free_space);

const char* storage_error_get_desc(StorageApp* app, FS_Error error_id);

FS_Error storage_sd_format(StorageApp* app);
FS_Error storage_sd_unmount(StorageApp* app);
FS_Error storage_sd_info(StorageApp* app, SDInfo* info);
FS_Error storage_sd_status(StorageApp* app);

#ifdef __cplusplus
}
#endif