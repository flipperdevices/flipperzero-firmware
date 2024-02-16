#pragma once

#include <nfc/protocols/nfc_protocol.h>

typedef struct {
    NfcProtocol protocol;
    const char* cmd_name;
} NfcCliProtocolSupportBase;
