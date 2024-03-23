#include "nfc_magic_protocols.h"

#include <furi/furi.h>

static const char* nfc_magic_protocol_names[NfcMagicProtocolNum] = {
    [NfcMagicProtocolGen1] = "Gen1A/B",
    [NfcMagicProtocolGen2] = "Gen2",
    [NfcMagicProtocolClassic] = "MIFARE Classic",
    [NfcMagicProtocolGen4] = "Gen4 GTU",
};

const char* nfc_magic_protocols_get_name(NfcMagicProtocol protocol) {
    furi_assert(protocol < NfcMagicProtocolNum);

    return nfc_magic_protocol_names[protocol];
}