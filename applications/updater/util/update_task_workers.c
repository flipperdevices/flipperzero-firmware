#include "update_task.h"
#include "update_task_i.h"

#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <toolbox/path.h>
#include <update_util/dfu_file.h>
#include <update_util/lfs_backup.h>
#include <update_util/update_operation.h>
#include <toolbox/tar/tar_archive.h>
#include <toolbox/crc32_calc.h>

#define CHECK_RESULT(x) \
    if(!(x)) {          \
        break;          \
    }

#define STM_DFU_VENDOR_ID 0x0483
#define STM_DFU_PRODUCT_ID 0xDF11
/* Written into DFU file by build pipeline */
#define FLIPPER_ZERO_DFU_DEVICE_CODE 0xFFFF

#define EXT_PATH "/ext"
#define TAG "UpdWorker"

static const DfuValidationParams flipper_dfu_params = {
    .device = FLIPPER_ZERO_DFU_DEVICE_CODE,
    .product = STM_DFU_PRODUCT_ID,
    .vendor = STM_DFU_VENDOR_ID,
};

static void update_task_file_progress(const uint8_t progress, void* context) {
    UpdateTask* update_task = context;
    update_task_set_progress(update_task, UpdateTaskStageProgress, progress);
}

static bool page_task_compare_flash(
    const uint8_t i_page,
    const uint8_t* update_block,
    uint16_t update_block_len) {
    const size_t page_addr = furi_hal_flash_get_base() + furi_hal_flash_get_page_size() * i_page;
    return (memcmp(update_block, (void*)page_addr, update_block_len) == 0);
}

/* Verifies a flash operation address for fitting into writable memory
 */
static bool check_address_boundaries(const size_t address) {
    const size_t min_allowed_address = furi_hal_flash_get_base();
    const size_t max_allowed_address = (size_t)furi_hal_flash_get_free_end_address();
    return ((address >= min_allowed_address) && (address < max_allowed_address));
}

static bool update_task_write_dfu(UpdateTask* update_task) {
    DfuUpdateTask page_task = {
        .address_cb = &check_address_boundaries,
        .progress_cb = &update_task_file_progress,
        .task_cb = &furi_hal_flash_program_page,
        .context = update_task,
    };

    bool success = false;
    do {
        update_task_set_progress(update_task, UpdateTaskStageValidateDFUImage, 0);
        CHECK_RESULT(
            update_task_open_file(update_task, update_task->manifest->firmware_dfu_image));
        CHECK_RESULT(
            dfu_file_validate_crc(update_task->file, &update_task_file_progress, update_task));

        const uint8_t valid_targets =
            dfu_file_validate_headers(update_task->file, &flipper_dfu_params);
        if(valid_targets == 0) {
            break;
        }

        update_task_set_progress(update_task, UpdateTaskStageFlashWrite, 0);
        CHECK_RESULT(dfu_file_process_targets(&page_task, update_task->file, valid_targets));

        page_task.task_cb = &page_task_compare_flash;

        update_task_set_progress(update_task, UpdateTaskStageFlashValidate, 0);
        CHECK_RESULT(dfu_file_process_targets(&page_task, update_task->file, valid_targets));
        success = true;
    } while(false);

    return success;
}

static bool update_task_write_stack_data(UpdateTask* update_task) {
    furi_check(storage_file_is_open(update_task->file));
    const size_t FLASH_PAGE_SIZE = furi_hal_flash_get_page_size();

    uint32_t stack_size = storage_file_size(update_task->file);
    storage_file_seek(update_task->file, 0, true);

    if(!check_address_boundaries(update_task->manifest->radio_address) ||
       !check_address_boundaries(update_task->manifest->radio_address + stack_size)) {
        return false;
    }

    update_task_set_progress(update_task, UpdateTaskStageRadioWrite, 0);
    uint8_t* fw_block = malloc(FLASH_PAGE_SIZE);
    uint16_t bytes_read = 0;
    uint32_t element_offs = 0;

    while(element_offs < stack_size) {
        uint32_t n_bytes_to_read = FLASH_PAGE_SIZE;
        if((element_offs + n_bytes_to_read) > stack_size) {
            n_bytes_to_read = stack_size - element_offs;
        }

        bytes_read = storage_file_read(update_task->file, fw_block, n_bytes_to_read);
        if(bytes_read == 0) {
            break;
        }

        int16_t i_page =
            furi_hal_flash_get_page_number(update_task->manifest->radio_address + element_offs);
        if(i_page < 0) {
            break;
        }

        if(!furi_hal_flash_program_page(i_page, fw_block, bytes_read)) {
            break;
        }

        element_offs += bytes_read;
        update_task_set_progress(
            update_task, UpdateTaskStageProgress, element_offs * 100 / stack_size);
    }

    free(fw_block);
    return element_offs == stack_size;
}

