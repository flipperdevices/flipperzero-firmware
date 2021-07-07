#pragma once
#include <furi.h>
#include "filesystem-i.h"

#ifdef __cplusplus
extern "C" {
#endif

void fs_api_set_filesystem_app(FilesystemApp* filesystem_app);

bool fs_api_file_open(
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode);
bool fs_api_file_close(File* file);
uint16_t fs_api_file_read(File* file, void* buff, uint16_t bytes_to_read);
uint16_t fs_api_file_write(File* file, const void* buff, uint16_t bytes_to_write);
bool fs_api_file_seek(File* file, uint32_t offset, bool from_start);
uint64_t fs_api_file_tell(File* file);
bool fs_api_file_truncate(File* file);
uint64_t fs_api_file_size(File* file);
bool fs_api_file_sync(File* file);
bool fs_api_file_eof(File* file);

bool fs_api_dir_open(File* file, const char* path);
bool fs_api_dir_close(File* file);
bool fs_api_dir_read(File* file, FileInfo* fileinfo, char* name, uint16_t name_length);
bool fs_api_dir_rewind(File* file);

FS_Error fs_api_common_info(
    const char* path,
    FileInfo* fileinfo,
    char* name,
    const uint16_t name_length);
FS_Error fs_api_common_remove(const char* path);
FS_Error fs_api_common_rename(const char* old_path, const char* new_path);
FS_Error fs_api_common_mkdir(const char* path);
FS_Error fs_api_common_get_fs_info(uint64_t* total_space, uint64_t* free_space);

const char* fs_api_error_get_desc(FS_Error error_id);
const char* fs_api_error_get_internal_desc(uint32_t internal_error_id);

#ifdef __cplusplus
}
#endif