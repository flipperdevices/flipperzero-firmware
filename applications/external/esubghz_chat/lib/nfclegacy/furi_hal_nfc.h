/**
 * @file furry_hal_nfc.h
 * NFC HAL API
 */

#pragma once

#include "ST25RFAL002/st_errno.h"
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "ST25RFAL002/include/rfal_nfc.h"
#include "protocols/nfca.h"

#define FURRY_HAL_NFC_UID_MAX_LEN 10
#define FURRY_HAL_NFC_DATA_BUFF_SIZE (512)
#define FURRY_HAL_NFC_PARITY_BUFF_SIZE (FURRY_HAL_NFC_DATA_BUFF_SIZE / 8)

#define FURRY_HAL_NFC_TXRX_DEFAULT                                                   \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_AUTO | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_REMV | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_REMV | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_AUTO)

#define FURRY_HAL_NFC_TX_DEFAULT_RX_NO_CRC                                           \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_AUTO | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_KEEP | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_REMV | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_AUTO)

#define FURRY_HAL_NFC_TXRX_WITH_PAR                                                    \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_MANUAL | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_KEEP | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_KEEP | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_AUTO)

#define FURRY_HAL_NFC_TXRX_RAW                                                         \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_MANUAL | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_KEEP | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_KEEP | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_NONE)

#define FURRY_HAL_NFC_TX_RAW_RX_DEFAULT                                                \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_MANUAL | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_REMV | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_REMV | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_NONE)

typedef enum {
    FurryHalNfcTxRxTypeDefault,
    FurryHalNfcTxRxTypeRxNoCrc,
    FurryHalNfcTxRxTypeRxKeepPar,
    FurryHalNfcTxRxTypeRaw,
    FurryHalNfcTxRxTypeRxRaw,
    FurryHalNfcTxRxTransparent,
} FurryHalNfcTxRxType;

typedef bool (*FurryHalNfcEmulateCallback)(
    uint8_t* buff_rx,
    uint16_t buff_rx_len,
    uint8_t* buff_tx,
    uint16_t* buff_tx_len,
    uint32_t* flags,
    void* context);

typedef enum {
    FurryHalNfcTypeA,
    FurryHalNfcTypeB,
    FurryHalNfcTypeF,
    FurryHalNfcTypeV,
} FurryHalNfcType;

typedef enum {
    FurryHalNfcInterfaceRf,
    FurryHalNfcInterfaceIsoDep,
    FurryHalNfcInterfaceNfcDep,
} FurryHalNfcInterface;

typedef struct {
    FurryHalNfcType type;
    FurryHalNfcInterface interface;
    uint8_t uid_len;
    uint8_t uid[10];
    uint32_t cuid;
    uint8_t atqa[2];
    uint8_t sak;
} FurryHalNfcDevData;

typedef void (
    *FurryHalNfcTxRxSniffCallback)(uint8_t* data, uint16_t bits, bool crc_dropped, void* context);

typedef struct {
    uint8_t tx_data[FURRY_HAL_NFC_DATA_BUFF_SIZE];
    uint8_t tx_parity[FURRY_HAL_NFC_PARITY_BUFF_SIZE];
    uint16_t tx_bits;
    uint8_t rx_data[FURRY_HAL_NFC_DATA_BUFF_SIZE];
    uint8_t rx_parity[FURRY_HAL_NFC_PARITY_BUFF_SIZE];
    uint16_t rx_bits;
    FurryHalNfcTxRxType tx_rx_type;
    NfcaSignal* nfca_signal;

    FurryHalNfcTxRxSniffCallback sniff_tx;
    FurryHalNfcTxRxSniffCallback sniff_rx;
    void* sniff_context;
} FurryHalNfcTxRxContext;

/** Init nfc
 */
void furry_hal_nfc_init();

/** Deinit nfc
 */
void furry_hal_nfc_deinit();

/** Check if nfc worker is busy
 *
 * @return     true if busy
 */
bool furry_hal_nfc_is_busy();

/** Check if nfc is initialized
 *
 * @return     true if initialized
 */
bool furry_hal_nfc_is_init();

/** NFC field on
 */
void furry_hal_nfc_field_on();

/** NFC field off
 */
void furry_hal_nfc_field_off();

/** NFC start sleep
 */
void furry_hal_nfc_start_sleep();

void furry_hal_nfc_stop_cmd();

/** NFC stop sleep
 */
void furry_hal_nfc_exit_sleep();

/** NFC poll
 *
 * @param      dev_list    pointer to rfalNfcDevice buffer
 * @param      dev_cnt     pointer device count
 * @param      timeout     timeout in ms
 * @param      deactivate  deactivate flag
 *
 * @return     true on success
 */
