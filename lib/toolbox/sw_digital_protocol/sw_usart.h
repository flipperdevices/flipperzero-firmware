#pragma once
#include <furi.h>
#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SwUsart SwUsart;

typedef enum {
    SwUsartModeOnlyAsyncRx, // Only receive data asynchronously
    SwUsartModeOnlySyncRx, // Only receive data synchronously

    SwUsartModeOnlyAsyncTx, // Only transmit data asynchronously
    SwUsartModeOnlySyncTx, // Only transmit data synchronously

    SwUsartModeAsyncRxTx, // Receive and transmit data asynchronously
    SwUsartModeSyncRxTx, // Receive and transmit data synchronously

    SwUsartModeAsyncRxTxHalfDuplex, // Receive and transmit data asynchronously in half-duplex mode
    SwUsartModeSyncRxTxHalfDuplex, // Receive and transmit data synchronously in half-duplex mode
} SwUsartMode;

typedef enum {
    SwUsartWordLength5b = 5U,
    SwUsartWordLength6b = 6U,
    SwUsartWordLength7b = 7U,
    SwUsartWordLength8b = 8U,
    SwUsartWordLength9b = 9U,
} SwUsartWordLength;

typedef enum {
    SwUsartStopBits1 = 1U,
    SwUsartStopBits2 = 2U,
} SwUsartStopBits;

typedef enum {
    SwUsartParityNone = 0U,
    SwUsartParityEven = 2U,
    SwUsartParityOdd = 3U,
} SwUsartParity;

typedef struct {
    SwUsartMode mode;
    uint32_t baud_rate;
    SwUsartWordLength word_length;
    SwUsartParity parity;
    SwUsartStopBits stop_bits;
    const GpioPin* rx_pin;
    const GpioPin* tx_pin;
} SwUsartConfig;

SwUsart* sw_usart_alloc(SwUsartConfig* config);
void sw_usart_free(SwUsart* sw_usart);
uint32_t sw_usart_tx_encoder_yield(void* context);
void sw_usart_dma_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len);
bool sw_usart_is_end_tx(SwUsart* sw_usart);

#ifdef __cplusplus
}
#endif