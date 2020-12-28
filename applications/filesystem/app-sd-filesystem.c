#include "fatfs.h"
#include "app-filesystem-api.h"

#define SD_FS_MAX_FILES _FS_LOCK

typedef FIL SDFile;
typedef DIR SDDir;
typedef FILINFO SDFileInfo;

typedef enum {
    SD_OK = FR_OK,
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
} SDErrors;

typedef enum {
    FDF_DIR,
    FDF_FILE,
    FDF_ANY,
} FiledataFilter;

typedef struct {
    osThreadId_t thread_id;
    bool is_dir;
    union {
        SDFile file;
        SDDir dir;
    } data;
} FileData;

osMutexId_t sd_fs_mutex;
FileData sd_fs_files[SD_FS_MAX_FILES];
bool sd_fs_status = false;

/******************* Core Functions *******************/

bool _fs_init() {
    bool result = true;
    sd_fs_mutex = osMutexCreate(NULL);
    if(sd_fs_mutex == NULL) result = false;

    for(uint8_t i = 0; i < SD_FS_MAX_FILES; i++) {
        sd_fs_files[i].thread_id = NULL;
    }

    return result;
}
bool _fs_lock() {
    return (osMutexAcquire(sd_fs_mutex, osWaitForever) == osOK);
}

bool _fs_unlock() {
    return (osMutexRelease(sd_fs_mutex) == osOK);
}

SDErrors _get_filedata(File* file, FileData* filedata, FiledataFilter filter) {
    _fs_lock();
    SDErrors error;

    if(sd_fs_status) {
        if(file == NULL || file->file_id >= SD_FS_MAX_FILES) {
            if(sd_fs_files[file->file_id].thread_id == osThreadGetId()) {
                if(filter == FDF_ANY) {
                    // any type
                    filedata = &sd_fs_files[file->file_id];
                } else if(filter == FDF_FILE) {
                    // file type
                    if(!sd_fs_files[file->file_id].is_dir) {
                        filedata = &sd_fs_files[file->file_id];
                    } else {
                        error = SD_NOT_A_FILE;
                    }
                } else if(filter == FDF_DIR) {
                    // dir type
                    if(sd_fs_files[file->file_id].is_dir) {
                        filedata = &sd_fs_files[file->file_id];
                    } else {
                        error = SD_NOT_A_DIR;
                    }
                }
            } else {
                error = SD_OTHER_APP;
            }
        } else {
            error = SD_INVALID_PARAMETER;
        }
    } else {
        error = SD_NO_CARD;
    }

    _fs_unlock();
    return error;
}

SDErrors _get_file(File* file, FileData* filedata) {
    return _get_filedata(file, filedata, FDF_FILE);
}

SDErrors _get_dir(File* file, FileData* filedata) {
    return _get_filedata(file, filedata, FDF_DIR);
}

SDErrors _get_any(File* file, FileData* filedata) {
    return _get_filedata(file, filedata, FDF_ANY);
}

bool _fs_status(void) {
    bool result;

    _fs_lock();
    result = sd_fs_status;
    _fs_unlock();

    return result;
}

void _fs_on_client_app_exit(void* none) {
    _fs_lock();
    for(uint8_t i = 0; i < SD_FS_MAX_FILES; i++) {
        if(sd_fs_files[i].thread_id == osThreadGetId()) {
            if(sd_fs_files[i].is_dir) {
                // TODO close dir
            } else {
                // TODO close file
            }
        }
    }
    _fs_unlock();
}

