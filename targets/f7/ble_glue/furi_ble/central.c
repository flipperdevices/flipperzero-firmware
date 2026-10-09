#include "central.h"

#include "app_common.h"
#include "event_dispatcher.h"
#include <ble/ble.h>
#include <core/mutex.h>
#include <furi.h>
#include <furi_hal_bt.h>

#define TAG "BleCentral"

/* aci_gatt_set_event_mask replaces the complete controller-side GATT/ATT event
 * mask. Enable all events used by the client, including fragmented reads and
 * notifications. Bit 10 is reserved; bit 19 is multiple-notification. */
#define GATT_EVENT_MASK_ALL 0x007FFBFFU

/* 0x0060 * 0.625ms = 60ms interval, 0x0030 = 30ms window: a 50% duty cycle,
 * fast enough to catch a 100ms-ish advertiser without saturating the radio. */
#define BLE_CENTRAL_SCAN_INTERVAL 0x0060
#define BLE_CENTRAL_SCAN_WINDOW   0x0030

/* Interval 24-40 (30-50ms), 0 latency, 5000ms supervision timeout. The longer
 * timeout leaves room for a peripheral's subsequent connection-parameter
 * request while remaining within the Core specification limits. */
#define BLE_CENTRAL_CONN_INTERVAL_MIN     0x0018
#define BLE_CENTRAL_CONN_INTERVAL_MAX     0x0028
#define BLE_CENTRAL_SUPERVISION_TIMEOUT   0x01F4
/* Link termination may complete only when supervision detects that an
 * unreachable peer is gone. Allow that configured 5s timeout plus controller
 * event-delivery margin before reporting teardown failure. */
#define BLE_CENTRAL_DISCONNECT_TIMEOUT_MS (BLE_CENTRAL_SUPERVISION_TIMEOUT * 10UL + 1000UL)

typedef struct {
    /* Guards the scan fields below and nothing else. Scanning is the one piece of
     * state a caller can change while the BLE event thread is reading it: every
     * other field is touched only between proc_begin and the matching wait, which
     * already serialises the two threads. The event thread must never block on a
     * mutex a caller can hold across an ACI call. */
    FuriMutex* mutex;
    /* Serialises every GAP/GATT client operation. The controller permits only
     * one client procedure at a time and all procedure results share the fields
     * below, so the semaphore alone is not sufficient to protect callers from
     * each other. */
    FuriMutex* op_mutex;
    GapSvcEventHandler* event_handler;
    bool scanning;
    bool scan_callback_active;
    FuriSemaphore* scan_callback_done;
    uint8_t last_status;
    BleCentralScanCallback scan_callback;
    void* scan_context;

    /* Connection state */
    bool connected;
    bool connection_usable;
    uint16_t connection_handle;
    uint16_t att_mtu;
    FuriSemaphore* proc_sem;
    FuriSemaphore* disconnect_sem;
    uint8_t proc_error;
    bool connect_in_progress;
    bool connect_cancelled;

    /* Discovery results, filled from ATT responses during a find. */
    uint16_t disc_service_start;
    uint16_t disc_service_end;
    uint16_t disc_value_handle;
    uint8_t disc_char_uuid[16];
    uint8_t disc_uuid_len;
    bool disc_matched;
    uint16_t disc_cccd_handle;
    bool disc_desc_done;

    /* Enumeration callbacks, live only for the duration of one enumerate call. */
    BleCentralServiceCallback svc_callback;
    BleCentralCharCallback chr_callback;
    void* enum_context;
    FuriMutex* enum_mutex;
    uint8_t read_len;
    uint8_t* read_buf;
    uint8_t read_capacity;

    BleCentralNotifyCallback notify_callback;
    void* notify_context;
    FuriMutex* notify_mutex;
    uint16_t notify_ext_handle;
    uint16_t notify_ext_len;
    uint8_t notify_ext_buf[CFG_BLE_MAX_ATT_MTU - 3];
} BleCentral;

static BleCentral* ble_central = NULL;

static BleEventAckStatus ble_central_event_handler(void* event_raw, void* context);
static void ble_central_op_lock(void);
static void ble_central_op_unlock(void);
static void ble_central_scan_callback_clear(void);
static bool ble_central_disconnect_locked(uint32_t timeout_ms);

void ble_central_init(void) {
    if(ble_central) return;

    ble_central = malloc(sizeof(BleCentral));
    memset(ble_central, 0, sizeof(BleCentral));

    /* Configure this after both GATT and GAP initialization. The Full stack's
     * default is firmware-version dependent, and a CCCD write can succeed while
     * incoming notifications remain masked. */
    tBleStatus status = aci_gatt_set_event_mask(GATT_EVENT_MASK_ALL);
    if(status) {
        FURI_LOG_E(TAG, "failed enabling GATT client events: %d", status);
    }
    ble_central->proc_sem = furi_semaphore_alloc(1, 0);
    ble_central->disconnect_sem = furi_semaphore_alloc(1, 0);
    ble_central->scan_callback_done = furi_semaphore_alloc(1, 0);
    ble_central->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ble_central->op_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ble_central->enum_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ble_central->notify_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    ble_central->event_handler =
        ble_event_dispatcher_register_svc_handler(ble_central_event_handler, ble_central);
}

uint8_t ble_central_last_status(void) {
    if(!ble_central) return 0xFF;
    return ble_central->last_status;
}

bool ble_central_deinit(void) {
    if(!ble_central) return true;

    /* Do this before any deferred-teardown return. Even when the controller
     * refuses to stop scanning, application-owned notification context must no
     * longer be reachable after deinit returns. */
    furi_check(furi_mutex_acquire(ble_central->notify_mutex, FuriWaitForever) == FuriStatusOk);
    ble_central->notify_callback = NULL;
    ble_central->notify_context = NULL;
    furi_check(furi_mutex_release(ble_central->notify_mutex) == FuriStatusOk);

    ble_central_scan_stop();
    if(ble_central_is_scanning()) {
        /* Public scan_stop preserves the callback when termination is rejected,
         * but deinit must not leave application code/context reachable after the
         * caller unloads. Keep only the firmware-owned handler alive so a later
         * deinit call can retry controller termination safely. */
        ble_central_scan_callback_clear();
        FURI_LOG_E(TAG, "deinit deferred; scan callback detached while termination is pending");
        return false;
    }

    ble_central_op_lock();
    if(ble_central->connect_in_progress) {
        ble_central->connect_cancelled = true;
        aci_gap_terminate_gap_proc(GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC);
        furi_semaphore_acquire(ble_central->proc_sem, 2000);
    }
    if(ble_central->connected) {
        ble_central_disconnect_locked(BLE_CENTRAL_DISCONNECT_TIMEOUT_MS);
    }
    if(ble_central->connect_in_progress || ble_central->connected) {
        /* The event handler must remain registered while a late connection or
         * disconnection event can still reference this object. */
        FURI_LOG_E(TAG, "deinit deferred while link termination is pending");
        ble_central_op_unlock();
        return false;
    }
    ble_central_op_unlock();
    ble_event_dispatcher_unregister_svc_handler(ble_central->event_handler);
    furi_semaphore_free(ble_central->proc_sem);
    furi_semaphore_free(ble_central->disconnect_sem);
    furi_semaphore_free(ble_central->scan_callback_done);
    furi_mutex_free(ble_central->mutex);
    furi_mutex_free(ble_central->op_mutex);
    furi_mutex_free(ble_central->enum_mutex);
    furi_mutex_free(ble_central->notify_mutex);
    free(ble_central);
    ble_central = NULL;
    return true;
}

