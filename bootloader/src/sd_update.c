#include "sd_update.h"
#include "dfu_headers.h"

#include <target.h>
#include <furi_hal.h>
#include <fatfs.h>

#define CHECK_FRESULT(result) { if ((result) != FR_OK) { return ; } }

void execute_sdcard_update() {
    FATFS fs;
    //FRESULT fs_res;
    FIL fs_file;
    FILINFO fs_stat;

    const char fs_root_path[] = "/";
    const char update_file_path[] = "/firmware/fw.dfu";
    

    MX_FATFS_Init();
    if(!hal_sd_detect()) {
        return;
    }

    if(BSP_SD_Init(true)) {
        return;
    }

    CHECK_FRESULT(f_mount(&fs, fs_root_path, 1));
    CHECK_FRESULT(f_stat(update_file_path, &fs_stat));
    CHECK_FRESULT(f_open(&fs_file, update_file_path, FA_OPEN_EXISTING | FA_READ));
 
    delay(3000.f);
}