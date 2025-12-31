/**
 * @file furi_hal_subghz.h
 * SubGhz HAL API
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <toolbox/level_duration.h>
#include <furi_hal_gpio.h>
#include "driver/si446x_regs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Switchable Radio Paths */
typedef enum {
    SubGhzDeviceSi4463ExtPathIsolate, /**< Isolate Radio from antenna */
    SubGhzDeviceSi4463ExtPath433, /**< Center Frquency: 433MHz. Path 1: SW1RF1-SW2RF2, LCLCL */
    SubGhzDeviceSi4463ExtPath315, /**< Center Frquency: 315MHz. Path 2: SW1RF2-SW2RF1, LCLCLCL */
    SubGhzDeviceSi4463ExtPath868, /**< Center Frquency: 868MHz. Path 3: SW1RF3-SW2RF3, LCLC */
} SubGhzDeviceSi4463ExtPath;

/* Mirror RX/TX async modulation signal to specified pin
 *
 * @warning    Configures pin to output mode. Make sure it is not connected
 *             directly to power or ground.
 *
 * @param[in]  pin   pointer to the gpio pin structure or NULL to disable
 */
void subghz_device_si4463_ext_set_async_mirror_pin(const GpioPin* pin);

/** Get data GPIO
 *
 * @return     pointer to the gpio pin structure
 */
const GpioPin* subghz_device_si4463_ext_get_data_gpio();

/** Initialize device
 *
 * @return     true if success
 */
bool subghz_device_si4463_ext_alloc();

/** Deinitialize device
 */
void subghz_device_si4463_ext_free();

/** Check and switch to power save mode Used by internal API-HAL
 * initialization routine Can be used to reinitialize device to safe state and
 * send it to sleep
 */
bool subghz_device_si4463_ext_is_connect();

/** Send device to sleep mode
 */
void subghz_device_si4463_ext_sleep();

/** Dump info to stdout
 */
void subghz_device_si4463_ext_dump_state();

// /** Load registers from preset by preset name
//  *
//  * @param      preset  to load
//  */
// void subghz_device_si4463_ext_load_preset(FuriHalSubGhzPreset preset);

void subghz_device_si4463_ext_mod_gpio_for_async(SI446X_Prop_Modem_Mod_Type_t modulation);

void subghz_device_si4463_ext_load_config(const uint8_t config[]);

void subghz_device_si4463_set_pa(uint8_t pa);

// /** Load PATABLE
//  *
//  * @param      data  8 uint8_t values
//  */
// void subghz_device_si4463_ext_load_patable(const uint8_t data[8]);

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

bool subghz_device_si4463_ext_get_properties(SI446X_Prop_t prop, uint8_t* data, uint8_t size);
bool subghz_device_si4463_ext_set_properties(SI446X_Prop_t prop, uint8_t* data, uint8_t size);

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
void subghz_device_si4463_ext_set_path(SubGhzDeviceSi4463ExtPath path);

/* High Level API */

/** Signal Timings Capture callback */
typedef void (*SubGhzDeviceSi4463ExtCaptureCallback)(bool level, uint32_t duration, void* context);

/** Enable signal timings capture Initializes GPIO and TIM2 for timings capture
 *
 * @param      callback  SubGhzDeviceSi4463ExtCaptureCallback
 * @param      context   callback context
 */
void subghz_device_si4463_ext_start_async_rx(
    SubGhzDeviceSi4463ExtCaptureCallback callback,
    void* context);

/** Disable signal timings capture Resets GPIO and TIM2
 */
void subghz_device_si4463_ext_stop_async_rx();

/** Async TX callback type
 * @param      context  callback context
 * @return     LevelDuration
 */
typedef LevelDuration (*SubGhzDeviceSi4463ExtCallback)(void* context);

/** Start async TX Initializes GPIO, TIM2 and DMA1 for signal output
 *
 * @param      callback  SubGhzDeviceSi4463ExtCallback
 * @param      context   callback context
 *
 * @return     true if the transfer is allowed by belonging to the region
 */
bool subghz_device_si4463_ext_start_async_tx(SubGhzDeviceSi4463ExtCallback callback, void* context);

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