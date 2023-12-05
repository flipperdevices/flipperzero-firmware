#pragma once

/**
 * @file button_model.h
 * @brief This file contains the ButtonModel type and related functions.
 * @details This file contains the ButtonModel type and related functions.
 * The ButtonModel type is used to store the settings for a button, for
 * example the color, frequency, message, and keystrokes.
 */

#include <furi.h>
#include <flipper_format/flipper_format.h>

typedef struct ButtonModel ButtonModel;

typedef struct {
    uint16_t button_code;
    uint8_t count;
} Keystroke;

typedef enum {
    ButtonModelFieldNone = 0,
    ButtonModelFieldColorUp = 1 << 0,
    ButtonModelFieldColorDown = 1 << 1,
    ButtonModelFieldFrequency = 1 << 2,
    ButtonModelFieldMessage = 1 << 3,
    ButtonModelFieldKeystrokes = 1 << 4,
    ButtonModelFieldAll = (1 << 5) - 1,
} ButtonModelFields;

/**
 * @brief Allocates a new button model.
 * @param button_id The button id for the model.
 * @return The new button model.
 */
ButtonModel* button_model_alloc(uint8_t button_id);

/**
 * @brief Allocates a new button model from a FlipperFormat.
 * @param button_id The button id for the model.
 * @param flipper_format The FlipperFormat to load from.
 * @return The new button model.
*/
ButtonModel* button_model_alloc_from_ff(uint8_t button_id, FlipperFormat* flipper_format);

/**
 * @brief Frees a button model.
 * @param model The model to free.
 */
void button_model_free(ButtonModel* model);

/**
 * @brief Gets the button id for the model.
 * @param model The model to get the button id for.
 * @return The button id for the model.
 */
uint8_t button_model_get_button_id(ButtonModel* model);

/**
 * @brief Gets the HEX color when the button is not pressed.
 * @param model The model to get the color for.
 * @return The hex color for when button is up.
 */
uint32_t button_model_get_color_up(ButtonModel* model);

/**
 * @brief Gets the HEX color when the button is pressed.
 * @param model The model to get the color for.
 * @return The hex color for when button is pressed down.
 */
uint32_t button_model_get_color_down(ButtonModel* model);

/**
 * @brief Gets the index of the menu item for editing the message.
 * @param model The model to get the message index for.
 * @return The index of the menu item for editing the message.
 */
uint8_t button_model_get_message_index(ButtonModel* model);

/**
 * @brief Gets the index of the menu item for adding a keystroke.
 * @param model The model to get the keystroke index for.
 * @return The index of the menu item for adding a keystroke.
 */
uint8_t button_model_get_keystroke_index(ButtonModel* model);

/**
 * @brief Gets the temp buffer for editing the message.
 * @param model The model to get the temp buffer for.
 * @return The temp buffer for editing the message.
 */
char* button_model_get_temp_buffer(ButtonModel* model);

/**
 * @brief Gets the size of the temp buffer for editing the message.
 * @param model The model to get the temp buffer size for.
 * @return The size of the temp buffer for editing the message.
 */
size_t button_model_get_temp_buffer_size(ButtonModel* model);

/**
 * @brief Gets the index of the item being edited.
 * @param model The model to get the temp index for.
 * @return The index of the item being edited.
 */
uint8_t button_model_get_temp_index(ButtonModel* model);

/**
 * @brief Gets the button config associated with this model.
 * @param model The model to get the button config for.
 * @return The button config associated with this model.
 */
void* button_model_get_button_config(ButtonModel* model);

/**
 * @brief Gets the frequency for the button, in Hz.
 * @param model The model to get the frequency for.
 * @return The frequency for the button.
 */
float button_model_get_frequency(ButtonModel* model);

/**
 * @brief Gets the number of keystrokes for the button.
 * @param model The model to get the keystrokes count for.
 * @return The number of keystrokes for the button.
 */
uint8_t button_model_get_keystrokes_count(ButtonModel* model);

