#pragma once

#include "st25tb_poller.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    St25tbPollerStateSelect,
    St25tbPollerStateRequestMode,
    St25tbPollerStateRead,
    St25tbPollerStateWrite,
    St25tbPollerStateSuccess,
    St25tbPollerStateFailure,

    St25tbPollerStateNum,
} St25tbPollerState;

typedef struct {
    NfcEventType event;
    St25tbPollerState state;
    St25tbError error;
    NfcCommand command;
} FURI_PACKED St25tbPollerHistoryData;

#ifdef __cplusplus
}
#endif
