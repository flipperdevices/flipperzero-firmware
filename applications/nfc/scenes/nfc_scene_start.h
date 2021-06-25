#pragma once

#include <nfc/scenes/nfc_scene.h>

typedef struct {
    NfcScene scene;
} NfcSceneStart;

NfcSceneStart* nfc_scence_start_alloc();

void nfc_scence_start_free(NfcSceneStart* scene);
