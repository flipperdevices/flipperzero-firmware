#pragma once

#include <stdbool.h>
#include <m-string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Checks if supplied full manifest path is valid
 * @param full_path Full path to manifest file. Must be named UPDATE_MANIFEST_DEFAULT_NAME
 * @param out_manifest_dir Directory to apply update from, if supplied path is valid. 
 *   May be empty if update is in root update directory
 * @return bool if supplied path is valid and out_manifest_dir contains dir to apply
 */
bool update_hl_get_package_dir_name(const char* full_path, string_t out_manifest_dir);

typedef enum {
    UpdatePrepareResultOK,
    UpdatePrepareResultManifestPathInvalid,
    UpdatePrepareResultManifestFolderNotFound,
    UpdatePrepareResultManifestInvalid,
    UpdatePrepareResultStageMissing,
    UpdatePrepareResultStageIntegrityError,
} UpdatePrepareResult;

const char* update_hl_describe_preparation_result(const UpdatePrepareResult value);

/* Validates next stage and sets up registers to apply update after restart
 * @param manifest_dir_path Full path to manifest for update to apply 
 * @return UpdatePrepareResult validation & arm result
 */
UpdatePrepareResult update_hl_prepare(const char* manifest_file_path);

typedef struct Storage Storage;

/* Gets update package index to pass in RTC registers
 * @param storage Storage API
 * @param update_package_dir Package directory name
 * @return int32_t <0 - error, >= 0 - update index value
 */
int32_t update_hl_get_package_index(Storage* storage, const char* update_package_dir);

/* Gets filesystem path for current update package 
 * @param storage Storage API
 * @param out_path Path to directory with manifest & related files. Must be initialized
 * @return true if path was restored successfully
 */
bool update_hl_get_current_package_path(Storage* storage, string_t out_path);

void update_hl_persist_package_index(uint32_t index);

bool update_hl_is_armed();

void update_hl_disarm();

#ifdef __cplusplus
}
#endif