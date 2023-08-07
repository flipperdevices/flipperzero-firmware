#include "felica_poller_i.h"

#include <nfc/helpers/iso14443_crc.h>

#define TAG "FelicaPoller"

static FelicaError felica_poller_process_error(NfcError error) {
    switch(error) {
    case NfcErrorNone:
        return FelicaErrorNone;
    case NfcErrorTimeout:
        return FelicaErrorTimeout;
    default:
        return FelicaErrorNotPresent;
    }
}

static FelicaError felica_poller_prepare_trx(FelicaPoller* instance) {
    furi_assert(instance);

    if(instance->state == FelicaPollerStateIdle) {
        return felica_poller_async_activate(instance, NULL);
    }

    return FelicaErrorNone;
}

static FelicaError felica_poller_frame_exchange(
    FelicaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);

    const size_t tx_bytes = bit_buffer_get_size_bytes(tx_buffer);
    furi_assert(
        tx_bytes <= bit_buffer_get_capacity_bytes(instance->tx_buffer) - ISO14443_CRC_SIZE);

    bit_buffer_copy(instance->tx_buffer, tx_buffer);
    iso14443_crc_append(Iso14443CrcTypeB, instance->tx_buffer);

    FelicaError ret = FelicaErrorNone;

    do {
        NfcError error = nfc_trx(instance->nfc, instance->tx_buffer, instance->rx_buffer, fwt);
        if(error != NfcErrorNone) {
            ret = felica_poller_process_error(error);
            break;
        }

        bit_buffer_copy(rx_buffer, instance->rx_buffer);
        if(!iso14443_crc_check(Iso14443CrcTypeB, instance->rx_buffer)) {
            ret = FelicaErrorWrongCrc;
            break;
        }

        iso14443_crc_trim(rx_buffer);
    } while(false);

    return ret;
}

FelicaError felica_poller_async_activate(FelicaPoller* instance, FelicaData* data) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    felica_reset(data);

    FelicaError ret;

    do {
        instance->state = FelicaPollerStateColResInProgress;

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_reset(instance->rx_buffer);

        // Send Polling command
        FURI_LOG_I(TAG, "Polling command tx");
        const uint8_t tx[] = {0xb2, 0x4d, 0x06, 0x00, 0xff, 0xff, 0x00, 0x00, 0x09, 0x21};
        bit_buffer_copy_bytes(instance->tx_buffer, tx, sizeof(tx));

        ret = felica_poller_frame_exchange(
            instance, instance->tx_buffer, instance->rx_buffer, FELICA_FDT_POLL_FC);
        if(ret != FelicaErrorNone) {
            FURI_LOG_I(TAG, "Polling command rx error: %d", ret);
            instance->state = FelicaPollerStateColResFailed;
            break;
        }

        FURI_LOG_I(TAG, "Polling command rx");
        for(size_t i = 0; i < bit_buffer_get_size_bytes(instance->rx_buffer); i++) {
            printf("%02X ", bit_buffer_get_byte(instance->rx_buffer, i));
        }
        printf("\r\n");

        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(FelicaAtqB)) {
            FURI_LOG_D(TAG, "Unexpected Polling response");
            instance->state = FelicaPollerStateColResFailed;
            ret = FelicaErrorCommunication;
            break;
        }

        instance->state = FelicaPollerStateActivationInProgress;

        const FelicaAtqB* atqb = (const FelicaAtqB*)bit_buffer_get_data(instance->rx_buffer);

        memcpy(data->uid, atqb->uid, FELICA_UID_SIZE);
        memcpy(data->app_data, atqb->app_data, FELICA_APP_DATA_SIZE);
        memcpy(data->protocol_info, atqb->protocol_info, FELICA_PROTOCOL_INFO_SIZE);

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_reset(instance->rx_buffer);

        // Send ATTRIB
        bit_buffer_append_byte(instance->tx_buffer, 0x1d);
        bit_buffer_append_bytes(instance->tx_buffer, data->uid, FELICA_UID_SIZE);
        bit_buffer_append_byte(instance->tx_buffer, 0x00);
        bit_buffer_append_byte(instance->tx_buffer, FELICA_ATTRIB_FRAME_SIZE_256);
        bit_buffer_append_byte(instance->tx_buffer, 0x01);
        bit_buffer_append_byte(instance->tx_buffer, 0x00);

        ret = felica_poller_frame_exchange(
            instance, instance->tx_buffer, instance->rx_buffer, FELICA_FDT_ATTRIB_FC);
        if(ret != FelicaErrorNone) {
            instance->state = FelicaPollerStateActivationFailed;
            break;
        }

        if(bit_buffer_get_size_bytes(instance->rx_buffer) != 1 ||
           bit_buffer_get_byte(instance->rx_buffer, 0) != 0) {
            FURI_LOG_D(TAG, "Unexpected ATTRIB response");
            instance->state = FelicaPollerStateActivationFailed;
            ret = FelicaErrorCommunication;
            break;
        }

        instance->state = FelicaPollerStateActivated;
    } while(false);

    return ret;
}

FelicaError felica_poller_halt(FelicaPoller* instance) {
    furi_assert(instance);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    bit_buffer_append_byte(instance->tx_buffer, 0x50);
    bit_buffer_append_bytes(instance->tx_buffer, instance->data->uid, FELICA_UID_SIZE);

    FelicaError ret;

    do {
        ret = felica_poller_frame_exchange(
            instance, instance->tx_buffer, instance->rx_buffer, FELICA_FDT_POLL_FC);
        if(ret != FelicaErrorNone) {
            break;
        }

        if(bit_buffer_get_size_bytes(instance->rx_buffer) != sizeof(uint8_t) ||
           bit_buffer_get_byte(instance->rx_buffer, 0) != 0) {
            ret = FelicaErrorCommunication;
            break;
        }

        instance->state = FelicaPollerStateIdle;
    } while(false);

    return ret;
}

FelicaError felica_poller_send_frame(
    FelicaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    FelicaError ret;

    do {
        ret = felica_poller_prepare_trx(instance);
        if(ret != FelicaErrorNone) break;

        ret = felica_poller_frame_exchange(instance, tx_buffer, rx_buffer, fwt);
    } while(false);

    return ret;
}
