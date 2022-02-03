#pragma once

#include "dfu_headers.h"
#include "fsfile.h"
#include "stdbool.h"
#include "stdint.h"

#define DFU_DATA_BUFFER_MAX_LEN 512

typedef enum {
    UpdateBlockResult_Unknown,
    UpdateBlockResult_OK,
    UpdateBlockResult_Skipped,
    UpdateBlockResult_Failed
} DfuUpdateBlockResult;

typedef bool (*DfuPageTaskCb)(
    const uint8_t i_page,
    const uint8_t* update_block,
    uint16_t update_block_len);
typedef void (*DfuPageTaskProgressCb)(const uint8_t progress);
typedef bool (*DfuAddressValidationCb)(const size_t address);

typedef struct {
    DfuPageTaskCb task_cb;
    DfuPageTaskProgressCb progress_cb;
    DfuAddressValidationCb address_cb;
} DfuUpdateTask;

typedef struct {
    uint16_t vendor;
    uint16_t product;
    uint16_t device;
} DfuValidationParams;

bool dfu_file_validate_crc(FsFile* dfuf, const DfuPageTaskProgressCb progress_cb);

/* Returns number of valid targets from file header
 * If file is invalid, returns 0
 */
uint8_t dfu_file_validate_headers(FsFile* dfuf, const DfuValidationParams* reference_params);

bool dfu_file_process_targets(const DfuUpdateTask* task, FsFile* dfuf, const uint8_t n_targets);
