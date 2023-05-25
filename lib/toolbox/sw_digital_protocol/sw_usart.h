#pragma once
#include <furi.h>
#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SwUsart SwUsart;

#define SW_USART_DEFAULT_TIMEOUT 1000

typedef enum {
    SwUsartModeOnlyRx, // Only receive data
    SwUsartModeOnlyTx, // Only transmit data
    SwUsartModeRxTx, // Receive and transmit data
    SwUsartModeRxTxHalfDuplex, // Receive and transmit data in half-duplex mode

} SwUsartMode;

typedef enum {
    SwUsartDataBit5 = 5U,
    SwUsartDataBit6 = 6U,
    SwUsartDataBit7 = 7U,
    SwUsartDataBit8 = 8U,
} SwUsartWordLength;

typedef enum {
    SwUsartStopBit1 = 1U,
    SwUsartStopBit2 = 2U,
} SwUsartStopBits;

typedef enum {
    SwUsartParityNone = 0U,
    SwUsartParityEven = 2U,
    SwUsartParityOdd = 3U,
} SwUsartParity;

SwUsart* sw_usart_alloc(void);
void sw_usart_free(SwUsart* sw_usart);
void sw_usart_start(
    SwUsart* sw_usart,
    uint32_t baudrate,
    const GpioPin* tx_pin,
    const GpioPin* rx_pin,
    bool inverted);
void sw_usart_set_rx_buffer_size(SwUsart* sw_usart, size_t size);
void sw_usart_set_config(
    SwUsart* sw_usart,
    SwUsartMode mode,
    SwUsartWordLength data_bit,
    SwUsartParity parity,
    SwUsartStopBits stop_bit);
void sw_usart_sync_start(SwUsart* sw_usart);
void sw_usart_sync_stop(SwUsart* sw_usart);
void sw_usart_set_sync_config(SwUsart* sw_usart, const GpioPin* gpio, bool inverse);
void sw_usart_dma_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len);
void sw_usart_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len, uint32_t timeout_ms);

bool sw_usart_is_end_tx(SwUsart* sw_usart);
void sw_usart_print_data(SwUsart* sw_usart);

size_t sw_usart_available(SwUsart* sw_usart);
bool sw_usart_read(SwUsart* sw_usart, uint8_t* data, uint32_t timeout_ms);
void sw_usart_read_line(SwUsart* sw_usart, FuriString* data, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif