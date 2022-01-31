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

static const char fs_root_path[] = "/";
static const char update_file_path[] = "/firmware/bl.dfu";

static FATFS fs;
static FIL fs_file;
static FILINFO fs_stat;

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
    CHECK_FRESULT(f_stat(update_file_path, &fs_stat));
    CHECK_FRESULT(f_open(&fs_file, update_file_path, FA_OPEN_EXISTING | FA_READ));
    
    return true;
}

// Close & cleanup everything
static void cleanup_sdcard_update() {
    furi_hal_crc_deinit();
    f_close(&fs_file);
    // TODO: unmount?
}

static bool validate_dfu_file_crc(FIL* dfu_file) {
    uint32_t file_crc = 0;

    uint8_t data_buffer[DATA_BUFFER_MAX_LEN] = {0};
    uint16_t data_buffer_valid_len;

    for(uint32_t fptr = 0; fptr < fs_stat.fsize;) {
        CHECK_FRESULT(f_read(dfu_file, data_buffer, DATA_BUFFER_MAX_LEN, &data_buffer_valid_len));
        fptr += data_buffer_valid_len;

        file_crc = furi_hal_crc_feed(data_buffer, data_buffer_valid_len);
    }

    // Last 4 bytes of DFU file = CRC of previous file contents, inverted
    // If we calculate whole file CRC32, incl. embedded CRC,
    // that should give us 0xFFFFFFFF
    return file_crc == VALID_FULL_FILE_CRC;
}

static bool validate_dfu_file(FIL* dfu_file) {
    return validate_dfu_file_crc(dfu_file);
}

void execute_sdcard_update() {
    do {
        if(!prepare_sdcard_update()) {
            break;
        }

        if(!validate_dfu_file(&fs_file)) {
            break;
        }

    } while(0);

    cleanup_sdcard_update();
    delay(4000.f);
}