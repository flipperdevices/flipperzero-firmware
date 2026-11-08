#include "felica_seac_poller.h"
#include "felica_seac_poller_defs.h"

#include <furi.h>
#include <nfc/helpers/felica_crc.h>

#define FELICA_SEAC_GUARD_TIME_US    (20000U)
#define FELICA_SEAC_FDT_POLL_FC      (10000U)
#define FELICA_SEAC_POLL_POLL_MIN_US (1280U)
#define FELICA_SEAC_POLLING_FWT_FC   (200000U)
#define FELICA_SEAC_POLLER_BUF_SIZE  (32U)

#define FELICA_SEAC_POLLING_CMD           (0x00U)
#define FELICA_SEAC_SYSTEM_CODE           (0x0101U)
#define FELICA_SEAC_SELECTOR              (0x01U)
#define FELICA_SEAC_POLLING_FRAME_SIZE    (6U)
#define FELICA_SEAC_POLLING_RESPONSE_SIZE (15U)
#define FELICA_SEAC_TIME_SLOT_MASK        (0x01U)

struct FelicaSeacPoller {
    Nfc* nfc;
    BitBuffer* tx_buffer;
    BitBuffer* rx_buffer;
    FelicaSeacData* data;

    NfcGenericCallback callback;
    void* context;
    NfcGenericEvent general_event;
    FelicaSeacPollerEvent felica_seac_event;
    FelicaSeacPollerEventData felica_seac_event_data;
};

static FelicaSeacError felica_seac_poller_poll(FelicaSeacPoller* instance, FelicaSeacData* data) {
    const uint8_t polling_frame[] = {
        FELICA_SEAC_POLLING_FRAME_SIZE,
        FELICA_SEAC_POLLING_CMD,
        FELICA_SEAC_SYSTEM_CODE >> 8,
        FELICA_SEAC_SYSTEM_CODE & 0xFF,
        FELICA_SEAC_SELECTOR,
        FELICA_SEAC_TIME_SLOT_MASK,
    };
    // SEAC responses echo the command, system code and selector.
    const uint8_t response_header[] = {
        FELICA_SEAC_POLLING_RESPONSE_SIZE,
        FELICA_SEAC_POLLING_CMD,
        FELICA_SEAC_SYSTEM_CODE >> 8,
        FELICA_SEAC_SYSTEM_CODE & 0xFF,
        FELICA_SEAC_SELECTOR,
    };

    bit_buffer_copy_bytes(instance->tx_buffer, polling_frame, sizeof(polling_frame));
    felica_crc_append(instance->tx_buffer);
    bit_buffer_reset(instance->rx_buffer);

    const NfcError error = nfc_poller_trx(
        instance->nfc, instance->tx_buffer, instance->rx_buffer, FELICA_SEAC_POLLING_FWT_FC);
    if(error != NfcErrorNone) {
        return error == NfcErrorTimeout ? FelicaSeacErrorTimeout : FelicaSeacErrorNotPresent;
    }
    if(bit_buffer_get_size(instance->rx_buffer) !=
       (FELICA_SEAC_POLLING_RESPONSE_SIZE + FELICA_CRC_SIZE) * 8U) {
        return FelicaSeacErrorProtocol;
    }
    if(!felica_crc_check(instance->rx_buffer)) return FelicaSeacErrorWrongCrc;

    const uint8_t* response = bit_buffer_get_data(instance->rx_buffer);
    if(memcmp(response, response_header, sizeof(response_header)) != 0) {
        return FelicaSeacErrorProtocol;
    }

    FelicaSeacData result;
    memcpy(result.idm, response + sizeof(response_header), sizeof(result.idm));
    memcpy(
        result.context,
        response + sizeof(response_header) + sizeof(result.idm),
        sizeof(result.context));
    *data = result;
    return FelicaSeacErrorNone;
}

static const FelicaSeacData* felica_seac_poller_get_data(FelicaSeacPoller* instance) {
    furi_assert(instance);
    return instance->data;
}

static FelicaSeacPoller* felica_seac_poller_alloc(Nfc* nfc) {
    furi_assert(nfc);

    FelicaSeacPoller* instance = malloc(sizeof(FelicaSeacPoller));
    instance->nfc = nfc;
    instance->tx_buffer = bit_buffer_alloc(FELICA_SEAC_POLLER_BUF_SIZE);
    instance->rx_buffer = bit_buffer_alloc(FELICA_SEAC_POLLER_BUF_SIZE);
    instance->data = malloc(sizeof(FelicaSeacData));
    memset(instance->data, 0, sizeof(FelicaSeacData));

    nfc_config(instance->nfc, NfcModePoller, NfcTechFelica);
    nfc_set_guard_time_us(instance->nfc, FELICA_SEAC_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, FELICA_SEAC_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, FELICA_SEAC_POLL_POLL_MIN_US);

    instance->felica_seac_event.data = &instance->felica_seac_event_data;
    instance->general_event.protocol = NfcProtocolFelicaSeac;
    instance->general_event.event_data = &instance->felica_seac_event;
    instance->general_event.instance = instance;

    return instance;
}

static void felica_seac_poller_free(FelicaSeacPoller* instance) {
    furi_assert(instance);
    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    free(instance->data);
    free(instance);
}

static void felica_seac_poller_set_callback(
    FelicaSeacPoller* instance,
    NfcGenericCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

static NfcCommand felica_seac_poller_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);

    FelicaSeacPoller* instance = context;
    const NfcEvent* nfc_event = event.event_data;
    if(nfc_event->type != NfcEventTypePollerReady) return NfcCommandContinue;

    memset(instance->data, 0, sizeof(FelicaSeacData));
    const FelicaSeacError error = felica_seac_poller_poll(instance, instance->data);
    instance->felica_seac_event_data.error = error;
    instance->felica_seac_event.type = error == FelicaSeacErrorNone ?
                                           FelicaSeacPollerEventTypeReady :
                                           FelicaSeacPollerEventTypeError;
    const NfcCommand command = instance->callback(instance->general_event, instance->context);
    if(error != FelicaSeacErrorNone && command == NfcCommandContinue) {
        furi_delay_ms(100);
    }

    return command;
}

static bool felica_seac_poller_detect(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);

    const NfcEvent* nfc_event = event.event_data;
    if(nfc_event->type != NfcEventTypePollerReady) return false;

    FelicaSeacData data;
    return felica_seac_poller_poll(context, &data) == FelicaSeacErrorNone;
}

const NfcPollerBase nfc_poller_felica_seac = {
    .alloc = (NfcPollerAlloc)felica_seac_poller_alloc,
    .free = (NfcPollerFree)felica_seac_poller_free,
    .set_callback = (NfcPollerSetCallback)felica_seac_poller_set_callback,
    .run = (NfcPollerRun)felica_seac_poller_run,
    .detect = (NfcPollerDetect)felica_seac_poller_detect,
    .get_data = (NfcPollerGetData)felica_seac_poller_get_data,
};
