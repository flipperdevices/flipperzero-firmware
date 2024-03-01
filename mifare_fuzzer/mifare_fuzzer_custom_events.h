#pragma once

typedef enum MifareFuzzerEvent {
    MifareFuzzerEventClassic1k = 1,
    MifareFuzzerEventClassic4k,
    MifareFuzzerEventUltralight,
    MifareFuzzerEventFile,
    MifareFuzzerEventTestValueAttack,
    MifareFuzzerEventRandomValuesAttack,
    MifareFuzzerEventLoadUIDsFromFileAttack,
    MifareFuzzerEventStartAttack,
    MifareFuzzerEventStopAttack,
    MifareFuzzerEventIncrementTicks,
    MifareFuzzerEventDecrementTicks,
} MifareFuzzerEvent;
