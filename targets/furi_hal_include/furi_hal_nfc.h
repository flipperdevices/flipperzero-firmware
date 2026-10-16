/**
 * @file furi_hal_nfc.h
 * @brief NFC HAL library.
 *
 * This library contains functions and definitions needed for NFC hardware low-level access.
 *
 * Application developers should first consider using the NFC protocol stack or
 * the NFC transport layer and see if the APIs provided there sufficient
 * for the applicaton's intended purpose.
 *
 * @see nfc.h
 * @see nfc_protocol.h
 *
 * If any of the above mentioned options is used, calling any of the functions provided by this
 * library is hardly necessary, as it will be taken care of under the hood.
 *
 */
#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief NFC carrier frequency, in Hz.
 */
#define FURI_HAL_NFC_CARRIER_HZ (13560000UL)

/**
 * @brief Special value indicating that waiting for an event shall never time out.
 */
#define FURI_HAL_NFC_EVENT_WAIT_FOREVER (0xFFFFFFFFU)

/**
 * @brief Enumeration of possible NFC HAL events.
 */
typedef enum {
    FuriHalNfcEventOscOn = (1U << 0), /**< Oscillator has been started. */
    FuriHalNfcEventFieldOn = (1U << 1), /**< External field (carrier) has been detected. */
    FuriHalNfcEventFieldOff = (1U << 2), /**< External field (carrier) has been lost. */
    FuriHalNfcEventListenerActive = (1U << 3), /**< Reader has issued a wake-up command. */
    FuriHalNfcEventTxStart = (1U << 4), /**< Transmission has started. */
    FuriHalNfcEventTxEnd = (1U << 5), /**< Transmission has ended. */
    FuriHalNfcEventRxStart = (1U << 6), /**< Reception has started. */
    FuriHalNfcEventRxEnd = (1U << 7), /**< Reception has ended. */
    FuriHalNfcEventCollision = (1U << 8), /**< A collision has occurred. */
    FuriHalNfcEventTimerFwtExpired = (1U << 9), /**< Frame wait timer has expired. */
    FuriHalNfcEventTimerBlockTxExpired = (1U << 10), /**< Transmission block timer has expired. */
    FuriHalNfcEventTimeout =
        (1U << 11), /**< No events have occurred in a specified time period. */
    FuriHalNfcEventAbortRequest =
        (1U << 12), /**< User has requested to abort current operation. */
} FuriHalNfcEvent;

/**
 * @brief Enumeration of possible NFC HAL errors.
 */
typedef enum {
    FuriHalNfcErrorNone, /**< No error has occurred. */
    FuriHalNfcErrorBusy, /**< The communication bus is busy. */
    FuriHalNfcErrorCommunication, /**< NFC hardware did not respond or responded unexpectedly. */
    FuriHalNfcErrorOscillator, /**< Oscillator failed to start. */
    FuriHalNfcErrorCommunicationTimeout, /**< NFC hardware did not respond in time. */
    FuriHalNfcErrorBufferOverflow, /**< Receive buffer was too small for the received data. */
    FuriHalNfcErrorIncompleteFrame, /**< Not enough data was received to parse a valid frame. */
    FuriHalNfcErrorDataFormat, /**< Cannot parse a frame due to unexpected/invalid data. */
} FuriHalNfcError;

/**
 * @brief Enumeration of possible NFC HAL operating modes.
 */
typedef enum {
    FuriHalNfcModePoller, /**< Configure NFC HAL to operate as a poller. */
    FuriHalNfcModeListener, /**< Configure NFC HAL to operate as a listener. */

    FuriHalNfcModeNum, /**< Special value equal to the operating modes count. Internal use. */
} FuriHalNfcMode;

/**
 * @brief Enumeration of supported NFC technologies.
 */
