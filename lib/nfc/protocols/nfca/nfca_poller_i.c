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
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(rx_data);
    furi_assert(rx_bits);
    furi_assert(tx_bits >= 8);
    furi_assert(instance->buff);

    NfcPollerBuffer* buff = instance->buff;
    uint16_t tx_bytes = tx_bits / 8;
    furi_assert(tx_bytes <= buff->tx_data_size - 2);

    memcpy(buff->tx_data, tx_data, tx_bytes);
    nfca_append_crc(buff->tx_data, tx_bytes);
    NfcaError ret = NfcaErrorNone;

    do {
        NfcError error = nfc_trx(
            instance->nfc,
            buff->tx_data,
            tx_bits + 16,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }
        if(buff->rx_bits < 8) {
            rx_data[0] = buff->rx_data[0];
            *rx_bits = buff->rx_bits;
            ret = NfcaErrorWrongCrc;
            break;
        } else if(buff->rx_bits < 3 * 8) {
            uint16_t rx_bytes = buff->rx_bits / 8;
            memcpy(rx_data, buff->rx_data, rx_bytes);
            *rx_bits = buff->rx_bits;
            ret = NfcaErrorWrongCrc;
            break;
        } else {
            uint16_t rx_bytes = buff->rx_bits / 8;
            if(rx_bytes - 2 > rx_data_size) {
                ret = NfcaErrorBufferOverflow;
                break;
            }

            if(!nfca_check_crc(buff->rx_data, rx_bytes)) {
                memcpy(rx_data, buff->rx_data, rx_bytes);
                *rx_bits = rx_bytes * 8;
                ret = NfcaErrorWrongCrc;
                break;
            }

            memcpy(rx_data, buff->rx_data, rx_bytes - 2);
            *rx_bits = (rx_bytes - 2) * 8;
        }
    } while(false);

    return ret;
}

NfcaError nfca_poller_config(NfcaPoller* instance) {
    furi_assert(instance);

    instance->buff =
        nfc_poller_buffer_alloc(NFCA_POLLER_MAX_BUFFER_SIZE, NFCA_POLLER_MAX_BUFFER_SIZE);

    nfc_config(instance->nfc, NfcModeNfcaPoller);
    nfc_set_guard_time_us(instance->nfc, NFCA_GUARD_TIME_US);
    nfc_set_fdt_poll_fc(instance->nfc, NFCA_FDT_POLL_FC);
    nfc_set_fdt_poll_poll_us(instance->nfc, NFCA_POLL_POLL_MIN_US);

    return NfcaErrorNone;
}

NfcaError nfca_poller_reset(NfcaPoller* instance) {
    furi_assert(instance);

    instance->callback = NULL;
    instance->context = NULL;
    memset(&instance->col_res, 0, sizeof(NfcaPollerColRes));
    instance->state = NfcaPollerStateIdle;
    nfc_poller_buffer_free(instance->buff);
    instance->buff = NULL;

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
    furi_assert(instance->buff);

    NfcPollerBuffer* buff = instance->buff;
    buff->tx_data[0] = 0x50;
    buff->tx_data[1] = 0x00;
    buff->tx_bits = 16;

    nfca_poller_standart_frame_exchange(
        instance,
        buff->tx_data,
        buff->tx_bits,
        buff->rx_data,
        buff->rx_data_size,
        &buff->rx_bits,
        NFCA_FDT_LISTEN_FC);
    instance->state = NfcaPollerStateIdle;
    return NfcaErrorNone;
}

NfcaError nfca_poller_async_activate(NfcaPoller* instance, NfcaData* nfca_data) {
    furi_assert(instance);
    furi_assert(instance->nfc);

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
                    instance->state = NfcaPollerStateColResSuccess;
                }
            }
        }

        if(instance->state == NfcaPollerStateColResSuccess) {
            // Check whether ATS is available
            if(instance->data->sak & (1U << 5)) {
                // Send RATS & receive ATS
                instance->iso_protocol.ats_req.cmd = 0xe0;
                instance->iso_protocol.ats_req.param = 0x80;

                ret = nfca_poller_send_standart_frame(
                    instance,
                    (uint8_t*)&instance->iso_protocol.ats_req,
                    8 * sizeof(instance->iso_protocol.ats_req),
                    (uint8_t*)&instance->iso_protocol.ats_resp,
                    8 * sizeof(instance->iso_protocol.ats_resp),
                    &rx_bits,
                    NFCA_FDT_LISTEN_FC);

                if(ret != NfcaErrorNone) {
                    FURI_LOG_E(TAG, "Ats request failed: %d", error);
                    instance->state = NfcaPollerStateIsoProtocolFailed;
                    ret = NfcaErrorCommunication;
                    break;
                }

                if(rx_bits != 8 * sizeof(instance->iso_protocol.ats_resp)) {
                    FURI_LOG_E(TAG, "Ats response wrong length: %d bits", rx_bits);
                    instance->state = NfcaPollerStateIsoProtocolFailed;
                    ret = NfcaErrorCommunication;
                    break;
                }
            }

            instance->state = NfcaPollerStateActivated;
        }

        activated = (instance->state == NfcaPollerStateActivated);
    } while(false);

    if(activated && nfca_data) {
        *nfca_data = *instance->data;
    }

    return ret;
}

