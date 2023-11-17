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

PicopassError picopass_poller_send_frame(
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
