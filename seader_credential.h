#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <storage/storage.h>
#include <dialogs/dialogs.h>

#define SEADER_CRED_NAME_MAX_LEN 22
#define SEADER_APP_EXTENSION ".credential"

typedef void (*SeaderLoadingCallback)(void* context, bool state);

typedef enum {
    SeaderCredentialTypeNone,
    SeaderCredentialTypePicopass,
    SeaderCredentialType14A,
    // Might need to make 14a into "javacard" and add Desfire
    SeaderCredentialTypeMifareClassic,
} SeaderCredentialType;

typedef enum {
    SeaderCredentialSaveFormatAgnostic,
    SeaderCredentialSaveFormatPicopass,
    SeaderCredentialSaveFormatRFID,
} SeaderCredentialSaveFormat;

typedef struct {
    Storage* storage;
    DialogsApp* dialogs;
    uint64_t credential;
    size_t bit_length;
    SeaderCredentialType type;
    SeaderCredentialSaveFormat save_format;
    char name[SEADER_CRED_NAME_MAX_LEN + 1];
    FuriString* load_path;
    SeaderLoadingCallback loading_cb;
    void* loading_cb_ctx;
} SeaderCredential;

SeaderCredential* seader_credential_alloc();

void seader_credential_free(SeaderCredential* seader_cred);

void seader_credential_set_loading_callback(
    SeaderCredential* cred,
    SeaderLoadingCallback callback,
    void* context);

void seader_credential_set_name(SeaderCredential* cred, const char* name);

bool seader_credential_save(SeaderCredential* cred, const char* name);

bool seader_file_select(SeaderCredential* cred);

void seader_credential_clear(SeaderCredential* cred);

bool seader_credential_delete(SeaderCredential* cred, bool use_load_path);