/**
 * @brief Gets the keystroke at the given index.
 * @param model The model to get the keystroke for.
 * @param index The index of the keystroke to get.
 * @return The keystroke at the given index.
 */
Keystroke button_model_get_keystroke(ButtonModel* model, uint8_t index);

/**
 * @brief Gets the message for the button.
 * @param model The model to get the message for.
 * @param message_number The message number to get.
 * @return The message for the button.
 */
FuriString* button_model_get_message(ButtonModel* model, uint8_t message_number);

/**
 * @brief Sets the HEX color when the button is not pressed.
 * @param model The model to set the color for.
 * @param color_up The hex color for when button is up.
 */
void button_model_set_color_up(ButtonModel* model, uint32_t color_up);

/**
 * @brief Sets the HEX color when the button is pressed.
 * @param model The model to set the color for.
 * @param color_down The hex color for when button is pressed down.
 */
void button_model_set_color_down(ButtonModel* model, uint32_t color_down);

/**
 * @brief Sets the index of the menu item for editing the message.
 * @param model The model to set the message index for.
 * @param index The index of the menu item for editing the message.
 */
void button_model_set_message_index(ButtonModel* model, uint8_t index);

/**
 * @brief Sets the index of the menu item for adding a keystroke.
 * @param model The model to set the keystroke index for.
 * @param index The index of the menu item for adding a keystroke.
 */
void button_model_set_keystroke_index(ButtonModel* model, uint8_t index);

/**
 * @brief Sets the index of the item being edited.
 * @param model The model to set the temp index for.
 */
void button_model_set_temp_index(ButtonModel* model, uint8_t index);

/**
 * @brief Sets the button config associated with this model.
 * @param model The model to set the button config for.
 * @param button_config The button config associated with this model.
 */
void button_model_set_button_config(ButtonModel* model, void* button_config);

/**
 * @brief Sets the frequency for the button, in Hz.
 * @param model The model to set the frequency for.
 * @param frequency The frequency for the button.
 */
void button_model_set_frequency(ButtonModel* model, float frequency);

/**
 * @brief Sets the keystrokes and count for the button.
 * @param model The model to set the keystrokes count for.
 * @param index The index of the keystroke to set.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 * @return True if the keystroke was set, false otherwise.
 */
bool button_model_set_keystroke(
    ButtonModel* model,
    uint8_t index,
    uint16_t button_code,
    uint8_t count);

/**
 * @brief Appends a keystroke to the button.
 * @param model The model to append the keystroke to.
 * @param button_code The key code to send when this key is pressed.
 * @param count The number of keystrokes for the button.
 */
void button_model_append_keystroke(ButtonModel* model, uint16_t button_code, uint8_t count);

/**
 * @brief Removes the last keystroke from the button.
 * @param model The model to remove the keystroke from.
 * @return True if the keystroke was removed, false otherwise.
 */
bool button_model_remove_last_keystroke(ButtonModel* model);

/**
 * @brief Sets the message for the button.
 * @details Sets the message for the button. If the message is a space character, it will be
 * be considered as empty string.
 * @param model The model to set the message for.
 * @param message The message for the button.
 * @param message_number The message number to set.
 */
void button_model_set_message(ButtonModel* model, const char* message, uint8_t message_number);

/**
 * @brief Saves the button model to a FlipperFormat.
 * @param model The model to save.
 * @param flipper_format The FlipperFormat to save to.
 * @param fields The fields to save.
 * @return True if the model was saved, false otherwise.
 */
bool button_model_save(ButtonModel* model, FlipperFormat* flipper_format, ButtonModelFields fields);

/**
 * @brief Loads the button model from a FlipperFormat.
 * @param model The model to load.
 * @param flipper_format The FlipperFormat to load from.
 * @return The fields that were loaded (ButtonModelFieldNone if not found)
*/
ButtonModelFields button_model_load(ButtonModel* model, FlipperFormat* flipper_format);
