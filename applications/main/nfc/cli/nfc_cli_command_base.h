#pragma once

#include <furi.h>
#include <toolbox/cli/cli_command.h>
#include <nfc/nfc.h>

typedef void NfcCliActionContext;

typedef NfcCliActionContext* (*NfcCliActionContextAlloc)(Nfc* nfc);

typedef void (*NfcCliActionContextFree)(NfcCliActionContext* action_ctx);
typedef bool (*NfcCliActionContextCanReuse)(NfcCliActionContext* ctx);

typedef void (*NfcCliCommandHandlerCallback)(PipeSide* pipe, void* ctx);

typedef bool (*NfcCliArgParseCallback)(FuriString* value, void* output);

typedef struct NfcCliKeyFeatureSupport NfcCliKeyFeatureSupport;

typedef struct NfcCliKeyDescriptor NfcCliKeyDescriptor;

typedef struct NfcCliArgumentDescriptor NfcCliArgumentDescriptor;

typedef struct NfcCliActionDescriptor NfcCliActionDescriptor;

typedef struct NfcCliCommandDescriptor NfcCliCommandDescriptor;