/* Clears any stale completion and arms the error slot. MUST be called before
 * issuing the ACI command, never after: the BLE event thread can deliver
 * ACI_GATT_PROC_COMPLETE between the command returning and the wait starting,
 * and draining at that point swallows the very completion we are about to wait
 * for, which shows up as an intermittent timeout. */
static void ble_central_proc_begin(void) {
    while(furi_semaphore_acquire(ble_central->proc_sem, 0) == FuriStatusOk) {
    }
    ble_central->proc_error = 0;
}

static void ble_central_op_lock(void) {
    furi_check(furi_mutex_acquire(ble_central->op_mutex, FuriWaitForever) == FuriStatusOk);
}

static void ble_central_op_unlock(void) {
    furi_check(furi_mutex_release(ble_central->op_mutex) == FuriStatusOk);
}

/* The BLE event thread holds enum_mutex throughout each user callback. Clearing
 * through this helper therefore also waits for a callback already using the
 * caller-owned context to finish. */
static void ble_central_enum_callback_clear(void) {
    furi_check(furi_mutex_acquire(ble_central->enum_mutex, FuriWaitForever) == FuriStatusOk);
    ble_central->svc_callback = NULL;
    ble_central->chr_callback = NULL;
    ble_central->enum_context = NULL;
    furi_check(furi_mutex_release(ble_central->enum_mutex) == FuriStatusOk);
}

static void ble_central_scan_callback_clear(void) {
    furi_check(furi_mutex_acquire(ble_central->mutex, FuriWaitForever) == FuriStatusOk);
    ble_central->scan_callback = NULL;
    ble_central->scan_context = NULL;
    bool wait_for_callback = ble_central->scan_callback_active;
    furi_check(furi_mutex_release(ble_central->mutex) == FuriStatusOk);

    if(wait_for_callback) {
        furi_check(
            furi_semaphore_acquire(ble_central->scan_callback_done, FuriWaitForever) ==
            FuriStatusOk);
    }
}

static void ble_central_disconnect_begin(void) {
    while(furi_semaphore_acquire(ble_central->disconnect_sem, 0) == FuriStatusOk) {
    }
}

/* Called with op_mutex held. Connection state is deliberately left intact when
 * the event does not arrive: a late event must still be recognised and kept
 * away from the peripheral GAP state machine. */
static bool ble_central_disconnect_locked(uint32_t timeout_ms) {
    if(!ble_central->connected) return false;

    ble_central_disconnect_begin();
    tBleStatus status = aci_gap_terminate(ble_central->connection_handle, 0x13);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        FURI_LOG_E(TAG, "aci_gap_terminate failed: 0x%02X", status);
        return !ble_central->connected;
    }

    if(furi_semaphore_acquire(ble_central->disconnect_sem, timeout_ms) != FuriStatusOk) {
        FURI_LOG_E(TAG, "disconnect timed out after %lums", timeout_ms);
        return false;
    }
    return !ble_central->connected;
}

/* Blocks until the awaited event releases the semaphore. */
static bool ble_central_wait_proc(uint32_t timeout_ms) {
    if(furi_semaphore_acquire(ble_central->proc_sem, timeout_ms) != FuriStatusOk) {
        FURI_LOG_E(TAG, "procedure timed out after %lums", timeout_ms);
        /* There is no procedure identifier in ACI_GATT_PROC_COMPLETE. If this
         * completion were allowed to arrive after the next command started, it
         * could satisfy the next wait and associate its result with the wrong
         * operation. Tear down the link before another operation is admitted. */
        ble_central->connection_usable = false;
        ble_central_disconnect_locked(BLE_CENTRAL_DISCONNECT_TIMEOUT_MS);
        return false;
    }
    if(ble_central->proc_error != 0) {
        FURI_LOG_E(TAG, "procedure failed: 0x%02X", ble_central->proc_error);
        return false;
    }
    return true;
}

bool ble_central_is_connected(void) {
    return ble_central && ble_central->connected;
}

static bool ble_central_can_operate(void) {
    return ble_central->connected && ble_central->connection_usable;
}

uint16_t ble_central_connection_handle(void) {
    return ble_central ? ble_central->connection_handle : 0;
}

void ble_central_set_notify_callback(BleCentralNotifyCallback callback, void* context) {
    furi_check(ble_central);
    /* The event thread holds this lock throughout callback execution. Thus, when
     * this function returns, a replaced context is no longer in use. */
    furi_check(furi_mutex_acquire(ble_central->notify_mutex, FuriWaitForever) == FuriStatusOk);
    ble_central->notify_callback = callback;
    ble_central->notify_context = context;
    furi_check(furi_mutex_release(ble_central->notify_mutex) == FuriStatusOk);
}

bool ble_central_connect(
    const uint8_t addr[BLE_CENTRAL_ADDR_LEN],
    uint8_t addr_type,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(addr);
    ble_central_op_lock();
    bool result = false;

    if(ble_central->connected || ble_central->connect_in_progress) {
        FURI_LOG_W(TAG, "already connected");
        goto out;
    }
    /* Scanning owns a GAP procedure; connecting is another one. */
    if(ble_central->scanning && !ble_central_scan_stop()) goto out;

    ble_central_proc_begin();
    ble_central->connect_in_progress = true;
    ble_central->connect_cancelled = false;
    tBleStatus status = aci_gap_create_connection(
        BLE_CENTRAL_SCAN_INTERVAL,
        BLE_CENTRAL_SCAN_WINDOW,
        addr_type,
        addr,
        1, /* own address type: random, matching the scan path */
        BLE_CENTRAL_CONN_INTERVAL_MIN,
        BLE_CENTRAL_CONN_INTERVAL_MAX,
        0,
        BLE_CENTRAL_SUPERVISION_TIMEOUT,
        0,
        0);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        ble_central->connect_in_progress = false;
        FURI_LOG_E(TAG, "aci_gap_create_connection failed: 0x%02X", status);
        goto out;
    }
    if(furi_semaphore_acquire(ble_central->proc_sem, timeout_ms) != FuriStatusOk) {
        FURI_LOG_E(TAG, "connection timed out after %lums", timeout_ms);
        ble_central->connect_cancelled = true;
        aci_gap_terminate_gap_proc(GAP_DIRECT_CONNECTION_ESTABLISHMENT_PROC);
        /* Cancellation itself completes asynchronously. Consume that completion
         * before admitting a later connect whenever the controller responds in
         * time; otherwise connect_in_progress keeps later calls quarantined. */
        if(furi_semaphore_acquire(ble_central->proc_sem, 2000) == FuriStatusOk &&
           ble_central->connected) {
            ble_central_disconnect_locked(BLE_CENTRAL_DISCONNECT_TIMEOUT_MS);
        }
        goto out;
    }
    if(ble_central->proc_error != 0 || !ble_central->connected) {
        FURI_LOG_E(TAG, "connection failed: 0x%02X", ble_central->proc_error);
        goto out;
    }
    FURI_LOG_I(TAG, "connected, handle 0x%04X", ble_central->connection_handle);
    result = true;

