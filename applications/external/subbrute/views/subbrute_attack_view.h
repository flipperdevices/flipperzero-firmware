#pragma once

#include "../subbrute_custom_event.h"
#include <gui/view.h>
#include <input/input.h>
#include <gui/elements.h>

typedef void (*SubBruteAttackViewCallback)(SubBruteCustomEvent event, void* context);
typedef struct SubBruteAttackView SubBruteAttackView;

/**
 * @brief Sets the callback function and context for the SubBruteAttackView.
 *
 * This function sets the callback function and context that will be used by the SubBruteAttackView
 * instance. The callback function will be called when a certain event occurs in the SubBruteAttackView.
 * The context parameter will be passed to the callback function as an argument.
 *
 * @param instance The SubBruteAttackView instance to set the callback for.
 * @param callback The callback function to be called when the event occurs.
 * @param context The context to be passed to the callback function when it is called.
 *
 * @note The callback function should have the following signature:
 *       void callback(void* context)
 *
 * @note The callback function should not take ownership of the instance, it should only perform
 *       the necessary logic based on the event.
 *
 * @warning The instance parameter must not be NULL.
 * @warning The callback parameter must not be NULL.
 *
 */
void subbrute_attack_view_set_callback(
    SubBruteAttackView* instance,
    SubBruteAttackViewCallback callback,
    void* context);

/**
 * @brief Allocates memory for a new SubBruteAttackView object.
 *
 * This function allocates memory for a new SubBruteAttackView object on the heap and returns a pointer to it.
 *
 * @return A pointer to a newly allocated SubBruteAttackView object.
 */
SubBruteAttackView* subbrute_attack_view_alloc();

/**
 * @brief Frees the memory allocated for a SubBruteAttackView instance.
 *
 * @param instance Pointer to the SubBruteAttackView instance to free.
 */
void subbrute_attack_view_free(SubBruteAttackView* instance);

/**
 * @brief Retrieves the view associated with the SubBruteAttackView instance.
 *
 * @param instance The SubBruteAttackView instance to retrieve the view from.
 *
 * @return A pointer to the associated view.
 */
View* subbrute_attack_view_get_view(SubBruteAttackView* instance);

/**
 * @brief Sets the current step for the SubBruteAttackView instance.
 *
 * This function sets the current step of the SubBruteAttackView instance
 * to the specified value.
 *
 * @param instance A pointer to the SubBruteAttackView instance.
 * @param current_step The value to set as the current step.
 *
 * @note The current step represents the current progress of the attack view.
 *       It should be an unsigned 64-bit integer.
 */
void subbrute_attack_view_set_current_step(SubBruteAttackView* instance, uint64_t current_step);

/**
 * @class SubBruteAttackView
 * @brief Class for initializing the values of a SubBruteAttackView instance.
 *
 * The SubBruteAttackView class is used to store and initialize the values
 * of a SubBruteAttackView instance. These values include the index, maximum value,
 * current step, attack status, and extra repeats.
 */
void subbrute_attack_view_init_values(
    SubBruteAttackView* instance,
    uint8_t index,
    uint64_t max_value,
    uint64_t current_step,
    bool is_attacking,
    uint8_t extra_repeats);