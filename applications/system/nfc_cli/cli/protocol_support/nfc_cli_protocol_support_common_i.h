#include "nfc_cli_protocol_support_common.h"

#include <nfc/nfc.h>
#include <furi/core/message_queue.h>
#include <toolbox/bit_buffer.h>
#include <m-array.h>

#define NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE (255)

typedef struct {
    bool activation_required;
    bool append_crc;
    uint32_t timeout;
} NfcCliPollCmdParameters;

ARRAY_DEF(NfcCliPollCmdDataArray, NfcCliPollCmdData, M_POD_OPLIST);

typedef struct {
    Nfc* nfc;
    NfcPoller* poller;
    FuriMessageQueue* user_queue;
    FuriMessageQueue* worker_queue;
    uint8_t buffer[NFC_CLI_PROTOCOL_SUPPORT_MAX_BUFFER_SIZE];
    BitBuffer* rx_data;
    FuriString* formatted_data;
    NfcCliProtocolSupportCommonCallback callback;
} NfcCliPollerContext;

typedef enum {
    NfcCliWorkerMessageTypeAbort,
    NfcCliWorkerMessageTypeActivate,
    NfcCliWorkerMessageTypeFrameExchange,
    NfcCliWorkerMessageTypeReset,
} NfcCliWorkerMessageType;

typedef union {
    const NfcCliPollCmdData* tx_data;
} NfcCliWorkerMessageData;

typedef struct {
    NfcCliWorkerMessageType type;
    NfcCliWorkerMessageData data;
} NfcCliWorkerMessage;

typedef enum {
    NfcCliUserMessageTypeStatus,
    NfcCliUserMessageTypeRx,
} NfcCliUserMessageType;

typedef struct {
    NfcCliPollerError error;
    union {
        BitBuffer* rx_data;
        FuriString* formatted_data;
    };
} NfcCliUserMessageData;

typedef struct {
    NfcCliUserMessageType type;
    NfcCliUserMessageData data;
} NfcCliUserMessage;

typedef void (*NfcCliStartPollerHandler)(FuriString* cmd, NfcCliPollerContext* context);

typedef struct {
    const char* cmd;
    NfcCliStartPollerHandler handler;
} NfcCliStartPollerCommand;
