#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FURI_HAL_NFC_TIMER_COMPENSATION_FC (500)
#define FURI_HAL_NFC_EVENT_WAIT_FOREVER (0xFFFFFFFFU)

typedef enum {
    FuriHalNfcEventOscOn = (1U << 0),
    FuriHalNfcEventFieldOn = (1U << 1),
    FuriHalNfcEventFieldOff = (1U << 2),
    FuriHalNfcEventListenerActive = (1U << 3),
    FuriHalNfcEventListenerActiveA = (1U << 4),
    FuriHalNfcEventTxStart = (1U << 5),
    FuriHalNfcEventTxEnd = (1U << 6),
    FuriHalNfcEventRxStart = (1U << 7),
    FuriHalNfcEventRxEnd = (1U << 8),
    FuriHalNfcEventCollision = (1U << 9),
    FuriHalNfcEventTimerFwtExpired = (1U << 10),
    FuriHalNfcEventTimerBlockTxExpired = (1U << 11),
    FuriHalNfcEventTimeout = (1U << 12),
    FuriHalNfcEventAbortRequest = (1U << 13),
} FuriHalNfcEvent;

typedef enum {
    FuriHalNfcErrorNone,
    FuriHalNfcErrorBusy,
    FuriHalNfcErrorChipCommunication,
    FuriHalNfcErrorCommunication,
    FuriHalNfcErrorOscillator,
    FuriHalNfcErrorIsrTimeout,
    FuriHalNfcErrorCommunicationTimeout,
    FuriHalNfcErrorBufferOverflow,
    FuriHalNfcErrorDataFormat,
} FuriHalNfcError;

typedef enum {
    FuriHalNfcModePoller,
    FuriHalNfcModeListener,

    FuriHalNfcModeNum,
} FuriHalNfcMode;

typedef enum {
    FuriHalNfcTechIso14443a,
    FuriHalNfcTechIso14443b,
    FuriHalNfcTechIso15693,
    FuriHalNfcTechFelica,

    FuriHalNfcTechNum,
    FuriHalNfcTechInvalid,
} FuriHalNfcTech;

typedef enum {
    FuriHalNfcaShortFrameAllReq,
    FuriHalNfcaShortFrameSensReq,
} FuriHalNfcaShortFrame;

/** Nfc HAL initialization
 * 
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_init();

/** Check if Nfc HAL is ready
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_is_hal_ready();

/** Acquire Nfc HAL
 *
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_acquire();

/** Release Nfc HAL
 *
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_release();

/** Enter low power mode
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_low_power_mode_start();

/** Exit low power mode
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_low_power_mode_stop();

/** Set NFC mode
 *
 * @param mode  - FuriHalNfcMode value
 * @param tech  - FuriHalNfcTech value
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_set_mode(FuriHalNfcMode mode, FuriHalNfcTech tech);

/** Reset mode
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_reset_mode();

/** Start field detection
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_field_detect_start();

/** Stop field detection
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_field_detect_stop();

/** Check if external NFC field is present
 *
 * @return FuriHalNfcError
 */
bool furi_hal_nfc_field_is_present();

/** Turn on field
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_poller_field_on();

/** Wait for Nfc HAL event in poller mode
 *
 * @param timeout_ms    - timeout in milliseconds
 * 
 * @return FuriHalNfcEvent value
 */
FuriHalNfcEvent furi_hal_nfc_poller_wait_event(uint32_t timeout_ms);

/** Wait for Nfc HAL event in listener mode
 *
 * @param timeout_ms    - timeout in milliseconds
 * 
 * @return FuriHalNfcEvent value
 */
FuriHalNfcEvent furi_hal_nfc_listener_wait_event(uint32_t timeout_ms);

/** Transmit data in poller mode
 *
 * @param tx_data       - buffer to transmit
 * @param tx_bits       - number of bits to transmit
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_poller_tx(const uint8_t* tx_data, size_t tx_bits);

/** Receive data in poller mode
 *
 * @param rx_data       - buffer to receive
 * @param rx_data_size  - size of receive buffer in bytes
 * @param rx_bits       - number of bits received
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_poller_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

/** Tranmit data in listener mode
 *
 * @param tx_data       - buffer to transmit
 * @param tx_bits       - number of bits to transmit
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_listener_tx(const uint8_t* tx_data, size_t tx_bits);

/** Receive data in listener mode
 *
 * @param rx_data       - buffer to receive
 * @param rx_data_size  - size of receive buffer in bytes
 * @param rx_bits       - number of bits received
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_listener_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

/** Go to sleep in listener mode
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_listener_sleep();

/** Go to idle in listener mode
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_listener_idle();

/** Enable receive in listener mode
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_listener_enable_rx();

/** Reset communication
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_trx_reset();

/** Start Nfc HAL events
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_event_start();

/** Emit Abort event
 *
 * @return FuriHalNfcError
*/
FuriHalNfcError furi_hal_nfc_abort();

/** Start Frame Wait Timeout timer
 *
 * @param time_fc   - time in NFC frequency clock
 */
void furi_hal_nfc_timer_fwt_start(uint32_t time_fc);

/** Stop Frame Wait Timeout timer */
void furi_hal_nfc_timer_fwt_stop();

/** Start block transmit timer
 *
 * @param time_fc   - time in NFC frequency clock
*/
void furi_hal_nfc_timer_block_tx_start(uint32_t time_fc);

/** Start block transmit timer
 *
 * @param time_us   - time in microseconds
 */
void furi_hal_nfc_timer_block_tx_start_us(uint32_t time_us);

/** Stop block transmit timer */
void furi_hal_nfc_timer_block_tx_stop();

/** Check if block transmit timer is running
 *
 * @return true if timer is running
 */
bool furi_hal_nfc_timer_block_tx_is_running();

/******************* Iso14443a specific API *******************/

/** Send Iso14443a Short frame in poller mode
 *
 * @param frame     - FuriHalNfcaShortFrame value
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_iso14443a_poller_trx_short_frame(FuriHalNfcaShortFrame frame);

/** Send Iso14443a SDD frame in poller mode
 *
 * @param tx_data   - buffer to transmit
 * @param tx_bits   - number of bits to transmit
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_iso14443a_tx_sdd_frame(const uint8_t* tx_data, size_t tx_bits);

/** Receive Iso14443a SDD frame in poller mode
 *
 * @param rx_data       - buffer to receive
 * @param rx_data_size  - size of receive buffer in bytes
 * @param rx_bits       - number of bits received
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError
    furi_hal_nfc_iso14443a_rx_sdd_frame(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

/** Send Iso14443a frame with custom parity bits in poller mode
 *
 * @param tx_data   - buffer to transmit
 * @param tx_bits   - number of bits to transmit
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError
    furi_hal_nfc_iso14443a_poller_tx_custom_parity(const uint8_t* tx_data, size_t tx_bits);

/** Set Iso14443a collision resolution data in listener mode
 *
 * @param uid       - pointer to UID buffer
 * @param uid_len   - UID length in bytes
 * @param atqa      - pointer to ATQA buffer
 * @param sak       - SAK value
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_iso14443a_listener_set_col_res_data(
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak);

/** Send ISO14443a frame with custom parity bits in listener mode
 *
 * @param tx_data   - buffer to transmit
 * @param tx_parity - buffer with parity bits
 * @param tx_bits   - number of bits to transmit
 *
 * @return FuriHalNfcError
 */
FuriHalNfcError furi_hal_nfc_iso14443a_listener_tx_custom_parity(
    const uint8_t* tx_data,
    const uint8_t* tx_parity,
    size_t tx_bits);

#ifdef __cplusplus
}
#endif