//static void await_fus_op() {
//    uint16_t fus_state = 0xFFFF;
//    SHCI_FUS_GetState_ErrorCode_t error_code = 0;
//    do {
//        osDelay(100);
//        fus_state = SHCI_C2_FUS_GetState(&error_code);
//        FURI_LOG_I(TAG, "FUS state: %X, error = %x", fus_state, error_code);
//    } while((fus_state != FUS_STATE_VALUE_IDLE) || (fus_state != FUS_STATE_VALUE_ERROR));
//}

static bool update_task_apply_radiostack(UpdateTask* update_task) {
    bool success = false;
    do {
        CHECK_RESULT(ble_glue_wait_for_c2_start());

        const BleGlueC2Info* c2_state = ble_glue_get_c2_info();

        const UpdateManifestRadioVersion* radio_ver = &update_task->manifest->radio_version;
        bool stack_version_match = (c2_state->VersionMajor == radio_ver->version.major) &&
                                   (c2_state->VersionMinor == radio_ver->version.minor) &&
                                   (c2_state->VersionSub == radio_ver->version.sub) &&
                                   (c2_state->VersionBranch == radio_ver->version.branch) &&
                                   (c2_state->VersionReleaseType == radio_ver->version.release);
        bool stack_missing = (c2_state->VersionMajor == 0) && (c2_state->VersionMinor == 0);
        // Stack type is not available when we have FUS running.
        if(c2_state->mode == BleGlueC2ModeStack) {
            bool full_stack_match = stack_version_match &&
                                    (c2_state->StackType == radio_ver->version.type);
            if(full_stack_match) {
                // Nothing to do.
                FURI_LOG_W(TAG, "Stack version is up2date");
                furi_hal_rtc_reset_flag(FuriHalRtcFlagC2Update);
                success = true;
                break;
            } else {
                FURI_LOG_W(TAG, "Restarting to FUS");
                // Version mismatch. Let's boot to FUS and start updating.
                furi_hal_rtc_set_flag(FuriHalRtcFlagC2Update);
                CHECK_RESULT(furi_hal_bt_ensure_c2_mode(BleGlueC2ModeFUS));
                // ...system will restart here.
            }
        } else if(c2_state->mode == BleGlueC2ModeFUS) {
            // OK, we're in FUS mode.
            if(stack_version_match) {
                // We can't check StackType with FUS, but partial version matches
                if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagC2Update)) {
                    // This flag was set when full version was checked. So, delete it.
                    FURI_LOG_W(TAG, "Forcing stack removal (match)");
                    CHECK_RESULT(ble_glue_fus_stack_delete() != BleGlueCommandResultError);
                    CHECK_RESULT(ble_glue_fus_wait_operation() == BleGlueCommandResultOK);
                    // ...system will restart here.
                } else {
                    // We might just have the stack installed.
                    // Let's start it up to check its version
                    FURI_LOG_W(TAG, "Starting stack to check full version");
                    CHECK_RESULT(furi_hal_bt_ensure_c2_mode(BleGlueC2ModeStack));
                    // ...system will restart here.
                }
            } else {
                if(stack_missing) {
                    // Install stack.
                    FURI_LOG_W(TAG, "Writing stack");
                    update_task_set_progress(update_task, UpdateTaskStageRadioImageValidate, 0);
                    CHECK_RESULT(
                        update_task_open_file(update_task, update_task->manifest->radio_image));
                    CHECK_RESULT(
                        crc32_calc_file(
                            update_task->file, &update_task_file_progress, update_task) ==
                        update_task->manifest->radio_crc);

                    CHECK_RESULT(update_task_write_stack_data(update_task));
                    update_task_set_progress(update_task, UpdateTaskStageRadioInstall, 0);
                    CHECK_RESULT(
                        ble_glue_fus_stack_install(
                            update_task->manifest->radio_address,
                            update_task->manifest->radio_address) != BleGlueCommandResultError);
                    CHECK_RESULT(ble_glue_fus_wait_operation() == BleGlueCommandResultOK);
                    // ...system will restart here.
                } else {
                    FURI_LOG_W(TAG, "Forcing stack removal (mismatch)");
                    CHECK_RESULT(ble_glue_fus_stack_delete() != BleGlueCommandResultError);
                    CHECK_RESULT(ble_glue_fus_wait_operation() == BleGlueCommandResultOK);
                    // ...system will restart here.
                }
            }
        }

        //FURI_LOG_I(TAG, "Starting FUS");
        //CHECK_RESULT(furi_hal_bt_wait_fus());
        //WirelessFwInfo_t info;
        //CHECK_RESULT(SHCI_GetWirelessFwInfo(&info) == SHCI_Success);
        //if((info.VersionMajor == radio_ver->version.major) &&
        //   (info.VersionMinor == radio_ver->version.minor) &&
        //   (info.VersionSub == radio_ver->version.sub) &&
        //   (info.VersionBranch == radio_ver->version.branch) &&
        //   (info.VersionReleaseType == radio_ver->version.release) &&
        //   (info.StackType == radio_ver->version.type)) {
        //    FURI_LOG_I(TAG, "Stack version matches manifest");
        //    furi_hal_rtc_reset_flag(FuriHalRtcFlagC2Update);
        //    success = true;
        //    break;
        //}

        //ensure_fus_running();

        //bool stack_missing = (info.VersionMajor == 0) && (info.VersionMinor == 0);
        //FURI_LOG_I(TAG, "Stack missing: %d", stack_missing);
        //if(!stack_missing) {
        //    // Erase
        //    furi_hal_rtc_set_boot_mode(FuriHalRtcBootModeUpdate);
        //    furi_hal_rtc_set_flag(FuriHalRtcFlagC2Update);
        //    update_task_set_progress(update_task, UpdateTaskStageRadioErase, 0);
        //    CHECK_RESULT(ble_glue_fus_stack_delete() == BleGlueCommandResultOperationOngoing);
        //    CHECK_RESULT(ble_glue_fus_wait_operation() == BleGlueCommandResultOK);
        //    FURI_LOG_I(TAG, "Erased");
        //}

        //update_task_set_progress(update_task, UpdateTaskStageRadioImageValidate, 0);
        //CHECK_RESULT(update_task_open_file(update_task, update_task->manifest->radio_image));
        //CHECK_RESULT(
        //    crc32_calc_file(update_task->file, &update_task_file_progress, update_task) ==
        //    update_task->manifest->radio_crc);

        //// TODO: check SFSR
        //CHECK_RESULT(update_task_write_stack_data(update_task));
        //update_task_set_progress(update_task, UpdateTaskStageRadioInstall, 0);
        ////CHECK_RESULT(
        ////    SHCI_C2_FUS_FwUpgrade(
        ////        update_task->manifest->radio_address, update_task->manifest->radio_address) ==
        ////    SHCI_Success);
        //SHCI_CmdStatus_t upgrade_stat = SHCI_C2_FUS_FwUpgrade(
        //    update_task->manifest->radio_address, update_task->manifest->radio_address);
        //FURI_LOG_I(TAG, "Cmd res = %x", upgrade_stat);
        //CHECK_RESULT(upgrade_stat == SHCI_Success);

        //await_fus_op();

        //osDelay(6000);

        success = true;
    } while(false);

    return success;
}

