#pragma once

#include <nfc/protocols/nfc_device_base_i.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FELICA_SEAC_IDM_SIZE     (8U)
#define FELICA_SEAC_CONTEXT_SIZE (2U)

typedef enum {
    FelicaSeacErrorNone,
    FelicaSeacErrorNotPresent,
    FelicaSeacErrorWrongCrc,
    FelicaSeacErrorProtocol,
    FelicaSeacErrorTimeout,
} FelicaSeacError;

typedef struct {
    uint8_t idm[FELICA_SEAC_IDM_SIZE];
    uint8_t context[FELICA_SEAC_CONTEXT_SIZE];
} FelicaSeacData;

extern const NfcDeviceBase nfc_device_felica_seac;

#ifdef __cplusplus
}
#endif
