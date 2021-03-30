#pragma once

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Radio Presets */
typedef enum {
    ApiHalSubGhzPresetTestToneRx,
    ApiHalSubGhzPresetTestToneTx,
    ApiHalSubGhzPresetTestPacket,
} ApiHalSubGhzPreset;

/**  Switchable Radio Paths */
typedef enum {
    ApiHalSubGhzPathIsolate,    /** Isolate Radio from antenna */
    ApiHalSubGhzPath1,          /** Path 1: SW1RF1-SW2RF2, LCLCL */
    ApiHalSubGhzPath2,          /** Path 2: SW1RF2-SW2RF1, LCLCLCL */
    ApiHalSubGhzPath3,          /** Path 3: SW1RF3-SW2RF3, LCLC */
} ApiHalSubGhzPath;

/** Initialize IC and switch to power save mode */
void api_hal_subghz_init();

/** Load IC registers from preset by preset name 
 * @param preset to load
 */
void api_hal_subghz_load_preset(ApiHalSubGhzPreset preset);

/** Load IC registers from preset data array
 * @param register-value pairs array, terminated with {0,0}
 */
void api_hal_subghz_load_preset_data(const uint8_t data[][2]);

/** Dump IC info to stdout */
void api_hal_subghz_dump_state();

/** Switch to power save mode */
void api_hal_subghz_sleep();

/** Wakeup IC */
void api_hal_subghz_wakeup();

/** Switch to Idle */
void api_hal_subghz_idle();

/** Switch to Recieve */
void api_hal_subghz_rx();

/** Switch to Transmit */
void api_hal_subghz_tx();

/** Get RSSI value in dBm */
float api_hal_subghz_get_rssi();

/** Set frequency
 * @param frequency in herz
 * @return real frequency in herz
 */
uint32_t api_hal_subghz_set_frequency(uint32_t value);

/** Set path
 * @param radio path to use
 */
void api_hal_subghz_set_path(ApiHalSubGhzPath path);

#ifdef __cplusplus
}
#endif
