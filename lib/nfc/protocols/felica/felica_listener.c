#include "felica_listener_i.h"

#include "nfc/protocols/nfc_listener_base.h"
#include <nfc/helpers/felica_crc.h>
#include <furi_hal_nfc.h>

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

    mbedtls_des3_init(&instance->auth.des_context);
    nfc_set_fdt_listen_fc(instance->nfc, FELICA_FDT_LISTEN_FC);
    ///TODO: Remove this after MAC calc testing
    /*     instance->data->data.fs.rc.data[0] = 0xF1;
    instance->data->data.fs.rc.data[1] = 0x87;
    instance->data->data.fs.rc.data[2] = 0x5A;
    instance->data->data.fs.rc.data[3] = 0x01;
    instance->data->data.fs.rc.data[4] = 0xF9;
    instance->data->data.fs.rc.data[5] = 0xB2;
    instance->data->data.fs.rc.data[6] = 0x9E;
    instance->data->data.fs.rc.data[7] = 0x4C;
    instance->data->data.fs.rc.data[8] = 0x06;
    instance->data->data.fs.rc.data[9] = 0xA1;
    instance->data->data.fs.rc.data[10] = 0xCE;
    instance->data->data.fs.rc.data[11] = 0xC4;
    instance->data->data.fs.rc.data[12] = 0x16;
    instance->data->data.fs.rc.data[13] = 0x55;
    instance->data->data.fs.rc.data[14] = 0x85;
    instance->data->data.fs.rc.data[15] = 0xCF;

    instance->rc_written = true;
    felica_calculate_session_key(
        &instance->auth.des_context,
        instance->data->data.fs.ck.data,
        instance->data->data.fs.rc.data,
        instance->auth.session_key.data); */
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

static bool
    felica_get_mc_bit(const FelicaListener* instance, uint8_t byte_index, uint8_t bit_number) {
    uint8_t* mc = instance->data->data.fs.mc.data;

    uint16_t flags = *((uint16_t*)&mc[byte_index]);
    bool bit = (flags & (1 << bit_number)) != 0;
    return bit;
}

static bool felica_block_requires_auth(
    const FelicaListener* instance,
    uint8_t command,
    uint8_t block_number) {
    uint8_t mc_flag_index = command; //(command == FELICA_CMD_READ_WITHOUT_ENCRYPTION) ? 6 : 8;
    return felica_get_mc_bit(instance, mc_flag_index, block_number);
}

static bool felica_block_is_readonly(const FelicaListener* instance, uint8_t block_number) {
    uint8_t mc_flag_index = 0;
    ///TODO: Add more checks for other blocks not only first 15
    if(block_number <= FELICA_BLOCK_INDEX_REG) {
        return !felica_get_mc_bit(instance, mc_flag_index, block_number);
    } else if(
        (block_number >= FELICA_BLOCK_INDEX_ID && block_number <= FELICA_BLOCK_INDEX_SER_C) ||
        (block_number >= FELICA_BLOCK_INDEX_CKV && block_number <= FELICA_BLOCK_INDEX_CK)) {
        return instance->data->data.fs.mc.data[2] == 0xFF; ///TODO: replace this with some macro
    } else
        return false;
}