int32_t update_task_worker_flash_writer(void* context) {
    furi_assert(context);
    UpdateTask* update_task = context;
    bool success = false;

    update_task->state.current_stage_idx = 0;
    update_task->state.total_stages = 4;

    do {
        CHECK_RESULT(update_task_parse_manifest(update_task));

        if(!string_empty_p(update_task->manifest->radio_image)) {
            if(!update_task_apply_radiostack(update_task)) {
                break;
            }
        }

        if(!string_empty_p(update_task->manifest->firmware_dfu_image)) {
            if(!update_task_write_dfu(update_task)) {
                break;
            }
        }
        update_task_set_progress(update_task, UpdateTaskStageCompleted, 100);
        furi_hal_rtc_set_boot_mode(FuriHalRtcBootModePostUpdate);
        success = true;
    } while(false);

    if(!success) {
        update_task_set_progress(update_task, UpdateTaskStageError, update_task->state.progress);
    }

    return success ? UPDATE_TASK_NOERR : UPDATE_TASK_FAILED;
}

//int32_t update_task_worker_flash_writer(void* context) {
//    furi_assert(context);
//    UpdateTask* update_task = context;
//}

static bool update_task_pre_update(UpdateTask* update_task) {
    bool success = false;
    string_t backup_file_path;
    string_init(backup_file_path);
    path_concat(
        string_get_cstr(update_task->update_path), LFS_BACKUP_DEFAULT_FILENAME, backup_file_path);

    update_task->state.total_stages = 1;
    update_task_set_progress(update_task, UpdateTaskStageLfsBackup, 0);
    furi_hal_rtc_set_boot_mode(FuriHalRtcBootModeNormal); // to avoid bootloops
    if((success = lfs_backup_create(update_task->storage, string_get_cstr(backup_file_path)))) {
        furi_hal_rtc_set_boot_mode(FuriHalRtcBootModeUpdate);
    }

    string_clear(backup_file_path);
    return success;
}