out:
    ble_central_op_unlock();
    return result;
}

bool ble_central_disconnect(void) {
    furi_check(ble_central);
    ble_central_op_lock();
    bool result = ble_central_disconnect_locked(BLE_CENTRAL_DISCONNECT_TIMEOUT_MS);
    ble_central_op_unlock();
    return result;
}

bool ble_central_exchange_mtu(uint32_t timeout_ms) {
    furi_check(ble_central);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;
    ble_central_proc_begin();
    tBleStatus status = aci_gatt_exchange_config(ble_central->connection_handle);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        FURI_LOG_E(TAG, "aci_gatt_exchange_config failed: 0x%02X", status);
        goto out;
    }
    result = ble_central_wait_proc(timeout_ms);

out:
    ble_central_op_unlock();
    return result;
}

static bool ble_central_find_characteristic(
    uint8_t uuid_type,
    const uint8_t* service_uuid,
    const uint8_t* char_uuid,
    uint16_t* out_value_handle,
    uint16_t* out_cccd_handle,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(out_value_handle);
    if(!ble_central_can_operate()) return false;

    uint8_t uuid_len = (uuid_type == UUID_TYPE_16) ? 2 : 16;

    ble_central->disc_service_start = 0;
    ble_central->disc_service_end = 0;
    ble_central->disc_value_handle = 0;
    ble_central->disc_matched = false;
    ble_central->disc_uuid_len = uuid_len;
    memcpy(ble_central->disc_char_uuid, char_uuid, uuid_len);

    UUID_t service_uuid_arg;
    memcpy(&service_uuid_arg, service_uuid, uuid_len);

    ble_central_proc_begin();
    tBleStatus status = aci_gatt_disc_primary_service_by_uuid(
        ble_central->connection_handle, uuid_type, &service_uuid_arg);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        FURI_LOG_E(TAG, "service discovery failed: 0x%02X", status);
        return false;
    }
    if(!ble_central_wait_proc(timeout_ms)) return false;
    if(ble_central->disc_service_start == 0) {
        FURI_LOG_E(TAG, "service not found");
        return false;
    }

    ble_central_proc_begin();
    status = aci_gatt_disc_all_char_of_service(
        ble_central->connection_handle,
        ble_central->disc_service_start,
        ble_central->disc_service_end);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        FURI_LOG_E(TAG, "characteristic discovery failed: 0x%02X", status);
        return false;
    }
    if(!ble_central_wait_proc(timeout_ms)) return false;
    if(!ble_central->disc_matched) {
        FURI_LOG_E(TAG, "characteristic not found in service");
        return false;
    }

    *out_value_handle = ble_central->disc_value_handle;

    /* Only look for a CCCD if the caller wants one. Discover it rather than
     * assuming it follows the value handle: writing to the wrong descriptor
     * handle still succeeds and is indistinguishable from a real subscription
     * until nothing ever arrives. */
    if(out_cccd_handle) {
        ble_central->disc_cccd_handle = 0;
        ble_central->disc_desc_done = false;
        if(ble_central->disc_value_handle == ble_central->disc_service_end) {
            *out_cccd_handle = 0;
            goto found;
        }
        ble_central_proc_begin();
        status = aci_gatt_disc_all_char_desc(
            ble_central->connection_handle,
            ble_central->disc_value_handle,
            ble_central->disc_service_end);
        ble_central->last_status = status;
        if(status != BLE_STATUS_SUCCESS) {
            FURI_LOG_E(TAG, "descriptor discovery failed: 0x%02X", status);
            return false;
        }
        if(!ble_central_wait_proc(timeout_ms)) return false;
        /* A read-only characteristic legitimately has no CCCD, so absence is
         * reported as handle 0 rather than treated as failure. */
        *out_cccd_handle = ble_central->disc_cccd_handle;
    }

found:
    FURI_LOG_I(
        TAG,
        "value handle 0x%04X, cccd 0x%04X",
        *out_value_handle,
        out_cccd_handle ? *out_cccd_handle : 0);
    return true;
}

bool ble_central_find_characteristic_128(
    const uint8_t service_uuid[16],
    const uint8_t char_uuid[16],
    uint16_t* out_value_handle,
    uint16_t* out_cccd_handle,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(service_uuid);
    furi_check(char_uuid);
    ble_central_op_lock();
    bool result = ble_central_find_characteristic(
        UUID_TYPE_128, service_uuid, char_uuid, out_value_handle, out_cccd_handle, timeout_ms);
    ble_central_op_unlock();
    return result;
}

bool ble_central_find_characteristic_16(
    uint16_t service_uuid,
    uint16_t char_uuid,
    uint16_t* out_value_handle,
    uint16_t* out_cccd_handle,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    ble_central_op_lock();
    bool result = ble_central_find_characteristic(
        UUID_TYPE_16,
        (const uint8_t*)&service_uuid,
        (const uint8_t*)&char_uuid,
        out_value_handle,
        out_cccd_handle,
        timeout_ms);
    ble_central_op_unlock();
    return result;
}

bool ble_central_enumerate_services(
    BleCentralServiceCallback callback,
    void* context,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(callback);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;

    furi_check(furi_mutex_acquire(ble_central->enum_mutex, FuriWaitForever) == FuriStatusOk);
    ble_central->svc_callback = callback;
    ble_central->enum_context = context;
    furi_check(furi_mutex_release(ble_central->enum_mutex) == FuriStatusOk);

    ble_central_proc_begin();
    tBleStatus status = aci_gatt_disc_all_primary_services(ble_central->connection_handle);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        ble_central_enum_callback_clear();
        FURI_LOG_E(TAG, "service enumeration failed: 0x%02X", status);
        goto out;
    }
    result = ble_central_wait_proc(timeout_ms);
    ble_central_enum_callback_clear();

out:
    ble_central_op_unlock();
    return result;
}

bool ble_central_enumerate_characteristics(
    uint16_t start_handle,
    uint16_t end_handle,
    BleCentralCharCallback callback,
    void* context,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(callback);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;

    furi_check(furi_mutex_acquire(ble_central->enum_mutex, FuriWaitForever) == FuriStatusOk);
    ble_central->chr_callback = callback;
    ble_central->enum_context = context;
    furi_check(furi_mutex_release(ble_central->enum_mutex) == FuriStatusOk);

    ble_central_proc_begin();
    tBleStatus status = aci_gatt_disc_all_char_of_service(
        ble_central->connection_handle, start_handle, end_handle);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        ble_central_enum_callback_clear();
        FURI_LOG_E(TAG, "characteristic enumeration failed: 0x%02X", status);
        goto out;
    }
    result = ble_central_wait_proc(timeout_ms);
    ble_central_enum_callback_clear();

out:
    ble_central_op_unlock();
    return result;
}

