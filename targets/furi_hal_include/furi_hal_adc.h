/**
 * @file furi_hal_adc.h
 * @brief ADC HAL API
 * 
 * For the sake of simplicity this API implements only small subset of what ADC is actually capable.
 * Feel free to visit Reference Manual for STM32WB series and implement any other modes by your self.
 * 
 * How to use:
 * 
 * 0. Configure your pins to 
 * 1. furi_hal_adc_acquire - acquire ADC handle to work with
 * 2. furi_hal_adc_configure - configure ADC block
 * 3. furi_hal_adc_read - read some values
 * 4. furi_hal_adc_release - release ADC to the system
 *
 */

#pragma once

#include <furi.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct FuriHalAdcHandle FuriHalAdcHandle;

typedef enum {
    FuriHalAdcScale2048, /**< 2.048V scale */
    FuriHalAdcScale2500, /**< 2.5V scale */
} FuriHalAdcScale;

typedef enum {
    FuriHalAdcOversample2, /**< ADC will take 2 samples per each value */
    FuriHalAdcOversample4, /**< ADC will take 4 samples per each value */
    FuriHalAdcOversample8, /**< ADC will take 8 samples per each value */
    FuriHalAdcOversample16, /**< ADC will take 16 samples per each value */
    FuriHalAdcOversample32, /**< ADC will take 32 samples per each value */
    FuriHalAdcOversample64, /**< ADC will take 64 samples per each value */
    FuriHalAdcOversample128, /**< ADC will take 128 samples per each value */
    FuriHalAdcOversample256, /**< ADC will take 256 samples per each value */
    FuriHalAdcOversampleNone, /**< disable oversampling */
} FuriHalAdcOversample;

typedef enum {
    FuriHalAdcSamplingtime2_5, /**< Sampling time 2.5 ADC clock */
    FuriHalAdcSamplingtime6_5, /**< Sampling time 6.5 ADC clock */
    FuriHalAdcSamplingtime12_5, /**< Sampling time 12.5 ADC clock */
    FuriHalAdcSamplingtime24_5, /**< Sampling time 24.5 ADC clock */
    FuriHalAdcSamplingtime47_5, /**< Sampling time 47.5 ADC clock */
    FuriHalAdcSamplingtime92_5, /**< Sampling time 92.5 ADC clock */
    FuriHalAdcSamplingtime247_5, /**< Sampling time 247.5 ADC clock */
    FuriHalAdcSamplingtime640_5, /**< Sampling time 640.5 ADC clock */
} FuriHalAdcSamplingTime;

typedef enum {
    /* Channels 0 - 5 are fast channels */
    FuriHalAdcChannel0, /**< Internal channel, see `FuriHalAdcChannelVREFINT`. */
    FuriHalAdcChannel1, /**< Channel 1p */
    FuriHalAdcChannel2, /**< Channel 2p or 1n */
    FuriHalAdcChannel3, /**< Channel 3p or 2n */
    FuriHalAdcChannel4, /**< Channel 4p or 3n */
    FuriHalAdcChannel5, /**< Channel 5p or 4n */
    /* Channels 6 - 18 are slow channels */
    FuriHalAdcChannel6, /**< Channel 6p or 5n */
    FuriHalAdcChannel7, /**< Channel 7p or 6n */
    FuriHalAdcChannel8, /**< Channel 8p or 7n */
    FuriHalAdcChannel9, /**< Channel 9p or 8n */
    FuriHalAdcChannel10, /**< Channel 10p or 9n */
    FuriHalAdcChannel11, /**< Channel 11p or 10n */
    FuriHalAdcChannel12, /**< Channel 12p or 11n */
    FuriHalAdcChannel13, /**< Channel 13p or 12n */
    FuriHalAdcChannel14, /**< Channel 14p or 13n */
    FuriHalAdcChannel15, /**< Channel 15p or 14n */
    FuriHalAdcChannel16, /**< Channel 16p or 15n */
    FuriHalAdcChannel17, /**< Internal channel, see `FuriHalAdcChannelTEMPSENSOR`. */
    FuriHalAdcChannel18, /**< Internal channel, see `FuriHalAdcChannelVBAT`. */
    /* Special Channels: combines one of the 0-18 channel and additional internal peripherals */
    FuriHalAdcChannelVREFINT, /**< Special channel for VREFINT, used for calibration and self test */
    FuriHalAdcChannelTEMPSENSOR, /**< Special channel for on-die temperature sensor */
    FuriHalAdcChannelVBAT, /**< Special channel for VBAT/3 voltage. */
    /* Special value to indicate that pin is not connected to ADC */
    FuriHalAdcChannelNone, /**< No channel */
} FuriHalAdcChannel;

