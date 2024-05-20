#include "felica_listener_i.h"

#include "nfc/protocols/nfc_listener_base.h"
#include <nfc/helpers/felica_crc.h>
#include <furi_hal_nfc.h>

#define FELICA_LISTENER_MAX_BUFFER_SIZE (128)
#define FELICA_LISTENER_RESPONSE_CODE_READ (0x07)
#define FELICA_LISTENER_RESPONSE_CODE_WRITE (0x09)

#define TAG "FelicaListener"

FelicaListener* felica_listener_alloc(Nfc* nfc, FelicaData* data) {
    furi_assert(nfc);
    furi_assert(data);

    FelicaListener* instance = malloc(sizeof(FelicaListener));
    instance->nfc = nfc;
    instance->data = data;
    instance->tx_buffer = bit_buffer_alloc(FELICA_LISTENER_MAX_BUFFER_SIZE);
    instance->rx_buffer = bit_buffer_alloc(FELICA_LISTENER_MAX_BUFFER_SIZE);

    mbedtls_des3_init(&instance->auth.des_context);
    nfc_set_fdt_listen_fc(instance->nfc, FELICA_FDT_LISTEN_FC);

    //------------------------------
    memcpy(instance->mc_shadow.data, instance->data->data.fs.mc.data, FELICA_DATA_BLOCK_SIZE);
    instance->data->data.fs.state.data[0] = 0;
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

FelicaError felica_listener_frame_exchange( ///TODO: move this to listener_i.c file
    const FelicaListener* instance,
    const BitBuffer* tx_buffer) {
    furi_assert(instance);

    const size_t tx_bytes = bit_buffer_get_size_bytes(tx_buffer);
    furi_assert(tx_bytes <= bit_buffer_get_capacity_bytes(instance->tx_buffer) - FELICA_CRC_SIZE);

    felica_crc_append(instance->tx_buffer);

    FelicaError ret = FelicaErrorNone;

    do {
        NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
        if(error != NfcErrorNone) {
            //ret = felica_poller_process_error(error);
            break;
        }

    } while(false);

    return ret;
}

static bool felica_validate_read_block_list(
    const FelicaListener* instance,
    const FelicaListenerReadRequest* const request,
    FelicaCommandResponseHeader* response) {
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
        } else if(
            !felica_block_exists(item.block_number) ||
            (felica_block_is_readonly(instance, item.block_number) &&
             request->header.service_code != FELICA_SERVICE_RO_ACCESS)) {
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
        } else if(
            felica_block_requires_auth(instance, request->header.code, item.block_number) &&
            !instance->auth.context.auth_status.external) {
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

static bool felica_listener_validate_read_request_and_set_sf(
    const FelicaListener* instance,
    const FelicaListenerReadRequest* const request,
    FelicaCommandResponseHeader* resp_header) {
    bool valid = false;
    do {
        if(request->header.service_num != 0x01) {
            resp_header->SF1 = 0xFF;
            resp_header->SF2 = 0xA1;
            break;
        }
        if((request->header.code == FELICA_CMD_READ_WITHOUT_ENCRYPTION) &&
           (request->header.block_count < 0x01 || request->header.block_count > 0x04)) {
            resp_header->SF1 = 0xFF;
            resp_header->SF2 = 0xA2;
            break;
        }

        if(request->header.service_code != FELICA_SERVICE_RO_ACCESS &&
           request->header.service_code != FELICA_SERVICE_RW_ACCESS) {
            resp_header->SF1 = 0x01;
            resp_header->SF2 = 0xA6;
            break;
        }

        if(!felica_validate_read_block_list(instance, request, resp_header)) break;

        resp_header->SF1 = 0x00;
        resp_header->SF2 = 0x00;
        valid = true;
    } while(false);

    return valid;
}

static void felica_listener_command_handler_read(
    FelicaListener* instance,
    const FelicaListenerGenericRequest* const generic_request) {
    const FelicaListenerReadRequest* request = (FelicaListenerReadRequest*)generic_request;
    FURI_LOG_D(TAG, "Read cmd");

    FelicaListenerReadCommandResponse* resp = malloc(
        sizeof(FelicaCommandResponseHeader) + 1 +
        request->header.block_count * FELICA_DATA_BLOCK_SIZE);
    furi_check(resp);

    resp->header.response_code = FELICA_LISTENER_RESPONSE_CODE_READ;
    resp->header.idm = request->header.idm;
    resp->header.length = sizeof(FelicaCommandResponseHeader);

    if(felica_listener_validate_read_request_and_set_sf(instance, request, &resp->header)) {
        resp->block_count = request->header.block_count;
        resp->header.length++;
    } else {
        resp->block_count = 0;
    }

    instance->mac_calc_start = 0;
    memset(instance->requested_blocks, 0, 4);
    for(uint8_t i = 0; i < resp->block_count; i++) {
        const FelicaBlockListElement* item = &request->list[i];
        instance->requested_blocks[i] = item->block_number;
        FelicaCommanReadBlockHandler handler =
            felica_listener_get_read_block_handler(item->block_number);

        handler(instance, item->block_number, i, resp);
    }

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_bytes(instance->tx_buffer, (uint8_t*)resp, resp->header.length);
    free(resp);
    ///TODO: Process return value of this function
    felica_listener_frame_exchange(instance, instance->tx_buffer);
}

static void felica_listener_command_handler_write(
    FelicaListener* instance,
    const FelicaListenerGenericRequest* const generic_request) {
    FURI_LOG_D(TAG, "Write cmd");

    const FelicaListenerWriteRequest* request = (FelicaListenerWriteRequest*)generic_request;
    const FelicaListenerWriteBlockData* data_ptr =
        (const FelicaListenerWriteBlockData*)((uint8_t*)generic_request +
                                              sizeof(FelicaListenerGenericRequest) +
                                              sizeof(FelicaBlockListElement) *
                                                  request->header.block_count);

    FelicaListenerWriteCommandResponse* resp =
        (FelicaListenerWriteCommandResponse*)malloc(sizeof(FelicaListenerWriteCommandResponse));
    furi_check(resp);

    resp->response_code = FELICA_LISTENER_RESPONSE_CODE_WRITE;
    resp->idm = request->header.idm;
    resp->length = sizeof(FelicaListenerWriteCommandResponse);

    if(felica_listener_validate_write_request_and_set_sf(instance, request, data_ptr, resp)) {
        for(uint8_t i = 0; i < request->header.block_count; i++) {
            const FelicaBlockListElement* item = &request->list[i];
            FelicaCommandWriteBlockHandler handler =
                felica_listener_get_write_block_handler(item->block_number);

            handler(instance, item->block_number, &data_ptr->blocks[i]);
        }
        felica_wcnt_increment(instance->data);
    }

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_bytes(instance->tx_buffer, (uint8_t*)resp, resp->length);
    free(resp);

    ///TODO: Process return value of this function
    felica_listener_frame_exchange(instance, instance->tx_buffer);
}

typedef void (*FelicaListenerCommandHandler)(
    FelicaListener* instance,
    const FelicaListenerGenericRequest* const generic_request);

static FelicaListenerCommandHandler felica_listener_get_cmd_handler(uint8_t command_code) {
    FelicaListenerCommandHandler handler = NULL;
    switch(command_code) {
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

    if(nfc_event->type == NfcEventTypeFieldOn) {
        FURI_LOG_D(TAG, "Field On");
    } else if(nfc_event->type == NfcEventTypeListenerActivated) {
        instance->state = Felica_ListenerStateActivated;
        FURI_LOG_D(TAG, "Activated");
    } else if(nfc_event->type == NfcEventTypeFieldOff) {
        instance->state = Felica_ListenerStateIdle;
        FURI_LOG_D(TAG, "Field Off");
        ///TODO: Move this to a separate functions aka felica_listener_reset
        instance->auth.context.auth_status.internal = false;
        instance->auth.context.auth_status.external = false;
        instance->data->data.fs.state.data[0] = 0;
        instance->rc_written = false;
        memset(instance->auth.session_key.data, 0, FELICA_DATA_BLOCK_SIZE);
        //-------------------------------------------------------
        memcpy(
            instance->data->data.fs.mc.data,
            instance->mc_shadow.data,
            FELICA_DATA_BLOCK_SIZE); ///TODO: replace this to mc_post_process
        felica_wcnt_post_process(instance->data);
    } else if(nfc_event->type == NfcEventTypeRxEnd) {
        FURI_LOG_D(TAG, "Rx Done");
        FelicaListenerGenericRequest* request =
            (FelicaListenerGenericRequest*)bit_buffer_get_data(nfc_event->data.buffer);
        uint8_t size = bit_buffer_get_size_bytes(nfc_event->data.buffer) - 2;

        if(felica_listener_check_idm(instance, &request->header.idm) && request->length == size) {
            FelicaListenerCommandHandler handler =
                felica_listener_get_cmd_handler(request->header.code);
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