bool furry_hal_nfc_detect(FurryHalNfcDevData* nfc_data, uint32_t timeout);

/** Activate NFC-A tag
 *
 * @param      timeout      timeout in ms
 * @param      cuid         pointer to 32bit uid
 *
 * @return     true on succeess
 */
bool furry_hal_nfc_activate_nfca(uint32_t timeout, uint32_t* cuid);

/** NFC listen
 *
 * @param      uid                 pointer to uid buffer
 * @param      uid_len             uid length
 * @param      atqa                pointer to atqa
 * @param      sak                 sak
 * @param      activate_after_sak  activate after sak flag
 * @param      timeout             timeout in ms
 *
 * @return     true on success
 */
bool furry_hal_nfc_listen(
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak,
    bool activate_after_sak,
    uint32_t timeout);

/** Start Target Listen mode
 * @note RFAL free implementation
 *
 * @param       nfc_data            FurryHalNfcDevData instance
 */
void furry_hal_nfc_listen_start(FurryHalNfcDevData* nfc_data);

/** Read data in Target Listen mode
 * @note Must be called only after furry_hal_nfc_listen_start()
 *
 * @param       tx_rx               FurryHalNfcTxRxContext instance
 * @param       timeout_ms          timeout im ms
 *
 * @return      true on not empty receive
 */
bool furry_hal_nfc_listen_rx(FurryHalNfcTxRxContext* tx_rx, uint32_t timeout_ms);

/** Set Target in Sleep state */
void furry_hal_nfc_listen_sleep();

/** Emulate NFC-A Target
 * @note RFAL based implementation
 *
 * @param       uid                 NFC-A UID
 * @param       uid_len             NFC-A UID length
 * @param       atqa                NFC-A ATQA
 * @param       sak                 NFC-A SAK
 * @param       callback            FurryHalNfcEmulateCallback instance
 * @param       context             pointer to context for callback
 * @param       timeout             timeout in ms
 *
 * @return      true on success
 */
bool furry_hal_nfc_emulate_nfca(
    uint8_t* uid,
    uint8_t uid_len,
    uint8_t* atqa,
    uint8_t sak,
    FurryHalNfcEmulateCallback callback,
    void* context,
    uint32_t timeout);

/** NFC data exchange
 *
 * @param       tx_rx_ctx   FurryHalNfcTxRxContext instance
 *
 * @return      true on success
 */
bool furry_hal_nfc_tx_rx(FurryHalNfcTxRxContext* tx_rx, uint16_t timeout_ms);

/** NFC data full exhange
 *
 * @param       tx_rx_ctx   FurryHalNfcTxRxContext instance
 *
 * @return      true on success
 */
bool furry_hal_nfc_tx_rx_full(FurryHalNfcTxRxContext* tx_rx);

/** NFC deactivate and start sleep
 */
void furry_hal_nfc_sleep();

void furry_hal_nfc_stop();

/* Low level transport API, use it to implement your own transport layers */

#define furry_hal_nfc_ll_ms2fc rfalConvMsTo1fc

#define FURRY_HAL_NFC_LL_TXRX_FLAGS_CRC_TX_MANUAL RFAL_TXRX_FLAGS_CRC_TX_MANUAL
#define FURRY_HAL_NFC_LL_TXRX_FLAGS_AGC_ON RFAL_TXRX_FLAGS_AGC_ON
#define FURRY_HAL_NFC_LL_TXRX_FLAGS_PAR_RX_REMV RFAL_TXRX_FLAGS_PAR_RX_REMV
#define FURRY_HAL_NFC_LL_TXRX_FLAGS_CRC_RX_KEEP RFAL_TXRX_FLAGS_CRC_RX_KEEP

