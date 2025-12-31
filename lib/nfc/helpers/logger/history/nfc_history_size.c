#include "nfc_history_size.h"
#include "nfc_history_i.h"
#include "nfc_history_chain_data_type.h"

#include <nfc/protocols/iso14443_3a/iso14443_3a_listener_history_data.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a_listener_history_data.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight_listener_history_data.h>
#include <nfc/protocols/mf_classic/mf_classic_listener_history_data.h>
#include <nfc/protocols/iso15693_3/iso15693_3_listener_history_data.h>
#include <nfc/protocols/felica/felica_listener_history_data.h>
#include <nfc/protocols/slix/slix_listener_history_data.h>

#include <nfc/protocols/iso14443_3a/iso14443_3a_poller_history_data.h>
#include <nfc/protocols/iso14443_3b/iso14443_3b_poller_history_data.h>
#include <nfc/protocols/iso14443_4a/iso14443_4a_poller_history_data.h>
#include <nfc/protocols/iso14443_4b/iso14443_4b_poller_history_data.h>
#include <nfc/protocols/mf_classic/mf_classic_poller_history_data.h>
#include <nfc/protocols/iso15693_3/iso15693_3_poller_history_data.h>
#include <nfc/protocols/mf_desfire/mf_desfire_poller_history_data.h>
#include <nfc/protocols/mf_plus/mf_plus_poller_history_data.h>
#include <nfc/protocols/mf_ultralight/mf_ultralight_poller_history_data.h>
#include <nfc/protocols/felica/felica_poller_history_data.h>
#include <nfc/protocols/slix/slix_poller_history_data.h>
#include <nfc/protocols/st25tb/st25tb_poller_history_data.h>

#define TAG "NfcHistorySize"

static const uint8_t listener_history_chain_size[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = sizeof(Iso14443_3aListenerHistoryData),
    [NfcProtocolIso14443_4a] = sizeof(Iso14443_4aListenerHistoryData),
    [NfcProtocolIso15693_3] = sizeof(Iso15693_3ListenerHistoryData),
    [NfcProtocolMfUltralight] = sizeof(MfUltralightListenerHistoryData),
    [NfcProtocolFelica] = sizeof(FelicaListenerHistoryData),
    [NfcProtocolSlix] = sizeof(SlixListenerHistoryData),
    [NfcProtocolMfClassic] = sizeof(MfClassicListenerHistoryData),
    [NfcProtocolMfPlus] = 0,
    [NfcProtocolMfDesfire] = 0,
    [NfcProtocolIso14443_3b] = 0,
    [NfcProtocolSt25tb] = 0,
    [NfcProtocolIso14443_4b] = 0,
};

static const uint8_t poller_history_chain_size[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = sizeof(Iso14443_3aPollerHistoryData),
    [NfcProtocolIso14443_3b] = sizeof(Iso14443_3bPollerHistoryData),
    [NfcProtocolIso14443_4a] = sizeof(Iso14443_4aPollerHistoryData),
    [NfcProtocolIso14443_4b] = sizeof(Iso14443_4bPollerHistoryData),
    [NfcProtocolIso15693_3] = sizeof(Iso15693_3PollerHistoryData),
    [NfcProtocolMfDesfire] = sizeof(MfDesfirePollerHistoryData),
    [NfcProtocolMfPlus] = sizeof(MfPlusPollerHistoryData),
    [NfcProtocolMfUltralight] = sizeof(MfUltralightPollerHistoryData),
    [NfcProtocolFelica] = sizeof(FelicaPollerHistoryData),
    [NfcProtocolSt25tb] = sizeof(St25tbPollerHistoryData),
    [NfcProtocolSlix] = sizeof(SlixPollerHistoryData),
    [NfcProtocolMfClassic] = sizeof(MfClassicPollerHistoryData),
};

static uint8_t nfc_history_get_chain_size_bytes(NfcProtocol protocol, NfcMode mode) {
    const uint8_t* history_data_size_source =
        (mode == NfcModeListener) ? listener_history_chain_size : poller_history_chain_size;

    uint8_t chain_size_bytes = sizeof(NfcChainLength);
    do {
        chain_size_bytes += sizeof(NfcHistoryItemBase);
        uint8_t buf = history_data_size_source[protocol];

        FURI_LOG_D(
            TAG,
            "Protocol: %d data_size: %d item base: %d",
            protocol,
            buf,
            sizeof(NfcHistoryItemBase));

        chain_size_bytes += buf;
        protocol = nfc_protocol_get_parent(protocol);
    } while(protocol != NfcProtocolInvalid);
    FURI_LOG_D(TAG, "Chain size: %d", chain_size_bytes);
    return chain_size_bytes;
}

uint8_t nfc_history_get_size_bytes(NfcProtocol protocol, NfcMode mode, uint8_t max_chain_count) {
    const uint8_t chain_size_bytes = nfc_history_get_chain_size_bytes(protocol, mode);
    uint8_t history_size_bytes = sizeof(NfcHistoryBase) + chain_size_bytes * max_chain_count;
    FURI_LOG_D(TAG, "History size: %d", history_size_bytes);
    return history_size_bytes;
}
