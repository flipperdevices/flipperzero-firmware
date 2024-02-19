#include "nfc_cli_protocol_support_common.h"

#include <furi/core/message_queue.h>
#include <toolbox/bit_buffer.h>
#include <m-array.h>

typedef struct {
    bool activation_required;
    bool append_crc;
    uint32_t timeout;
} NfcCliPollCmdParameters;

typedef struct {
    BitBuffer* tx_data;
    bool append_crc;
    uint32_t timeout;
} NfcCliPollCmdData;

ARRAY_DEF(NfcCliPollCmdDataArray, NfcCliPollCmdData, M_POD_OPLIST);

typedef struct {
    NfcPoller* poller;
    FuriMessageQueue* cli_queue;
    FuriMessageQueue* poller_queue;
    BitBuffer* rx_data;
} NfcCliPollerContext;

typedef enum {
    NfcCliWorkerMessageTypeAbort,
    NfcCliWorkerMessageTypeActivate,
    NfcCliWorkerMessageTypeTx,
    NfcCliWorkerMessageTypeReset,
} NfcCliWorkerMessageType;

typedef union {
    NfcCliPollCmdData* tx_data;
} NfcCliWorkerMessageData;

typedef struct {
    NfcCliWorkerMessageType type;
    NfcCliWorkerMessageData data;
} NfcPollerMessage;

typedef enum {
    NfcCliUserMessageTypeStatus,
    NfcCliUserMessageTypeRx,
} NfcCliUserMessageType;

typedef enum {
    NfcCliPollerErrorNone,
    NfcCliPollerErrorTimeout,
    NfcCliPollerErrorInternal,
} NfcCliPollerError;

typedef struct {
    BitBuffer* rx_data;
} NfcCliUserMessageRxData;

typedef union {
    NfcCliPollerError error;
    NfcCliUserMessageRxData data;
} NfcCliUserMessageData;

typedef struct {
    NfcCliUserMessageType type;
    NfcCliUserMessageData data;
} NfcCliUserMessage;