typedef enum {
    FuriHalNfcTechIso14443a, /**< Configure NFC HAL to use the ISO14443 (type A) technology. */
    FuriHalNfcTechIso14443b, /**< Configure NFC HAL to use the ISO14443 (type B) technology. */
    FuriHalNfcTechIso15693, /**< Configure NFC HAL to use the ISO15693 technology. */
    FuriHalNfcTechFelica, /**< Configure NFC HAL to use the FeliCa technology. */
    FuriHalNfcTechLegic, /**< Configure NFC HAL to use the LEGIC Prime technology. */

    FuriHalNfcTechNum, /**< Special value equal to the supported technologies count. Internal use. */
    FuriHalNfcTechInvalid, /**< Special value indicating the unconfigured state. Internal use. */
} FuriHalNfcTech;

/**
 * @brief Initialise the NFC HAL and associated hardware.
 *
 * This function is called automatically during the firmware initialisation,
 * so there is no need to call it explicitly.
 * 
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_init(void);

/**
 * @brief Check whether the NFC HAL was properly initialised and is ready.
 *
 * @returns FuriHalNfcErrorNone if ready, any other error code if not ready.
 */
FuriHalNfcError furi_hal_nfc_is_hal_ready(void);

/**
 * @brief Exclusively take over the NFC HAL and associated hardware.
 *
 * This function needs to be called whenever an interaction with the NFC HAL
 * is to take place (usually once upon the application start).
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_acquire(void);

/**
 * @brief Release the exclusive lock and make the NFC HAL available for others.
 *
 * This function needs to be called when the user code is done working
 * with the NFC HAL (usually once upon application exit). It must be called
 * from the same thread that has called furi_hal_nfc_acquire().
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_release(void);

/**
 * @brief Configure the NFC hardware to enter the low-power mode.
 *
 * This function must be called each time when the user code is done working
 * with the NFC HAL for the time being (e.g. waiting on user input).
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_low_power_mode_start(void);

/**
 * @brief Configure the NFC hardware to exit the low-power mode.
 *
 * This function must be called each time when the user code begins working
 * with the NFC HAL, as the default state is low-power mode.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_low_power_mode_stop(void);

/**
 * @brief Configure the NFC HAL to work in a particular mode.
 *
 * Not all technologies implement the listener operating mode.
 *
 * @param[in] mode required operating mode.
 * @param[in] tech required technology configuration.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_set_mode(FuriHalNfcMode mode, FuriHalNfcTech tech);

/**
 * @brief Reset the NFC HAL to its default (unconfigured) state.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_reset_mode(void);

/**
 * @brief Enable field (carrier) detection by the NFC hardware.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_field_detect_start(void);

/**
 * @brief Disable field (carrier) detection by the NFC hardware.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_field_detect_stop(void);

/**
 * @brief Check if the reader field (carrier) was detected by the NFC hardware.
 *
 * @returns true if the field was detected, false otherwise.
 */
bool furi_hal_nfc_field_is_present(void);

/**
 * @brief Enable field (carrier) generation by the NFC hardware.
 *
 * No carrier modulation will occur unless a transmission is explicitly started.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_poller_field_on(void);

/**
 * @brief Wait for an NFC HAL event in poller mode.
 *
 * @param[in] timeout_ms time to wait (timeout) in milliseconds.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcEvent furi_hal_nfc_poller_wait_event(uint32_t timeout_ms);

/**
 * @brief Wait for an NFC HAL event in listener mode.
 * @param[in] timeout_ms time to wait (timeout) in milliseconds.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcEvent furi_hal_nfc_listener_wait_event(uint32_t timeout_ms);

/**
 * @brief Transmit data in poller mode.
 *
 * @param[in] tx_data pointer to a byte array containing the data to be transmitted.
 * @param[in] tx_bits transmit data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_poller_tx(const uint8_t* tx_data, size_t tx_bits);

/**
 * @brief Receive data in poller mode.
 *
 * The receive buffer must be big enough to accomodate all of the expected data.
 *
 * @param[out] rx_data pointer to a byte array to be filled with received data.
 * @param[in] rx_data_size maximum received data size, in bytes.
 * @param[out] rx_bits pointer to the variable to hold received data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_poller_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

/**
 * @brief Transmit data in listener mode.
 *
 * @param[in] tx_data pointer to a byte array containing the data to be transmitted.
 * @param[in] tx_bits transmit data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_listener_tx(const uint8_t* tx_data, size_t tx_bits);

/**
 * @brief Receive data in listener mode.
 *
 * The receive buffer must be big enough to accomodate all of the expected data.
 *
 * @param[out] rx_data pointer to a byte array to be filled with received data.
 * @param[in] rx_data_size maximum received data size, in bytes.
 * @param[out] rx_bits pointer to the variable to hold received data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_listener_rx(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

/**
 * @brief Go to sleep in listener mode.
 *
 * Puts the passive target logic into Sleep (Halt) state.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_listener_sleep(void);

/**
 * @brief Go to idle in listener mode.
 *
 * Puts the passive target logic into Sense (Idle) state.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_listener_idle(void);

/**
 * @brief Enable reception in listener mode.
 *
 * Starts hardware receivers and receive decoders.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_listener_enable_rx(void);

/**
 * @brief Reset communication.
 *
 * Resets the communication state and stops all activities: transmission, reception, etc.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_trx_reset(void);

/**
 * @brief Enable generation of NFC HAL events.
 *
 * @warning This function must be called from the same thread from which
 * the the furi_hal_nfc_*_wait_event() calls will be made.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_event_start(void);

/**
 * @brief Disable generation of NFC HAL events.
 *
 * Unlike furi_hal_nfc_event_start(), this function may be called from any thread.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_event_stop(void);

/**
 * @brief Manually emit the FuriHalNfcEventAbortRequest event.
 *
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
*/
FuriHalNfcError furi_hal_nfc_abort(void);

