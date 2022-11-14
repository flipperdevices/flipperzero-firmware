#include "rfal_event.h"
#include <furi/furi.h>
#include <furi/core/log.h>

#define TAG "RfalEvent"

FuriEventFlag* rfal_event_flag = NULL;

void rfal_event_init() {
    rfal_event_flag = furi_event_flag_alloc();
}

void rfal_event_interrupt_received() {
    furi_assert(rfal_event_flag);

    furi_event_flag_set(rfal_event_flag, RfalEventInterruptReceived);
}

void rfal_event_user_abort() {
    furi_assert(rfal_event_flag);

    furi_event_flag_set(rfal_event_flag, RfalEventUserAbort);
}

void rfal_event_state_changed() {
    furi_assert(rfal_event_flag);

    furi_event_flag_set(rfal_event_flag, RflaEventStateChaged);
}

RfalEvent rfal_event_wait(uint32_t timeout) {
    RfalEvent event = furi_event_flag_wait(
        rfal_event_flag,
        RfalEventInterruptReceived | RfalEventUserAbort,
        FuriFlagWaitAny,
        timeout);

    return event;
}
