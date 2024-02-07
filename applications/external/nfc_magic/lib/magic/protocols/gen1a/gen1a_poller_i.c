#include "gen1a_poller_i.h"
#include <nfc/helpers/iso14443_crc.h>

#include <furi/furi.h>

static Gen1aPollerError gen1a_poller_process_nfc_error(NfcError error) {
    Gen1aPollerError ret = Gen1aPollerErrorNone;

    if(error == NfcErrorNone) {
        ret = Gen1aPollerErrorNone;
    } else if(error == NfcErrorTimeout) {
        ret = Gen1aPollerErrorTimeout;
    } else {
        ret = Gen1aPollerErrorNotPresent;
    }

    return ret;
}

Gen1aPollerError gen1a_poller_wupa(Gen1aPoller* instance) {
    furi_assert(instance);

    Gen1aPollerError ret = Gen1aPollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_set_size(instance->tx_buffer, 7);
        bit_buffer_set_byte(instance->tx_buffer, 0, 0x40);

        NfcError error = nfc_poller_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, GEN1A_POLLER_MAX_FWT);

        if(error != NfcErrorNone) {
            ret = gen1a_poller_process_nfc_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_buffer) != 4) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }
        if(bit_buffer_get_byte(instance->rx_buffer, 0) != 0x0A) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

Gen1aPollerError gen1a_poller_data_access(Gen1aPoller* instance) {
    furi_assert(instance);

    Gen1aPollerError ret = Gen1aPollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_set_size(instance->tx_buffer, 8);
        bit_buffer_set_byte(instance->tx_buffer, 0, 0x43);

        NfcError error = nfc_poller_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, GEN1A_POLLER_MAX_FWT);

        if(error != NfcErrorNone) {
            ret = gen1a_poller_process_nfc_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_buffer) != 4) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }
        if(bit_buffer_get_byte(instance->rx_buffer, 0) != 0x0A) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}

Gen1aPollerError gen1a_poller_write_block(
    Gen1aPoller* instance,
    uint8_t block_num,
    const MfClassicBlock* block) {
    furi_assert(instance);
    furi_assert(block);

    Gen1aPollerError ret = Gen1aPollerErrorNone;
    bit_buffer_reset(instance->tx_buffer);

    do {
        bit_buffer_append_byte(instance->tx_buffer, 0xA0);
        bit_buffer_append_byte(instance->tx_buffer, block_num);
        iso14443_crc_append(Iso14443CrcTypeA, instance->tx_buffer);

        NfcError error = nfc_poller_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, GEN1A_POLLER_MAX_FWT);

        if(error != NfcErrorNone) {
            ret = gen1a_poller_process_nfc_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_buffer) != 4) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }
        if(bit_buffer_get_byte(instance->rx_buffer, 0) != 0x0A) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }

        bit_buffer_copy_bytes(instance->tx_buffer, block->data, sizeof(MfClassicBlock));
        iso14443_crc_append(Iso14443CrcTypeA, instance->tx_buffer);

        error = nfc_poller_trx(
            instance->nfc, instance->tx_buffer, instance->rx_buffer, GEN1A_POLLER_MAX_FWT);

        if(error != NfcErrorNone) {
            ret = gen1a_poller_process_nfc_error(error);
            break;
        }
        if(bit_buffer_get_size(instance->rx_buffer) != 4) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }
        if(bit_buffer_get_byte(instance->rx_buffer, 0) != 0x0A) {
            ret = Gen1aPollerErrorProtocol;
            break;
        }
    } while(false);

    return ret;
}
