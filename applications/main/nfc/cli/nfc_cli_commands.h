#pragma once

#include "nfc_cli_command_base.h"
#include <cli/cli.h>

size_t nfc_cli_command_get_count();
const NfcCliCommandDescriptor* nfc_cli_command_get_by_index(size_t index);
const NfcCliCommandDescriptor* nfc_cli_command_get_by_name(FuriString* name);
bool nfc_cli_command_has_multiple_actions(const NfcCliCommandDescriptor* cmd);

const NfcCliActionDescriptor*
    nfc_cli_command_get_action_by_name(const NfcCliCommandDescriptor* cmd, const FuriString* name);

const char* nfc_cli_command_get_name(const NfcCliCommandDescriptor* cmd);

void nfc_cli_command_format_info(const NfcCliCommandDescriptor* cmd, FuriString* output);
void nfc_cli_action_format_info(const NfcCliActionDescriptor* action, FuriString* output);

CliExecuteCallback nfc_cli_command_get_execute(const NfcCliCommandDescriptor* cmd);

size_t nfc_cli_action_get_required_keys_count(const NfcCliActionDescriptor* action);
const NfcCliKeyDescriptor*
    nfc_cli_action_get_key_descriptor(const NfcCliActionDescriptor* action, FuriString* argument);