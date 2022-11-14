#include "rfal_event.h"

#define TAG "RfalEvent"

FuriEventFlag* rfal_event_flag = NULL;

FuriThreadId rfal_thread = 0;

void rfal_event_init(FuriThreadId thread) {
    // rfal_event_flag = furi_event_flag_alloc();
    rfal_thread = thread;
}

void rfal_event_interrupt_received() {
    // furi_assert(rfal_event_flag);

    // furi_event_flag_set(rfal_event_flag, RfalEventInterruptReceived);
    furi_thread_flags_set(rfal_thread, RfalEventInterruptReceived);
}

void rfal_event_user_abort() {
    // furi_assert(rfal_event_flag);

    // furi_event_flag_set(rfal_event_flag, RfalEventUserAbort);
    furi_thread_flags_set(rfal_thread, RfalEventUserAbort);
}

RfalEvent rfal_event_wait(uint32_t timeout) {
    RfalEvent event = furi_thread_flags_wait(
        RfalEventInterruptReceived | RfalEventUserAbort, FuriFlagWaitAny, timeout);

    return event;
}
