/**
 * @file target.h
 * @brief Debug target functions.
 *
 * This file is responsible for configuring the debug target
 * and accessing its memory-mapped devices.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Attach and halt the debug target.
 *
 * @param[in] target_id target address or id (specified in device datasheet)
 * @returns true on success, false otherwise.
 */
bool target_attach(uint32_t id);

/**
 * @brief Detach and resume the debug target.
 *
 * @returns true on success, false otherwise.
 */
bool target_detach(void);

/**
 * @brief Read a 32-bit word within target address space.
 *
 * @param[in] address target memory address.
 * @param[out] data pointer to the value to hold the read data.
 * @returns true on success, false otherwise.
 */
bool target_read_memory_32(uint32_t address, uint32_t* data);

/**
 * @brief Write a 32-bit word within target address space.
 *
 * @param[in] address target memory address.
 * @param[in] data value to be written as data.
 * @returns true on success, false otherwise.
 */
bool target_write_memory_32(uint32_t address, uint32_t data);
