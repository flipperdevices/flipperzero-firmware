#pragma once

#include <nfc/protocols/nfc_protocol.h>
#include <furi/core/string.h>
#include <cli/cli.h>

typedef enum {
    NfcCliProtocolSupportCommandPoll,
    NfcCliProtocolSupportCommandStartPoller,

    NfcCliProtocolSupportCommandNum,
} NfcCliProtocolSupportCommand;

typedef void (*NfcCliProtocolSupportCommandHandler)(Cli* cli, FuriString* args);

typedef struct {
    NfcProtocol protocol;
    const char* cmd_name;
    NfcCliProtocolSupportCommandHandler cmd_handler[NfcCliProtocolSupportCommandNum];
} NfcCliProtocolSupportBase;
