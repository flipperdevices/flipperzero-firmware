#include "u2f_nfc.h"
#include "furi_hal.h"

#define TAG "U2F_NFC"

// TODO: determine actual max payload length
#define U2F_NFC_MAX_PAYLOAD_LEN (7680)

#define U2F_NFC_MAX_PAYLOAD_PER_PACKET (256)

typedef enum {
    WorkerEvtReserved = (1 << 0),
    WorkerEvtStop = (1 << 1),
} WorkerEvtFlags;

typedef enum {
    StateUnselected,
    StateSelected,
    StateSending,
} U2fNfcState;

struct U2fNfc {
    FuriThread* thread;
    U2fData* u2f_instance;
    U2fNfcState state;
    uint8_t payload[U2F_NFC_MAX_PAYLOAD_LEN];
    uint16_t payload_len;
    uint16_t payload_cursor;
};

static bool u2f_nfc_callback(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len,
    uint32_t* data_type,
    void* context) {
    U2fNfc* u2f_nfc = context;
    *data_type = FURI_HAL_NFC_TXRX_DEFAULT;
    buff_tx[0] = 0x02;
    *buff_tx_len = 0;
    FURI_LOG_D(TAG, "Something detected of length %d", buff_rx_len);
    if(buff_rx_len >= 24) {
        uint8_t pcb = buff_rx[0];
        uint8_t cla = buff_rx[1];
        uint8_t ins = buff_rx[2];
        FURI_LOG_D(TAG, "pcb: 0x%02x cla: 0x%02x ins: 0x%02x", pcb, cla, ins);
        if((pcb & ~0x03) != 0) {
            buff_tx[1] = 0x68;
            buff_tx[2] = 0x00;
            *buff_tx_len = 24;
            FURI_LOG_E(TAG, "pcb 0x%02x not supported", pcb);
            return true;
        }
        if(cla != 0x00) {
            buff_tx[1] = 0x68;
            buff_tx[2] = 0x00;
            *buff_tx_len = 24;
            FURI_LOG_D(TAG, "cla 0x%02x not supported", cla);
            return true;
        }
        if(u2f_nfc->state == StateUnselected && ins == 0xa4) {
            if(memcmp(&buff_rx[3], "\x04\x00\x08\xa0\x00\x00\x06\x47\x2f\x00\x01", 11) != 0) {
                buff_tx[1] = 0x6A;
                buff_tx[2] = 0x82;
                *buff_tx_len = 24;
                FURI_LOG_D(TAG, "Unknown AID");
                return true;
            }
            memcpy(buff_tx, "\x02U2F_V2\x90\x00", 9);
            FURI_LOG_D(TAG, "Card selected");
            *buff_tx_len = 72;
            u2f_nfc->state = StateSelected;
        } else if(u2f_nfc->state == StateSelected && ins != 0 && (ins & ~0x03) == 0) {
            buff_tx[0] = 0x02;
            uint16_t max_resp_len = U2F_NFC_MAX_PAYLOAD_PER_PACKET;
//            uint16_t max_resp_len = buff_rx[6 + buff_rx[5]];
//            if(max_resp_len == 0) {
//                max_resp_len = U2F_NFC_MAX_PAYLOAD_PER_PACKET;
//            }
            FURI_LOG_D(TAG, "Expected response size: %03x or %03x or %03x", buff_rx[6 + buff_rx[5]], buff_rx[5 + buff_rx[5]], buff_rx[7 + buff_rx[5]]);
            u2f_nfc->payload_len = (buff_rx_len / 8) - 1;
            memcpy(u2f_nfc->payload, &buff_rx[1], u2f_nfc->payload_len);
            u2f_confirm_user_present(
                u2f_nfc
                    ->u2f_instance); // TODO: Check on discord whether this is normal behaviour for NFC
            u2f_nfc->payload_len =
                u2f_msg_parse(u2f_nfc->u2f_instance, u2f_nfc->payload, u2f_nfc->payload_len);

            if(u2f_nfc->payload_len > max_resp_len) {
                memcpy(&buff_tx[1], u2f_nfc->payload, max_resp_len);
                buff_tx[1 + max_resp_len] = 0x61;
                uint16_t remaining_len = u2f_nfc->payload_len - max_resp_len;
                if(remaining_len >= U2F_NFC_MAX_PAYLOAD_PER_PACKET) {
                    buff_tx[2 + max_resp_len] = 0x00;
                } else {
                    buff_tx[2 + max_resp_len] = remaining_len - 2;  // Do not include the 0x9000 at the end in this count
                }
                *buff_tx_len = 8 + 8 * max_resp_len + 16;
                u2f_nfc->payload_cursor = max_resp_len;
                u2f_nfc->state = StateSending;
            } else {
                memcpy(&buff_tx[1], u2f_nfc->payload, u2f_nfc->payload_len);
                *buff_tx_len = 8 + 8 * u2f_nfc->payload_len;
            }
            FURI_LOG_D(
                TAG,
                "Request sent of p1: %02x, p2: %02x, len: %02x",
                buff_rx[3],
                buff_rx[4],
                buff_rx[5]);
            return true;
        } else if(u2f_nfc->state == StateSending && ins == 0xC0) {
            buff_tx[0] = 0x02;
            uint16_t max_resp_len = buff_rx[6 + buff_rx[5]];
            if(max_resp_len == 0) {
                max_resp_len = U2F_NFC_MAX_PAYLOAD_PER_PACKET;
            }

            uint16_t remaining_len = u2f_nfc->payload_len - u2f_nfc->payload_cursor;

            if(remaining_len > max_resp_len) {
                memcpy(&buff_tx[1], &u2f_nfc->payload[u2f_nfc->payload_cursor], max_resp_len);
                remaining_len -= max_resp_len;
                if(remaining_len >= U2F_NFC_MAX_PAYLOAD_PER_PACKET) {
                    buff_tx[2 + max_resp_len] = 0x00;
                } else {
                    buff_tx[2 + max_resp_len] = remaining_len - 2;  // Do not include the 0x9000 at the end in this count
                }
                *buff_tx_len = 8 + 8 * max_resp_len + 16;
                u2f_nfc->payload_cursor += max_resp_len;
                u2f_nfc->state = StateSending;
            } else {
                memcpy(&buff_tx[1], &u2f_nfc->payload[u2f_nfc->payload_cursor], remaining_len);
                *buff_tx_len = 8 + 8 * remaining_len;
                u2f_nfc->state = StateSelected;
            }
        } else {
            buff_tx[1] = 0x69;
            buff_tx[2] = 0x00;
            *buff_tx_len = 216;
            FURI_LOG_D(TAG, "Unknown instruction");
            return true;
        }
    } else {
        FURI_LOG_D(TAG, "Unknown short message");
        return false;
    }
    return buff_rx_len > 0;
}

