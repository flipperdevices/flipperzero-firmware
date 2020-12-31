#pragma once
#include "flipper.h"
#include "flipper_v2.h"

/*
Open mode & Access flags

Flag                Meaning
FSM_READ             Read access.
FSM_WRITE            Write access.
FSM_OPEN_EXISTING    Open file, fail if file doesn't exist.
FSM_OPEN_ALWAYS      Open file. If file not exist, a new file will be created. 
FSM_OPEN_APPEND      Open file. If file not exist, a new file will be created. File pointer will be set to the end of file.
FSM_CREATE_NEW       Creates a new file. Fails if the file is exist.
FSM_CREATE_ALWAYS    Creates a new file. If the file is exist, it will be truncated to zero size.
*/
typedef enum {
    FSM_READ = (1 << 0),
    FSM_WRITE = (1 << 1),
    FSM_OPEN_EXISTING = (1 << 2),
    FSM_OPEN_ALWAYS = (1 << 3),
    FSM_OPEN_APPEND = (1 << 4),
    FSM_CREATE_NEW = (1 << 5),
    FSM_CREATE_ALWAYS = (1 << 6),
} FS_Mode;

/* fs api errors */
typedef enum {
    FSE_OK,
    FSE_NOT_READY,
    FSE_EXIST,
    FSE_NOT_EXIST,
    FSE_INVALID_PARAMETER,
    FSE_DENIED,
    FSE_INVALID_NAME,
    FSE_INTERNAL,
    FSE_NOT_IMPLEMENTED,
} FS_Error;

/* FS fileinfo flags*/
typedef enum {
    FSF_READ_ONLY = (1 << 0),
    FSF_HIDDEN = (1 << 1),
    FSF_SYSTEM = (1 << 2),
    FSF_DIRECTORY = (1 << 3),
    FSF_ARCHIVE = (1 << 4),
} FS_Flags;

/* file/dir data */
typedef struct {
    /* file ID for internal references */
    uint32_t file_id;
    /* standart API error from FS_Error list */
    FS_Error error_id;
    /* internal API error */
    uint32_t internal_error_id;
} File;

typedef struct {
    uint8_t flags;
    uint64_t size;
    uint16_t date;
    uint16_t time;
} FileInfo;

/* file api */
typedef struct {
    bool (*open)(File* file, const char* path, FS_Flags mode);
    bool (*close)(File* file);
    uint16_t (*read)(File* file, void* buff, uint16_t bytes_to_read);
    uint16_t (*write)(File* file, void* buff, uint16_t bytes_to_write);
    bool (*seek)(File* file, uint32_t offset, bool from_start);
    uint64_t (*tell)(File* file);
    bool (*truncate)(File* file);
    uint64_t (*size)(File* file);
    bool (*sync)(File* file);
    bool (*eof)(File* file);
} FS_File_Api;

/* dir api */
typedef struct {
    bool (*open)(File* file, const char* path);
    bool (*close)(File* file);
    bool (*read)(File* file, FileInfo* fileinfo, char* name, uint16_t name_length);
    bool (*rewind)(File* file);
} FS_Dir_Api;

/* common api */
typedef struct {
    FS_Error (*info)(const char* path, FileInfo* fileinfo, char* name, const uint16_t name_length);
    FS_Error (*delete)(const char* path);
    FS_Error (*rename)(const char* old_path, const char* new_path);
    FS_Error (*set_attr)(const char* path, uint8_t attr, uint8_t mask);
    FS_Error (*mkdir)(const char* path);
    FS_Error (*set_time)(
        const char* path,
        uint16_t year,
        uint8_t month,
        uint8_t month_day,
        uint8_t hour,
        uint8_t minute,
        uint8_t second);
    FS_Error (*get_fs_info)(uint64_t* total_space, uint64_t* free_space);
} FS_Common_Api;

/* errors api */
typedef struct {
    const char* (*get_desc)(FS_Error error_id);
    const char* (*get_internal_desc)(uint32_t internal_error_id);
} FS_Error_Api;

/* full api set */
typedef struct {
    FS_File_Api file;
    FS_Dir_Api dir;
    FS_Common_Api common;
    FS_Error_Api error;
} FS_Api;