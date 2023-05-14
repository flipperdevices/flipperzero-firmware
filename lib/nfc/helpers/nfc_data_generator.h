#pragma once

#include <lib/nfc/nfc_device_data.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    NfcDataGeneratorTypeMfUltralight,
    NfcDataGeneratorTypeMfUltralightEV1_11,
    NfcDataGeneratorTypeMfUltralightEV1_H11,
    NfcDataGeneratorTypeMfUltralightEV1_21,
    NfcDataGeneratorTypeMfUltralightEV1_H21,
    NfcDataGeneratorTypeNTAG203,
    NfcDataGeneratorTypeNTAG213,
    NfcDataGeneratorTypeNTAG215,
    NfcDataGeneratorTypeNTAG216,
    NfcDataGeneratorTypeNTAGI2C1k,
    NfcDataGeneratorTypeNTAGI2C2k,
    NfcDataGeneratorTypeNTAGI2CPlus1k,
    NfcDataGeneratorTypeNTAGI2CPlus2k,

    NfcDataGeneratorTypeMfClassicMini,
    NfcDataGeneratorTypeMfClassic1k_4b,
    NfcDataGeneratorTypeMfClassic1k_7b,
    NfcDataGeneratorTypeMfClassic2k_4b,
    NfcDataGeneratorTypeMfClassic2k_7b,

    NfcDataGeneratorTypeNum,

} NfcDataGeneratorType;

const char* nfc_data_generator_get_name(NfcDataGeneratorType type);

void nfc_data_generator_fill_data(NfcDataGeneratorType type, NfcDevData* data);

#ifdef __cplusplus
}
#endif
