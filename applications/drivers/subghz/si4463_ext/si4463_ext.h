/**
 * @file furi_hal_subghz.h
 * SubGhz HAL API
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <toolbox/level_duration.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Radio Presets */
typedef enum {
    FuriHalSubGhzPresetIDLE, /**< default configuration */
    FuriHalSubGhzPresetOok270Async, /**< OOK, bandwidth 270kHz, asynchronous */
    FuriHalSubGhzPresetOok650Async, /**< OOK, bandwidth 650kHz, asynchronous */
    FuriHalSubGhzPresetOok650AsyncFreq, /**< OOK, bandwidth 650kHz, asynchronous, 500bps, for frequency analyzer */
    FuriHalSubGhzPreset2FSKDev238Async, /**< FM, deviation 2.380371 kHz, asynchronous */
    FuriHalSubGhzPreset2FSKDev476Async, /**< FM, deviation 4.760742 kHz, asynchronous */
    FuriHalSubGhzPresetMSK99_97KbAsync, /**< MSK, deviation 47.60742 kHz, 99.97Kb/s, asynchronous */
    FuriHalSubGhzPresetGFSK9_99KbAsync /**< GFSK, deviation 19.042969 kHz, 9.996Kb/s, asynchronous */
} FuriHalSubGhzPreset;

/** Switchable Radio Paths */
typedef enum {
    FuriHalSubGhzPathIsolate, /**< Isolate Radio from antenna */
    FuriHalSubGhzPath433, /**< Center Frquency: 433MHz. Path 1: SW1RF1-SW2RF2, LCLCL */
    FuriHalSubGhzPath315, /**< Center Frquency: 315MHz. Path 2: SW1RF2-SW2RF1, LCLCLCL */
    FuriHalSubGhzPath868, /**< Center Frquency: 868MHz. Path 3: SW1RF3-SW2RF3, LCLC */
} FuriHalSubGhzPath;

/** SubGhz state */
typedef enum {
    SubGhzStateInit, /**< Init pending */

    SubGhzStateIdle, /**< Idle, energy save mode */

    SubGhzStateAsyncRx, /**< Async RX started */

    SubGhzStateAsyncTx, /**< Async TX started, DMA and timer is on */
    SubGhzStateAsyncTxLast, /**< Async TX continue, DMA completed and timer got last value to go */
    SubGhzStateAsyncTxEnd, /**< Async TX complete, cleanup needed */

} SubGhzState;

/** SubGhz regulation, receive transmission on the current frequency for the
 * region */
typedef enum {
    SubGhzRegulationOnlyRx, /**only Rx*/
    SubGhzRegulationTxRx, /**TxRx*/
} SubGhzRegulation;

/** Initialize and switch to power save mode Used by internal API-HAL
 * initalization routine Can be used to reinitialize device to safe state and
 * send it to sleep
 */
void subghz_device_si4463_ext_init();

/** Send device to sleep mode
 */
void subghz_device_si4463_ext_sleep();

/** Dump info to stdout
 */
void subghz_device_si4463_ext_dump_state();

/** Load registers from preset by preset name
 *
 * @param      preset  to load
 */
void subghz_device_si4463_ext_load_preset(FuriHalSubGhzPreset preset);

/** Load registers
 *
 * @param      data  Registers data
 */
void subghz_device_si4463_ext_load_registers(const uint8_t data[][2]);
void subghz_device_si4463_ext_load_config(const uint8_t config[]);

/** Load PATABLE
 *
 * @param      data  8 uint8_t values
 */
void subghz_device_si4463_ext_load_patable(const uint8_t data[8]);

/** Write packet to FIFO
 *
 * @param      data  bytes array
 * @param      size  size
 */
void subghz_device_si4463_ext_write_packet(const uint8_t* data, uint8_t size);

/** Check if recieve pipe is not empty
 *
 * @return     true if not empty
 */
bool subghz_device_si4463_ext_rx_pipe_not_empty();

