#include "sd_update.h"
#include "dfu_headers.h"
#include "fsfile.h"

#include <target.h>
#include <furi_hal.h>

#define DATA_BUFFER_MAX_LEN 512
#define VALID_FULL_FILE_CRC 0xFFFFFFFF
#define FLIPPER_DFU_VENDOR 0x0483
#define FLIPPER_DFU_PRODUCT 0xdf11
#define DFU_SUFFIX_VERSION 0x011a

//#define FLASH_PAGE_SIZE 4096 // TODO: get from HAL?..
#define FLASH_PAGE_ALIGNMENT_MASK (FLASH_PAGE_SIZE - 1)

typedef enum {
    UpdateBlockResult_Unknown,
    UpdateBlockResult_OK,
    UpdateBlockResult_Skipped,
    UpdateBlockResult_Failed
} UpdateBlockResult;

static const char fs_root_path[] = "/";
static const char update_file_path[] = "/firmware/fw.dfu";

static FATFS fs;
static FsFile dfu_file;

static SdUpdateProgress progress = {0};

static void sdcard_update_set_progress(SdUpdateState state, uint8_t progress_pct);

// Initialize FS & CRC & open update file
static bool prepare_sdcard_update() {
    sdcard_update_set_progress(SdUpdateState_Mount, 0);
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
    sdcard_update_set_progress(SdUpdateState_Open, 0);
    return fs_file_open(&dfu_file, update_file_path);
}

// Close & cleanup everything
static void cleanup_sdcard_update() {
    furi_hal_crc_deinit();
    fs_file_close(&dfu_file);
    // TODO: unmount FS?
}

static bool validate_dfu_file_crc(FsFile* dfuf) {
    if(!fs_file_is_valid(dfuf)) {
        return false;
    }

    f_rewind(&dfuf->file);
    uint32_t file_crc = 0;

    uint8_t data_buffer[DATA_BUFFER_MAX_LEN] = {0};
    uint16_t data_buffer_valid_len;

    // Feed file contents per sector into CRC calc
    for(uint32_t fptr = 0; fptr < dfuf->stat.fsize;) {
        CHECK_FRESULT(
            f_read(&dfuf->file, data_buffer, DATA_BUFFER_MAX_LEN, &data_buffer_valid_len));
        fptr += data_buffer_valid_len;

        if((fptr % DATA_BUFFER_MAX_LEN == 0)) {
            sdcard_update_set_progress(SdUpdateState_ValidateImage, fptr * 100 / dfuf->stat.fsize);
        }
        file_crc = furi_hal_crc_feed(data_buffer, data_buffer_valid_len);
    }

    // Last 4 bytes of DFU file = CRC of previous file contents, inverted
    // If we calculate whole file CRC32, incl. embedded CRC,
    // that should give us 0xFFFFFFFF
    return file_crc == VALID_FULL_FILE_CRC;
}

// Returns number of valid targets from file header
// If file is invalid, returns 0
static uint8_t validate_dfu_file_headers(FsFile* dfuf) {
    DfuPrefix dfu_prefix = {0};
    DfuSuffix dfu_suffix = {0};
    uint16_t bytes_read = 0;

    if(!fs_file_is_valid(dfuf)) {
        return 0;
    }

    f_rewind(&dfuf->file);

    uint32_t dfu_suffix_offset = dfuf->stat.fsize - sizeof(DfuSuffix);

    CHECK_FRESULT(f_read(&dfuf->file, &dfu_prefix, sizeof(DfuPrefix), &bytes_read));
    if((dfu_prefix.szSignature[0] != 'D') || (dfu_prefix.szSignature[1] != 'f') ||
       (dfu_prefix.szSignature[2] != 'u') || (dfu_prefix.szSignature[3] != 'S') ||
       (dfu_prefix.szSignature[4] != 'e')) {
        return 0;
    }

    if((dfu_prefix.bVersion != 1) || (dfu_prefix.DFUImageSize != dfu_suffix_offset)) {
        return 0;
    }

    f_lseek(&dfuf->file, dfu_suffix_offset);
    CHECK_FRESULT(f_read(&dfuf->file, &dfu_suffix, sizeof(DfuSuffix), &bytes_read));
    if((dfu_suffix.bLength != sizeof(DfuSuffix)) || (dfu_suffix.bcdDFU != DFU_SUFFIX_VERSION)) {
        return 0;
    }
    // TODO: check DfuSignature?..
    // TODO: check bcdDevice?

    if((dfu_suffix.idVendor != FLIPPER_DFU_VENDOR) ||
       (dfu_suffix.idProduct != FLIPPER_DFU_PRODUCT)) {
        return 0;
    }

    return dfu_prefix.bTargets;
}

// Verifies a flash operation address for fitting into writable memory
static bool check_address_boundaries(size_t address, bool allow_bl_region) {
    size_t min_allowed_address = allow_bl_region ? furi_hal_flash_get_base() : FW_ADDRESS;
    size_t max_allowed_address = (size_t)furi_hal_flash_get_free_end_address();
    return ((address >= min_allowed_address) && (address < max_allowed_address));
}

typedef bool (*PageTask)(uint8_t i_page, uint8_t* update_block, uint16_t update_block_len);

static bool
    page_task_validate_flash(uint8_t i_page, uint8_t* update_block, uint16_t update_block_len) {
    size_t page_addr = furi_hal_flash_get_base() + furi_hal_flash_get_page_size() * i_page;
    return (memcmp(update_block, (void*)page_addr, update_block_len) == 0);
}

// Assumes file is open, valid and read pointer is set at the start of image data
static UpdateBlockResult perform_task_for_update_pages(
    const PageTask task,
    const SdUpdateState update_stage,
    FsFile* dfuf,
    ImageElementHeader* header) {
    sdcard_update_set_progress(update_stage, 0);
    if((header->dwElementAddress & FLASH_PAGE_ALIGNMENT_MASK) != 0) {
        // start address is not aligned by page boundary -- we don't support that. Yet.
        return UpdateBlockResult_Failed;
    }

    if(!check_address_boundaries(header->dwElementAddress, false) ||
       !check_address_boundaries(header->dwElementAddress + header->dwElementSize, false)) {
        return UpdateBlockResult_Skipped;
    }

    uint8_t fw_block[FLASH_PAGE_SIZE] = {0};
    uint16_t bytes_read = 0;
    for(uint32_t element_offs = 0; element_offs < header->dwElementSize;) {
        uint32_t n_bytes_to_read = FLASH_PAGE_SIZE;
        if((element_offs + n_bytes_to_read) > header->dwElementSize) {
            n_bytes_to_read = header->dwElementSize - element_offs;
        }

        if(f_read(&dfuf->file, fw_block, n_bytes_to_read, &bytes_read) != FR_OK) {
            return UpdateBlockResult_Failed;
        }

        int16_t i_page = furi_hal_flash_get_page_number(header->dwElementAddress + element_offs);
        if(i_page < 0) {
            return UpdateBlockResult_Failed;
        }

        if(!task(i_page, fw_block, bytes_read)) {
            return UpdateBlockResult_Failed;
        }

        sdcard_update_set_progress(update_stage, element_offs * 100 / header->dwElementSize);

        element_offs += bytes_read;
    }

    sdcard_update_set_progress(update_stage, 100);

    return UpdateBlockResult_OK;
}

static bool dfu_file_process_targets(
    const PageTask task,
    const SdUpdateState update_stage,
    FsFile* dfuf,
    uint8_t n_targets) {
    TargetPrefix target_prefix = {0};
    ImageElementHeader image_element = {0};
    uint16_t bytes_read = 0;

    CHECK_FRESULT(f_lseek(&dfuf->file, sizeof(DfuPrefix)));
    for(uint8_t i_target = 0; i_target < n_targets; ++i_target) {
        CHECK_FRESULT(f_read(&dfuf->file, &target_prefix, sizeof(TargetPrefix), &bytes_read));
        for(uint32_t i_element = 0; i_element < target_prefix.dwNbElements; ++i_element) {
            CHECK_FRESULT(
                f_read(&dfuf->file, &image_element, sizeof(ImageElementHeader), &bytes_read));
            if(perform_task_for_update_pages(task, update_stage, dfuf, &image_element) ==
               UpdateBlockResult_Failed) {
                return false;
            }
        }
    }

    return true;
}

static bool write_dfu_file(FsFile* dfuf) {
    sdcard_update_set_progress(SdUpdateState_ValidateImage, 0);
    if(!validate_dfu_file_crc(dfuf)) {
        return false;
    }

    uint8_t valid_targets = validate_dfu_file_headers(dfuf);
    return dfu_file_process_targets(
               &furi_hal_flash_program_page, SdUpdateState_Write, dfuf, valid_targets) &&
           dfu_file_process_targets(
               &page_task_validate_flash, SdUpdateState_Verify, dfuf, valid_targets);
}

static void init_display() {
    furi_hal_light_set(LightBacklight, 0xFF);
    furi_hal_fb_init();
    u8g2_t* fb = furi_hal_fb_get();
    u8g2_SetDrawColor(fb, 0x01);
    u8g2_SetFont(fb, u8g2_font_helvB08_tr);
}

static const char* update_state_text[] = {
    "Unknown",
    "Mounting storage",
    "Opening update file",
    "Validating image",
    "",
    "Writing",
    "Verifying",
    "Completed",
    "Error",
    "Hardware mismatch",
    "Update failed"};

static void render_update_progress() {
    furi_hal_fb_clear();
    u8g2_t* fb = furi_hal_fb_get();
    u8g2_DrawStr(fb, 2, 8, "Update & Recovery Mode");
    u8g2_DrawStr(fb, 2, 21, "SD card .dfu update");
    const char* stage_text = update_state_text[progress.operation];
    u8g2_DrawStr(fb, 16, 39, stage_text);
    u8g2_DrawBox(fb, 12, 47, 104, 12);
    u8g2_SetDrawColor(fb, 0x0);
    u8g2_DrawBox(fb, 13, 48, 102, 10);

    u8g2_SetDrawColor(fb, 0x1);
    uint8_t progress_pct = progress.progress;
    if(progress_pct > 100) {
        progress_pct = 100;
    }
    u8g2_DrawBox(fb, 14, 49, progress_pct, 8);

    furi_hal_fb_present();
}

static void sdcard_update_set_progress(SdUpdateState state, uint8_t progress_pct) {
    bool render_update = (progress.operation != state) ||
                         (((progress_pct - progress.rendered_progress) > 4));
    progress.operation = state;
    progress.progress = progress_pct;
    if(render_update) {
        progress.rendered_progress = progress.progress;
        render_update_progress();
    }
}

void sdcard_update_execute() {
    bool success = false;
    do {
        init_display();

        if(!prepare_sdcard_update()) {
            break;
        }

        if(!write_dfu_file(&dfu_file)) {
            break;
        }

        sdcard_update_set_progress(SdUpdateState_Complete, 100);
        success = true;

    } while(0);

    cleanup_sdcard_update();
    if(success) {
        delay(1500.f);
    } else {
        if(progress.operation < SdUpdateState_ErrorBase) {
            sdcard_update_set_progress(SdUpdateState_Error_FlashError, 100);
        }
        delay(1e5);
    }
}