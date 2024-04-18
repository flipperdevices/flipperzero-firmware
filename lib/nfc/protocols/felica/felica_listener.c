#include "felica_listener_i.h"

#include "nfc/protocols/nfc_listener_base.h"
#include <nfc/helpers/felica_crc.h>

#define FELICA_LISTENER_MAX_BUFFER_SIZE (128)
#define TAG "Felica"

FelicaListener* felica_listener_alloc(Nfc* nfc, FelicaData* data) {
    furi_assert(nfc);
    furi_assert(data);

    FelicaListener* instance = malloc(sizeof(FelicaListener));
    instance->nfc = nfc;
    instance->data = data;
    instance->tx_buffer = bit_buffer_alloc(FELICA_LISTENER_MAX_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(FELICA_LISTENER_MAX_BUFFER_SIZE);

    nfc_set_fdt_listen_fc(instance->nfc, FELICA_FDT_LISTEN_FC);

    nfc_config(instance->nfc, NfcModeListener, NfcTechFelica);
    nfc_felica_listener_set_sensf_res_data(
        nfc, data->idm.data, sizeof(data->idm), data->pmm.data, sizeof(data->pmm));

    return instance;
}

void felica_listener_free(FelicaListener* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_buffer);

    bit_buffer_free(instance->tx_buffer);
    bit_buffer_free(instance->rx_buffer);
    free(instance);
}

void felica_listener_set_callback(
    FelicaListener* listener,
    NfcGenericCallback callback,
    void* context) {
    UNUSED(listener);
    UNUSED(callback);
    UNUSED(context);
}

const FelicaData* felica_listener_get_data(const FelicaListener* instance) {
    furi_assert(instance);
    furi_assert(instance->data);

    return instance->data;
}

FelicaError felica_listener_frame_exchange( //TODO move this to listener_i.c file
    const FelicaListener* instance,
    const BitBuffer* tx_buffer) {
    furi_assert(instance);

    const size_t tx_bytes = bit_buffer_get_size_bytes(tx_buffer);
    furi_assert(tx_bytes <= bit_buffer_get_capacity_bytes(instance->tx_buffer) - FELICA_CRC_SIZE);

    felica_crc_append(instance->tx_buffer);

    FelicaError ret = FelicaErrorNone;

    do {
        NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
        //nfc_poller_trx(instance->nfc, instance->tx_buffer, instance->rx_buffer, fwt);
        if(error != NfcErrorNone) {
            //ret = felica_poller_process_error(error);
            break;
        }

        /*  bit_buffer_copy(rx_buffer, instance->rx_buffer);
        if(!felica_crc_check(instance->rx_buffer)) {
            ret = FelicaErrorWrongCrc;
            break;
        }

        felica_crc_trim(rx_buffer); */
    } while(false);

    return ret;
}

static uint8_t felica_listener_get_block_index(uint8_t number) {
    if(number >= FELICA_BLOCK_INDEX_RC && number < FELICA_BLOCK_INDEX_WCNT) {
        return number - 0x80 + FELICA_BLOCK_INDEX_REG + 1;
    } else if(number >= FELICA_BLOCK_INDEX_WCNT && number <= FELICA_BLOCK_INDEX_STATE) {
        return number - 0x90 + 9 + FELICA_BLOCK_INDEX_REG + 1;
    } else if(number == FELICA_BLOCK_INDEX_CRC_CHECK) {
        return number - 0x90 + 9 + FELICA_BLOCK_INDEX_REG + 2;
    }

    return number;
}

static bool felica_block_exists(uint8_t number) {
    bool exist = true;
    if(number > FELICA_BLOCK_INDEX_REG && number < FELICA_BLOCK_INDEX_RC) {
        exist = false;
    } else if(number > FELICA_BLOCK_INDEX_MC && number < FELICA_BLOCK_INDEX_WCNT) {
        exist = false;
    } else if(number > FELICA_BLOCK_INDEX_STATE && number < FELICA_BLOCK_INDEX_CRC_CHECK) {
        exist = false;
    } else if(number > FELICA_BLOCK_INDEX_CRC_CHECK) {
        exist = false;
    }

    /*  exist =
        !((number > FELICA_BLOCK_INDEX_REG && number < FELICA_BLOCK_INDEX_RC) ||
          (number > FELICA_BLOCK_INDEX_MC && number < FELICA_BLOCK_INDEX_WCNT) ||
          (number > FELICA_BLOCK_INDEX_STATE && number < FELICA_BLOCK_INDEX_CRC_CHECK) ||
          (number > FELICA_BLOCK_INDEX_CRC_CHECK)); */
    return exist;
}

