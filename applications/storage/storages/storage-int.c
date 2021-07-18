#include "storage-int.h"
#include <lfs.h>
#include <api-hal.h>

#define TAG "storage-int"
#define STORAGE_PATH "/int"

typedef struct {
    const size_t start_address;
    const size_t start_page;
    struct lfs_config config;
    lfs_t lfs;
} LFSData;

/* INTERNALS */

static int storage_int_device_read(
    const struct lfs_config* c,
    lfs_block_t block,
    lfs_off_t off,
    void* buffer,
    lfs_size_t size) {
    LFSData* lfs_data = c->context;
    size_t address = lfs_data->start_address + block * c->block_size + off;

    FURI_LOG_D(
        TAG,
        "Device read: block %d, off %d, buffer: %p, size %d, translated address: %p",
        block,
        off,
        buffer,
        size,
        address);

    memcpy(buffer, (void*)address, size);

    return 0;
}

static int storage_int_device_prog(
    const struct lfs_config* c,
    lfs_block_t block,
    lfs_off_t off,
    const void* buffer,
    lfs_size_t size) {
    LFSData* lfs_data = c->context;
    size_t address = lfs_data->start_address + block * c->block_size + off;

    FURI_LOG_D(
        TAG,
        "Device prog: block %d, off %d, buffer: %p, size %d, translated address: %p",
        block,
        off,
        buffer,
        size,
        address);

    int ret = 0;
    while(size > 0) {
        if(!api_hal_flash_write_dword(address, *(uint64_t*)buffer)) {
            ret = -1;
            break;
        }
        address += c->prog_size;
        buffer += c->prog_size;
        size -= c->prog_size;
    }

    return ret;
}

static int storage_int_device_erase(const struct lfs_config* c, lfs_block_t block) {
    LFSData* lfs_data = c->context;
    size_t page = lfs_data->start_page + block;

    FURI_LOG_D(TAG, "Device erase: page %d, translated page: %d", block, page);

    if(api_hal_flash_erase(page, 1)) {
        return 0;
    } else {
        return -1;
    }
}

static int storage_int_device_sync(const struct lfs_config* c) {
    FURI_LOG_D(TAG, "Device sync: skipping, cause ");
    return 0;
}

static LFSData* storage_int_lfs_data_alloc() {
    LFSData* lfs_data = furi_alloc(sizeof(LFSData));

    // Internal storage start address
    *(size_t*)(&lfs_data->start_address) = api_hal_flash_get_free_page_start_address();
    *(size_t*)(&lfs_data->start_page) =
        (lfs_data->start_address - api_hal_flash_get_base()) / api_hal_flash_get_page_size();

    // LFS configuration
    // Glue and context
    lfs_data->config.context = lfs_data;
    lfs_data->config.read = storage_int_device_read;
    lfs_data->config.prog = storage_int_device_prog;
    lfs_data->config.erase = storage_int_device_erase;
    lfs_data->config.sync = storage_int_device_sync;

    // Block device description
    lfs_data->config.read_size = api_hal_flash_get_read_block_size();
    lfs_data->config.prog_size = api_hal_flash_get_write_block_size();
    lfs_data->config.block_size = api_hal_flash_get_page_size();
    lfs_data->config.block_count = api_hal_flash_get_free_page_count();
    lfs_data->config.block_cycles = api_hal_flash_get_cycles_count();
    lfs_data->config.cache_size = 16;
    lfs_data->config.lookahead_size = 16;

    return lfs_data;
};

