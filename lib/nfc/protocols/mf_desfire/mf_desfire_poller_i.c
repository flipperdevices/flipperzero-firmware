#include "mf_desfire_poller_i.h"

#include <furi.h>

#define TAG "MfDesfirePoller"

MfDesfireError mf_desfire_process_error(Iso14443_4aError error) {

    switch(error) {
    case Iso14443_4aErrorNone:
        return MfDesfireErrorNone;
    case Iso14443_4aErrorNotPresent:
        return MfDesfireErrorNotPresent;
    case Iso14443_4aErrorTimeout:
        return MfDesfireErrorTimeout;
    default:
        return MfDesfireErrorProtocol;
    }
}

MfDesfireError
    mf_desfire_poller_async_read_version(MfDesfirePoller* instance, MfDesfireVersion* data) {
    NfcPollerBuffer* buf = instance->buffer;

    buf->tx_data[0] = MF_DESFIRE_CMD_GET_VERSION;
    buf->tx_bits = 8U;

    buf->rx_data_size = sizeof(MfDesfireVersion);

    MfDesfireError ret = MfDesfireErrorNone;
    Iso14443_4aError error = Iso14443_4aErrorNone;
    do {
        error = iso14443_4a_poller_send_frame(
            instance->iso14443_4a_poller,
            buf->tx_data,
            buf->tx_bits,
            buf->rx_data,
            buf->rx_data_size,
            &buf->rx_bits,
            MF_DESFIRE_POLLER_STANDARD_FWT_FC);

        if(error != Iso14443_4aErrorNone) {
            ret = mf_desfire_process_error(error);
            break;
        }
        if(buf->rx_bits != sizeof(MfDesfireVersion) * 8) {
            ret = MfDesfireErrorProtocol;
            break;
        }
        memcpy(data, buf->rx_data, sizeof(MfDesfireVersion));
    } while(false);

    return ret;
}
