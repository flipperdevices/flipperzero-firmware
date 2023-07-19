#include "seader_i.h"

#define RFAL_PICOPASS_TXRX_FLAGS                                                    \
    (FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_TX_MANUAL | FURI_HAL_NFC_LL_TXRX_FLAGS_AGC_ON | \
     FURI_HAL_NFC_LL_TXRX_FLAGS_PAR_RX_REMV | FURI_HAL_NFC_LL_TXRX_FLAGS_CRC_RX_KEEP)

#define TAG "RFAL_PICOPASS"

typedef struct {
    uint8_t CMD;
    uint8_t CSN[RFAL_PICOPASS_UID_LEN];
} rfalPicoPassSelectReq;

FuriHalNfcReturn rfalPicoPassPollerInitialize(void) {
    FuriHalNfcReturn ret;

    ret = furi_hal_nfc_ll_set_mode(
        FuriHalNfcModePollPicopass, FuriHalNfcBitrate26p48, FuriHalNfcBitrate26p48);
    if(ret != FuriHalNfcReturnOk) {
        return ret;
    };

    furi_hal_nfc_ll_set_error_handling(FuriHalNfcErrorHandlingNfc);
    furi_hal_nfc_ll_set_guard_time(FURI_HAL_NFC_LL_GT_PICOPASS);
    furi_hal_nfc_ll_set_fdt_listen(FURI_HAL_NFC_LL_FDT_LISTEN_PICOPASS_POLLER);
    furi_hal_nfc_ll_set_fdt_poll(FURI_HAL_NFC_LL_FDT_POLL_PICOPASS_POLLER);

    return FuriHalNfcReturnOk;
}

FuriHalNfcReturn rfalPicoPassPollerCheckPresence(void) {
    FuriHalNfcReturn ret;
    uint8_t txBuf[1] = {RFAL_PICOPASS_CMD_ACTALL};
    uint8_t rxBuf[32] = {0};
    uint16_t recvLen = 0;
    uint32_t flags = RFAL_PICOPASS_TXRX_FLAGS;
    uint32_t fwt = furi_hal_nfc_ll_ms2fc(20);

    ret = furi_hal_nfc_ll_txrx(txBuf, 1, rxBuf, 32, &recvLen, flags, fwt);
    return ret;
}

FuriHalNfcReturn rfalPicoPassPollerIdentify(rfalPicoPassIdentifyRes* idRes) {
    FuriHalNfcReturn ret;

    uint8_t txBuf[1] = {RFAL_PICOPASS_CMD_IDENTIFY};
    uint16_t recvLen = 0;
    uint32_t flags = RFAL_PICOPASS_TXRX_FLAGS;
    uint32_t fwt = furi_hal_nfc_ll_ms2fc(20);

    ret = furi_hal_nfc_ll_txrx(
        txBuf,
        sizeof(txBuf),
        (uint8_t*)idRes,
        sizeof(rfalPicoPassIdentifyRes),
        &recvLen,
        flags,
        fwt);
    // printf("identify rx: %d %s\n", recvLen, hex2Str(idRes->CSN, RFAL_PICOPASS_UID_LEN));

    return ret;
}

FuriHalNfcReturn rfalPicoPassPollerSelect(uint8_t* csn, rfalPicoPassSelectRes* selRes) {
    FuriHalNfcReturn ret;

    rfalPicoPassSelectReq selReq;
    selReq.CMD = RFAL_PICOPASS_CMD_SELECT;
    memcpy(selReq.CSN, csn, RFAL_PICOPASS_UID_LEN);
    uint16_t recvLen = 0;
    uint32_t flags = RFAL_PICOPASS_TXRX_FLAGS;
    uint32_t fwt = furi_hal_nfc_ll_ms2fc(20);

    ret = furi_hal_nfc_ll_txrx(
        (uint8_t*)&selReq,
        sizeof(rfalPicoPassSelectReq),
        (uint8_t*)selRes,
        sizeof(rfalPicoPassSelectRes),
        &recvLen,
        flags,
        fwt);
    // printf("select rx: %d %s\n", recvLen, hex2Str(selRes->CSN, RFAL_PICOPASS_UID_LEN));
    if(ret == FuriHalNfcReturnTimeout) {
        return FuriHalNfcReturnOk;
    }

    return ret;
}
