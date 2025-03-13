#pragma once

#include <furi.h>

typedef void (*NfcCliCommandHandlerCallback)(void* ctx);

typedef bool (*NfcCliArgParseCallback)(FuriString* arg, void* output);

typedef struct NfcCliKeyFeatureSupport NfcCliKeyFeatureSupport;

typedef struct NfcCliKeyDescriptor NfcCliKeyDescriptor;

typedef struct NfcCliArgumentDescriptor NfcCliArgumentDescriptor;

typedef struct NfcCliActionDescriptor NfcCliActionDescriptor;

typedef struct NfcCliCommandDescriptor NfcCliCommandDescriptor;
