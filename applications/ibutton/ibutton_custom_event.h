#pragma once

enum iButtonCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    iButtonCustomEventReserved = 100,

    iButtonCustomEventTick,
    iButtonCustomEventBack,
    iButtonCustomEventMenuSelected,
    iButtonCustomEventDialogResult,
    iButtonCustomEventTextEditResult,
    iButtonCustomEventByteEditResult,
    iButtonCustomEventWidgetButtonResult,
    iButtonCustomEventWorkerEmulated,
    iButtonCustomEventWorkerRead,
    iButtonCustomEventWorkerWrite,
};
