#include "nfc_hal_formatter.h"
#include <furi_hal_nfc.h>

static const char* events[] = {
    [NfcEventTypeUserAbort] = "Abort",
    [NfcEventTypeFieldOn] = "Field On",
    [NfcEventTypeFieldOff] = "Field Off",
    [NfcEventTypeTxStart] = "TxStart",
    [NfcEventTypeTxEnd] = "TxEnd",
    [NfcEventTypeRxStart] = "RxStart",
    [NfcEventTypeRxEnd] = "RxEnd",

    [NfcEventTypeListenerActivated] = "LstActived",
    [NfcEventTypePollerReady] = "PollerReady",
};

static const char* hal_nfc_events[] = {
    [FuriHalNfcBitMaskOscOn] = "OscOn",
    [FuriHalNfcBitMaskFieldOn] = "Field On",
    [FuriHalNfcBitMaskFieldOff] = "Field Off",
    [FuriHalNfcBitMaskListenerActive] = "Active",
    [FuriHalNfcBitMaskTxStart] = "TxStart",
    [FuriHalNfcBitMaskTxEnd] = "TxEnd",
    [FuriHalNfcBitMaskRxStart] = "RxStart",
    [FuriHalNfcBitMaskRxEnd] = "RxEnd",
    [FuriHalNfcBitMaskCollision] = "Collision",
    [FuriHalNfcBitMaskTimerFwtExpired] = "FwtExpired",
    [FuriHalNfcBitMaskTimerBlockTxExpired] = "BlockTxExpired",
    [FuriHalNfcBitMaskTimeout] = "Timeout",
    [FuriHalNfcBitMaskAbortRequest] = "Abort",
};

static const char* commands[] = {
    [NfcCommandContinue] = "Continue",
    [NfcCommandReset] = "Reset",
    [NfcCommandSleep] = "Sleep",
    [NfcCommandStop] = "Stop",
};

const char* nfc_hal_data_format_nfc_command(const NfcCommand command) {
    return commands[command];
}

const char* nfc_hal_data_format_event_type(const NfcEventType event) {
    return events[event];
}

void nfc_hal_data_format_hal_event_type(const FuriHalNfcEvent event, FuriString* output) {
    uint8_t flags_cnt = 0;
    for(size_t i = 0; i < COUNT_OF(hal_nfc_events); i++) {
        uint32_t flag_mask = (1 << i);
        if(event & flag_mask) {
            furi_string_cat_printf(output, (flags_cnt > 0) ? ", %s" : "%s", hal_nfc_events[i]);
            flags_cnt++;
        }
    }
}
