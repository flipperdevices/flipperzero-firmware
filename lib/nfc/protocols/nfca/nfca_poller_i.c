#include "nfca_poller_i.h"

#include <furi.h>

#define TAG "NFCA"

static NfcaError nfca_poller_process_error(NfcError error) {
    NfcaError ret = NfcaErrorNone;
    if(error == NfcErrorNone) {
        ret = NfcaErrorNone;
    } else if(error == NfcErrorTimeout) {
        ret = NfcaErrorTimeout;
    } else {
        ret = NfcaErrorNotPresent;
    }
    return ret;
}

static NfcaError nfca_poller_prepare_trx(NfcaPoller* instance) {
    NfcaError ret = NfcaErrorNone;

    if(instance->state == NfcaPollerStateIdle) {
        ret = nfca_poller_async_activate(instance, NULL);
    }

    return ret;
}

static NfcaError nfca_poller_standart_frame_exchange(
    NfcaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    uint16_t tx_bytes = bit_buffer_get_size_bytes(tx_buffer);
    furi_assert(tx_bytes <= bit_buffer_get_capacity_bytes(instance->tx_buffer) - 2);

    bit_buffer_copy(instance->tx_buffer, tx_buffer);
    nfca_append_crc_buff(instance->tx_buffer);
    NfcaError ret = NfcaErrorNone;

    do {
        NfcError error = nfc_trx(instance->nfc, instance->tx_buffer, instance->rx_buffer, fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }
        if(!nfca_check_and_trim_crc_buff(rx_buffer, instance->rx_buffer)) {
            ret = NfcaErrorWrongCrc;
            break;
        }
    } while(false);

    return ret;
}

NfcaError nfca_poller_config(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_buffer == NULL);
    furi_assert(instance->rx_buffer == NULL);

    instance->tx_buffer = bit_buffer_alloc(NFCA_POLLER_MAX_BUFFER_SIZE);
    instance->tx_buffer = bit_buffer_alloc(NFCA_POLLER_MAX_BUFFER_SIZE);

    nfc_config(instance->nfc, NfcModeNfcaPoller);
    nfc_set_guard_time_us(instance->nfc, NFCA_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, NFCA_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, NFCA_POLL_POLL_MIN_US);
    instance->config_state = NfcaPollerConfigStateDone;

    return NfcaErrorNone;
}

NfcaError nfca_poller_reset(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->tx_buffer);
    furi_assert(instance->rx_buffer);

    instance->callback = NULL;
    instance->context = NULL;
    memset(&instance->col_res, 0, sizeof(NfcaPollerColRes));

    bit_buffer_free(instance->tx_buffer);
    instance->tx_buffer = NULL;
    bit_buffer_free(instance->rx_buffer);
    instance->rx_buffer = NULL;

    instance->config_state = NfcaPollerConfigStateIdle;
    instance->state = NfcaPollerStateIdle;

    return NfcaErrorNone;
}

NfcaError nfca_poller_check_presence(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);

    uint16_t rx_bits = 0;
    NfcError error = NfcErrorNone;
    NfcaError ret = NfcaErrorNone;
    do {
        error = nfc_iso13444a_short_frame(
            instance->nfc,
            NfcIso14443aShortFrameSensReq,
            (uint8_t*)&instance->col_res.sens_resp,
            sizeof(instance->col_res.sens_resp),
            &rx_bits,
            NFCA_FDT_LISTEN_FC);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }
        if(rx_bits != 8 * sizeof(instance->col_res.sens_resp)) {
            ret = NfcaErrorCommunication;
            break;
        }
    } while(false);

    return ret;
}

NfcaError nfca_poller_halt(NfcaPoller* instance) {
    furi_assert(instance);
    furi_assert(instance->nfc);
    furi_assert(instance->tx_buffer);

    uint8_t halt_cmd[2] = {0x50, 0x00};
    bit_buffer_copy_bytes(instance->tx_buffer, halt_cmd, sizeof(halt_cmd));

    nfca_poller_standart_frame_exchange(
        instance, instance->tx_buffer, instance->rx_buffer, NFCA_FDT_LISTEN_FC);

    instance->state = NfcaPollerStateIdle;
    return NfcaErrorNone;
}