static void storage_int_lfs_mount(LFSData* lfs_data, StorageData* storage) {
    int err;
    ApiHalBootFlag boot_flags = api_hal_boot_get_flags();
    if(boot_flags & ApiHalBootFlagFactoryReset) {
        // Factory reset
        err = lfs_format(&lfs_data->lfs, &lfs_data->config);
        if(err == 0) {
            FURI_LOG_I(TAG, "Factory reset: Format successful, trying to mount");
            api_hal_boot_set_flags(boot_flags & ~ApiHalBootFlagFactoryReset);
            err = lfs_mount(&lfs_data->lfs, &lfs_data->config);
            if(err == 0) {
                FURI_LOG_I(TAG, "Factory reset: Mounted");
                storage->status = StorageStatusOK;
            } else {
                FURI_LOG_E(TAG, "Factory reset: Mount after format failed");
                storage->status = StorageStatusNotMounted;
            }
        } else {
            FURI_LOG_E(TAG, "Factory reset: Format failed");
            storage->status = StorageStatusNoFS;
        }
    } else {
        // Normal
        err = lfs_mount(&lfs_data->lfs, &lfs_data->config);
        if(err == 0) {
            FURI_LOG_I(TAG, "Mounted");
            storage->status = StorageStatusOK;
        } else {
            FURI_LOG_E(TAG, "Mount failed, formatting");
            err = lfs_format(&lfs_data->lfs, &lfs_data->config);
            if(err == 0) {
                FURI_LOG_I(TAG, "Format successful, trying to mount");
                err = lfs_mount(&lfs_data->lfs, &lfs_data->config);
                if(err == 0) {
                    FURI_LOG_I(TAG, "Mounted");
                    storage->status = StorageStatusOK;
                } else {
                    FURI_LOG_E(TAG, "Mount after format failed");
                    storage->status = StorageStatusNotMounted;
                }
            } else {
                FURI_LOG_E(TAG, "Format failed");
                storage->status = StorageStatusNoFS;
            }
        }
    }
}

/****************** Common Functions ******************/

static FS_Error storage_int_parse_error(int error) {
    FS_Error result = FSE_INTERNAL;

    if(error >= LFS_ERR_OK) {
        result = FSE_OK;
    } else {
        switch(error) {
        case LFS_ERR_IO:
            result = FSE_INTERNAL;
            break;
        case LFS_ERR_CORRUPT:
            result = FSE_INTERNAL;
            break;
        case LFS_ERR_NOENT:
            result = FSE_NOT_EXIST;
            break;
        case LFS_ERR_EXIST:
            result = FSE_EXIST;
            break;
        case LFS_ERR_NOTDIR:
            result = FSE_INVALID_NAME;
            break;
        case LFS_ERR_ISDIR:
            result = FSE_INVALID_NAME;
            break;
        case LFS_ERR_NOTEMPTY:
            result = FSE_DENIED;
            break;
        case LFS_ERR_BADF:
            result = FSE_INVALID_NAME;
            break;
        case LFS_ERR_FBIG:
            result = FSE_INTERNAL;
            break;
        case LFS_ERR_INVAL:
            result = FSE_INVALID_PARAMETER;
            break;
        case LFS_ERR_NOSPC:
            result = FSE_INTERNAL;
            break;
        case LFS_ERR_NOMEM:
            result = FSE_INTERNAL;
            break;
        case LFS_ERR_NOATTR:
            result = FSE_INVALID_PARAMETER;
            break;
        case LFS_ERR_NAMETOOLONG:
            result = FSE_INVALID_NAME;
            break;
        default:
            break;
        }
    }

    return result;
}

/******************* File Functions *******************/

static bool storage_int_file_open(
    void* ctx,
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;

    int flags = 0;

    if(access_mode & FSAM_READ) flags |= LFS_O_RDONLY;
    if(access_mode & FSAM_WRITE) flags |= LFS_O_WRONLY;

    if(open_mode & FSOM_OPEN_EXISTING) flags = flags;
    if(open_mode & FSOM_OPEN_ALWAYS) flags |= LFS_O_CREAT;
    if(open_mode & FSOM_OPEN_APPEND) flags |= LFS_O_CREAT | LFS_O_APPEND;
    if(open_mode & FSOM_CREATE_NEW) flags |= LFS_O_CREAT | LFS_O_EXCL;
    if(open_mode & FSOM_CREATE_ALWAYS) flags |= LFS_O_CREAT | LFS_O_TRUNC;

    lfs_file_t* file_data = malloc(sizeof(lfs_file_t));
    storage_set_storage_file_data(file, file_data, storage);
    file->internal_error_id = lfs_file_open(&lfs_data->lfs, file_data, path, flags);
    file->error_id = storage_int_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static bool storage_int_file_close(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = lfs_file_close(&lfs_data->lfs, file_data);
    file->error_id = storage_int_parse_error(file->internal_error_id);
    free(file_data);
    return (file->error_id == FSE_OK);
}

static uint16_t
    storage_int_file_read(void* ctx, File* file, void* buff, uint16_t const bytes_to_read) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    uint16_t bytes_readed = 0;
    file->internal_error_id = lfs_file_read(&lfs_data->lfs, file_data, buff, bytes_to_read);
    file->error_id = storage_int_parse_error(file->internal_error_id);

    if(file->error_id == FSE_OK) {
        bytes_readed = file->internal_error_id;
        file->internal_error_id = 0;
    }
    return bytes_readed;
}

