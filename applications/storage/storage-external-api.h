#pragma once
#include <furi.h>
#include "storage-i.h"

#ifdef __cplusplus
extern "C" {
#endif

bool s_api_file_open(
    void* context,
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode);
bool s_api_file_close(void* context, File* file);
uint16_t s_api_file_read(void* context, File* file, void* buff, uint16_t bytes_to_read);
uint16_t s_api_file_write(void* context, File* file, const void* buff, uint16_t bytes_to_write);
bool s_api_file_seek(void* context, File* file, uint32_t offset, bool from_start);
uint64_t s_api_file_tell(void* context, File* file);
bool s_api_file_truncate(void* context, File* file);
uint64_t s_api_file_size(void* context, File* file);
bool s_api_file_sync(void* context, File* file);
bool s_api_file_eof(void* context, File* file);

bool s_api_dir_open(void* context, File* file, const char* path);
bool s_api_dir_close(void* context, File* file);
bool s_api_dir_read(void* context, File* file, FileInfo* fileinfo, char* name, uint16_t name_length);
bool s_api_dir_rewind(void* context, File* file);

FS_Error s_api_common_stat(void* context, const char* path, FileInfo* fileinfo);
FS_Error s_api_common_remove(void* context, const char* path);
FS_Error s_api_common_rename(void* context, const char* old_path, const char* new_path);
FS_Error s_api_common_mkdir(void* context, const char* path);
FS_Error s_api_common_fs_info(
    void* context,
    const char* fs_path,
    uint64_t* total_space,
    uint64_t* free_space);

const char* s_api_error_get_desc(void* context, FS_Error error_id);
const char* s_api_error_get_internal_desc(void* context, uint32_t internal_error_id);

#ifdef __cplusplus
}
#endif