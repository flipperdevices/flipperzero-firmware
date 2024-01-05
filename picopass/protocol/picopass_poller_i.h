#pragma once

#include "picopass_poller.h"
#include "picopass_protocol.h"

#include <nfc/helpers/iso13239_crc.h>

#define PICOPASS_POLLER_BUFFER_SIZE (255)
#define PICOPASS_CRC_SIZE (2)

typedef enum {
    PicopassPollerSessionStateIdle,
    PicopassPollerSessionStateActive,
    PicopassPollerSessionStateStopRequest,
} PicopassPollerSessionState;

typedef enum {
    PicopassPollerStateRequestMode,
    PicopassPollerStateDetect,
    PicopassPollerStateSelect,
    PicopassPollerStatePreAuth,
    PicopassPollerStateCheckSecurity,
    PicopassPollerStateNrMacAuth,
    PicopassPollerStateAuth,
    PicopassPollerStateReadBlock,
    PicopassPollerStateWriteBlock,
    PicopassPollerStateWriteKey,
    PicopassPollerStateParseCredential,
    PicopassPollerStateParseWiegand,
    PicopassPollerStateSuccess,
    PicopassPollerStateFail,
    PicopassPollerStateAuthFail,

    PicopassPollerStateNum,
} PicopassPollerState;

struct PicopassPoller {
    Nfc* nfc;
    PicopassPollerSessionState session_state;
    PicopassPollerState state;
    PicopassPollerMode mode;

    PicopassColResSerialNum col_res_serial_num;
    PicopassSerialNum serial_num;
    PicopassMac mac;
    uint8_t div_key[8];
    uint8_t current_block;
    uint8_t app_limit;
    bool secured;

    PicopassDeviceData* data;

    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    BitBuffer* tmp_buffer;

    PicopassPollerEvent event;
    PicopassPollerEventData event_data;
    PicopassPollerCallback callback;
    void* context;
};

PicopassError picopass_poller_actall(PicopassPoller* instance);

PicopassError
    picopass_poller_identify(PicopassPoller* instance, PicopassColResSerialNum* col_res_serial_num);

PicopassError picopass_poller_select(
    PicopassPoller* instance,
    PicopassColResSerialNum* col_res_serial_num,
    PicopassSerialNum* serial_num);

PicopassError
    picopass_poller_read_block(PicopassPoller* instance, uint8_t block_num, PicopassBlock* block);

PicopassError
    picopass_poller_read_check(PicopassPoller* instance, PicopassReadCheckResp* read_check_resp);

PicopassError picopass_poller_check(
    PicopassPoller* instance,
    uint8_t* nr,
    PicopassMac* mac,
    PicopassCheckResp* check_resp);

PicopassError picopass_poller_write_block(
    PicopassPoller* instance,
    uint8_t block_num,
    const PicopassBlock* block,
    const PicopassMac* mac);
