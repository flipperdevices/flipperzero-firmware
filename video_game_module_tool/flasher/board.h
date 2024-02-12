/**
 * @file board.h
 * @brief Video Game Module-specific functions.
 */
#pragma once

/**
 * @brief Initialise the module-specific hardware.
 */
void board_init(void);

/**
 * @brief Disable the module-specific hardware.
 */
void board_deinit(void);

/**
 * @brief Reset the module.
 *
 * Resets the Video Game Module through the dedicated
 * reset pin (Pin 15)
 */
void board_reset(void);
