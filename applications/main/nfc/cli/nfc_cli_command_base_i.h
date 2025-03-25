#pragma once

#include "nfc_cli_command_base.h"
#include <toolbox/cli/cli_ansi.h>
#include <nfc/nfc.h>
#include <nfc/protocols/nfc_protocol.h>
#include "nfc_cli_command_processor.h"

struct FURI_PACKED NfcCliKeyFeatureSupport {
    bool required   : 1;
    bool parameter  : 1;
    bool multivalue : 1;
};

struct NfcCliKeyDescriptor {
    NfcCliKeyFeatureSupport features;
    const char* long_name;
    const char* short_name;
    const char* description;

    NfcCliArgParseCallback parse;
};

struct NfcCliActionDescriptor {
    const char* name;
    const char* description;
    size_t key_count;
    const NfcCliKeyDescriptor* keys;

    NfcCliCommandHandlerCallback execute;
    NfcCliActionContextAlloc alloc;
    NfcCliActionContextFree free;
    NfcCliActionContextCanReuse can_reuse;
};

struct NfcCliCommandDescriptor {
    const char* name;
    const char* description;
    size_t action_count;
    const NfcCliActionDescriptor** actions;
    CliCommandExecuteCallback callback;
};

#define ADD_NFC_CLI_COMMAND(name, description, actions)                  \
    static void nfc_cli_command_##name##_callback(                       \
        PipeSide* pipe, FuriString* args, void* context);                \
                                                                         \
    const NfcCliCommandDescriptor name##_cmd = {                         \
        #name,                                                           \
        #description,                                                    \
        COUNT_OF(actions),                                               \
        actions,                                                         \
        nfc_cli_command_##name##_callback,                               \
    };                                                                   \
                                                                         \
    static void nfc_cli_command_##name##_callback(                       \
        PipeSide* pipe, FuriString* args, void* context) {               \
        nfc_cli_command_processor_run(&name##_cmd, pipe, args, context); \
    }
