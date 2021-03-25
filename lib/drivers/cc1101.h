#pragma once

#include "cc1101_regs.h"

#include <stdbool.h>
#include <stdint.h>
#include <api-hal-spi.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Reset CC1101
 * @param device - pointer to ApiHalSpiDevice
 */
void cc1101_reset(const ApiHalSpiDevice* device);

/** Enable shutdown mode
 * @param device - pointer to ApiHalSpiDevice
 */
void cc1101_shutdown(const ApiHalSpiDevice* device);

/** Calibrate oscillator
 * @param device - pointer to ApiHalSpiDevice
 */
void cc1101_calibrate(const ApiHalSpiDevice* device);

/** Switch to idle
 * @param device - pointer to ApiHalSpiDevice
 */
void cc1101_switch_to_idle(const ApiHalSpiDevice* device);

/** Switch to RX
 * @param device - pointer to ApiHalSpiDevice
 */
void cc1101_switch_to_rx(const ApiHalSpiDevice* device);

/** Switch to TX
 * @param device - pointer to ApiHalSpiDevice
 */
void cc1101_switch_to_tx(const ApiHalSpiDevice* device);

/** Set Frequency
 * Is not 100% precise, depends on quartz used
 * @param device - pointer to ApiHalSpiDevice
 * @param value - frequency in herz
 * @return real frequency that were set
 */
uint32_t cc1101_set_frequency(const ApiHalSpiDevice* device, uint32_t value);

/** Set Frequency Offset
 * Is not 100% precise, depends on quartz used
 * @param device - pointer to ApiHalSpiDevice
 * @param value - frequency offset in herz
 * @return real frequency that were set
 */
uint32_t cc1101_set_frequency_offset(const ApiHalSpiDevice* device, uint32_t value);

/** Set Power Amplifier level, no ramp
 * @param device - pointer to ApiHalSpiDevice
 * @param value - power level value
 */
void cc1101_set_pa_single(const ApiHalSpiDevice* device, uint8_t value);

/** Set Power Amplifier level table, ramp
 * @param device - pointer to ApiHalSpiDevice
 * @param value - array of power level values
 */
void cc1101_set_pa_table(const ApiHalSpiDevice* device, uint8_t value[8]);

#ifdef __cplusplus
}
#endif