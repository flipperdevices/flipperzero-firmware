#pragma once

#include <furi.h>
#include <furi_hal_nfc.h>
#include <furi_hal_spi.h>

#include <drivers/st25r3916.h>
#include <drivers/st25r3916_reg.h>

#ifdef __cplusplus
extern "C" {
#endif

#define FURI_HAL_NFC_POLLER_FDT_COMP_FC (-500)
#define FURI_HAL_NFC_POLLER_FWT_COMP_FC (FURI_HAL_NFC_POLLER_FDT_COMP_FC)

typedef enum {
    FuriHalNfcEventInternalTypeAbort = (1U << 0),
    FuriHalNfcEventInternalTypeIrq = (1U << 1),
    FuriHalNfcEventInternalTypeTimerFwtExpired = (1U << 2),
    FuriHalNfcEventInternalTypeTimerBlockTxExpired = (1U << 3),
    FuriHalNfcEventInternalTypeTransparentDataReceived = (1U << 4),
} FuriHalNfcEventInternalType;

#define FURI_HAL_NFC_EVENT_INTERNAL_ALL                                   \
    ((FuriHalNfcEventInternalTypeAbort | FuriHalNfcEventInternalTypeIrq | \
      FuriHalNfcEventInternalTypeTimerFwtExpired |                        \
      FuriHalNfcEventInternalTypeTimerBlockTxExpired |                    \
      FuriHalNfcEventInternalTypeTransparentDataReceived))

typedef struct {
    FuriThreadId thread;
    void* context;
} FuriHalNfcEventInternal;

typedef struct {
    FuriMutex* mutex;
    FuriHalNfcMode mode;
    FuriHalNfcTech tech;
} FuriHalNfc;

// Technology specific API
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

extern FuriHalNfc furi_hal_nfc;

void furi_hal_nfc_event_init();

void furi_hal_nfc_event_set(FuriHalNfcEventInternalType event);

void furi_hal_nfc_init_gpio_isr();

void furi_hal_nfc_deinit_gpio_isr();

void furi_hal_nfc_timers_init();

void furi_hal_nfc_timers_deinit();

uint32_t furi_hal_nfc_get_irq(FuriHalSpiBusHandle* handle);

bool furi_hal_nfc_event_wait_for_specific_irq(
    FuriHalSpiBusHandle* handle,
    uint32_t mask,
    uint32_t timeout_ms);

// Common technology methods
FuriHalNfcEvent furi_hal_nfc_wait_event_common(uint32_t timeout_ms);

FuriHalNfcError furi_hal_nfc_common_listener_rx_start(FuriHalSpiBusHandle* handle);

FuriHalNfcError furi_hal_nfc_common_fifo_tx(
    FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits);

FuriHalNfcError furi_hal_nfc_common_fifo_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits);

FuriHalNfcError furi_hal_nfc_poller_tx_common(
    FuriHalSpiBusHandle* handle,
    const uint8_t* tx_data,
    size_t tx_bits);

#ifdef __cplusplus
}
#endif
