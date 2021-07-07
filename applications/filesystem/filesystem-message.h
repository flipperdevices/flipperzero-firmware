#pragma once
#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FILESYSTEM_THREAD_FLAG_COMPLETE (1)

typedef struct {
    File* file;
    const char* path;
    FS_AccessMode access_mode;
    FS_OpenMode open_mode;
} FSADataFOpen;

typedef struct {
    File* file;
    void* buff;
    uint16_t bytes_to_read;
} FSADataFRead;

typedef struct {
    File* file;
    const void* buff;
    uint16_t bytes_to_write;
} FSADataFWrite;

typedef struct {
    File* file;
    uint32_t offset;
    bool from_start;
} FSADataFSeek;

typedef struct {
    File* file;
    const char* path;
} FSADataDOpen;

typedef struct {
    File* file;
    FileInfo* fileinfo;
    char* name;
    uint16_t name_length;
} FSADataDRead;

typedef struct {
    const char* path;
    FileInfo* fileinfo;
    char* name;
    const uint16_t name_length;
} FSADataCInfo;

typedef struct {
    const char* old_path;
    const char* new_path;
} FSADataCRename;

typedef struct {
    uint32_t id;
} FSADataError;

typedef struct {
    const char* path;
} FSADataPath;

typedef struct {
    File* file;
} FSADataFile;

typedef union {
    FSADataFOpen fopen;
    FSADataFRead fread;
    FSADataFWrite fwrite;
    FSADataFSeek fseek;

    FSADataDOpen dopen;
    FSADataDRead dread;

    FSADataCInfo cinfo;
    FSADataCRename crename;

    FSADataError error;

    FSADataFile file;
    FSADataPath path;
} FSAData;

typedef union {
    bool bool_value;
    uint16_t uint16_value;
    uint64_t uint64_value;
    FS_Error error_value;
    const char* cstring_value;
} FSAReturn;

typedef enum {
    FC_FILE_OPEN,
    FC_FILE_CLOSE,
    FC_FILE_READ,
    FC_FILE_WRITE,
    FC_FILE_SEEK,
    FC_FILE_TELL,
    FC_FILE_TRUNCATE,
    FC_FILE_SIZE,
    FC_FILE_SYNC,
    FC_FILE_EOF,
    FC_DIR_OPEN,
    FC_DIR_CLOSE,
    FC_DIR_READ,
    FC_DIR_REWIND,
    FC_COMMON_INFO,
    FC_COMMON_REMOVE,
    FC_COMMON_RENAME,
    FC_COMMON_MKDIR,
    FC_GET_FS_INFO,
    FC_ERROR_GET_DESC,
    FC_ERROR_GET_INTERNAL_DESC
} FilesystemCommand;

typedef struct {
    osThreadId thread;
    FilesystemCommand command;
    FSAData* data;
    FSAReturn* return_data;
} FilesystemMessage;

#ifdef __cplusplus
}
#endif