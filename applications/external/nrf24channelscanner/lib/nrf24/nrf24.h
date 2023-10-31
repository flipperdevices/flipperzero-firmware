#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <furi_hal_spi.h>
#include <cfw.h>

#ifdef __cplusplus
extern "C" {
#endif

#define R_REGISTER 0x00
#define W_REGISTER 0x20
#define REGISTER_MASK 0x1F
#define ACTIVATE 0x50
#define R_RX_PL_WID 0x60
#define R_RX_PAYLOAD 0x61
#define W_TX_PAYLOAD 0xA0
#define W_TX_PAYLOAD_NOACK 0xB0
#define W_ACK_PAYLOAD 0xA8
#define FLUSH_TX 0xE1
#define FLUSH_RX 0xE2
#define REUSE_TX_PL 0xE3
#define RF24_NOP 0xFF

#define REG_CONFIG 0x00
#define REG_EN_AA 0x01
#define REG_EN_RXADDR 0x02
#define REG_SETUP_AW 0x03
#define REG_SETUP_RETR 0x04
#define REG_RDP 0x09
#define REG_DYNPD 0x1C
#define REG_FEATURE 0x1D
#define REG_RF_SETUP 0x06
#define REG_STATUS 0x07
#define REG_RX_ADDR_P0 0x0A
#define REG_RF_CH 0x05
#define REG_TX_ADDR 0x10

#define RX_PW_P0 0x11
#define TX_DS 0x20
#define MAX_RT 0x10

#define nrf24_TIMEOUT 500
#define nrf24_CE_PIN &gpio_ext_pb2
#define nrf24_HANDLE                                                                         \
    (CFW_SETTINGS()->spi_nrf24_handle == SpiDefault ? &furi_hal_spi_bus_handle_external : \
                                                         &furi_hal_spi_bus_handle_external_extra)

/* Low level API */

/** Write device register
 *
 * @param      handle  - pointer to FuriHalSpiHandle
 * @param      reg     - register
 * @param      data    - data to write
 *
 * @return     device status
 */
uint8_t nrf24_write_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t data);

/** Read device register
 *
 * @param      handle  - pointer to FuriHalSpiHandle
 * @param      reg     - register
 * @param[out] data    - pointer to data
 *
 * @return     device status
 */
uint8_t nrf24_read_reg(FuriHalSpiBusHandle* handle, uint8_t reg, uint8_t* data, uint8_t size);

/** Power down the radio
 * 
 * @param      handle  - pointer to FuriHalSpiHandle
 * 
 * @return     device status
 */
uint8_t nrf24_set_idle(FuriHalSpiBusHandle* handle);

/** Sets the radio to RX mode
 *
 * @param      handle  - pointer to FuriHalSpiHandle
 * 
 * @return     device status
 */
uint8_t nrf24_set_rx_mode(FuriHalSpiBusHandle* handle, bool nodelay);

/*=============================================================================================================*/

/* High level API */

/** Must call this before using any other nrf24 API
 * 
 */
void nrf24_init();

/** Must call this when we end using nrf24 device
 * 
 */
void nrf24_deinit();

/** Send flush rx command
 *
 * @param      handle  - pointer to FuriHalSpiHandle
 *
 * @return     device status
 */
uint8_t nrf24_flush_rx(FuriHalSpiBusHandle* handle);

/** Gets RDP from register 0x09
 *
 * @param      handle  - pointer to FuriHalSpiHandle
 * 
 * @return     RDP from register 0x09
 */
uint8_t nrf24_get_rdp(FuriHalSpiBusHandle* handle);

/** Gets the current status flags from the STATUS register
 * 
 * @param      handle  - pointer to FuriHalSpiHandle
 * 
 * @return     status flags
 */
uint8_t nrf24_status(FuriHalSpiBusHandle* handle);

bool nrf24_check_connected(FuriHalSpiBusHandle* handle);

#ifdef __cplusplus
}
#endif