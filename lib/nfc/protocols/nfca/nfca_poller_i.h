#pragma once

#include "nfca_poller.h"

#include <lib/nfc/helpers/bit_buffer.h>

#ifdef __cplusplus
extern "C" {
#endif

#define NFCA_POLLER_MAX_BUFFER_SIZE (512U)

#define NFCA_POLLER_SEL_CMD(cascade_lvl) (0x93 + 2 * (cascade_lvl))
#define NFCA_POLLER_SEL_PAR(bytes, bits) (((bytes) << 4 & 0xf0U) | ((bits)&0x0fU))
#define NFCA_POLLER_SDD_CL (0x88U)

typedef enum {
    NfcaPollerColResStateStateIdle,
    NfcaPollerColResStateStateNewCascade,
    NfcaPollerColResStateStateSelectCascade,
    NfcaPollerColResStateStateSuccess,
    NfcaPollerColResStateStateFail,
} NfcaPollerColResState;

typedef struct {
    NfcaPollerColResState state;
    NfcaSensResp sens_resp;
    NfcaSddReq sdd_req;
    NfcaSddResp sdd_resp;
    NfcaSelReq sel_req;
    NfcaSelResp sel_resp;
    uint8_t cascade_level;
} NfcaPollerColRes;

typedef enum {
    NfcaPollerStateIdle,
    NfcaPollerStateColResInProgress,
    NfcaPollerStateColResFailed,
    NfcaPollerStateActivated,
} NfcaPollerState;

typedef enum {
    NfcaPollerSessionStateIdle,
    NfcaPollerSessionStateActive,
    NfcaPollerSessionStateStopRequest,
} NfcaPollerSessionState;

typedef enum {
    NfcaPollerConfigStateIdle,
    NfcaPollerConfigStateDone,
} NfcaPollerConfigState;

struct NfcaPoller {
    Nfc* nfc;
    NfcaPollerState state;
    NfcaPollerSessionState session_state;
    NfcaPollerConfigState config_state;
    NfcaPollerColRes col_res;
    NfcaData* data;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    NfcaPollerEventCallback callback;
    void* context;
};

NfcaError nfca_poller_config(NfcaPoller* instance);

NfcaError nfca_poller_reset(NfcaPoller* instance);

NfcaError nfca_poller_check_presence(NfcaPoller* instance);

NfcaError nfca_poller_async_activate(NfcaPoller* instance, NfcaData* nfca_data);

NfcaError nfca_poller_halt(NfcaPoller* instance);

NfcaError nfca_poller_txrx_custom_parity(
    NfcaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

NfcaError nfca_poller_txrx(
    NfcaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

NfcaError nfca_poller_send_standart_frame(
    NfcaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt);

#ifdef __cplusplus
}
#endif
