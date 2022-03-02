#include "update.h"

#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>

#include <fatfs.h>
#include <flipper_format/flipper_format.h>

#define TAG "Main"

static FATFS fs;

static const char FS_ROOT_PATH[] = "/";
static const char LOADER_IMAGE[] = "/update/loader.bin"; // TODO: load from manifest

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

    //furi_hal_bootloader_init();
    //furi_hal_version_init();

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

#define RAM_START 0x20000000

static inline void flipper_update_target_switch(void* offset) {
    asm volatile("ldr    r3, [%0]    \n"
                 "msr    msp, r3     \n"
                 "ldr    r3, [%1]    \n"
                 "mov    pc, r3      \n"
                 :
                 : "r"(offset), "r"(offset + 0x4)
                 : "r3");
}

static bool flipper_update_load_stage() {
    FIL file;
    FILINFO stat;

    CHECK_FRESULT(f_stat(LOADER_IMAGE, &stat));
    CHECK_FRESULT(f_open(&file, LOADER_IMAGE, FA_OPEN_EXISTING | FA_READ));

    void* img = malloc(stat.fsize);
    uint32_t bytes_read = 0;
    const uint16_t MAX_READ = 0xFFFF;
    do {
        uint16_t size_read = 0;
        CHECK_FRESULT(f_read(&file, img + bytes_read, MAX_READ, &size_read));
        bytes_read += size_read;
    } while(bytes_read == MAX_READ);

    if(bytes_read != stat.fsize) {
        return false;
    }

    memmove((void*)(RAM_START), img, stat.fsize);
    //LL_RTC_BAK_SetRegister(RTC, LL_RTC_BKP_DR0, BOOT_REQUEST_TAINTED);
    __HAL_SYSCFG_REMAPMEMORY_SRAM();
    flipper_update_target_switch((void*)RAM_START);
    return true;
}

static bool flipper_update_process_manifest() {
    return false;
}

void flipper_update_exec() {
    if(!flipper_update_init()) {
        return;
    }

    if(!flipper_update_load_stage()) {
        return;
    }
}