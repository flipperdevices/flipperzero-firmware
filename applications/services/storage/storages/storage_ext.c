#include "fatfs.h"
#include "../filesystem_api_internal.h"
#include "storage_ext.h"
#include <furi_hal.h>
#include "sd_notify.h"
#include <furi_hal_sd.h>

typedef FIL SDFile;
typedef DIR SDDir;
typedef FILINFO SDFileInfo;
typedef FRESULT SDError;

#define TAG "StorageExt"

/********************* Definitions ********************/

typedef struct {
    FATFS* fs;
    const char* path;
    bool sd_was_present;
} SDData;

static FS_Error storage_ext_parse_error(SDError error);

/******************* Core Functions *******************/

static bool sd_mount_card_internal(StorageData* storage, bool notify) {
    bool result = false;
    uint8_t counter = furi_hal_sd_max_mount_retry_count();
    uint8_t bsp_result;
    SDData* sd_data = storage->data;

    while(result == false && counter > 0 && furi_hal_sd_is_present()) {
        if(notify) {
            NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
            sd_notify_wait(notification);
            furi_record_close(RECORD_NOTIFICATION);
        }

        if((counter % 2) == 0) {
            // power reset sd card
            bsp_result = furi_hal_sd_init(true);
        } else {
            bsp_result = furi_hal_sd_init(false);
        }

        if(bsp_result) {
            // bsp error
            storage->status = StorageStatusErrorInternal;
        } else {
            SDError status = f_mount(sd_data->fs, sd_data->path, 1);

            if(status == FR_OK || status == FR_NO_FILESYSTEM) {
#ifndef FURI_RAM_EXEC
                FATFS* fs;
                uint32_t free_clusters;

                status = f_getfree(sd_data->path, &free_clusters, &fs);
#endif

                if(status == FR_OK || status == FR_NO_FILESYSTEM) {
                    result = true;
                }

                if(status == FR_OK) {
                    storage->status = StorageStatusOK;
                } else if(status == FR_NO_FILESYSTEM) {
                    storage->status = StorageStatusNoFS;
                } else {
                    storage->status = StorageStatusNotAccessible;
                }
            } else {
                storage->status = StorageStatusNotMounted;
            }
        }

        if(notify) {
            NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
            sd_notify_wait_off(notification);
            furi_record_close(RECORD_NOTIFICATION);
        }

        if(!result) {
            furi_delay_ms(1000);
            FURI_LOG_E(
                TAG, "init cycle %d, error: %s", counter, storage_data_status_text(storage));
            counter--;
        }
    }

    storage_data_timestamp(storage);

    return result;
}

FS_Error sd_unmount_card(StorageData* storage) {
    SDData* sd_data = storage->data;
    SDError error;

    storage->status = StorageStatusNotReady;
    error = FR_DISK_ERR;

    // TODO FL-3522: do i need to close the files?
    f_mount(0, sd_data->path, 0);

    return storage_ext_parse_error(error);
}

FS_Error sd_mount_card(StorageData* storage, bool notify) {
    sd_mount_card_internal(storage, notify);
    FS_Error error;

    if(storage->status != StorageStatusOK) {
        FURI_LOG_E(TAG, "sd init error: %s", storage_data_status_text(storage));
        if(notify) {
            NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
            sd_notify_error(notification);
            furi_record_close(RECORD_NOTIFICATION);
        }
        error = FSE_INTERNAL;
    } else {
        FURI_LOG_I(TAG, "card mounted");
        if(notify) {
            NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
            sd_notify_success(notification);
            furi_record_close(RECORD_NOTIFICATION);
        }

        error = FSE_OK;
    }

    return error;
}

FS_Error sd_format_card(StorageData* storage) {
#ifdef FURI_RAM_EXEC
    UNUSED(storage);
    return FSE_NOT_READY;
#else
    uint8_t* work_area;
    SDData* sd_data = storage->data;
    SDError error;

    work_area = malloc(_MAX_SS);
    error = f_mkfs(sd_data->path, FM_ANY, 0, work_area, _MAX_SS);
    free(work_area);

    do {
        storage->status = StorageStatusNotAccessible;
        if(error != FR_OK) break;
        storage->status = StorageStatusNoFS;
        char label[] = "X:Flipper SD";
        label[0] = sd_data->path[0]; // Drive number
        error = f_setlabel(label);
        if(error != FR_OK) break;
        storage->status = StorageStatusNotMounted;
        error = f_mount(sd_data->fs, sd_data->path, 1);
        if(error != FR_OK) break;
        storage->status = StorageStatusOK;
    } while(false);

    return storage_ext_parse_error(error);
#endif
}

