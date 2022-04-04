#include "update_task.h"
#include "update_task_i.h"

#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <toolbox/path.h>
#include <update_util/dfu_file.h>
#include <update_util/lfs_backup.h>
#include <update_util/update_hl.h>

#define CHECK_RESULT(x) \
    if(!(x)) {          \
        break;          \
    }

static const DfuValidationParams flipper_dfu_params = {
    .device = 0xFFFF,
    .product = 0xDF11,
    .vendor = 0x0483,
};

static void update_task_dfu_progress(const uint8_t progress, void* context) {
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
static bool check_address_boundaries(const size_t address, bool allow_bl_region) {
    const size_t min_allowed_address = furi_hal_flash_get_base();
    const size_t max_allowed_address = (size_t)furi_hal_flash_get_free_end_address();
    return ((address >= min_allowed_address) && (address < max_allowed_address));
}

static bool validate_main_fw_address(const size_t address) {
    return check_address_boundaries(address, false);
}

int32_t update_task_worker_flash_writer(void* context) {
    furi_assert(context);
    UpdateTask* update_task = context;
    bool success = false;
    DfuUpdateTask page_task = {
        .address_cb = &validate_main_fw_address,
        .progress_cb = &update_task_dfu_progress,
        .task_cb = &furi_hal_flash_program_page,
        .context = update_task,
    };

    update_task->state.current_stage_idx = 0;
    update_task->state.total_stages = 4;

    do {
        CHECK_RESULT(update_task_parse_manifest(update_task));

        if(!string_empty_p(update_task->manifest->firmware_dfu_image)) {
            update_task_set_progress(update_task, UpdateTaskStageValidateDFUImage, 0);
            CHECK_RESULT(
                update_task_open_file(update_task, update_task->manifest->firmware_dfu_image));
            CHECK_RESULT(
                dfu_file_validate_crc(update_task->file, &update_task_dfu_progress, update_task));

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
        }

        update_task_set_progress(update_task, UpdateTaskStageComplete, 100);

        furi_hal_rtc_reset_flag(FuriHalRtcFlagExecuteUpdate);
        furi_hal_rtc_set_flag(FuriHalRtcFlagExecutePostUpdate);

        success = true;
    } while(false);

    if(!success) {
        update_task_set_progress(update_task, UpdateTaskStageError, update_task->state.progress);
    }

    return success ? UPDATE_TASK_NOERR : UPDATE_TASK_FAILED;
}

int32_t update_task_worker_backup_restore(void* context) {
    furi_assert(context);
    UpdateTask* update_task = context;
    bool success = false;
    string_t backup_file_path;
    string_init(backup_file_path);

    update_task->state.current_stage_idx = 0;
    update_task->state.total_stages = 1;
    do {
        if(!update_hl_get_current_package_path(update_task->storage, update_task->update_path)) {
            break;
        }

        path_concat(
            string_get_cstr(update_task->update_path), DEFAULT_BACKUP_FILENAME, backup_file_path);

        if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePreUpdate)) {
            update_task_set_progress(update_task, UpdateTaskStageLfsBackup, 0);
            furi_hal_rtc_reset_flag(FuriHalRtcFlagExecutePreUpdate);
            if((success =
                    lfs_backup_create(update_task->storage, string_get_cstr(backup_file_path)))) {
                furi_hal_rtc_set_flag(FuriHalRtcFlagExecuteUpdate);
            }

        } else if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePostUpdate)) {
            update_task_set_progress(update_task, UpdateTaskStageLfsRestore, 0);
            furi_hal_rtc_reset_flag(FuriHalRtcFlagExecutePostUpdate);
            success = lfs_backup_unpack(update_task->storage, string_get_cstr(backup_file_path));
        }

        update_task_set_progress(update_task, UpdateTaskStageComplete, 100);
        success = true;
    } while(false);

    if(!success) {
        update_task_set_progress(update_task, UpdateTaskStageError, update_task->state.progress);
    }

    string_clear(backup_file_path);

    return success ? UPDATE_TASK_NOERR : UPDATE_TASK_FAILED;
}