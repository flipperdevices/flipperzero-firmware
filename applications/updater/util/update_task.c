#include "update_task.h"
#include "dfu_file.h"
#include "lfs_backup.h"
#include "update_hl.h"

#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>

#define DELAY_OPERATION_OK 600
#define DELAY_OPERATION_ERROR 20000

typedef struct UpdateTask {
    UpdateTaskState state;
    string_t update_path;
    UpdateManifest* manifest;
    FuriThread* thread;
    Storage* storage;
    File* file;
    updateProgressCb status_change_cb;
    void* status_change_cb_state;
} UpdateTask;

static const char* update_task_stage_descr[] = {
    [UpdateTaskStageProgress] = "...",
    [UpdateTaskStageReadManifest] = "Loading update manifest",
    [UpdateTaskStageValidateDFUImage] = "Validating DFU file",
    [UpdateTaskStageFlashErase] = "Erasing flash",
    [UpdateTaskStageFlashWrite] = "Writing flash memory",
    [UpdateTaskStageFlashValidate] = "Validating",
    [UpdateTaskStageRadioWrite] = "Writing radio stack",
    [UpdateTaskStageRadioCommit] = "Applying radio stack",
    [UpdateTaskStageLfsBackup] = "Backing up LFS",
    [UpdateTaskStageLfsRestore] = "Restoring LFS",
    [UpdateTaskStageComplete] = "Complete",
    [UpdateTaskStageError] = "Error",
};

static int32_t update_task_worker_ram(void* context);
static int32_t update_task_worker_flash(void* context);

static void update_task_set_status(UpdateTask* update_task, const char* status) {
    if(!status) {
        if(update_task->state.stage >= COUNT_OF(update_task_stage_descr)) {
            status = "...";
        } else {
            status = update_task_stage_descr[update_task->state.stage];
        }
    }
    string_set_str(update_task->state.status, status);
}

static void
    update_task_set_progress(UpdateTask* update_task, UpdateTaskStage stage, uint8_t progress) {
    if(stage != UpdateTaskStageProgress) {
        update_task->state.stage = stage;
        update_task_set_status(update_task, NULL);
    }

    if(progress > 100) {
        progress = 100;
    }

    update_task->state.progress = progress;
    if(update_task->status_change_cb) {
        (update_task->status_change_cb)(
            string_get_cstr(update_task->state.status),
            progress,
            update_task->status_change_cb_state);
    }
}

static void update_task_close_file(UpdateTask* update_task) {
    furi_assert(update_task);
    if(!storage_file_is_open(update_task->file)) {
        return;
    }

    storage_file_close(update_task->file);
}

static bool update_task_check_file_exists(UpdateTask* update_task, string_t filename) {
    furi_assert(update_task);
    string_t tmp_path;
    string_init_set(tmp_path, update_task->update_path);
    path_append(tmp_path, string_get_cstr(filename));
    bool exists =
        (storage_common_stat(update_task->storage, string_get_cstr(tmp_path), NULL) == FSE_OK);
    string_clear(tmp_path);
    return exists;
}

static bool update_task_open_file(UpdateTask* update_task, string_t filename) {
    furi_assert(update_task);
    update_task_close_file(update_task);

    string_t tmp_path;
    string_init_set(tmp_path, update_task->update_path);
    path_append(tmp_path, string_get_cstr(filename));
    bool open_success = storage_file_open(
        update_task->file, string_get_cstr(tmp_path), FSAM_READ, FSOM_OPEN_EXISTING);
    string_clear(tmp_path);
    return open_success;
}

UpdateTask* update_task_alloc() {
    UpdateTask* update_task = malloc(sizeof(UpdateTask));

    update_task->state.stage = UpdateTaskStageProgress;
    update_task->state.progress = 0;
    string_init(update_task->state.status);

    update_task->manifest = update_manifest_alloc();
    update_task->storage = furi_record_open("storage");
    update_task->file = storage_file_alloc(update_task->storage);
    update_task->status_change_cb = NULL;
    //update_task->update_path = NULL;

    FuriThread* thread = update_task->thread = furi_thread_alloc();

    furi_thread_set_name(thread, "UpdateWorker");
    furi_thread_set_stack_size(thread, 2048);
    furi_thread_set_context(thread, update_task);
#ifdef FURI_RAM_EXEC
    (void)update_task_worker_flash;
    furi_thread_set_callback(thread, update_task_worker_ram);
#else
    (void)update_task_worker_ram;
    furi_thread_set_callback(thread, update_task_worker_flash);
#endif

    return update_task;
}

void update_task_free(UpdateTask* update_task) {
    furi_assert(update_task);

    furi_thread_join(update_task->thread);

    furi_thread_free(update_task->thread);
    update_task_close_file(update_task);
    storage_file_free(update_task->file);
    update_manifest_free(update_task->manifest);

    furi_record_close("storage");
    string_clear(update_task->update_path);

    free(update_task);
}

bool update_task_init(UpdateTask* update_task) {
    furi_assert(update_task);
    string_init(update_task->update_path);
    return true;
}

