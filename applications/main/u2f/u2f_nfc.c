#include "u2f_nfc.h"
#include "furi_hal.h"

#define TAG "U2F_NFC"

typedef enum {
    WorkerEvtReserved = (1 << 0),
    WorkerEvtStop = (1 << 1),
} WorkerEvtFlags;

struct U2fNfc {
    FuriThread* thread;
    U2fData* u2f_instance;
    uint8_t payload[65535];
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

    uint8_t pcb = buff_rx[0];
    if(pcb == 0xC2) {
        memcpy(buff_tx, buff_rx, buff_rx_len);
        *buff_tx_len = buff_rx_len;
        return true;
    }

    if(buff_rx_len < 24) {
        FURI_LOG_D(
            TAG, "Unknown short message 0x%02x 0x%02x 0x%02x", buff_rx[0], buff_rx[1], buff_rx[2]);
        return false;
    }

    if((pcb & ~0x03) != 0) {
        buff_tx[0] = 0x02;
        buff_tx[1] = 0x68;
        buff_tx[2] = 0x00;
        *buff_tx_len = 3 * 8;
        FURI_LOG_E(TAG, "pcb 0x%02x not supported", pcb);
        return true;
    }

    if(buff_rx[1] == 0xC0) {
        if(u2f_nfc->payload_len == 0) {
            FURI_LOG_D(TAG, "requested block but not chaining");
            buff_tx[0] = 0x02;
            buff_tx[1] = 0x69;
            buff_tx[2] = 0x00;
            *buff_tx_len = 8 * 3;
            return true;
        }

        FURI_LOG_D(TAG, "continued chaining %d/%d", u2f_nfc->payload_cursor, u2f_nfc->payload_len);

        buff_tx[0] = 0x02;
        uint16_t max_resp_len = buff_rx[6 + buff_rx[5]];
        if(max_resp_len == 0) {
            max_resp_len = 256;
        }

        uint16_t remaining_len = u2f_nfc->payload_len - u2f_nfc->payload_cursor;
        if(remaining_len > max_resp_len) {
            memcpy(&buff_tx[1], &u2f_nfc->payload[u2f_nfc->payload_cursor], max_resp_len);
            remaining_len -= max_resp_len;
            buff_tx[max_resp_len + 1] = 0x61;
            if(remaining_len >= 256) {
                buff_tx[max_resp_len + 2] = 0x00;
            } else {
                buff_tx[max_resp_len + 2] = remaining_len - 2;
            }
            *buff_tx_len = (max_resp_len + 2 + 1) * 8;
            u2f_nfc->payload_cursor += max_resp_len;
        } else {
            memcpy(&buff_tx[1], &u2f_nfc->payload[u2f_nfc->payload_cursor], remaining_len);
            *buff_tx_len = (remaining_len + 1) * 8;
            u2f_nfc->payload_len = 0;
        }

        return true;
    }

    // Presence is implied by touching the NFC devices.
    u2f_confirm_user_present(u2f_nfc->u2f_instance);

    u2f_nfc->payload_len =
        u2f_msg_parse(u2f_nfc->u2f_instance, &buff_rx[1], (buff_rx_len / 8) - 1, u2f_nfc->payload);

    uint16_t max_resp_len = 256;

    if(u2f_nfc->payload_len - 2 <= max_resp_len) {
        buff_tx[0] = 0x02;
        memcpy(&buff_tx[1], u2f_nfc->payload, u2f_nfc->payload_len);
        *buff_tx_len = (u2f_nfc->payload_len + 1) * 8;
        u2f_nfc->payload_len = 0;
        FURI_LOG_D(TAG, "single response");
        return true;
    }

    buff_tx[0] = 0x02;
    memcpy(&buff_tx[1], u2f_nfc->payload, max_resp_len);
    buff_tx[max_resp_len + 1] = 0x61;
    uint16_t remaining_len = (u2f_nfc->payload_len - 2) - max_resp_len;
    if(remaining_len >= 256) {
        buff_tx[max_resp_len + 2] = 0x00;
    } else {
        buff_tx[max_resp_len + 2] = remaining_len;
    }
    *buff_tx_len = (max_resp_len + 1 + 2) * 8;
    u2f_nfc->payload_cursor = max_resp_len;
    FURI_LOG_D(TAG, "started chaining %d/%d", u2f_nfc->payload_cursor, u2f_nfc->payload_len);

    return true;
}

static int32_t u2f_nfc_worker(void* context) {
    U2fNfc* u2f_nfc = context;
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
        uint32_t flags = furi_thread_flags_wait(WorkerEvtStop, FuriFlagWaitAny, 1);
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
               1000)) {
            FURI_LOG_T(TAG, "No device found");
        }
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
