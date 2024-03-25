#pragma once

#include <nfc/protocols/nfc_protocol.h>
#include "../nfc_cli_i.h"

typedef enum {
    NfcCliProtocolSupportCommandPoll,
    NfcCliProtocolSupportCommandStartPoller,

    NfcCliProtocolSupportCommandNum,
} NfcCliProtocolSupportCommand;

typedef void (*NfcCliProtocolSupportCommandHandler)(NfcCli* nfc_cli, FuriString* args);

typedef struct {
    NfcProtocol protocol;
    const char* cmd_name;
    NfcCliProtocolSupportCommandHandler cmd_handler[NfcCliProtocolSupportCommandNum];
} NfcCliProtocolSupportBase;