static uint16_t
    nfca_data_to_bitstream(uint8_t* data, uint8_t* parity, uint16_t bits, uint8_t* bitstream) {
    furi_assert(data);
    furi_assert(parity);
    furi_assert(bitstream);

    uint8_t next_par_bit = 0;
    uint16_t curr_bit_pos = 0;
    uint16_t bytes = bits / 8;

    for(size_t i = 0; i < bytes; i++) {
        next_par_bit = FURI_BIT(parity[i / 8], 7 - (i % 8));
        if(curr_bit_pos % 8 == 0) {
            bitstream[curr_bit_pos / 8] = data[i];
            curr_bit_pos += 8;
            bitstream[curr_bit_pos / 8] = next_par_bit;
            curr_bit_pos++;
        } else {
            bitstream[curr_bit_pos / 8] |= data[i] << (curr_bit_pos % 8);
            bitstream[curr_bit_pos / 8 + 1] = data[i] >> (8 - curr_bit_pos % 8);
            bitstream[curr_bit_pos / 8 + 1] |= next_par_bit << (curr_bit_pos % 8);
            curr_bit_pos += 9;
        }
    }
    return curr_bit_pos;
}

static uint16_t
    nfca_bitstream_to_data(uint8_t* bitstream, uint16_t bits, uint8_t* data, uint8_t* parity) {
    uint32_t data_bits = 0;
    uint8_t curr_byte = 0;
    uint16_t bit_processed = 0;

    if(bits < 8) {
        data[0] = bitstream[0];
        data_bits = bits;
    } else if(bits % 9 != 0) {
        data_bits = 0;
    } else {
        memset(parity, 0, bits / 9);
        while(bit_processed < bits) {
            data[curr_byte] = bitstream[bit_processed / 8] >> (bit_processed % 8);
            data[curr_byte] |= bitstream[bit_processed / 8 + 1] << (8 - bit_processed % 8);
            parity[curr_byte / 8] |= FURI_BIT(bitstream[bit_processed / 8 + 1], bit_processed % 8)
                                     << (7 - curr_byte % 8);
            bit_processed += 9;
            curr_byte++;
        }
        data_bits = curr_byte * 8;
    }

    return data_bits;
}

NfcaError nfca_poller_txrx_custom_parity(
    NfcaPoller* instance,
    uint8_t* tx_data,
    uint8_t* tx_parity,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint8_t* rx_parity,
    uint16_t rx_buff_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(tx_parity);
    furi_assert(rx_data);
    furi_assert(rx_parity);
    furi_assert(rx_bits);

    NfcaError ret = NfcaErrorNone;
    NfcError error = NfcErrorNone;
    NfcPollerBuffer* buff = instance->buff;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        buff->tx_bits = nfca_data_to_bitstream(tx_data, tx_parity, tx_bits, buff->tx_data);

        error = nfc_trx_custom_parity(
            instance->nfc,
            buff->tx_data,
            buff->tx_bits,
            buff->rx_data,
            buff->rx_data_size,
            &buff->rx_bits,
            fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }

        uint16_t rx_bytes = buff->rx_bits / 9;
        if(rx_buff_size < rx_bytes) {
            ret = NfcaErrorBufferOverflow;
            break;
        }

        *rx_bits = nfca_bitstream_to_data(buff->rx_data, buff->rx_bits, rx_data, rx_parity);
    } while(false);

    return ret;
}

NfcaError nfca_poller_txrx(
    NfcaPoller* instance,
    uint8_t* tx_buff,
    uint16_t tx_bits,
    uint8_t* rx_buff,
    uint16_t rx_buff_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_buff);
    furi_assert(rx_buff);
    furi_assert(rx_bits);

    NfcaError ret = NfcaErrorNone;
    NfcError error = NfcErrorNone;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        error = nfc_trx(instance->nfc, tx_buff, tx_bits, rx_buff, rx_buff_size, rx_bits, fwt);
        if(error != NfcErrorNone) {
            ret = nfca_poller_process_error(error);
            break;
        }
    } while(false);

    return ret;
}

NfcaError nfca_poller_send_standart_frame(
    NfcaPoller* instance,
    uint8_t* tx_data,
    uint16_t tx_bits,
    uint8_t* rx_data,
    uint16_t rx_data_size,
    uint16_t* rx_bits,
    uint32_t fwt) {
    furi_assert(instance);
    furi_assert(tx_data);
    furi_assert(rx_data);
    furi_assert(rx_bits);
    furi_assert(tx_bits >= 8);

    NfcaError ret = NfcaErrorNone;

    do {
        ret = nfca_poller_prepare_trx(instance);
        if(ret != NfcaErrorNone) break;

        ret = nfca_poller_standart_frame_exchange(
            instance, tx_data, tx_bits, rx_data, rx_data_size, rx_bits, fwt);
        if(ret != NfcaErrorNone) break;
    } while(false);

    return ret;
}
