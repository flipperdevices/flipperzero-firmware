#include "as_card_worker.h"
#include <nfc/protocols/iso14443_4a/iso14443_4a_listener.h>

#define TAG "AsCardWorker"

static uint8_t CMD_14A_WTX[4] = {0xf2, 0x01, 0x91, 0x40};

#define NFCA_CRC_INIT (0x6363)
uint16_t nfca_get_crc16(uint8_t* buff, uint16_t len) {
    uint16_t crc = NFCA_CRC_INIT;
    uint8_t byte = 0;

    for(uint8_t i = 0; i < len; i++) {
        byte = buff[i];
        byte ^= (uint8_t)(crc & 0xff);
        byte ^= byte << 4;
        crc = (crc >> 8) ^ (((uint16_t)byte) << 8) ^ (((uint16_t)byte) << 3) ^
              (((uint16_t)byte) >> 4);
    }

    return crc;
}
void nfca_append_crc16(uint8_t* buff, uint16_t len) {
    uint16_t crc = nfca_get_crc16(buff, len);
    buff[len] = (uint8_t)crc;
    buff[len + 1] = (uint8_t)(crc >> 8);
}

void as_card_worker_change_state(AsCardWorker* as_card_worker, AsCardWorkerState state) {
    as_card_worker->state = state;
    if(as_card_worker->callback) as_card_worker->callback(state, as_card_worker->nfc_relay);
}

AsCardWorker* as_card_worker_alloc(NfcRelay* nfc_relay) {
    furi_assert(nfc_relay);
    FURI_LOG_D(TAG, "as_card_worker_alloc");

    AsCardWorker* as_card_worker = malloc(sizeof(AsCardWorker));
    as_card_worker->thread =
        furi_thread_alloc_ex("AsCardWorkerThread", 8192, as_card_worker_task, as_card_worker);
    as_card_worker->running = false;
    as_card_worker->comm = comm_alloc(nfc_relay->config);
    as_card_worker->nfc_relay = nfc_relay;
    as_card_worker->callback = NULL;
    as_card_worker->nfc = nfc_alloc();
    as_card_worker->dev_data = iso14443_4a_alloc();
    as_card_worker->bitbuffer = bit_buffer_alloc(256);
    as_card_worker->buff_tx = malloc(256);
    // TODO: Make the following two configurable
    as_card_worker->delayus = 3500;
    CMD_14A_WTX[1] = 1;
    nfca_append_crc16(&CMD_14A_WTX[0], 2);

    return as_card_worker;
}

void as_card_worker_free(AsCardWorker* as_card_worker) {
    furi_assert(as_card_worker);

    if(as_card_worker->dev_data) {
        iso14443_4a_free(as_card_worker->dev_data);
        as_card_worker->dev_data = NULL;
    }
    if(as_card_worker->bitbuffer) {
        bit_buffer_free(as_card_worker->bitbuffer);
        as_card_worker->bitbuffer = NULL;
    }
    if(as_card_worker->buff_tx) {
        free(as_card_worker->buff_tx);
        as_card_worker->buff_tx = NULL;
    }
    if(as_card_worker->nfc) {
        nfc_free(as_card_worker->nfc);
        as_card_worker->nfc = NULL;
    }

    comm_free(as_card_worker->comm);
    furi_thread_free(as_card_worker->thread);
    free(as_card_worker);
}

void as_card_worker_stop(AsCardWorker* as_card_worker) {
    furi_assert(as_card_worker);

    as_card_worker->running = false;
    comm_deinit(as_card_worker->comm);
    as_card_worker->callback = NULL;
    if(as_card_worker->listener) {
        nfc_listener_stop(as_card_worker->listener);
        nfc_listener_free(as_card_worker->listener);
        as_card_worker->listener = NULL;
    }

    furi_thread_join(as_card_worker->thread);
}

void as_card_worker_start(AsCardWorker* as_card_worker, AsCardWorkerCallback callback) {
    FURI_LOG_D(TAG, "as_card_worker_start");
    furi_assert(as_card_worker);
    as_card_worker->running = true;

    comm_init(as_card_worker->comm);
    as_card_worker->callback = callback;
    furi_thread_start(as_card_worker->thread);
}

