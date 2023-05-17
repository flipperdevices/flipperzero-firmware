#include "mf_classic_poller_i.h"

#include <furi.h>

#define MF_CLASSIC_FWT_FC (60000)

MfClassicError mf_classic_process_error(NfcaError error) {
    MfClassicError ret = MfClassicErrorNone;

    switch(error) {
    case NfcaErrorNone:
        ret = MfClassicErrorNone;
        break;
    case NfcaErrorNotPresent:
        ret = MfClassicErrorNotPresent;
        break;
    case NfcaErrorColResFailed:
    case NfcaErrorCommunication:
    case NfcaErrorWrongCrc:
        ret = MfClassicErrorProtocol;
        break;
    case NfcaErrorTimeout:
        ret = MfClassicErrorTimeout;
        break;
    default:
        ret = MfClassicErrorProtocol;
        break;
    }

    return ret;
}

MfClassicError mf_classic_async_auth(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicKey* key,
    MfClassicKeyType key_type,
    MfClassicAuthContext* data) {
    UNUSED(key);
    NfcPollerBuffer* buff = instance->buffer;
    MfClassicError ret = MfClassicErrorNone;
    NfcaError error = NfcaErrorNone;

    uint8_t auth_cmd = (key_type == MfClassicKeyTypeB) ? MF_CLASSIC_AUTH_KEY_B_CMD :
                                                         MF_CLASSIC_AUTH_KEY_A_CMD;
    buff->tx_data[0] = auth_cmd;
    buff->tx_data[1] = block_num;
    buff->tx_bits = 2 * 8;

    do {
        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_CLASSIC_FWT_FC);
        if(error != NfcaErrorWrongCrc) {
            ret = mf_classic_process_error(error);
            break;
        }
        if(buff->rx_bits != sizeof(MfClassicNt) * 8) {
            ret = MfClassicErrorProtocol;
            break;
        }
        memcpy(data->nt.data, buff->rx_data, sizeof(MfClassicNt));
    } while(false);

    return ret;
}

MfClassicError
    mf_classic_async_read_block(MfClassicPoller* instance, uint8_t block_num, MfClassicBlock* data);
