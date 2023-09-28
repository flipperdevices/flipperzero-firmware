#pragma once

#include <toolbox/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Nfc Nfc;

typedef enum {
    NfcEventTypeUserAbort,
    NfcEventTypeFieldOn,
    NfcEventTypeFieldOff,
    NfcEventTypeTxStart,
    NfcEventTypeTxEnd,
    NfcEventTypeRxStart,
    NfcEventTypeRxEnd,

    NfcEventTypeListenerActivated,
    NfcEventTypePollerReady,
} NfcEventType;

typedef struct {
    BitBuffer* buffer;
} NfcEventData;

typedef struct {
    NfcEventType type;
    NfcEventData data;
} NfcEvent;

typedef enum {
    NfcCommandContinue,
    NfcCommandReset,
    NfcCommandStop,
    NfcCommandSleep,
} NfcCommand;

typedef NfcCommand (*NfcEventCallback)(NfcEvent event, void* context);

typedef enum {
    NfcModePoller,
    NfcModeListener,

    NfcModeNum,
} NfcMode;

typedef enum {
    NfcTechIso14443a,
    NfcTechIso14443b,
    NfcTechIso15693,
    NfcTechFelica,

    NfcTechNum,
} NfcTech;

typedef enum {
    NfcErrorNone,
    NfcErrorInternal,
    NfcErrorTimeout,
    NfcErrorWrongState,
    NfcErrorCollision,
    NfcErrorLinkLoss,
    NfcErrorAbortRequest,
    NfcErrorIncompleteFrame,
    NfcErrorDataFormat,
} NfcError;

typedef enum {
    NfcIso14443aShortFrameSensReq,
    NfcIso14443aShortFrameAllReqa,
} NfcIso14443aShortFrame;

/** Allocate Nfc instance
 * Acquires Nfc HAL
 *
 * @return Nfc instance
 */
Nfc* nfc_alloc();

/** Delete Nfc instance
 * Releases Nfc HAL
 *
 * @param instance          - Nfc instance
 */
void nfc_free(Nfc* instance);

/** Configure Nfc
 *
 * @param instance          - Nfc instance
 * @param mode              - NfcMode value
 * @param tech              - NfcTech value
 */
void nfc_config(Nfc* instance, NfcMode mode, NfcTech tech);

/** Set poller frame delay time
 *
 * @param instance          - Nfc instance
 * @param fdt_poll_fc       - FDT Poll value in Nfc frequency units
*/
void nfc_set_fdt_poll_fc(Nfc* instance, uint32_t fdt_poll_fc);

/** Set listener frame delay time
 *
 * @param instance          - Nfc instance
 * @param fdt_listen_fc     - FDT Listen value in Nfc frequency units
 */
void nfc_set_fdt_listen_fc(Nfc* instance, uint32_t fdt_listen_fc);

/** Set mask receive time
 *
 * @param instance          - Nfc instance
 * @param mask_rx_time      - Mask Receive Time value in Nfc frequency units
 */
void nfc_set_mask_receive_time_fc(Nfc* instance, uint32_t mask_rx_time_fc);

/** Set frame delay time between two consecutive poll frames
 *
 * @param instance          - Nfc instance
 * @param fdt_poll_poll_us  - FDT Poll-Poll value in microseconds
 */
void nfc_set_fdt_poll_poll_us(Nfc* instance, uint32_t fdt_poll_poll_us);

/** Set Guard time
 *
 * @param instance          - Nfc instance
 * @param guard_time_us     - Guard Time value in microseconds
 */
void nfc_set_guard_time_us(Nfc* instance, uint32_t guard_time_us);

/** Start Nfc worker
 *
 * @param instance          - Nfc instance
 * @param callback          - NfcEventCallback function
 * @param context           - context pointer
 */
void nfc_start(Nfc* instance, NfcEventCallback callback, void* context);

/** Stop Nfc worker
 *
 * @param instance          - Nfc instance
 */
void nfc_stop(Nfc* instance);

/** Transmit and receive frames in poller mode
 * Must be called from Nfc worker thread
 *
 * @param instance          - Nfc instance
 * @param tx_buffer         - buffer to transmit
 * @param rx_buffer         - buffer to receive
 * @param fwt               - FWT value in Nfc frequency units
 *
 * @return NfcError value
 */
NfcError
    nfc_poller_trx(Nfc* instance, const BitBuffer* tx_buffer, BitBuffer* rx_buffer, uint32_t fwt);

/** Transmit frame in listener mode
 * Must be called from Nfc worker thread
 *
 * @param instance          - Nfc instance
 * @param tx_buffer         - buffer to transmit
 *
 * @return NfcError value
 */
NfcError nfc_listener_tx(Nfc* instance, const BitBuffer* tx_buffer);

/******************* Iso14443a specific API *******************/

/** Send Iso14443a Short frame and receive response in poller mode
 *
 * @param instance          - Nfc instance
 * @param frame             - NfcIso14443aShortFrame value
 * @param rx_buffer         - buffer to receive
 * @param fwt               - FWT value in Nfc frequency units
 *
 * @return NfcError value
 */
NfcError nfc_iso14443a_poller_trx_short_frame(
    Nfc* instance,
    NfcIso14443aShortFrame frame,
    BitBuffer* rx_buffer,
    uint32_t fwt);

/** Send Iso14443a SDD frame and receive response in poller mode
 *
 * @param instance          - Nfc instance
 * @param tx_buffer         - buffer to transmit
 * @param rx_buffer         - buffer to receive
 * @param fwt               - FWT value in Nfc frequency units
 *
 * @return NfcError value
 */
NfcError nfc_iso14443a_poller_trx_sdd_frame(
    Nfc* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

/** Transmit Iso14443a frame with custom parity bits and receive response in poller mode
 *
 * @param instance          - Nfc instance
 * @param tx_buffer         - buffer to transmit
 * @param rx_buffer         - buffer to receive
 * @param fwt               - FWT value in Nfc frequency units
 *
 * @return NfcError value
 */
NfcError nfc_iso14443a_poller_trx_custom_parity(
    Nfc* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

/** Transmit Iso14443a frame with custom parity bits in listener mode
 *
 * @param instance          - Nfc instance
 * @param tx_buffer         - buffer to transmit
 *
 * @return NfcError value
 */
NfcError nfc_iso14443a_listener_tx_custom_parity(Nfc* instance, const BitBuffer* tx_buffer);

/** Set Iso14443 collision resolution parameters in listener mode
 *
 * @param instance          - Nfc instance
 * @param uid               - UID value
 * @param uid_len           - UID length in bytes
 * @param atqa              - ATQA value
 * @param sak               - SAK value
 *
 * @return NfcError value
 */
NfcError nfc_iso14443a_listener_set_col_res_data(
    Nfc* instance,
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak);

#ifdef __cplusplus
}
#endif
