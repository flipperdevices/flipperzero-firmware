#pragma once

typedef enum {
    // Reserve first 100 events for button types and indexes, starting from 0
    NfcMagicCustomEventReserved = 100,

    NfcMagicCustomEventViewExit,

    NfcMagicCustomEventCardDetected,
    NfcMagicCustomEventCardLost,
    NfcMagicCustomEventWorkerSuccess,
    NfcMagicCustomEventWorkerFail,

} NfcMagicCustomEvent;
