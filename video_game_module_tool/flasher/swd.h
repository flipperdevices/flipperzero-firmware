/**
 * @file swd.h
 * @brief Serial Wire Debug (SWD) bus functions.
 *
 * This file is responsible for:
 *
 * - Debug hardware initialisation
 * - Target selection in a multidrop bus
 * - Debug and Access port access
 *
 * For more information, see ARM IHI0031G
 * https://documentation-service.arm.com/static/622222b2e6f58973271ebc21
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

// Only bits [3:2] are used to access DP registers
#define SWD_DP_REG_ADDR_SHIFT (2U)

// Debug port registers - write
#define SWD_DP_REG_WO_ABORT (0x0U >> SWD_DP_REG_ADDR_SHIFT)
#define SWD_DP_REG_WO_SELECT (0x8U >> SWD_DP_REG_ADDR_SHIFT)
#define SWD_DP_REG_WO_TASRGETSEL (0xCU >> SWD_DP_REG_ADDR_SHIFT)

// Debug port registers - read
#define SWD_DP_REG_RO_DPIDR (0x0U >> SWD_DP_REG_ADDR_SHIFT)
#define SWD_DP_REG_RO_RESEND (0x8U >> SWD_DP_REG_ADDR_SHIFT)
#define SWD_DP_REG_RO_RDBUFF (0xCU >> SWD_DP_REG_ADDR_SHIFT)

// Debug port registers - read/write
#define SWD_DP_REG_RW_BANK (0x4U >> SWD_DP_REG_ADDR_SHIFT)
#define SWD_DP_REG_RW_CTRL_STAT (SWD_DP_REG_RW_BANK)

// Access port registers
#define SWD_AP_REG_RW_CSW (0x00U)
#define SWD_AP_REG_RW_TAR (0x04U)
#define SWD_AP_REG_RW_DRW (0x0CU)
#define SWD_AP_REG_RO_IDR (0xFCU)

// CTRL/STAT bits
#define SWD_DP_REG_CTRL_STAT_CDBGPWRUPREQ (1UL << 28U)
#define SWD_DP_REG_CTRL_STAT_CDBGPWRUPACK (1UL << 29U)
#define SWD_DP_REG_CTRL_STAT_CSYSPWRUPREQ (1UL << 30U)
#define SWD_DP_REG_CTRL_STAT_CSYSPWRUPACK (1UL << 31U)

// CSW bits (PROT bits are for AHB3)
#define SWD_AP_REG_CSW_SIZE_WORD (2UL << 0U)
#define SWD_AP_REG_CSW_HPROT_DATA (1UL << 24U)
#define SWD_AP_REG_CSW_HPROT_PRIVILIGED (1UL << 25U)
#define SWD_AP_REG_CSW_HPROT_BUFFERABLE (1UL << 26U)
#define SWD_AP_REG_CSW_HPROT_CACHEABLE (1UL << 27U)
#define SWD_AP_REG_CSW_HNONSEC (1UL << 30U)

/**
 * @brief Initialise SWD bus.
 *
 * Configures SWCLK and SWDIO pins, wakes up the target from
 * dormant state and resets the SWD bus.
 */
void swd_init(void);

/**
 * @brief Disable SWD bus.
 *
 * Sets the target to dormant state and returns
 * SWCLK and SWDIO pins to analog mode.
 */
void swd_deinit(void);

/**
 * @brief Select one target on a multidrop (SWD v2) bus.
 *
 * @param[in] target_id target address or id (specified in device datasheet)
 */
void swd_select_target(uint32_t target_id);

/**
 * @brief Perform a Debug Port (DP) read.
 *
 * Reads a 32-bit word from the designated DP register.
 *
 * @param[in] address DP register address.
 * @param[out] data pointer to the value to contain the read data.
 * @returns true on success, false otherwise.
 */
bool swd_dp_read(uint8_t address, uint32_t* data);

/**
 * @brief Perform a Debug Port (DP) write.
 *
 * Writes a 32-bit word to the designated DP register.
 *
 * @param[in] address DP register address.
 * @param[in] data value to be written as data.
 * @returns true on success, false otherwise.
 */
bool swd_dp_write(uint8_t address, uint32_t data);

/**
 * @brief Perform an Access Port (AP) read.
 *
 * Reads a 32-bit word from the designated AP register.
 *
 * @param[in] address AP register address.
 * @param[out] data pointer to the value to contain the read data.
 * @returns true on success, false otherwise.
 */
bool swd_ap_read(uint8_t address, uint32_t* data);

/**
 * @brief Perform an Access Port (AP) write.
 *
 * Writes a 32-bit word to the designated AP register.
 *
 * @param[in] address AP register address.
 * @param[in] data value to be written as data.
 * @returns true on success, false otherwise.
 */
bool swd_ap_write(uint8_t address, uint32_t data);
