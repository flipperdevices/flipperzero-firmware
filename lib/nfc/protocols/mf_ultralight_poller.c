#include "mf_ultralight_poller.h"

#include <furi.h>
#include "nfca_poller.h"

#define TAG "MfUltralightPoller"

#define MF_ULTRALIGHT_MAX_BUFF_SIZE (128)

#define MF_ULTRALIGHT_POLLER_STANDART_FWT_FC (12000)

struct MfUltralightPoller {
    NfcaPoller* nfca_poller;
    NfcaData nfca_data;
};

MfUltralightPoller* mf_ultralight_poller_alloc() {
    MfUltralightPoller* instance = malloc(sizeof(MfUltralightPoller));
    instance->nfca_poller = nfca_poller_alloc();

    return instance;
}

void mf_ultralight_poller_free(MfUltralightPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfca_poller);

    nfca_poller_free(instance->nfca_poller);
    free(instance);
}

static MfUltralightError mf_ultralight_process_error(NfcaError error) {
    MfUltralightError ret = MfUltralightErrorNone;

    switch(error) {
    case NfcaErrorNone:
        ret = MfUltralightErrorNone;
        break;
    case NfcaErrorNotPresent:
        ret = MfUltralightErrorNotPresent;
        break;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        ret = MfUltralightErrorProtocol;
        break;
    case NfcaErrorTimeout:
        ret = MfUltralightErrorTimeout;
        break;
    default:
        ret = MfUltralightErrorProtocol;
        break;
    }

    return ret;
}

MfUltralightError mf_ultralight_poller_read_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGHT_CMD_READ_PAGE, page};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != (MF_ULTRALIGHT_PAGE_SIZE * 4) * 8) {
            FURI_LOG_E(
                TAG, "Received %d bits. Expected %d bits", rx_bits, (MF_ULTRALIGHT_PAGE_SIZE)*8);
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, rx_data, MF_ULTRALIGHT_PAGE_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_write_page(
    MfUltralightPoller* instance,
    uint16_t page,
    MfUltralightPage* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[MF_ULTRALIGHT_PAGE_SIZE + 2] = {MF_ULTRALIGHT_CMD_WRITE_PAGE, page};
    memcpy(&tx_data[2], data, MF_ULTRALIGHT_PAGE_SIZE);
    uint16_t tx_bits = (MF_ULTRALIGHT_PAGE_SIZE + 2) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(!(error == NfcaErrorNone || error == NfcaErrorWrongCrc)) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 4) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 4);
            break;
        }
        if(rx_data[0] != MF_ULTRALIGHT_CMD_ACK) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Nack received");
            break;
        }
    } while(false);

    return ret;
}

MfUltralightError
    mf_ultralight_poller_read_version(MfUltralightPoller* instance, MfUltralightVersion* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGHT_CMD_GET_VERSION};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 8 * 8) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 8 * 8);
            break;
        }
        memcpy(data, rx_data, sizeof(MfUltralightVersion));
    } while(false);

    return ret;
}

MfUltralightError
    mf_ultralight_poller_read_signature(MfUltralightPoller* instance, MfUltralightSignature* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGTH_CMD_READ_SIG, 0x00};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 32 * 8) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 32 * 8);
            break;
        }
        memcpy(data, rx_data, sizeof(MfUltralightSignature));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGHT_CMD_READ_CNT, counter_num};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 3 * 8) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 32 * 8);
            break;
        }
        memcpy(data, rx_data, sizeof(MfUltralightCounter));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t flag_num,
    MfUltralightTearingFlag* data) {
    furi_assert(instance);
    furi_assert(data);

    uint8_t tx_data[] = {MF_ULTRALIGHT_CMD_CHECK_TEARING, flag_num};
    uint16_t tx_bits = sizeof(tx_data) * 8;
    uint8_t rx_data[MF_ULTRALIGHT_MAX_BUFF_SIZE] = {};
    uint16_t rx_bits = 0;
    NfcaError error = NfcaErrorNone;
    MfUltralightError ret = MfUltralightErrorNone;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            tx_data,
            tx_bits,
            rx_data,
            sizeof(rx_data),
            &rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(rx_bits != 8) {
            ret = MfUltralightErrorProtocol;
            FURI_LOG_E(TAG, "Received %d bits. Expected %d bits", rx_bits, 8);
            break;
        }
        memcpy(data, rx_data, sizeof(MfUltralightTearingFlag));
    } while(false);

    return ret;
}
