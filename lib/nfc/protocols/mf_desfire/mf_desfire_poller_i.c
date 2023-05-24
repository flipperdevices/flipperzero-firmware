#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

MfDesfireError mf_desfire_process_error(NfcaError error) {
    MfDesfireError ret = MfDesfireErrorNone;

    switch(error) {
    case NfcaErrorNone:
        ret = MfDesfireErrorNone;
        break;
    case NfcaErrorNotPresent:
        ret = MfDesfireErrorNotPresent;
        break;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        ret = MfDesfireErrorProtocol;
        break;
    case NfcaErrorTimeout:
        ret = MfDesfireErrorTimeout;
        break;
    default:
        ret = MfDesfireErrorProtocol;
        break;
    }

    return ret;
}

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data) {
    NfcPollerBuffer* buf = instance->buffer;
    buf->tx_data[0] = MF_DESFIRE_CMD_GET_VERSION;
    buf->tx_bits = 8;

    MfDesfireError ret = MfDesfireErrorNone;
    NfcaError error = NfcaErrorNone;
    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buf->tx_data,
            buf->tx_bits,
            buf->rx_data,
            buf->rx_data_size,
            &buf->rx_bits,
            MF_DESFIRE_POLLER_STANDARD_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_desfire_process_error(error);
            break;
        }
        if(buf->rx_bits != sizeof(MfDesfireVersion) * 8) {
            FURI_LOG_E(TAG, "Not enough bits: %u", buf->rx_bits);
            ret = MfDesfireErrorProtocol;
            break;
        }
        memcpy(data, buf->rx_data, sizeof(MfDesfireVersion));
    } while(false);

    return ret;
}
