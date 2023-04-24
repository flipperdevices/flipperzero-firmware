#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define NFCB_UID_MAX_SIZE (7)

// TODO change values
#define NFCB_GUARD_TIME_US (5000)
#define NFCB_FDT_POLL_FC (1620)
#define NFCB_FDT_LISTEN_FC (1172)
#define NFCB_POLLER_MASK_RX_FS ((NFCA_FDT_LISTEN_FC) / 2)
#define NFCB_POLL_POLL_MIN_US (1100)

typedef enum {
    NfcbErrorNone,
    NfcbErrorTimeout,
} NfcbError;

typedef struct {
    uint8_t uid[NFCB_UID_MAX_SIZE];
    uint16_t uid_len;
} NfcbData;

#ifdef __cplusplus
}
#endif
