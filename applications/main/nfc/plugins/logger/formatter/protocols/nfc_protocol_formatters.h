#pragma once

#include "nfc_protocol_formatter_base.h"

#ifdef __cplusplus
extern "C" {
#endif

const NfcProtocolFormatterBase* nfc_protocol_formatter_get(NfcProtocol protocol, NfcMode mode);
NfcTransactionDecryptCallback
    nfc_protocol_formatter_get_decrypt_callback(NfcProtocol protocol, NfcMode mode);

bool nfc_protocol_formatter_support_decryption(NfcProtocol protocol, NfcMode mode);

#ifdef __cplusplus
}
#endif
