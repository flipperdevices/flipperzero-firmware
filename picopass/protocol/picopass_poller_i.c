#include "picopass_poller_i.h"

#include <nfc/helpers/iso14443_crc.h>

#define PICOPASS_POLLER_FWT_FC (100000)

#define TAG "Picopass"

static PicopassError picopass_poller_process_error(NfcError error) {
    PicopassError ret = PicopassErrorNone;

    switch(error) {
    case NfcErrorNone:
        ret = PicopassErrorNone;
        break;

    default:
        ret = PicopassErrorTimeout;
        break;
    }

    return ret;
}

static PicopassError picopass_poller_send_frame(
    PicopassPoller* instance,
    BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt_fc) {
    PicopassError ret = PicopassErrorNone;

    do {
        NfcError error = nfc_poller_trx(instance->nfc, tx_buffer, rx_buffer, fwt_fc);
        if(error != NfcErrorNone) {
            ret = picopass_poller_process_error(error);
            break;
        }
        if(!iso13239_crc_check(Iso13239CrcTypePicopass, rx_buffer)) {
            ret = PicopassErrorIncorrectCrc;
            break;
        }
        iso13239_crc_trim(instance->rx_buffer);
    } while(false);

    return ret;
}

PicopassError picopass_poller_actall(PicopassPoller* instance) {
    PicopassError ret = PicopassErrorNone;

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_byte(instance->tx_buffer, RFAL_PICOPASS_CMD_ACTALL);

    NfcError error = nfc_poller_trx(
        instance->nfc, instance->tx_buffer, instance->rx_buffer, PICOPASS_POLLER_FWT_FC);
    if(error != NfcErrorIncompleteFrame) {
        ret = picopass_poller_process_error(error);
    }

    return ret;
}

PicopassError picopass_poller_identify(
    PicopassPoller* instance,
    PicopassColResSerialNum* col_res_serial_num) {
    PicopassError ret = PicopassErrorNone;

    do {
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, RFAL_PICOPASS_CMD_READ_OR_IDENTIFY);
        ret = picopass_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, PICOPASS_POLLER_FWT_FC);
        if(ret != PicopassErrorNone) break;
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(PicopassColResSerialNum)) {
            ret = PicopassErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(
            instance->rx_buffer, col_res_serial_num->data, sizeof(PicopassColResSerialNum));
    } while(false);

    return ret;
}

PicopassError picopass_poller_select(
    PicopassPoller* instance,
    PicopassColResSerialNum* col_res_serial_num,
    PicopassSerialNum* serial_num) {
    PicopassError ret = PicopassErrorNone;

    do {
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, RFAL_PICOPASS_CMD_SELECT);
        bit_buffer_append_bytes(
            instance->tx_buffer, col_res_serial_num->data, sizeof(PicopassColResSerialNum));
        ret = picopass_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, PICOPASS_POLLER_FWT_FC);
        if(ret != PicopassErrorNone) break;
        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(PicopassSerialNum)) {
            ret = PicopassErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, serial_num->data, sizeof(PicopassSerialNum));
    } while(false);

    return ret;
}

PicopassError
    picopass_poller_read_block(PicopassPoller* instance, uint8_t block_num, PicopassBlock* block) {
    PicopassError ret = PicopassErrorNone;

    do {
        bit_buffer_reset(instance->tmp_buffer);
        bit_buffer_append_byte(instance->tmp_buffer, block_num);
        iso13239_crc_append(Iso13239CrcTypePicopass, instance->tmp_buffer);
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, RFAL_PICOPASS_CMD_READ_OR_IDENTIFY);
        bit_buffer_append(instance->tx_buffer, instance->tmp_buffer);

        ret = picopass_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, PICOPASS_POLLER_FWT_FC);
        if(ret != PicopassErrorNone) break;

        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(PicopassBlock)) {
            ret = PicopassErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, block->data, sizeof(PicopassBlock));
    } while(false);

    return ret;
}

PicopassError
    picopass_poller_read_check(PicopassPoller* instance, PicopassReadCheckResp* read_check_resp) {
    PicopassError ret = PicopassErrorNone;

    do {
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, RFAL_PICOPASS_CMD_READCHECK_KD);
        bit_buffer_append_byte(instance->tx_buffer, 0x02);

        NfcError error = nfc_poller_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, PICOPASS_POLLER_FWT_FC);
        if(error != NfcErrorNone) {
            ret = picopass_poller_process_error(error);
            break;
        }

        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(PicopassReadCheckResp)) {
            ret = PicopassErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(
            instance->rx_buffer, read_check_resp->data, sizeof(PicopassReadCheckResp));
    } while(false);

    return ret;
}

PicopassError picopass_poller_check(
    PicopassPoller* instance,
    uint8_t* nr,
    PicopassMac* mac,
    PicopassCheckResp* check_resp) {
    PicopassError ret = PicopassErrorNone;
    uint8_t null_arr[4] = {};

    do {
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, RFAL_PICOPASS_CMD_CHECK);
        if(nr) {
            bit_buffer_append_bytes(instance->tx_buffer, nr, 4);
        } else {
            bit_buffer_append_bytes(instance->tx_buffer, null_arr, sizeof(null_arr));
        }
        bit_buffer_append_bytes(instance->tx_buffer, mac->data, sizeof(PicopassMac));

        NfcError error = nfc_poller_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, PICOPASS_POLLER_FWT_FC);
        if(error != NfcErrorNone) {
            ret = picopass_poller_process_error(error);
            break;
        }

        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(PicopassCheckResp)) {
            ret = PicopassErrorProtocol;
            break;
        }
        bit_buffer_write_bytes(instance->rx_buffer, check_resp->data, sizeof(PicopassCheckResp));

    } while(false);

    return ret;
}

PicopassError picopass_poller_write_block(
    PicopassPoller* instance,
    uint8_t block_num,
    const PicopassBlock* block,
    const PicopassMac* mac) {
    PicopassError ret = PicopassErrorNone;

    do {
        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, RFAL_PICOPASS_CMD_UPDATE);
        bit_buffer_append_byte(instance->tx_buffer, block_num);
        bit_buffer_append_bytes(instance->tx_buffer, block->data, sizeof(PicopassBlock));
        bit_buffer_append_bytes(instance->tx_buffer, mac->data, sizeof(PicopassMac));

        NfcError error = nfc_poller_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, PICOPASS_POLLER_FWT_FC);
        if(error != NfcErrorNone) {
            ret = picopass_poller_process_error(error);
            break;
        }

    } while(false);

    return ret;
}