/** Initialize ADC subsystem */
void furi_hal_adc_init(void);

/** Acquire ADC handle
 *
 * Enables appropriate power and clocking domains, enables VREF with `FuriHalAdcScale2500`
 *
 * @return     FuriHalAdcHandle pointer
 */
FuriHalAdcHandle* furi_hal_adc_acquire(void);

/** Release ADC handle
 *
 * @param      handle  The handle
 */
void furi_hal_adc_release(FuriHalAdcHandle* handle);

/** Configure ADC
 * 
 * default parameters:
 * - FuriHalAdcScale2500
 * - FuriHalAdcOversample16
 * - FuriHalAdcSamplingtime92_5
 *
 * @param      handle  The ADC handle
 */
void furi_hal_adc_configure(FuriHalAdcHandle* handle);

/** Configure ADC with extended options
 *
 * @warning    Please carefully read STM32WB series reference manual. Setting
 *             incorrect parameters leads to poor results.
 *
 * @param      handle         The ADC handle
 * @param[in]  scale          The voltage scale
 * @param[in]  oversample     The oversample mode
 * @param[in]  sampling_time  The sampling time
 */
void furi_hal_adc_configure_ex(
    FuriHalAdcHandle* handle,
    FuriHalAdcScale scale,
    FuriHalAdcOversample oversample,
    FuriHalAdcSamplingTime sampling_time);

/** Read single ADC value
 *
 * @param      handle   The ADC handle
 * @param[in]  channel  The channel to sample
 *
 * @return     value, 12 bits
 */
uint16_t furi_hal_adc_read(FuriHalAdcHandle* handle, FuriHalAdcChannel channel);

/** Convert sampled value to voltage
 *
 * @param      handle  The handle
 * @param[in]  value   The value acquired with `furi_hal_adc_read`
 *
 * @return     Voltage in mV
 */
float furi_hal_adc_convert_to_voltage(FuriHalAdcHandle* handle, uint16_t value);

/** Convert sampled VREFINT value to voltage
 *
 * @param      handle  The handle
 * @param[in]  value   The value acquired with `furi_hal_adc_read` for `FuriHalAdcChannelVREFINT` channel
 *
 * @return     Voltage in mV
 */
float furi_hal_adc_convert_vref(FuriHalAdcHandle* handle, uint16_t value);

/** Convert sampled TEMPSENSOR value to temperature
 *
 * @param      handle  The handle
 * @param[in]  value   The value acquired with `furi_hal_adc_read` for `FuriHalAdcChannelTEMPSENSOR` channel
 *
 * @return     temperature in degree C
 */
float furi_hal_adc_convert_temp(FuriHalAdcHandle* handle, uint16_t value);

/** Convert sampled VBAT value to voltage
 *
 * @param      handle  The handle
 * @param[in]  value   The value acquired with `furi_hal_adc_read` for `FuriHalAdcChannelVBAT` channel
 *
 * @return     Voltage in mV
 */
float furi_hal_adc_convert_vbat(FuriHalAdcHandle* handle, uint16_t value);

#ifdef __cplusplus
}
#endif
