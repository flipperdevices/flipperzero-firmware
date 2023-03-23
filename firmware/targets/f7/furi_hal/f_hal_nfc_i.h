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
    FHalNfcEventInternalAbort = (1U << 0),
    FHalNfcEventInternalTypeIrq = (1U << 1),
    FHalNfcEventInternalTypeTimerFwtExpired = (1U << 2),
    FHalNfcEventInternalTypeTimerBlockTxExpired = (1U << 3),
} FHalNfcEventInternalType;

typedef struct {
    FuriThreadId thread;
    FHalNfcCallback callback;
    void* context;
} FHalNfcEventInternal;

extern FHalNfcEventInternal* f_hal_nfc;

void f_hal_nfc_event_init();

void f_hal_nfc_event_set_callback(FHalNfcCallback callback, void* context);

void f_hal_nfc_set_event(FHalNfcEventInternalType event);

void f_hal_nfc_init_gpio_isr();

void f_hal_nfc_deinit_gpio_isr();

void f_hal_nfc_timers_init();

void f_hal_nfc_timers_deinit();

uint32_t f_hal_nfc_get_irq(FuriHalSpiBusHandle* handle);

bool f_hal_nfc_event_wait_for_specific_irq(
    FuriHalSpiBusHandle* handle,
    uint32_t mask,
    uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif
