#pragma once

#include "nfc_cli_protocol_support_base.h"

const char* nfc_cli_protocol_support_get_command_name(NfcCliProtocolSupportCommand cmd);

extern const NfcCliProtocolSupportBase* nfc_cli_protocol_support[];