bool update_task_parse_manifest(UpdateTask* update_task) {
    furi_assert(update_task);
    update_task_set_progress(update_task, UpdateTaskStageReadManifest, 0);
    bool result = false;
    string_t manifest_path;
    string_init(manifest_path);

    do {
        update_task_set_progress(update_task, UpdateTaskStageProgress, 10);
        if(!update_hl_get_current_package_path(update_task->storage, update_task->update_path)) {
            break;
        }

        path_concat(
            string_get_cstr(update_task->update_path),
            UPDATE_MANIFEST_DEFAULT_NAME,
            manifest_path);
        update_task_set_progress(update_task, UpdateTaskStageProgress, 30);
        if(!update_manifest_init(update_task->manifest, string_get_cstr(manifest_path))) {
            break;
        }

        update_task_set_progress(update_task, UpdateTaskStageProgress, 50);
        if(!string_empty_p(update_task->manifest->firmware_dfu_image) &&
           !update_task_check_file_exists(update_task, update_task->manifest->firmware_dfu_image)) {
            break;
        }

        update_task_set_progress(update_task, UpdateTaskStageProgress, 70);
        if(!string_empty_p(update_task->manifest->radio_image) &&
           !update_task_check_file_exists(update_task, update_task->manifest->radio_image)) {
            break;
        }

        update_task_set_progress(update_task, UpdateTaskStageProgress, 100);
        result = true;
    } while(false);

    string_clear(manifest_path);
    return result;
}

void update_task_set_progress_cb(UpdateTask* update_task, updateProgressCb cb, void* state) {
    update_task->status_change_cb = cb;
    update_task->status_change_cb_state = state;
}

bool update_task_start(UpdateTask* update_task) {
    furi_assert(update_task);
    return furi_thread_start(update_task->thread);
}

UpdateTaskState const* update_task_get_state(UpdateTask* update_task) {
    furi_assert(update_task);
    return &update_task->state;
}

UpdateManifest const* update_task_get_manifest(UpdateTask* update_task) {
    furi_assert(update_task);
    return update_task->manifest;
}

static void update_task_dfu_progress(const uint8_t progress, void* context) {
    UpdateTask* update_task = context;
    update_task_set_progress(update_task, UpdateTaskStageProgress, progress);
}

//static void

#define CHECK_RESULT(x) \
    if(!(x)) {          \
        break;          \
    }

static const DfuValidationParams flipper_dfu_params = {
    .device = 0xFFFF,
    .product = 0xDF11,
    .vendor = 0x0483,
};

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

static int32_t update_task_worker_ram(void* context) {
    furi_assert(context);
    UpdateTask* update_task = context;
    bool success = false;
    DfuUpdateTask page_task = {
        .address_cb = &validate_main_fw_address,
        .progress_cb = &update_task_dfu_progress,
        .task_cb = &furi_hal_flash_program_page,
        .context = update_task,
    };

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
        success = true;
    } while(false);

    if(!success) {
        update_task_set_progress(update_task, UpdateTaskStageError, update_task->state.progress);
        osDelay(DELAY_OPERATION_ERROR);
        //return -1;
    }

    // TODO: move out of worker code?

    furi_hal_rtc_reset_flag(FuriHalRtcFlagExecuteUpdate);
    furi_hal_rtc_set_flag(FuriHalRtcFlagExecutePostUpdate);

    osDelay(DELAY_OPERATION_OK);
    furi_hal_power_reset();

    return 0;
}

static int32_t update_task_worker_flash(void* context) {
    furi_assert(context);
    UpdateTask* update_task = context;
    bool success = false;
    string_t backup_file_path;
    string_init(backup_file_path);

    do {
        if(!update_hl_get_current_package_path(update_task->storage, update_task->update_path)) {
            break;
        }

        path_concat(
            string_get_cstr(update_task->update_path), DEFAULT_BACKUP_FILENAME, backup_file_path);

        if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePreUpdate)) {
            update_task_set_progress(update_task, UpdateTaskStageLfsBackup, 0);
            furi_hal_rtc_reset_flag(FuriHalRtcFlagExecutePreUpdate);
            if((success = lfs_backup_create(string_get_cstr(backup_file_path)))) {
                furi_hal_rtc_set_flag(FuriHalRtcFlagExecuteUpdate);
            }

        } else if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePostUpdate)) {
            update_task_set_progress(update_task, UpdateTaskStageLfsRestore, 0);
            furi_hal_rtc_reset_flag(FuriHalRtcFlagExecutePostUpdate);
            success = lfs_backup_unpack(string_get_cstr(backup_file_path));
        }

        update_task_set_progress(update_task, UpdateTaskStageComplete, 100);
        success = true;
    } while(false);

    string_clear(backup_file_path);
    if(!success) {
        update_task_set_progress(update_task, UpdateTaskStageError, update_task->state.progress);
        osDelay(DELAY_OPERATION_ERROR);
        //return -1;
    }

    osDelay(DELAY_OPERATION_OK);
    furi_hal_power_reset();

    return 0;
}