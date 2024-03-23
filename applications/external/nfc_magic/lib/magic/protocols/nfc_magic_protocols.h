#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcMagicProtocolGen1,
    NfcMagicProtocolGen2,
    NfcMagicProtocolGen4,
    NfcMagicProtocolClassic, // Last to give priority to the others

    NfcMagicProtocolNum,
    NfcMagicProtocolInvalid,
} NfcMagicProtocol;

const char* nfc_magic_protocols_get_name(NfcMagicProtocol protocol);

#ifdef __cplusplus
}
#endif
