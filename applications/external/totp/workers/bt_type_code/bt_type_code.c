#include "bt_type_code.h"
#include <furi_hal_bt.h>
#include <extra_profiles/hid_profile.h>
#include <furi_hal_version.h>
#include <furi/core/thread.h>
#include <furi/core/mutex.h>
#include <furi/core/string.h>
#include <furi/core/kernel.h>
#include <bt/bt_service/bt.h>
#include <storage/storage.h>
#include "../../types/common.h"
#include "../../types/token_info.h"
#include "../type_code_common.h"
#include "../../config/app/config.h"
#include "../../services/config/constants.h"

#define HID_BT_KEYS_STORAGE_PATH CONFIG_FILE_DIRECTORY_PATH "/.bt_hid.keys"

struct TotpBtTypeCodeWorkerContext {
    char* code_buffer;
    uint8_t code_buffer_size;
    uint8_t flags;
    FuriThread* thread;
    FuriMutex* code_buffer_sync;
    Bt* bt;
    FuriHalBleProfileBase* ble_hid_profile;
    bool is_advertising;
    bool is_connected;
    AutomationKeyboardLayout keyboard_layout;
    uint16_t initial_delay;
};

static inline bool totp_type_code_worker_stop_requested() {
    return furi_thread_flags_get() & TotpBtTypeCodeWorkerEventStop;
}

static bool hid_key_press(uint16_t button, void* context) {
    FuriHalBleProfileBase* profile = context;
    return ble_profile_hid_kb_press(profile, button);
}

static bool hid_key_release(uint16_t button, void* context) {
    FuriHalBleProfileBase* profile = context;
    return ble_profile_hid_kb_release(profile, button);
}

static void totp_type_code_worker_type_code(TotpBtTypeCodeWorkerContext* context) {
    uint8_t i = 0;
    do {
        furi_delay_ms(500);
        i++;
    } while(!context->is_connected && i < 100 && !totp_type_code_worker_stop_requested());

    if(context->is_connected &&
       furi_mutex_acquire(context->code_buffer_sync, 500) == FuriStatusOk) {
        totp_type_code_worker_execute_automation(
            &hid_key_press,
            &hid_key_release,
            context->code_buffer,
            context->code_buffer_size,
            context->flags,
            context->keyboard_layout,
            context->initial_delay,
            context->ble_hid_profile);
        furi_mutex_release(context->code_buffer_sync);
    }
}

static int32_t totp_type_code_worker_callback(void* context) {
    furi_check(context);
    FuriMutex* context_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    TotpBtTypeCodeWorkerContext* bt_context = context;

    while(true) {
        uint32_t flags = furi_thread_flags_wait(
            TotpBtTypeCodeWorkerEventStop | TotpBtTypeCodeWorkerEventType,
            FuriFlagWaitAny,
            FuriWaitForever);
        furi_check((flags & FuriFlagError) == 0); //-V562
        if(flags & TotpBtTypeCodeWorkerEventStop) break;

        if(furi_mutex_acquire(context_mutex, FuriWaitForever) == FuriStatusOk) {
            if(flags & TotpBtTypeCodeWorkerEventType) {
                totp_type_code_worker_type_code(bt_context);
            }

            furi_mutex_release(context_mutex);
        }
    }

    furi_mutex_free(context_mutex);

    return 0;
}

static void connection_status_changed_callback(BtStatus status, void* context) {
    TotpBtTypeCodeWorkerContext* bt_context = context;
    if(status == BtStatusConnected) {
        bt_context->is_connected = true;
    } else if(status < BtStatusConnected) {
        bt_context->is_connected = false;
    }
}

void totp_bt_type_code_worker_start(
    TotpBtTypeCodeWorkerContext* context,
    char* code_buffer,
    uint8_t code_buffer_size,
    FuriMutex* code_buffer_sync,
    AutomationKeyboardLayout keyboard_layout,
    uint16_t initial_delay) {
    furi_check(context != NULL);
    context->code_buffer = code_buffer;
    context->code_buffer_size = code_buffer_size;
    context->code_buffer_sync = code_buffer_sync;
    context->keyboard_layout = keyboard_layout;
    context->initial_delay = initial_delay;
    context->thread = furi_thread_alloc();
    furi_thread_set_name(context->thread, "TOTPBtHidWorker");
    furi_thread_set_stack_size(context->thread, 1024);
    furi_thread_set_context(context->thread, context);
    furi_thread_set_callback(context->thread, totp_type_code_worker_callback);
    furi_thread_start(context->thread);
}

void totp_bt_type_code_worker_stop(TotpBtTypeCodeWorkerContext* context) {
    furi_check(context != NULL);
    furi_thread_flags_set(furi_thread_get_id(context->thread), TotpBtTypeCodeWorkerEventStop);
    furi_thread_join(context->thread);
    furi_thread_free(context->thread);
    context->thread = NULL;
}

void totp_bt_type_code_worker_notify(
    TotpBtTypeCodeWorkerContext* context,
    TotpBtTypeCodeWorkerEvent event,
    uint8_t flags) {
    furi_check(context != NULL);
    context->flags = flags;
    furi_thread_flags_set(furi_thread_get_id(context->thread), event);
}

TotpBtTypeCodeWorkerContext* totp_bt_type_code_worker_init(uint16_t mac_xor) {
    TotpBtTypeCodeWorkerContext* context = malloc(sizeof(TotpBtTypeCodeWorkerContext));
    furi_check(context != NULL);

    context->bt = furi_record_open(RECORD_BT);
    context->is_advertising = false;
    context->is_connected = false;
    bt_disconnect(context->bt);
    furi_delay_ms(200);
    bt_keys_storage_set_storage_path(context->bt, HID_BT_KEYS_STORAGE_PATH);

    BleProfileHidParams ble_params = {.device_name_prefix = "TOTP", .mac_xor = mac_xor};
    context->ble_hid_profile = bt_profile_start(context->bt, ble_profile_hid, &ble_params);
    furi_check(context->ble_hid_profile);

    furi_hal_bt_start_advertising();

    context->is_advertising = true;
    bt_set_status_changed_callback(context->bt, connection_status_changed_callback, context);

    return context;
}

void totp_bt_type_code_worker_free(TotpBtTypeCodeWorkerContext* context) {
    furi_check(context != NULL);

    if(context->thread != NULL) {
        totp_bt_type_code_worker_stop(context);
    }

    bt_set_status_changed_callback(context->bt, NULL, NULL);

    furi_hal_bt_stop_advertising();
    context->is_advertising = false;
    context->is_connected = false;

    bt_disconnect(context->bt);
    furi_delay_ms(200);
    bt_keys_storage_set_default_path(context->bt);
    if(!bt_profile_restore_default(context->bt)) {
        FURI_LOG_E(LOGGING_TAG, "Failed to restore to default BT profile");
    }

    furi_record_close(RECORD_BT);
    context->bt = NULL;

    free(context);
}

bool totp_bt_type_code_worker_is_advertising(const TotpBtTypeCodeWorkerContext* context) {
    return context->is_advertising;
}
