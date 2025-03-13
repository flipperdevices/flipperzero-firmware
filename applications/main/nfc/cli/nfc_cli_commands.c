#include "nfc_cli_commands.h"
#include "nfc_cli_command_base_i.h"
#include "commands/nfc_cli_command_raw.h"
#include "commands/nfc_cli_command_apdu.h"
#include "commands/nfc_cli_command_emulate.h"
#include "commands/nfc_cli_command_mfu.h"

static const NfcCliCommandDescriptor* nfc_cli_commands[NfcCliCommandDescriptorNum] = {
    [NfcCliCommandDescriptorApdu] = &apdu_cmd,
    [NfcCliCommandDescriptorRaw] = &raw_cmd,
    [NfcCliCommandDescriptorEmulate] = &emulate_cmd,
    [NfcCliCommandDescriptorMfu] = &mfu_cmd,
};

size_t nfc_cli_command_get_count() {
    return COUNT_OF(nfc_cli_commands);
}

const NfcCliCommandDescriptor* nfc_cli_command_get_by_name(FuriString* name) {
    furi_assert(name);
    const NfcCliCommandDescriptor* cmd = NULL;
    for(size_t i = 0; i < COUNT_OF(nfc_cli_commands); i++) {
        if(!furi_string_equal_str(name, nfc_cli_commands[i]->name)) continue;
        cmd = nfc_cli_commands[i];
        break;
    }
    return cmd;
}

const NfcCliActionDescriptor*
    nfc_cli_command_get_action_by_name(const NfcCliCommandDescriptor* cmd, const FuriString* name) {
    furi_assert(cmd);
    furi_assert(name);

    for(size_t i = 0; i < cmd->action_count; i++) {
        const NfcCliActionDescriptor* action = cmd->actions[i];
        if(furi_string_equal_str(name, action->name)) return action;
    }
    return NULL;
}

///TODO: we need to modify all cmd description structs in order to be able to search on them with one iterator
//static void* nfc_cli_get_item_by_name(const FuriString* name, const )

const NfcCliCommandDescriptor* nfc_cli_command_get_by_index(size_t index) {
    furi_assert(index < NfcCliCommandDescriptorNum);
    return nfc_cli_commands[index];
}

bool nfc_cli_command_has_multiple_actions(const NfcCliCommandDescriptor* cmd) {
    furi_assert(cmd);
    furi_check(cmd->action_count > 0);
    return (cmd->action_count > 1);
}

const char* nfc_cli_command_get_name(const NfcCliCommandDescriptor* cmd) {
    furi_assert(cmd);
    return cmd->name;
}

CliExecuteCallback nfc_cli_command_get_execute(const NfcCliCommandDescriptor* cmd) {
    furi_assert(cmd);
    return cmd->callback;
}
/* size_t nfc_cli_command_get_count() {
    return COUNT_OF(nfc_cli_commands);
} */