typedef struct {
    UpdateTask* update_task;
    int32_t total_files, processed_files;
} TarUnpackProgress;

static bool update_task_resource_unpack_cb(const char* name, bool is_directory, void* context) {
    UNUSED(name);
    UNUSED(is_directory);
    TarUnpackProgress* unpack_progress = context;
    unpack_progress->processed_files++;
    update_task_set_progress(
        unpack_progress->update_task,
        UpdateTaskStageProgress,
        unpack_progress->processed_files * 100 / (unpack_progress->total_files + 1));
    return true;
}

static bool update_task_post_update(UpdateTask* update_task) {
    bool success = false;

    string_t file_path;
    string_init(file_path);

    update_task->state.total_stages = 2;

    do {
        CHECK_RESULT(update_task_parse_manifest(update_task));
        path_concat(
            string_get_cstr(update_task->update_path), LFS_BACKUP_DEFAULT_FILENAME, file_path);

        bool unpack_resources = !string_empty_p(update_task->manifest->resource_bundle);
        if(unpack_resources) {
            update_task->state.total_stages++;
        }

        update_task_set_progress(update_task, UpdateTaskStageLfsRestore, 0);
        furi_hal_rtc_set_boot_mode(FuriHalRtcBootModeNormal);

        CHECK_RESULT(lfs_backup_unpack(update_task->storage, string_get_cstr(file_path)));

        if(unpack_resources) {
            TarUnpackProgress progress = {
                .update_task = update_task,
                .total_files = 0,
                .processed_files = 0,
            };
            update_task_set_progress(update_task, UpdateTaskStageResourcesUpdate, 0);

            path_concat(
                string_get_cstr(update_task->update_path),
                string_get_cstr(update_task->manifest->resource_bundle),
                file_path);

            TarArchive* archive = tar_archive_alloc(update_task->storage);
            tar_archive_set_file_callback(archive, update_task_resource_unpack_cb, &progress);
            success = tar_archive_open(archive, string_get_cstr(file_path), TAR_OPEN_MODE_READ);
            if(success) {
                progress.total_files = tar_archive_get_entries_count(archive);
                if(progress.total_files > 0) {
                    tar_archive_unpack_to(archive, EXT_PATH);
                }
            }
            tar_archive_free(archive);
        }
    } while(false);

    string_clear(file_path);
    return success;
}

int32_t update_task_worker_backup_restore(void* context) {
    furi_assert(context);
    UpdateTask* update_task = context;
    bool success = false;

    FuriHalRtcBootMode boot_mode = furi_hal_rtc_get_boot_mode();
    if((boot_mode != FuriHalRtcBootModePreUpdate) && (boot_mode != FuriHalRtcBootModePostUpdate)) {
        // no idea how we got here. Clear to normal boot
        furi_hal_rtc_set_boot_mode(FuriHalRtcBootModeNormal);
        return UPDATE_TASK_NOERR;
    }

    update_task->state.current_stage_idx = 0;

    if(!update_operation_get_current_package_path(update_task->storage, update_task->update_path)) {
        return UPDATE_TASK_FAILED;
    }

    if(boot_mode == FuriHalRtcBootModePreUpdate) {
        success = update_task_pre_update(update_task);
    } else if(boot_mode == FuriHalRtcBootModePostUpdate) {
        success = update_task_post_update(update_task);
    }

    if(success) {
        update_task_set_progress(update_task, UpdateTaskStageCompleted, 100);
    } else {
        update_task_set_progress(update_task, UpdateTaskStageError, update_task->state.progress);
    }

    return success ? UPDATE_TASK_NOERR : UPDATE_TASK_FAILED;
}