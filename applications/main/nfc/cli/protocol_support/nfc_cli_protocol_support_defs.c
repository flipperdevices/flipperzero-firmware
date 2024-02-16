#include "nfc_cli_protocol_support_defs.h"

#include "iso14443_3a/iso14443_3a.h"

const NfcCliProtocolSupportBase* nfc_cli_protocol_support[1] = {
    [0] = &nfc_cli_protocol_support_base_iso14443_3a,
};
