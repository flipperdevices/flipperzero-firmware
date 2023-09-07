#include <f_hal_nfc_i.h>

FHalNfcEventInternal* f_hal_nfc_event = NULL;

void f_hal_nfc_event_init() {
    f_hal_nfc_event = malloc(sizeof(FHalNfcEventInternal));
}

FHalNfcError f_hal_nfc_event_start() {
    furi_assert(f_hal_nfc_event);

    f_hal_nfc_event->thread = furi_thread_get_current_id();
    furi_thread_flags_clear(F_HAL_NFC_EVENT_INTERNAL_ALL);

    return FHalNfcErrorNone;
}

void f_hal_nfc_event_set(FHalNfcEventInternalType event) {
    furi_assert(f_hal_nfc_event);
    furi_assert(f_hal_nfc_event->thread);

    furi_thread_flags_set(f_hal_nfc_event->thread, event);
}

FHalNfcError f_hal_nfc_abort() {
    f_hal_nfc_event_set(FHalNfcEventInternalTypeAbort);
    return FHalNfcErrorNone;
}

FHalNfcEvent f_hal_nfc_wait_event_common(uint32_t timeout_ms) {
    furi_assert(f_hal_nfc_event);
    furi_assert(f_hal_nfc_event->thread);

    FHalNfcEvent event = 0;
    uint32_t event_timeout = timeout_ms == F_HAL_NFC_EVENT_WAIT_FOREVER ? FuriWaitForever :
                                                                          timeout_ms;
    uint32_t event_flag =
        furi_thread_flags_wait(F_HAL_NFC_EVENT_INTERNAL_ALL, FuriFlagWaitAny, event_timeout);
    if(event_flag != FuriFlagErrorTimeout) {
        if(event_flag & FHalNfcEventInternalTypeIrq) {
            furi_thread_flags_clear(FHalNfcEventInternalTypeIrq);
            FuriHalSpiBusHandle* handle = &furi_hal_spi_bus_handle_nfc;
            uint32_t irq = f_hal_nfc_get_irq(handle);
            if(irq & ST25R3916_IRQ_MASK_OSC) {
                event |= FHalNfcEventOscOn;
            }
            if(irq & ST25R3916_IRQ_MASK_TXE) {
                event |= FHalNfcEventTxEnd;
            }
            if(irq & ST25R3916_IRQ_MASK_RXS) {
                event |= FHalNfcEventRxStart;
            }
            if(irq & ST25R3916_IRQ_MASK_RXE) {
                event |= FHalNfcEventRxEnd;
            }
            if(irq & ST25R3916_IRQ_MASK_COL) {
                event |= FHalNfcEventCollision;
            }
            if(irq & ST25R3916_IRQ_MASK_EON) {
                event |= FHalNfcEventFieldOn;
            }
            if(irq & ST25R3916_IRQ_MASK_EOF) {
                event |= FHalNfcEventFieldOff;
            }
            if(irq & ST25R3916_IRQ_MASK_WU_A) {
                event |= FHalNfcEventListenerActive;
            }
            if(irq & ST25R3916_IRQ_MASK_WU_A_X) {
                event |= FHalNfcEventListenerActive;
            }
        }
        if(event_flag & FHalNfcEventInternalTypeTimerFwtExpired) {
            event |= FHalNfcEventTimerFwtExpired;
            furi_thread_flags_clear(FHalNfcEventInternalTypeTimerFwtExpired);
        }
        if(event_flag & FHalNfcEventInternalTypeTimerBlockTxExpired) {
            event |= FHalNfcEventTimerBlockTxExpired;
            furi_thread_flags_clear(FHalNfcEventInternalTypeTimerBlockTxExpired);
        }
        if(event_flag & FHalNfcEventInternalTypeAbort) {
            event |= FHalNfcEventAbortRequest;
            furi_thread_flags_clear(FHalNfcEventInternalTypeAbort);
        }
    } else {
        event = FHalNfcEventTimeout;
    }

    return event;
}

bool f_hal_nfc_event_wait_for_specific_irq(
    FuriHalSpiBusHandle* handle,
    uint32_t mask,
    uint32_t timeout_ms) {
    furi_assert(f_hal_nfc_event);
    furi_assert(f_hal_nfc_event->thread);

    bool irq_received = false;
    uint32_t event_flag =
        furi_thread_flags_wait(FHalNfcEventInternalTypeIrq, FuriFlagWaitAny, timeout_ms);
    if(event_flag == FHalNfcEventInternalTypeIrq) {
        uint32_t irq = f_hal_nfc_get_irq(handle);
        irq_received = ((irq & mask) == mask);
        furi_thread_flags_clear(FHalNfcEventInternalTypeIrq);
    }

    return irq_received;
}
