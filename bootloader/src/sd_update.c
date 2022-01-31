#include "sd_update.h"
#include "dfu_headers.h"

#include <target.h>
#include <furi_hal.h>
#include <fatfs.h>

#define CHECK_FRESULT(result)   \
    {                           \
        if((result) != FR_OK) { \
            return false;       \
        }                       \
    }

#define DATA_BUFFER_MAX_LEN 512
#define VALID_FULL_FILE_CRC 0xFFFFFFFF
#define FLIPPER_DFU_VENDOR 0x0483
#define FLIPPER_DFU_PRODUCT 0xdf11
#define DFU_SUFFIX_VERSION 0x011a

#define FLASH_PAGE_SIZE 4096 // TODO: get from HAL?..

typedef enum {
    UpdateBlockResult_Unknown,
    UpdateBlockResult_OK,
    UpdateBlockResult_Skipped,
    UpdateBlockResult_Failed
} UpdateBlockResult;

static const char fs_root_path[] = "/";
static const char update_file_path[] = "/firmware/bl.dfu";

typedef struct {
    FILINFO stat;
    FIL file;
    bool valid;
} FsFile;

static FATFS fs;
static FsFile dfu_file;

static bool fs_file_is_valid(FsFile* fsfile) {
    return fsfile && fsfile->valid;
}

static bool fs_file_open(FsFile* fsfile, const char* fpath) {
    CHECK_FRESULT(f_stat(fpath, &fsfile->stat));
    CHECK_FRESULT(f_open(&fsfile->file, fpath, FA_OPEN_EXISTING | FA_READ));
    return fsfile->valid = true;
}

static void fs_file_close(FsFile* fsfile) {
    if(!fs_file_is_valid(fsfile)) {
        return;
    }

    f_close(&fsfile->file);
    fsfile->valid = false;
}

// Initialize FS & CRC & open update file
static bool prepare_sdcard_update() {
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

static bool write_flash_region(uint32_t address, uint8_t* data, uint16_t length) {
    return false;
}

// Assumes file is open, valid and read pointer is set at the start of image data
static UpdateBlockResult write_flash_region_from_file(FsFile* dfuf, ImageElementHeader* header) {
    if(!check_address_boundaries(header->dwElementAddress, false) ||
       !check_address_boundaries(header->dwElementAddress + header->dwElementSize, false)) {
        return UpdateBlockResult_Skipped;
    }

    uint8_t fw_block[FLASH_PAGE_SIZE] = {0};
    uint16_t bytes_read = 0;
    for(uint32_t element_offs = 0; element_offs < header->dwElementSize; ++element_offs) {
        if(f_read(&dfuf->file, fw_block, FLASH_PAGE_SIZE, &bytes_read) != FR_OK) {
            return UpdateBlockResult_Failed;
        }

        if(!write_flash_region(header->dwElementAddress + element_offs, fw_block, bytes_read)) {
            return UpdateBlockResult_Failed;
        }
    }

    return UpdateBlockResult_OK;
}

static bool dfu_file_write_targets(FsFile* dfuf, uint8_t n_targets) {
    TargetPrefix target_prefix = {0};
    ImageElementHeader image_element = {0};
    uint16_t bytes_read = 0;

    CHECK_FRESULT(f_lseek(&dfuf->file, sizeof(DfuPrefix)));
    for(uint8_t i_target = 0; i_target < n_targets; ++i_target) {
        CHECK_FRESULT(f_read(&dfuf->file, &target_prefix, sizeof(TargetPrefix), &bytes_read));
        for(uint32_t i_element = 0; i_element < target_prefix.dwNbElements; ++i_element) {
            CHECK_FRESULT(
                f_read(&dfuf->file, &image_element, sizeof(ImageElementHeader), &bytes_read));
            if(write_flash_region_from_file(dfuf, &image_element) == UpdateBlockResult_Failed) {
                return false;
            }
        }
    }

    return true;
}

static bool write_dfu_file(FsFile* dfuf) {
    if(!validate_dfu_file_crc(dfuf)) {
        return false;
    }

    uint8_t valid_targets = validate_dfu_file_headers(dfuf);
    return dfu_file_write_targets(dfuf, valid_targets);
}

void execute_sdcard_update() {
    do {
        if(!prepare_sdcard_update()) {
            break;
        }

        if(!write_dfu_file(&dfu_file)) {
            break;
        }

    } while(0);

    cleanup_sdcard_update();
    delay(4000.f);
}