typedef enum {
    FurryHalNfcReturnOk = 0, /*!< no error occurred */
    FurryHalNfcReturnNomem = 1, /*!< not enough memory to perform the requested operation */
    FurryHalNfcReturnBusy = 2, /*!< device or resource busy */
    FurryHalNfcReturnIo = 3, /*!< generic IO error */
    FurryHalNfcReturnTimeout = 4, /*!< error due to timeout */
    FurryHalNfcReturnRequest =
        5, /*!< invalid request or requested function can't be executed at the moment */
    FurryHalNfcReturnNomsg = 6, /*!< No message of desired type */
    FurryHalNfcReturnParam = 7, /*!< Parameter error */
    FurryHalNfcReturnSystem = 8, /*!< System error */
    FurryHalNfcReturnFraming = 9, /*!< Framing error */
    FurryHalNfcReturnOverrun = 10, /*!< lost one or more received bytes */
    FurryHalNfcReturnProto = 11, /*!< protocol error */
    FurryHalNfcReturnInternal = 12, /*!< Internal Error */
    FurryHalNfcReturnAgain = 13, /*!< Call again */
    FurryHalNfcReturnMemCorrupt = 14, /*!< memory corruption */
    FurryHalNfcReturnNotImplemented = 15, /*!< not implemented */
    FurryHalNfcReturnPcCorrupt =
        16, /*!< Program Counter has been manipulated or spike/noise trigger illegal operation */
    FurryHalNfcReturnSend = 17, /*!< error sending*/
    FurryHalNfcReturnIgnore = 18, /*!< indicates error detected but to be ignored */
    FurryHalNfcReturnSemantic = 19, /*!< indicates error in state machine (unexpected cmd) */
    FurryHalNfcReturnSyntax = 20, /*!< indicates error in state machine (unknown cmd) */
    FurryHalNfcReturnCrc = 21, /*!< crc error */
    FurryHalNfcReturnNotfound = 22, /*!< transponder not found */
    FurryHalNfcReturnNotunique =
        23, /*!< transponder not unique - more than one transponder in field */
    FurryHalNfcReturnNotsupp = 24, /*!< requested operation not supported */
    FurryHalNfcReturnWrite = 25, /*!< write error */
    FurryHalNfcReturnFifo = 26, /*!< fifo over or underflow error */
    FurryHalNfcReturnPar = 27, /*!< parity error */
    FurryHalNfcReturnDone = 28, /*!< transfer has already finished */
    FurryHalNfcReturnRfCollision =
        29, /*!< collision error (Bit Collision or during RF Collision avoidance ) */
    FurryHalNfcReturnHwOverrun = 30, /*!< lost one or more received bytes */
    FurryHalNfcReturnReleaseReq = 31, /*!< device requested release */
    FurryHalNfcReturnSleepReq = 32, /*!< device requested sleep */
    FurryHalNfcReturnWrongState = 33, /*!< incorrent state for requested operation */
    FurryHalNfcReturnMaxReruns = 34, /*!< blocking procedure reached maximum runs */
    FurryHalNfcReturnDisabled = 35, /*!< operation aborted due to disabled configuration */
    FurryHalNfcReturnHwMismatch = 36, /*!< expected hw do not match  */
    FurryHalNfcReturnLinkLoss =
        37, /*!< Other device's field didn't behave as expected: turned off by Initiator in Passive mode, or AP2P did not turn on field */
    FurryHalNfcReturnInvalidHandle = 38, /*!< invalid or not initalized device handle */
    FurryHalNfcReturnIncompleteByte = 40, /*!< Incomplete byte rcvd         */
    FurryHalNfcReturnIncompleteByte01 = 41, /*!< Incomplete byte rcvd - 1 bit */
    FurryHalNfcReturnIncompleteByte02 = 42, /*!< Incomplete byte rcvd - 2 bit */
    FurryHalNfcReturnIncompleteByte03 = 43, /*!< Incomplete byte rcvd - 3 bit */
    FurryHalNfcReturnIncompleteByte04 = 44, /*!< Incomplete byte rcvd - 4 bit */
    FurryHalNfcReturnIncompleteByte05 = 45, /*!< Incomplete byte rcvd - 5 bit */
    FurryHalNfcReturnIncompleteByte06 = 46, /*!< Incomplete byte rcvd - 6 bit */
    FurryHalNfcReturnIncompleteByte07 = 47, /*!< Incomplete byte rcvd - 7 bit */
} FurryHalNfcReturn;

