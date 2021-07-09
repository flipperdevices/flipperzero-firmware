#include "fatfs.h"
#include <filesystem-api-v2.h>
#include "sd-storage.h"
#include <api-hal.h>
#include "sd-notify.h"
#include <api-hal-sd.h>

typedef FIL SDFile;
typedef DIR SDDir;
typedef FILINFO SDFileInfo;
typedef FRESULT SDError;

#define TAG "sd-storage"
/********************* Definitions ********************/

typedef struct {
    FATFS* fs;
    const char* path;
    bool sd_was_present;
} SDData;

static void storage_sd_tick(StorageData* storage);
static bool storage_sd_file_open(
    void* ctx,
    File* file,
    const char* path,
    FS_AccessMode access_mode,
    FS_OpenMode open_mode);
static bool storage_sd_file_close(void* ctx, File* file);
static FS_Error storage_sd_parse_error(SDError error);
static uint16_t
    storage_sd_file_read(void* ctx, File* file, void* buff, uint16_t const bytes_to_read);
static uint16_t
    storage_sd_file_write(void* ctx, File* file, const void* buff, uint16_t const bytes_to_write);

/******************* Core Functions *******************/

void storage_sd_init(StorageData* storage) {
    SDData* sd_data = malloc(sizeof(SDData));
    sd_data->fs = &USERFatFS;
    sd_data->path = "0:/";
    sd_data->sd_was_present = true;

    storage->data = sd_data;
    storage->api.tick = storage_sd_tick;
    storage->fs_api.file.open = storage_sd_file_open;
    storage->fs_api.file.close = storage_sd_file_close;
    storage->fs_api.file.read = storage_sd_file_read;
    storage->fs_api.file.write = storage_sd_file_write;

    hal_sd_detect_init();

    storage_sd_tick(storage);
}

static bool sd_mount_card(StorageData* storage) {
    bool result = false;
    const uint8_t max_init_counts = 10;
    uint8_t counter = max_init_counts;
    uint8_t bsp_result;
    SDData* sd_data = storage->data;

    NotificationApp* notification = furi_record_open("notification");
    storage_data_lock(storage);

    while(result == false && counter > 0 && hal_sd_detect()) {
        sd_notify_wait(notification);

        if((counter % 2) == 0) {
            // power reset sd card
            bsp_result = BSP_SD_Init(true);
        } else {
            bsp_result = BSP_SD_Init(false);
        }

        if(bsp_result) {
            // bsp error
            storage->status = SE_ERROR_INTERNAL;
        } else {
            SDError status = f_mount(sd_data->fs, sd_data->path, 1);

            if(status == FR_OK || status == FR_NO_FILESYSTEM) {
                FATFS* fs;
                uint32_t free_clusters;

                status = f_getfree(sd_data->path, &free_clusters, &fs);

                if(status == FR_OK || status == FR_NO_FILESYSTEM) {
                    result = true;
                }

                if(status == FR_OK) {
                    storage->status = SE_OK;
                } else if(status == FR_NO_FILESYSTEM) {
                    storage->status = SE_ERROR_NO_FILESYSTEM;
                } else {
                    storage->status = SE_ERROR_NOT_ACCESSIBLE;
                }
            } else {
                storage->status = SE_ERROR_NOT_MOUNTED;
            }
        }
        sd_notify_wait_off(notification);

        if(!result) {
            delay(1000);
            FURI_LOG_E(
                TAG, "init cycle %d, error: %s", counter, storage_data_status_text(storage));
            counter--;
        }
    }

    storage_data_unlock(storage);
    furi_record_close("notification");

    return result;
}

static void sd_unmount_card(StorageData* storage) {
    SDData* sd_data = storage->data;

    storage_data_lock(storage);
    storage->status = SE_ERROR_NOT_READY;

    // TODO do i need to close the files?

    f_mount(0, sd_data->path, 0);
    storage_data_unlock(storage);
}

static void storage_sd_tick(StorageData* storage) {
    SDData* sd_data = storage->data;

    if(sd_data->sd_was_present) {
        if(hal_sd_detect()) {
            FURI_LOG_I(TAG, "card detected");
            sd_mount_card(storage);

            NotificationApp* notification = furi_record_open("notification");
            if(storage->status != SE_OK) {
                FURI_LOG_E(TAG, "sd init error: %s", storage_data_status_text(storage));
                sd_notify_error(notification);
            } else {
                FURI_LOG_I(TAG, "card mounted");
                sd_notify_success(notification);
            }
            furi_record_close("notification");

            sd_data->sd_was_present = false;

            if(!hal_sd_detect()) {
                FURI_LOG_I(TAG, "card removed while mounting");
                sd_unmount_card(storage);
                sd_data->sd_was_present = true;
            }
        }
    } else {
        if(!hal_sd_detect()) {
            FURI_LOG_I(TAG, "card removed");
            sd_data->sd_was_present = true;

            sd_unmount_card(storage);
            NotificationApp* notification = furi_record_open("notification");
            sd_notify_eject(notification);
            furi_record_close("notification");
        }
    }
}

/****************** Common Functions ******************/
static FS_Error storage_sd_parse_error(SDError error) {
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

/******************* File Functions *******************/

static bool storage_sd_file_open(
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

    file->internal_error_id = f_open(file_data, path, _mode);
    file->error_id = storage_sd_parse_error(file->internal_error_id);
    return (file->error_id == FSE_OK);
}

static bool storage_sd_file_close(void* ctx, File* file) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);
    file->internal_error_id = f_close(file_data);
    file->error_id = storage_sd_parse_error(file->internal_error_id);
    free(file_data);
    return (file->error_id == FSE_OK);
}

static uint16_t
    storage_sd_file_read(void* ctx, File* file, void* buff, uint16_t const bytes_to_read) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);
    uint16_t bytes_readed = 0;
    file->internal_error_id = f_read(file_data, buff, bytes_to_read, &bytes_readed);
    file->error_id = storage_sd_parse_error(file->internal_error_id);
    return bytes_readed;
}

static uint16_t
    storage_sd_file_write(void* ctx, File* file, const void* buff, uint16_t const bytes_to_write) {
    StorageData* storage = ctx;
    SDFile* file_data = storage_get_storage_file_data(file, storage);
    uint16_t bytes_written = 0;
    file->internal_error_id = f_write(file_data, buff, bytes_to_write, &bytes_written);
    file->error_id = storage_sd_parse_error(file->internal_error_id);
    return bytes_written;
}