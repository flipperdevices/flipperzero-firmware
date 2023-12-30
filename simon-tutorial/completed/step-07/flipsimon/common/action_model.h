/**
 * @file action_model.h
 * @brief This file contains the ActionModel type and related functions.
 * @details This file contains the ActionModel type and related functions.
 * The ActionModel type is used to store the settings for an action, for
 * example the color, frequency, message, and keystrokes.
 */

#pragma once

#include <furi.h>
#include <flipper_format/flipper_format.h>

typedef struct ActionModel ActionModel;

typedef struct {
    uint16_t button_code;
    uint8_t count;
} Keystroke;

typedef enum {
    ActionModelFieldNone = 0,
    ActionModelFieldColorUp = 1 << 0,
    ActionModelFieldColorDown = 1 << 1,
    ActionModelFieldFrequency = 1 << 2,
    ActionModelFieldMessage = 1 << 3,
    ActionModelFieldKeystrokes = 1 << 4,
    ActionModelFieldAll = (1 << 5) - 1,
} ActionModelFields;

/**
 * @brief Allocates a new action model.
 * @param action_id The action id for the model.
 * @return The new action model.
 */
ActionModel* action_model_alloc(uint8_t action_id);

/**
 * @brief Allocates a new action model from a FlipperFormat.
 * @param action_id The action id for the model.
 * @param flipper_format The FlipperFormat to load from.
 * @return The new action model.
*/
ActionModel* action_model_alloc_from_ff(uint8_t action_id, FlipperFormat* flipper_format);

/**
 * @brief Frees a action model.
 * @param model The model to free.
 */
void action_model_free(ActionModel* model);

/**
 * @brief Gets the action id for the model.
 * @param model The model to get the action id for.
 * @return The action id for the model.
 */
uint8_t action_model_get_action_id(ActionModel* model);

/**
 * @brief Gets the HEX color when the action is not active.
 * @param model The model to get the color for.
 * @return The hex color for when action is not active.
 */
uint32_t action_model_get_color_up(ActionModel* model);

/**
 * @brief Gets the HEX color when the action is active.
 * @param model The model to get the color for.
 * @return The hex color for when action is active.
 */
uint32_t action_model_get_color_down(ActionModel* model);

/**
 * @brief Gets the index of the menu item for editing the message.
 * @param model The model to get the message index for.
 * @return The index of the menu item for editing the message.
 */
uint8_t action_model_get_message_index(ActionModel* model);

/**
 * @brief Gets the index of the menu item for adding a keystroke.
 * @param model The model to get the keystroke index for.
 * @return The index of the menu item for adding a keystroke.
 */
uint8_t action_model_get_keystroke_index(ActionModel* model);

/**
 * @brief Gets the temp buffer for editing the message.
 * @param model The model to get the temp buffer for.
 * @return The temp buffer for editing the message.
 */
char* action_model_get_temp_buffer(ActionModel* model);

/**
 * @brief Gets the size of the temp buffer for editing the message.
 * @param model The model to get the temp buffer size for.
 * @return The size of the temp buffer for editing the message.
 */
size_t action_model_get_temp_buffer_size(ActionModel* model);

/**
 * @brief Gets the index of the item being edited.
 * @param model The model to get the temp index for.
 * @return The index of the item being edited.
 */
uint8_t action_model_get_temp_index(ActionModel* model);

/**
 * @brief Gets the action config associated with this model.
 * @param model The model to get the action config for.
 * @return The action config associated with this model.
 */
void* action_model_get_action_config(ActionModel* model);

/**
 * @brief Gets the frequency for the action, in Hz.
 * @param model The model to get the frequency for.
 * @return The frequency for the action.
 */
float action_model_get_frequency(ActionModel* model);

/**
 * @brief Gets the number of keystrokes for the action.
 * @param model The model to get the keystrokes count for.
 * @return The number of keystrokes for the action.
 */
uint8_t action_model_get_keystrokes_count(ActionModel* model);

/**
 * @brief Gets the keystroke at the given index.
 * @param model The model to get the keystroke for.
 * @param index The index of the keystroke to get.
 * @return The keystroke at the given index.
 */
Keystroke action_model_get_keystroke(ActionModel* model, uint8_t index);

/**
 * @brief Gets the message for the action.
 * @param model The model to get the message for.
 * @param message_number The message number to get.
 * @return The message for the action.
 */
FuriString* action_model_get_message(ActionModel* model, uint8_t message_number);

/**
 * @brief Sets the HEX color when the action is not active.
 * @param model The model to set the color for.
 * @param color_up The hex color for when action is not active.
 */
void action_model_set_color_up(ActionModel* model, uint32_t color_up);

/**
 * @brief Sets the HEX color when the action is pressed.
 * @param model The model to set the color for.
 * @param color_down The hex color for when action is pressed down.
 */
void action_model_set_color_down(ActionModel* model, uint32_t color_down);

/**
 * @brief Sets the index of the menu item for editing the message.
 * @param model The model to set the message index for.
 * @param index The index of the menu item for editing the message.
 */
void action_model_set_message_index(ActionModel* model, uint8_t index);

/**
 * @brief Sets the index of the menu item for adding a keystroke.
 * @param model The model to set the keystroke index for.
 * @param index The index of the menu item for adding a keystroke.
 */
void action_model_set_keystroke_index(ActionModel* model, uint8_t index);

/**
 * @brief Sets the index of the item being edited.
 * @param model The model to set the temp index for.
 */
void action_model_set_temp_index(ActionModel* model, uint8_t index);

/**
 * @brief Sets the action config associated with this model.
 * @param model The model to set the action config for.
 * @param action_config The action config associated with this model.
 */
void action_model_set_action_config(ActionModel* model, void* action_config);

/**
 * @brief Sets the frequency for the action, in Hz.
 * @param model The model to set the frequency for.
 * @param frequency The frequency for the action.
 */
void action_model_set_frequency(ActionModel* model, float frequency);

/**
 * @brief Sets the keystrokes and count for the action.
 * @param model The model to set the keystrokes count for.
 * @param index The index of the keystroke to set.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 * @return True if the keystroke was set, false otherwise.
 */
bool action_model_set_keystroke(
    ActionModel* model,
    uint8_t index,
    uint16_t button_code,
    uint8_t count);

/**
 * @brief Appends a keystroke to the action.
 * @param model The model to append the keystroke to.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 */
void action_model_append_keystroke(ActionModel* model, uint16_t button_code, uint8_t count);

/**
 * @brief Removes the last keystroke from the action.
 * @param model The model to remove the keystroke from.
 * @return True if the keystroke was removed, false otherwise.
 */
bool action_model_remove_last_keystroke(ActionModel* model);

/**
 * @brief Sets the message for the action.
 * @details Sets the message for the action. If the message is a space character, it will be
 * be considered as empty string.
 * @param model The model to set the message for.
 * @param message The message for the action.
 * @param message_number The message number to set.
 */
void action_model_set_message(ActionModel* model, const char* message, uint8_t message_number);

/**
 * @brief Saves the action model to a FlipperFormat.
 * @param model The model to save.
 * @param flipper_format The FlipperFormat to save to.
 * @param fields The fields to save.
 * @return True if the model was saved, false otherwise.
 */
bool action_model_save(ActionModel* model, FlipperFormat* flipper_format, ActionModelFields fields);

/**
 * @brief Loads the action model from a FlipperFormat.
 * @param model The model to load.
 * @param flipper_format The FlipperFormat to load from.
 * @return The fields that were loaded (ActionModelFieldNone if not found)
*/
ActionModelFields action_model_load(ActionModel* model, FlipperFormat* flipper_format);
