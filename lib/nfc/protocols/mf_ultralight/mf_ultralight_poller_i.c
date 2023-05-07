#include "mf_ultralight_poller_i.h"

#include <furi.h>

#define TAG "MfUltralightPoller"

MfUltralightError mf_ultralight_process_error(NfcaError error) {
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

MfUltralightError
    mf_ultralight_poller_async_auth(MfUltralightPoller* instance, MfUltralightAuthPassword* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_AUTH;
    // fill password in lsb
    nfc_util_num2bytes(data->pass, MF_ULTRALIGHT_AUTH_PASSWORD_SIZE, &buff->tx_data[1]);
    buff->tx_bits = (MF_ULTRALIGHT_AUTH_PASSWORD_SIZE + 1) * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != (MF_ULTRALIGHT_AUTH_PACK_SIZE * 8)) {
            ret = MfUltralightErrorAuth;
            break;
        }
        if(instance->pages_total != 0) {
            memcpy(
                &instance->data->page[instance->pages_total - 2],
                data,
                MF_ULTRALIGHT_AUTH_PASSWORD_SIZE);
            memcpy(
                &instance->data->page[instance->pages_total - 1],
                buff->rx_data,
                MF_ULTRALIGHT_AUTH_PACK_SIZE);
        }
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_page(
    MfUltralightPoller* instance,
    uint8_t start_page,
    MfUltralightPageReadCommandData* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_READ_PAGE;
    buff->tx_data[1] = start_page;
    buff->tx_bits = 16;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightPageReadCommandData) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, sizeof(MfUltralightPageReadCommandData));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_write_page(
    MfUltralightPoller* instance,
    uint8_t page,
    MfUltralightPage* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_WRITE_PAGE;
    buff->tx_data[1] = page;
    memcpy(&buff->tx_data[2], data, MF_ULTRALIGHT_PAGE_SIZE);
    buff->tx_bits = (2 + MF_ULTRALIGHT_PAGE_SIZE) * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        FURI_LOG_I(TAG, "Rx bits: %d", buff->rx_bits);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != 4) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(&instance->data->page[page], buff->rx_data, MF_ULTRALIGHT_PAGE_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_version(
    MfUltralightPoller* instance,
    MfUltralightVersion* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_GET_VERSION;
    buff->tx_bits = 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightVersion) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(&data, buff->rx_data, sizeof(MfUltralightVersion));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_signature(
    MfUltralightPoller* instance,
    MfUltralightSignature* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGTH_CMD_READ_SIG;
    buff->rx_data[1] = 0x00;
    buff->tx_bits = 16;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightSignature) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, sizeof(MfUltralightSignature));
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_counter(
    MfUltralightPoller* instance,
    uint8_t counter_num,
    MfUltralightCounter* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_READ_CNT;
    buff->tx_data[1] = counter_num;
    buff->tx_bits = 2 * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != 3 * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, MF_ULTRALIGHT_COUNTER_SIZE);
    } while(false);

    return ret;
}

MfUltralightError mf_ultralight_poller_async_read_tearing_flag(
    MfUltralightPoller* instance,
    uint8_t tearing_falg_num,
    MfUltralightTearingFlag* data) {
    NfcPollerBuffer* buff = instance->buffer;
    buff->tx_data[0] = MF_ULTRALIGHT_CMD_CHECK_TEARING;
    buff->tx_data[1] = tearing_falg_num;
    buff->tx_bits = 2 * 8;

    MfUltralightError ret = MfUltralightErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_ULTRALIGHT_POLLER_STANDART_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_ultralight_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfUltralightTearingFlag) * 8) {
            ret = MfUltralightErrorProtocol;
            break;
        }
        memcpy(data, buff->rx_data, MF_ULTRALIGHT_TEARING_FLAG_SIZE);
    } while(false);

    return ret;
}