/**
 * @brief Start frame wait timeout timer.
 *
 * @param[in] time_fc time to wait, in carrier cycles.
 */
void furi_hal_nfc_timer_fwt_start(uint32_t time_fc);

/**
 * @brief Stop frame wait timeout timer.
 */
void furi_hal_nfc_timer_fwt_stop(void);

/**
 * @brief Start block transmit (frame delay) timer.
 *
 * @param[in] time_fc time to wait, in carrier cycles.
*/
void furi_hal_nfc_timer_block_tx_start(uint32_t time_fc);

/**
 * @brief Start block transmit (frame delay) timer.
 *
 * @param[in] time_us time to wait, in microseconds.
 */
void furi_hal_nfc_timer_block_tx_start_us(uint32_t time_us);

/**
 * @brief Stop block transmit (frame delay) timer.
 */
void furi_hal_nfc_timer_block_tx_stop(void);

/**
 * @brief Check whether block transmit (frame delay) timer is running.
 *
 * @returns true if timer is running, false otherwise.
 */
bool furi_hal_nfc_timer_block_tx_is_running(void);

/*
 * Technology-specific functions.
 *
 * In a perfect world, this would not be necessary.
 * However, the current implementation employs NFC hardware that partially implements
 * certain protocols (e.g. ISO14443-3A), thus requiring methods to access such features.
 */

/******************* Iso14443a specific API *******************/

/**
 * @brief Enumeration of ISO14443 (Type A) short frame types.
 */
typedef enum {
    FuriHalNfcaShortFrameAllReq,
    FuriHalNfcaShortFrameSensReq,
} FuriHalNfcaShortFrame;

