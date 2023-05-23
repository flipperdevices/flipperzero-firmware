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
    NfcPollerBuffer* buff = instance->plain_buff;
    MfClassicError ret = MfClassicErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        nfca_poller_get_data(instance->nfca_poller, &instance->data->nfca_data);
        uint8_t auth_cmd = (key_type == MfClassicKeyTypeB) ? MF_CLASSIC_AUTH_KEY_B_CMD :
                                                             MF_CLASSIC_AUTH_KEY_A_CMD;
        buff->tx_data[0] = auth_cmd;
        buff->tx_data[1] = block_num;
        buff->tx_bits = 2 * 8;

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
        if(data) {
            memcpy(data->nt.data, buff->rx_data, sizeof(MfClassicNt));
        }

        uint8_t* cuid_start = instance->data->nfca_data.uid;
        if(instance->data->nfca_data.uid_len == 7) {
            cuid_start = &instance->data->nfca_data.uid[3];
        }
        uint32_t cuid = (cuid_start[0] << 24) | (cuid_start[1] << 16) | (cuid_start[2] << 8) |
                        (cuid_start[3]);

        uint32_t nt = (uint32_t)nfc_util_bytes2num(buff->rx_data, sizeof(MfClassicNt));
        uint64_t key_num = nfc_util_bytes2num(key->data, sizeof(MfClassicKey));
        crypto1_init(instance->crypto, key_num);
        crypto1_word(instance->crypto, nt ^ cuid, 0);

        MfClassicNr nr = {};
        furi_hal_random_fill_buf(nr.data, sizeof(MfClassicNr));
        // TODO rework!
        for(size_t i = 0; i < sizeof(MfClassicNr); i++) {
            buff->tx_data[i] = crypto1_byte(instance->crypto, nr.data[i], 0) ^ nr.data[i];
            buff->tx_parity[0] |=
                (((crypto1_filter(instance->crypto->odd) ^ nfc_util_odd_parity8(nr.data[i])) &
                  0x01)
                 << (7 - i));
            nr.data[i] = buff->tx_data[i];
        }
        nt = prng_successor(nt, 32);
        for(size_t i = 4; i < 8; i++) {
            nt = prng_successor(nt, 8);
            buff->tx_data[i] = crypto1_byte(instance->crypto, 0, 0) ^ (nt & 0xff);
            buff->tx_parity[0] |=
                (((crypto1_filter(instance->crypto->odd) ^ nfc_util_odd_parity8(nt & 0xff)) & 0x01)
                 << (7 - i));
        }

        buff->tx_bits = 8 * 8;
        error = nfca_poller_txrx_custom_parity(
            instance->nfca_poller,
            buff->tx_data,
            buff->tx_parity,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_parity,
            buff->rx_data_size,
            &buff->rx_bits,
            MF_CLASSIC_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_classic_process_error(error);
            break;
        }
        if(buff->rx_bits != 4 * 8) {
            ret = MfClassicErrorAuth;
        }

        crypto1_word(instance->crypto, 0, 0);
        instance->auth_state = MfClassicAuthStatePassed;

        if(data) {
            data->nr = nr;
            memcpy(data->ar.data, &buff->tx_data[4], sizeof(MfClassicAr));
            memcpy(data->at.data, buff->rx_data, sizeof(MfClassicAt));
        }
    } while(false);

    if(ret != MfClassicErrorNone) {
        nfca_poller_halt(instance->nfca_poller);
    }

    return ret;
}

MfClassicError mf_classic_async_read_block(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicBlock* data) {
    NfcPollerBuffer* plain_buff = instance->plain_buff;
    NfcPollerBuffer* encrypted_buff = instance->encrypted_buff;
    MfClassicError ret = MfClassicErrorNone;
    NfcaError error = NfcaErrorNone;

    do {
        plain_buff->tx_data[0] = MF_CLASSIC_READ_BLOCK_CMD;
        plain_buff->tx_data[1] = block_num;
        nfca_append_crc(plain_buff->tx_data, 2);
        plain_buff->tx_bits = 4 * 8;

        encrypted_buff->tx_bits = plain_buff->tx_bits;
        crypto1_encrypt(
            instance->crypto,
            NULL,
            plain_buff->tx_data,
            plain_buff->tx_bits,
            encrypted_buff->tx_data,
            encrypted_buff->tx_parity);

        error = nfca_poller_txrx_custom_parity(
            instance->nfca_poller,
            encrypted_buff->tx_data,
            encrypted_buff->tx_parity,
            encrypted_buff->tx_bits,
            encrypted_buff->rx_data,
            encrypted_buff->rx_parity,
            encrypted_buff->rx_data_size,
            &encrypted_buff->rx_bits,
            MF_CLASSIC_FWT_FC);
        if(error != NfcaErrorNone) {
            ret = mf_classic_process_error(error);
            break;
        }
        if(encrypted_buff->rx_bits != (sizeof(MfClassicBlock) + 2) * 8) {
            ret = MfClassicErrorProtocol;
            break;
        }

        crypto1_decrypt(
            instance->crypto,
            encrypted_buff->rx_data,
            encrypted_buff->rx_bits,
            plain_buff->rx_data);

        if(!nfca_check_crc(plain_buff->rx_data, sizeof(MfClassicBlock) + 2)) {
            FURI_LOG_D(TAG, "CRC error");
            ret = MfClassicErrorProtocol;
            break;
        }
        memcpy(data->data, plain_buff->rx_data, sizeof(MfClassicBlock));
    } while(false);

    return ret;
}