FS_Error sd_card_info(StorageData* storage, SDInfo* sd_info) {
#ifndef FURI_RAM_EXEC
    uint32_t free_clusters, free_sectors, total_sectors;
    FATFS* fs;
#endif
    SDData* sd_data = storage->data;
    SDError error;

    // clean data
    memset(sd_info, 0, sizeof(SDInfo));

    // get fs info
    error = f_getlabel(sd_data->path, sd_info->label, NULL);
    if(error == FR_OK) {
#ifndef FURI_RAM_EXEC
        error = f_getfree(sd_data->path, &free_clusters, &fs);
#endif
    }

    if(error == FR_OK) {
        // calculate size
#ifndef FURI_RAM_EXEC
        total_sectors = (fs->n_fatent - 2) * fs->csize;
        free_sectors = free_clusters * fs->csize;
#endif

        uint16_t sector_size = _MAX_SS;
#if _MAX_SS != _MIN_SS
        sector_size = fs->ssize;
#endif

#ifdef FURI_RAM_EXEC
        sd_info->fs_type = 0;
        sd_info->kb_total = 0;
        sd_info->kb_free = 0;
        sd_info->cluster_size = 512;
        sd_info->sector_size = sector_size;
#else
        sd_info->fs_type = fs->fs_type;
        switch(fs->fs_type) {
        case FS_FAT12:
            sd_info->fs_type = FST_FAT12;
            break;
        case FS_FAT16:
            sd_info->fs_type = FST_FAT16;
            break;
        case FS_FAT32:
            sd_info->fs_type = FST_FAT32;
            break;
        case FS_EXFAT:
            sd_info->fs_type = FST_EXFAT;
            break;
        default:
            sd_info->fs_type = FST_UNKNOWN;
            break;
        }

        sd_info->kb_total = total_sectors / 1024 * sector_size;
        sd_info->kb_free = free_sectors / 1024 * sector_size;
        sd_info->cluster_size = fs->csize;
        sd_info->sector_size = sector_size;
#endif
    }

    FuriHalSdInfo info;
    FuriStatus status = furi_hal_sd_info(&info);

    if(status == FuriStatusOk) {
        sd_info->manufacturer_id = info.manufacturer_id;
        memcpy(sd_info->oem_id, info.oem_id, sizeof(info.oem_id));
        memcpy(sd_info->product_name, info.product_name, sizeof(info.product_name));
        sd_info->product_revision_major = info.product_revision_major;
        sd_info->product_revision_minor = info.product_revision_minor;
        sd_info->product_serial_number = info.product_serial_number;
        sd_info->manufacturing_year = info.manufacturing_year;
        sd_info->manufacturing_month = info.manufacturing_month;
    }

    return storage_ext_parse_error(error);
}

static void storage_ext_tick_internal(StorageData* storage, bool notify) {
    SDData* sd_data = storage->data;

    if(sd_data->sd_was_present) {
        if(furi_hal_sd_is_present()) {
            FURI_LOG_I(TAG, "card detected");
            sd_data->sd_was_present = false;
            sd_mount_card(storage, notify);

            if(!furi_hal_sd_is_present()) {
                FURI_LOG_I(TAG, "card removed while mounting");
                sd_unmount_card(storage);
                sd_data->sd_was_present = true;
            }
        }
    } else {
        if(!furi_hal_sd_is_present()) {
            FURI_LOG_I(TAG, "card removed");
            sd_data->sd_was_present = true;

            sd_unmount_card(storage);
            if(notify) {
                NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
                sd_notify_eject(notification);
                furi_record_close(RECORD_NOTIFICATION);
            }
        }
    }
}

static void storage_ext_tick(StorageData* storage) {
    storage_ext_tick_internal(storage, true);
}

/****************** Common Functions ******************/

