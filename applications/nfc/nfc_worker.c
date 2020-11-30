#include "nfc_worker.h"

#define NFC_WORKER_DEVICE_MAX 5

// TODO replace with pubsub
static volatile bool isr_enabled = false;

void nfc_isr() {
    if(isr_enabled) {
        st25r3916Isr();
    }
}

void nfc_worker_task(NfcWorkerContext* context) {
    ReturnCode ret;

    rfalAnalogConfigInitialize();
    ret = rfalNfcInitialize();

    while(1) {
        rfalWorker();
        nfc_worker_nfca();
        platformDelay(500);
    }

    rfalFieldOff();
    rfalLowPowerModeStart();

    osThreadExit();
}

void nfc_worker_poll(NfcWorkerContext* context) {
    while(1){
        nfc_worker_nfca_poll(context);
        nfc_worker_nfcb_poll(context);
        nfc_worker_nfcf_poll(context);
        nfc_worker_nfcv_poll(context);
    }
}

ReturnCode nfc_worker_nfca_poll(NfcWorkerContext* context) {
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
    err = rfalNfcaPollerFullCollisionResolution(RFAL_COMPLIANCE_MODE_NFC, 1, &nfcaDevList, &dev_cnt);
    if(err != ERR_NONE) {
        return err;
    }

    rfalNfcaPollerSleep();

    // Prepare response
    NfcWorkerResponse response;
    response.type = NfcWorkerResponseTypeDeviceFound;
    response.device.type = NfcDeviceTypeNfca;
    response.device.nfca = device;
    furi_check(osMessageQueuePut(context->response_queue, &response, 0, osWaitForever) == osOK);
}

ReturnCode nfc_worker_nfcb_poll(NfcWorkerContext* context) {

}

ReturnCode nfc_worker_nfcf_poll(NfcWorkerContext* context) {

}

ReturnCode nfc_worker_nfcv_poll(NfcWorkerContext* context) {

}










// void nfc_worker_nfca(NfcWorkerContext* context) {
//     rfalNfcaSensRes sense_res;
//     rfalNfcaSelRes selRes;
//     rfalNfcaListenDevice nfcaDevList[EXAMPLE_NFCA_DEVICES];
//     uint8_t devCnt;
//     uint8_t devIt;

//     isr_enabled = true;

//     while(widget_is_enabled(nfc->widget)) {
//         rfalFieldOff();
//         platformDelay(500);
//         rfalNfcaPollerInitialize();
//         rfalFieldOnAndStartGT();
//         nfc->ret = err = rfalNfcaPollerTechnologyDetection(RFAL_COMPLIANCE_MODE_NFC, &sensRes);
//         if(err == ERR_NONE) {
//             err = rfalNfcaPollerFullCollisionResolution(
//                 RFAL_COMPLIANCE_MODE_NFC, EXAMPLE_NFCA_DEVICES, nfcaDevList, &devCnt);
//             nfc->devCnt = devCnt;
//             if((err == ERR_NONE) && (devCnt > 0)) {
//                 platformLog("NFC-A device(s) found %d\r\n", devCnt);
//                 devIt = 0;
//                 if(nfcaDevList[devIt].isSleep) {
//                     err = rfalNfcaPollerCheckPresence(
//                         RFAL_14443A_SHORTFRAME_CMD_WUPA, &sensRes); /* Wake up all cards  */
//                     if(err != ERR_NONE) {
//                         continue;
//                     }
//                     err = rfalNfcaPollerSelect(
//                         nfcaDevList[devIt].nfcId1,
//                         nfcaDevList[devIt].nfcId1Len,
//                         &selRes); /* Select specific device  */
//                     if(err != ERR_NONE) {
//                         continue;
//                     }
//                 }

//                 nfc->first_atqa = nfcaDevList[devIt].sensRes;
//                 nfc->first_sak = nfcaDevList[devIt].selRes;

//                 switch(nfcaDevList[devIt].type) {
//                 case RFAL_NFCA_T1T:
//                     /* No further activation needed for a T1T (RID already performed)*/
//                     platformLog(
//                         "NFC-A T1T device found \r\n"); /* NFC-A T1T device found, NFCID/UID is contained in: t1tRidRes.uid */
//                     nfc->current = "NFC-A T1T";
//                     /* Following communications shall be performed using:
//                          *   - Non blocking: rfalStartTransceive() + rfalGetTransceiveState()
//                          *   -     Blocking: rfalTransceiveBlockingTx() + rfalTransceiveBlockingRx() or rfalTransceiveBlockingTxRx()    */
//                     break;
//                 case RFAL_NFCA_T2T:
//                     /* No specific activation needed for a T2T */
//                     platformLog(
//                         "NFC-A T2T device found \r\n"); /* NFC-A T2T device found, NFCID/UID is contained in: nfcaDev.nfcid */
//                     nfc->current = "NFC-A T2T";
//                     /* Following communications shall be performed using:
//                          *   - Non blocking: rfalStartTransceive() + rfalGetTransceiveState()
//                          *   -     Blocking: rfalTransceiveBlockingTx() + rfalTransceiveBlockingRx() or rfalTransceiveBlockingTxRx()    */
//                     break;
//                 case RFAL_NFCA_T4T:
//                     platformLog(
//                         "NFC-A T4T (ISO-DEP) device found \r\n"); /* NFC-A T4T device found, NFCID/UID is contained in: nfcaDev.nfcid */
//                     nfc->current = "NFC-A T4T";
//                     /* Activation should continue using rfalIsoDepPollAHandleActivation(), see exampleRfalPoller.c */
//                     break;
//                 case RFAL_NFCA_T4T_NFCDEP: /* Device supports T4T and NFC-DEP */
//                 case RFAL_NFCA_NFCDEP: /* Device supports NFC-DEP */
//                     platformLog(
//                         "NFC-A P2P (NFC-DEP) device found \r\n"); /* NFC-A P2P device found, NFCID/UID is contained in: nfcaDev.nfcid */
//                     nfc->current = "NFC-A P2P";
//                     /* Activation should continue using rfalNfcDepInitiatorHandleActivation(), see exampleRfalPoller.c */
//                     break;
//                 }
//                 rfalNfcaPollerSleep(); /* Put device to sleep / HLTA (useless as the field will be turned off anyhow) */
//             }
//         }
//         widget_update(nfc->widget);
//     }

//     isr_enabled = false;
    
//     nfc->ret = ERR_NONE;
//     nfc->worker = NULL;
// }