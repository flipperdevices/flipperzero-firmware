#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FURI_HAL_NFC_TIMER_OFFSET_FC (500)

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

/**
 * @brief Nfc HAL initialization
 * 
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_init();

FuriHalNfcError furi_hal_nfc_is_hal_ready();

/**
 * @brief Start Nfc hardware low power mode
 * 
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_low_power_mode_start();

/**
 * @brief Stop Nfc hardware low power mode
 * 
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_low_power_mode_stop();

/**
 * @brief Configure Nfc HAL mode
 * 
 * @param mode          FuriHalNfcMode instance
 * @param bitrate       FuriHalNfcBitrate instance
 * 
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_set_mode(FuriHalNfcMode mode, FuriHalNfcTech tech);

FuriHalNfcError furi_hal_nfc_reset_mode();

/**
 * @brief Turn on field in poller mode
 * 
 * @return FuriHalNfcError 
 */
FuriHalNfcError furi_hal_nfc_poller_field_on();

FuriHalNfcError furi_hal_nfc_acquire();

FuriHalNfcError furi_hal_nfc_release();

FuriHalNfcEvent furi_hal_nfc_poller_wait_event(uint32_t timeout_ms);

FuriHalNfcEvent furi_hal_nfc_listener_wait_event(uint32_t timeout_ms);

FuriHalNfcError furi_hal_nfc_poller_tx(const uint8_t* tx_data, size_t tx_bits);

FuriHalNfcError furi_hal_nfc_poller_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

FuriHalNfcError furi_hal_nfc_listener_tx(const uint8_t* tx_data, size_t tx_bits);

FuriHalNfcError furi_hal_nfc_listener_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

FuriHalNfcError furi_hal_nfc_listener_sleep();

FuriHalNfcError furi_hal_nfc_listener_idle();

FuriHalNfcError furi_hal_nfc_listener_enable_rx();

FuriHalNfcError furi_hal_nfc_trx_reset();

FuriHalNfcError furi_hal_nfc_abort();

FuriHalNfcError furi_hal_nfc_event_start();

void furi_hal_nfc_timer_fwt_start(uint32_t time_fc);

void furi_hal_nfc_timer_fwt_stop();

void furi_hal_nfc_timer_block_tx_start(uint32_t time_fc);

void furi_hal_nfc_timer_block_tx_start_us(uint32_t time_us);

void furi_hal_nfc_timer_block_tx_stop();

bool furi_hal_nfc_timer_block_tx_is_running();

/******************* NFCA specific API *******************/

FuriHalNfcError furi_hal_nfc_iso14443a_poller_trx_short_frame(FuriHalNfcaShortFrame frame);

FuriHalNfcError furi_hal_nfc_iso14443a_tx_sdd_frame(const uint8_t* tx_data, size_t tx_bits);

FuriHalNfcError
    furi_hal_nfc_iso14443a_rx_sdd_frame(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

FuriHalNfcError furi_hal_nfc_iso14443a_listener_set_col_res_data(
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak);

FuriHalNfcError
    furi_hal_nfc_iso14443a_poller_tx_custom_parity(const uint8_t* tx_data, size_t tx_bits);

FuriHalNfcError furi_hal_nfc_iso14443a_listener_tx_custom_parity(
    const uint8_t* tx_data,
    const uint8_t* tx_parity,
    size_t tx_bits);

#ifdef __cplusplus
}
#endif
