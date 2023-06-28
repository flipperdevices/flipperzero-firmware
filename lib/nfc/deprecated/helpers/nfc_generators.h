#pragma once

#include "../nfc_device_old.h"

typedef void (*NfcGeneratorFunc)(NfcDeviceOldDataOld* data);

typedef struct {
    const char* name;
    NfcGeneratorFunc generator_func;
} NfcGenerator;

extern const NfcGenerator* const nfc_generators[];

void nfc_generate_mf_classic(NfcDeviceOldDataOld* data, uint8_t uid_len, MfClassicType type);
