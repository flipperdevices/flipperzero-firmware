#pragma once

#include <furi_hal_nfc.h>
#include <furi_hal_spi.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef FuriHalNfcError (*FuriHalNfcChipConfig)(FuriHalSpiBusHandle* handle);
typedef FuriHalNfcError (
    *FuriHalNfcTx)(FuriHalSpiBusHandle* handle, const uint8_t* tx_data, size_t tx_bits);
typedef FuriHalNfcError (*FuriHalNfcRx)(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits);
typedef FuriHalNfcEvent (*FuriHalNfcWaitEvent)(uint32_t timeout_ms);
typedef FuriHalNfcError (*FuriHalNfcSleep)(FuriHalSpiBusHandle* handle);
typedef FuriHalNfcError (*FuriHalNfcIdle)(FuriHalSpiBusHandle* handle);

typedef struct {
    int32_t fdt;
    int32_t fwt;
} FuriHalNfcPollerCompensation;

typedef struct {
    FuriHalNfcPollerCompensation compensation;
    FuriHalNfcChipConfig init;
    FuriHalNfcChipConfig deinit;
    FuriHalNfcWaitEvent wait_event;
    FuriHalNfcTx tx;
    FuriHalNfcRx rx;
} FuriHalNfcTechPollerBase;

typedef struct {
    int32_t fdt;
} FuriHalNfcListenerCompensation;

typedef struct {
    FuriHalNfcListenerCompensation compensation;
    FuriHalNfcChipConfig init;
    FuriHalNfcChipConfig deinit;
    FuriHalNfcWaitEvent wait_event;
    FuriHalNfcTx tx;
    FuriHalNfcRx rx;
    FuriHalNfcSleep sleep;
    FuriHalNfcIdle idle;
} FuriHalNfcTechListenerBase;

typedef struct {
    FuriHalNfcTechPollerBase poller;
    FuriHalNfcTechListenerBase listener;
} FuriHalNfcTechBase;

extern const FuriHalNfcTechBase furi_hal_nfc_iso14443a;
extern const FuriHalNfcTechBase furi_hal_nfc_iso14443b;
extern const FuriHalNfcTechBase furi_hal_nfc_iso15693;
extern const FuriHalNfcTechBase furi_hal_nfc_felica;

extern const FuriHalNfcTechBase* furi_hal_nfc_tech[];

#ifdef __cplusplus
}
#endif