FS_Error _fs_parse_error(SDErrors error) {
    FS_Error result;
    switch(error) {
    case SD_OK:
        result = FSE_OK;
        break;
    case SD_INT_ERR:
        result = FSE_INTERNAL;
        break;
    case SD_NO_FILE:
        result = FSE_NOT_EXIST;
        break;
    case SD_NO_PATH:
        result = FSE_NOT_EXIST;
        break;
    case SD_INVALID_NAME:
        result = FSE_INVALID_NAME;
        break;
    case SD_DENIED:
        result = FSE_DENIED;
        break;
    case SD_EXIST:
        result = FSE_EXIST;
        break;
    case SD_INVALID_OBJECT:
        result = FSE_INTERNAL;
        break;
    case SD_WRITE_PROTECTED:
        result = FSE_INTERNAL;
        break;
    case SD_INVALID_DRIVE:
        result = FSE_INTERNAL;
        break;
    case SD_NOT_ENABLED:
        result = FSE_INTERNAL;
        break;
    case SD_NO_FILESYSTEM:
        result = FSE_NOT_READY;
        break;
    case SD_MKFS_ABORTED:
        result = FSE_INTERNAL;
        break;
    case SD_TIMEOUT:
        result = FSE_INTERNAL;
        break;
    case SD_LOCKED:
        result = FSE_INTERNAL;
        break;
    case SD_NOT_ENOUGH_CORE:
        result = FSE_INTERNAL;
        break;
    case SD_TOO_MANY_OPEN_FILES:
        result = FSE_INTERNAL;
        break;
    case SD_INVALID_PARAMETER:
        result = FSE_INVALID_PARAMETER;
        break;
    case SD_NO_CARD:
        result = FSE_NOT_READY;
        break;
    case SD_NOT_A_FILE:
        result = FSE_INVALID_PARAMETER;
        break;
    case SD_NOT_A_DIR:
        result = FSE_INVALID_PARAMETER;
        break;
    case SD_OTHER_APP:
        result = FSE_INTERNAL;
        break;

    default:
        result = FSE_INTERNAL;
        break;
    }

    return result;
}

/******************* File Functions *******************/

// Open/Create a file
bool fs_file_open(File* file, const char* path, FS_Flags mode) {
    SDFile* sd_file = NULL;

    _fs_lock();
    for(uint8_t index = 0; index < SD_FS_MAX_FILES; index++) {
        if(sd_fs_files[index].thread_id == NULL) {
            memset(&(sd_fs_files[index].data), 0, sizeof(sd_fs_files[index].data));
            sd_fs_files[index].thread_id = osThreadGetId();
            sd_fs_files[index].is_dir = false;
            sd_file = &(sd_fs_files[index].data.file);

            break;
        }
    }
    _fs_unlock();

    if(sd_file == NULL) {
        file->error_id = SD_TOO_MANY_OPEN_FILES;
    } else {
        uint8_t _mode = 0;

        if(mode & FSM_READ) _mode |= FA_READ;
        if(mode & FSM_WRITE) _mode |= FA_WRITE;
        if(mode & FSM_OPEN_EXISTING) _mode |= FA_OPEN_EXISTING;
        if(mode & FSM_OPEN_ALWAYS) _mode |= FA_OPEN_ALWAYS;
        if(mode & FSM_OPEN_APPEND) _mode |= FA_OPEN_APPEND;
        if(mode & FSM_CREATE_NEW) _mode |= FA_CREATE_NEW;
        if(mode & FSM_CREATE_ALWAYS) _mode |= FA_CREATE_ALWAYS;

        if(file->error_id == SD_OK) file->error_id = f_open(sd_file, path, _mode);
    }

    // TODO on exit
    //furiac_onexit(_fs_on_client_app_exit, NULL);

    return (file->error_id == SD_OK);
}

// Close an opened file
bool fs_file_close(File* file) {
    FileData* filedata = NULL;
    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        file->error_id = f_close(&filedata->data.file);

        _fs_lock();
        filedata->thread_id = NULL;
        _fs_unlock();
    }

    return (file->error_id == SD_OK);
}

// Read data from the file
uint16_t fs_file_read(File* file, void* buff, uint16_t const bytes_to_read) {
    FileData* filedata = NULL;
    uint16_t bytes_readed = 0;

    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        file->error_id = f_read(&filedata->data.file, buff, bytes_to_read, &bytes_readed);
    }

    return bytes_readed;
}

// Write data to the file
uint16_t fs_file_write(File* file, void* buff, uint16_t const bytes_to_write) {
    FileData* filedata = NULL;
    uint16_t bytes_written = 0;

    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        file->error_id = f_write(&filedata->data.file, buff, bytes_to_write, &bytes_written);
    }

    return bytes_written;
}

// Move read/write pointer, expand size
bool fs_file_seek(File* file, const uint32_t offset, const bool from_start) {
    FileData* filedata = NULL;

    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        if(from_start) {
            file->error_id = f_lseek(&filedata->data.file, offset);
        } else {
            uint64_t position = f_tell(&filedata->data.file);
            position += offset;
            file->error_id = f_lseek(&filedata->data.file, position);
        }
    }

    return (file->error_id == SD_OK);
}

uint64_t fs_file_tell(File* file) {
    FileData* filedata = NULL;
    uint64_t position = 0;
    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        position = f_tell(&filedata->data.file);
    }

    return position;
}

