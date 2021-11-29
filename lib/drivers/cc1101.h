#pragma once

#include "cc1101_regs.h"

#include <stdbool.h>
#include <stdint.h>
#include <furi-hal-spi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Low level API */

/** Strobe command to the device
 * @param device - pointer to FuriHalSpiDevice
 * @param strobe - command to execute
 * @return device status
 */
CC1101Status cc1101_strobe(FuriHalSpiBusHandle* handle, uint8_t strobe);

/** Write device register
 * @param device - pointer to FuriHalSpiDevice
 * @param reg - register
 * @param data - data to write
 * @return device status
 */
CC1101Status cc1101_write_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t data);

/** Read device register
 * @param device - pointer to FuriHalSpiDevice
 * @param reg - register
 * @param[out] data - pointer to data
 * @return device status
 */
CC1101Status cc1101_read_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t* data);

/* High level API */

/** Reset
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_reset(FuriHalSpiBusHandle* handle);

/** Get status
 * @param device - pointer to FuriHalSpiDevice
 */
CC1101Status cc1101_get_status(FuriHalSpiBusHandle* handle);

/** Enable shutdown mode
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_shutdown(FuriHalSpiBusHandle* handle);

/** Get Partnumber
 * @param device - pointer to FuriHalSpiDevice
 */
uint8_t cc1101_get_partnumber(FuriHalSpiBusHandle* handle);

/** Get Version
 * @param device - pointer to FuriHalSpiDevice
 */
uint8_t cc1101_get_version(FuriHalSpiBusHandle* handle);

/** Get raw RSSI value
 * @param device - pointer to FuriHalSpiDevice
 */
uint8_t cc1101_get_rssi(FuriHalSpiBusHandle* handle);

/** Calibrate oscillator
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_calibrate(FuriHalSpiBusHandle* handle);

/** Switch to idle
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_switch_to_idle(FuriHalSpiBusHandle* handle);

/** Switch to RX
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_switch_to_rx(FuriHalSpiBusHandle* handle);

/** Switch to TX
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_switch_to_tx(FuriHalSpiBusHandle* handle);

/** Flush RX FIFO
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_flush_rx(FuriHalSpiBusHandle* handle);

/** Flush TX FIFO
 * @param device - pointer to FuriHalSpiDevice
 */
void cc1101_flush_tx(FuriHalSpiBusHandle* handle);

/** Set Frequency
 * @param device - pointer to FuriHalSpiDevice
 * @param value - frequency in herz
 * @return real frequency that were synthesized
 */
uint32_t cc1101_set_frequency(FuriHalSpiBusHandle* handle, uint32_t value);

/** Set Intermediate Frequency
 * @param device - pointer to FuriHalSpiDevice
 * @param value - frequency in herz
 * @return real inermediate frequency that were synthesized
 */
uint32_t cc1101_set_intermediate_frequency(FuriHalSpiBusHandle* handle, uint32_t value);

/** Set Power Amplifier level table, ramp
 * @param device - pointer to FuriHalSpiDevice
 * @param value - array of power level values
 */
void cc1101_set_pa_table(FuriHalSpiBusHandle* handle, const uint8_t value[8]);

/** Set Power Amplifier level table, ramp
 * @param device - pointer to FuriHalSpiDevice
 * @param value - array of power level values
 */
void cc1101_set_pa_table(FuriHalSpiBusHandle* handle, const uint8_t value[8]);

/** Write FIFO
 * @param device - pointer to FuriHalSpiDevice
 * @param data, pointer to byte array
 * @param size, write bytes count
 * @return size, written bytes count
 */
uint8_t cc1101_write_fifo(FuriHalSpiBusHandle* handle, const uint8_t* data, uint8_t size);

/** Read FIFO
 * @param device - pointer to FuriHalSpiDevice
 * @param data, pointer to byte array
 * @param size, bytes to read from fifo
 * @return size, read bytes count
 */
uint8_t cc1101_read_fifo(FuriHalSpiBusHandle* handle, uint8_t* data, uint8_t* size);

#ifdef __cplusplus
}
#endif