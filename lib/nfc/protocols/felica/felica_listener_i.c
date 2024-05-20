#include "felica_listener_i.h"

#include <nfc/helpers/felica_crc.h>

#define FELICA_CHECK_MC_VALUE

static uint32_t felica_wcnt_get_max_value(const FelicaData* data) {
    const uint8_t mc = data->data.fs.mc.data[2];

    if(mc != 0xFF && mc != 0x00) {
        furi_crash("Reserved value is forbidden");
    }
    return (mc == 0xFF) ? 0x00FFFFFF : 0x00FFFE00;
}

static bool felica_wcnt_check_warning_boundary(const FelicaData* data) {
    const uint8_t mc = data->data.fs.mc.data[2];
    const uint32_t* wcnt_ptr = (uint32_t*)data->data.fs.wcnt.data;
    bool res = false;
    if(mc == 0x00) {
        if(*wcnt_ptr > 0x00001027 && *wcnt_ptr < 0x00FFFDFF) {
            res = true;
        }
    }

    return res;
    //return ((mc == 00) && ((*wcnt_ptr > 0x00001027) && (*wcnt_ptr < 0x00FFFDFF)));
}

static bool felica_wcnt_check_error_boundary(const FelicaData* data) {
    const uint32_t wcnt_max = felica_wcnt_get_max_value(data);
    const uint32_t* wcnt_ptr = (const uint32_t*)data->data.fs.wcnt.data;

    return *wcnt_ptr != wcnt_max;
}

void felica_wcnt_increment(FelicaData* data) {
    const uint32_t wcnt_max = felica_wcnt_get_max_value(data);
    uint32_t* wcnt_ptr = (uint32_t*)data->data.fs.wcnt.data;
    if(*wcnt_ptr < wcnt_max) {
        *wcnt_ptr += 1;
    }
}

static void felica_wcnt_post_process(FelicaData* data) {
    uint32_t* wcnt_ptr = (uint32_t*)data->data.fs.wcnt.data;

    if((data->data.fs.mc.data[2] == 0x00) && (*wcnt_ptr > 0x00FFFE00)) {
        *wcnt_ptr = 0;
    }
}

bool felica_listener_check_idm(const FelicaListener* instance, const FelicaIDm* request_idm) {
    const FelicaIDm* idm = &instance->data->idm;
    return memcmp(idm->data, request_idm->data, 8) == 0;
}

