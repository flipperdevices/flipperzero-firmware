#pragma once

#include <rfal_nfc.h>
#include <furi/check.h>

#define NFC_TXRX_DEFAULT                                                             \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_AUTO | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_REMV | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_REMV | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_AUTO)

#define NFC_TX_DEFAULT_RX_NO_CRC                                                     \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_AUTO | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_KEEP | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_REMV | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_AUTO)

#define NFC_TXRX_WITH_PAR                                                              \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_MANUAL | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_KEEP | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_KEEP | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_AUTO)

#define NFC_TXRX_RAW                                                                   \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_MANUAL | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_KEEP | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_KEEP | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_NONE)

#define NFC_TX_RAW_RX_DEFAULT                                                          \
    ((uint32_t)RFAL_TXRX_FLAGS_CRC_TX_MANUAL | (uint32_t)RFAL_TXRX_FLAGS_CRC_RX_REMV | \
     (uint32_t)RFAL_TXRX_FLAGS_PAR_RX_REMV | (uint32_t)RFAL_TXRX_FLAGS_PAR_TX_NONE)
