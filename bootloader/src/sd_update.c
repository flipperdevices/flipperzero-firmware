#include "sd_update.h"
#include <target.h>
#include <furi_hal.h>
#include <fatfs.h>

void execute_sdcard_update() {
    FATFS fs;
    const char path[] = "/";

    MX_FATFS_Init();
    if(!hal_sd_detect()) {
        return;
    }

    if(BSP_SD_Init(true)) {
        return;
    }

    FRESULT mount_res = f_mount(&fs, path, 1);
 
    delay(3000.f);
}