NfcCommand emu_callback(NfcGenericEvent event, void* context) {
    furi_assert(context);
    furi_assert(event.protocol == NfcProtocolIso14443_4a);
    furi_assert(event.event_data);

    AsCardWorker* as_card_worker = context;
    Iso14443_4aListenerEvent* iso14443_4a_event = event.event_data;
    //FURI_LOG_T(TAG, "iso14443_4a_event->type: %d", iso14443_4a_event->type);
    if(iso14443_4a_event->type != Iso14443_4aListenerEventTypeReceivedData) {
        return NfcCommandContinue;
    }
    if(bit_buffer_get_size_bytes(iso14443_4a_event->data->buffer) == 0) {
        FURI_LOG_E(TAG, "bit_buffer_get_size_bytes(iso14443_4a_event->data->buffer) == 0");
        return NfcCommandContinue;
    }
    trace_bit_buffer_hexdump(TAG, "Emu Card RX", iso14443_4a_event->data->buffer);
    uint8_t rx_bytes = bit_buffer_get_size_bytes(iso14443_4a_event->data->buffer);
    const uint8_t* buff_rx = bit_buffer_get_data(iso14443_4a_event->data->buffer);
    uint8_t pcb = buff_rx[0];

    if((pcb & 0xfe) == 0xb2) {
        FURI_LOG_D(TAG, "Recv R(NACK)");
        if(as_card_worker->state == AsCardWorkerStateWaitApduResp) {
            FURI_LOG_D(TAG, "NACK as WTX Resp");
            memcpy(as_card_worker->buff_tx, &CMD_14A_WTX[0], sizeof(CMD_14A_WTX));
            as_card_worker->buff_tx[0] &= 0xfe;
            as_card_worker->buff_tx[0] |= pcb & 0x1;
            furi_delay_ms(as_card_worker->delayus / 1000);
            bit_buffer_copy_bytes(
                as_card_worker->bitbuffer, as_card_worker->buff_tx, sizeof(CMD_14A_WTX));
            nfc_iso14443a_listener_tx_custom_parity(
                as_card_worker->nfc, as_card_worker->bitbuffer);
            return NfcCommandContinue;
        } else {
            FURI_LOG_D(TAG, "send 0xa2");
            as_card_worker->buff_tx[0] = 0xa2;
            bit_buffer_copy_bytes(as_card_worker->bitbuffer, as_card_worker->buff_tx, 1);
            nfc_iso14443a_listener_tx_custom_parity(
                as_card_worker->nfc, as_card_worker->bitbuffer);
            return NfcCommandContinue;
        }
    } else if((pcb & 0xfe) == 0xc2) {
        FURI_LOG_D(TAG, "Recv S(DESELECT)");
    } else if((pcb & 0xfe) == 0xf2) {
        if(as_card_worker->state == AsCardWorkerStateWaitApduResp) {
            FURI_LOG_D(TAG, "Recv WTX Resp");
            memcpy(as_card_worker->buff_tx, &CMD_14A_WTX[0], sizeof(CMD_14A_WTX));
            as_card_worker->buff_tx[0] &= 0xfe;
            as_card_worker->buff_tx[0] |= pcb & 0x1;
            furi_delay_ms(as_card_worker->delayus / 1000);
            bit_buffer_copy_bytes(
                as_card_worker->bitbuffer, as_card_worker->buff_tx, sizeof(CMD_14A_WTX));
            nfc_iso14443a_listener_tx_custom_parity(
                as_card_worker->nfc, as_card_worker->bitbuffer);

            return NfcCommandContinue;
        } else if(as_card_worker->state == AsCardWorkerStateGetApduResp) {
            FURI_LOG_D(TAG, "Recv WTX Resp, Ready to send Response back");
            memcpy(
                as_card_worker->buff_tx, as_card_worker->apdu_buf, as_card_worker->apdu_buf_len);
            nfca_append_crc16(&as_card_worker->buff_tx[0], as_card_worker->apdu_buf_len);

            bit_buffer_copy_bytes(
                as_card_worker->bitbuffer,
                as_card_worker->buff_tx,
                as_card_worker->apdu_buf_len + 2);
            //nfc_iso14443a_listener_tx_custom_parity(emu_card_worker->nfc, emu_card_worker->bitbuffer);
            trace_bit_buffer_hexdump(TAG, "Emu Card TX", as_card_worker->bitbuffer);
            NfcError error = nfc_listener_tx(as_card_worker->nfc, as_card_worker->bitbuffer);
            if(error != NfcErrorNone) {
                FURI_LOG_E(TAG, "Tx error: %d", error);
            }

            as_card_worker->state = AsCardWorkerStateWaitApduReq;
            return NfcCommandContinue;
        } else {
            FURI_LOG_E(TAG, "Recv WTX Resp but we hasn't request");
        }
    } else if((pcb & 0xfe) == 0x2) {
        // I-Block ignore the block number at LSB
        FURI_LOG_T(TAG, "Recv I-Block");
        NfcRelayPacket* packet;
        packet = packet_alloc_data(NfcRelayPacketApduReq, rx_bytes, &buff_rx[0]);
        comm_write_packet(as_card_worker->comm, packet);
        as_card_worker_change_state(as_card_worker, AsCardWorkerStateWaitApduResp);
        as_card_worker->apdu_buf[0] = buff_rx[0];
        free(packet);

        bit_buffer_copy_bytes(as_card_worker->bitbuffer, &CMD_14A_WTX[0], sizeof(CMD_14A_WTX));
        nfc_iso14443a_listener_tx_custom_parity(as_card_worker->nfc, as_card_worker->bitbuffer);

        as_card_worker->state = AsCardWorkerStateWaitApduResp;
        return NfcCommandContinue;
    } else {
        FURI_LOG_E(TAG, "Something unknown");
        return NfcCommandContinue;
    }

    return NfcCommandContinue;
}

