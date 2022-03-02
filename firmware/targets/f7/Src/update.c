#include "update.h"

#include <furi.h>
#include <furi_hal.h>
#include <flipper.h>

#include <fatfs.h>
#include <flipper_format/flipper_format.h>

#include <updater/util/update_manifest.h>

#define TAG "Main"

static FATFS fs;

static const char FS_ROOT_PATH[] = "/";
static const char CONFIG_PATH[] = "/update/update.cfg";

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
    furi_hal_crc_init();

    MX_FATFS_Init();
    if(!hal_sd_detect()) {
        return false;
    }

    if(BSP_SD_Init(true)) {
        return false;
    }

    CHECK_FRESULT(f_mount(&fs, FS_ROOT_PATH, 1));
    return true;
}

static inline void flipper_update_target_switch(void* offset) {
    asm volatile("ldr    r3, [%0]    \n"
                 "msr    msp, r3     \n"
                 "ldr    r3, [%1]    \n"
                 "mov    pc, r3      \n"
                 :
                 : "r"(offset), "r"(offset + 0x4)
                 : "r3");
}

static bool flipper_update_load_stage(UpdateManifest* manifest) {
    FIL file;
    FILINFO stat;

    furi_hal_crc_reset();

    string_t loader_img_path;
    string_init_printf(
        loader_img_path, "/update/%s", string_get_cstr(manifest->staged_loader_file));

    CHECK_FRESULT(f_stat(string_get_cstr(loader_img_path), &stat));
    CHECK_FRESULT(f_open(&file, string_get_cstr(loader_img_path), FA_OPEN_EXISTING | FA_READ));

    void* img = malloc(stat.fsize);
    uint32_t bytes_read = 0;
    const uint16_t MAX_READ = 0xFFFF;
    uint32_t crc = 0;
    do {
        uint16_t size_read = 0;
        CHECK_FRESULT(f_read(&file, img + bytes_read, MAX_READ, &size_read));
        crc = furi_hal_crc_feed(img + bytes_read, size_read);
        bytes_read += size_read;
    } while(bytes_read == MAX_READ);

    do {
        //if(crc == 0) {
        if(crc != manifest->staged_loader_crc) {
            break;
        }

        if(bytes_read != stat.fsize) {
            break;
        }

        FURI_CRITICAL_ENTER();
        memmove((void*)(SRAM1_BASE), img, stat.fsize);
        __HAL_SYSCFG_REMAPMEMORY_SRAM();
        flipper_update_target_switch((void*)SRAM1_BASE);
        /* unreachable -- but makes compiler happy */
        FURI_CRITICAL_EXIT();

        return true;
    } while(false);

    string_clear(loader_img_path);
    return false;
}

static UpdateManifest* flipper_update_process_manifest() {
    FIL file;
    FILINFO stat;

    furi_hal_crc_reset();

    CHECK_FRESULT(f_stat(CONFIG_PATH, &stat));
    CHECK_FRESULT(f_open(&file, CONFIG_PATH, FA_OPEN_EXISTING | FA_READ));

    uint8_t* manifest_data = malloc(stat.fsize);
    uint32_t bytes_read = 0;
    const uint16_t MAX_READ = 0xFFFF;

    do {
        uint16_t size_read = 0;
        CHECK_FRESULT(f_read(&file, manifest_data + bytes_read, MAX_READ, &size_read));
        bytes_read += size_read;
    } while(bytes_read == MAX_READ);

    if(bytes_read != stat.fsize) {
        return false;
    }

    UpdateManifest* manifest = update_manifest_alloc();
    if(!update_manifest_init_mem(manifest, manifest_data, bytes_read)) {
        free(manifest_data);
        update_manifest_free(manifest);
        return NULL;
    }

    free(manifest_data);
    return manifest;
}

void flipper_update_exec() {
    if(!flipper_update_init()) {
        return;
    }

    UpdateManifest* manifest = flipper_update_process_manifest();

    if(!manifest) {
        return;
    }

    if(!flipper_update_load_stage(manifest)) {
        return;
    }
}