#include "nfca_poller.h"
#include "nfca.h"

#include <lib/nfc/nfc.h>
#include <furi.h>

#define TAG "NFCA"

#define NFCA_POLLER_MAX_TX_BUFFER_SIZE (512U)

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
    NfcaPollerColResInProgress,
    NfcaPollerColResFailed,
    NfcaPollerActivated,
} NfcaPollerState;

struct NfcaPoller {
    Nfc* nfc;
    NfcaPollerState state;
    NfcaPollerColRes col_res;
    NfcaData data;
};

static NfcaError nfca_poller_process_error(NfcError error) {
    NfcaError ret = NfcaErrorNone;
    if(error == NfcErrorNone) {
        ret = NfcaErrorNone;
    } else if(error == NfcErrorTimeout) {
        ret = NfcaErrorTimeout;
    } else {
        ret = NfcaErrorNotPresent;
    }
    return ret;
}

static NfcaError nfca_poller_standart_frame_exchange(
    NfcaPoller* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(rx_data);
    furi_assert(rx_bits);
    furi_assert(tx_bits >= 8);

    uint16_t tx_bytes = tx_bits / 8;
    furi_assert(tx_bytes <= NFCA_POLLER_MAX_TX_BUFFER_SIZE - 2);
    uint16_t rx_buff_bits = 0;
    uint8_t rx_buff[NFCA_POLLER_MAX_TX_BUFFER_SIZE] = {};
    uint8_t tx_buff[NFCA_POLLER_MAX_TX_BUFFER_SIZE] = {};

    memcpy(tx_buff, tx_data, tx_bytes);
    nfca_append_crc(tx_buff, tx_bytes);
    NfcaError ret = NfcaErrorNone;

    do {
        NfcError error = nfc_trx(
            instance->nfc, tx_buff, tx_bits + 16, rx_buff, sizeof(rx_buff), &rx_buff_bits, fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }
        if(rx_buff_bits < 8) {
            rx_data[0] = rx_buff[0];
            *rx_bits = rx_buff_bits;
            ret = NfcaErrorWrongCrc;
            break;
        } else if(rx_buff_bits < 3 * 8) {
            uint16_t rx_bytes = rx_buff_bits / 8;
            memcpy(rx_data, rx_buff, rx_bytes);
            *rx_bits = rx_buff_bits;
            ret = NfcaErrorWrongCrc;
            break;
        } else {
            uint16_t rx_bytes = rx_buff_bits / 8;
            if(rx_bytes - 2 > rx_data_size) {
                ret = NfcaErrorBufferOverflow;
                break;
            }
            if(!nfca_check_crc(rx_buff, rx_bytes)) {
                ret = NfcaErrorWrongCrc;
                break;
            }
            memcpy(rx_data, rx_buff, rx_bytes - 2);
            *rx_bits = (rx_bytes - 2) * 8;
        }
    } while(false);

    return ret;
}

static NfcaError nfca_poller_prepare_trx(NfcaPoller* instance) {
    NfcaError ret = NfcaErrorNone;

    if(instance->state == NfcaPollerStateIdle) {
        ret = nfca_poller_activate(instance, NULL);
    }

    return ret;
}

NfcaPoller* nfca_poller_alloc() {
    NfcaPoller* instance = malloc(sizeof(NfcaPoller));
    instance->nfc = nfc_alloc();
    nfc_config(instance->nfc, NfcModeNfcaPoller);
    nfc_set_guard_time_us(instance->nfc, NFCA_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, NFCA_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, NFCA_POLL_POLL_MIN_US);

    return instance;
}

void nfca_poller_free(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    nfc_free(instance->nfc);
    free(instance);
}

NfcaError nfca_poller_check_presence(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    uint16_t rx_bits = 0;
    NfcError error = NfcErrorNone;
    NfcaError ret = 0;
    do {
        error = nfc_iso13444a_short_frame(
            instance->nfc,
            NfcIso14443aShortFrameSensReq,
            (uint8_t*)&instance->col_res.sens_resp,
            sizeof(instance->col_res.sens_resp),
            &rx_bits,
            NFCA_FDT_LISTEN_FC);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
        }
        if(rx_bits != 8 * sizeof(instance->col_res.sens_resp)) {
            ret = NfcaErrorCommunication;
            break;
        }
    } while(false);

    return ret;
}

NfcaError nfca_poller_halt(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    // Halt
    // Set Idle state
    instance->state = NfcaPollerStateIdle;
    return NfcaErrorNone;
}

