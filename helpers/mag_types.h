#pragma once

typedef enum {
    MagReverseStateOn,
    MagReverseStateOff,
} MagReverseState;

typedef enum {
    MagTrackStateAll,
    MagTrackStateOne,
    MagTrackStateTwo,
} MagTrackState;

typedef enum {
    MagTxStateRFID,
    MagTxStateGPIOA6A7,
} MagTxState;
