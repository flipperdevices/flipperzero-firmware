#include "nfc_magic_protocols.h"

#include <furi/furi.h>

static const char* nfc_magic_protocol_names[NfcMagicProtocolNum] = {
    [NfcMagicProtocolGen1] = "Classic Gen 1A/B",
    [NfcMagicProtocolGen2] = "Classic Gen 2",
    [NfcMagicProtocolClassic] = "Classic (possibly not magic)",
    [NfcMagicProtocolGen4] = "Gen 4 GTU",
};

const char* nfc_magic_protocols_get_name(NfcMagicProtocol protocol) {
    furi_assert(protocol < NfcMagicProtocolNum);

    return nfc_magic_protocol_names[protocol];
}