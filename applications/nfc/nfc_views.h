#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <gui/canvas.h>
#include <flipper_v2.h>

#include "nfc_types.h"

typedef enum {
    NfcViewRead,
} NfcView;

typedef enum {
    NfcViewReadModelStatusInitializing,
    NfcViewReadModelStatusReady,
    NfcViewReadModelStatusError,
    NfcViewReadModelStatusSearching,
    NfcViewReadModelStatusFound,
} NfcViewReadModelStatus;

typedef struct {
    NfcViewReadModelStatus status;
    ReturnCode error;
    NfcDevice device;
} NfcViewReadModel;

void nfc_view_read_draw(Canvas* canvas, void* model);
void nfc_view_read_nfca_draw(Canvas* canvas, NfcViewReadModel* model);
void nfc_view_read_nfcb_draw(Canvas* canvas, NfcViewReadModel* model);
void nfc_view_read_nfcf_draw(Canvas* canvas, NfcViewReadModel* model);
void nfc_view_read_nfcv_draw(Canvas* canvas, NfcViewReadModel* model);
uint32_t nfc_view_read_exit(void* context);