#include "u2f_nfc.h"
#include "nfc_worker.h"

#define TAG "U2F_NFC"

#define U2F_SELECT_CMD (0xA4)

typedef enum {
    WorkerEvtReserved = (1 << 0),
    WorkerEvtStop = (1 << 1),
} WorkerEvtFlags;

struct U2fNfc {
    FuriThread* thread;
    U2fData* u2f_instance;
};

static bool u2f_nfc_callback(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len,
    uint32_t* data_type,
    void* context) {
    UNUSED(context);
    *data_type = FURI_HAL_NFC_TXRX_DEFAULT;
    *buff_tx_len = 0;
//    FURI_LOG_D(TAG, "Something detected");
    if(buff_rx_len >= 16) {
        uint8_t cla = buff_rx[0];
        uint8_t ins = buff_rx[1];
//        FURI_LOG_D(TAG, "cla: 0x%02x ins: 0x%02x", cla, ins);
        if (cla != 0x00) {
            buff_tx[0] = 0x68;
            buff_tx[1] = 0x00;
            *buff_tx_len = 16;
//            FURI_LOG_D(TAG, "cla 0x%02x not supported", cla);
            return true;
        }
        if (ins == 0xa4) {
            if (memcmp(&buff_rx[2], "\x04\x00\x08\xa0\x00\x00\x06\x47\x2f\x00\x01", 11) != 0) {
                buff_tx[0] = 0x6A;
                buff_tx[1] = 0x82;
                *buff_tx_len = 16;
//                FURI_LOG_D(TAG, "Unknown AID");
                return true;
            }
            memcpy(buff_tx, "U2F_V2\x90\x00", 8);
//            FURI_LOG_D(TAG, "Card selected");
            *buff_tx_len = 64;
        } else if (ins == 0x01) {
            buff_tx[0] = 0x69;
            buff_tx[1] = 0x00;
            *buff_tx_len = 16;
//            FURI_LOG_D(TAG, "Request sent of p1: %02x, p2: %02x, len: %02x", buff_tx[2], buff_tx[3], buff_tx[4]);
            return true;
        } else {
            buff_tx[0] = 0x69;
            buff_tx[1] = 0x00;
            *buff_tx_len = 216;
//            FURI_LOG_D(TAG, "Unknown instruction");
            return true;
        }
    }
    return *buff_tx_len > 0;
}

static int32_t u2f_nfc_worker(void* context) {
    U2fNfc* u2f_nfc = context;
    FURI_LOG_D(TAG, "Init");
    //    NfcWorker* nfc_worker = nfc_worker_alloc();
    while(furi_hal_nfc_is_busy()) {
        furi_delay_ms(10);
    }
    FuriHalNfcDevData nfc_data = {
        // TODO: Randomize this or something?
        .uid = {0xCF, 0x72, 0xd4, 0x40},
        .uid_len = 4,
        .atqa = {0x00, 0x04},
        .sak = 0x42,
        .type = FuriHalNfcTypeA,
    };
    furi_hal_nfc_exit_sleep();
    FURI_LOG_D(TAG, "Start");
    while(1) {
        uint32_t flags = furi_thread_flags_wait(WorkerEvtStop, FuriFlagWaitAny, 100);
        furi_check((flags & FuriFlagError));
        if(flags & WorkerEvtStop) break;

        if (!furi_hal_nfc_emulate_nfca(
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
    //    nfc_worker_free(nfc_worker);
    FURI_LOG_D(TAG, "End");
    return 0;
}

U2fNfc* u2f_nfc_start(U2fData* u2f_inst) {
    U2fNfc* u2f_nfc = malloc(sizeof(U2fNfc));
    u2f_nfc->u2f_instance = u2f_inst;

    u2f_nfc->thread = furi_thread_alloc();
    furi_thread_set_name(u2f_nfc->thread, "U2fNFCWorker");
    furi_thread_set_stack_size(u2f_nfc->thread, 2048);
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