#pragma once
#include <furi.h>
#include "dialogs_i.h"
#include "dialogs_api_lock.h"
#include "m-string.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char* path;
    const char* extension;
    char* result;
    uint8_t result_size;
    const char* preselected_filename;
} DialogsAppMessageDataFileSelect;

typedef struct {
    const char* extension;
    bool skip_assets;
    bool hide_ext;
    const Icon* file_icon;
    string_t* result_path;
    string_t* preselected_filename;
} DialogsAppMessageDataFileBrowser;

typedef struct {
    const DialogMessage* message;
} DialogsAppMessageDataDialog;

typedef union {
    DialogsAppMessageDataFileSelect file_select;
    DialogsAppMessageDataFileBrowser file_browser;
    DialogsAppMessageDataDialog dialog;
} DialogsAppData;

typedef union {
    bool bool_value;
    DialogMessageButton dialog_value;
} DialogsAppReturn;

typedef enum {
    DialogsAppCommandFileOpen,
    DialogsAppCommandFileBrowser,
    DialogsAppCommandDialog,
} DialogsAppCommand;

typedef struct {
    FuriApiLock lock;
    DialogsAppCommand command;
    DialogsAppData* data;
    DialogsAppReturn* return_data;
} DialogsAppMessage;

#ifdef __cplusplus
}
#endif
