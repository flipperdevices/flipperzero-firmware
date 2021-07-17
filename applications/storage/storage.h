#pragma once
#include <furi.h>
#include "filesystem-api-v2.h"
#include "storage-sd-api.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct StorageApp StorageApp;

File storage_file(StorageApp* app);
bool storage_file_open(
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode);
bool storage_file_close(File* file);
uint16_t storage_file_read(File* file, void* buff, uint16_t bytes_to_read);
uint16_t storage_file_write(File* file, const void* buff, uint16_t bytes_to_write);
bool storage_file_seek(File* file, uint32_t offset, bool from_start);
uint64_t storage_file_tell(File* file);
bool storage_file_truncate(File* file);
uint64_t storage_file_size(File* file);
bool storage_file_sync(File* file);
bool storage_file_eof(File* file);
bool storage_dir_open(File* file, const char* path);
bool storage_dir_close(File* file);
bool storage_dir_read(File* file, FileInfo* fileinfo, char* name, uint16_t name_length);
bool storage_dir_rewind(File* file);

FS_Error storage_common_stat(StorageApp* app, const char* path, FileInfo* fileinfo);
FS_Error storage_common_remove(StorageApp* app, const char* path);
FS_Error storage_common_rename(StorageApp* app, const char* old_path, const char* new_path);
FS_Error storage_common_mkdir(StorageApp* app, const char* path);
FS_Error storage_common_fs_info(
    StorageApp* app,
    const char* fs_path,
    uint64_t* total_space,
    uint64_t* free_space);

const char* storage_error_get_desc(FS_Error error_id);
const char* storage_file_error_get_desc(File* file);

FS_Error storage_sd_format(StorageApp* app);
FS_Error storage_sd_unmount(StorageApp* app);
FS_Error storage_sd_info(StorageApp* app, SDInfo* info);
FS_Error storage_sd_status(StorageApp* app);

#ifdef __cplusplus
}
#endif