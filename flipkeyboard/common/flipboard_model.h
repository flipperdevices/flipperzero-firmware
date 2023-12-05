#pragma once

/**
 * @file   flipboard_model.h
 * @brief  The flipboard model.
 * @details  The flipboard model contains all the data needed for the flipboard application.
 * There are methods to get and set the data & to perform actions with the data, like sending
 * text, playing tones, etc.
*/

#include <furi.h>
#include "button_monitor.h"
#include "button_model.h"
#include "resources.h"
#include "speaker.h"

typedef struct FlipboardModel FlipboardModel;
typedef struct FlipboardKeyboard FlipboardKeyboard;
typedef struct FlipboardLeds FlipboardLeds;
typedef struct ButtonModel ButtonModel;

/**
 * @brief flipboard_model_alloc allocates a FlipboardModel.
 * @param app_name The name of the application.  The name is used to 
 * load and save the key settings, but could also be displayed to the user.
 * @param single_button_mode If true, only one button can be pressed at a time.
 * @param fields The fields of the key settings that apply for this application.
 * @return A pointer to a FlipboardModel.
*/
FlipboardModel*
    flipboard_model_alloc(char* app_name, bool single_button_mode, ButtonModelFields fields);

/**
 * @brief flipboard_model_free frees a FlipboardModel.
 * @details flipboard_model_free frees a FlipboardModel.  It also 
 * saves the key settings.
 * @param model The FlipboardModel to free.
*/
void flipboard_model_free(FlipboardModel* model);

/**
 * @brief flipboard_model_get_name gets the name of the FlipboardModel.  
 * @details The name is used to load and save the key settings, but could
 * also be displayed to the user.
 * @param model The FlipboardModel.
 * @return The name of the FlipboardModel.
*/
char* flipboard_model_get_name(FlipboardModel* model);

/**
 * @brief flipboard_model_get_resources gets the Resources for the FlipboardModel.
 * @details flipboard_model_get_resources gets the Resources for the FlipboardModel.
 * @param model The FlipboardModel.
 * @return The Resources for the FlipboardModel.
*/
Resources* flipboard_model_get_resources(FlipboardModel* model);

/**
 * @brief flipboard_model_get_single_button_mode gets the single button mode
 *  of the FlipboardModel.
 * @details  flipboard_model_get_single_button_mode gets the single button mode
 *  of the FlipboardModel. If true, only one button can be pressed at a time.
 * @param model The FlipboardModel.
 * @return True if only one button can be pressed at a time.
*/
bool flipboard_model_get_single_button_mode(FlipboardModel* model);

/**
 * @brief flipboard_model_get_button_model_fields gets the fields of the
 * button settings that apply for this application.
 * @param model The FlipboardModel.
 * @return The fields of the button settings that apply for this application.
*/
ButtonModelFields flipboard_model_get_button_model_fields(FlipboardModel* model);

/**
 * @brief flipboard_model_get_button_model gets the ButtonModel for a given button.
 * @brief flipboard_model_get_button_model gets the ButtonModel for a given button. 
 * For single buttons, the valid indexes are 0, 1, 3, 7.  For multi buttons, the valid indexes
 * are 0-15.  This function may return NULL, if there is no setting.
 * @param model The FlipboardModel.
 * @param button The button.
 * @return The ButtonModel for the button.
*/
ButtonModel* flipboard_model_get_button_model(FlipboardModel* model, uint8_t button);

/**
 * @brief flipboard_model_get_button_monitor gets the ButtonMonitor for the FlipboardModel.
 * @details flipboard_model_get_button_monitor gets the ButtonMonitor for the FlipboardModel.
 * The ButtonMonitor is used to monitor the FlipBoard button presses.
 * @param model The FlipboardModel.
 * @return The ButtonMonitor for the FlipboardModel.
*/
ButtonMonitor* flipboard_model_get_button_monitor(FlipboardModel* model);

/**
 * @brief flipboard_model_get_keyboard gets the Keyboard for the FlipboardModel.
 * @details flipboard_model_get_keyboard gets the Keyboard for the FlipboardModel.
 * The Keyboard is used to send keystrokes to the host.
 * @param model The FlipboardModel.
 * @return The Keyboard for the FlipboardModel.
*/
FlipboardKeyboard* flipboard_model_get_keyboard(FlipboardModel* model);

/**
 * @brief flipboard_model_get_leds gets the Leds for the FlipboardModel.
 * @details flipboard_model_get_leds gets the Leds for the FlipboardModel. 
 * The Leds are the addressable LEDs on the FlipBoard, that light each button.
 * @param model The FlipboardModel.
 * @return The Leds for the FlipboardModel.
*/
FlipboardLeds* flipboard_model_get_leds(FlipboardModel* model);

/**
 * @brief flipboard_model_get_speaker gets the Speaker for the FlipboardModel.
 * @details flipboard_model_get_speaker gets the Speaker for the FlipboardModel.
 * The Speaker is the internal Flipper Zero speaker and is used to play tones.
 * @param model The FlipboardModel.
 * @return The Speaker for the FlipboardModel.
*/
Speaker* flipboard_model_get_speaker(FlipboardModel* model);

