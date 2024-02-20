#pragma once

#include "../subbrute_protocols.h"
#include "subbrute_radio_device_loader.h"

/**
 * @enum SubBruteWorkerState
 * @brief Enumeration defining the different states of the SubBruteWorker.
 *
 * This enumeration defines the possible states of the SubBruteWorker, which is responsible for performing
 * sub-brute forcing tasks.
 *
 * @var SubBruteWorkerStateIDLE The worker is in idle state and not performing any task.
 * @var SubBruteWorkerStateReady The worker is ready to start a new task.
 * @var SubBruteWorkerStateTx The worker is currently performing a sub-brute forcing task.
 * @var SubBruteWorkerStateFinished The worker has finished the sub-brute forcing task.
 */
typedef enum {
    SubBruteWorkerStateIDLE,
    SubBruteWorkerStateReady,
    SubBruteWorkerStateTx,
    SubBruteWorkerStateFinished
} SubBruteWorkerState;

typedef void (*SubBruteWorkerCallback)(void* context, SubBruteWorkerState state);

typedef struct SubBruteWorker SubBruteWorker;

/**
 * @brief Allocates memory for a SubBruteWorker object and initializes it with the given radio_device.
 *
 * This function creates a new SubBruteWorker object by allocating memory for it on the heap and
 * initializes it with the provided radio_device. The radio_device parameter must not be NULL.
 *
 * @param radio_device A pointer to a valid SubGhzDevice object.
 * @return A pointer to the newly allocated SubBruteWorker object, or NULL if memory allocation failed.
 */
SubBruteWorker* subbrute_worker_alloc(const SubGhzDevice* radio_device);

/**
 * @brief Frees the memory associated with the given SubBruteWorker instance.
 *
 * This function must be called to release the resources allocated to the SubBruteWorker instance.
 *
 * @param instance Pointer to the SubBruteWorker instance to be freed.
 */
void subbrute_worker_free(SubBruteWorker* instance);

/**
 * @brief Get the current step of a sub brute force worker.
 *
 * @param instance A pointer to the SubBruteWorker instance.
 * @return The current step of the sub brute force worker as a uint64_t value.
 */
uint64_t subbrute_worker_get_step(SubBruteWorker* instance);

/**
 * @brief Set the step value for a SubBruteWorker instance.
 *
 * This function sets the step value for a given SubBruteWorker instance. The step value determines the increment that
 * will be used for each iteration in the worker's execution loop.
 *
 * @param instance Pointer to the SubBruteWorker instance.
 * @param step The step value to set.
 * @return true if the step value was successfully set, false otherwise.
 */
bool subbrute_worker_set_step(SubBruteWorker* instance, uint64_t step);

/**
 * @brief Check if the SubBruteWorker is currently running.
 *
 * @param instance Pointer to the SubBruteWorker instance.
 *
 * @return true if the SubBruteWorker is running, false otherwise.
 */
bool subbrute_worker_is_running(SubBruteWorker* instance);

/**
 * @brief Initializes a SubBruteWorker with default attack settings.
 *
 * This function initializes a SubBruteWorker instance with default attack settings. The attack_type parameter determines the type of attack to be performed. The step parameter specifies
* the iteration step to be used during the attack. The protocol parameter provides the necessary protocol information for the attack. The repeats parameter specifies the number of times
* the attack should be repeated.
 *
 * @param instance A pointer to a SubBruteWorker instance.
 * @param attack_type The type of attack to be performed.
 * @param step The iteration step to be used during the attack.
 * @param protocol A pointer to the SubBruteProtocol structure containing protocol information.
 * @param repeats The number of times the attack should be repeated.
 * @return Returns true if the initialization is successful, false otherwise.
 */
bool subbrute_worker_init_default_attack(
    SubBruteWorker* instance,
    SubBruteAttacks attack_type,
    uint64_t step,
    const SubBruteProtocol* protocol,
    uint8_t repeats);

/**
 * @brief Initializes a file-based attack worker for the sub-brute algorithm.
 *
 * This function initializes a sub-brute worker to perform file-based attacks.
 * It sets the necessary parameters for the attack, such as the attack step,
 * the load index, the file key, the protocol to use, the number of repeats,
 * and whether to use two bytes.
 *
 * @param instance A pointer to the sub-brute worker instance.
 * @param step The attack step value to set.
 * @param load_index The load index value to set.
 * @param file_key The file key value to set.
 * @param protocol The sub-brute protocol to use for the attack.
 * @param repeats The number of times to repeat the attack.
 * @param two_bytes A flag indicating whether to use two bytes for the attack.
 *
 * @return True if the initialization was successful, false otherwise.
 */
bool subbrute_worker_init_file_attack(
    SubBruteWorker* instance,
    uint64_t step,
    uint8_t load_index,
    uint64_t file_key,
    SubBruteProtocol* protocol,
    uint8_t repeats,
    bool two_bytes);

/**
 * @brief Start the SubBruteWorker instance.
 *
 * This function starts the SubBruteWorker instance, allowing it to begin its work.
 *
 * @param instance Pointer to the SubBruteWorker instance to start.
 * @return Whether starting the SubBruteWorker instance was successful.
 *     - true: Starting the SubBruteWorker instance was successful.
 *     - false: Starting the SubBruteWorker instance failed.
 *
 * @note Before calling this function, make sure all the necessary inputs and configurations
 *       have been set on the SubBruteWorker instance.
 */