// Truncate file size to current pointer value
bool fs_file_truncate(File* file) {
    FileData* filedata = NULL;
    uint64_t position = 0;

    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        file->error_id = f_truncate(&filedata->data.file);
    }

    return (file->error_id == SD_OK);
}

// Flush cached data
bool fs_file_sync(File* file) {
    FileData* filedata = NULL;

    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        file->error_id = f_sync(&filedata->data.file);
    }

    return (file->error_id == SD_OK);
}

// Get size
uint64_t fs_file_size(File* file) {
    FileData* filedata = NULL;
    uint64_t size = 0;
    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        size = f_size(&filedata->data.file);
    }

    return size;
}

// Test EOF
bool fs_file_eof(File* file) {
    FileData* filedata = NULL;
    bool eof = true;
    file->error_id = _get_file(file, filedata);

    if(file->error_id == SD_OK) {
        eof = f_eof(&filedata->data.file);
    }

    return eof;
}

/******************* Dir Functions *******************/

// Open directory
bool fs_dir_open(File* file, const char* path) {
    SDDir* sd_dir = NULL;

    _fs_lock();
    for(uint8_t index = 0; index < SD_FS_MAX_FILES; index++) {
        if(sd_fs_files[index].thread_id == NULL) {
            memset(&(sd_fs_files[index].data), 0, sizeof(sd_fs_files[index].data));
            sd_fs_files[index].thread_id = osThreadGetId();
            sd_fs_files[index].is_dir = true;
            sd_dir = &(sd_fs_files[index].data.dir);

            break;
        }
    }
    _fs_unlock();

    if(sd_dir == NULL) {
        file->error_id = SD_TOO_MANY_OPEN_FILES;
    } else {
        if(file->error_id == SD_OK) file->error_id = f_opendir(sd_dir, path);
    }

    // TODO on exit
    //furiac_onexit(_fs_on_client_app_exit, NULL);

    return (file->error_id == SD_OK);
}

// Close directory
bool fs_dir_close(File* file) {
    FileData* filedata = NULL;
    file->error_id = _get_dir(file, filedata);

    if(file->error_id == SD_OK) {
        file->error_id = f_closedir(&filedata->data.dir);

        _fs_lock();
        filedata->thread_id = NULL;
        _fs_unlock();
    }

    return (file->error_id == SD_OK);
}

// Read next file info and name from directory
bool fs_dir_read(File* file, FileInfo* fileinfo, char* name, const uint16_t name_length) {
    FileData* filedata = NULL;
    file->error_id = _get_dir(file, filedata);

    if(file->error_id == SD_OK) {
        SDFileInfo _fileinfo;
        file->error_id = f_readdir(&filedata->data.dir, &_fileinfo);

        if(fileinfo != NULL) {
            fileinfo->date = _fileinfo.fdate;
            fileinfo->time = _fileinfo.ftime;
            fileinfo->size = _fileinfo.fsize;
            fileinfo->flags = 0;

            if(_fileinfo.fattrib & AM_RDO) fileinfo->flags |= FSF_READ_ONLY;
            if(_fileinfo.fattrib & AM_HID) fileinfo->flags |= FSF_HIDDEN;
            if(_fileinfo.fattrib & AM_SYS) fileinfo->flags |= FSF_SYSTEM;
            if(_fileinfo.fattrib & AM_DIR) fileinfo->flags |= FSF_DIRECTORY;
            if(_fileinfo.fattrib & AM_ARC) fileinfo->flags |= FSF_ARCHIVE;
        }

        if(name != NULL && name_length > 0) {
            strncpy(name, _fileinfo.fname, name_length);
        }
    }

    return (file->error_id == SD_OK);
}

bool fs_dir_rewind(File* file) {
    FileData* filedata = NULL;
    file->error_id = _get_dir(file, filedata);

    if(file->error_id == SD_OK) {
        file->error_id = f_readdir(&filedata->data.dir, NULL);
    }

    return (file->error_id == SD_OK);
}

/******************* Common FS Functions *******************/

