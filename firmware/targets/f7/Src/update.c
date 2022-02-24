#include "update.h"

#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>

#include <fatfs.h>

#define TAG "Main"

static FATFS fs;

static const char FS_ROOT_PATH[] = "/ext";
static const char LOADER_IMAGE[] = "/ext/update/lodaer.bin"; // TODO: load from manifest

#define CHECK_FRESULT(result)   \
    {                           \
        if((result) != FR_OK) { \
            return false;       \
        }                       \
    }

static bool flipper_update_init() {
    furi_hal_interrupt_init();
    furi_hal_delay_init(false);

    MX_GPIO_Init();
    FURI_LOG_I(TAG, "GPIO OK");

    furi_hal_bootloader_init();
    furi_hal_version_init();

    furi_hal_spi_init();

    MX_FATFS_Init();
    if(!hal_sd_detect()) {
        return false;
    }

    if(BSP_SD_Init(true)) {
        return false;
    }

    //furi_hal_crc_init();
    //furi_hal_crc_reset();

    CHECK_FRESULT(f_mount(&fs, FS_ROOT_PATH, 1));
    return true;
    //return false;
}

static bool flipper_update_process_manifest() {
    return false;
}

void flipper_update_exec() {
    if(!flipper_update_init()) {
        return;
    }
}