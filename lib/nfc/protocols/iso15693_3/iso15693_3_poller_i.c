#include "iso15693_3_poller_i.h"

#include <nfc/helpers/iso14443_crc.h>

#define TAG "Iso15693_3Poller"

// static Iso15693_3Error iso15693_3_poller_process_error(NfcError error) {
//     switch(error) {
//     case NfcErrorNone:
//         return Iso15693_3ErrorNone;
//     case NfcErrorTimeout:
//         return Iso15693_3ErrorTimeout;
//     default:
//         return Iso15693_3ErrorNotPresent;
//     }
// }

static Iso15693_3Error iso15693_3_poller_prepare_trx(Iso15693_3Poller* instance) {
    furi_assert(instance);

    if(instance->state == Iso15693_3PollerStateIdle) {
        return iso15693_3_poller_async_activate(instance, NULL);
    }

    return Iso15693_3ErrorNone;
}

static Iso15693_3Error iso15693_3_poller_frame_exchange(
    Iso15693_3Poller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);

    UNUSED(instance);
    UNUSED(tx_buffer);
    UNUSED(rx_buffer);
    UNUSED(fwt);

    // const size_t tx_bytes = bit_buffer_get_size_bytes(tx_buffer);
    // furi_assert(
    //     tx_bytes <= bit_buffer_get_capacity_bytes(instance->tx_buffer) - ISO14443_CRC_SIZE);
    //
    // bit_buffer_copy(instance->tx_buffer, tx_buffer);
    // iso14443_crc_append(Iso14443CrcTypeB, instance->tx_buffer);

    Iso15693_3Error ret = Iso15693_3ErrorNone;

    do {
        //     NfcError error = nfc_trx(instance->nfc, instance->tx_buffer, instance->rx_buffer, fwt);
        //     if(error != NfcErrorNone) {
        //         ret = iso15693_3_poller_process_error(error);
        //         break;
        //     }
        //
        //     bit_buffer_copy(rx_buffer, instance->rx_buffer);
        //     if(!iso14443_crc_check(Iso14443CrcTypeB, instance->rx_buffer)) {
        //         ret = Iso15693_3ErrorWrongCrc;
        //         break;
        //     }
        //
        //     iso14443_crc_trim(rx_buffer);
    } while(false);

    return ret;
}

Iso15693_3Error
    iso15693_3_poller_async_activate(Iso15693_3Poller* instance, Iso15693_3Data* data) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    iso15693_3_reset(instance->data);

    Iso15693_3Error ret = Iso15693_3ErrorNone;

    do {
        instance->state = Iso15693_3PollerStateColResInProgress;

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_reset(instance->rx_buffer);

        // TODO: Activation procedure

        instance->state = Iso15693_3PollerStateActivated;

        if(data) {
            iso15693_3_copy(data, instance->data);
        }

    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_halt(Iso15693_3Poller* instance) {
    furi_assert(instance);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    Iso15693_3Error ret = Iso15693_3ErrorNone;

    do {
        // TODO: Implement HALT command
        instance->state = Iso15693_3PollerStateIdle;
    } while(false);

    return ret;
}

Iso15693_3Error iso15693_3_poller_send_frame(
    Iso15693_3Poller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    Iso15693_3Error ret;

    do {
        ret = iso15693_3_poller_prepare_trx(instance);
        if(ret != Iso15693_3ErrorNone) break;

        ret = iso15693_3_poller_frame_exchange(instance, tx_buffer, rx_buffer, fwt);
    } while(false);

    return ret;
}
