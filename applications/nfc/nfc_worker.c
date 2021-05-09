#include "nfc_worker_i.h"
#include <api-hal.h>

NfcWorker* nfc_worker_alloc(osMessageQueueId_t message_queue) {
    NfcWorker* nfc_worker = furi_alloc(sizeof(NfcWorker));
    nfc_worker->message_queue = message_queue;
    // Worker thread attributes
    nfc_worker->thread_attr.name = "nfc_worker";
    nfc_worker->thread_attr.stack_size = 2048;
    // Initialize rfal
    nfc_worker->error = api_hal_nfc_init();
    if(nfc_worker->error == ERR_NONE) {
        api_hal_nfc_start_sleep();
        nfc_worker_change_state(nfc_worker, NfcWorkerStateReady);
    } else {
        nfc_worker_change_state(nfc_worker, NfcWorkerStateBroken);
    }

    return nfc_worker;
}

void nfc_worker_free(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    free(nfc_worker);
}

NfcWorkerState nfc_worker_get_state(NfcWorker* nfc_worker) {
    return nfc_worker->state;
}

ReturnCode nfc_worker_get_error(NfcWorker* nfc_worker) {
    return nfc_worker->error;
}

void nfc_worker_start(NfcWorker* nfc_worker, NfcWorkerState state) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->state == NfcWorkerStateReady);
    nfc_worker_change_state(nfc_worker, state);
    nfc_worker->thread = osThreadNew(nfc_worker_task, nfc_worker, &nfc_worker->thread_attr);
}

void nfc_worker_stop(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    if(nfc_worker->state == NfcWorkerStateBroken) {
        return;
    }

    nfc_worker_change_state(nfc_worker, NfcWorkerStateStop);
}

void nfc_worker_change_state(NfcWorker* nfc_worker, NfcWorkerState state) {
    nfc_worker->state = state;
}

void nfc_worker_task(void* context) {
    NfcWorker* nfc_worker = context;

    api_hal_power_insomnia_enter();

    if(nfc_worker->state == NfcWorkerStatePoll) {
        nfc_worker_poll(nfc_worker);
    } else if(nfc_worker->state == NfcWorkerStateReadEMV) {
        nfc_worker_read_emv(nfc_worker);
    } else if(nfc_worker->state == NfcWorkerStateEmulate) {
        nfc_worker_emulate(nfc_worker);
    } else if(nfc_worker->state == NfcWorkerStateField) {
        nfc_worker_field(nfc_worker);
    }
    nfc_worker_change_state(nfc_worker, NfcWorkerStateReady);
    api_hal_power_insomnia_exit();
    osThreadExit();
}