static bool felica_block_requires_mac(const FelicaListener* instance, uint8_t block_number) {
    uint8_t mc_flag_index = 10;
    return felica_get_mc_bit(instance, mc_flag_index, block_number);
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

static bool felica_validate_write_block_list(
    FelicaListener* instance,
    const FelicaListenerWriteRequest* const request,
    const FelicaListenerWriteBlockData* const data,
    FelicaListenerWriteCommandResponse* response) {
    instance->write_with_mac = false;
    if(request->header.block_count == 2 &&
       request->list[1].block_number == FELICA_BLOCK_INDEX_MAC_A) {
        instance->write_with_mac = true;
    } else if(request->header.block_count < 1 || request->header.block_count > 2) {
        response->SF1 = 0x02;
        response->SF2 = 0xB2;
        return false;
    }
    ///зачем мне цикл, если может быть всего 2 блока???
    for(uint8_t i = 0; i < request->header.block_count; i++) {
        //const FelicaListenerWriteBlockListElement* dat = &request->data[i];
        //const FelicaBlockListElement* item = &dat->item;
        const FelicaBlockListElement* item = &request->list[i];
        if(felica_block_is_readonly(instance, item->block_number) ||
           (felica_block_requires_mac(instance, item->block_number) &&
            !instance->write_with_mac)) {
            response->SF1 = 0x01;
            response->SF2 = 0xA8;
            return false;
        }

        if(item->service_code != 0) {
            response->SF1 = (1 << i);
            response->SF2 = 0xA3;
            return false;
        } else if(item->access_mode != 0) {
            response->SF1 = (1 << i);
            response->SF2 = 0xA7;
            return false;
        } else if(!felica_block_exists(item->block_number)) {
            response->SF1 = (1 << i);
            response->SF2 = 0xA8;
            return false;
        } else if((i == 1) && (item->block_number == FELICA_BLOCK_INDEX_MAC_A)) {
            uint8_t calculated_mac[8];
            felica_calculate_mac_write(
                &instance->auth.des_context,
                instance->auth.session_key.data,
                instance->data->data.fs.rc.data,
                data->blocks[1].data + 8,
                data->blocks[0].data,
                calculated_mac);

            if(!instance->rc_written || (memcmp(calculated_mac, data->blocks[1].data, 8) != 0) ||
               !felica_wcnt_check_error_boundary(instance->data)) {
                response->SF1 = 0x02;
                response->SF2 = 0xB2;
                return false;
            }
        } else if(
            felica_block_requires_auth(instance, request->header.code, item->block_number) &&
            !instance->auth.context.auth_status.external) {
            response->SF1 = 0x01;
            response->SF2 = 0xB1;
            return false;
        }
    }
    return true;
}

static bool felica_listener_validate_write_request_and_set_sf(
    FelicaListener* instance,
    const FelicaListenerWriteRequest* const request,
    const FelicaListenerWriteBlockData* const data,
    FelicaListenerWriteCommandResponse* response) {
    bool valid = false;
    do {
        if(request->header.service_num != 0x01) {
            response->SF1 = 0xFF;
            response->SF2 = 0xA1;
            break;
        }

        if((request->header.code == FELICA_CMD_WRITE_WITHOUT_ENCRYPTION) &&
           (request->header.block_count < 0x01 || request->header.block_count > 0x02)) {
            response->SF1 = 0xFF;
            response->SF2 = 0xA2;
            break;
        }

        if(request->header.service_code != FELICA_SERVICE_RW_ACCESS) {
            response->SF1 = 0x01;
            response->SF2 = 0xA6;
            break;
        }

        if(!felica_validate_write_block_list(instance, request, data, response)) break;

        if(felica_wcnt_check_warning_boundary(instance->data)) {
            response->SF1 = 0xFF;
            response->SF2 = 0x71;
        } else {
            response->SF1 = 0x00;
            response->SF2 = 0x00;
        }
        valid = true;
    } while(false);
    return valid;
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

    resp->header.response_code = 0x07;
    resp->header.idm = request->header.idm;
    resp->header.length = sizeof(FelicaCommandResponseHeader);

    if(felica_listener_validate_read_request_and_set_sf(instance, request, &resp->header)) {
        resp->block_count = request->header.block_count;
        resp->header.length++;
    } else {
        resp->block_count = 0;
    }

    uint8_t mac_calc_start = 0;
    uint8_t blocks[4] = {};
    for(uint8_t i = 0; i < resp->block_count; i++) {
        const FelicaBlockListElement* item = &request->list[i];
        blocks[i] = item->block_number;
        bool skip_block = false;
        if(item->block_number == FELICA_BLOCK_INDEX_MAC_A) {
            if(i != resp->block_count - 1) {
                memset(&resp->data[i * FELICA_DATA_BLOCK_SIZE], 0, FELICA_DATA_BLOCK_SIZE);
                mac_calc_start = i + 1;
                skip_block = true;
                resp->header.length += FELICA_DATA_BLOCK_SIZE;
            } else {
                felica_calculate_mac_read(
                    &instance->auth.des_context,
                    instance->auth.session_key.data,
                    instance->data->data.fs.rc.data,
                    &blocks[mac_calc_start],
                    resp->block_count - mac_calc_start,
                    &resp->data[mac_calc_start * FELICA_DATA_BLOCK_SIZE],
                    instance->data->data.fs.mac_a.data);
            }
        } else if(
            item->block_number == FELICA_BLOCK_INDEX_RC ||
            item->block_number == FELICA_BLOCK_INDEX_CK) {
            memset(&resp->data[i * FELICA_DATA_BLOCK_SIZE], 0, FELICA_DATA_BLOCK_SIZE);
            skip_block = true;
            resp->header.length += FELICA_DATA_BLOCK_SIZE;
        }

        if(!skip_block) {
            uint8_t num = felica_listener_get_block_index(item->block_number);
            memcpy(
                &resp->data[i * FELICA_DATA_BLOCK_SIZE],
                &instance->data->data.dump[num * (FELICA_DATA_BLOCK_SIZE + 2) + 2],
                FELICA_DATA_BLOCK_SIZE);
            resp->header.length += FELICA_DATA_BLOCK_SIZE;
        }
    }

    bit_buffer_reset(instance->tx_buffer);
    bit_buffer_append_bytes(instance->tx_buffer, (uint8_t*)resp, resp->header.length);
    free(resp);
    ///TODO: Process return value of this function
    felica_listener_frame_exchange(instance, instance->tx_buffer);
}

static void felica_listener_write_mc_block(
    FelicaListener* instance,
    const FelicaListenerWriteRequest* request,
    const FelicaBlockData* data,
    FelicaListenerWriteCommandResponse* response) {
    UNUSED(request);
    UNUSED(response);

    bool mc_bits_permission = felica_get_mc_bit(instance, 0, 15);
    ///TODO: replace this with some function
    bool mc_system_block_permission = instance->data->data.fs.mc.data[2] == 0xFF;
    for(uint8_t i = 0; i < FELICA_DATA_BLOCK_SIZE; i++) {
        if((i <= 1) && mc_bits_permission) {
            instance->mc_shadow.data[i] &= data->data[i];
        } else if((i >= 2 && i <= 5) && (mc_system_block_permission)) {
            instance->mc_shadow.data[i] = data->data[i];
        } else if((i >= 6 && i <= 12) && mc_bits_permission) {
            instance->mc_shadow.data[i] |= data->data[i];
        } else if(i >= 13 && i <= 15) {
            instance->mc_shadow.data[i] = 0;
        } /* else {
            instance->mc_shadow.data[i] = data->data[i];
        } */
    }
}

static void felica_listener_write_reg_block(
    FelicaListener* instance,
    const FelicaListenerWriteRequest* request,
    const FelicaBlockData* data,
    FelicaListenerWriteCommandResponse* response) {
    UNUSED(request);
    UNUSED(response);

    typedef struct {
        uint32_t A;
        uint32_t B;
        uint64_t C;
    } FELICA_REG_BLOCK;

    FELICA_REG_BLOCK* Reg = (FELICA_REG_BLOCK*)instance->data->data.fs.reg.data;
    FELICA_REG_BLOCK* RegNew = (FELICA_REG_BLOCK*)data;

    if(Reg->A >= RegNew->A) {
        Reg->A = RegNew->A;
    }

    if(Reg->B >= RegNew->B) {
        Reg->B = RegNew->B;
    }

    if((Reg->A >= RegNew->A) && (Reg->B >= RegNew->B)) {
        Reg->C = RegNew->C;
    }
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

    resp->response_code = 0x09;
    resp->idm = request->header.idm;
    resp->length = sizeof(FelicaListenerWriteCommandResponse);
    if(felica_listener_validate_write_request_and_set_sf(instance, request, data_ptr, resp)) {
        for(uint8_t i = 0; i < request->header.block_count; i++) {
            const FelicaBlockListElement* item = &request->list[i];
            uint8_t num = felica_listener_get_block_index(item->block_number);

            if(item->block_number == FELICA_BLOCK_INDEX_REG) {
                felica_listener_write_reg_block(instance, request, &data_ptr->blocks[i], resp);
            } else if(item->block_number == FELICA_BLOCK_INDEX_MC) {
                felica_listener_write_mc_block(instance, request, &data_ptr->blocks[i], resp);
            } else {
                memcpy(
                    &instance->data->data.dump[num * (FELICA_DATA_BLOCK_SIZE + 2) + 2],
                    data_ptr->blocks[i].data,
                    FELICA_DATA_BLOCK_SIZE);

                if(item->block_number == FELICA_BLOCK_INDEX_STATE) {
                    instance->auth.context.auth_status.external =
                        instance->data->data.fs.state.data[0] == 0x01;

                    instance->auth.context.auth_status.internal =
                        instance->auth.context.auth_status.external;
                } else if(item->block_number == FELICA_BLOCK_INDEX_RC) {
                    felica_calculate_session_key(
                        &instance->auth.des_context,
                        instance->data->data.fs.ck.data,
                        instance->data->data.fs.rc.data,
                        instance->auth.session_key.data);
                    instance->rc_written = true;
                }
            }
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