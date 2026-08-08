#include "innovatron_poller_i.h"

#include "innovatron_poller_defs.h"

#include <furi.h>
#include <nfc/helpers/iso14443_crc.h>

#define TAG "InnovatronPoller"

static InnovatronError innovatron_poller_process_error(NfcError error) {
    switch(error) {
    case NfcErrorNone:
        return InnovatronErrorNone;
    case NfcErrorTimeout:
        return InnovatronErrorTimeout;
    default:
        return InnovatronErrorNotPresent;
    }
}

static InnovatronError innovatron_poller_send_frame(
    InnovatronPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_check(instance);
    furi_check(tx_buffer);
    furi_check(rx_buffer);

    const size_t tx_bytes = bit_buffer_get_size_bytes(tx_buffer);
    furi_assert(
        tx_bytes <= bit_buffer_get_capacity_bytes(instance->tx_buffer) - ISO14443_CRC_SIZE);

    bit_buffer_copy(instance->tx_buffer, tx_buffer);
    iso14443_crc_append(Iso14443CrcTypeB, instance->tx_buffer);

    InnovatronError ret = InnovatronErrorNone;

    do {
        NfcError error =
            nfc_poller_trx(instance->nfc, instance->tx_buffer, instance->rx_buffer, fwt);
        if(error != NfcErrorNone) {
            FURI_LOG_T(TAG, "error during trx: %d", error);
            ret = innovatron_poller_process_error(error);
            break;
        }

        bit_buffer_copy(rx_buffer, instance->rx_buffer);
        if(!iso14443_crc_check(Iso14443CrcTypeB, instance->rx_buffer)) {
            ret = InnovatronErrorWrongCrc;
            break;
        }

        iso14443_crc_trim(rx_buffer);
    } while(false);

    return ret;
}

static uint8_t innovatron_poller_next_com_sequence(uint8_t sequence) {
    return (sequence >= INNOVATRON_COM_SEQUENCE_END) ? INNOVATRON_COM_SEQUENCE_START :
                                                       (uint8_t)(sequence + 1U);
}

static InnovatronError
    innovatron_poller_attrib(InnovatronPoller* instance, const InnovatronData* data) {
    furi_check(instance);
    furi_check(data);

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    bit_buffer_append_byte(instance->tx_buffer, data->vt_addr);
    bit_buffer_append_byte(instance->tx_buffer, INNOVATRON_CMD_ATTRIB);
    bit_buffer_append_bytes(instance->tx_buffer, data->div, INNOVATRON_DIV_SIZE);

    InnovatronError ret;

    do {
        ret = innovatron_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, INNOVATRON_RR_TR0_FC);
        if(ret != InnovatronErrorNone) break;

        const size_t rr_len = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(rr_len != 2U) {
            FURI_LOG_D(TAG, "Unexpected RR length: %zu", rr_len);
            ret = InnovatronErrorCommunication;
            break;
        }

        const uint8_t* rr = bit_buffer_get_data(instance->rx_buffer);
        if((rr[0] != data->vt_addr) || (rr[1] != INNOVATRON_CMD_RR)) {
            FURI_LOG_D(TAG, "Unexpected ATTRIB response: %02X %02X", rr[0], rr[1]);
            ret = InnovatronErrorCommunication;
            break;
        }
    } while(false);

    return ret;
}

