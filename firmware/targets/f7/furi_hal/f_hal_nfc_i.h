#pragma once

#include <furi.h>
#include <f_hal_nfc.h>
#include <furi_hal_spi.h>
#include <drivers/st25r3916.h>
#include <drivers/st25r3916_reg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FHalNfcEventInternalTypeAbort = (1U << 0),
    FHalNfcEventInternalTypeIrq = (1U << 1),
    FHalNfcEventInternalTypeTimerFwtExpired = (1U << 2),
    FHalNfcEventInternalTypeTimerBlockTxExpired = (1U << 3),
    FHalNfcEventInternalTypeTransparentDataReceived = (1U << 4),
} FHalNfcEventInternalType;

#define F_HAL_NFC_EVENT_INTERNAL_ALL                                                          \
    ((FHalNfcEventInternalTypeAbort | FHalNfcEventInternalTypeIrq |                           \
      FHalNfcEventInternalTypeTimerFwtExpired | FHalNfcEventInternalTypeTimerBlockTxExpired | \
      FHalNfcEventInternalTypeTransparentDataReceived))

typedef struct {
    FuriThreadId thread;
    void* context;
} FHalNfcEventInternal;

typedef struct {
    FuriMutex* mutex;
    FHalNfcMode mode;
    FHalNfcTech tech;
} FHalNfc;

extern FHalNfc f_hal_nfc;

void f_hal_nfc_event_init();

void f_hal_nfc_event_set(FHalNfcEventInternalType event);

void f_hal_nfc_init_gpio_isr();

void f_hal_nfc_deinit_gpio_isr();

void f_hal_nfc_timers_init();

void f_hal_nfc_timers_deinit();

uint32_t f_hal_nfc_get_irq(FuriHalSpiBusHandle* handle);

bool f_hal_nfc_event_wait_for_specific_irq(
    FuriHalSpiBusHandle* handle,
    uint32_t mask,
    uint32_t timeout_ms);

// Common technology methods
FHalNfcEvent f_hal_nfc_wait_event_common(uint32_t timeout_ms);
FHalNfcError f_hal_nfc_common_listener_rx_start(FuriHalSpiBusHandle* handle);
FHalNfcError
    f_hal_nfc_common_fifo_tx(FuriHalSpiBusHandle* handle, const uint8_t* tx_data, size_t tx_bits);
FHalNfcError f_hal_nfc_common_fifo_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits);

FHalNfcError
    f_hal_nfc_poller_tx_common(FuriHalSpiBusHandle* handle, const uint8_t* tx_data, size_t tx_bits);

// Technology specific API
typedef FHalNfcError (*FHalNfcChipConfig)(FuriHalSpiBusHandle* handle);
typedef FHalNfcError (
    *FHalNfcTx)(FuriHalSpiBusHandle* handle, const uint8_t* tx_data, size_t tx_bits);
typedef FHalNfcError (*FHalNfcRx)(
    FuriHalSpiBusHandle* handle,
    uint8_t* rx_data,
    size_t rx_data_size,
    size_t* rx_bits);
typedef FHalNfcEvent (*FHalNfcWaitEvent)(uint32_t timeout_ms);
typedef FHalNfcError (*FHalNfcSleep)(FuriHalSpiBusHandle* handle);
typedef FHalNfcError (*FHalNfcIdle)(FuriHalSpiBusHandle* handle);

typedef struct {
    FHalNfcChipConfig init;
    FHalNfcChipConfig deinit;
    FHalNfcWaitEvent wait_event;
    FHalNfcTx tx;
    FHalNfcRx rx;
} FHalNfcTechPollerBase;

typedef struct {
    FHalNfcChipConfig init;
    FHalNfcChipConfig deinit;
    FHalNfcWaitEvent wait_event;
    FHalNfcTx tx;
    FHalNfcRx rx;
    FHalNfcSleep sleep;
    FHalNfcIdle idle;
} FHalNfcTechListenerBase;

typedef struct {
    FHalNfcTechPollerBase poller;
    FHalNfcTechListenerBase listener;
} FHalNfcTechBase;

extern const FHalNfcTechBase f_hal_nfc_iso14443a;
extern const FHalNfcTechBase f_hal_nfc_iso14443b;
extern const FHalNfcTechBase f_hal_nfc_iso15693;
extern const FHalNfcTechBase f_hal_nfc_felica;

#ifdef __cplusplus
}
#endif