static FS_Error storage_ext_parse_error(SDError error) {
    FS_Error result;
    switch(error) {
    case FR_OK:
        result = FSE_OK;
        break;
    case FR_NOT_READY:
        result = FSE_NOT_READY;
        break;
    case FR_NO_FILE:
    case FR_NO_PATH:
    case FR_NO_FILESYSTEM:
        result = FSE_NOT_EXIST;
        break;
    case FR_EXIST:
        result = FSE_EXIST;
        break;
    case FR_INVALID_NAME:
        result = FSE_INVALID_NAME;
        break;
    case FR_INVALID_OBJECT:
    case FR_INVALID_PARAMETER:
        result = FSE_INVALID_PARAMETER;
        break;
    case FR_DENIED:
        result = FSE_DENIED;
        break;
    default:
        result = FSE_INTERNAL;
        break;
    }

    return result;
}

static char* storage_ext_drive_path(StorageData* storage, const char* path) {
    SDData* sd_data = storage->data;
    size_t path_len = strlen(path) + 3;
    char* path_drv = malloc(path_len);
    path_drv[0] = sd_data->path[0];
    path_drv[1] = ':';
    strlcpy(path_drv + 2, path, path_len - 2);
    return path_drv;
}

/******************* File Functions *******************/

static bool storage_ext_file_open(
    void* ctx,
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode) {
    StorageData* storage = ctx;
    uint8_t _mode = 0;

    if(access_mode & FSAM_READ) _mode |= FA_READ;
    if(access_mode & FSAM_WRITE) _mode |= FA_WRITE;
    if(open_mode & FSOM_OPEN_EXISTING) _mode |= FA_OPEN_EXISTING;
    if(open_mode & FSOM_OPEN_ALWAYS) _mode |= FA_OPEN_ALWAYS;
    if(open_mode & FSOM_OPEN_APPEND) _mode |= FA_OPEN_APPEND;
    if(open_mode & FSOM_CREATE_NEW) _mode |= FA_CREATE_NEW;
    if(open_mode & FSOM_CREATE_ALWAYS) _mode |= FA_CREATE_ALWAYS;

    SDFile* file_data = malloc(sizeof(SDFile));
    storage_set_storage_file_data(file, file_data, storage);

    char* drive_path = storage_ext_drive_path(storage, path);
    file->internal_error_id = f_open(file_data, drive_path, _mode);
    free(drive_path);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static bool storage_ext_file_close(void* ctx, File* file) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);
    file->internal_error_id = f_close(file_data);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
    free(file_data);
    storage_set_storage_file_data(file, NULL, storage);
    return (file->error_id == FSE_OK);
}

static uint16_t
    storage_ext_file_read(void* ctx, File* file, void* buff, uint16_t const bytes_to_read) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);
    uint16_t bytes_read = 0;
    file->internal_error_id = f_read(file_data, buff, bytes_to_read, &bytes_read);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
    return bytes_read;
}

static uint16_t
    storage_ext_file_write(void* ctx, File* file, const void* buff, uint16_t const bytes_to_write) {
    uint16_t bytes_written = 0;
#ifdef FURI_RAM_EXEC
    UNUSED(ctx);
    UNUSED(file);
    UNUSED(buff);
    UNUSED(bytes_to_write);
    file->error_id = FSE_NOT_READY;
#else
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);
    file->internal_error_id = f_write(file_data, buff, bytes_to_write, &bytes_written);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
#endif
    return bytes_written;
}

static bool
    storage_ext_file_seek(void* ctx, File* file, const uint32_t offset, const bool from_start) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);

    if(from_start) {
        file->internal_error_id = f_lseek(file_data, offset);
    } else {
        uint64_t position = f_tell(file_data);
        position += offset;
        file->internal_error_id = f_lseek(file_data, position);
    }

    file->error_id = storage_ext_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static uint64_t storage_ext_file_tell(void* ctx, File* file) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);

    uint64_t position = 0;
    position = f_tell(file_data);
    file->error_id = FSE_OK;
    return position;
}

static bool storage_ext_file_expand(void* ctx, File* file, const uint64_t size) {
#ifdef FURI_RAM_EXEC
    UNUSED(ctx);
    UNUSED(file);
    UNUSED(size);
    file->error_id = FSE_NOT_READY;
#else
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = f_expand(file_data, size, 1);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
#endif
    return (file->error_id == FSE_OK);
}

static bool storage_ext_file_truncate(void* ctx, File* file) {
#ifdef FURI_RAM_EXEC
    UNUSED(ctx);
    UNUSED(file);
    file->error_id = FSE_NOT_READY;
#else
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = f_truncate(file_data);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
#endif
    return (file->error_id == FSE_OK);
}

