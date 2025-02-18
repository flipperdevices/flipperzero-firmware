#pragma once

#include <furi.h>

/**
 * UART channels
 */
typedef enum {
    FuriHalSerialIdUsart,
    FuriHalSerialIdLpuart,

    FuriHalSerialIdMax,
} FuriHalSerialId;

typedef enum {
    FuriHalSerialDirectionTx,
    FuriHalSerialDirectionRx,

    FuriHalSerialDirectionMax,
} FuriHalSerialDirection;

/**
 * @brief Actual data bits, i.e. not including start/stop and parity bits
 * @note 6 data bits are only permitted when parity is enabled
 * @note 9 data bits are only permitted when parity is disabled
 */
typedef enum {
    FuriHalSerialDataBits6 = 1,
    FuriHalSerialDataBits7 = 1 << 28,
    FuriHalSerialDataBits8 = 0,
    FuriHalSerialDataBits9 = 1 << 12,
} FuriHalSerialDataBits;

typedef enum {
    FuriHalSerialParityNone = 0,
    FuriHalSerialParityEven = 1 << 10,
    FuriHalSerialParityOdd = ((1 << 10) | (1 << 9)),
} FuriHalSerialParity;

/**
 * @brief Stop bit length
 * @note LPUART only supports whole stop bit lengths (i.e. 1 and 2, but not 0.5 and 1.5)
 */
typedef enum {
    FuriHalSerialStopBits0_5 = 1 << 12,
    FuriHalSerialStopBits1 = 0,
    FuriHalSerialStopBits1_5 = 3 << 12,
    FuriHalSerialStopBits2 = 2 << 12,
} FuriHalSerialStopBits;

typedef struct FuriHalSerialHandle FuriHalSerialHandle;