/**
 * @brief flipboard_model_get_custom_data gets the custom data for the FlipboardModel.
 * @details flipboard_model_get_custom_data gets the custom data for the FlipboardModel.
 * The custom data is used to store application specific data, use this to further
 * extend the FlipboardModel.
 * @param model The FlipboardModel.
 * @return The custom data for the FlipboardModel.
*/
void* flipboard_model_get_custom_data(FlipboardModel* model);

/**
 * @brief flipboard_model_set_custom_data sets the custom data for the FlipboardModel.
 * @details flipboard_model_set_custom_data sets the custom data for the FlipboardModel.
 * The custom data is used to store application specific data, use this to further
 * extend the FlipboardModel.  The caller is responsible for freeing any previous data
 * and for freeing the new custom_data.
 * @param model The FlipboardModel.
 * @param custom_data The custom data for the FlipboardModel.
*/
void flipboard_model_set_custom_data(FlipboardModel* model, void* custom_data);

/**
 * @brief flipboard_model_update_gui redraws the GUI.
 * @param model The FlipboardModel.
*/
void flipboard_model_update_gui(FlipboardModel* model);

/**
 * @brief flipboard_model_get_gui_refresh_speed_ms gets the GUI refresh speed in milliseconds.
 * @details flipboard_model_get_gui_refresh_speed_ms gets the GUI refresh speed in milliseconds.
 * The GUI refresh speed is the rate at which the GUI is updated.  The GUI is updated
 * by calling the flipboard_model_tick callback, which invokes __gui_redraw.
 * @param model The FlipboardModel.
 * @return The GUI refresh speed in milliseconds.
*/
void flipboard_model_set_gui_refresh_speed_ms(FlipboardModel* model, uint32_t update_rate_ms);

/**
 * @brief flipboard_model_set_button_model sets the ButtonModel for a given button.
 * @details flipboard_model_set_button_model sets the ButtonModel for a given button.
 * For single buttons, the valid indexes are 0, 1, 3, 7.  For multi buttons, the valid indexes
 * are 0-15.  The ButtonModel is used to configure the button settings.
 * @param model The FlipboardModel.
 * @param index The index of the button.
 * @param button_model The ButtonModel for the button.
*/
void flipboard_model_set_button_model(
    FlipboardModel* model,
    uint8_t index,
    ButtonModel* button_model);

/**
 * @brief flipboard_model_set_button_monitor sets the ButtonMonitor for the FlipboardModel.
 * @details flipboard_model_set_button_monitor sets the ButtonMonitor for the FlipboardModel.
 * The ButtonMonitor is used to monitor the FlipBoard button presses.
 * @param model The FlipboardModel.
 * @param callback The callback to invoke when a button is pressed.
 * @param context The context to pass to the callback.
 */
void flipboard_model_set_button_monitor(
    FlipboardModel* model,
    ButtonMonitorEventCallback callback,
    void* context);

/**
 * @brief flipboard_model_play_tone plays a tone on the FlipboardModel speaker.
 * @param model The FlipboardModel.
 * @param bm The ButtonModel for the button that was pressed.
*/
void flipboard_model_play_tone(FlipboardModel* model, ButtonModel* bm);

/**
 * @brief flipboard_model_set_colors sets the colors for the FlipboardModel.
 * @details flipboard_model_set_colors sets the colors for the FlipboardModel.
 * The colors are used to set the color of the LEDs for each button.
 * @param model The FlipboardModel.
 * @param bm The ButtonModel for the button that was pressed.
 * @param new_button The button that was pressed.
*/
void flipboard_model_set_colors(FlipboardModel* model, ButtonModel* bm, uint8_t new_button);

/**
 * @brief flipboard_model_send_keystrokes sends keystrokes to the host.
 * @details flipboard_model_send_keystrokes sends keystrokes to the host.
 * @param model The FlipboardModel.
 * @param bm The ButtonModel for the button that was pressed.
 * @return True if any "messages" (Msg1-Msg4) were also sent.
*/
bool flipboard_model_send_keystrokes(FlipboardModel* model, ButtonModel* bm);

/**
 * @brief flipboard_model_send_text sends text to the host.
 * @details flipboard_model_send_text sends text to the host.
 * @param model The FlipboardModel.
 * @param bm The ButtonModel for the button that was pressed.
 * @param message_number The message number to send (0-3).
*/
void flipboard_model_send_text(FlipboardModel* model, ButtonModel* bm, uint8_t message_number);

/**
 * @brief flipboard_model_reduce reduces the button presses to a single button.
 * @details flipboard_model_reduce reduces the button presses to a single button, if in
 * single button mode (otherwise returns the button combo).
 * @param model The FlipboardModel.
 * @param new_button The button that was pressed.
 * @param left_wins If true, the leftmost button wins, otherwise the rightmost button wins.
 * @return The reduced button.
*/
uint8_t flipboard_model_reduce(FlipboardModel* model, uint8_t new_button, bool left_wins);
