#pragma once

#include "subbrute_protocols.h"
#include <lib/subghz/protocols/base.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/environment.h>
#include "helpers/subbrute_radio_device_loader.h"

#define SUBBRUTE_MAX_LEN_NAME 64
#define SUBBRUTE_PATH EXT_PATH("subghz")
#define SUBBRUTE_FILE_EXT ".sub"

/**
 * @enum SubBruteFileResult
 * @brief Represents the possible results of a sub-brute force file operation.
 *
 * This enumeration defines a set of possible results that can occur when performing
 * a sub-brute force operation on a file. Each result represents a specific error or
 * condition that can occur during the operation.
 */
typedef enum {
    SubBruteFileResultUnknown,
    SubBruteFileResultOk,
    SubBruteFileResultErrorOpenFile,
    SubBruteFileResultMissingOrIncorrectHeader,
    SubBruteFileResultFrequencyNotAllowed,
    SubBruteFileResultMissingOrIncorrectFrequency,
    SubBruteFileResultPresetInvalid,
    SubBruteFileResultMissingProtocol,
    SubBruteFileResultProtocolNotSupported,
    SubBruteFileResultDynamicProtocolNotValid,
    SubBruteFileResultProtocolNotFound,
    SubBruteFileResultMissingOrIncorrectBit,
    SubBruteFileResultMissingOrIncorrectKey,
    SubBruteFileResultMissingOrIncorrectTe,
} SubBruteFileResult;

/**
 * @struct SubBruteDevice
 * @brief Represents a device for SubBrute attack.
 *
 * This structure contains information and state variables required for performing
 * a SubBrute attack.
 */
typedef struct {
    const SubBruteProtocol* protocol_info;
    SubBruteProtocol* file_protocol_info;

    // Current step
    uint64_t current_step;

    // SubGhz
    SubGhzReceiver* receiver;
    SubGhzProtocolDecoderBase* decoder_result;
    SubGhzEnvironment* environment;
    const SubGhzDevice* radio_device;

    // Attack state
    SubBruteAttacks attack;
    uint64_t max_value;
    uint8_t extra_repeats;

    // Loaded info for attack type
    uint64_t key_from_file;
    uint64_t current_key_from_file;
    bool two_bytes;

    // Index of group to bruteforce in loaded file
    uint8_t bit_index;
} SubBruteDevice;

/**
 * @brief Allocates memory for a SubBruteDevice structure.
 *
 * This function allocates memory for a SubBruteDevice structure and
 * initializes it using the given SubGhzDevice.
 *
 * @param radio_device The SubGhzDevice used to initialize the SubBruteDevice.
 * @return A pointer to the allocated SubBruteDevice structure.
 */
SubBruteDevice* subbrute_device_alloc(const SubGhzDevice* radio_device);

/**
 * @brief Frees the memory allocated for a SubBruteDevice instance.
 *
 * This function frees the memory allocated for a SubBruteDevice instance.
 * After calling this function, the instance is no longer valid and should not be used.
 *
 * @param instance Pointer to the SubBruteDevice instance to be freed.
 */
void subbrute_device_free(SubBruteDevice* instance);

/**
 * Saves a file with the specified key name using the given SubBruteDevice instance.
 *
 * @param instance The SubBruteDevice instance to use for saving the file.
 * @param key_name The name of the key to be used for saving the file.
 * @return True if the file is successfully saved, False otherwise.
 */
bool subbrute_device_save_file(SubBruteDevice* instance, const char* key_name);

/**
 * @brief Retrieves the description for a specific device error.
 *
 * This function returns a string that describes the given device error ID. The error ID
 * should be obtained from a SubBruteFileResult value. The returned string provides
 * additional information about the error, such as its cause or possible solutions.
 *
 * @param error_id The device error ID for which to retrieve the description.
 *
 * @return A pointer to a constant string containing the description of the device error.
 */
const char* subbrute_device_error_get_desc(SubBruteFileResult error_id);
SubBruteFileResult subbrute_device_attack_set(
    SubBruteDevice* context,
    SubBruteAttacks type,
    uint8_t extra_repeats);

/**
 * @brief Loads data from a file into a SubBruteDevice context.
 *
 * This function reads data from the specified file and populates the
 * SubBruteDevice context with the loaded data. The file contents are
 * expected to be in a specific format supported by the SubBruteDevice.
 * Use this function to initialize the context with pre-existing data.
 *
 * @param context Pointer to the SubBruteDevice context to be populated.
 * @param file_path Path of the file to load data from.
 * @return The status of the loading operation.
 *     - Returns 0 if the loading is successful.
 *     - Returns a non-zero value if an error occurs during loading.
 */
uint8_t subbrute_device_load_from_file(SubBruteDevice* context, const char* file_path);

/**
 * @brief Adds a step to the SubBruteDevice instance.
 *
 * This function adds a step to the given SubBruteDevice instance. It updates
 * the internal state of the device accordingly.
 *
 * @param[in,out] instance The SubBruteDevice instance.
 * @param[in] step The step to be added.
 *
 * @return The updated value of the device state after adding the step.
 */
uint64_t subbrute_device_add_step(SubBruteDevice* instance, int8_t step);

/**
 * @brief Frees the memory allocated for the protocol information of a SubBruteDevice instance.
 *
 * This function will deallocate the memory used by the protocol information of a SubBruteDevice instance,
 * including all the individual protocol objects and their data.
 *
 * @param instance Pointer to the SubBruteDevice instance.
 */
void subbrute_device_free_protocol_info(SubBruteDevice* instance);

/**
 * @brief Set the default values for a SubBruteDevice attack configuration.
 *
 * This function sets the default values for a SubBruteDevice attack configuration based on the specified default_attack type.
 *
 * @param context Pointer to a SubBruteDevice instance.
 * @param default_attack The default attack type to use.
 *
 * @see SubBruteDevice
 */
void subbrute_device_attack_set_default_values(
    SubBruteDevice* context,
    SubBruteAttacks default_attack);
