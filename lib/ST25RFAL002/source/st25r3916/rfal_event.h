#pragma once

#include <stdint.h>

typedef enum {
    RfalEventInterruptReceived = 1 << 0,
    RflaEventStateChaged = 1 << 1,
    RfalEventUserAbort = 1 << 2,
} RfalEvent;

void rfal_event_init();

void rfal_event_interrupt_received();

void rfal_event_user_abort();

void rfal_event_state_changed();

RfalEvent rfal_event_wait(uint32_t timeout);
