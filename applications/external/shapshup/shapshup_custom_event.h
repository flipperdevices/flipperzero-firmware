#pragma once

/**
 * @brief ShapshupCustomEvent
*  ShapShupCustomEventTypeReserved = 100,
*  ShapShupCustomEventTypeBackPressed,
*  ShapShupCustomEventTypeIndexSelected,
*  ShapShupCustomEventTypeTransmitStarted,
*  ShapShupCustomEventTypeTransmitContinuousStarted,
*  ShapShupCustomEventTypeTransmitFinished,
*  ShapShupCustomEventTypeTransmitNotStarted,
*  ShapShupCustomEventTypeTransmitCustom,
*  ShapShupCustomEventTypeSaveFile,
*  ShapShupCustomEventTypeUpdateView,
*  ShapShupCustomEventTypeChangeStepUp,
*  ShapShupCustomEventTypeChangeStepDown,
*  ShapShupCustomEventTypeChangeStepUpMore,
*  ShapShupCustomEventTypeChangeStepDownMore,
*  ShapShupCustomEventTypeMenuSelected,
*  ShapShupCustomEventTypeTextEditDone,
*  ShapShupCustomEventTypePopupClosed,
*  ShapShupCustomEventTypeLoadFile,
 */
typedef enum {
    // Reserve first 100 events for button types and indexes, starting from 0
    ShapShupCustomEventTypeReserved = 100,

    ShapShupCustomEventTypeBackPressed,
    ShapShupCustomEventTypeIndexSelected,
    ShapShupCustomEventTypeTransmitStarted,
    ShapShupCustomEventTypeTransmitContinuousStarted,
    ShapShupCustomEventTypeTransmitFinished,
    ShapShupCustomEventTypeTransmitNotStarted,
    ShapShupCustomEventTypeTransmitCustom,
    ShapShupCustomEventTypeSaveFile,
    ShapShupCustomEventTypeUpdateView,
    ShapShupCustomEventTypeChangeStepUp,
    ShapShupCustomEventTypeChangeStepDown,
    ShapShupCustomEventTypeChangeStepUpMore,
    ShapShupCustomEventTypeChangeStepDownMore,

    ShapShupCustomEventTypeMenuSelected,
    ShapShupCustomEventTypeTextEditDone,
    ShapShupCustomEventTypePopupClosed,

    ShapShupCustomEventTypeLoadFile,
} ShapshupCustomEvent;
