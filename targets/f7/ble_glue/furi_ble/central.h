#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* BLE central / GATT client role.
 *
 * The stock BLE glue is peripheral-only: gap.c requests GAP_PERIPHERAL_ROLE and
 * furi_hal_bt exposes nothing for scanning, connecting or GATT client operations.
 * This module adds the central side on top of the same copro stack, which does
 * accept GAP_PERIPHERAL_ROLE|GAP_CENTRAL_ROLE, so a profile can be a peripheral
 * and this module can be a central at the same time.
 *
 * Requires the BLE Full radio stack; gap.c requests the central role only when
 * furi_hal_bt_is_central_supported() is true, and every call here fails with a
 * controller error otherwise. See documentation/OTA.md for the complete update
 * package command, including the matching stack binary, type, and option bytes.
 */

#define BLE_CENTRAL_ADV_DATA_MAX 31
#define BLE_CENTRAL_ADDR_LEN     6

/* HCI advertising report Event_Type. An active scan yields the primary
 * advertisement and the scan response as two separate reports for the same
 * address, carrying different payloads — callers that deduplicate by address
 * must not let one overwrite the other. */
#define BLE_CENTRAL_EVT_SCAN_RSP 0x04

typedef struct {
    uint8_t addr[BLE_CENTRAL_ADDR_LEN];
    uint8_t addr_type;
    uint8_t event_type;
    int8_t rssi;
    uint8_t data_len;
    uint8_t data[BLE_CENTRAL_ADV_DATA_MAX];
} BleCentralAdvReport;

/* Invoked from the BLE event thread for every advertising report while scanning.
 * Keep it short and do not block. */
typedef void (*BleCentralScanCallback)(const BleCentralAdvReport* report, void* context);

void ble_central_init(void);

/* Stops central activity and releases all resources. Returns false when the
 * controller has not yet confirmed scan/link termination; callbacks are still
 * detached before returning, and the caller may retry deinitialization later. */
bool ble_central_deinit(void);

/* Starts general discovery. Reports arrive on `callback` until
 * ble_central_scan_stop. Returns false if a scan is already running or the
 * controller rejected the request.
 *
 * The callback runs on the BLE event thread and must remain short. Do not call
 * central APIs from it. ble_central_scan_stop waits for an in-flight callback,
 * so its context may be released safely after that function returns true.
 *
 * The controller will not scan while the peripheral side is advertising: it
 * fails the procedure with BLE_STATUS_INVALID_PARAMS (0x92), which reads as a
 * bad argument and is not. Stop advertising for the duration of the scan:
 *
 *     bool was_advertising = furi_hal_bt_is_advertising();
 *     if(was_advertising) furi_hal_bt_stop_advertising();
 *
 * Left to the caller rather than done here, so that starting a scan cannot
 * silently take down a profile something else is relying on. */
bool ble_central_scan_start(BleCentralScanCallback callback, void* context);

/* Returns false and preserves the scan callback/state when the controller
 * rejects termination, allowing the caller to retry without desynchronising the
 * host from the active controller procedure. */
bool ble_central_scan_stop(void);
bool ble_central_is_scanning(void);

/* tBleStatus from the last controller call, 0 on success. Exposed because the
 * failure reason is the only useful diagnostic when scanning is refused. */
uint8_t ble_central_last_status(void);

/* ---- connection / GATT client ----
 *
 * Every ACI procedure below is asynchronous and finishes with
 * ACI_GATT_PROC_COMPLETE. Only one may be outstanding at a time, so each of
 * these blocks on an internal semaphore until the procedure completes or
 * `timeout_ms` elapses. Call them from a normal thread, never from the BLE
 * event callback. */

/* Notification payloads arrive on the BLE event thread. Flipper binds stdout
 * per thread — buffer here and render elsewhere, never print from this. */
typedef void (
    *BleCentralNotifyCallback)(uint16_t handle, const uint8_t* data, uint16_t len, void* context);

/* Just Works pairing is handled by the controller, but this API does not expose
 * passkey or numeric-comparison UI callbacks. Those methods are rejected on the
 * central link and never forwarded to the peripheral GAP profile. */
bool ble_central_connect(
    const uint8_t addr[BLE_CENTRAL_ADDR_LEN],
    uint8_t addr_type,
    uint32_t timeout_ms);

/* Returns true only after the controller confirms disconnection. If it times
 * out, the connection remains tracked so that a late event is still routed to
 * the central module rather than the peripheral GAP state machine. */
bool ble_central_disconnect(void);
bool ble_central_is_connected(void);
uint16_t ble_central_connection_handle(void);

