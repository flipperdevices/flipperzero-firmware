#include "nfc_cli_commands.h"
#include "nfc_cli_command_base_i.h"

#include "commands/raw/nfc_cli_command_raw.h"
#include "commands/apdu/nfc_cli_command_apdu.h"
#include "commands/dump/nfc_cli_command_dump.h"
#include "commands/nfc_cli_command_emulate.h"
#include "commands/nfc_cli_command_mfu.h"
#include "commands/nfc_cli_command_scanner.h"

#define TAG "NfcCliCommands"

static const NfcCliCommandDescriptor* nfc_cli_commands[NfcCliCommandDescriptorNum] = {
    [NfcCliCommandDescriptorApdu] = &apdu_cmd,
    [NfcCliCommandDescriptorRaw] = &raw_cmd,
    [NfcCliCommandDescriptorEmulate] = &emulate_cmd,
    [NfcCliCommandDescriptorMfu] = &mfu_cmd,
    [NfcCliCommandDescriptorScanner] = &scanner_cmd,
    [NfcCliCommandDescriptorDump] = &dump_cmd,
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

static const NfcCliKeyDescriptor* nfc_cli_action_get_key_by_n(
    const NfcCliActionDescriptor* action,
    const FuriString* name,
    bool long_name) {
    for(size_t i = 0; i < action->key_count; i++) {
        const NfcCliKeyDescriptor* key = &action->keys[i];
        const char* buf = long_name ? key->long_name : key->short_name;
        if((buf != NULL) && furi_string_equal_str(name, buf)) return key;
    }
    return NULL;
}

const NfcCliKeyDescriptor*
    nfc_cli_action_get_key_descriptor(const NfcCliActionDescriptor* action, FuriString* argument) {
    furi_assert(action);
    furi_assert(argument);

    return nfc_cli_action_get_key_by_n(action, argument, furi_string_size(argument) > 1);
}

size_t nfc_cli_action_get_required_keys_count(const NfcCliActionDescriptor* action) {
    furi_assert(action);

    size_t required_key_count = 0;
    for(size_t i = 0; i < action->key_count; i++) {
        const NfcCliKeyDescriptor* key = &action->keys[i];
        if(!key->features.required) continue;
        required_key_count++;
    }
    return required_key_count;
}

/* static void nfc_cli_subscribe_commands(NfcCliContext* instance) {
    size_t cnt = nfc_cli_command_get_count();
    for(size_t i = 0; i < cnt; i++) {
        const NfcCliCommandDescriptor* cmd = nfc_cli_command_get_by_index(i);
        CliExecuteCallback callback = nfc_cli_command_get_execute(cmd);
        if(callback != NULL) {
            const char* name = nfc_cli_command_get_name(cmd);
            cli_add_command(
                instance->nfc_cli,
                name,
                CliCommandFlagParallelUnsafe,
                callback,
                instance->processor_context);
        }
    }
} */
