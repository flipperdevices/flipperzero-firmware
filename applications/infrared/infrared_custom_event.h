#pragma once

enum InfraredCustomEvent {
    // Reserve first 100 events for button types and indexes, starting from 0
    InfraredCustomEventReserved = 100,
    InfraredCustomEventMenuSelected,
    InfraredCustomEventTransmitStarted,
    InfraredCustomEventTransmitStopped,
};