NfcaError nfca_poller_activate(NfcaPoller* instance, NfcaData* nfca_data) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    // Reset Nfca poller state
    memset(&instance->col_res, 0, sizeof(instance->col_res));
    memset(&instance->data, 0, sizeof(instance->data));

    // Halt if necessary
    if(instance->state != NfcaPollerStateIdle) {
        nfca_poller_halt(instance);
        instance->state = NfcaPollerStateIdle;
    }

    NfcError error = NfcErrorNone;
    NfcaError ret = NfcaErrorNone;
    uint16_t rx_bits = 0;
    uint16_t tx_bits = 0;

    bool activated = false;
    do {
        error = nfc_iso13444a_short_frame(
            instance->nfc,
            NfcIso14443aShortFrameSensReq,
            (uint8_t*)&instance->col_res.sens_resp,
            sizeof(instance->col_res.sens_resp),
            &rx_bits,
            NFCA_FDT_LISTEN_FC);
        if(error != NfcErrorNone) {
            ret = NfcaErrorNotPresent;
            break;
        }
        if(rx_bits != 8 * sizeof(instance->col_res.sens_resp)) {
            FURI_LOG_W(TAG, "Wrong response: %d", rx_bits);
            ret = NfcaErrorCommunication;
            break;
        }
        memcpy(
            instance->data.atqa, &instance->col_res.sens_resp, sizeof(instance->col_res.sel_resp));

        instance->state = NfcaPollerColResInProgress;
        instance->col_res.cascade_level = 0;
        instance->col_res.state = NfcaPollerColResStateStateNewCascade;

        while(instance->state == NfcaPollerColResInProgress) {
            if(instance->col_res.state == NfcaPollerColResStateStateNewCascade) {
                instance->col_res.sdd_req.sel_cmd =
                    NFCA_POLLER_SEL_CMD(instance->col_res.cascade_level);
                instance->col_res.sdd_req.sel_par = NFCA_POLLER_SEL_PAR(2, 0);
                tx_bits = 16;
                error = nfc_iso13444a_sdd_frame(
                    instance->nfc,
                    (uint8_t*)&instance->col_res.sdd_req,
                    tx_bits,
                    (uint8_t*)&instance->col_res.sdd_resp,
                    sizeof(instance->col_res.sdd_resp),
                    &rx_bits,
                    NFCA_FDT_LISTEN_FC);
                if(error != NfcErrorNone) {
                    FURI_LOG_E(TAG, "Sdd request failed: %d", error);
                    instance->state = NfcaPollerColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                if(rx_bits != 5 * 8) {
                    FURI_LOG_E(TAG, "Sdd response wrong length: %d bits", rx_bits);
                    instance->state = NfcaPollerColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                // TODO BCC check here
                instance->col_res.state = NfcaPollerColResStateStateSelectCascade;
            } else if(instance->col_res.state == NfcaPollerColResStateStateSelectCascade) {
                instance->col_res.sel_req.sel_cmd =
                    NFCA_POLLER_SEL_CMD(instance->col_res.cascade_level);
                instance->col_res.sel_req.sel_par = NFCA_POLLER_SEL_PAR(7, 0);
                memcpy(
                    instance->col_res.sel_req.nfcid,
                    instance->col_res.sdd_resp.nfcid,
                    sizeof(instance->col_res.sdd_resp.nfcid));
                instance->col_res.sel_req.bcc = instance->col_res.sdd_resp.bss;
                // Todo remove after Nfc handles timings
                furi_delay_ms(10);
                ret = nfca_poller_send_standart_frame(
                    instance,
                    (uint8_t*)&instance->col_res.sel_req,
                    8 * sizeof(instance->col_res.sel_req),
                    (uint8_t*)&instance->col_res.sel_resp,
                    sizeof(NfcaSelResp),
                    &rx_bits,
                    NFCA_FDT_LISTEN_FC);
                if(ret != NfcaErrorNone) {
                    FURI_LOG_E(TAG, "Sel request failed: %d", error);
                    instance->state = NfcaPollerColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                if(rx_bits != 8 * sizeof(instance->col_res.sel_resp)) {
                    FURI_LOG_E(TAG, "Sel response wrong length: %d bits", rx_bits);
                    instance->state = NfcaPollerColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                FURI_LOG_T(TAG, "Sel resp: %02X", instance->col_res.sel_resp.sak);
                if(instance->col_res.sel_req.nfcid[0] == NFCA_POLLER_SDD_CL) {
                    // Copy part of UID
                    memcpy(
                        &instance->data.uid[instance->data.uid_len],
                        &instance->col_res.sel_req.nfcid[1],
                        3);
                    instance->data.uid_len += 3;
                    instance->col_res.cascade_level++;
                    instance->col_res.state = NfcaPollerColResStateStateNewCascade;
                } else {
                    FURI_LOG_T(TAG, "Col resolution complete");
                    instance->data.sak = instance->col_res.sel_resp.sak;
                    memcpy(
                        &instance->data.uid[instance->data.uid_len],
                        &instance->col_res.sel_req.nfcid[0],
                        4);
                    instance->data.uid_len += 4;
                    instance->col_res.state = NfcaPollerColResStateStateSuccess;
                    instance->state = NfcaPollerActivated;
                }
            }
        }
        activated = (instance->state == NfcaPollerActivated);
    } while(false);

    if(activated && nfca_data) {
        *nfca_data = instance->data;
    }

    return ret;
}

NfcaError nfca_poller_txrx(
    NfcaPoller* instance,
    uint8_t* tx_buff,
    uint16_t tx_bits,
    uint8_t* rx_buff,
    uint16_t rx_buff_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buff);
    furi_assert(rx_buff);
    furi_assert(rx_bits);

    NfcaError ret = NfcaErrorNone;
    NfcError error = NfcErrorNone;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        error = nfc_trx(instance->nfc, tx_buff, tx_bits, rx_buff, rx_buff_size, rx_bits, fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }
    } while(false);

    return ret;
}

NfcaError nfca_poller_send_standart_frame(
    NfcaPoller* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(rx_data);
    furi_assert(rx_bits);
    furi_assert(tx_bits >= 8);

    NfcaError ret = NfcaErrorNone;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        ret = nfca_poller_standart_frame_exchange(
            instance, tx_data, tx_bits, rx_data, rx_data_size, rx_bits, fwt);
        if(ret != NfcaErrorNone) break;
    } while(false);

    return ret;
}
