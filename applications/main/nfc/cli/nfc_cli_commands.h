#pragma once

#include "nfc_cli_command_base.h"
#include <cli/cli.h>

typedef enum {
    NfcCliCommandDescriptorRaw,
    NfcCliCommandDescriptorApdu,
    NfcCliCommandDescriptorEmulate,
    NfcCliCommandDescriptorMfu,
    NfcCliCommandDescriptorScanner,
    NfcCliCommandDescriptorDump,
    /* Add new protocols here */

    NfcCliCommandDescriptorNum, /**< Special value representing the number of available commands. */
    NfcCliCommandDescriptorInvalid, /**< Special value representing an invalid state. */
} NfcCliCommandDescriptorIndex;

size_t nfc_cli_command_get_count();
const NfcCliCommandDescriptor* nfc_cli_command_get_by_index(size_t index);
const NfcCliCommandDescriptor* nfc_cli_command_get_by_name(FuriString* name);
bool nfc_cli_command_has_multiple_actions(const NfcCliCommandDescriptor* cmd);

const NfcCliActionDescriptor*
    nfc_cli_command_get_action_by_name(const NfcCliCommandDescriptor* cmd, const FuriString* name);

const char* nfc_cli_command_get_name(const NfcCliCommandDescriptor* cmd);
CliExecuteCallback nfc_cli_command_get_execute(const NfcCliCommandDescriptor* cmd);

size_t nfc_cli_action_get_required_keys_count(const NfcCliActionDescriptor* action);
const NfcCliKeyDescriptor*
    nfc_cli_action_get_key_descriptor(const NfcCliActionDescriptor* action, FuriString* argument);