#pragma once

#include "si446x_regs.h"

#include <stdbool.h>
#include <stdint.h>
#include <furi_hal_spi.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Low level API */

bool si446x_wait_cts_spi(FuriHalSpiBusHandle* handle);
bool si446x_write_data(FuriHalSpiBusHandle* handle, const uint8_t* data, uint8_t size);
bool si446x_read_data(FuriHalSpiBusHandle* handle, uint8_t* data, uint8_t size);
bool si446x_clear_interrupt_status(FuriHalSpiBusHandle* handle);
uint8_t si446x_get_version(FuriHalSpiBusHandle* handle);
uint16_t si446x_get_partnumber(FuriHalSpiBusHandle* handle);
bool si446x_set_state(FuriHalSpiBusHandle* handle, SI446X_State_t state);
SI446X_State_t si446x_get_state(FuriHalSpiBusHandle* handle);
SI446X_State_t si446x_get_current_channel(FuriHalSpiBusHandle* handle);
uint8_t si446x_get_fast_reg(FuriHalSpiBusHandle* handle, uint8_t reg);
uint8_t si446x_get_rssi(FuriHalSpiBusHandle* handle);
uint8_t si446x_set_rssi_control(FuriHalSpiBusHandle* handle);
bool si446x_set_rssi_threshold(FuriHalSpiBusHandle* handle, int rssi_dbi);
uint8_t si446x_get_lqi(FuriHalSpiBusHandle* handle);
bool si446x_switch_to_start_tx(
    FuriHalSpiBusHandle* handle,
    uint8_t channel,
    SI446X_State_t state_on_tx_finish,
    uint8_t len_package_tx);
bool si446x_switch_to_start_rx(
    FuriHalSpiBusHandle* handle,
    uint8_t channel,
    SI446X_State_t state_on_rx_finish,
    uint8_t len_package_rx);
bool si446x_switch_to_idle(FuriHalSpiBusHandle* handle);
bool si446x_write_gpio(FuriHalSpiBusHandle* handle, SI446X_GPIO_t pin, uint8_t gpio_mode);
bool si446x_write_sw(
    FuriHalSpiBusHandle* handle,
    SI446X_GPIO_t sw0_pin,
    uint8_t sw0_gpio_mode,
    SI446X_GPIO_t sw1_pin,
    uint8_t sw1_gpio_mode);
bool si446x_read_gpio(FuriHalSpiBusHandle* handle, SI446X_GPIO_t pin);
bool si446x_set_properties(
    FuriHalSpiBusHandle* handle,
    SI446X_Prop_t prop,
    uint8_t* data,
    uint8_t size);
bool si446x_get_properties(
    FuriHalSpiBusHandle* handle,
    SI446X_Prop_t prop,
    uint8_t* data,
    uint8_t size);
bool si446x_set_pa(FuriHalSpiBusHandle* handle, uint8_t pa_level);
uint32_t si446x_set_frequency_and_step_channel(
    FuriHalSpiBusHandle* handle,
    uint32_t freq_hz,
    uint32_t step_channel_hz);
bool si446x_set_deviation(FuriHalSpiBusHandle* handle, uint32_t freq_hz, uint32_t deviation_hz);
bool si446x_set_bps(FuriHalSpiBusHandle* handle, uint32_t bps);

#ifdef __cplusplus
}
#endif