static uint16_t
    storage_int_file_write(void* ctx, File* file, const void* buff, uint16_t const bytes_to_write) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    uint16_t bytes_written = 0;
    file->internal_error_id = lfs_file_write(&lfs_data->lfs, file_data, buff, bytes_to_write);
    file->error_id = storage_int_parse_error(file->internal_error_id);

    if(file->error_id == FSE_OK) {
        bytes_written = file->internal_error_id;
        file->internal_error_id = 0;
    }
    return bytes_written;
}

static bool
    storage_int_file_seek(void* ctx, File* file, const uint32_t offset, const bool from_start) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    if(from_start) {
        file->internal_error_id = lfs_file_seek(&lfs_data->lfs, file_data, offset, LFS_SEEK_SET);
    } else {
        file->internal_error_id = lfs_file_seek(&lfs_data->lfs, file_data, offset, LFS_SEEK_CUR);
    }

    file->error_id = storage_int_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static uint64_t storage_int_file_tell(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = lfs_file_tell(&lfs_data->lfs, file_data);
    file->error_id = storage_int_parse_error(file->internal_error_id);

    int32_t position = 0;
    if(file->error_id == FSE_OK) {
        position = file->internal_error_id;
        file->internal_error_id = 0;
    }

    return position;
}

static bool storage_int_file_truncate(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = lfs_file_tell(&lfs_data->lfs, file_data);
    file->error_id = storage_int_parse_error(file->internal_error_id);

    if(file->error_id == FSE_OK) {
        uint32_t position = file->internal_error_id;
        file->internal_error_id = lfs_file_truncate(&lfs_data->lfs, file_data, position);
        file->error_id = storage_int_parse_error(file->internal_error_id);
    }

    return (file->error_id == FSE_OK);
}

static bool storage_int_file_sync(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = lfs_file_sync(&lfs_data->lfs, file_data);
    file->error_id = storage_int_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static uint64_t storage_int_file_size(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = lfs_file_size(&lfs_data->lfs, file_data);
    file->error_id = storage_int_parse_error(file->internal_error_id);

    uint32_t size = 0;
    if(file->error_id == FSE_OK) {
        size = file->internal_error_id;
        file->internal_error_id = 0;
    }

    return size;
}

static bool storage_int_file_eof(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_file_t* file_data = storage_get_storage_file_data(file, storage);

    bool eof = false;
    int32_t position = lfs_file_tell(&lfs_data->lfs, file_data);
    int32_t size = lfs_file_size(&lfs_data->lfs, file_data);

    if(position < 0) {
        file->internal_error_id = position;
    } else if(size < 0) {
        file->internal_error_id = size;
    } else {
        file->error_id = FSE_OK;
        eof = (position >= size);
    }

    file->error_id = storage_int_parse_error(file->internal_error_id);
    return eof;
}

/******************* Dir Functions *******************/

static bool storage_int_dir_open(void* ctx, File* file, const char* path) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_dir_t* file_data = malloc(sizeof(lfs_dir_t));

    storage_set_storage_file_data(file, file_data, storage);

    file->internal_error_id = lfs_dir_open(&lfs_data->lfs, file_data, path);
    file->error_id = storage_int_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static bool storage_int_dir_close(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_dir_t* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = lfs_dir_close(&lfs_data->lfs, file_data);
    file->error_id = storage_int_parse_error(file->internal_error_id);
    free(file_data);
    return (file->error_id == FSE_OK);
}

static bool storage_int_dir_read(
    void* ctx,
    File* file,
    FileInfo* fileinfo,
    char* name,
    const uint16_t name_length) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_dir_t* file_data = storage_get_storage_file_data(file, storage);

    struct lfs_info _fileinfo;
    // LFS returns virtual directories "." and "..", so we read until we get something meaningful or an empty string
    do {
        file->internal_error_id = lfs_dir_read(&lfs_data->lfs, file_data, &_fileinfo);
        file->error_id = storage_int_parse_error(file->internal_error_id);
    } while(strcmp(_fileinfo.name, ".") == 0 || strcmp(_fileinfo.name, "..") == 0);

    if(fileinfo != NULL) {
        fileinfo->size = _fileinfo.size;
        fileinfo->flags = 0;
        if(_fileinfo.type & LFS_TYPE_DIR) fileinfo->flags |= FSF_DIRECTORY;
    }

    if(name != NULL) {
        snprintf(name, name_length, "%s", _fileinfo.name);
    }

    if(file->internal_error_id == 0) {
        file->error_id = FSE_NOT_EXIST;
    }

    return (file->error_id == FSE_OK);
}

