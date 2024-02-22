/**
 * @file rp2040.h
 * @brief RP2040-specific functions.
 *
 * This file is responsible for initialising and accessing
 * the SPI flash chip via RP2040 hardware.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#define RP2040_CORE0_ADDR (0x01002927UL)
#define RP2040_CORE1_ADDR (0x11002927UL)
#define RP2040_RESCUE_ADDR (0xF1002927UL)

#define RP2040_FAMILY_ID (0xE48BFF56UL)

/**
 * @brief Initialise RP2040-specific hardware.
 *
 * @returns true on success, false otherwise.
 */
bool rp2040_init(void);

/**
 * @brief Read data from the SPI flash chip.
 *
 * @param[in] address target address within the flash address space.
 * @param[out] data pointer to the buffer to contain the data to be read.
 * @param[in] data_size size of the data to be read.
 * @returns true on success, false otherwise.
 */
bool rp2040_flash_read_data(uint32_t address, void* data, size_t data_size);

/**
 * @brief Erase one sector (4K) of the SPI flash chip.
 *
 * @param[in] address target address within the flash address space (must be sector-aligned).
 * @returns true on success, false otherwise.
 */
bool rp2040_flash_erase_sector(uint32_t address);

/**
 * @brief Program one page (256B) of the SPI flash chip.
 *
 * @param[in] address target address within the flash address space.
 * @param[in] data pointer to the buffer containing the data to be written.
 * @param[in] data_size size of the data to be written.
 * @returns true on success, false otherwise.
 */
bool rp2040_flash_program_page(uint32_t address, const void* data, size_t data_size);