typedef enum {
    FurryHalNfcModeNone = 0, /*!< No mode selected/defined */
    FurryHalNfcModePollNfca = 1, /*!< Mode to perform as NFCA (ISO14443A) Poller (PCD) */
    FurryHalNfcModePollNfcaT1t = 2, /*!< Mode to perform as NFCA T1T (Topaz) Poller (PCD) */
    FurryHalNfcModePollNfcb = 3, /*!< Mode to perform as NFCB (ISO14443B) Poller (PCD) */
    FurryHalNfcModePollBPrime = 4, /*!< Mode to perform as B' Calypso (Innovatron) (PCD) */
    FurryHalNfcModePollBCts = 5, /*!< Mode to perform as CTS Poller (PCD) */
    FurryHalNfcModePollNfcf = 6, /*!< Mode to perform as NFCF (FeliCa) Poller (PCD) */
    FurryHalNfcModePollNfcv = 7, /*!< Mode to perform as NFCV (ISO15963) Poller (PCD) */
    FurryHalNfcModePollPicopass = 8, /*!< Mode to perform as PicoPass / iClass Poller (PCD) */
    FurryHalNfcModePollActiveP2p = 9, /*!< Mode to perform as Active P2P (ISO18092) Initiator  */
    FurryHalNfcModeListenNfca = 10, /*!< Mode to perform as NFCA (ISO14443A) Listener (PICC) */
    FurryHalNfcModeListenNfcb = 11, /*!< Mode to perform as NFCA (ISO14443B) Listener (PICC) */
    FurryHalNfcModeListenNfcf = 12, /*!< Mode to perform as NFCA (ISO15963) Listener (PICC) */
    FurryHalNfcModeListenActiveP2p = 13 /*!< Mode to perform as Active P2P (ISO18092) Target  */
} FurryHalNfcMode;

typedef enum {
    FurryHalNfcBitrate106 = 0, /*!< Bit Rate 106 kbit/s (fc/128) */
    FurryHalNfcBitrate212 = 1, /*!< Bit Rate 212 kbit/s (fc/64) */
    FurryHalNfcBitrate424 = 2, /*!< Bit Rate 424 kbit/s (fc/32) */
    FurryHalNfcBitrate848 = 3, /*!< Bit Rate 848 kbit/s (fc/16) */
    FurryHalNfcBitrate1695 = 4, /*!< Bit Rate 1695 kbit/s (fc/8) */
    FurryHalNfcBitrate3390 = 5, /*!< Bit Rate 3390 kbit/s (fc/4) */
    FurryHalNfcBitrate6780 = 6, /*!< Bit Rate 6780 kbit/s (fc/2) */
    FurryHalNfcBitrate13560 = 7, /*!< Bit Rate 13560 kbit/s (fc) */
    FurryHalNfcBitrate52p97 = 0xEB, /*!< Bit Rate 52.97 kbit/s (fc/256) Fast Mode VICC->VCD */
    FurryHalNfcBitrate26p48 =
        0xEC, /*!< Bit Rate 26,48 kbit/s (fc/512) NFCV VICC->VCD & VCD->VICC 1of4 */
    FurryHalNfcBitrate1p66 = 0xED, /*!< Bit Rate 1,66 kbit/s (fc/8192) NFCV VCD->VICC 1of256 */
    FurryHalNfcBitrateKeep = 0xFF /*!< Value indicating to keep the same previous bit rate */
} FurryHalNfcBitrate;

FurryHalNfcReturn furry_hal_nfc_ll_set_mode(
    FurryHalNfcMode mode,
    FurryHalNfcBitrate txBR,
    FurryHalNfcBitrate rxBR);

#define FURRY_HAL_NFC_LL_GT_NFCA \
    furry_hal_nfc_ll_ms2fc(5U) /*!< GTA  Digital 2.0  6.10.4.1 & B.2 */
#define FURRY_HAL_NFC_LL_GT_NFCB \
    furry_hal_nfc_ll_ms2fc(5U) /*!< GTB  Digital 2.0  7.9.4.1  & B.3 */
#define FURRY_HAL_NFC_LL_GT_NFCF \
    furry_hal_nfc_ll_ms2fc(20U) /*!< GTF  Digital 2.0  8.7.4.1  & B.4 */
#define FURRY_HAL_NFC_LL_GT_NFCV \
    furry_hal_nfc_ll_ms2fc(5U) /*!< GTV  Digital 2.0  9.7.5.1  & B.5 */
#define FURRY_HAL_NFC_LL_GT_PICOPASS furry_hal_nfc_ll_ms2fc(1U) /*!< GT Picopass */
#define FURRY_HAL_NFC_LL_GT_AP2P furry_hal_nfc_ll_ms2fc(5U) /*!< TIRFG  Ecma 340  11.1.1 */
#define FURRY_HAL_NFC_LL_GT_AP2P_ADJUSTED \
    furry_hal_nfc_ll_ms2fc(               \
        5U +                              \
        25U) /*!< Adjusted GT for greater interoperability (Sony XPERIA P, Nokia N9, Huawei P2) */

void furry_hal_nfc_ll_set_guard_time(uint32_t cycles);

typedef enum {
    FurryHalNfcErrorHandlingNone = 0, /*!< No special error handling will be performed */
    FurryHalNfcErrorHandlingNfc = 1, /*!< Error handling set to perform as NFC compliant device */
    FurryHalNfcErrorHandlingEmvco =
        2 /*!< Error handling set to perform as EMVCo compliant device */
} FurryHalNfcErrorHandling;

