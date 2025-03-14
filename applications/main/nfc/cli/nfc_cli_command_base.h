#pragma once

#include <furi.h>
#include <cli/cli.h>
#include <nfc/nfc.h>

typedef void NfcCliActionContext;

typedef NfcCliActionContext* (*NfcCliActionContextAlloc)(Nfc* nfc);

typedef void (*NfcCliActionContextFree)(NfcCliActionContext* action_ctx);

typedef void (*NfcCliCommandHandlerCallback)(PipeSide* pipe, void* ctx);

typedef bool (*NfcCliArgParseCallback)(FuriString* argument, FuriString* args, void* output);

typedef struct NfcCliKeyFeatureSupport NfcCliKeyFeatureSupport;

typedef struct NfcCliKeyDescriptor NfcCliKeyDescriptor;

typedef struct NfcCliArgumentDescriptor NfcCliArgumentDescriptor;

typedef struct NfcCliActionDescriptor NfcCliActionDescriptor;

typedef struct NfcCliCommandDescriptor NfcCliCommandDescriptor;