InnovatronError innovatron_poller_apgen(InnovatronPoller* instance, InnovatronData* data) {
    furi_check(instance);
    furi_check(instance->nfc);
    furi_check(data);

    innovatron_reset(data);
    instance->activated = false;
    instance->com_sequence = INNOVATRON_COM_SEQUENCE_START;

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    bit_buffer_append_byte(instance->tx_buffer, INNOVATRON_VT_ADDR_DEFAULT);
    bit_buffer_append_byte(instance->tx_buffer, INNOVATRON_CMD_APGEN);
    bit_buffer_append_byte(instance->tx_buffer, INNOVATRON_APGEN_OCCUPAR_DEFAULT);
    bit_buffer_append_byte(instance->tx_buffer, INNOVATRON_APGEN_CONFIG_ATR_REQUEST_MASK);

    InnovatronError ret;

    do {
        ret = innovatron_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, INNOVATRON_APGEN_TR0_FC);
        if(ret != InnovatronErrorNone) break;

        const size_t repgen_len = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(repgen_len < 8U) {
            FURI_LOG_D(TAG, "REPGEN too short: %zu", repgen_len);
            ret = InnovatronErrorCommunication;
            break;
        }

        const uint8_t* repgen = bit_buffer_get_data(instance->rx_buffer);
        if((repgen[0] != INNOVATRON_VT_ADDR_DEFAULT) || (repgen[1] != INNOVATRON_CMD_REPGEN)) {
            ret = InnovatronErrorCommunication;
            break;
        }

        data->vt_addr = repgen[0];
        memcpy(data->div, &repgen[2], INNOVATRON_DIV_SIZE);
        data->verlog = repgen[6];

        if(!(data->verlog & INNOVATRON_REPGEN_VERLOG_LONG_FORMAT_MASK)) {
            FURI_LOG_D(TAG, "Invalid REPGEN VerLog: %02X", data->verlog);
            ret = InnovatronErrorCommunication;
            break;
        }

        data->config = repgen[7];

        const size_t atr_len = repgen_len - 8U;
        const bool atr_present = data->config & INNOVATRON_REPGEN_CONFIG_ATR_PRESENT_MASK;
        if((atr_present != (atr_len > 0U)) || (atr_len > INNOVATRON_ATR_MAX_SIZE)) {
            FURI_LOG_D(TAG, "Invalid REPGEN ATR length: %zu", atr_len);
            ret = InnovatronErrorCommunication;
            break;
        }

        if(atr_present) {
            data->atr_len = atr_len;
            memcpy(data->atr, &repgen[8], data->atr_len);
        }
    } while(false);

    return ret;
}

InnovatronError innovatron_poller_activate(InnovatronPoller* instance, InnovatronData* data) {
    furi_check(instance);
    furi_check(data);

    InnovatronError ret = InnovatronErrorNone;

    do {
        if(instance->activated) {
            innovatron_copy(data, instance->data);
            break;
        }

        ret = innovatron_poller_apgen(instance, data);
        if(ret != InnovatronErrorNone) break;

        ret = innovatron_poller_attrib(instance, data);
        if(ret != InnovatronErrorNone) break;

        innovatron_copy(instance->data, data);
        instance->activated = true;
        instance->com_sequence = INNOVATRON_COM_SEQUENCE_START;
    } while(false);

    return ret;
}

InnovatronError innovatron_poller_com_r(
    InnovatronPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer) {
    furi_check(instance);
    furi_check(tx_buffer);
    furi_check(rx_buffer);

    InnovatronError ret = InnovatronErrorNone;

    do {
        if(!instance->activated) {
            ret = innovatron_poller_activate(instance, instance->data);
            if(ret != InnovatronErrorNone) break;
        }

        const size_t apdu_len = bit_buffer_get_size_bytes(tx_buffer);
        const size_t max_apdu_len =
            bit_buffer_get_capacity_bytes(instance->tx_buffer) - 3U - ISO14443_CRC_SIZE;
        if((apdu_len > max_apdu_len) || (apdu_len > UINT8_MAX - 1U)) {
            ret = InnovatronErrorCommunication;
            break;
        }

        bit_buffer_reset(instance->tx_buffer);
        bit_buffer_append_byte(instance->tx_buffer, instance->data->vt_addr);
        bit_buffer_append_byte(instance->tx_buffer, INNOVATRON_CMD_COM_R(instance->com_sequence));
        bit_buffer_append_byte(instance->tx_buffer, apdu_len + 1U);
        bit_buffer_append_bytes(
            instance->tx_buffer,
            bit_buffer_get_data(tx_buffer),
            bit_buffer_get_size_bytes(tx_buffer));

        ret = innovatron_poller_send_frame(
            instance, instance->tx_buffer, instance->rx_buffer, INNOVATRON_COM_R_TR0_FC);
        if(ret != InnovatronErrorNone) break;

        const size_t frame_len = bit_buffer_get_size_bytes(instance->rx_buffer);
        if(frame_len < 3) {
            ret = InnovatronErrorCommunication;
            break;
        }

        const uint8_t* frame = bit_buffer_get_data(instance->rx_buffer);
        const uint8_t rx_len = frame[2];
        if((frame[0] != instance->data->vt_addr) ||
           (frame[1] != INNOVATRON_CMD_REC(instance->com_sequence)) || (rx_len == 0) ||
           (frame_len != (size_t)rx_len + 2U)) {
            ret = InnovatronErrorCommunication;
            break;
        }

        bit_buffer_copy_bytes(rx_buffer, &frame[3], rx_len - 1U);
        instance->com_sequence = innovatron_poller_next_com_sequence(instance->com_sequence);
    } while(false);

    return ret;
}