static int32_t u2f_nfc_worker(void* context) {
    U2fNfc* u2f_nfc = context;
    //    FuriHalNfcTxRxContext tx_rx = {};
    FURI_LOG_D(TAG, "Init");
    while(furi_hal_nfc_is_busy()) {
        furi_delay_ms(10);
    }
    FuriHalNfcDevData nfc_data = {
        // TODO: Randomize this or something?
        .uid = {0x50, 0x3F, 0xA2, 0xE6},
        .uid_len = 4,
        .atqa = {0x04, 0x00},
        .sak = 0x20,
        .type = FuriHalNfcTypeA,
    };
    furi_hal_nfc_exit_sleep();
    FURI_LOG_D(TAG, "Start");
    while(1) {
        uint32_t flags = furi_thread_flags_wait(WorkerEvtStop, FuriFlagWaitAny, 10);
        if(flags != FuriFlagErrorTimeout) {
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtStop) break;
        }

        if(!furi_hal_nfc_emulate_nfca(
               nfc_data.uid,
               nfc_data.uid_len,
               nfc_data.atqa,
               nfc_data.sak,
               u2f_nfc_callback,
               u2f_nfc,
               5000)) {
            FURI_LOG_T(TAG, "No device found");
        }
        //        if(furi_hal_nfc_listen(
        //               nfc_data.uid, nfc_data.uid_len, nfc_data.atqa, nfc_data.sak, true, 100)) {
        //            tx_rx.tx_bits = 0;
        //            tx_rx.tx_rx_type = FuriHalNfcTxRxTypeDefault;
        //            while(furi_hal_nfc_tx_rx(&tx_rx, 100)) {
        //                if(!u2f_nfc_callback(
        //                       tx_rx.rx_data,
        //                       tx_rx.rx_bits,
        //                       tx_rx.tx_data,
        //                       &tx_rx.tx_bits,
        //                       (uint32_t*)&tx_rx.tx_rx_type,
        //                       u2f_nfc)) {
        //                    break;
        //                }
        //            }
        //            FURI_LOG_T(TAG, "Done communicating");
        //        } else {
        //            FURI_LOG_T(TAG, "No device found");
        //        }
    }
    FURI_LOG_D(TAG, "Cleanup");
    furi_hal_nfc_sleep();
    FURI_LOG_D(TAG, "End");
    return 0;
}

U2fNfc* u2f_nfc_start(U2fData* u2f_inst) {
    U2fNfc* u2f_nfc = malloc(sizeof(U2fNfc));
    u2f_nfc->u2f_instance = u2f_inst;

    u2f_nfc->thread = furi_thread_alloc();
    furi_thread_set_name(u2f_nfc->thread, "U2fNFCWorker");
    furi_thread_set_stack_size(u2f_nfc->thread, 4096);
    furi_thread_set_context(u2f_nfc->thread, u2f_nfc);
    furi_thread_set_callback(u2f_nfc->thread, u2f_nfc_worker);
    furi_thread_start(u2f_nfc->thread);

    return u2f_nfc;
}

void u2f_nfc_stop(U2fNfc* u2f_nfc) {
    furi_assert(u2f_nfc);
    furi_thread_flags_set(furi_thread_get_id(u2f_nfc->thread), WorkerEvtStop);
    furi_thread_join(u2f_nfc->thread);
    furi_thread_free(u2f_nfc->thread);
    free(u2f_nfc);
}