#include "nfc_worker_i.h"

// TODO replace with pubsub
static volatile bool isr_enabled = false;

void nfc_isr() {
    if(isr_enabled) {
        st25r3916Isr();
    }
}

NfcWorker* nfc_worker_alloc(osMessageQueueId_t message_queue) {
    NfcWorker* nfc_worker = furi_alloc(sizeof(NfcWorker));
    nfc_worker->message_queue = message_queue;
    // Worker thread attributes
    nfc_worker->thread_attr.name = "nfc_worker";
    // nfc_worker->thread_attr.attr_bits = osThreadJoinable;
    nfc_worker->thread_attr.stack_size = 4096;
    // Initialize rfal
    isr_enabled = true;
    rfalAnalogConfigInitialize();
    nfc_worker->error_code = rfalNfcInitialize();
    if (nfc_worker->error_code == ERR_NONE) {
        rfalLowPowerModeStart();
        nfc_worker_change_state(nfc_worker, NfcWorkerStateReady);
    } else {
        nfc_worker_change_state(nfc_worker, NfcWorkerStateBroken);
    }

    return nfc_worker;
}

void nfc_worker_free(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);

}

void nfc_worker_poll_start(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->state == NfcWorkerStateReady);
    nfc_worker->thread = osThreadNew(nfc_worker_task, nfc_worker, &nfc_worker->thread_attr);
}

void nfc_worker_poll_stop(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->state == NfcWorkerStatePolling);

}

void nfc_worker_field_on(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->state == NfcWorkerStateReady);

    rfalLowPowerModeStop();
    st25r3916TxRxOn();
}

void nfc_worker_field_off(NfcWorker* nfc_worker) {
    furi_assert(nfc_worker);
    furi_assert(nfc_worker->state == NfcWorkerStateReady);

    st25r3916TxRxOff();
    rfalLowPowerModeStart();
}

void nfc_worker_change_state(NfcWorker* nfc_worker, NfcWorkerState state) {
    nfc_worker->state = state;
    NfcMessage message;
    message.type = NfcMessageTypeWorkerStateChange;
    message.worker_state = nfc_worker->state;
    furi_check(osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);
}

void nfc_worker_task(void* context) {
    NfcWorker* nfc_worker = context;
    ReturnCode ret;

    nfc_worker_change_state(nfc_worker, NfcWorkerStatePolling);

    rfalLowPowerModeStop();

    while(1) {
        rfalWorker();
        ret = nfc_worker_nfca_poll(nfc_worker);
        ret = nfc_worker_nfcb_poll(nfc_worker);
        ret = nfc_worker_nfcf_poll(nfc_worker);
        ret = nfc_worker_nfcv_poll(nfc_worker);
        rfalFieldOff();
        platformDelay(500);
    }

    rfalLowPowerModeStart();

    nfc_worker_change_state(nfc_worker, NfcWorkerStateReady);

    osThreadExit();
}

ReturnCode nfc_worker_nfca_poll(NfcWorker* nfc_worker) {
    ReturnCode err;
    rfalNfcaSensRes sense_res;

    rfalNfcaPollerInitialize();
    rfalFieldOnAndStartGT();
    err = rfalNfcaPollerTechnologyDetection(RFAL_COMPLIANCE_MODE_NFC, &sense_res);
    if(err != ERR_NONE) {
        return err;
    }

    uint8_t dev_cnt;
    rfalNfcaListenDevice device;
    err = rfalNfcaPollerFullCollisionResolution(RFAL_COMPLIANCE_MODE_NFC, 1, &device, &dev_cnt);
    if(err != ERR_NONE) {
        return err;
    }

    rfalNfcaPollerSleep();

    if (dev_cnt) {
        NfcMessage message;
        message.type = NfcMessageTypeDeviceFound;
        message.device.type = NfcDeviceTypeNfca;
        message.device.nfca = device;
        furi_check(osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);
    }

    return ERR_NONE;
}

ReturnCode nfc_worker_nfcb_poll(NfcWorker* nfc_worker) {
    ReturnCode err;

    rfalNfcbPollerInitialize();
    rfalFieldOnAndStartGT();

    rfalNfcbSensbRes sensb_res;
    uint8_t sensb_res_len;
    err = rfalNfcbPollerTechnologyDetection(RFAL_COMPLIANCE_MODE_NFC, &sensb_res, &sensb_res_len);
    if(err != ERR_NONE) {
        return err;
    }

    uint8_t dev_cnt;
    rfalNfcbListenDevice device;
    err = rfalNfcbPollerCollisionResolution( RFAL_COMPLIANCE_MODE_NFC, 1, &device, &dev_cnt);
    if(err != ERR_NONE) {
        return err;
    }

    if (dev_cnt) {
        rfalNfcbPollerSleep(device.sensbRes.nfcid0);
        NfcMessage message;
        message.type = NfcMessageTypeDeviceFound;
        message.device.type = NfcDeviceTypeNfcb;
        message.device.nfcb = device;
        furi_check(osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);
    }

    return ERR_NONE;
}

ReturnCode nfc_worker_nfcf_poll(NfcWorker* nfc_worker) {
    ReturnCode err;

    rfalNfcfPollerInitialize( RFAL_BR_212 );
    rfalFieldOnAndStartGT();
    
    err = rfalNfcfPollerCheckPresence();
    if(err != ERR_NONE) {
        return err;
    }

    uint8_t dev_cnt;
    rfalNfcfListenDevice device;
    err = rfalNfcfPollerCollisionResolution(RFAL_COMPLIANCE_MODE_NFC, 1, &device, &dev_cnt);
    if(err != ERR_NONE) {
        return err;
    }

    if (dev_cnt) {
        NfcMessage message;
        message.type = NfcMessageTypeDeviceFound;
        message.device.type = NfcDeviceTypeNfcf;
        message.device.nfcf = device;
        furi_check(osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);
    }

    return ERR_NONE;
}

ReturnCode nfc_worker_nfcv_poll(NfcWorker* nfc_worker) {
    ReturnCode err;
    rfalNfcvInventoryRes invRes;

    rfalNfcvPollerInitialize();
    rfalFieldOnAndStartGT();

    err = rfalNfcvPollerCheckPresence(&invRes);
    if(err != ERR_NONE) {
        return err;
    }

    uint8_t dev_cnt;
    rfalNfcvListenDevice device;
    err = rfalNfcvPollerCollisionResolution(RFAL_COMPLIANCE_MODE_NFC, 1, &device, &dev_cnt);
    if(err != ERR_NONE) {
        return err;
    }

    if (dev_cnt) {
        NfcMessage message;
        message.type = NfcMessageTypeDeviceFound;
        message.device.type = NfcDeviceTypeNfcv;
        message.device.nfcv = device;
        furi_check(osMessageQueuePut(nfc_worker->message_queue, &message, 0, osWaitForever) == osOK);
    }

    return ERR_NONE;
}
