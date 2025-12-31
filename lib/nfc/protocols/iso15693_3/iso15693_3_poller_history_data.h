#pragma once

#include "iso15693_3.h"
#include <nfc.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Enumeration of possible Iso15693_3 poller event types.
 */
typedef enum {
    Iso15693_3PollerEventTypeError, /**< An error occured during activation procedure. */
    Iso15693_3PollerEventTypeReady, /**< The card was activated by the poller. */
} Iso15693_3PollerEventType;

typedef enum {
    Iso15693_3PollerStateIdle,
    Iso15693_3PollerStateColResInProgress,
    Iso15693_3PollerStateColResFailed,
    Iso15693_3PollerStateActivated,
} Iso15693_3PollerState;

typedef struct {
    NfcEventType event;
    Iso15693_3PollerState state;
    Iso15693_3Error error;
    NfcCommand command;
} FURI_PACKED Iso15693_3PollerHistoryData;

#ifdef __cplusplus
}
#endif
