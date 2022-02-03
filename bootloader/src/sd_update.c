#include "sd_update.h"
#include "sd_update_progress.h"
#include "dfu_file.h"

#include <target.h>
#include <furi_hal.h>

static const char fs_root_path[] = "/";
static const char update_file_path[] = "/firmware/fw.dfu";

static FATFS fs;
static FsFile dfu_file;

/* Verifies a flash operation address for fitting into writable memory
 */
static bool check_address_boundaries(const size_t address, bool allow_bl_region) {
    size_t min_allowed_address = allow_bl_region ? furi_hal_flash_get_base() : FW_ADDRESS;
    size_t max_allowed_address = (size_t)furi_hal_flash_get_free_end_address();
    return ((address >= min_allowed_address) && (address < max_allowed_address));
}

static bool validate_main_fw_address(const size_t address) {
    return check_address_boundaries(address, false);
}

static bool page_task_validate_flash(
    const uint8_t i_page,
    const uint8_t* update_block,
    uint16_t update_block_len) {
    const size_t page_addr = furi_hal_flash_get_base() + furi_hal_flash_get_page_size() * i_page;
    return (memcmp(update_block, (void*)page_addr, update_block_len) == 0);
}

static void init_display() {
    furi_hal_light_set(LightBacklight, 0xFF);
    furi_hal_fb_init();
    u8g2_t* fb = furi_hal_fb_get();
    u8g2_SetDrawColor(fb, 0x01);
    u8g2_SetFont(fb, u8g2_font_helvB08_tr);
}

/* Initialize FS & CRC & open update file
 */
static bool sdcard_update_prepare() {
    MX_FATFS_Init();
    if(!hal_sd_detect()) {
        return false;
    }

    if(BSP_SD_Init(true)) {
        return false;
    }

    furi_hal_crc_init();
    furi_hal_crc_reset();

    CHECK_FRESULT(f_mount(&fs, fs_root_path, 1));
    return true;
}

/* Close & cleanup everything
 */
static void sdcard_update_cleanup() {
    furi_hal_crc_deinit();
    fs_file_close(&dfu_file);
    /* TODO: unmount FS? */
}

static const DfuUpdateTask page_write_task = {
    .address_cb = &validate_main_fw_address,
    .progress_cb = &sdcard_update_progress_update,
    .task_cb = &furi_hal_flash_program_page,
};

static const DfuUpdateTask page_validation_task = {
    .address_cb = &validate_main_fw_address,
    .progress_cb = &sdcard_update_progress_update,
    .task_cb = &page_task_validate_flash,
};

static const DfuValidationParams flipper_dfu_params = {
    .device = 0xFFFF,
    .product = 0xDF11,
    .vendor = 0x0483,
};

void sdcard_update_execute() {
    bool success = false;
    init_display();
    do {
        sdcard_update_progress_set_stage(SdUpdateState_Mount, 0);
        if(!sdcard_update_prepare()) {
            break;
        }

        sdcard_update_progress_set_stage(SdUpdateState_Open, 0);
        if(!fs_file_open(&dfu_file, update_file_path)) {
            break;
        }

        sdcard_update_progress_set_stage(SdUpdateState_ValidateImage, 0);
        if(!dfu_file_validate_crc(&dfu_file, &sdcard_update_progress_update)) {
            break;
        }

        const uint8_t valid_targets = dfu_file_validate_headers(&dfu_file, &flipper_dfu_params);
        if(valid_targets == 0) {
            break;
        }

        sdcard_update_progress_set_stage(SdUpdateState_Write, 0);
        if(!dfu_file_process_targets(&page_write_task, &dfu_file, valid_targets)) {
            break;
        }

        sdcard_update_progress_set_stage(SdUpdateState_Verify, 0);
        if(!dfu_file_process_targets(&page_validation_task, &dfu_file, valid_targets)) {
            break;
        }

        sdcard_update_progress_set_stage(SdUpdateState_Complete, 100);
        success = true;
    } while(0);

    sdcard_update_cleanup();

    if(success) {
        delay(1500.f);
    } else {
        if(sdcard_update_progress_get()->operation < SdUpdateState_ErrorBase) {
            sdcard_update_progress_set_stage(SdUpdateState_Error_FlashError, 100);
        }
        delay(120 * 1e3);
    }
}