/* Negotiates the ATT MTU. CFG_BLE_MAX_ATT_MTU caps what we can ask for. */
bool ble_central_exchange_mtu(uint32_t timeout_ms);

/* Discovers a primary service and locates one characteristic within it.
 *
 * `out_cccd_handle` may be NULL, and is set to 0 when the characteristic has no
 * Client Characteristic Configuration descriptor — normal for read-only
 * characteristics such as Battery Level or the Device Information strings.
 * Absence of a CCCD is not an error; only a missing service or characteristic
 * is. */
bool ble_central_find_characteristic_128(
    const uint8_t service_uuid[16],
    const uint8_t char_uuid[16],
    uint16_t* out_value_handle,
    uint16_t* out_cccd_handle,
    uint32_t timeout_ms);

/* Same, for the 16-bit UUIDs used by the SIG-adopted services. */
bool ble_central_find_characteristic_16(
    uint16_t service_uuid,
    uint16_t char_uuid,
    uint16_t* out_value_handle,
    uint16_t* out_cccd_handle,
    uint32_t timeout_ms);

/* ---- enumeration ----
 *
 * Walks whatever the peer exposes rather than looking for something known.
 * Callbacks fire on the BLE event thread, once per entry. Even on timeout or
 * error, an active callback finishes before the enumerate call returns, so the
 * caller may then release `context`. `uuid` is little-endian and `uuid_len` is
 * 2 or 16. */

typedef void (*BleCentralServiceCallback)(
    uint16_t start_handle,
    uint16_t end_handle,
    const uint8_t* uuid,
    uint8_t uuid_len,
    void* context);

/* `properties` is the GATT characteristic property bitfield: 0x02 read,
 * 0x04 write-without-response, 0x08 write, 0x10 notify, 0x20 indicate. */
typedef void (*BleCentralCharCallback)(
    uint16_t value_handle,
    uint8_t properties,
    const uint8_t* uuid,
    uint8_t uuid_len,
    void* context);

bool ble_central_enumerate_services(
    BleCentralServiceCallback callback,
    void* context,
    uint32_t timeout_ms);

bool ble_central_enumerate_characteristics(
    uint16_t start_handle,
    uint16_t end_handle,
    BleCentralCharCallback callback,
    void* context,
    uint32_t timeout_ms);

/* Finds the CCCD for a characteristic, searching from its value handle to the
 * end of its service. Returns 0 in `out_cccd_handle` when there is none. */
bool ble_central_find_cccd(
    uint16_t value_handle,
    uint16_t service_end_handle,
    uint16_t* out_cccd_handle,
    uint32_t timeout_ms);

/* Writes 0x0001 to a CCCD to enable notifications. */
bool ble_central_subscribe(uint16_t cccd_handle, uint32_t timeout_ms);

/* Writes 0x0000 to a CCCD to stop them. */
bool ble_central_unsubscribe(uint16_t cccd_handle, uint32_t timeout_ms);

/* Reads an attribute into a caller-supplied buffer. `out_len` receives the
 * number of bytes written. Values longer than `buf_size` are truncated. */
bool ble_central_read(
    uint16_t handle,
    uint8_t* buf,
    uint8_t buf_size,
    uint8_t* out_len,
    uint32_t timeout_ms);

/* write_response=false uses Write Without Response, which does not consume a
 * GATT procedure slot and so returns as soon as the controller accepts it. */
bool ble_central_write(
    uint16_t value_handle,
    const uint8_t* data,
    uint8_t len,
    bool write_response,
    uint32_t timeout_ms);

/* Notification callbacks run on the BLE event thread and must remain short. Do
 * not call central APIs from them. Replacing or clearing the callback waits for
 * an in-flight invocation, so the previous context may be released when this
 * function returns. */
void ble_central_set_notify_callback(BleCentralNotifyCallback callback, void* context);

/* Finds an AD structure in an advertising payload. Advertising data is a series
 * of [length][type][value] records; every consumer of BleCentralAdvReport needs
 * to walk them, so the bounds arithmetic lives here once.
 *
 * `ad_type` values are the AD_TYPE_* constants from the Bluetooth spec, e.g.
 * 0x09 complete local name, 0x03 complete 16-bit service UUID list, 0x16
 * service data. Returns false when the type is absent. */
bool ble_central_adv_find(
    const BleCentralAdvReport* report,
    uint8_t ad_type,
    const uint8_t** out_value,
    uint8_t* out_len);

#ifdef __cplusplus
}
#endif