// Get info about file/dir
FS_Error
fs_common_info(const char* path, FileInfo* fileinfo, char* name, const uint16_t name_length) {
    SDFileInfo _fileinfo;
    FRESULT fresult = f_stat(path, &_fileinfo);

    if(fresult == FR_OK) {
        if(fileinfo != NULL) {
            fileinfo->date = _fileinfo.fdate;
            fileinfo->time = _fileinfo.ftime;
            fileinfo->size = _fileinfo.fsize;
            fileinfo->flags = 0;

            if(_fileinfo.fattrib & AM_RDO) fileinfo->flags |= FSF_READ_ONLY;
            if(_fileinfo.fattrib & AM_HID) fileinfo->flags |= FSF_HIDDEN;
            if(_fileinfo.fattrib & AM_SYS) fileinfo->flags |= FSF_SYSTEM;
            if(_fileinfo.fattrib & AM_DIR) fileinfo->flags |= FSF_DIRECTORY;
            if(_fileinfo.fattrib & AM_ARC) fileinfo->flags |= FSF_ARCHIVE;
        }

        if(name != NULL && name_length > 0) {
            strncpy(name, _fileinfo.fname, name_length);
        }
    }

    return _fs_parse_error(fresult);
}

// Delete file/dir
// File/dir must not have read-only attribute.
// File/dir must be empty.
// File/dir must not be opened, or the FAT volume can be collapsed. FF_FS_LOCK fix that.
FS_Error fs_common_delete(const char* path) {
    FRESULT fresult = f_unlink(path);
    return _fs_parse_error(fresult);
}

// Rename file/dir
// File/dir must not be opened, or the FAT volume can be collapsed. FF_FS_LOCK fix that.
FS_Error fs_common_rename(const char* old_path, const char* new_path) {
    FRESULT fresult = f_rename(old_path, new_path);
    return _fs_parse_error(fresult);
}

// Set attributes of file/dir
// For example:
// set "read only" flag and remove "hidden" flag
// fs_common_set_attr("file.txt", FSF_READ_ONLY, FSF_READ_ONLY | FSF_HIDDEN);
FS_Error fs_common_set_attr(const char* path, uint8_t attr, uint8_t mask) {
    uint8_t _mask = 0;
    uint8_t _attr = 0;

    if(mask & FSF_READ_ONLY) _mask |= AM_RDO;
    if(mask & FSF_HIDDEN) _mask |= AM_HID;
    if(mask & FSF_SYSTEM) _mask |= AM_SYS;
    if(mask & FSF_DIRECTORY) _mask |= AM_DIR;
    if(mask & FSF_ARCHIVE) _mask |= AM_ARC;

    if(attr & FSF_READ_ONLY) _attr |= AM_RDO;
    if(attr & FSF_HIDDEN) _attr |= AM_HID;
    if(attr & FSF_SYSTEM) _attr |= AM_SYS;
    if(attr & FSF_DIRECTORY) _attr |= AM_DIR;
    if(attr & FSF_ARCHIVE) _attr |= AM_ARC;

    FRESULT fresult = f_chmod(path, attr, mask);
    return _fs_parse_error(fresult);
}

// Set time of file/dir
FS_Error fs_common_set_time(
    const char* path,
    uint16_t year,
    uint8_t month,
    uint8_t month_day,
    uint8_t hour,
    uint8_t minute,
    uint8_t second) {
    SDFileInfo _fileinfo;

    _fileinfo.fdate = (WORD)(((year - 1980) * 512U) | month * 32U | month_day);
    _fileinfo.ftime = (WORD)(hour * 2048U | minute * 32U | second / 2U);

    FRESULT fresult = f_utime(path, &_fileinfo);
    return _fs_parse_error(fresult);
}

// Create new directory
FS_Error fs_common_mkdir(const char* path) {
    FRESULT fresult = f_mkdir(path);
    return _fs_parse_error(fresult);
}

void app_filesystem(void* p) {
    FS_Api fs_api;

    if(!_fs_init()) {
        // TODO stop app
    }

    // fill file api
    fs_api.file.open = fs_file_open;
    fs_api.file.close = fs_file_close;
    fs_api.file.read = fs_file_read;
    fs_api.file.write = fs_file_write;
    fs_api.file.seek = fs_file_seek;
    fs_api.file.tell = fs_file_tell;
    fs_api.file.truncate = fs_file_truncate;
    fs_api.file.size = fs_file_size;
    fs_api.file.sync = fs_file_sync;
    fs_api.file.eof = fs_file_eof;

    // fill dir api
    fs_api.dir.open = fs_dir_open;
    fs_api.dir.close = fs_dir_close;
    fs_api.dir.read = fs_dir_read;
    fs_api.dir.rewind = fs_dir_rewind;
}