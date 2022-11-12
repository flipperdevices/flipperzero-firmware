#pragma once

#include <stdint.h>
#include <stddef.h>

typedef enum {
    // Reserve first 100 events for button types and indexes, starting from 0
    shapshupCustomEventTypeReserved = 100,

    shapshupCustomEventTypeBackPressed,
    shapshupCustomEventTypeIndexSelected,
    shapshupCustomEventTypeTransmitStarted,
    shapshupCustomEventTypeTransmitContinuousStarted,
    shapshupCustomEventTypeTransmitFinished,
    shapshupCustomEventTypeTransmitNotStarted,
    shapshupCustomEventTypeTransmitCustom,
    shapshupCustomEventTypeSaveFile,
    shapshupCustomEventTypeUpdateView,
    shapshupCustomEventTypeChangeStepUp,
    shapshupCustomEventTypeChangeStepDown,
    shapshupCustomEventTypeChangeStepUpMore,
    shapshupCustomEventTypeChangeStepDownMore,

    shapshupCustomEventTypeMenuSelected,
    shapshupCustomEventTypeTextEditDone,
    shapshupCustomEventTypePopupClosed,

    shapshupCustomEventTypeLoadFile,
} shapshupCustomEvent;