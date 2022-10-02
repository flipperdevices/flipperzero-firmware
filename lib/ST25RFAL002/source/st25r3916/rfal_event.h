#pragma once

#include <stdint.h>

typedef enum {
    RfalEventInterruptReceived = 1 << 0,
    RfalEventUserAbort = 1 << 1,
} RfalEvent;

void rfal_event_init();

void rfal_event_interrupt_received();

void rfal_event_user_abort();

RfalEvent rfal_event_wait(uint32_t timeout);
