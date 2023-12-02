#include "emu_card_worker.h"

#define TAG "EmuCardWorker"

static uint8_t CMD_14A_WTX[4] = {0xf2, 0x01, 0x91, 0x40};

bool emu_card_worker_set_apdu(
    EmuCardWorker* emu_card_worker,
    uint8_t offset,
    uint8_t* data,
    uint16_t len) {
    furi_assert(len + offset < sizeof(emu_card_worker->apdu_buf));
    memcpy(&emu_card_worker->apdu_buf[offset], data, len);
    emu_card_worker->apdu_buf_len = len + offset;
    return false;
}

bool emu_card_worker_get_apdu(EmuCardWorker* emu_card_worker, uint8_t* data, uint16_t* len) {
    if(!emu_card_worker->apdu_buf_len) {
        return false;
    }
    memcpy(data, &emu_card_worker->apdu_buf[0], emu_card_worker->apdu_buf_len);
    *len = emu_card_worker->apdu_buf_len;
    emu_card_worker->apdu_buf_len = 0;
    return true;
}

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

static bool emu_callback(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len,
    uint32_t* flags,
    void* context) {
#ifdef FURI_DEBUG
    FuriString* debug_buf;
    debug_buf = furi_string_alloc();
    for(int i = 0; i < (buff_rx_len + 7) / 8; ++i) {
        furi_string_cat_printf(debug_buf, "%02x ", buff_rx[i]);
    }
    furi_string_trim(debug_buf);
    FURI_LOG_T(TAG, "Emu RX (%d): %s", buff_rx_len, furi_string_get_cstr(debug_buf));
    furi_string_reset(debug_buf);
#endif

    furi_assert(context);
    EmuCardWorker* emu_card_worker = context;
    if(buff_rx_len == 0) {
        FURI_LOG_E(TAG, "buff_rx_len=0 ?");
        return false;
    }
    uint8_t rx_bytes = buff_rx_len / 8;
    uint8_t pcb = buff_rx[0];

    if((pcb & 0xfe) == 0xb2) {
        FURI_LOG_D(TAG, "Recv R(NACK)");
        if(emu_card_worker->state == EmuCardWorkerStateWaitApduResp) {
            FURI_LOG_D(TAG, "NACK as WTX Resp");
            memcpy(buff_tx, &CMD_14A_WTX[0], sizeof(CMD_14A_WTX));
            buff_tx[0] &= 0xfe;
            buff_tx[0] |= pcb & 0x1;
            furi_delay_us(emu_card_worker->delayus);
            *buff_tx_len = sizeof(CMD_14A_WTX) * 8;
            *flags = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
            return true;
        } else {
            FURI_LOG_D(TAG, "send 0xa2");
            buff_tx[0] = 0xa2;
            *buff_tx_len = 1 * 8;
            return true;
        }
    } else if((pcb & 0xfe) == 0xc2) {
        FURI_LOG_D(TAG, "Recv S(DESELECT)");
    } else if((pcb & 0xfe) == 0xf2) {
        if(emu_card_worker->state == EmuCardWorkerStateWaitApduResp) {
            FURI_LOG_T(TAG, "Recv WTX Resp");
            memcpy(buff_tx, &CMD_14A_WTX[0], sizeof(CMD_14A_WTX));
            buff_tx[0] &= 0xfe;
            buff_tx[0] |= pcb & 0x1;
            furi_delay_us(emu_card_worker->delayus);
            *buff_tx_len = sizeof(CMD_14A_WTX) * 8;
            *flags = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
            return true;
        } else if(emu_card_worker->state == EmuCardWorkerStateGetApduResp) {
            FURI_LOG_D(TAG, "Recv WTX Resp, Ready to send Response back");
            memcpy(&buff_tx[0], &emu_card_worker->apdu_buf[0], emu_card_worker->apdu_buf_len);
            nfca_append_crc16(&buff_tx[0], emu_card_worker->apdu_buf_len);
            *buff_tx_len = (emu_card_worker->apdu_buf_len + 2) * 8;
            *flags = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
#ifdef FURI_DEBUG
            debug_buf = furi_string_alloc();
            for(int i = 0; i < (*buff_tx_len + 7) / 8; ++i) {
                furi_string_cat_printf(debug_buf, "%02x ", buff_tx[i]);
            }
            furi_string_trim(debug_buf);
            FURI_LOG_T(TAG, "Emu Send (%d): %s", *buff_tx_len, furi_string_get_cstr(debug_buf));
            furi_string_reset(debug_buf);
#endif
            emu_card_worker->state = EmuCardWorkerStateWaitApduReq;
            return true;
        } else {
            FURI_LOG_E(TAG, "Recv WTX Resp but we hasn't request");
        }
    } else if((pcb & 0xfe) == 0x2) {
        // I-Block ignore the block number at LSB
        FURI_LOG_T(TAG, "Recv I-Block");
        emu_card_worker_set_apdu(emu_card_worker, 0, buff_rx, rx_bytes);

        FURI_LOG_D(TAG, "Start send WTX");
        memcpy(buff_tx, &CMD_14A_WTX[0], sizeof(CMD_14A_WTX));
        *buff_tx_len = sizeof(CMD_14A_WTX) * 8;
        *flags = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
        emu_card_worker->state = EmuCardWorkerStateWaitApduResp;
        return true;
    } else {
        FURI_LOG_E(TAG, "Something unknown");
        *buff_tx_len = UINT16_MAX;
        *flags = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
        return true;
    }
    FURI_LOG_E(TAG, "?");
    *buff_tx_len = UINT16_MAX;
    *flags = FURI_HAL_NFC_TX_RAW_RX_DEFAULT;
    return false;
}

EmuCardWorker* emu_card_worker_alloc() {
    EmuCardWorker* emu_card_worker = malloc(sizeof(EmuCardWorker));

    emu_card_worker->thread =
        furi_thread_alloc_ex("EmuCardWorker", 2048, emu_card_worker_task, emu_card_worker);

    emu_card_worker->state = EmuCardWorkerStateStop;
    emu_card_worker->delayus = 3000; //TODO: Make that configurable

    return emu_card_worker;
}

void emu_card_worker_free(EmuCardWorker* emu_card_worker) {
    furi_assert(emu_card_worker);

    furi_thread_free(emu_card_worker->thread);
    free(emu_card_worker);
}

void emu_card_worker_stop(EmuCardWorker* emu_card_worker) {
    furi_assert(emu_card_worker);

    emu_card_worker->state = EmuCardWorkerStateStop;
    furi_thread_join(emu_card_worker->thread);
}

void emu_card_worker_start(EmuCardWorker* emu_card_worker, FuriHalNfcDevData* dev_data) {
    furi_assert(emu_card_worker);
    furi_hal_nfc_deinit();
    furi_hal_nfc_init();

    memcpy(&emu_card_worker->dev_data, dev_data, sizeof(FuriHalNfcDevData));
    furi_thread_start(emu_card_worker->thread);
}

int32_t emu_card_worker_task(void* context) {
    furi_assert(context);
    EmuCardWorker* emu_card_worker = context;
    FURI_LOG_D(TAG, "task start");
    while(furi_hal_nfc_is_busy()) {
        furi_delay_ms(10);
        FURI_LOG_D(TAG, "busy");
    }

    emu_card_worker->state = EmuCardWorkerStateWaitApduReq;
    while(1) {
        if(emu_card_worker->state == EmuCardWorkerStateStop) break;
        FURI_LOG_D(TAG, "one round");
        furi_hal_nfc_emulate_nfca(
            emu_card_worker->dev_data.uid,
            emu_card_worker->dev_data.uid_len,
            emu_card_worker->dev_data.atqa,
            emu_card_worker->dev_data.sak,
            emu_callback,
            emu_card_worker,
            5000);
    }
    emu_card_worker->state = EmuCardWorkerStateStop;
    FURI_LOG_D(TAG, "task end");
    return 0;
}