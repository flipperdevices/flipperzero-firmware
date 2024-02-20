#pragma once

/**
 * @enum SubBruteCustomEvent
 * @brief Enumerates the custom event types used in the SubBrute application.
 *
 * This enumeration defines custom event types used in the SubBrute application.
 * The first 100 event types are reserved for button types and indexes, starting
 * from 0. The remaining event types are specific to the application and are used
 * to handle various events.
 *
 * @note These event types are used to define custom events in the application's
 *       event handling system. They should be used to specify the type of custom
 *       events and differentiate them from other events.
 *
 * @var SubBruteCustomEventTypeReserved
 * Reserved event type for button types and indexes. Starting from 0.
 *
 * @var SubBruteCustomEventTypeBackPressed
 * Event type for the back button press event.
 *
 * @var SubBruteCustomEventTypeIndexSelected
 * Event type for the index selection event.
 *
 * @var SubBruteCustomEventTypeTransmitStarted
 * Event type for the transmit start event.
 *
 * @var SubBruteCustomEventTypeError
 * Event type for the error event.
 *
 * @var SubBruteCustomEventTypeTransmitFinished
 * Event type for the transmit finish event.
 *
 * @var SubBruteCustomEventTypeTransmitNotStarted
 * Event type for the transmit not started event.
 *
 * @var SubBruteCustomEventTypeTransmitCustom
 * Event type for the custom transmit event.
 *
 * @var SubBruteCustomEventTypeSaveFile
 * Event type for the save file event.
 *
 * @var SubBruteCustomEventTypeExtraSettings
 * Event type for the extra settings event.
 *
 * @var SubBruteCustomEventTypeUpdateView
 * Event type for the update view event.
 *
 * @var SubBruteCustomEventTypeChangeStepUp
 * Event type for the change step up event.
 *
 * @var SubBruteCustomEventTypeChangeStepDown
 * Event type for the change step down event.
 *
 * @var SubBruteCustomEventTypeChangeStepUpMore
 * Event type for the change step up more event.
 *
 * @var SubBruteCustomEventTypeChangeStepDownMore
 * Event type for the change step down more event.
 *
 * @var SubBruteCustomEventTypeMenuSelected
 * Event type for the menu selection event.
 *
 * @var SubBruteCustomEventTypeTextEditDone
 * Event type for the text edit done event.
 *
 * @var SubBruteCustomEventTypePopupClosed
 * Event type for the popup close event.
 *
 * @var SubBruteCustomEventTypeLoadFile
 * Event type for the load file event.
 */
typedef enum {
    // Reserve first 100 events for button types and indexes, starting from 0
    SubBruteCustomEventTypeReserved = 100,

    SubBruteCustomEventTypeBackPressed,
    SubBruteCustomEventTypeIndexSelected,
    SubBruteCustomEventTypeTransmitStarted,
    SubBruteCustomEventTypeError,
    SubBruteCustomEventTypeTransmitFinished,
    SubBruteCustomEventTypeTransmitNotStarted,
    SubBruteCustomEventTypeTransmitCustom,
    SubBruteCustomEventTypeSaveFile,
    SubBruteCustomEventTypeExtraSettings,
    SubBruteCustomEventTypeUpdateView,
    SubBruteCustomEventTypeChangeStepUp,
    SubBruteCustomEventTypeChangeStepDown,
    SubBruteCustomEventTypeChangeStepUpMore,
    SubBruteCustomEventTypeChangeStepDownMore,

    SubBruteCustomEventTypeMenuSelected,
    SubBruteCustomEventTypeTextEditDone,
    SubBruteCustomEventTypePopupClosed,

    SubBruteCustomEventTypeLoadFile,
} SubBruteCustomEvent;