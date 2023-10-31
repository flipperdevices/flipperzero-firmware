#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcMagicProtocolGen1,
    NfcMagicProtocolGen4,

    NfcMagicProtocolNum,
    NfcMagicProtocolInvalid,
} NfcMagicProtocol;

const char* nfc_magic_protocols_get_name(NfcMagicProtocol protocol);

#ifdef __cplusplus
}
#endif