/**
 * @brief Transmit ISO14443 (Type A) short frame in poller mode.
 *
 * @param[in] frame short frame type to be transmitted.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_iso14443a_poller_trx_short_frame(FuriHalNfcaShortFrame frame);

/** Transmit ISO14443 (Type A) SDD frame in poller mode.
 *
 * @param[in] tx_data pointer to a byte array containing the data to be transmitted.
 * @param[in] tx_bits transmit data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_iso14443a_tx_sdd_frame(const uint8_t* tx_data, size_t tx_bits);

/**
 * Receive ISO14443 (Type A) SDD frame in poller mode.
 *
 * The receive buffer must be big enough to accomodate all of the expected data.
 *
 * @param[in] rx_data pointer to a byte array to be filled with received data.
 * @param[in] rx_data_size maximum received data size, in bytes.
 * @param[in] rx_bits pointer to the variable to hold received data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError
    furi_hal_nfc_iso14443a_rx_sdd_frame(uint8_t* rx_data, size_t rx_data_size, size_t* rx_bits);

/**
 * @brief Transmit ISO14443 (Type A) frame with custom parity bits in poller mode.
 *
 * Same as furi_hal_nfc_poller_tx(), but uses the parity bits provided
 * by the user code instead of calculating them automatically.
 *
 * @param[in] tx_data pointer to a byte array containing the data to be transmitted.
 * @param[in] tx_bits transmit data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError
    furi_hal_nfc_iso14443a_poller_tx_custom_parity(const uint8_t* tx_data, size_t tx_bits);

/**
 * @brief Set ISO14443 (Type A) collision resolution parameters in listener mode.
 *
 * Configures the NFC hardware for automatic collision resolution.
 *
 * @param[in] uid pointer to a byte array containing the UID.
 * @param[in] uid_len UID length in bytes (must be supported by the protocol).
 * @param[in] atqa ATQA byte value.
 * @param[in] sak SAK byte value.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_iso14443a_listener_set_col_res_data(
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak);

/**
 * @brief Transmit ISO14443 (Type A) frame with custom parity bits in listener mode.
 *
 * @param[in] tx_data pointer to a byte array containing the data to be transmitted.
 * @param[in] tx_parity pointer to a (bit-packed) byte array containing the parity to be transmitted.
 * @param[in] tx_bits transmit data size, in bits.
 * @returns FuriHalNfcErrorNone on success, any other error code on failure.
 */
FuriHalNfcError furi_hal_nfc_iso14443a_listener_tx_custom_parity(
    const uint8_t* tx_data,
    const uint8_t* tx_parity,
    size_t tx_bits);

/** Send ISO15693 SOF in listener mode
 *
 * @return FuriHalNfcError
*/
FuriHalNfcError furi_hal_nfc_iso15693_listener_tx_sof(void);

/**
 * @brief Set FeliCa collision resolution parameters in listener mode.
 * 
 * Configures the NFC hardware for automatic collision resolution.
 *
 * @param[in] idm pointer to a byte array containing the IDm.
 * @param[in] idm_len IDm length in bytes.
 * @param[in] pmm pointer to a byte array containing the PMm.
 * @param[in] pmm_len PMm length in bytes.
 * @param[in] sys_code System code from SYS_C block
 * @returns NfcErrorNone on success, any other error code on failure.
*/
FuriHalNfcError furi_hal_nfc_felica_listener_set_sensf_res_data(
    const uint8_t* idm,
    const uint8_t idm_len,
    const uint8_t* pmm,
    const uint8_t pmm_len,
    const uint16_t sys_code);

/*
 * The furi_hal_nfc_legic_* functions below are deliberately not exported to
 * applications (api_symbols: -): LEGIC Prime is driven through lib/nfc, whose
 * poller and listener are their only users.
 */

/**
 * @brief Set the length of the tag reply expected for the next frame.
 *
 * Every LEGIC Prime frame sent by the poller gets a reply of known length.
 *
 * @param[in] bits number of reply bits, 1 to 64 (the longest is the 60-window
 *            wait for a write ACK).
 */
void furi_hal_nfc_legic_poller_set_rx_bits(size_t bits);

/**
 * @brief Expect a LEGIC Prime write ACK as the reply to the next frame.
 *
 * The ACK is a single bit in the bit window in which the tag finished
 * writing.  A frame queued with furi_hal_nfc_legic_poller_set_next() is sent
 * 240 µs after the ACK's measured end, but only if the ACK came in bit window
 * `window` (counted from 0 like reply bits), because the queued frame's
 * keystream depends on it; otherwise, if no ACK comes, or if the HAL noticed
 * the ACK too late to send the frame on time, the queued frame is dropped.
 * Applies to the next exchange only.
 *
 * @param[in] window bit window the queued frame was built for (below 64).
 */
void furi_hal_nfc_legic_poller_set_rx_ack(size_t window);

/**
 * @brief Select the receiver gain for LEGIC Prime exchanges.
 *
 * Takes effect with the next frame sent by the poller's tx() (not with a
 * frame queued by furi_hal_nfc_legic_poller_set_next()).
 *
 * @param[in] reduced true for the default first-stage gain reduction, false for full gain.
 */
void furi_hal_nfc_legic_poller_set_rx_reduced_gain(bool reduced);