static bool storage_ext_file_sync(void* ctx, File* file) {
#ifdef FURI_RAM_EXEC
    UNUSED(ctx);
    UNUSED(file);
    file->error_id = FSE_NOT_READY;
#else
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = f_sync(file_data);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
#endif
    return (file->error_id == FSE_OK);
}

static uint64_t storage_ext_file_size(void* ctx, File* file) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);

    uint64_t size = 0;
    size = f_size(file_data);
    file->error_id = FSE_OK;
    return size;
}

static bool storage_ext_file_eof(void* ctx, File* file) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);

    bool eof = f_eof(file_data);
    file->internal_error_id = 0;
    file->error_id = FSE_OK;
    return eof;
}

/******************* Dir Functions *******************/

static bool storage_ext_dir_open(void* ctx, File* file, const char* path) {
    StorageData* storage = ctx;

    SDDir* file_data = malloc(sizeof(SDDir));
    storage_set_storage_file_data(file, file_data, storage);
    char* drive_path = storage_ext_drive_path(storage, path);
    file->internal_error_id = f_opendir(file_data, drive_path);
    free(drive_path);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static bool storage_ext_dir_close(void* ctx, File* file) {
    StorageData* storage = ctx;
    SDDir* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = f_closedir(file_data);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
    free(file_data);
    return (file->error_id == FSE_OK);
}

static bool storage_ext_dir_read(
    void* ctx,
    File* file,
    FileInfo* fileinfo,
    char* name,
    const uint16_t name_length) {
    StorageData* storage = ctx;
    SDDir* file_data = storage_get_storage_file_data(file, storage);

    SDFileInfo _fileinfo;
    file->internal_error_id = f_readdir(file_data, &_fileinfo);
    file->error_id = storage_ext_parse_error(file->internal_error_id);

    if(fileinfo != NULL) {
        fileinfo->size = _fileinfo.fsize;
        fileinfo->flags = 0;

        if(_fileinfo.fattrib & AM_DIR) fileinfo->flags |= FSF_DIRECTORY;
    }

    if(name != NULL) {
        snprintf(name, name_length, "%s", _fileinfo.fname);
    }

    if(_fileinfo.fname[0] == 0) {
        file->error_id = FSE_NOT_EXIST;
    }

    return (file->error_id == FSE_OK);
}

static bool storage_ext_dir_rewind(void* ctx, File* file) {
    StorageData* storage = ctx;
    SDDir* file_data = storage_get_storage_file_data(file, storage);

    file->internal_error_id = f_readdir(file_data, NULL);
    file->error_id = storage_ext_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}
/******************* Common FS Functions *******************/

static FS_Error storage_ext_common_stat(void* ctx, const char* path, FileInfo* fileinfo) {
    StorageData* storage = ctx;
    SDFileInfo _fileinfo;
    char* drive_path = storage_ext_drive_path(storage, path);
    SDError result = f_stat(drive_path, &_fileinfo);
    free(drive_path);

    if(fileinfo != NULL) {
        fileinfo->size = _fileinfo.fsize;
        fileinfo->flags = 0;

        if(_fileinfo.fattrib & AM_DIR) fileinfo->flags |= FSF_DIRECTORY;
    }

    return storage_ext_parse_error(result);
}

static FS_Error storage_ext_common_remove(void* ctx, const char* path) {
    StorageData* storage = ctx;
#ifdef FURI_RAM_EXEC
    UNUSED(storage);
    UNUSED(path);
    return FSE_NOT_READY;
#else
    char* drive_path = storage_ext_drive_path(storage, path);
    SDError result = f_unlink(drive_path);
    free(drive_path);
    return storage_ext_parse_error(result);
#endif
}

static FS_Error storage_ext_common_mkdir(void* ctx, const char* path) {
    StorageData* storage = ctx;
#ifdef FURI_RAM_EXEC
    UNUSED(storage);
    UNUSED(path);
    return FSE_NOT_READY;
#else
    char* drive_path = storage_ext_drive_path(storage, path);
    SDError result = f_mkdir(drive_path);
    free(drive_path);
    return storage_ext_parse_error(result);
#endif
}

static FS_Error storage_ext_common_fs_info(
    void* ctx,
    const char* fs_path,
    uint64_t* total_space,
    uint64_t* free_space) {
    UNUSED(fs_path);
#ifdef FURI_RAM_EXEC
    UNUSED(ctx);
    UNUSED(total_space);
    UNUSED(free_space);
    return FSE_NOT_READY;
#else
    StorageData* storage = ctx;
    SDData* sd_data = storage->data;

    DWORD free_clusters;
    FATFS* fs;

    SDError fresult = f_getfree(sd_data->path, &free_clusters, &fs);
    if((FRESULT)fresult == FR_OK) {
        uint32_t total_sectors = (fs->n_fatent - 2) * fs->csize;
        uint32_t free_sectors = free_clusters * fs->csize;

        uint16_t sector_size = _MAX_SS;
#if _MAX_SS != _MIN_SS
        sector_size = fs->ssize;
#endif

        if(total_space != NULL) {
            *total_space = (uint64_t)total_sectors * (uint64_t)sector_size;
        }

        if(free_space != NULL) {
            *free_space = (uint64_t)free_sectors * (uint64_t)sector_size;
        }
    }

    return storage_ext_parse_error(fresult);
#endif
}

static bool storage_ext_common_equivalent_path(const char* path1, const char* path2) {
#ifdef FURI_RAM_EXEC
    UNUSED(path1);
    UNUSED(path2);
    return false;
#else
    return strcasecmp(path1, path2) == 0;
#endif
}

/******************* Init Storage *******************/
static const FS_Api fs_api = {
    .file =
        {
            .open = storage_ext_file_open,
            .close = storage_ext_file_close,
            .read = storage_ext_file_read,
            .write = storage_ext_file_write,
            .seek = storage_ext_file_seek,
            .tell = storage_ext_file_tell,
            .expand = storage_ext_file_expand,
            .truncate = storage_ext_file_truncate,
            .size = storage_ext_file_size,
            .sync = storage_ext_file_sync,
            .eof = storage_ext_file_eof,
        },
    .dir =
        {
            .open = storage_ext_dir_open,
            .close = storage_ext_dir_close,
            .read = storage_ext_dir_read,
            .rewind = storage_ext_dir_rewind,
        },
    .common =
        {
            .stat = storage_ext_common_stat,
            .mkdir = storage_ext_common_mkdir,
            .remove = storage_ext_common_remove,
            .fs_info = storage_ext_common_fs_info,
            .equivalent_path = storage_ext_common_equivalent_path,
        },
};

void storage_ext_init(StorageData* storage) {
    fatfs_init();

    SDData* sd_data = malloc(sizeof(SDData));
    sd_data->fs = &fatfs_object;
    sd_data->path = fatfs_path;
    sd_data->sd_was_present = true;

    storage->data = sd_data;
    storage->api.tick = storage_ext_tick;
    storage->fs_api = &fs_api;

    furi_hal_sd_presence_init();

    // do not notify on first launch, notifications app is waiting for our thread to read settings
    storage_ext_tick_internal(storage, false);
}

#include "fatfs/ff_gen_drv.h"

#define SCSI_BLOCK_SIZE (0x200UL)
static File* mnt_image = NULL;
static StorageData* mnt_image_storage = NULL;
bool mnt_mounted = false;
;

FS_Error storage_process_virtual_init(StorageData* image_storage, File* image) {
    if(mnt_image) return FSE_ALREADY_OPEN;
    mnt_image = image;
    mnt_image_storage = image_storage;
    return FSE_OK;
}

FS_Error storage_process_virtual_format(StorageData* storage) {
#ifdef FURI_RAM_EXEC
    UNUSED(storage);
    return FSE_NOT_READY;
#else
    if(!mnt_image) return FSE_NOT_READY;
    SDData* sd_data = storage->data;
    uint8_t* work = malloc(_MAX_SS);
    SDError error = f_mkfs(sd_data->path, FM_ANY, 0, work, _MAX_SS);
    free(work);
    if(error != FR_OK) return FSE_INTERNAL;
    return FSE_OK;
#endif
}

FS_Error storage_process_virtual_mount(StorageData* storage) {
    if(!mnt_image) return FSE_NOT_READY;
    SDData* sd_data = storage->data;
    SDError error = f_mount(sd_data->fs, sd_data->path, 1);
    if(error == FR_NO_FILESYSTEM) return FSE_INVALID_PARAMETER;
    if(error != FR_OK) return FSE_INTERNAL;
    mnt_mounted = true;
    return FSE_OK;
}

FS_Error storage_process_virtual_unmount(StorageData* storage) {
    if(!mnt_image) return FSE_NOT_READY;
    SDData* sd_data = storage->data;
    SDError error = f_mount(0, sd_data->path, 0);
    if(error != FR_OK) return FSE_INTERNAL;
    mnt_mounted = false;
    return FSE_OK;
}

FS_Error storage_process_virtual_quit(StorageData* storage) {
    if(!mnt_image) return FSE_NOT_READY;
    if(mnt_mounted) storage_process_virtual_unmount(storage);
    mnt_image = NULL;
    mnt_image_storage = NULL;
    return FSE_OK;
}

/**
  * @brief  Initializes a Drive
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
static DSTATUS mnt_driver_initialize(BYTE pdrv) {
    UNUSED(pdrv);
    return RES_OK;
}

/**
  * @brief  Gets Disk Status
  * @param  pdrv: Physical drive number (0..)
  * @retval DSTATUS: Operation status
  */
static DSTATUS mnt_driver_status(BYTE pdrv) {
    UNUSED(pdrv);
    if(!mnt_image) return STA_NOINIT;
    return RES_OK;
}

/**
  * @brief  Reads Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data buffer to store read data
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to read (1..128)
  * @retval DRESULT: Operation result
  */
static DRESULT mnt_driver_read(BYTE pdrv, BYTE* buff, DWORD sector, UINT count) {
    UNUSED(pdrv);
    if(!storage_ext_file_seek(mnt_image_storage, mnt_image, sector * SCSI_BLOCK_SIZE, true)) {
        return RES_ERROR;
    }
    size_t size = count * SCSI_BLOCK_SIZE;
    size_t read = storage_ext_file_read(mnt_image_storage, mnt_image, buff, size);
    return read == size ? RES_OK : RES_ERROR;
}

/**
  * @brief  Writes Sector(s)
  * @param  pdrv: Physical drive number (0..)
  * @param  *buff: Data to be written
  * @param  sector: Sector address (LBA)
  * @param  count: Number of sectors to write (1..128)
  * @retval DRESULT: Operation result
  */
static DRESULT mnt_driver_write(BYTE pdrv, const BYTE* buff, DWORD sector, UINT count) {
    UNUSED(pdrv);
    if(!storage_ext_file_seek(mnt_image_storage, mnt_image, sector * SCSI_BLOCK_SIZE, true)) {
        return RES_ERROR;
    }
    size_t size = count * SCSI_BLOCK_SIZE;
    size_t wrote = storage_ext_file_write(mnt_image_storage, mnt_image, buff, size);
    return wrote == size ? RES_OK : RES_ERROR;
}

/**
  * @brief  I/O control operation
  * @param  pdrv: Physical drive number (0..)
  * @param  cmd: Control code
  * @param  *buff: Buffer to send/receive control data
  * @retval DRESULT: Operation result
  */
static DRESULT mnt_driver_ioctl(BYTE pdrv, BYTE cmd, void* buff) {
    DRESULT res = RES_ERROR;

    DSTATUS status = mnt_driver_status(pdrv);
    if(status & STA_NOINIT) return RES_NOTRDY;

    switch(cmd) {
    /* Make sure that no pending write process */
    case CTRL_SYNC:
        res = RES_OK;
        break;

    /* Get number of sectors on the disk (DWORD) */
    case GET_SECTOR_COUNT:
        *(DWORD*)buff = storage_ext_file_size(mnt_image_storage, mnt_image) / SCSI_BLOCK_SIZE;
        res = RES_OK;
        break;

    /* Get R/W sector size (WORD) */
    case GET_SECTOR_SIZE:
        *(WORD*)buff = SCSI_BLOCK_SIZE;
        res = RES_OK;
        break;

    /* Get erase block size in unit of sector (DWORD) */
    case GET_BLOCK_SIZE:
        *(DWORD*)buff = SCSI_BLOCK_SIZE;
        res = RES_OK;
        break;

    default:
        res = RES_PARERR;
    }

    return res;
}

static Diskio_drvTypeDef mnt_driver = {
    mnt_driver_initialize,
    mnt_driver_status,
    mnt_driver_read,
    mnt_driver_write,
    mnt_driver_ioctl,
};

void storage_mnt_init(StorageData* storage) {
    char path[4] = {0};
    FATFS_LinkDriver(&mnt_driver, path);

    SDData* sd_data = malloc(sizeof(SDData));
    sd_data->fs = malloc(sizeof(FATFS));
    sd_data->path = strdup(path);

    storage->data = sd_data;
    storage->fs_api = &fs_api;
}
