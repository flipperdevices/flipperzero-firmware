#pragma once

#include "nfc_cli_command_base.h"
#include <nfc/nfc.h>
#include <nfc/protocols/nfc_protocol.h>
#include "nfc_cli_command_processor.h"

struct FURI_PACKED NfcCliKeyFeatureSupport {
    bool required  : 1;
    bool parameter : 1;
};

struct NfcCliKeyDescriptor {
    NfcCliKeyFeatureSupport features;
    const char* long_name;
    const char* short_name;
    const char* description;

    NfcCliArgParseCallback parse;
    // size_t valid_values_count;
    // const char** valid_values;
};

/* struct NfcCliArgumentDescriptor {
    const char* name;
    NfcCliArgParseCallback parse;
}; */

struct NfcCliActionDescriptor {
    const char* name;
    size_t key_count;
    const NfcCliKeyDescriptor* keys;

    NfcCliCommandHandlerCallback execute;
    NfcCliActionContextAlloc alloc;
    NfcCliActionContextFree free;
    NfcCliActionContextCanReuse can_reuse;
};

struct NfcCliCommandDescriptor {
    const char* name;
    const char* description; // optional
    size_t action_count; //optional
    const NfcCliActionDescriptor** actions;
    CliExecuteCallback callback;
};

#define ADD_NFC_CLI_COMMAND(name, actions)                         \
    static void nfc_cli_command_##name##_callback(                 \
        PipeSide* pipe, FuriString* args, void* context);          \
                                                                   \
    const NfcCliCommandDescriptor name##_cmd = {                   \
        #name,                                                     \
        "tempo",                                                   \
        COUNT_OF(actions),                                         \
        actions,                                                   \
        nfc_cli_command_##name##_callback,                         \
    };                                                             \
                                                                   \
    static void nfc_cli_command_##name##_callback(                 \
        PipeSide* pipe, FuriString* args, void* context) {         \
        nfc_cli_command_process(&name##_cmd, pipe, args, context); \
    }
