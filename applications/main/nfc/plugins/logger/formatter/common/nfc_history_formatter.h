#pragma once

#include "../nfc_formatter_context.h"

#include <furi.h>
#include <furi_hal_nfc.h>
#include <nfc/helpers/logger/history/nfc_history.h>
#include <nfc/helpers/logger/transaction/nfc_packet_data_type.h>

#ifdef __cplusplus
extern "C" {
#endif

void nfc_histroy_format_annotation(
    const NfcFormatter* instance,
    const NfcPacket* request,
    const NfcHistory* history,
    const FuriHalNfcEvent nfc_event,
    FuriString* annotation);

const NfcHistoryItem* nfc_history_get_item(const NfcHistory* history, const NfcProtocol protocol);

#ifdef __cplusplus
}
#endif
