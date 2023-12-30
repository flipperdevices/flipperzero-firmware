#pragma once

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <storage/storage.h>
#include "subbrute_protocols.h"

/**
 * @struct SubBruteSettings
 * @brief Structure to store settings for a sub-brute attack.
 *
 * This structure stores the settings for a sub-brute attack, such as an array of repeat values and
 * the last index used.
 */
typedef struct {
    uint8_t repeat_values[SubBruteAttackTotalCount];
    uint32_t last_index;
} SubBruteSettings;

/**
 * @brief Allocates memory for a SubBruteSettings structure.
 *
 * @return A pointer to a newly allocated SubBruteSettings structure.
 * @note The returned structure should be freed using subbrute_settings_free() function.
 */
SubBruteSettings* subbrute_settings_alloc(void);

/**
 * @brief Frees the memory allocated for the SubBruteSettings instance and all its members.
 *
 * This function should be used to release the memory allocated for an instance of the SubBruteSettings structure.
 * It ensures that all members of the structure are properly freed.
 *
 * @param instance The SubBruteSettings instance to be freed.
 */
void subbrute_settings_free(SubBruteSettings* instance);

/**
 * @brief Loads the settings for the SubBrute instance.
 *
 * This function loads the settings for the SubBrute instance and populates the SubBruteSettings structure
 * pointed to by the given instance.
 *
 * @param instance A pointer to the SubBruteSettings structure to populate with the loaded settings.
 */
void subbrute_settings_load(SubBruteSettings* instance);

/**
 * @brief Saves the settings of the SubBrute instance.
 *
 * This function is used to save the settings of the SubBrute instance to a storage.
 *
 * @param instance A pointer to the SubBruteSettings instance.
 *
 * @note The settings will be saved to a storage. The exact storage and format
 *       will depend on the implementation. It is the responsibility of the caller
 *       to ensure that the instance is valid and has been initialized properly.
 *       The caller should handle any errors that may occur during the saving process.
 */
bool subbrute_settings_save(SubBruteSettings* instance);

/**
 * @brief Sets the value for a specific attack in the SubBruteSettings instance.
 *
 * This function allows you to set the value for a specific attack in the SubBruteSettings instance.
 *
 * @param instance A pointer to the SubBruteSettings instance.
 * @param index The index of the attack for which the value needs to be set.
 * @param value The value to be set for the specified attack.
 */
void subbrute_settings_set_value(SubBruteSettings* instance, SubBruteAttacks index, uint8_t value);

/**
 * @brief Get the value of a specific attack setting in the SubBruteSettings instance.
 *
 * @param instance The pointer to the SubBruteSettings instance.
 * @param index    The index of the attack setting to get the value of.
 *
 * @return The value of the specified attack setting.
 *
 * @note The index parameter should be one of the SubBruteAttacks enum values.
 */
uint8_t subbrute_settings_get_value(SubBruteSettings* instance, SubBruteAttacks index);

/**
 * @brief Sets the repeated values for the SubBruteSettings instance.
 *
 * This function sets the repeated values for the SubBruteSettings instance.
 * The repeated values are used for the sub-brute operation.
 *
 * @param instance Pointer to the SubBruteSettings instance.
 * @param repeated_values Pointer to an array of repeated values.
 */
void subbrute_settings_set_repeats(SubBruteSettings* instance, const uint8_t* repeated_values);

/**
 * @brief Gets the current number of repeats from the SubBruteSettings instance.
 *
 * This function retrieves the current number of repeats stored in the given SubBruteSettings instance.
 *
 * @param instance Pointer to the SubBruteSettings instance.
 * @return The current number of repeats as a uint8_t.
 */
uint8_t subbrute_settings_get_current_repeats(SubBruteSettings* instance);
