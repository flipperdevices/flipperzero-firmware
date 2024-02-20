#pragma once

#include "../subbrute_custom_event.h"
#include "../subbrute_protocols.h"
#include <gui/view.h>
#include <input/input.h>
#include <gui/elements.h>

typedef void (*SubBruteMainViewCallback)(SubBruteCustomEvent event, void* context);
typedef struct SubBruteMainView SubBruteMainView;

/**
 * @brief Sets the callback function and context for the SubBruteMainView instance.
 *
 * This function sets the callback function and context for the SubBruteMainView instance. The callback function will be
 * invoked when specific events occur in the SubBruteMainView. The context parameter allows passing additional data
 * to the callback function.
 *
 * @param instance Pointer to the SubBruteMainView instance.
 * @param callback Pointer to the callback function.
 * @param context Pointer to the context data.
 */
void subbrute_main_view_set_callback(
    SubBruteMainView* instance,
    SubBruteMainViewCallback callback,
    void* context);

/**
 * @brief Allocates memory for a new instance of SubBruteMainView.
 *
 * @return A pointer to the newly allocated SubBruteMainView object.
 * @note The caller is responsible for freeing the allocated memory using the `subbrute_main_view_free` function.
 */
SubBruteMainView* subbrute_main_view_alloc();

/**
 * @brief Frees the memory allocated for a SubBruteMainView instance.
 *
 * This function frees all the memory allocated for the given SubBruteMainView instance,
 * including its internal data structures.
 *
 * @param instance  The SubBruteMainView instance to be freed.
 */
void subbrute_main_view_free(SubBruteMainView* instance);

/**
 * @brief Get the view associated with the SubBruteMainView instance.
 *
 * This function returns the view associated with the given SubBruteMainView instance.
 *
 * @param instance The SubBruteMainView instance.
 * @return A pointer to the View object.
 */
View* subbrute_main_view_get_view(SubBruteMainView* instance);

/**
 * @brief Sets the index and other parameters for the SubBruteMainView instance.
 *
 * @param instance The SubBruteMainView instance.
 * @param idx The index value to be set.
 * @param repeats Pointer to an array of repeat values.
 * @param is_select_byte Select byte value true or false.
 * @param two_bytes Two bytes value true or false.
 * @param key_from_file The key value from file.
 */
void subbrute_main_view_set_index(
    SubBruteMainView* instance,
    uint8_t idx,
    const uint8_t* repeats,
    bool is_select_byte,
    bool two_bytes,
    uint64_t key_from_file);

/**
 * @brief Get the index of the current subview in the main view.
 *
 * This function retrieves the index of the current subview within the main view. The current
 * subview is represented by a SubBruteMainView instance.
 *
 * @param instance A pointer to the SubBruteMainView instance.
 * @return The index of the current subview.
 */
SubBruteAttacks subbrute_main_view_get_index(SubBruteMainView* instance);

/**
 * @brief Retrieves the repeats of the main view in the SubBrute application.
 *
 * This function returns the repeats of the main view in the SubBrute application as a constant pointer to an array of uint8_t values.
 *
 * @param instance A pointer to the SubBruteMainView instance.
 * @return A constant pointer to an array of uint8_t values representing the repeats of the main view.
 */
const uint8_t* subbrute_main_view_get_repeats(SubBruteMainView* instance);

/**
 * @brief Retrieves two bytes from the SubBruteMainView instance.
 *
 * This function retrieves two bytes from the SubBruteMainView instance.
 * It returns a boolean value indicating the success of the operation.
 *
 * @param instance The SubBruteMainView instance from which to retrieve two bytes.
 *
 * @return true if the bytes were successfully retrieved, false otherwise.
 */
bool subbrute_main_view_get_two_bytes(SubBruteMainView* instance);

/**
 * @brief The function handles the entry point for the brute force attack view.
 *
 * This function is responsible for handling the entry point for the brute force
 * attack view. It takes a void pointer to the context data as an argument.
 *
 * @param context A void pointer to the context data.
 */
void subbrute_attack_view_enter(void* context);

/**
 * @brief Function to exit the subbrute attack view.
 *
 * This function is used to exit the subbrute attack view, by performing any necessary clean
 * up operations and freeing up any allocated memory.
 *
 * @param context A pointer to the context of the subbrute attack view.
 *                This context contains all the necessary data and variables required by the view.
 *                It should not be NULL.
 */
void subbrute_attack_view_exit(void* context);

/**
 * @brief View input handler for subbrute attack.
 *
 * This function processes an input event and performs the appropriate action
 * for the subbrute attack view. It is called by the main event loop.
 *
 * @param event       The input event to be processed.
 * @param context     A pointer to the context data for the subbrute attack view.
 * @return            Boolean indicating whether the input event was handled successfully.
 *                    True if the event was handled successfully, false otherwise.
 */
bool subbrute_attack_view_input(InputEvent* event, void* context);

/**
 * @brief Draws a subbrute attack view on the given canvas.
 *
 * This function is responsible for drawing the subbrute attack view on the canvas.
 * The provided canvas should already be initialized and in a valid state.
 *
 * @param canvas   The canvas on which the view should be drawn.
 * @param context  A pointer to any additional context or data that may be needed for drawing.
 */
void subbrute_attack_view_draw(Canvas* canvas, void* context);