bool ble_central_find_cccd(
    uint16_t value_handle,
    uint16_t service_end_handle,
    uint16_t* out_cccd_handle,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(out_cccd_handle);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;

    ble_central->disc_cccd_handle = 0;
    ble_central->disc_desc_done = false;
    if(value_handle > service_end_handle) goto out;
    if(value_handle == service_end_handle) {
        *out_cccd_handle = 0;
        result = true;
        goto out;
    }
    ble_central_proc_begin();
    tBleStatus status = aci_gatt_disc_all_char_desc(
        ble_central->connection_handle, value_handle, service_end_handle);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) goto out;
    if(!ble_central_wait_proc(timeout_ms)) goto out;
    *out_cccd_handle = ble_central->disc_cccd_handle;
    result = true;

out:
    ble_central_op_unlock();
    return result;
}

bool ble_central_unsubscribe(uint16_t cccd_handle, uint32_t timeout_ms) {
    furi_check(ble_central);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;
    const uint8_t disable[2] = {0x00, 0x00};
    ble_central_proc_begin();
    tBleStatus status = aci_gatt_write_char_desc(
        ble_central->connection_handle, cccd_handle, sizeof(disable), disable);
    ble_central->last_status = status;
    if(status == BLE_STATUS_SUCCESS) result = ble_central_wait_proc(timeout_ms);

out:
    ble_central_op_unlock();
    return result;
}

bool ble_central_subscribe(uint16_t cccd_handle, uint32_t timeout_ms) {
    furi_check(ble_central);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;
    /* This API subscribes to notifications, whose standard CCCD value is
     * 0x0001. Indication-only characteristics require an explicit 0x0002 write. */
    const uint8_t enable_notifications[2] = {0x01, 0x00};
    ble_central_proc_begin();
    tBleStatus status = aci_gatt_write_char_desc(
        ble_central->connection_handle,
        cccd_handle,
        sizeof(enable_notifications),
        enable_notifications);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) {
        FURI_LOG_E(TAG, "cccd write failed: 0x%02X", status);
        goto out;
    }
    result = ble_central_wait_proc(timeout_ms);

out:
    ble_central_op_unlock();
    return result;
}

bool ble_central_read(
    uint16_t handle,
    uint8_t* buf,
    uint8_t buf_size,
    uint8_t* out_len,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(buf && out_len);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;
    ble_central->read_len = 0;
    ble_central->read_buf = buf;
    ble_central->read_capacity = buf_size;
    ble_central_proc_begin();
    /* Start with an ordinary read: some servers reject Read Blob for attributes
     * that fit in one ATT response. If that response is completely full and the
     * caller has more room, continue from its end with the long-read procedure. */
    tBleStatus status = aci_gatt_read_char_value(ble_central->connection_handle, handle);
    ble_central->last_status = status;
    if(status != BLE_STATUS_SUCCESS) goto out;
    if(!ble_central_wait_proc(timeout_ms)) goto out;
    if(ble_central->read_len == ble_central->att_mtu - 1 &&
       ble_central->read_len < ble_central->read_capacity) {
        uint16_t offset = ble_central->read_len;
        ble_central_proc_begin();
        status = aci_gatt_read_long_char_value(ble_central->connection_handle, handle, offset);
        ble_central->last_status = status;
        if(status != BLE_STATUS_SUCCESS) goto out;
        if(!ble_central_wait_proc(timeout_ms) && ble_central->proc_error != 0x07 &&
           ble_central->proc_error != 0x0B) {
            goto out;
        }
    }
    *out_len = ble_central->read_len;
    /* A zero-length value is a successful read of an empty characteristic, which
     * several Device Information strings legitimately are. Callers distinguish
     * via out_len. */
    result = true;

out:
    ble_central->read_buf = NULL;
    ble_central->read_capacity = 0;
    ble_central_op_unlock();
    return result;
}

bool ble_central_write(
    uint16_t value_handle,
    const uint8_t* data,
    uint8_t len,
    bool write_response,
    uint32_t timeout_ms) {
    furi_check(ble_central);
    furi_check(data || len == 0);
    ble_central_op_lock();
    bool result = false;
    if(!ble_central_can_operate()) goto out;

    tBleStatus status;
    if(write_response) {
        ble_central_proc_begin();
        status =
            aci_gatt_write_char_value(ble_central->connection_handle, value_handle, len, data);
        ble_central->last_status = status;
        if(status == BLE_STATUS_SUCCESS) result = ble_central_wait_proc(timeout_ms);
        goto out;
    }

    status = aci_gatt_write_without_resp(ble_central->connection_handle, value_handle, len, data);
    ble_central->last_status = status;
    result = status == BLE_STATUS_SUCCESS;

out:
    ble_central_op_unlock();
    return result;
}

bool ble_central_adv_find(
    const BleCentralAdvReport* report,
    uint8_t ad_type,
    const uint8_t** out_value,
    uint8_t* out_len) {
    furi_check(report);
    /* Each record is [length][type][value...], where length covers type+value,
     * so a record occupies len+1 bytes and is only valid if it fits entirely. */
    for(uint8_t i = 0; i + 1 < report->data_len;) {
        uint8_t len = report->data[i];
        if(len == 0 || i + len + 1 > report->data_len) break;
        if(report->data[i + 1] == ad_type) {
            if(out_value) *out_value = &report->data[i + 2];
            if(out_len) *out_len = len - 1;
            return true;
        }
        i += len + 1;
    }
    return false;
}

bool ble_central_scan_start(BleCentralScanCallback callback, void* context) {
    furi_check(ble_central);
    furi_check(callback);

    if(!furi_hal_bt_is_central_supported()) {
        ble_central->last_status = BLE_STATUS_FAILED;
        FURI_LOG_E(TAG, "central role requires the Full BLE radio stack");
        return false;
    }

    furi_check(furi_mutex_acquire(ble_central->mutex, FuriWaitForever) == FuriStatusOk);
    bool started = false;

    if(ble_central->scanning) {
        FURI_LOG_W(TAG, "scan already running");
    } else {
        /* Own_Address_Type 1 = random: gap.c programs a random static address via
         * CONFIG_DATA_RANDOM_ADDRESS_OFFSET and never sets a public one, so
         * scanning as public yields no reports. Filter_Duplicates 0 so we keep
         * seeing a device as its RSSI changes and its resolvable address rotates. */
        tBleStatus status = aci_gap_start_general_discovery_proc(
            BLE_CENTRAL_SCAN_INTERVAL, BLE_CENTRAL_SCAN_WINDOW, 1, 0);
        ble_central->last_status = status;
        if(status == BLE_STATUS_SUCCESS) {
            ble_central->scan_callback = callback;
            ble_central->scan_context = context;
            ble_central->scanning = true;
            started = true;
            FURI_LOG_I(TAG, "scan started");
        } else {
            FURI_LOG_E(TAG, "aci_gap_start_general_discovery_proc failed: %d", status);
        }
    }

    furi_mutex_release(ble_central->mutex);
    return started;
}

