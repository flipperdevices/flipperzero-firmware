#include "mf_classic_poller_i.h"

#include <furi.h>
#include <furi_hal_random.h>

#define TAG "MfCLassicPoller"

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
    MfClassicError ret = MfClassicErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        nfca_poller_get_data(instance->nfca_poller, &instance->data->nfca_data);
        uint8_t auth_type = (key_type == MfClassicKeyTypeB) ? MF_CLASSIC_AUTH_KEY_B_CMD :
                                                              MF_CLASSIC_AUTH_KEY_A_CMD;
        uint8_t auth_cmd[2] = {auth_type, block_num};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, auth_cmd, sizeof(auth_cmd));

        error = nfca_poller_send_standart_frame(
            instance->nfca_poller,
            instance->tx_plain_buffer,
            instance->rx_plain_buffer,
            MF_CLASSIC_FWT_FC);
        if(error != NfcaErrorWrongCrc) {
            ret = mf_classic_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_plain_buffer) != sizeof(MfClassicNt)) {
            ret = MfClassicErrorProtocol;
            break;
        }

        MfClassicNt nt = {};
        bit_buffer_write_bytes(instance->rx_plain_buffer, nt.data, sizeof(nt.data));
        if(data) {
            data->nt = nt;
        }

        uint32_t cuid = nfca_get_cuid(&instance->data->nfca_data);
        uint64_t key_num = nfc_util_bytes2num(key->data, sizeof(MfClassicKey));
        MfClassicNr nr = {};
        furi_hal_random_fill_buf(nr.data, sizeof(MfClassicNr));

        crypto1_encrypt_reader_nonce(
            instance->crypto, key_num, cuid, nt.data, nt.data, instance->tx_encrypted_buffer);
        error = nfca_poller_txrx_custom_parity(
            instance->nfca_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            MF_CLASSIC_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_classic_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_encrypted_buffer) != 4) {
            ret = MfClassicErrorAuth;
        }

        crypto1_word(instance->crypto, 0, 0);
        instance->auth_state = MfClassicAuthStatePassed;

        if(data) {
            data->nr = nr;
            bit_buffer_write_bytes(
                instance->tx_encrypted_buffer, data->ar.data, sizeof(MfClassicAr));
            bit_buffer_write_bytes(
                instance->rx_encrypted_buffer, data->at.data, sizeof(MfClassicAt));
        }
    } while(false);

    if(ret != MfClassicErrorNone) {
        nfca_poller_halt(instance->nfca_poller);
    }

    return ret;
}

MfClassicError mf_classic_aync_halt(MfClassicPoller* instance) {
    MfClassicError ret = MfClassicErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t halt_cmd[2] = {MF_CLASSIC_HALT_MSB_CMD, MF_CLASSIC_HALT_LSB_CMD};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, halt_cmd, sizeof(halt_cmd));
        nfca_append_crc(instance->tx_plain_buffer);

        crypto1_encrypt(
            instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);

        error = nfca_poller_txrx_custom_parity(
            instance->nfca_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            MF_CLASSIC_FWT_FC);
        if(error != NfcaErrorTimeout) {
            ret = mf_classic_process_error(error);
            break;
        }
        instance->auth_state = MfClassicAuthStateIdle;
        instance->nfca_poller->state = NfcaPollerStateIdle;
    } while(false);

    return ret;
}

MfClassicError mf_classic_async_read_block(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicBlock* data) {
    MfClassicError ret = MfClassicErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        uint8_t read_block_cmd[2] = {MF_CLASSIC_READ_BLOCK_CMD, block_num};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, read_block_cmd, sizeof(read_block_cmd));
        nfca_append_crc(instance->tx_plain_buffer);

        crypto1_encrypt(
            instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);

        error = nfca_poller_txrx_custom_parity(
            instance->nfca_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            MF_CLASSIC_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_classic_process_error(error);
            break;
        }
        if(bit_buffer_get_size_bytes(instance->rx_encrypted_buffer) !=
           (sizeof(MfClassicBlock) + 2)) {
            ret = MfClassicErrorProtocol;
            break;
        }

        crypto1_decrypt(
            instance->crypto, instance->rx_encrypted_buffer, instance->rx_plain_buffer);

        if(!nfca_check_crc(instance->rx_plain_buffer)) {
            FURI_LOG_D(TAG, "CRC error");
            ret = MfClassicErrorProtocol;
            break;
        }

        bit_buffer_write_bytes(instance->rx_plain_buffer, data->data, sizeof(MfClassicBlock));
    } while(false);

    return ret;
}
