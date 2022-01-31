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
    CHECK_FRESULT(f_stat(fpath, &fsfile->file));
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
    for(uint32_t fptr = 0; fptr <  dfuf->stat.fsize;) {
        CHECK_FRESULT(f_read(&dfuf->file, data_buffer, DATA_BUFFER_MAX_LEN, &data_buffer_valid_len));
        fptr += data_buffer_valid_len;

        file_crc = furi_hal_crc_feed(data_buffer, data_buffer_valid_len);
    }

    // Last 4 bytes of DFU file = CRC of previous file contents, inverted
    // If we calculate whole file CRC32, incl. embedded CRC,
    // that should give us 0xFFFFFFFF
    return file_crc == VALID_FULL_FILE_CRC;
}

static bool validate_dfu_file_headers(FsFile* dfuf) {
    f_rewind(dfuf);
    // TBD
    return true;
}

static bool validate_dfu_file(FsFile* dfuf) {
    return validate_dfu_file_crc(dfuf) && validate_dfu_file_headers(dfuf);
}

void execute_sdcard_update() {
    do {
        if(!prepare_sdcard_update()) {
            break;
        }

        if(!validate_dfu_file(&dfu_file)) {
            break;
        }

    } while(0);

    cleanup_sdcard_update();
    delay(4000.f);
}