bool subbrute_worker_start(SubBruteWorker* instance);

/**
 * @brief Stops the given SubBruteWorker instance.
 *
 * This function stops the SubBruteWorker instance by performing necessary clean-up operations.
 * After calling this function, the instance is no longer usable.
 *
 * @param instance A pointer to the SubBruteWorker instance to stop.
 */
void subbrute_worker_stop(SubBruteWorker* instance);

/**
 * @brief Transmits the current key of the SubBruteWorker instance to another device.
 *
 * This function transmits the current key of the SubBruteWorker instance to another device
 * using the specified step value.
 *
 * @param instance The pointer to the SubBruteWorker instance.
 * @param step The step value used for transmission.
 *
 * @return True if the key was successfully transmitted, otherwise false.
 */
bool subbrute_worker_transmit_current_key(SubBruteWorker* instance, uint64_t step);

/**
 * @brief Check if the `SubBruteWorker` instance can transmit manually.
 *
 * This function is used to determine if the `SubBruteWorker` instance is capable of manual transmission.
 *
 * @param instance Pointer to the `SubBruteWorker` instance.
 * @return `true` if the `SubBruteWorker` instance can transmit manually, `false` otherwise.
 */
bool subbrute_worker_can_manual_transmit(SubBruteWorker* instance);

/**
 * @brief Set the callback function and its context for the SubBruteWorker.
 *
 * This function allows you to set the callback function and its context for
 * the given SubBruteWorker instance. The callback function will be called
 * by the SubBruteWorker at specific events, providing information to the
 * caller through the callback parameters.
 *
 * @param instance The SubBruteWorker instance to set the callback for.
 * @param callback The callback function to set.
 * @param context The context to be passed to the callback function.
 *
 * @note The callback function should be of the form:
 *        void callback(SubBruteWorker* instance, void* context);
 *      The instance parameter is the SubBruteWorker instance that triggered
 *      the callback, while the context parameter is the user-defined context
 *      that was set with this function.
 *
 * @see SubBruteWorkerCallback
 */
void subbrute_worker_set_callback(
    SubBruteWorker* instance,
    SubBruteWorkerCallback callback,
    void* context);

/**
 * @brief Get the timeout value of the SubBruteWorker instance.
 *
 * This function returns the timeout value of the SubBruteWorker instance.
 *
 * @param instance  Pointer to the SubBruteWorker instance.
 *
 * @return The timeout value as a uint8_t.
 */
uint8_t subbrute_worker_get_timeout(SubBruteWorker* instance);

/**
 * @brief Set the timeout for the SubBruteWorker instance.
 *
 * This function sets the timeout value for the SubBruteWorker instance. The timeout defines the maximum time allowed for the worker to complete its task before it is interrupted.
 *
 * @param instance A pointer to the SubBruteWorker instance.
 * @param timeout The timeout value in milliseconds.
 */
void subbrute_worker_set_timeout(SubBruteWorker* instance, uint8_t timeout);

/**
 * @brief Retrieves the number of repeated substrings found by the SubBruteWorker instance.
 *
 * This function returns the count of repeated substrings found during the execution of the SubBruteWorker instance.
 *
 * @param instance A pointer to the SubBruteWorker instance.
 * @return The number of repeated substrings.
 */
uint8_t subbrute_worker_get_repeats(SubBruteWorker* instance);

/**
 * @brief Set the number of repeats for the SubBruteWorker instance.
 *
 * This function sets the number of repeats for the SubBruteWorker instance. The repeats
 * determines how many times a certain operation should be repeated.
 *
 * @param instance A pointer to the SubBruteWorker instance.
 * @param repeats The number of repeats to be set.
 */
void subbrute_worker_set_repeats(SubBruteWorker* instance, uint8_t repeats);

/**
 * @brief Get the value of te from a SubBruteWorker instance.
 *
 * This function returns the value of the te member variable of the SubBruteWorker instance.
 *
 * @param instance A pointer to the SubBruteWorker instance.
 * @return The value of the te member variable.
 */
uint32_t subbrute_worker_get_te(SubBruteWorker* instance);

/**
 * @brief Set the value of te for the SubBruteWorker instance.
 *
 * This function sets the value of the te member variable for the given SubBruteWorker instance.
 * The te value determines the threshold for the worker to stop processing.
 *
 * @param instance Pointer to the SubBruteWorker instance
 * @param te The threshold value to set
 */
void subbrute_worker_set_te(SubBruteWorker* instance, uint32_t te);

// void subbrute_worker_timeout_inc(SubBruteWorker* instance);

// void subbrute_worker_timeout_dec(SubBruteWorker* instance);

/**
  * @brief Checks if transmission is allowed for the given value.
  *
  * This function checks the transmission allowance for the given value based on the state of the SubBruteWorker instance.
  *
  * @param instance A pointer to the SubBruteWorker instance.
  * @param value The value to check transmission allowance for.
  *
  * @return True if transmission is allowed for the given value, false otherwise.
  */
bool subbrute_worker_is_tx_allowed(SubBruteWorker* instance, uint32_t value);
