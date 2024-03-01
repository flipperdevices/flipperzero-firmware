#pragma once

#include <lib/subghz/devices/devices.h>

/**
 * @file subghz_radio.h
 * @brief Defines the Sub-GHz radio device type.
 */
typedef enum {
    SubGhzRadioDeviceTypeInternal,
    SubGhzRadioDeviceTypeExternalCC1101,
} SubGhzRadioDeviceType;

/**
 * @brief Sets the SubGhz radio device type.
 *
 * This function is used to set the SubGhz radio device type for the SubBrute radio device loader.
 *
 * @param current_radio_device Pointer to the current SubGhz radio device.
 * @param radio_device_type The desired SubGhz radio device type.
 * @return const SubGhzDevice* Pointer to the new SubGhz radio device.
 *
 * @remark This function sets the SubGhz radio device type for the SubBrute radio device loader.
 *         The current radio device will be replaced with a new instance of the specified radio device type.
 *         If @p current_radio_device is NULL, a new instance of the specified radio device type will be created.
 *
 * @note The caller is responsible for handling memory deallocation of the returned pointer.
 */
const SubGhzDevice* subbrute_radio_device_loader_set(
    const SubGhzDevice* current_radio_device,
    SubGhzRadioDeviceType radio_device_type);

/**
 * @brief Unloads a SubGhz radio device.
 *
 * This function unloads a SubGhz radio device and performs any necessary cleanup.
 *
 * @param radio_device Pointer to the SubGhzDevice structure representing the radio device to be unloaded.
 */
void subbrute_radio_device_loader_end(const SubGhzDevice* radio_device);
