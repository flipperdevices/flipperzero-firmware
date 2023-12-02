#pragma once

/**
 * @file flipboard.h
 * @brief The main Flipboard application.
 * @details Typically you will create a Flipboard application in your main function like this:
 *    Flipboard* app = flipboard_alloc(...);
 *    view_dispatcher_run(flipboard_get_view_dispatcher(app));
 *    flipboard_free(app);
 * 
 * If you have custom configuration, you can use flipboard_override_config_view.
 * If you have custom data, you can use flipboard_model_set_custom_data.
*/

#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include "button_model.h"
#include "keystroke_selector.h"

typedef struct Flipboard Flipboard;
typedef struct FlipboardModel FlipboardModel;

/**
 * @brief Callback to get the primary view of the application.
*/
typedef View* (*GetPrimaryView)(void* context);

/**
 * @brief Allocates a new Flipboard application.
 * @param app_name The name of the application.
 * @param primary_item_name The name of the primary action (the purpose of your app).
 * @param about_text The text to display in the about view.
 * @param fields The fields to display in the button model.
 * @param single_mode_button Whether to display the button model in single mode.
 * @param attach_keyboard Whether to attach the keyboard to the application.
 * @param keys The keys to display in the keystroke selector.
 * @param shift_keys The shift keys to display in the keystroke selector.
 * @param rows The number of rows to display in the keystroke selector.
 * @param get_primary_view Callback to get the primary view of the application.
 * @return A pointer to the allocated Flipboard application.
*/
Flipboard* flipboard_alloc(
    char* app_name,
    char* primary_item_name,
    char* about_text,
    ButtonModelFields fields,
    bool single_mode_button,
    bool attach_keyboard,
    KeystrokeSelectorKey* keys,
    KeystrokeSelectorKey* shift_keys,
    uint8_t rows,
    GetPrimaryView get_primary_view);

/**
 * @brief Frees a Flipboard application.
 * @param app The Flipboard application to free.
*/
void flipboard_free(Flipboard* app);

/**
 * @brief Gets the model of a Flipboard application.
 * @param app The Flipboard application.
 * @return A pointer to the model of the Flipboard application.
*/
FlipboardModel* flipboard_get_model(Flipboard* app);

/**
 * @brief Gets the view dispatcher of a Flipboard application.
 * @param app The Flipboard application.
 * @return A pointer to the view dispatcher of the Flipboard application.
*/
ViewDispatcher* flipboard_get_view_dispatcher(Flipboard* app);

/**
 * @brief Gets the primary view of a Flipboard application.
 * @param app The Flipboard application.
 * @return A pointer to the primary view of the Flipboard application.
*/
View* flipboard_get_primary_view(Flipboard* app);

/**
 * @brief Overrides the config view of a Flipboard application.
 * @param app The Flipboard application.
 * @param view The view to override the config view with.
*/
void flipboard_override_config_view(Flipboard* app, View* view);

/**
 * @brief Gets the button model of a Flipboard application.
 * @param app The Flipboard application.
 * @return The view id for the application menu (it should be 0.)
*/
uint32_t flipboard_navigation_show_app_menu(void* context);