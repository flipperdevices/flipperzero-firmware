#include "dfu_file.h"
#include <furi_hal.h>

#define VALID_WHOLE_FILE_CRC 0xFFFFFFFF
#define DFU_SUFFIX_VERSION 0x011a
#define FLASH_PAGE_ALIGNMENT_MASK (FLASH_PAGE_SIZE - 1)

bool dfu_file_validate_crc(FsFile* dfuf, const DfuPageTaskProgressCb progress_cb) {
    if(!fs_file_is_valid(dfuf)) {
        return false;
    }

    f_rewind(&dfuf->file);
    furi_hal_crc_reset();

    uint32_t file_crc = 0;

    uint8_t data_buffer[DFU_DATA_BUFFER_MAX_LEN] = {0};
    uint16_t data_buffer_valid_len;

    // Feed file contents per sector into CRC calc
    for(uint32_t fptr = 0; fptr < dfuf->stat.fsize;) {
        CHECK_FRESULT(
            f_read(&dfuf->file, data_buffer, DFU_DATA_BUFFER_MAX_LEN, &data_buffer_valid_len));
        fptr += data_buffer_valid_len;

        if((fptr % DFU_DATA_BUFFER_MAX_LEN == 0)) {
            progress_cb(fptr * 100 / dfuf->stat.fsize);
        }
        file_crc = furi_hal_crc_feed(data_buffer, data_buffer_valid_len);
    }

    // Last 4 bytes of DFU file = CRC of previous file contents, inverted
    // If we calculate whole file CRC32, incl. embedded CRC,
    // that should give us 0xFFFFFFFF
    return file_crc == VALID_WHOLE_FILE_CRC;
}

uint8_t dfu_file_validate_headers(FsFile* dfuf, const DfuValidationParams* reference_params) {
    furi_assert(reference_params);

    DfuPrefix dfu_prefix = {0};
    DfuSuffix dfu_suffix = {0};
    uint16_t bytes_read = 0;

    if(!fs_file_is_valid(dfuf)) {
        return 0;
    }

    f_rewind(&dfuf->file);

    const uint32_t dfu_suffix_offset = dfuf->stat.fsize - sizeof(DfuSuffix);

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

    if((dfu_suffix.idVendor != reference_params->vendor) ||
       (dfu_suffix.idProduct != reference_params->product) ||
       (dfu_suffix.bcdDevice != reference_params->device)) {
        return 0;
    }

    return dfu_prefix.bTargets;
}

/* Assumes file is open, valid and read pointer is set at the start of image data
 */
static DfuUpdateBlockResult dfu_file_perform_task_for_update_pages(
    const DfuUpdateTask* task,
    FsFile* dfuf,
    const ImageElementHeader* header) {

    furi_assert(task);
    furi_assert(header);
    task->progress_cb(0);
    if((header->dwElementAddress & FLASH_PAGE_ALIGNMENT_MASK) != 0) {
        // start address is not aligned by page boundary -- we don't support that. Yet.
        return UpdateBlockResult_Failed;
    }

    if(task->address_cb && (!task->address_cb(header->dwElementAddress) ||
                            !task->address_cb(header->dwElementAddress + header->dwElementSize))) {
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

        if(!task->task_cb(i_page, fw_block, bytes_read)) {
            return UpdateBlockResult_Failed;
        }

        task->progress_cb(element_offs * 100 / header->dwElementSize);

        element_offs += bytes_read;
    }

    task->progress_cb(100);
    return UpdateBlockResult_OK;
}

bool dfu_file_process_targets(const DfuUpdateTask* task, FsFile* dfuf, const uint8_t n_targets) {
    TargetPrefix target_prefix = {0};
    ImageElementHeader image_element = {0};
    uint16_t bytes_read = 0;

    CHECK_FRESULT(f_lseek(&dfuf->file, sizeof(DfuPrefix)));
    for(uint8_t i_target = 0; i_target < n_targets; ++i_target) {
        CHECK_FRESULT(f_read(&dfuf->file, &target_prefix, sizeof(TargetPrefix), &bytes_read));
        // TODO: look into TargetPrefix and validate/filter?..
        for(uint32_t i_element = 0; i_element < target_prefix.dwNbElements; ++i_element) {
            CHECK_FRESULT(
                f_read(&dfuf->file, &image_element, sizeof(ImageElementHeader), &bytes_read));
            if(dfu_file_perform_task_for_update_pages(task, dfuf, &image_element) ==
               UpdateBlockResult_Failed) {
                return false;
            }
        }
    }

    return true;
}
