#pragma once

#include "../nfc_i.h"

typedef void (*NfcGeneratorFunc)(NfcDeviceData* data);

typedef struct {
    const char* name;
    NfcGeneratorFunc generator_func;
    NfcScene next_scene;
} NfcGenerator;

extern const NfcGenerator* const nfc_generators[];