#pragma once

#include "nfc_transaction.h"
#include "nfc_packet_data_type.h"
#include "../history/nfc_history.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t id;
    NfcTransactionType type;
    FuriHalNfcEvent nfc_event;
    uint32_t start_time;
    uint32_t end_time;
} FURI_PACKED NfcTransactionHeader;

struct FURI_PACKED NfcTransaction {
    NfcTransactionHeader header;
    NfcPacket* request;
    NfcPacket* response;
    NfcHistory* history;
};

#ifdef __cplusplus
}
#endif