// TODO: NfcRelayPakcetErr
int32_t as_card_worker_task(void* context) {
    furi_assert(context);
    AsCardWorker* as_card_worker = context;
    NfcRelayPacket* recv_packet = NULL;

    FURI_LOG_D(TAG, "Send Ping Packet");
    comm_send_pingpong(as_card_worker->comm, NfcRelayPacketPing, NfcRelayAsCard);

    while(1) {
        if(!as_card_worker->running) break;
        if(as_card_worker->state == AsCardWorkerStateWaitPong) {
            if(comm_wait_pong(as_card_worker->comm, NfcRelayAsReader, NfcRelayAsCard)) {
                FURI_LOG_D(TAG, "comm_wait_pong succ, change state");

                as_card_worker_change_state(as_card_worker, AsCardWorkerStateWaitNfcDevData);
            } else {
                continue;
            }
        } else if(as_card_worker->state == AsCardWorkerStateWaitNfcDevData) {
            recv_packet = comm_wait_packet(as_card_worker->comm, NfcRelayPacketNfcDevData);
            if(!recv_packet) continue;
            // TODO: not grace, but do not familiar with FlipperFormat type
            if(recv_packet->len > sizeof(SerializedIso14443_4a) &&
               recv_packet->len == ((SerializedIso14443_4a*)recv_packet->buf)->len_t1_tk +
                                       sizeof(SerializedIso14443_4a)) {
                iso14443_4a_reset(as_card_worker->dev_data);
                iso14443_4a_deserialize(
                    as_card_worker->dev_data, (SerializedIso14443_4a*)recv_packet->buf);
                free(recv_packet);
                recv_packet = NULL;
                as_card_worker_change_state(as_card_worker, AsCardWorkerStateWaitApduReq);
                //if(dev_data->iso14443_3a_data->uid_len > 7) {
                //    FURI_LOG_E(TAG, "UID Length > 7, ST25R3916 not supported??");
                //    // TODO
                //    FURI_LOG_E(TAG, "Maybe it is suppported Now?");
                //    //dev_data.uid_len = 7;
                //}
                FURI_LOG_D(TAG, "Start Emulating");
                as_card_worker->listener = nfc_listener_alloc(
                    as_card_worker->nfc, NfcProtocolIso14443_4a, as_card_worker->dev_data);
                nfc_listener_start(as_card_worker->listener, emu_callback, as_card_worker);

                FURI_LOG_D(TAG, "Started Emulating");
            } else {
                FURI_LOG_D(TAG, "Recv NfcDevData with wrong size");
                free(recv_packet);
                recv_packet = NULL;
                // TODO: change state or not?
            }

        } else if(as_card_worker->state == AsCardWorkerStateWaitApduReq) {
            //FURI_LOG_T(TAG, "AsCardWorkerStateWaitApduReq");
            furi_delay_us(10);
        } else if(as_card_worker->state == AsCardWorkerStateWaitApduResp) {
            //FURI_LOG_D(TAG, "AsCardWorkerStateWaitApduResp");
            recv_packet = comm_wait_packet(as_card_worker->comm, NfcRelayPacketApduResp);
            if(!recv_packet) {
                continue;
            }
            // leave first pcb byte
            as_card_worker->apdu_buf_len = recv_packet->len + 1;
            memcpy(&as_card_worker->apdu_buf[1], recv_packet->buf, recv_packet->len);
            as_card_worker->state = AsCardWorkerStateGetApduResp;
            FURI_LOG_D(
                TAG,
                "Recv NfcRelayPacketApduResp, change as_card_worker state to AsCardWorkerStateGetApduResp");
            free(recv_packet);
            recv_packet = NULL;
        } else {
            recv_packet = comm_read_packet(as_card_worker->comm);
            if(!recv_packet) {
                continue;
            }
            FURI_LOG_E(TAG, "Not Yet");
            free(recv_packet);
            recv_packet = NULL;
        }
    }

    FURI_LOG_D(TAG, "as_card_worker stop running");

    return 0;
}
