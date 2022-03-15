#include "lfs_processing.h"

#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>

#define SD_WAIT_STEP 15
#define BACKUP_FILE_LOCATION "/ext/backup.tar"

static bool await_sd_card(Storage* storage, int32_t max_wait_ms) {
    const bool wait_forever = max_wait_ms < 0;
    while(wait_forever || (max_wait_ms > 0)) {
        FS_Error sd_status = storage_sd_status(storage);
        if(sd_status == FSE_OK) {
            return true;
        } else {
            osDelay(SD_WAIT_STEP);
            max_wait_ms -= SD_WAIT_STEP;
        }
    }
    return false;
}

int32_t lfs_processing_srv(void* p) {
    FuriThread* own_thread = p;
#ifdef FURI_RAM_EXEC
    furi_thread_signal_ready(own_thread);
    return 0;
#else
    bool success = false;
    if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePreUpdate)) {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagExecutePreUpdate);
        Storage* storage = furi_record_open("storage");
        if(await_sd_card(storage, -1)) {
            if(storage_int_backup(storage, BACKUP_FILE_LOCATION) == FSE_OK) {
                success = true;
                furi_hal_rtc_set_flag(FuriHalRtcFlagExecuteUpdate);
            }
        }
        furi_record_close("storage");

    } else if(furi_hal_rtc_is_flag_set(FuriHalRtcFlagExecutePostUpdate)) {
        furi_hal_rtc_reset_flag(FuriHalRtcFlagExecutePostUpdate);
        Storage* storage = furi_record_open("storage");
        if(await_sd_card(storage, -1)) {
            success = (storage_int_restore(storage, BACKUP_FILE_LOCATION) == FSE_OK);
            furi_record_close("storage");
        }
    }

    if(success) {
        osDelay(150);
        furi_hal_power_reset();
    }
    furi_thread_signal_ready(own_thread);

    return success ? 0 : -1;
#endif
}