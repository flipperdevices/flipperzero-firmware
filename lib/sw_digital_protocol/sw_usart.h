#pragma once
#include <furi.h>
#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SwUsart SwUsart;

typedef void (*SwUsartTxCallbackEnd)(void* context);

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

/**
 * @brief Allocate SwUsart
 * @return SwUsart pointer
 */
SwUsart* sw_usart_alloc(void);

/**
 * @brief Free SwUsart
 * @param sw_usart SwUsart pointer
 */
void sw_usart_free(SwUsart* sw_usart);

/**
 * @brief Set callback at the end of the transfer
 * @param sw_usart SwUsart pointer
 * @param callback Callback
 * @param context Context
 */
void sw_usart_set_tx_callback(SwUsart* sw_usart, SwUsartTxCallbackEnd callback, void* context);

/**
 * @brief Start SwUsart
 * @param sw_usart SwUsart pointer
 * @param baudrate Baudrate
 * @param tx_pin Tx pin
 * @param rx_pin Rx pin
 * @param inverted If false - normal bit transmission levels, if true - then inverted
 */
void sw_usart_start(
    SwUsart* sw_usart,
    uint32_t baudrate,
    const GpioPin* tx_pin,
    const GpioPin* rx_pin,
    bool inverted);

/**
 * @brief Set receive buffer size
 * @param sw_usart SwUsart pointer
 * @param size New size of receive buffer
 */
void sw_usart_set_rx_buffer_size(SwUsart* sw_usart, size_t size);

/**
 * @brief Set the package format to something other than the default
 * @note Default setting: 1 start bit, 8 data bits, no parity, 1 stop bit 
 * @param sw_usart SwUsart pointer
 * @param mode Mode
 * @param data_bit Data bit
 * @param parity Parity
 * @param stop_bit Stop bit
 */
void sw_usart_set_config(
    SwUsart* sw_usart,
    SwUsartMode mode,
    SwUsartWordLength data_bit,
    SwUsartParity parity,
    SwUsartStopBits stop_bit);

/**
 * @brief Start Sync clock output
 * @param sw_usart SwUsart pointer 
 */
void sw_usart_sync_start(SwUsart* sw_usart);

/**
 * @brief Stop Sync clock output
 * @param sw_usart SwUsart pointer 
 */
void sw_usart_sync_stop(SwUsart* sw_usart);

/**
 * @brief Set Sync clock output config
 * @param sw_usart SwUsart pointer 
 * @param gpio Gpio pin
 * @param inverse If false - rising edge, if true - falling edge.
 */
void sw_usart_set_sync_config(SwUsart* sw_usart, const GpioPin* gpio, bool inverse);

/**
 * @brief Start transmission in non-blocking mode
 * @param sw_usart SwUsart pointer
 * @param data Data pointer
 * @param len Data length
 */
void sw_usart_dma_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len);

/**
 * @brief Start transmission in blocking mode
 * @param sw_usart SwUsart pointer
 * @param data Data pointer
 * @param len Data length
 * @param timeout_ms Timeout in milliseconds
 */
void sw_usart_tx(SwUsart* sw_usart, uint8_t* data, uint8_t len, uint32_t timeout_ms);

/**
 * @brief Check if the transfer is completed in non-blocking mode
 * @param sw_usart SwUsart pointer
 */
bool sw_usart_is_end_tx(SwUsart* sw_usart);

/**
 * @brief Debug output of data in the receive buffer
 * @param sw_usart SwUsart pointer
 */
void sw_usart_print_debug_data(SwUsart* sw_usart);

/**
 * @brief Get the number of bytes in the receive buffer
 * @param sw_usart SwUsart pointer
 * @return Number of bytes in the receive buffer
 */
size_t sw_usart_available(SwUsart* sw_usart);

/**
 * @brief Read 1 byte data from the receive buffer
 * @param sw_usart SwUsart pointer
 * @param data Data pointer
 * @return True if data is read, false if timeout
 */
bool sw_usart_read(SwUsart* sw_usart, uint8_t* data);

/**
 * @brief Read line from the receive buffer
 * @param sw_usart SwUsart pointer
 * @param data Data furistring pointer
 * @param timeout_ms Timeout in milliseconds
 * @return True if data is read, false if timeout
 */
void sw_usart_read_line(SwUsart* sw_usart, FuriString* data, uint32_t timeout_ms);

#ifdef __cplusplus
}
#endif