/**
 * @brief Queue the LEGIC Prime reader frame that follows the next reply.
 *
 * Real tags only wait a few 99.1 µs slots for the reader's next frame, so it
 * is sent from the receive path, 260 µs after the tag's reply ends (240 µs
 * after a write ACK), before the reply is decoded.  The poller then passes
 * the same frame to tx() after furi_hal_nfc_legic_poller_claim_next(), and
 * tx() only waits for its reply, whose length is set with
 * furi_hal_nfc_legic_poller_set_rx_bits() as for any other frame.
 *
 * @param[in] first first frame, LSB-first bits.
 * @param[in] first_bits number of bits in the first frame.
 * @param[in] second optional frame chained 221.8 µs after the first (or NULL).
 * @param[in] second_bits number of bits in the second frame.
 */
void furi_hal_nfc_legic_poller_set_next(
    const uint8_t* first,
    size_t first_bits,
    const uint8_t* second,
    size_t second_bits);

/**
 * @brief Check whether the queued LEGIC Prime frame went out in its slot.
 *
 * @returns true if it was sent and has not been claimed by tx() yet.
 */
bool furi_hal_nfc_legic_poller_next_sent(void);

/**
 * @brief Declare the next poller tx() to be the queued LEGIC Prime frame.
 *
 * Call it before the nfc_poller_trx() of a frame that was queued with
 * furi_hal_nfc_legic_poller_set_next() and sent in the previous exchange
 * (furi_hal_nfc_legic_poller_next_sent()): tx() then only sets up the
 * reception of its reply instead of transmitting the frame again.  Without
 * the claim, or if the queued frame was not sent, tx() transmits as usual.
 * Applies to the next tx() only.
 */
void furi_hal_nfc_legic_poller_claim_next(void);

/**
 * @brief Drop the queued LEGIC Prime frame and forget an unclaimed sent one.
 *
 * Call it at the start of every session's handshake, before the session's
 * first furi_hal_nfc_legic_poller_set_next() and before the IV's
 * nfc_poller_trx().  A frame left over from a session that ended without
 * claiming it (frame-wait timeout, unknown card type) would otherwise be
 * transmitted after the handshake's reply on the old session's keystream,
 * or keep furi_hal_nfc_trx_reset() from stopping the receiver.
 */
void furi_hal_nfc_legic_poller_cancel_next(void);

/**
 * @brief Set the LEGIC Prime tag's keystream reference to the end of the last reader frame.
 *
 * The tag steps its keystream once per 99.1 µs slot while it waits for the
 * reader.  The protocol counts those slots from a reference point it sets
 * here or with furi_hal_nfc_legic_listener_set_ref_to_tx_end(), and reads
 * the time since it with furi_hal_nfc_legic_listener_get_elapsed_fc().
 *
 * @param[in] extra_fc offset after the end of the frame's closing pause, in carrier cycles (1/fc).
 */
void furi_hal_nfc_legic_listener_set_ref_to_frame_end(uint32_t extra_fc);

/**
 * @brief Set the LEGIC Prime tag's keystream reference to the end of its last reply.
 *
 * The end is nominal: the reply's delay after the reader's frame plus 99.1 µs
 * per bit, regardless of when the modulation actually started.
 */
void furi_hal_nfc_legic_listener_set_ref_to_tx_end(void);

/**
 * @brief Get the time from the keystream reference to the start of the last reader frame.
 *
 * @returns carrier cycles (1/fc), modulo one cycle counter wrap (~67 s).
 */
uint32_t furi_hal_nfc_legic_listener_get_elapsed_fc(void);

/**
 * @brief Delay the next LEGIC Prime tag reply.
 *
 * Applies to the next transmission only.  Replies normally start 330 µs (70
 * subcarrier periods) after the reader's frame; a write ACK comes ~3.6 ms
 * after it.
 *
 * @param[in] chips delay after the reader's closing pause, in 212 kHz
 *            subcarrier periods (64/fc, 4.72 µs).
 */
void furi_hal_nfc_legic_listener_set_tx_delay(uint32_t chips);

#ifdef __cplusplus
}
#endif
