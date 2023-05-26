#pragma once

#include <stddef.h>
#include <stdint.h>
#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint16_t* rx_buff;
    size_t rx_buff_size;
} SwDigitalPinRx;

typedef uint32_t (*FuriHalSwDigitalPinTxCallbackYield)(void* context);
typedef void (*FuriHalSwDigitalPinTxCallbackEnd)(void* context);
typedef void (*FuriHalSwDigitalPinRxCallback)(void* context, SwDigitalPinRx data);

/**
 * @brief Initialize sync clock
 * @param gpio GPIO pin.
 * @param inverse If false - rising edge, if true - falling edge.
 */
void furi_hal_sw_digital_pin_sync_init(const GpioPin* gpio, bool inverse);

/**
 * @brief Start out sync clock
 */
void furi_hal_sw_digital_pin_sync_start(void);

/**
 * @brief Stop out sync clock
 */
void furi_hal_sw_digital_pin_sync_stop(void);

/**
 * @brief Check if sync clock is running
 * @return true if sync clock is running, false otherwise
 */
bool furi_hal_sw_digital_pin_is_sync_running(void);

/**
 * @brief Deinitialize sync clock
 */
void furi_hal_sw_digital_pin_sync_deinit(void);

/**
 * @brief Initialize Tx pin
 * @param tx_callback_yield Callback to yield data.
 * @param tx_callback_end Callback to end data.
 * @param context Context for callbacks.
 * @param tim_psc Timer prescaler.
 * @param tim_arr Timer auto reload register.
 * @param samples Size of samples.
 * @param gpio GPIO Tx pin.
 */
void furi_hal_sw_digital_pin_tx_init(
    FuriHalSwDigitalPinTxCallbackYield tx_callback_yield,
    FuriHalSwDigitalPinTxCallbackEnd tx_callback_end,
    void* context,
    uint16_t tim_psc,
    uint32_t tim_arr,
    size_t samples,
    const GpioPin* gpio);

/** 
 * @brief Start of data transfer
 */
void furi_hal_sw_digital_pin_tx_start(void);

/**
 * @brief Check if the data transfer is completed
 * @return true if the data transfer is completed, false otherwise
 */
bool furi_hal_sw_digital_pin_is_tx_complete(void);

/**
 * @brief Forced stop of data transmission
 */
void furi_hal_sw_digital_pin_tx_stop(void);

/**
 * @brief Deinitialize tx pin
 */
void furi_hal_sw_digital_pin_tx_deinit(void);

/**
 * @brief Initialize Rx pin
 * @param rx_callback Callback to receive data.
 * @param context Context for callback.
 * @param tim_psc Timer prescaler.
 * @param tim_arr Timer auto reload register.
 * @param samples Size of samples.
 * @param gpio GPIO Rx pin.
 */
void furi_hal_sw_digital_pin_rx_init(
    FuriHalSwDigitalPinRxCallback rx_callback,
    void* context,
    uint16_t tim_psc,
    uint32_t tim_arr,
    size_t samples,
    const GpioPin* gpio);

/**
 * @brief Start of data reception
 * 
 */
void furi_hal_sw_digital_pin_rx_start(void);

/**
 * @brief Stop of data reception
 */
void furi_hal_sw_digital_pin_rx_stop(void);

/**
 * @brief Check if the data reception is running
 * @return true if the data reception is running, false otherwise
 */
bool furi_hal_sw_digital_pin_is_rx_running(void);

/**
 * @brief Deinitialize Rx pin
 */
void furi_hal_sw_digital_pin_rx_deinit(void);

/**
 * @brief Switching from reception to transmission in half duplex mode
 */
void furi_hal_sw_digital_pin_switch_rx_to_tx(void);

/**
 * @brief Switching from transmission to reception in half duplex mode
 */
void furi_hal_sw_digital_pin_switch_tx_to_rx(void);

#ifdef __cplusplus
}
#endif