void nfc_worker_read_emv(NfcWorker* nfc_worker) {
    ReturnCode err;
    rfalNfcDiscoverParam params;
    params.compMode = RFAL_COMPLIANCE_MODE_EMV;
    params.techs2Find = RFAL_NFC_POLL_TECH_A | RFAL_NFC_POLL_TECH_B | RFAL_NFC_POLL_TECH_F |
                        RFAL_NFC_POLL_TECH_V | RFAL_NFC_POLL_TECH_AP2P | RFAL_NFC_POLL_TECH_ST25TB;
    params.totalDuration = 100;
    params.devLimit = 3;
    params.wakeupEnabled = false;
    params.wakeupConfigDefault = true;
    params.nfcfBR = RFAL_BR_212;
    params.ap2pBR = RFAL_BR_424;
    params.maxBR = RFAL_BR_KEEP;
    params.GBLen = RFAL_NFCDEP_GB_MAX_LEN;
    params.notifyCb = NULL;
    rfalLowPowerModeStop();
    rfalNfcDiscover(&params);

    rfalNfcDevice* dev_list;
    rfalNfcDevice* dev_active;
    uint8_t tx_buff[255];
    uint8_t dev_cnt = 0;
    uint8_t ppse_select[] = {0x00, 0xA4, 0x04, 0x00, 0x0E, 0x32, 0x50, 0x41, 0x59, 0x2E,
                             0x53, 0x59, 0x53, 0x2E, 0x44, 0x44, 0x46, 0x30, 0x31, 0x00};
    // uint8_t aid_len = 0;
    // TODO read from data exchange
    uint8_t aid[] = {0xA0, 0x00 , 0x00, 0x00, 0x03, 0x10, 0x10};
    // Select mastercard application
    uint8_t app_select[] = {0x00, 0xA4, 0x04, 0x00};
    uint8_t* rx_buff;
    uint16_t* rx_len;
    rfalNfcState state;

    while(nfc_worker->state == NfcWorkerStateReadEMV) {
        rfalNfcWorker();
        state = rfalNfcGetState();
        FURI_LOG_I("NFC", "Current state: %d", state);
        if(state >= RFAL_NFC_STATE_ACTIVATED) {
            err = rfalNfcGetDevicesFound(&dev_list, &dev_cnt);
            FURI_LOG_E("NFC", "Get device err: %d", err);
            FURI_LOG_I("NFC", "Devices found: %d", dev_cnt);
            err = rfalNfcGetActiveDevice(&dev_active);
            FURI_LOG_E("NFC", "Get active device err: %d", err);
            break;
        }
        osDelay(10);
    }
    // Start data exchange
    err = rfalNfcDataExchangeStart(ppse_select, sizeof(ppse_select), &rx_buff, &rx_len, 0);
    FURI_LOG_E("NFC", "Start data exchange err: %d", err);
    while(true) {
        rfalNfcWorker();
        FURI_LOG_I("NFC", "State: %d", rfalNfcGetState());
        err = rfalNfcDataExchangeGetStatus();
        FURI_LOG_E("NFC", "Data Exchange err: %d", err);
        if(err == ERR_NONE) {
            break;
        }
        osDelay(10);
    }
    FURI_LOG_I("NFC", "Transaction is complete. Received %d bytes", *rx_len);

    for(uint16_t i = 0; i < *rx_len; i++) {
        printf("%02x ", rx_buff[i]); // Log data
        // if(rx_buff[i] == 0x61) { // Application template
        //     aid_len = rx_buff[i + 1];
        //     FURI_LOG_I("NFC", "aid len: %d", aid_len);
        // }
    }

    // Select application preparation buffer
    uint8_t size = sizeof(app_select);
    memcpy(tx_buff, app_select, size);
    tx_buff[size++] = sizeof(aid);
    memcpy(tx_buff + size, aid, sizeof(aid));
    size += sizeof(aid);
    tx_buff[size++] = 0;

    // Start data exchange
    err = rfalNfcDataExchangeStart(tx_buff, size, &rx_buff, &rx_len, 0);
    FURI_LOG_E("NFC", "Start data exchange err: %d", err);
    while(true) {
        rfalNfcWorker();
        FURI_LOG_I("NFC", "State: %d", rfalNfcGetState());
        err = rfalNfcDataExchangeGetStatus();
        FURI_LOG_E("NFC", "Data Exchange err: %d", err);
        if(err == ERR_NONE) {
            break;
        }
        osDelay(10);
    }
    FURI_LOG_I("NFC", "Transaction is complete. Received %d bytes", *rx_len);
    for(uint16_t i = 0; i < *rx_len; i++) {
        printf("%02x ", rx_buff[i]); // Log data
    }

    // READ PDOL
    uint8_t pdol_start[] = {0x80, 0xA8, 0x00, 0x00};
    size = sizeof(pdol_start);
    memcpy(tx_buff, pdol_start, size);
    tx_buff[size++] = 2 + 


    printf("\r\n");
}

void nfc_worker_poll(NfcWorker* nfc_worker) {
    rfalNfcDevice* dev_list;
    uint8_t dev_cnt;
    // Update screen before start searching
    NfcMessage message = {.type = NfcMessageTypeDeviceNotFound};
    furi_check(osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);

    while(nfc_worker->state == NfcWorkerStatePoll) {
        if(api_hal_nfc_detect(&dev_list, &dev_cnt, 100)) {
            // Send message with first device found
            message.type = NfcMessageTypeDeviceFound;
            if(dev_list[0].type == RFAL_NFC_LISTEN_TYPE_NFCA) {
                message.device.type = NfcDeviceTypeNfca;
                message.device.nfca = dev_list[0].dev.nfca;
            } else if(dev_list[0].type == RFAL_NFC_LISTEN_TYPE_NFCB) {
                message.device.type = NfcDeviceTypeNfcb;
                message.device.nfcb = dev_list[0].dev.nfcb;
            } else if(dev_list[0].type == RFAL_NFC_LISTEN_TYPE_NFCF) {
                message.device.type = NfcDeviceTypeNfcf;
                message.device.nfcf = dev_list[0].dev.nfcf;
            } else if(dev_list[0].type == RFAL_NFC_LISTEN_TYPE_NFCV) {
                message.device.type = NfcDeviceTypeNfcv;
                message.device.nfcv = dev_list[0].dev.nfcv;
            } else {
                // TODO show information about all found devices
                message.type = NfcMessageTypeDeviceNotFound;
            }
            furi_check(
                osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);
        } else {
            message.type = NfcMessageTypeDeviceNotFound;
            furi_check(
                osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);
        }
        osDelay(20);
    }
}