void furry_hal_nfc_ll_set_error_handling(FurryHalNfcErrorHandling eHandling);

/* RFAL Frame Delay Time (FDT) Listen default values   */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_NFCA_POLLER \
    1172U /*!< FDTA,LISTEN,MIN (n=9) Last bit: Logic "1" - tnn,min/2 Digital 1.1  6.10 ;  EMV CCP Spec Book D v2.01  4.8.1.3 */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_NFCB_POLLER \
    1008U /*!< TR0B,MIN         Digital 1.1  7.1.3 & A.3  ; EMV CCP Spec Book D v2.01  4.8.1.3 & Table A.5 */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_NFCF_POLLER \
    2672U /*!< TR0F,LISTEN,MIN  Digital 1.1  8.7.1.1 & A.4 */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_NFCV_POLLER \
    4310U /*!< FDTV,LISTEN,MIN  t1 min       Digital 2.1  B.5  ;  ISO15693-3 2009  9.1 */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_PICOPASS_POLLER \
    3400U /*!< ISO15693 t1 min - observed adjustment */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_AP2P_POLLER \
    64U /*!< FDT AP2P No actual FDTListen is required as fields switch and collision avoidance */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_NFCA_LISTENER 1172U /*!< FDTA,LISTEN,MIN  Digital 1.1  6.10 */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_NFCB_LISTENER \
    1024U /*!< TR0B,MIN         Digital 1.1  7.1.3 & A.3  ;  EMV CCP Spec Book D v2.01  4.8.1.3 & Table A.5 */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_NFCF_LISTENER \
    2688U /*!< TR0F,LISTEN,MIN  Digital 2.1  8.7.1.1 & B.4 */
#define FURRY_HAL_NFC_LL_FDT_LISTEN_AP2P_LISTENER \
    64U /*!< FDT AP2P No actual FDTListen exists as fields switch and collision avoidance */

void furry_hal_nfc_ll_set_fdt_listen(uint32_t cycles);

/*  RFAL Frame Delay Time (FDT) Poll default values    */
#define FURRY_HAL_NFC_LL_FDT_POLL_NFCA_POLLER \
    6780U /*!< FDTA,POLL,MIN   Digital 1.1  6.10.3.1 & A.2 */
#define FURRY_HAL_NFC_LL_FDT_POLL_NFCA_T1T_POLLER \
    384U /*!< RRDDT1T,MIN,B1  Digital 1.1  10.7.1 & A.5 */
#define FURRY_HAL_NFC_LL_FDT_POLL_NFCB_POLLER \
    6780U /*!< FDTB,POLL,MIN = TR2B,MIN,DEFAULT Digital 1.1 7.9.3 & A.3  ;  EMVCo 3.0 FDTB,PCD,MIN  Table A.5 */
#define FURRY_HAL_NFC_LL_FDT_POLL_NFCF_POLLER \
    6800U /*!< FDTF,POLL,MIN   Digital 2.1  8.7.3 & B.4 */
#define FURRY_HAL_NFC_LL_FDT_POLL_NFCV_POLLER 4192U /*!< FDTV,POLL  Digital 2.1  9.7.3.1  & B.5 */
#define FURRY_HAL_NFC_LL_FDT_POLL_PICOPASS_POLLER 1790U /*!< FDT Max */
#define FURRY_HAL_NFC_LL_FDT_POLL_AP2P_POLLER \
    0U /*!< FDT AP2P No actual FDTPoll exists as fields switch and collision avoidance */

void furry_hal_nfc_ll_set_fdt_poll(uint32_t FDTPoll);

void furry_hal_nfc_ll_txrx_on();

void furry_hal_nfc_ll_txrx_off();

FurryHalNfcReturn furry_hal_nfc_ll_txrx(
    uint8_t* txBuf,
    uint16_t txBufLen,
    uint8_t* rxBuf,
    uint16_t rxBufLen,
    uint16_t* actLen,
    uint32_t flags,
    uint32_t fwt);

FurryHalNfcReturn furry_hal_nfc_ll_txrx_bits(
    uint8_t* txBuf,
    uint16_t txBufLen,
    uint8_t* rxBuf,
    uint16_t rxBufLen,
    uint16_t* actLen,
    uint32_t flags,
    uint32_t fwt);

void furry_hal_nfc_ll_poll();

void furry_hal_nfc_field_detect_start();

bool furry_hal_nfc_field_is_present();

#ifdef __cplusplus
}
#endif