static bool felica_authentication_required(const FelicaListener* instance, uint8_t block_number) {
    uint8_t* mc = instance->data->data.fs.mc.data;
    ///TODO: add also choice for write check
    uint16_t auth = *((uint16_t*)&mc[6]);

    bool bit = (auth & 1 << block_number) != 0;
    return bit;
}

static bool felica_validate_block_list(
    const FelicaListener* instance,
    const FelicaListenerRequest* const request,
    FelicaListenerReadCommandResponse* response) {
    uint8_t mac_a_pos = 0;
    bool mac_a_present = false, mac_present = false;
    for(uint8_t i = 0; i < request->header.block_count; i++) {
        FelicaBlockListElement item = request->list[i];
        if(item.service_code != 0) {
            response->SF1 = (1 << i);
            response->SF2 = 0xA3;
            return false;
        } else if(item.access_mode != 0) {
            response->SF1 = (1 << i);
            response->SF2 = 0xA7;
            return false;
        } else if(!felica_block_exists(item.block_number)) {
            response->SF1 = (1 << i);
            response->SF2 = 0xA8;
            return false;
        } else if(item.block_number == FELICA_BLOCK_INDEX_MAC) {
            mac_present = true;
        } else if(item.block_number == FELICA_BLOCK_INDEX_MAC_A) {
            if(!instance->rc_written) {
                response->SF1 = (1 << i);
                response->SF2 = 0xB2;
                return false;
            }
            if(!mac_a_present) {
                mac_a_present = true;
                mac_a_pos = i;
            }
        } else if(felica_authentication_required(instance, item.block_number)) {
            response->SF1 = (1 << i);
            response->SF2 = 0xB1;
            return false;
        }

        if(mac_a_present && mac_present) {
            response->SF1 = (1 << mac_a_pos);
            response->SF2 = 0xB0;
            return false;
        }
    }
    return true;
}

static bool felica_listener_validate_request_and_set_sf(
    const FelicaListener* instance,
    const FelicaListenerRequest* const request,
    FelicaListenerReadCommandResponse* response) {
    bool valid = false;
    do {
        if(request->header.service_num != 0x01) {
            response->SF1 = 0xFF;
            response->SF2 = 0xA1;
            break;
        }
        ///TODO: here must be another value for write request
        if(request->header.block_count < 0x01 || request->header.block_count > 0x04) {
            response->SF1 = 0xFF;
            response->SF2 = 0xA2;
            break;
        }

        if(request->header.service_code != FELICA_SERVICE_RO_ACCESS &&
           request->header.service_code != FELICA_SERVICE_RW_ACCESS) {
            response->SF1 = 0x01;
            response->SF2 = 0xA6;
            break;
        }

        /* for(uint8_t i = 0; i < request->header.block_count; i++) {
            FelicaBlockListElement item = request->list[i];
            if(item.service_code != 0) {
                response->SF1 = (1 << i);
                response->SF2 = 0xA3;
                list_valid = false;
                break;
            } else if(item.access_mode != 0) {
                response->SF1 = (1 << i);
                response->SF2 = 0xA7;
                list_valid = false;
                break;
            } else if(!felica_block_exists(item.block_number)) {
                response->SF1 = (1 << i);
                response->SF2 = 0xA8;
                list_valid = false;
                break;
            }
        } */
        if(!felica_validate_block_list(instance, request, response)) break;

        response->SF1 = 0x00;
        response->SF2 = 0x00;
        valid = true;
    } while(false);

    return valid;
}

