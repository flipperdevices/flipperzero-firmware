#include "nfc_protocol_formatters.h"

#include "iso14443_3a/iso14443_3a_listener_data_formatter.h"
#include "iso14443_4a/iso14443_4a_listener_data_formatter.h"
#include "iso15693_3/iso15693_3_listener_data_formatter.h"
#include "mf_ultralight/mf_ultralight_listener_data_formatter.h"
#include "mf_classic/mf_classic_listener_data_formatter.h"
#include "felica/felica_listener_data_formatter.h"
#include "slix/slix_listener_data_formatter.h"

#include "iso14443_3a/iso14443_3a_poller_data_formatter.h"
#include "iso14443_4a/iso14443_4a_poller_data_formatter.h"
#include "iso14443_3b/iso14443_3b_poller_data_formatter.h"
#include "iso14443_4b/iso14443_4b_poller_data_formatter.h"
#include "iso15693_3/iso15693_3_poller_data_formatter.h"
#include "mf_ultralight/mf_ultralight_poller_data_formatter.h"
#include "mf_plus/mf_plus_poller_data_formatter.h"
#include "mf_desfire/mf_desfire_poller_data_formatter.h"
#include "mf_classic/mf_classic_poller_data_formatter.h"
#include "felica/felica_poller_data_formatter.h"
#include "slix/slix_poller_data_formatter.h"
#include "st25tb/st25tb_poller_data_formatter.h"

#define TAG "NfcProtocolFormatter"

static const NfcProtocolFormatterBase* listener_formatters[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &iso14443_3a_listener_data_formatter,
    [NfcProtocolIso14443_4a] = &iso14443_4a_listener_data_formatter,
    [NfcProtocolIso14443_3b] = NULL,
    [NfcProtocolIso14443_4b] = NULL,
    [NfcProtocolIso15693_3] = &iso15693_3_listener_data_formatter,
    [NfcProtocolFelica] = &felica_listener_data_formatter,
    [NfcProtocolMfUltralight] = &mf_ultralight_listener_data_formatter,
    [NfcProtocolMfClassic] = &mf_classic_listener_data_formatter,
    [NfcProtocolMfPlus] = NULL,
    [NfcProtocolMfDesfire] = NULL,
    [NfcProtocolSlix] = &slix_listener_data_formatter,
    [NfcProtocolSt25tb] = NULL,
};

static const NfcProtocolFormatterBase* poller_formatters[NfcProtocolNum] = {
    [NfcProtocolIso14443_3a] = &iso14443_3a_poller_data_formatter,
    [NfcProtocolIso14443_3b] = &iso14443_3b_poller_data_formatter,
    [NfcProtocolIso14443_4a] = &iso14443_4a_poller_data_formatter,
    [NfcProtocolIso14443_4b] = &iso14443_4b_poller_data_formatter,
    [NfcProtocolIso15693_3] = &iso15693_3_poller_data_formatter,
    [NfcProtocolFelica] = &felica_poller_data_formatter,
    [NfcProtocolMfUltralight] = &mf_ultralight_poller_data_formatter,
    [NfcProtocolMfClassic] = &mf_classic_poller_data_formatter,
    [NfcProtocolMfPlus] = &mf_plus_poller_data_formatter,
    [NfcProtocolMfDesfire] = &mf_desfire_poller_data_formatter,
    [NfcProtocolSlix] = &slix_poller_data_formatter,
    [NfcProtocolSt25tb] = &st25tb_poller_data_formatter,
};

const NfcProtocolFormatterBase* nfc_protocol_formatter_get(NfcProtocol protocol, NfcMode mode) {
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(mode < NfcModeNum);

    const NfcProtocolFormatterBase** formatters = (mode == NfcModeListener) ? listener_formatters :
                                                                              poller_formatters;

    const NfcProtocolFormatterBase* formatter = formatters[protocol];
    if(!formatter) {
        FURI_LOG_W(TAG, "Formatter for protocol: %d not found", protocol);
    }
    return formatter;
}

NfcTransactionDecryptCallback
    nfc_protocol_formatter_get_decrypt_callback(NfcProtocol protocol, NfcMode mode) {
    furi_assert(protocol < NfcProtocolNum);
    furi_assert(mode < NfcModeNum);

    const NfcProtocolFormatterBase* formatter = nfc_protocol_formatter_get(protocol, mode);
    return formatter->decrypt;
}

bool nfc_protocol_formatter_support_decryption(NfcProtocol protocol, NfcMode mode) {
    return nfc_protocol_formatter_get_decrypt_callback(protocol, mode) != NULL;
}