bool ble_central_scan_stop(void) {
    furi_check(ble_central);

    furi_check(furi_mutex_acquire(ble_central->mutex, FuriWaitForever) == FuriStatusOk);
    /* Stopping an already-completed discovery procedure is idempotent. The
     * controller can end general discovery on its own and report that through
     * ACI_GAP_PROC_COMPLETE before the caller's timer expires. */
    bool stopped = !ble_central->scanning;
    bool wait_for_callback = false;

    if(ble_central->scanning) {
        tBleStatus status = aci_gap_terminate_gap_proc(GAP_GENERAL_DISCOVERY_PROC);
        ble_central->last_status = status;
        if(status != BLE_STATUS_SUCCESS) {
            FURI_LOG_W(TAG, "aci_gap_terminate_gap_proc failed: %d", status);
        } else {
            ble_central->scanning = false;
            ble_central->scan_callback = NULL;
            ble_central->scan_context = NULL;
            wait_for_callback = ble_central->scan_callback_active;
            stopped = true;
            FURI_LOG_I(TAG, "scan stopped");
        }
    }

    furi_mutex_release(ble_central->mutex);
    if(wait_for_callback) {
        furi_check(
            furi_semaphore_acquire(ble_central->scan_callback_done, FuriWaitForever) ==
            FuriStatusOk);
    }
    return stopped;
}

bool ble_central_is_scanning(void) {
    if(!ble_central) return false;
    furi_check(furi_mutex_acquire(ble_central->mutex, FuriWaitForever) == FuriStatusOk);
    bool scanning = ble_central->scanning;
    furi_mutex_release(ble_central->mutex);
    return scanning;
}

/* Parses one HCI LE Advertising Report. Layout per report, after Num_Reports:
 *   Event_Type(1) Address_Type(1) Address(6) Length_Data(1) Data(Length_Data) RSSI(1)
 * Parsed by hand rather than via the vendor typedef to avoid struct-packing
 * assumptions across a variable-length field. */
static void ble_central_handle_adv_report(const uint8_t* data, uint8_t len) {
    BleCentralScanCallback callback;
    void* context;

    furi_check(furi_mutex_acquire(ble_central->mutex, FuriWaitForever) == FuriStatusOk);
    callback = ble_central->scan_callback;
    context = ble_central->scan_context;
    if(callback && len >= 1) {
        while(furi_semaphore_acquire(ble_central->scan_callback_done, 0) == FuriStatusOk) {
        }
        ble_central->scan_callback_active = true;
    }
    furi_mutex_release(ble_central->mutex);

    if(!callback || len < 1) return;

    uint8_t num_reports = data[0];
    size_t offset = 1;

    for(uint8_t i = 0; i < num_reports; i++) {
        /* Fixed part is Event_Type + Address_Type + Address + Length_Data = 9 bytes. */
        if(offset + 9 > len) break;

        uint8_t event_type = data[offset];
        uint8_t addr_type = data[offset + 1];
        const uint8_t* addr = &data[offset + 2];
        uint8_t data_len = data[offset + 8];
        offset += 9;

        /* Payload plus the trailing RSSI byte must both be present. */
        if(offset + data_len + 1 > len) break;

        BleCentralAdvReport report;
        memcpy(report.addr, addr, BLE_CENTRAL_ADDR_LEN);
        report.addr_type = addr_type;
        report.event_type = event_type;
        report.data_len = MIN(data_len, (uint8_t)BLE_CENTRAL_ADV_DATA_MAX);
        memcpy(report.data, &data[offset], report.data_len);
        report.rssi = (int8_t)data[offset + data_len];
        offset += data_len + 1;

        callback(&report, context);
    }

    furi_check(furi_mutex_acquire(ble_central->mutex, FuriWaitForever) == FuriStatusOk);
    ble_central->scan_callback_active = false;
    furi_check(furi_semaphore_release(ble_central->scan_callback_done) == FuriStatusOk);
    furi_check(furi_mutex_release(ble_central->mutex) == FuriStatusOk);
}

/* Scans a Read By Type response for the target characteristic UUID.
 * Each record is: decl_handle(2) properties(1) value_handle(2) uuid(2 or 16),
 * so a 128-bit characteristic gives a 21-byte record. */
static void ble_central_handle_char_disc(const aci_att_read_by_type_resp_event_rp0* resp) {
    uint8_t record_len = resp->Handle_Value_Pair_Length;
    uint8_t total = resp->Data_Length;
    if(record_len < 7 || total < record_len) return;

    furi_check(furi_mutex_acquire(ble_central->enum_mutex, FuriWaitForever) == FuriStatusOk);
    BleCentralCharCallback callback = ble_central->chr_callback;
    void* context = ble_central->enum_context;
    if(callback) {
        for(uint8_t offset = 0; offset + record_len <= total; offset += record_len) {
            const uint8_t* record = &resp->Handle_Value_Pair_Data[offset];
            callback(record[3] | (record[4] << 8), record[2], &record[5], record_len - 5, context);
        }
        furi_check(furi_mutex_release(ble_central->enum_mutex) == FuriStatusOk);
        return;
    }
    furi_check(furi_mutex_release(ble_central->enum_mutex) == FuriStatusOk);

    for(uint8_t offset = 0; offset + record_len <= total; offset += record_len) {
        const uint8_t* record = &resp->Handle_Value_Pair_Data[offset];
        uint16_t value_handle = record[3] | (record[4] << 8);
        uint8_t uuid_len = record_len - 5;

        /* Searching: stop at the first match. */
        if(uuid_len == ble_central->disc_uuid_len &&
           memcmp(&record[5], ble_central->disc_char_uuid, uuid_len) == 0) {
            ble_central->disc_value_handle = value_handle;
            ble_central->disc_matched = true;
            return;
        }
    }
}