void felica_listener_reset(FelicaListener* instance) {
    instance->auth.context.auth_status.internal = false;
    instance->auth.context.auth_status.external = false;
    instance->data->data.fs.state.data[0] = 0;
    instance->rc_written = false;
    memset(instance->auth.session_key.data, 0, FELICA_DATA_BLOCK_SIZE);

    memcpy(
        instance->data->data.fs.mc.data,
        instance->mc_shadow.data,
        FELICA_DATA_BLOCK_SIZE); ///TODO: repace this to mc_post_process

    felica_wcnt_post_process(instance->data);
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

static void felica_handler_read_block(
    FelicaListener* instance,
    const uint8_t block_number,
    const uint8_t resp_data_index,
    FelicaListenerReadCommandResponse* response) {
    UNUSED(instance);
    UNUSED(block_number);
    UNUSED(response);

    uint8_t num = felica_listener_get_block_index(block_number);
    memcpy(
        &response->data[resp_data_index * FELICA_DATA_BLOCK_SIZE],
        &instance->data->data.dump[num * (FELICA_DATA_BLOCK_SIZE + 2) + 2],
        FELICA_DATA_BLOCK_SIZE);
    response->header.length += FELICA_DATA_BLOCK_SIZE;
}

static void felica_handler_read_all_zeros(
    FelicaListener* instance,
    const uint8_t block_number,
    const uint8_t resp_data_index,
    FelicaListenerReadCommandResponse* response) {
    UNUSED(instance);
    UNUSED(block_number);

    memset(&response->data[resp_data_index * FELICA_DATA_BLOCK_SIZE], 0, FELICA_DATA_BLOCK_SIZE);
    response->header.length += FELICA_DATA_BLOCK_SIZE;
}

static void felica_handler_read_mac_a_block(
    FelicaListener* instance,
    const uint8_t block_number,
    const uint8_t resp_data_index,
    FelicaListenerReadCommandResponse* response) {
    if(resp_data_index != response->block_count - 1) {
        felica_handler_read_all_zeros(instance, block_number, resp_data_index, response);
        instance->mac_calc_start = resp_data_index + 1;
    } else {
        felica_calculate_mac_read(
            &instance->auth.des_context,
            instance->auth.session_key.data,
            instance->data->data.fs.rc.data,
            &instance->requested_blocks[instance->mac_calc_start],
            response->block_count - instance->mac_calc_start,
            &response->data[instance->mac_calc_start * FELICA_DATA_BLOCK_SIZE],
            instance->data->data.fs.mac_a.data);
        felica_handler_read_block(instance, block_number, resp_data_index, response);
    }
}

FelicaCommanReadBlockHandler felica_listener_get_read_block_handler(const uint8_t block_number) {
    FelicaCommanReadBlockHandler handler = felica_handler_read_block;

    if(block_number == FELICA_BLOCK_INDEX_RC || block_number == FELICA_BLOCK_INDEX_CK) {
        handler = felica_handler_read_all_zeros;
    } else if(block_number == FELICA_BLOCK_INDEX_MAC_A) {
        handler = felica_handler_read_mac_a_block;
    }

    return handler;
}

static bool felica_validate_read_block_list(
    const FelicaListener* instance,
    const FelicaListenerReadRequest* const request,
    FelicaCommandResponseHeader* response) {
    uint8_t mac_a_pos = 0;
    bool mac_a_present = false, mac_present = false;
    for(uint8_t i = 0; i < request->base.header.block_count; i++) {
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
             request->base.header.service_code != FELICA_SERVICE_RO_ACCESS)) {
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
            felica_block_requires_auth(instance, request->base.header.code, item.block_number) &&
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

bool felica_listener_validate_read_request_and_set_sf(
    const FelicaListener* instance,
    const FelicaListenerReadRequest* const request,
    FelicaCommandResponseHeader* resp_header) {
    bool valid = false;
    do {
        if(request->base.header.service_num != 0x01) {
            resp_header->SF1 = 0xFF;
            resp_header->SF2 = 0xA1;
            break;
        }
        if((request->base.header.code == FELICA_CMD_READ_WITHOUT_ENCRYPTION) &&
           (request->base.header.block_count < 0x01 || request->base.header.block_count > 0x04)) {
            resp_header->SF1 = 0xFF;
            resp_header->SF2 = 0xA2;
            break;
        }

        if(request->base.header.service_code != FELICA_SERVICE_RO_ACCESS &&
           request->base.header.service_code != FELICA_SERVICE_RW_ACCESS) {
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

static bool felica_validate_write_block_list(
    FelicaListener* instance,
    const FelicaListenerWriteRequest* const request,
    const FelicaListenerWriteBlockData* const data,
    FelicaListenerWriteCommandResponse* response) {
    bool write_with_mac = false;
    if(request->base.header.block_count == 2 &&
       request->list[1].block_number == FELICA_BLOCK_INDEX_MAC_A) {
        write_with_mac = true;
    } else if(request->base.header.block_count < 1 || request->base.header.block_count > 2) {
        response->SF1 = 0x02;
        response->SF2 = 0xB2;
        return false;
    }

    for(uint8_t i = 0; i < request->base.header.block_count; i++) {
        const FelicaBlockListElement* item = &request->list[i];
        if(felica_block_is_readonly(instance, item->block_number) ||
           (felica_block_requires_mac(instance, item->block_number) && !write_with_mac)) {
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
            felica_block_requires_auth(instance, request->base.header.code, item->block_number) &&
            !instance->auth.context.auth_status.external) {
            response->SF1 = 0x01;
            response->SF2 = 0xB1;
            return false;
        }
    }
    return true;
}

bool felica_listener_validate_write_request_and_set_sf(
    FelicaListener* instance,
    const FelicaListenerWriteRequest* const request,
    const FelicaListenerWriteBlockData* const data,
    FelicaListenerWriteCommandResponse* response) {
    UNUSED(data);
    bool valid = false;
    do {
        if(request->base.header.service_num != 0x01) {
            response->SF1 = 0xFF;
            response->SF2 = 0xA1;
            break;
        }

        if((request->base.header.code == FELICA_CMD_WRITE_WITHOUT_ENCRYPTION) &&
           (request->base.header.block_count < 0x01 || request->base.header.block_count > 0x02)) {
            response->SF1 = 0xFF;
            response->SF2 = 0xA2;
            break;
        }

        if(request->base.header.service_code != FELICA_SERVICE_RW_ACCESS) {
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

static void felica_handler_write_block(
    FelicaListener* instance,
    const uint8_t block_number,
    const FelicaBlockData* data_block) {
    uint8_t num = felica_listener_get_block_index(block_number);

    memcpy(
        &instance->data->data.dump[num * (FELICA_DATA_BLOCK_SIZE + 2) + 2],
        data_block->data,
        FELICA_DATA_BLOCK_SIZE);
}

static void felica_handler_write_rc_block(
    FelicaListener* instance,
    const uint8_t block_number,
    const FelicaBlockData* data_block) {
    felica_handler_write_block(instance, block_number, data_block);

    felica_calculate_session_key(
        &instance->auth.des_context,
        instance->data->data.fs.ck.data,
        instance->data->data.fs.rc.data,
        instance->auth.session_key.data);
    instance->rc_written = true;
}

static void felica_handler_write_reg_block(
    FelicaListener* instance,
    const uint8_t block_number,
    const FelicaBlockData* data_block) {
    UNUSED(block_number);

    typedef struct {
        uint32_t A;
        uint32_t B;
        uint64_t C;
    } FELICA_REG_BLOCK;

    FELICA_REG_BLOCK* Reg = (FELICA_REG_BLOCK*)instance->data->data.fs.reg.data;
    FELICA_REG_BLOCK* RegNew = (FELICA_REG_BLOCK*)data_block->data;

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

static void felica_handler_write_mc_block(
    FelicaListener* instance,
    const uint8_t block_number,
    const FelicaBlockData* data_block) {
    UNUSED(block_number);

    bool mc_bits_permission = felica_get_mc_bit(instance, 0, 15);
    bool mc_system_block_permission = instance->data->data.fs.mc.data[2] == 0xFF;
    for(uint8_t i = 0; i < FELICA_DATA_BLOCK_SIZE; i++) {
        if((i <= 1) && mc_bits_permission) {
            instance->mc_shadow.data[i] &= data_block->data[i];
        } else if((i >= 2 && i <= 5) && (mc_system_block_permission)) {
            instance->mc_shadow.data[i] = data_block->data[i];
        } else if((i >= 6 && i <= 12) && mc_bits_permission) {
            instance->mc_shadow.data[i] |= data_block->data[i];
        } else if(i >= 13 && i <= 15) {
            instance->mc_shadow.data[i] = 0;
        }
    }
}

static void felica_handler_write_state_block(
    FelicaListener* instance,
    const uint8_t block_number,
    const FelicaBlockData* data_block) {
    felica_handler_write_block(instance, block_number, data_block);
    bool state = instance->data->data.fs.state.data[0] == 0x01;
    instance->auth.context.auth_status.external = state;
    instance->auth.context.auth_status.internal = state;
}

FelicaCommandWriteBlockHandler
    felica_listener_get_write_block_handler(const uint8_t block_number) {
    FelicaCommandWriteBlockHandler handler = felica_handler_write_block;
    switch(block_number) {
    case FELICA_BLOCK_INDEX_RC:
        handler = felica_handler_write_rc_block;
        break;
    case FELICA_BLOCK_INDEX_REG:
        handler = felica_handler_write_reg_block;
        break;
    case FELICA_BLOCK_INDEX_MC:
        handler = felica_handler_write_mc_block;
        break;
    case FELICA_BLOCK_INDEX_STATE:
        handler = felica_handler_write_state_block;
        break;
    default:
        handler = felica_handler_write_block;
        break;
    }
    return handler;
}

const FelicaListenerWriteBlockData* felica_listener_get_write_request_data_pointer(
    const FelicaListenerGenericRequest* const generic_request) {
    return (const FelicaListenerWriteBlockData*)((uint8_t*)generic_request +
                                                 sizeof(FelicaListenerGenericRequest) +
                                                 sizeof(FelicaBlockListElement) *
                                                     generic_request->header.block_count);
}

static FelicaError felica_listener_process_error(NfcError error) {
    switch(error) {
    case NfcErrorNone:
        return FelicaErrorNone;
    case NfcErrorTimeout:
        return FelicaErrorTimeout;
    default:
        return FelicaErrorNotPresent;
    }
}

FelicaError
    felica_listener_frame_exchange(const FelicaListener* instance, const BitBuffer* tx_buffer) {
    furi_assert(instance);

    const size_t tx_bytes = bit_buffer_get_size_bytes(tx_buffer);
    furi_assert(tx_bytes <= bit_buffer_get_capacity_bytes(instance->tx_buffer) - FELICA_CRC_SIZE);

    felica_crc_append(instance->tx_buffer);

    FelicaError ret = FelicaErrorNone;

    do {
        NfcError error = nfc_listener_tx(instance->nfc, instance->tx_buffer);
        if(error != NfcErrorNone) {
            ret = felica_listener_process_error(error);
            break;
        }
    } while(false);

    return ret;
}