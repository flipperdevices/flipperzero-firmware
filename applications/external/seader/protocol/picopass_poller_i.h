#pragma once

#include "rfal_picopass.h"
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
    PicopassPollerStateSuccess,
    PicopassPollerStateFail,

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