/** Check if recieved data crc is valid
 *
 * @return     true if valid
 */
bool subghz_device_si4463_ext_is_rx_data_crc_valid();

/** Read packet from FIFO
 *
 * @param      data  pointer
 * @param      size  size
 */
void subghz_device_si4463_ext_read_packet(uint8_t* data, uint8_t* size);

/** Flush rx FIFO buffer
 */
void subghz_device_si4463_ext_flush_rx();

/** Flush tx FIFO buffer
 */
void subghz_device_si4463_ext_flush_tx();

/** Shutdown Issue spwd command
 * @warning    registers content will be lost
 */
void subghz_device_si4463_ext_shutdown();

/** Reset Issue reset command
 * @warning    registers content will be lost
 */
void subghz_device_si4463_ext_reset();

/** Switch to Idle
 */
void subghz_device_si4463_ext_idle();

/** Switch to Recieve
 */
void subghz_device_si4463_ext_rx();

/** Switch to Transmit
 *
 * @return     true if the transfer is allowed by belonging to the region
 */
bool subghz_device_si4463_ext_tx();

/** Get RSSI value in dBm
 *
 * @return     RSSI value
 */
float subghz_device_si4463_ext_get_rssi();

/** Get LQI
 *
 * @return     LQI value
 */
uint8_t subghz_device_si4463_ext_get_lqi();

/** Check if frequency is in valid range
 *
 * @param      value  frequency in Hz
 *
 * @return     true if frequncy is valid, otherwise false
 */
bool subghz_device_si4463_ext_is_frequency_valid(uint32_t value);

/** Set frequency and path This function automatically selects antenna matching
 * network
 *
 * @param      value  frequency in Hz
 *
 * @return     real frequency in herz
 */
uint32_t subghz_device_si4463_ext_set_frequency_and_path(uint32_t value);

/** Ñheck if transmission is allowed on this frequency for your flipper region
 *
 * @param      value  frequency in Hz
 *
 * @return     true if allowed
 */
bool subghz_device_si4463_ext_is_tx_allowed(uint32_t value);

/** Set frequency
 *
 * @param      value  frequency in Hz
 *
 * @return     real frequency in herz
 */
uint32_t subghz_device_si4463_ext_set_frequency(uint32_t value);

/** Set path
 *
 * @param      path  path to use
 */
void subghz_device_si4463_ext_set_path(FuriHalSubGhzPath path);

/* High Level API */

/** Signal Timings Capture callback */
typedef void (*FuriHalSubGhzCaptureCallback)(bool level, uint32_t duration, void* context);

/** Enable signal timings capture Initializes GPIO and TIM2 for timings capture
 *
 * @param      callback  FuriHalSubGhzCaptureCallback
 * @param      context   callback context
 */
void subghz_device_si4463_ext_start_async_rx(FuriHalSubGhzCaptureCallback callback, void* context);

/** Disable signal timings capture Resets GPIO and TIM2
 */
void subghz_device_si4463_ext_stop_async_rx();

/** Async TX callback type
 * @param      context  callback context
 * @return     LevelDuration
 */
typedef LevelDuration (*FuriHalSubGhzAsyncTxCallback)(void* context);

/** Start async TX Initializes GPIO, TIM2 and DMA1 for signal output
 *
 * @param      callback  FuriHalSubGhzAsyncTxCallback
 * @param      context   callback context
 *
 * @return     true if the transfer is allowed by belonging to the region
 */
bool subghz_device_si4463_ext_start_async_tx(FuriHalSubGhzAsyncTxCallback callback, void* context);

/** Wait for async transmission to complete
 *
 * @return     true if TX complete
 */
bool subghz_device_si4463_ext_is_async_tx_complete();

/** Stop async transmission and cleanup resources Resets GPIO, TIM2, and DMA1
 */
void subghz_device_si4463_ext_stop_async_tx();

#ifdef __cplusplus
}
#endif