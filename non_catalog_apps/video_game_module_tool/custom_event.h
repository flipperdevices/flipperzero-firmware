#pragma once

typedef enum {
    // Reserve first 100 events for submenu indexes, starting from 0
    CustomEventReserved = 100,

    CustomEventFileConfirmed,
    CustomEventFileRejected,
    CustomEventSuccessDismissed,
    CustomEventRetryRequested,
} CustomEvent;