static bool storage_int_dir_rewind(void* ctx, File* file) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    lfs_dir_t* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = lfs_dir_rewind(&lfs_data->lfs, file_data);
    file->error_id = storage_int_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

/******************* Common FS Functions *******************/

static FS_Error storage_int_common_stat(void* ctx, const char* path, FileInfo* fileinfo) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    struct lfs_info _fileinfo;
    int result = lfs_stat(&lfs_data->lfs, path, &_fileinfo);

    if(fileinfo != NULL) {
        fileinfo->size = _fileinfo.size;
        fileinfo->flags = 0;
        if(_fileinfo.type & LFS_TYPE_DIR) fileinfo->flags |= FSF_DIRECTORY;
    }

    return storage_int_parse_error(result);
}

static FS_Error storage_int_common_remove(void* ctx, const char* path) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    int result = lfs_remove(&lfs_data->lfs, path);
    return storage_int_parse_error(result);
}

static FS_Error storage_int_common_rename(void* ctx, const char* old_path, const char* new_path) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    int result = lfs_rename(&lfs_data->lfs, old_path, new_path);
    return storage_int_parse_error(result);
}

static FS_Error storage_int_common_mkdir(void* ctx, const char* path) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;
    int result = lfs_mkdir(&lfs_data->lfs, path);
    return storage_int_parse_error(result);
}

static FS_Error storage_int_common_fs_info(
    void* ctx,
    const char* fs_path,
    uint64_t* total_space,
    uint64_t* free_space) {
    StorageData* storage = ctx;
    LFSData* lfs_data = storage->data;

    *total_space = lfs_data->config.block_size * lfs_data->config.block_count;

    lfs_ssize_t result = lfs_fs_size(&lfs_data->lfs);
    if(result >= 0) {
        *free_space = *total_space - (result * lfs_data->config.block_size);
    }

    return storage_int_parse_error(result);
}

/******************* Init Storage *******************/

void storage_int_init(StorageData* storage) {
    FURI_LOG_I(TAG, "Starting");
    LFSData* lfs_data = storage_int_lfs_data_alloc();
    FURI_LOG_I(
        TAG,
        "Config: start %p, read %d, write %d, page size: %d, page count: %d, cycles: %d",
        lfs_data->start_address,
        lfs_data->config.read_size,
        lfs_data->config.prog_size,
        lfs_data->config.block_size,
        lfs_data->config.block_count,
        lfs_data->config.block_cycles);

    storage_int_lfs_mount(lfs_data, storage);

    storage->data = lfs_data;
    storage->api.tick = NULL;
    storage->fs_api.file.open = storage_int_file_open;
    storage->fs_api.file.close = storage_int_file_close;
    storage->fs_api.file.read = storage_int_file_read;
    storage->fs_api.file.write = storage_int_file_write;
    storage->fs_api.file.seek = storage_int_file_seek;
    storage->fs_api.file.tell = storage_int_file_tell;
    storage->fs_api.file.truncate = storage_int_file_truncate;
    storage->fs_api.file.size = storage_int_file_size;
    storage->fs_api.file.sync = storage_int_file_sync;
    storage->fs_api.file.eof = storage_int_file_eof;

    storage->fs_api.dir.open = storage_int_dir_open;
    storage->fs_api.dir.close = storage_int_dir_close;
    storage->fs_api.dir.read = storage_int_dir_read;
    storage->fs_api.dir.rewind = storage_int_dir_rewind;

    storage->fs_api.common.stat = storage_int_common_stat;
    storage->fs_api.common.mkdir = storage_int_common_mkdir;
    storage->fs_api.common.rename = storage_int_common_rename;
    storage->fs_api.common.remove = storage_int_common_remove;
    storage->fs_api.common.fs_info = storage_int_common_fs_info;
}