static void felica_listener_command_handler_read(
    FelicaListener* instance,
    const FelicaListenerRequest* const request) {
    FURI_LOG_D(TAG, "Read cmd");

    FelicaListenerReadCommandResponse* resp = (FelicaListenerReadCommandResponse*)malloc(
        sizeof(FelicaCommandResponseHeader) + 1 +
        request->header.block_count * FELICA_DATA_BLOCK_SIZE);
    furi_check(resp);

    resp->response_code = 0x07;
    resp->idm = request->header.idm;
    resp->length = sizeof(FelicaCommandResponseHeader);

    if(felica_listener_validate_request_and_set_sf(instance, request, resp)) {
        resp->block_count = request->header.block_count;
        resp->length++;
    } else {
        resp->block_count = 0;
    }

    for(uint8_t i = 0; i < resp->block_count; i++) {
        FelicaBlockListElement item = request->list[i];
        uint8_t num = felica_listener_get_block_index(item.block_number);
        memcpy(
            &resp->data[num * FELICA_DATA_BLOCK_SIZE],
            &instance->data->data.dump[num * (FELICA_DATA_BLOCK_SIZE + 2) + 2],
            FELICA_DATA_BLOCK_SIZE);
        resp->length += FELICA_DATA_BLOCK_SIZE;
    }

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_bytes(instance->tx_buffer, (uint8_t*)resp, resp->length);
    free(resp);

    felica_listener_frame_exchange(
        instance,
        instance->tx_buffer); //TODO: Process return value of this function
}

static void felica_listener_command_handler_write(
    FelicaListener* instance,
    const FelicaListenerRequest* const request) {
    FURI_LOG_D(TAG, "Write cmd");
    UNUSED(request);
    UNUSED(instance);
}

typedef void (*FelicaListenerCommandHandler)(
    FelicaListener* instance,
    const FelicaListenerRequest* const request);

static FelicaListenerCommandHandler
    felica_listener_get_cmd_handler(const FelicaListenerRequest* const request) {
    FelicaListenerCommandHandler handler = NULL;
    switch(request->header.code) {
    case FELICA_CMD_READ_WITHOUT_ENCRYPTION:
        handler = felica_listener_command_handler_read;
        break;
    case FELICA_CMD_WRITE_WITHOUT_ENCRYPTION:
        handler = felica_listener_command_handler_write;
        break;
    default:
        break;
    }
    return handler;
}

static bool
    felica_listener_check_idm(const FelicaListener* instance, const FelicaIDm* request_idm) {
    const FelicaIDm* idm = &instance->data->idm;
    return memcmp(idm->data, request_idm->data, 8) == 0;
}

NfcCommand felica_listener_run(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolInvalid);
    furi_assert(event.event_data);

    FelicaListener* instance = context;
    NfcEvent* nfc_event = event.event_data;
    NfcCommand command = NfcCommandContinue;

    FURI_LOG_D(TAG, "Event: %d", nfc_event->type);

    if(nfc_event->type == NfcEventTypeListenerActivated) {
        instance->state = Felica_ListenerStateActivated;

        FURI_LOG_D(TAG, "Activated");
    } else if(nfc_event->type == NfcEventTypeFieldOff) {
        instance->state = Felica_ListenerStateIdle;
        FURI_LOG_D(TAG, "Field Off");
    } else if(nfc_event->type == NfcEventTypeRxEnd) {
        FURI_LOG_D(TAG, "Rx Done");
        FelicaListenerRequest* request =
            (FelicaListenerRequest*)bit_buffer_get_data(nfc_event->data.buffer);
        uint8_t size = bit_buffer_get_size_bytes(nfc_event->data.buffer) - 2;
        if(felica_listener_check_idm(instance, &request->header.idm) && request->length == size) {
            FelicaListenerCommandHandler handler = felica_listener_get_cmd_handler(request);
            if(handler != NULL) {
                handler(instance, request);
            }
        }
        bit_buffer_reset(nfc_event->data.buffer);
    }
    return command;
}

const NfcListenerBase nfc_listener_felica = {
    .alloc = (NfcListenerAlloc)felica_listener_alloc,
    .free = (NfcListenerFree)felica_listener_free,
    .set_callback = (NfcListenerSetCallback)felica_listener_set_callback,
    .get_data = (NfcListenerGetData)felica_listener_get_data,
    .run = (NfcListenerRun)felica_listener_run,
};