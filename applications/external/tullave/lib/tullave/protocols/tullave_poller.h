#pragma once

#include <furi.h>
#include <nfc/protocols/nfc_poller_base.h>
#include <nfc/nfc_poller.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller_i.h>
#include <toolbox/bit_buffer.h>
#include <toolbox/hex.h>
#include "../tullave_data.h"

// I have not seeing a response greater than 53 bytes
#define TULLAVE_BIT_BUFFER_MAX 53
// As I know all cards contain only 16 digits
#define TULLAVE_CARD_NUM_LEN 16
// Apparently The highest balance supported by TuLlave is 999999 COP
#define TULLAVE_BAL_LEN 6
// Balance in transaction only have 24 bits (3 bytes)
#define TULLAVE_BAL_TRAN_LEN 3
// Apparently this is the max number of trasnactions supported by the TrasmiApp
#define TULLAVE_HIST_MAX_TRANSACTIONS 10

// APDU SW1 byte indicating the response from the card was successful
// This is used in the first read of the card (read card number)
#define APDU_SW1_GOOD_RESPONSE 0x6f

// NfC Poller custom events
typedef enum {
    //Start enum in 100 to preserve first 100 events for GUI actions. Taken from Nfc Flipper App.
    NfcCustomEventReserved = 100,
    NfcPollerEventReadingData,
    NfcPollerEventSuccess,
    NfcPollerEventError
} NfcCustomEvent;

typedef enum {
    TuLlaveErrorNone = 0x100,
    TuLlaveErrorCardNotSupported,
    TuLlaveErrorCouldNotReadCompleteData
} TuLlaveError;

typedef enum {
    TuLlaveStateReadIdle,
    TuLlaveStateReadCardNumber,
    TuLlaveStateReadBalance,
    TuLlaveStateReadHistory,
    TuLlaveStateReadCompleted,

    TuLlaveStateReadNum
} TuLlaveStateReadState;

typedef struct {
    Iso14443_4aPoller* iso_poller;
    BitBuffer* tx_data;
    BitBuffer* rx_data;
    NfcGenericCallback callback;
    TuLlaveData* card_data;
    TuLlaveError err_code;
    TuLlaveStateReadState read_state;
    void* context;
} TuLlavePoller;

void tullave_poller_free(TuLlavePoller* instance);

TuLlavePoller* tullave_poller_alloc();

NfcCommand tullave_poller_read_iso_data(TuLlavePoller* poller);

NfcCommand tullave_poller_read_card_number(TuLlavePoller* poller);

NfcCommand tullave_poller_read_balance(TuLlavePoller* poller);

NfcCommand tullave_poller_read_history(TuLlavePoller* poller);

NfcCommand tullave_poller_run(NfcGenericEvent event, void* context);