void nfc_worker_state_callback(rfalNfcState st) {
    (void)st;
}

ReturnCode nfc_worker_trx(
    uint8_t* txBuf,
    uint16_t txBufSize,
    uint8_t** rxData,
    uint16_t** rcvLen,
    uint32_t fwt) {
    ReturnCode err;

    err = rfalNfcDataExchangeStart(txBuf, txBufSize, rxData, rcvLen, fwt);
    if(err == ERR_NONE) {
        do {
            rfalNfcWorker();
            err = rfalNfcDataExchangeGetStatus();
        } while(err == ERR_BUSY);
    }
    return err;
}

void nfc_worker_exchange(NfcWorker* nfc_worker, rfalNfcDevice* nfc_device) {
    ReturnCode err = ERR_NONE;
    uint8_t* rxData;
    uint16_t* rcvLen;
    uint8_t txBuf[100];
    uint16_t txLen;

    do {
        rfalNfcWorker();
        switch(rfalNfcGetState()) {
        case RFAL_NFC_STATE_ACTIVATED:
            err = nfc_worker_trx(NULL, 0, &rxData, &rcvLen, 0);
            break;
        case RFAL_NFC_STATE_DATAEXCHANGE:
        case RFAL_NFC_STATE_DATAEXCHANGE_DONE:
            // Not supported
            txBuf[0] = ((char)0x68);
            txBuf[1] = ((char)0x00);
            txLen = 2;
            err = nfc_worker_trx(txBuf, txLen, &rxData, &rcvLen, RFAL_FWT_NONE);
            break;
        case RFAL_NFC_STATE_START_DISCOVERY:
            return;
        case RFAL_NFC_STATE_LISTEN_SLEEP:
        default:
            break;
        }
    } while((err == ERR_NONE) || (err == ERR_SLEEP_REQ));
}

void nfc_worker_emulate(NfcWorker* nfc_worker) {
    rfalNfcDiscoverParam params;
    params.compMode = RFAL_COMPLIANCE_MODE_NFC;
    params.techs2Find = RFAL_NFC_LISTEN_TECH_A;
    params.totalDuration = 1000U;
    params.devLimit = 1;
    params.wakeupEnabled = false;
    params.wakeupConfigDefault = true;
    params.nfcfBR = RFAL_BR_212;
    params.ap2pBR = RFAL_BR_424;
    params.maxBR = RFAL_BR_KEEP;
    params.GBLen = RFAL_NFCDEP_GB_MAX_LEN;
    params.notifyCb = nfc_worker_state_callback;

    params.lmConfigPA.nfcidLen = RFAL_LM_NFCID_LEN_07;
    params.lmConfigPA.nfcid[0] = 0x00;
    params.lmConfigPA.nfcid[1] = 0x01;
    params.lmConfigPA.nfcid[2] = 0x02;
    params.lmConfigPA.nfcid[3] = 0x03;
    params.lmConfigPA.nfcid[4] = 0x04;
    params.lmConfigPA.nfcid[5] = 0x05;
    params.lmConfigPA.nfcid[6] = 0x06;
    params.lmConfigPA.SENS_RES[0] = 0x44;
    params.lmConfigPA.SENS_RES[1] = 0x00;
    params.lmConfigPA.SEL_RES = 0x00;

    ReturnCode ret;
    ret = rfalNfcDiscover(&params);
    if(ret != ERR_NONE) {
        asm("bkpt 1");
        return;
    }

    rfalNfcDevice* nfc_device;
    while(nfc_worker->state == NfcWorkerStateEmulate) {
        rfalNfcWorker();
        if(rfalNfcIsDevActivated(rfalNfcGetState())) {
            rfalNfcGetActiveDevice(&nfc_device);
            nfc_worker_exchange(nfc_worker, nfc_device);
        }
        osDelay(10);
    }

    rfalNfcDeactivate(false);
}

void nfc_worker_field(NfcWorker* nfc_worker) {
    api_hal_nfc_field_on();
    while(nfc_worker->state == NfcWorkerStateField) {
        osDelay(50);
    }
    api_hal_nfc_field_off();
}
