#pragma once

typedef enum MifareFuzzerEvent {
    MifareFuzzerEventClassic1k = 1,
    MifareFuzzerEventClassic4k,
    MifareFuzzerEventUltralight,
    MifareFuzzerEventTestValueAttack,
    MifareFuzzerEventRandomValuesAttack,
    MifareFuzzerEventLoadUIDsFromFileAttack,
    MifareFuzzerEventStartAttack,
    MifareFuzzerEventStopAttack,
    MifareFuzzerEventIncrementTicks,
    MifareFuzzerEventDecrementTicks,
} MifareFuzzerEvent;
