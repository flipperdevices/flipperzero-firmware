#include "mf_classic_poller_i.h"

#include <furi.h>
#include <furi_hal_random.h>

#define TAG "MfCLassicPoller"

#define MF_CLASSIC_FWT_FC (60000)

MfClassicError mf_classic_process_error(Iso14443_3aError error) {
    MfClassicError ret = MfClassicErrorNone;

    switch(error) {
    case Iso14443_3aErrorNone:
        ret = MfClassicErrorNone;
        break;
    case Iso14443_3aErrorNotPresent:
        ret = MfClassicErrorNotPresent;
        break;
    case Iso14443_3aErrorColResFailed:
    case Iso14443_3aErrorCommunication:
    case Iso14443_3aErrorWrongCrc:
        ret = MfClassicErrorProtocol;
        break;
    case Iso14443_3aErrorTimeout:
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
    Iso14443_3aError error = Iso14443_3aErrorNone;

    do {
        iso14443_3a_copy(
            instance->data->iso14443_3a_data,
            iso14443_3a_poller_get_data(instance->iso14443_3a_poller));
        uint8_t auth_type = (key_type == MfClassicKeyTypeB) ? MF_CLASSIC_AUTH_KEY_B_CMD :
                                                              MF_CLASSIC_AUTH_KEY_A_CMD;
        uint8_t auth_cmd[2] = {auth_type, block_num};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, auth_cmd, sizeof(auth_cmd));

        error = iso14443_3a_poller_send_standart_frame(
            instance->iso14443_3a_poller,
            instance->tx_plain_buffer,
            instance->rx_plain_buffer,
            MF_CLASSIC_FWT_FC);
        if(error != Iso14443_3aErrorWrongCrc) {
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
        uint32_t cuid = iso14443_3a_get_cuid(instance->data->iso14443_3a_data);
        uint64_t key_num = nfc_util_bytes2num(key->data, sizeof(MfClassicKey));
        MfClassicNr nr = {};
        furi_hal_random_fill_buf(nr.data, sizeof(MfClassicNr));

        crypto1_encrypt_reader_nonce(
            instance->crypto, key_num, cuid, nt.data, nr.data, instance->tx_encrypted_buffer);
        error = iso14443_3a_poller_txrx_custom_parity(
            instance->iso14443_3a_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            MF_CLASSIC_FWT_FC);

        if(error != Iso14443_3aErrorNone) {
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
            const uint8_t* nr_ar = bit_buffer_get_data(instance->tx_encrypted_buffer);
            memcpy(data->ar.data, &nr_ar[4], sizeof(MfClassicAr));
            bit_buffer_write_bytes(
                instance->rx_encrypted_buffer, data->at.data, sizeof(MfClassicAt));
        }
    } while(false);

    if(ret != MfClassicErrorNone) {
        iso14443_3a_poller_halt(instance->iso14443_3a_poller);
    }

    return ret;
}

MfClassicError mf_classic_aync_halt(MfClassicPoller* instance) {
    MfClassicError ret = MfClassicErrorNone;
    Iso14443_3aError error = Iso14443_3aErrorNone;

    do {
        uint8_t halt_cmd[2] = {MF_CLASSIC_HALT_MSB_CMD, MF_CLASSIC_HALT_LSB_CMD};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, halt_cmd, sizeof(halt_cmd));
        iso14443_3a_append_crc(instance->tx_plain_buffer);

        crypto1_encrypt(
            instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);

        error = iso14443_3a_poller_txrx_custom_parity(
            instance->iso14443_3a_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            MF_CLASSIC_FWT_FC);
        if(error != Iso14443_3aErrorTimeout) {
            ret = mf_classic_process_error(error);
            break;
        }
        instance->auth_state = MfClassicAuthStateIdle;
        instance->iso14443_3a_poller->state = Iso14443_3aPollerStateIdle;
    } while(false);

    return ret;
}

MfClassicError mf_classic_async_read_block(
    MfClassicPoller* instance,
    uint8_t block_num,
    MfClassicBlock* data) {
    MfClassicError ret = MfClassicErrorNone;
    Iso14443_3aError error = Iso14443_3aErrorNone;

    do {
        uint8_t read_block_cmd[2] = {MF_CLASSIC_READ_BLOCK_CMD, block_num};
        bit_buffer_copy_bytes(instance->tx_plain_buffer, read_block_cmd, sizeof(read_block_cmd));
        iso14443_3a_append_crc(instance->tx_plain_buffer);

        crypto1_encrypt(
            instance->crypto, NULL, instance->tx_plain_buffer, instance->tx_encrypted_buffer);

        error = iso14443_3a_poller_txrx_custom_parity(
            instance->iso14443_3a_poller,
            instance->tx_encrypted_buffer,
            instance->rx_encrypted_buffer,
            MF_CLASSIC_FWT_FC);
        if(error != Iso14443_3aErrorNone) {
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

        if(!iso14443_3a_check_crc(instance->rx_plain_buffer)) {
            FURI_LOG_D(TAG, "CRC error");
            ret = MfClassicErrorProtocol;
            break;
        }

        iso14443_3a_trim_crc(instance->rx_plain_buffer);
        bit_buffer_write_bytes(instance->rx_plain_buffer, data->data, sizeof(MfClassicBlock));
    } while(false);

    return ret;
}
