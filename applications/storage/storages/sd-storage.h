#pragma once
#include <furi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SD_FS_MAX_FILES _FS_LOCK

typedef FIL SDFile;
typedef DIR SDDir;
typedef FILINFO SDFileInfo;

typedef union {
    SDFile file;
    SDDir dir;
} SDFileDirStorage;

typedef enum {
    SD_OK = FR_OK,
    SD_DISK_ERR = FR_DISK_ERR,
    SD_INT_ERR = FR_INT_ERR,
    SD_NO_FILE = FR_NO_FILE,
    SD_NO_PATH = FR_NO_PATH,
    SD_INVALID_NAME = FR_INVALID_NAME,
    SD_DENIED = FR_DENIED,
    SD_EXIST = FR_EXIST,
    SD_INVALID_OBJECT = FR_INVALID_OBJECT,
    SD_WRITE_PROTECTED = FR_WRITE_PROTECTED,
    SD_INVALID_DRIVE = FR_INVALID_DRIVE,
    SD_NOT_ENABLED = FR_NOT_ENABLED,
    SD_NO_FILESYSTEM = FR_NO_FILESYSTEM,
    SD_MKFS_ABORTED = FR_MKFS_ABORTED,
    SD_TIMEOUT = FR_TIMEOUT,
    SD_LOCKED = FR_LOCKED,
    SD_NOT_ENOUGH_CORE = FR_NOT_ENOUGH_CORE,
    SD_TOO_MANY_OPEN_FILES = FR_TOO_MANY_OPEN_FILES,
    SD_INVALID_PARAMETER = FR_INVALID_PARAMETER,
    SD_NO_CARD,
    SD_NOT_A_FILE,
    SD_NOT_A_DIR,
    SD_OTHER_APP,
    SD_LOW_LEVEL_ERR,
} SDError;

typedef enum {
    FDF_DIR,
    FDF_FILE,
    FDF_ANY,
} FiledataFilter;

typedef struct {
    osThreadId_t thread_id;
    bool is_dir;
    SDFileDirStorage data;
} FileData;

typedef struct {
    osMutexId_t mutex;
    FileData files[SD_FS_MAX_FILES];
    SDError status;
    char* path;
    FATFS fat_fs;
} SdFsInfo;

#ifdef __cplusplus
}
#endif