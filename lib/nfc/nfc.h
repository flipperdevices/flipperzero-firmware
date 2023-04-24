#pragma once

#include <stdint.h>

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

    NfcEventTypeConfigureRequest,

    NfcEventTypeListenerActivated,
    NfcEventTypePollerReady,
} NfcEventType;

typedef struct {
    uint8_t* rx_data;
    uint16_t rx_bits;
} NfcEventData;

typedef struct {
    NfcEventType type;
    NfcEventData data;
} NfcEvent;

typedef enum {
    NfcCommandContinue,
    NfcCommandReset,
    NfcCommandStop,
} NfcCommand;

typedef NfcCommand (*NfcEventCallback)(NfcEvent event, void* context);

typedef enum {
    NfcModeIdle,
    NfcModeNfcaPoller,
    NfcModeNfcaListener,
    NfcModeNfcbPoller,
    NfcModeNfcbListener,
    NfcModeNfcfPoller,
    NfcModeNfcfListener,
    NfcModeNfcvPoller,
    NfcModeNfcvListener,
} NfcMode;

typedef enum {
    NfcErrorNone,
    NfcErrorInternal,
    NfcErrorTimeout,
    NfcErrorWrongState,
    NfcErrorCollision,
    NfcErrorLinkLoss,
    NfcErrorAbortRequest,
} NfcError;

typedef enum {
    NfcIso14443aShortFrameSensReq,
    NfcIso14443aShortFrameAllReqa,
} NfcIso14443aShortFrame;

Nfc* nfc_alloc();

void nfc_free(Nfc* instance);

void nfc_config(Nfc* instance, NfcMode mode);

void nfc_set_fdt_poll_fc(Nfc* instance, uint32_t fdt_poll_fc);

void nfc_set_fdt_listen_fc(Nfc* instance, uint32_t fdt_listen_fc);

void nfc_set_mask_receive_time_fc(Nfc* instance, uint32_t mask_rx_time_fc);

void nfc_set_fdt_poll_poll_us(Nfc* instance, uint32_t fdt_poll_poll_us);

void nfc_set_guard_time_us(Nfc* instance, uint32_t guard_time_us);

NfcError nfc_listener_set_col_res_data(
    Nfc* instance,
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak);

void nfc_start_worker(Nfc* instance, NfcEventCallback callback, void* context);

NfcError nfc_listener_sleep(Nfc* instance);

void nfc_listener_abort(Nfc* instance);

// Called from worker thread

void nfc_stop(Nfc* instance);

NfcError nfc_listener_tx(Nfc* instance, uint8_t* tx_data, uint16_t tx_bits);

NfcError nfc_trx(
    Nfc* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt);

// Technology specific API

NfcError nfc_iso13444a_short_frame(
    Nfc* instance,
    NfcIso14443aShortFrame frame,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt);

NfcError nfc_iso13444a_sdd_frame(
    Nfc* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt);

#ifdef __cplusplus
}
#endif