static BleEventAckStatus ble_central_event_handler(void* event_raw, void* context) {
    UNUSED(context);
    /* Registered handlers run before gap's own app notification, which is the
     * ordering central needs: a central-role connection event must not reach
     * gap, or gap will mistake it for its own peripheral link. */
    hci_event_pckt* event = (hci_event_pckt*)(((hci_uart_pckt*)event_raw)->data);

    if(event->evt == HCI_LE_META_EVT_CODE) {
        /* plen counts the subevent byte, so it must be at least 1 before that
         * byte can be read, and `plen - 1` below would otherwise wrap to 255 and
         * hand the parser a length far past the end of the buffer. */
        if(event->plen < 1) return BleEventNotAck;
        evt_le_meta_event* meta_evt = (evt_le_meta_event*)event->data;

        if(meta_evt->subevent == HCI_LE_ADVERTISING_REPORT_SUBEVT_CODE) {
            if(!ble_central_is_scanning()) return BleEventNotAck;
            /* meta_evt->data starts at Num_Reports. The HCI event length lives in
             * the outer packet, so bound the parse with it. */
            ble_central_handle_adv_report(meta_evt->data, event->plen - 1);
            return BleEventAckFlowEnable;
        }

        if(meta_evt->subevent == HCI_LE_CONNECTION_COMPLETE_SUBEVT_CODE) {
            hci_le_connection_complete_event_rp0* conn =
                (hci_le_connection_complete_event_rp0*)meta_evt->data;
            /* Role 0x00 = central. A peripheral-role connection belongs to gap.c,
             * so leave it alone or gap will lose track of its own link. */
            if(conn->Role != 0x00) return BleEventNotAck;
            ble_central->connect_in_progress = false;
            ble_central->connected = (conn->Status == 0);
            ble_central->connection_usable = ble_central->connected &&
                                             !ble_central->connect_cancelled;
            ble_central->connection_handle = conn->Connection_Handle;
            ble_central->att_mtu = 23;
            ble_central->proc_error = conn->Status;
            furi_semaphore_release(ble_central->proc_sem);
            /* A connection-complete event may race the caller's timeout and the
             * cancellation command. Never leave that late successful link alive
             * after ble_central_connect has reported failure. */
            if(ble_central->connected && ble_central->connect_cancelled) {
                aci_gap_terminate(ble_central->connection_handle, 0x13);
            }
            return BleEventAckFlowEnable;
        }

        /* The link-layer events below need no handling here, but they must not
         * reach gap either: gap applies them to gap->service.connection_handle,
         * which is unset while the only link is this one. That turns a PHY update
         * on our connection into hci_le_read_phy(0), and a connection-update into
         * an L2CAP parameter request against a handle that does not exist. Claim
         * the ones that carry our handle and let everything else fall through. */
        if(meta_evt->subevent == HCI_LE_PHY_UPDATE_COMPLETE_SUBEVT_CODE) {
            hci_le_phy_update_complete_event_rp0* phy =
                (hci_le_phy_update_complete_event_rp0*)meta_evt->data;
            if(ble_central->connected &&
               phy->Connection_Handle == ble_central->connection_handle) {
                return BleEventAckFlowEnable;
            }
        }

        if(meta_evt->subevent == HCI_LE_CONNECTION_UPDATE_COMPLETE_SUBEVT_CODE) {
            hci_le_connection_update_complete_event_rp0* upd =
                (hci_le_connection_update_complete_event_rp0*)meta_evt->data;
            if(ble_central->connected &&
               upd->Connection_Handle == ble_central->connection_handle) {
                return BleEventAckFlowEnable;
            }
        }

        return BleEventNotAck;
    }

    if(event->evt == HCI_DISCONNECTION_COMPLETE_EVT_CODE) {
        hci_disconnection_complete_event_rp0* disc =
            (hci_disconnection_complete_event_rp0*)event->data;
        if(ble_central->connected && disc->Connection_Handle == ble_central->connection_handle) {
            FURI_LOG_I(TAG, "peer disconnected, reason 0x%02X", disc->Reason);
            ble_central->connected = false;
            ble_central->connection_usable = false;
            ble_central->connection_handle = 0;
            /* Releasing the semaphore wakes whatever procedure was in flight, so
             * it must also record why: without this the waiter sees proc_error 0
             * and reports success for a procedure the peer never answered. A
             * dropped link was being read back as a completed MTU exchange. */
            ble_central->proc_error = disc->Reason ? disc->Reason : 0xFF;
            furi_semaphore_release(ble_central->proc_sem);
            furi_semaphore_release(ble_central->disconnect_sem);
            return BleEventAckFlowEnable;
        }
        return BleEventNotAck;
    }

    if(event->evt == HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE) {
        evt_blecore_aci* blue_evt = (evt_blecore_aci*)event->data;

        switch(blue_evt->ecode) {
        case ACI_GAP_PROC_COMPLETE_VSEVT_CODE: {
            aci_gap_proc_complete_event_rp0* complete =
                (aci_gap_proc_complete_event_rp0*)blue_evt->data;
            if(complete->Procedure_Code != GAP_GENERAL_DISCOVERY_PROC) {
                return BleEventNotAck;
            }

            furi_check(furi_mutex_acquire(ble_central->mutex, FuriWaitForever) == FuriStatusOk);
            bool was_scanning = ble_central->scanning;
            ble_central->scanning = false;
            ble_central->scan_callback = NULL;
            ble_central->scan_context = NULL;
            furi_check(furi_mutex_release(ble_central->mutex) == FuriStatusOk);
            if(was_scanning) {
                FURI_LOG_I(TAG, "scan completed by controller, status 0x%02X", complete->Status);
            }
            return BleEventAckFlowEnable;
        }

        case ACI_L2CAP_CONNECTION_UPDATE_REQ_VSEVT_CODE: {
            aci_l2cap_connection_update_req_event_rp0* req =
                (aci_l2cap_connection_update_req_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               req->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }

            /* A peripheral uses this L2CAP request when it cannot perform the
             * Link Layer connection-update procedure itself. The central must
             * answer it explicitly (Core v5.x, Vol 3, Part A, section 4.20).
             * Letting it fall through to gap.c is especially harmful: that code
             * owns the peripheral-role link and interprets this request as a
             * response, so no reply is sent to the central-role peer. */
            bool valid = req->Interval_Min >= 0x0006 && req->Interval_Max <= 0x0C80 &&
                         req->Interval_Min <= req->Interval_Max && req->Latency <= 0x01F3 &&
                         req->Timeout_Multiplier >= 0x000A && req->Timeout_Multiplier <= 0x0C80;
            if(valid) {
                /* The supervision timeout must be greater than twice the
                 * maximum connection interval multiplied by (latency + 1).
                 * Timeout units are 10 ms; interval units are 1.25 ms. */
                valid = (uint32_t)req->Timeout_Multiplier * 4U >
                        (uint32_t)(req->Latency + 1U) * req->Interval_Max;
            }

            tBleStatus status = aci_l2cap_connection_parameter_update_resp(
                req->Connection_Handle,
                req->Interval_Min,
                req->Interval_Max,
                req->Latency,
                req->Timeout_Multiplier,
                0,
                0,
                req->Identifier,
                valid ? 1 : 0);
            FURI_LOG_I(
                TAG,
                "connection parameter request %u-%u latency %u timeout %u: %s (0x%02X)",
                req->Interval_Min,
                req->Interval_Max,
                req->Latency,
                req->Timeout_Multiplier,
                valid ? "accepted" : "rejected",
                status);
            return BleEventAckFlowEnable;
        }

        case ACI_GATT_INDICATION_VSEVT_CODE:
        case ACI_GATT_NOTIFICATION_VSEVT_CODE: {
            /* Both events share the same payload layout. An indication must be
             * confirmed or the peer will not send another one. */
            aci_gatt_notification_event_rp0* notif =
                (aci_gatt_notification_event_rp0*)blue_evt->data;

            /* Logged before the filtering below, because "the peer sent nothing"
             * and "we discarded what it sent" look identical from the outside and
             * are the first thing anyone debugging a silent subscription needs to
             * tell apart. Rate limited: a sensor stream can be very chatty, and
             * this runs on the BLE event thread. */
            static uint32_t seen = 0;
            if(++seen <= 3 || (seen % 64) == 0) {
                FURI_LOG_D(
                    TAG,
                    "notification #%lu conn %04X attr %04X len %u",
                    (unsigned long)seen,
                    notif->Connection_Handle,
                    notif->Attribute_Handle,
                    notif->Attribute_Value_Length);
            }

            if(!ble_central->connected ||
               notif->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            if(blue_evt->ecode == ACI_GATT_INDICATION_VSEVT_CODE) {
                aci_gatt_confirm_indication(ble_central->connection_handle);
            }
            furi_check(
                furi_mutex_acquire(ble_central->notify_mutex, FuriWaitForever) == FuriStatusOk);
            if(ble_central->notify_callback) {
                ble_central->notify_callback(
                    notif->Attribute_Handle,
                    notif->Attribute_Value,
                    notif->Attribute_Value_Length,
                    ble_central->notify_context);
            }
            furi_check(furi_mutex_release(ble_central->notify_mutex) == FuriStatusOk);
            return BleEventAckFlowEnable;
        }

        case ACI_GATT_INDICATION_EXT_VSEVT_CODE:
        case ACI_GATT_NOTIFICATION_EXT_VSEVT_CODE: {
            /* With ATT_MTU > BLE_EVT_MAX_PARAM_LEN - 4, STM32WB replaces the
             * legacy notification event with one or more EXT fragments. Offset
             * bit 15 means more fragments follow; bits 14..0 are the value
             * offset. Reassemble before invoking the application's callback so
             * the legacy and extended paths have identical semantics. */
            aci_gatt_notification_ext_event_rp0* notif =
                (aci_gatt_notification_ext_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               notif->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }

            uint16_t offset = notif->Offset & 0x7FFFU;
            bool more = (notif->Offset & 0x8000U) != 0;
            uint16_t len = notif->Attribute_Value_Length;
            if(offset == 0) {
                ble_central->notify_ext_handle = notif->Attribute_Handle;
                ble_central->notify_ext_len = 0;
            }
            if(notif->Attribute_Handle != ble_central->notify_ext_handle ||
               offset != ble_central->notify_ext_len ||
               offset > sizeof(ble_central->notify_ext_buf) ||
               len > sizeof(ble_central->notify_ext_buf) - offset) {
                FURI_LOG_W(
                    TAG,
                    "dropping malformed notification fragment attr %04X offset %u len %u",
                    notif->Attribute_Handle,
                    offset,
                    len);
                ble_central->notify_ext_len = 0;
                return BleEventAckFlowEnable;
            }

            memcpy(ble_central->notify_ext_buf + offset, notif->Attribute_Value, len);
            ble_central->notify_ext_len += len;
            if(!more) {
                furi_check(
                    furi_mutex_acquire(ble_central->notify_mutex, FuriWaitForever) ==
                    FuriStatusOk);
                if(ble_central->notify_callback) {
                    ble_central->notify_callback(
                        ble_central->notify_ext_handle,
                        ble_central->notify_ext_buf,
                        ble_central->notify_ext_len,
                        ble_central->notify_context);
                }
                furi_check(furi_mutex_release(ble_central->notify_mutex) == FuriStatusOk);
                ble_central->notify_ext_len = 0;
                if(blue_evt->ecode == ACI_GATT_INDICATION_EXT_VSEVT_CODE) {
                    aci_gatt_confirm_indication(ble_central->connection_handle);
                }
            }
            return BleEventAckFlowEnable;
        }

        case ACI_GATT_MULT_NOTIFICATION_VSEVT_CODE: {
            /* Bluetooth 5.2 may bundle notifications as Handle/Length/Value
             * tuples. ST assigns this event the otherwise-undocumented mask bit
             * 19. Like the EXT events, its HCI representation may be fragmented. */
            aci_gatt_mult_notification_event_rp0* notif =
                (aci_gatt_mult_notification_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               notif->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            uint16_t offset = notif->Offset & 0x7FFFU;
            bool more = (notif->Offset & 0x8000U) != 0;
            uint16_t len = notif->Data_Length;
            if(offset == 0) ble_central->notify_ext_len = 0;
            if(offset != ble_central->notify_ext_len ||
               offset > sizeof(ble_central->notify_ext_buf) ||
               len > sizeof(ble_central->notify_ext_buf) - offset) {
                FURI_LOG_W(TAG, "dropping malformed multi-notification fragment");
                ble_central->notify_ext_len = 0;
                return BleEventAckFlowEnable;
            }
            memcpy(ble_central->notify_ext_buf + offset, notif->Data, len);
            ble_central->notify_ext_len += len;
            if(more) return BleEventAckFlowEnable;

            furi_check(
                furi_mutex_acquire(ble_central->notify_mutex, FuriWaitForever) == FuriStatusOk);
            for(uint16_t pos = 0; pos + 4 <= ble_central->notify_ext_len;) {
                const uint8_t* tuple = ble_central->notify_ext_buf + pos;
                uint16_t handle = tuple[0] | (tuple[1] << 8);
                uint16_t value_len = tuple[2] | (tuple[3] << 8);
                pos += 4;
                if(value_len > ble_central->notify_ext_len - pos) {
                    FURI_LOG_W(TAG, "dropping malformed multi-notification tuple");
                    break;
                }
                FURI_LOG_D(TAG, "multi notification attr %04X len %u", handle, value_len);
                if(ble_central->notify_callback) {
                    ble_central->notify_callback(
                        handle,
                        ble_central->notify_ext_buf + pos,
                        value_len,
                        ble_central->notify_context);
                }
                pos += value_len;
            }
            furi_check(furi_mutex_release(ble_central->notify_mutex) == FuriStatusOk);
            ble_central->notify_ext_len = 0;
            return BleEventAckFlowEnable;
        }

        case ACI_ATT_READ_BY_GROUP_TYPE_RESP_VSEVT_CODE: {
            /* Response to disc_all_primary_services. Each entry is
             * start(2) end(2) uuid(2 or 16). */
            aci_att_read_by_group_type_resp_event_rp0* resp =
                (aci_att_read_by_group_type_resp_event_rp0*)blue_evt->data;
            uint8_t entry_len = resp->Attribute_Data_Length;
            uint8_t total = resp->Data_Length;
            if(entry_len < 6) return BleEventAckFlowEnable;

            furi_check(
                furi_mutex_acquire(ble_central->enum_mutex, FuriWaitForever) == FuriStatusOk);
            BleCentralServiceCallback callback = ble_central->svc_callback;
            void* context = ble_central->enum_context;
            if(!callback) {
                furi_check(furi_mutex_release(ble_central->enum_mutex) == FuriStatusOk);
                return BleEventAckFlowEnable;
            }
            for(uint8_t offset = 0; offset + entry_len <= total; offset += entry_len) {
                const uint8_t* e = &resp->Attribute_Data_List[offset];
                callback(e[0] | (e[1] << 8), e[2] | (e[3] << 8), &e[4], entry_len - 4, context);
            }
            furi_check(furi_mutex_release(ble_central->enum_mutex) == FuriStatusOk);
            return BleEventAckFlowEnable;
        }

        case ACI_ATT_FIND_BY_TYPE_VALUE_RESP_VSEVT_CODE: {
            /* Response to disc_primary_service_by_uuid. Take the first pair;
             * this protocol has exactly one instance of the service. */
            aci_att_find_by_type_value_resp_event_rp0* resp =
                (aci_att_find_by_type_value_resp_event_rp0*)blue_evt->data;
            if(ble_central->disc_service_start == 0 && resp->Num_of_Handle_Pair > 0) {
                ble_central->disc_service_start =
                    resp->Attribute_Group_Handle_Pair[0].Found_Attribute_Handle;
                ble_central->disc_service_end =
                    resp->Attribute_Group_Handle_Pair[0].Group_End_Handle;
            }
            return BleEventAckFlowEnable;
        }

        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE: {
            /* Completion is signalled separately by ACI_GATT_PROC_COMPLETE, so
             * nothing to do but keep it away from gap, which would log our MTU as
             * belonging to its own link. */
            aci_att_exchange_mtu_resp_event_rp0* mtu =
                (aci_att_exchange_mtu_resp_event_rp0*)blue_evt->data;
            if(ble_central->connected &&
               mtu->Connection_Handle == ble_central->connection_handle) {
                ble_central->att_mtu = mtu->Server_RX_MTU;
                return BleEventAckFlowEnable;
            }
            return BleEventNotAck;
        }

        case ACI_ATT_READ_RESP_VSEVT_CODE: {
            aci_att_read_resp_event_rp0* resp = (aci_att_read_resp_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               resp->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            uint8_t n = resp->Event_Data_Length;
            if(n > ble_central->read_capacity) n = ble_central->read_capacity;
            if(ble_central->read_buf) memcpy(ble_central->read_buf, resp->Attribute_Value, n);
            ble_central->read_len = n;
            return BleEventAckFlowEnable;
        }

        case ACI_ATT_READ_BLOB_RESP_VSEVT_CODE: {
            aci_att_read_blob_resp_event_rp0* resp =
                (aci_att_read_blob_resp_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               resp->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            uint8_t remaining = ble_central->read_capacity - ble_central->read_len;
            uint8_t n = resp->Event_Data_Length;
            if(n > remaining) n = remaining;
            if(ble_central->read_buf && n) {
                memcpy(ble_central->read_buf + ble_central->read_len, resp->Attribute_Value, n);
            }
            ble_central->read_len += n;
            return BleEventAckFlowEnable;
        }

        case ACI_GATT_READ_EXT_VSEVT_CODE: {
            aci_gatt_read_ext_event_rp0* resp = (aci_gatt_read_ext_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               resp->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            uint16_t offset = resp->Offset & 0x7FFFU;
            uint16_t len = resp->Event_Data_Length;
            if(offset < ble_central->read_capacity && ble_central->read_buf) {
                uint16_t remaining = ble_central->read_capacity - offset;
                uint16_t n = len > remaining ? remaining : len;
                memcpy(ble_central->read_buf + offset, resp->Attribute_Value, n);
                uint16_t end = offset + n;
                if(end > ble_central->read_len) ble_central->read_len = end;
            }
            return BleEventAckFlowEnable;
        }

        case ACI_ATT_FIND_INFO_RESP_VSEVT_CODE: {
            /* Response to disc_all_char_desc: (handle, uuid) pairs. Format 0x01 is
             * 16-bit UUIDs, which is what a CCCD (0x2902) uses. */
            aci_att_find_info_resp_event_rp0* resp =
                (aci_att_find_info_resp_event_rp0*)blue_evt->data;
            if(resp->Format == 0x01 && !ble_central->disc_desc_done) {
                for(uint8_t i = 0; i + 4 <= resp->Event_Data_Length; i += 4) {
                    uint16_t handle = resp->Handle_UUID_Pair[i] |
                                      (resp->Handle_UUID_Pair[i + 1] << 8);
                    uint16_t uuid = resp->Handle_UUID_Pair[i + 2] |
                                    (resp->Handle_UUID_Pair[i + 3] << 8);
                    /* 0x2803 begins the next characteristic. Its descriptors
                     * belong to that characteristic, even though the controller
                     * was asked to search through the service end handle. */
                    if(uuid == 0x2803) {
                        ble_central->disc_desc_done = true;
                        break;
                    } else if(uuid == 0x2902) {
                        ble_central->disc_cccd_handle = handle;
                        ble_central->disc_desc_done = true;
                        break;
                    }
                }
            }
            return BleEventAckFlowEnable;
        }

        case ACI_ATT_READ_BY_TYPE_RESP_VSEVT_CODE: {
            /* Response to disc_all_char_of_service; may arrive in several parts. */
            ble_central_handle_char_disc((aci_att_read_by_type_resp_event_rp0*)blue_evt->data);
            return BleEventAckFlowEnable;
        }

        case ACI_GATT_PROC_COMPLETE_VSEVT_CODE: {
            aci_gatt_proc_complete_event_rp0* complete =
                (aci_gatt_proc_complete_event_rp0*)blue_evt->data;
            if(complete->Connection_Handle != ble_central->connection_handle)
                return BleEventNotAck;
            ble_central->proc_error = complete->Error_Code;
            furi_semaphore_release(ble_central->proc_sem);
            return BleEventAckFlowEnable;
        }

        case ACI_GAP_PASS_KEY_REQ_VSEVT_CODE: {
            aci_gap_pass_key_req_event_rp0* request =
                (aci_gap_pass_key_req_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               request->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            /* The central API has no passkey UI contract. Terminate this link
             * instead of letting gap.c answer on the peripheral connection. */
            FURI_LOG_W(TAG, "central passkey pairing is not supported");
            aci_gap_terminate(ble_central->connection_handle, 0x05);
            return BleEventAckFlowEnable;
        }

        case ACI_GAP_AUTHORIZATION_REQ_VSEVT_CODE: {
            aci_gap_authorization_req_event_rp0* request =
                (aci_gap_authorization_req_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               request->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            aci_gap_authorization_resp(ble_central->connection_handle, 0x02);
            return BleEventAckFlowEnable;
        }

        case ACI_GAP_NUMERIC_COMPARISON_VALUE_VSEVT_CODE: {
            aci_gap_numeric_comparison_value_event_rp0* comparison =
                (aci_gap_numeric_comparison_value_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               comparison->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            FURI_LOG_W(
                TAG,
                "central numeric comparison %06lu rejected: no confirmation callback",
                (unsigned long)comparison->Numeric_Value);
            aci_gap_numeric_comparison_value_confirm_yesno(ble_central->connection_handle, false);
            return BleEventAckFlowEnable;
        }

        case ACI_GAP_PAIRING_COMPLETE_VSEVT_CODE: {
            aci_gap_pairing_complete_event_rp0* pairing =
                (aci_gap_pairing_complete_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               pairing->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            FURI_LOG_I(
                TAG,
                "central pairing complete, status 0x%02X reason 0x%02X",
                pairing->Status,
                pairing->Reason);
            return BleEventAckFlowEnable;
        }

        case ACI_GAP_ADDR_NOT_RESOLVED_VSEVT_CODE: {
            aci_gap_addr_not_resolved_event_rp0* unresolved =
                (aci_gap_addr_not_resolved_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               unresolved->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            FURI_LOG_W(TAG, "central peer address could not be resolved");
            return BleEventAckFlowEnable;
        }

        case ACI_GAP_KEYPRESS_NOTIFICATION_VSEVT_CODE: {
            aci_gap_keypress_notification_event_rp0* keypress =
                (aci_gap_keypress_notification_event_rp0*)blue_evt->data;
            if(!ble_central->connected ||
               keypress->Connection_Handle != ble_central->connection_handle) {
                return BleEventNotAck;
            }
            return BleEventAckFlowEnable;
        }

        default:
            return BleEventNotAck;
        }
    }

    return BleEventNotAck;
}