NfcaError nfca_poller_async_activate(NfcaPoller* instance, NfcaData* nfca_data) {
    furi_assert(instance);
    furi_assert(instance->nfc);
    furi_assert(instance->tx_buffer);
    furi_assert(instance->rx_buffer);

    // Reset Nfca poller state
    memset(&instance->col_res, 0, sizeof(instance->col_res));
    memset(instance->data, 0, sizeof(NfcaData));

    // Halt if necessary
    if(instance->state != NfcaPollerStateIdle) {
        nfca_poller_halt(instance);
        instance->state = NfcaPollerStateIdle;
    }

    NfcError error = NfcErrorNone;
    NfcaError ret = NfcaErrorNone;
    uint16_t rx_bits = 0;
    uint16_t tx_bits = 0;

    bool activated = false;
    do {
        error = nfc_iso13444a_short_frame(
            instance->nfc,
            NfcIso14443aShortFrameSensReq,
            (uint8_t*)&instance->col_res.sens_resp,
            sizeof(instance->col_res.sens_resp),
            &rx_bits,
            NFCA_FDT_LISTEN_FC);
        if(error != NfcErrorNone) {
            ret = NfcaErrorNotPresent;
            break;
        }
        if(rx_bits != 8 * sizeof(instance->col_res.sens_resp)) {
            FURI_LOG_W(TAG, "Wrong response: %d", rx_bits);
            ret = NfcaErrorCommunication;
            break;
        }
        memcpy(
            instance->data->atqa,
            &instance->col_res.sens_resp,
            sizeof(instance->col_res.sel_resp));

        instance->state = NfcaPollerStateColResInProgress;
        instance->col_res.cascade_level = 0;
        instance->col_res.state = NfcaPollerColResStateStateNewCascade;

        while(instance->state == NfcaPollerStateColResInProgress) {
            if(instance->col_res.state == NfcaPollerColResStateStateNewCascade) {
                instance->col_res.sdd_req.sel_cmd =
                    NFCA_POLLER_SEL_CMD(instance->col_res.cascade_level);
                instance->col_res.sdd_req.sel_par = NFCA_POLLER_SEL_PAR(2, 0);
                tx_bits = 16;
                error = nfc_iso13444a_sdd_frame(
                    instance->nfc,
                    (uint8_t*)&instance->col_res.sdd_req,
                    tx_bits,
                    (uint8_t*)&instance->col_res.sdd_resp,
                    sizeof(instance->col_res.sdd_resp),
                    &rx_bits,
                    NFCA_FDT_LISTEN_FC);
                if(error != NfcErrorNone) {
                    FURI_LOG_E(TAG, "Sdd request failed: %d", error);
                    instance->state = NfcaPollerStateColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                if(rx_bits != 5 * 8) {
                    FURI_LOG_E(TAG, "Sdd response wrong length: %d bits", rx_bits);
                    instance->state = NfcaPollerStateColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                // TODO BCC check here
                instance->col_res.state = NfcaPollerColResStateStateSelectCascade;
            } else if(instance->col_res.state == NfcaPollerColResStateStateSelectCascade) {
                instance->col_res.sel_req.sel_cmd =
                    NFCA_POLLER_SEL_CMD(instance->col_res.cascade_level);
                instance->col_res.sel_req.sel_par = NFCA_POLLER_SEL_PAR(7, 0);
                memcpy(
                    instance->col_res.sel_req.nfcid,
                    instance->col_res.sdd_resp.nfcid,
                    sizeof(instance->col_res.sdd_resp.nfcid));
                instance->col_res.sel_req.bcc = instance->col_res.sdd_resp.bss;
                // Todo remove after Nfc handles timings
                furi_delay_ms(10);
                ret = nfca_poller_send_standart_frame(
                    instance,
                    (uint8_t*)&instance->col_res.sel_req,
                    8 * sizeof(instance->col_res.sel_req),
                    (uint8_t*)&instance->col_res.sel_resp,
                    sizeof(NfcaSelResp),
                    &rx_bits,
                    NFCA_FDT_LISTEN_FC);
                if(ret != NfcaErrorNone) {
                    FURI_LOG_E(TAG, "Sel request failed: %d", error);
                    instance->state = NfcaPollerStateColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                if(rx_bits != 8 * sizeof(instance->col_res.sel_resp)) {
                    FURI_LOG_E(TAG, "Sel response wrong length: %d bits", rx_bits);
                    instance->state = NfcaPollerStateColResFailed;
                    ret = NfcaErrorColResFailed;
                    break;
                }
                FURI_LOG_T(TAG, "Sel resp: %02X", instance->col_res.sel_resp.sak);
                if(instance->col_res.sel_req.nfcid[0] == NFCA_POLLER_SDD_CL) {
                    // Copy part of UID
                    memcpy(
                        &instance->data->uid[instance->data->uid_len],
                        &instance->col_res.sel_req.nfcid[1],
                        3);
                    instance->data->uid_len += 3;
                    instance->col_res.cascade_level++;
                    instance->col_res.state = NfcaPollerColResStateStateNewCascade;
                } else {
                    FURI_LOG_T(TAG, "Col resolution complete");
                    instance->data->sak = instance->col_res.sel_resp.sak;
                    memcpy(
                        &instance->data->uid[instance->data->uid_len],
                        &instance->col_res.sel_req.nfcid[0],
                        4);
                    instance->data->uid_len += 4;
                    instance->col_res.state = NfcaPollerColResStateStateSuccess;
                    instance->state = NfcaPollerStateActivated;
                }
            }
        }

        activated = (instance->state == NfcaPollerStateActivated);
    } while(false);

    if(activated && nfca_data) {
        *nfca_data = *instance->data;
    }

    return ret;
}

// static uint16_t
//     nfca_data_to_bitstream(uint8_t* data, uint8_t* parity, uint16_t bits, uint8_t* bitstream) {
//     furi_assert(data);
//     furi_assert(parity);
//     furi_assert(bitstream);

//     uint8_t next_par_bit = 0;
//     uint16_t curr_bit_pos = 0;
//     uint16_t bytes = bits / 8;

//     for(size_t i = 0; i < bytes; i++) {
//         next_par_bit = FURI_BIT(parity[i / 8], 7 - (i % 8));
//         if(curr_bit_pos % 8 == 0) {
//             bitstream[curr_bit_pos / 8] = data[i];
//             curr_bit_pos += 8;
//             bitstream[curr_bit_pos / 8] = next_par_bit;
//             curr_bit_pos++;
//         } else {
//             bitstream[curr_bit_pos / 8] |= data[i] << (curr_bit_pos % 8);
//             bitstream[curr_bit_pos / 8 + 1] = data[i] >> (8 - curr_bit_pos % 8);
//             bitstream[curr_bit_pos / 8 + 1] |= next_par_bit << (curr_bit_pos % 8);
//             curr_bit_pos += 9;
//         }
//     }
//     return curr_bit_pos;
// }

// static uint16_t
//     nfca_bitstream_to_data(uint8_t* bitstream, uint16_t bits, uint8_t* data, uint8_t* parity) {
//     uint32_t data_bits = 0;
//     uint8_t curr_byte = 0;
//     uint16_t bit_processed = 0;

//     if(bits < 8) {
//         data[0] = bitstream[0];
//         data_bits = bits;
//     } else if(bits % 9 != 0) {
//         data_bits = 0;
//     } else {
//         memset(parity, 0, bits / 9);
//         while(bit_processed < bits) {
//             data[curr_byte] = bitstream[bit_processed / 8] >> (bit_processed % 8);
//             data[curr_byte] |= bitstream[bit_processed / 8 + 1] << (8 - bit_processed % 8);
//             parity[curr_byte / 8] |= FURI_BIT(bitstream[bit_processed / 8 + 1], bit_processed % 8)
//                                      << (7 - curr_byte % 8);
//             bit_processed += 9;
//             curr_byte++;
//         }
//         data_bits = curr_byte * 8;
//     }

//     return data_bits;
// }

NfcaError nfca_poller_txrx_custom_parity(
    NfcaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    NfcaError ret = NfcaErrorNone;
    NfcError error = NfcErrorNone;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        // buff->tx_bits = nfca_data_to_bitstream(tx_data, tx_parity, tx_bits, buff->tx_data);

        error = nfc_trx_custom_parity(instance->nfc, tx_buffer, rx_buffer, fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }

        // uint16_t rx_bytes = buff->rx_bits / 9;
        // if(rx_buff_size < rx_bytes) {
        //     ret = NfcaErrorBufferOverflow;
        //     break;
        // }

        // *rx_bits = nfca_bitstream_to_data(buff->rx_data, buff->rx_bits, rx_data, rx_parity);
    } while(false);

    return ret;
}

NfcaError nfca_poller_txrx(
    NfcaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    NfcaError ret = NfcaErrorNone;
    NfcError error = NfcErrorNone;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        error = nfc_trx(instance->nfc, tx_buffer, rx_buffer, fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }
    } while(false);

    return ret;
}

NfcaError nfca_poller_send_standart_frame(
    NfcaPoller* instance,
    const BitBuffer* tx_buffer,
    BitBuffer* rx_buffer,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buffer);
    furi_assert(rx_buffer);

    NfcaError ret = NfcaErrorNone;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        ret = nfca_poller_standart_frame_exchange(instance, tx_buffer, rx_buffer, fwt);
        if(ret != NfcaErrorNone) break;
    } while(false);

    return ret;
}