const InnovatronData* innovatron_poller_get_data(InnovatronPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

static InnovatronPoller* innovatron_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    InnovatronPoller* instance = malloc(sizeof(InnovatronPoller));
    instance->nfc = nfc;
    instance->tx_buffer = bit_buffer_alloc(INNOVATRON_POLLER_BUF_SIZE);
    instance->rx_buffer = bit_buffer_alloc(INNOVATRON_POLLER_BUF_SIZE);
    instance->data = innovatron_alloc();
    instance->activated = false;
    instance->com_sequence = INNOVATRON_COM_SEQUENCE_START;

    nfc_config(instance->nfc, NfcModePoller, NfcTechInnovatron);
    nfc_set_guard_time_us(instance->nfc, INNOVATRON_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, INNOVATRON_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, INNOVATRON_APGEN_REPEAT_MIN_US);

    instance->innovatron_event.data = &instance->innovatron_event_data;
    instance->general_event.protocol = NfcProtocolInnovatron;
    instance->general_event.event_data = &instance->innovatron_event;
    instance->general_event.instance = instance;

    return instance;
}

static void innovatron_poller_free(InnovatronPoller* instance) {
    furi_assert(instance);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    innovatron_free(instance->data);
    free(instance);
}

static void innovatron_poller_set_callback(
    InnovatronPoller* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);

    instance->callback = callback;
    instance->context = context;
}

static NfcCommand innovatron_poller_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);

    InnovatronPoller* instance = context;
    NfcEvent* nfc_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    if(nfc_event->type == NfcEventTypePollerReady) {
        const InnovatronError error = innovatron_poller_activate(instance, instance->data);
        if(error == InnovatronErrorNone) {
            instance->innovatron_event.type = InnovatronPollerEventTypeReady;
        } else {
            instance->innovatron_event.type = InnovatronPollerEventTypeError;
            instance->innovatron_event_data.error = error;
        }

        command = instance->callback(instance->general_event, instance->context);
        if(error != InnovatronErrorNone) {
            furi_delay_ms(100);
        }
    }

    return command;
}

static bool innovatron_poller_detect(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.event_data);
    furi_assert(event.instance);
    furi_assert(event.protocol == NfcProtocolInvalid);

    bool protocol_detected = false;
    InnovatronPoller* instance = context;
    NfcEvent* nfc_event = event.event_data;

    if(nfc_event->type == NfcEventTypePollerReady) {
        InnovatronData data = {};
        const InnovatronError error = innovatron_poller_apgen(instance, &data);
        protocol_detected = (error == InnovatronErrorNone);
    }

    return protocol_detected;
}

const NfcPollerBase nfc_poller_innovatron = {
    .alloc = (NfcPollerAlloc)innovatron_poller_alloc,
    .free = (NfcPollerFree)innovatron_poller_free,
    .set_callback = (NfcPollerSetCallback)innovatron_poller_set_callback,
    .run = (NfcPollerRun)innovatron_poller_run,
    .detect = (NfcPollerDetect)innovatron_poller_detect,
    .get_data = (NfcPollerGetData)innovatron_poller_get_data,
};
