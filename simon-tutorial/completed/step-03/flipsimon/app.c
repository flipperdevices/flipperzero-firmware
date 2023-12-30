#include <furi.h>
#include <gui/view.h>
#include "app_config.h"
#include "./common/config_colors.h"
#include "./common/custom_event.h"
#include "./common/flipboard.h"
#include "./common/flipboard_model.h"
#include "./common/leds.h"

/**
 * @brief Returns a View* object.
 * @details Returns a View* object, configured with default settings.
 * @param context Unused parameter.
 * @return View* The view* object.
 */
static View* get_primary_view(void* context) {
    UNUSED(context);
    return view_alloc();
}

/**
 * @brief Invoked whenever the main application menu is loaded.
 * @details This function is invoked whenever the main application 
 *          menu is loaded.  The first time (inital_load) we will
 *          show an LED startup sequence, then turn the LEDs off.
 *          If not the first time, we just turn the LEDs off.
 * @param model Pointer to FlipboardModel object.
 */
static void loaded_app_menu(FlipboardModel* model) {
    static bool initial_load = true;
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    if(initial_load) {
        flipboard_leds_set(leds, LedId1, adjust_color_brightness(LedColorRed, 16));
        flipboard_leds_set(leds, LedId2, adjust_color_brightness(LedColorGreen, 16));
        flipboard_leds_set(leds, LedId3, adjust_color_brightness(LedColorBlue, 16));
        flipboard_leds_set(leds, LedId4, adjust_color_brightness(LedColorCyan, 16));
        flipboard_leds_update(leds);
        furi_delay_ms(200);

        flipboard_leds_set(leds, LedId1, adjust_color_brightness(LedColorRed, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        flipboard_leds_set(leds, LedId1, adjust_color_brightness(LedColorRed, 16));
        flipboard_leds_set(leds, LedId3, adjust_color_brightness(LedColorBlue, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        flipboard_leds_set(leds, LedId3, adjust_color_brightness(LedColorBlue, 16));
        flipboard_leds_set(leds, LedId2, adjust_color_brightness(LedColorGreen, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        flipboard_leds_set(leds, LedId2, adjust_color_brightness(LedColorGreen, 16));
        flipboard_leds_set(leds, LedId4, adjust_color_brightness(LedColorCyan, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        initial_load = false;
    }

    flipboard_leds_set(leds, LedId1, LedColorBlack);
    flipboard_leds_set(leds, LedId2, LedColorBlack);
    flipboard_leds_set(leds, LedId3, LedColorBlack);
    flipboard_leds_set(leds, LedId4, LedColorBlack);
    flipboard_leds_update(leds);
}

/**
 * @brief Handles the custom events.
 * @details This function is invoked whenever the ViewDispatcher is
 *      processing a custom event.
 * @param context Pointer to Flipboard object.
 * @param event The custom event.
 * @return bool Returns true for event handled.
 */
static bool custom_event_handler(void* context, uint32_t event) {
    Flipboard* flipboard = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(flipboard);

    flipboard_model_update_gui(model);

    if(event == CustomEventAppMenuEnter) {
        loaded_app_menu(model);
    }

    return true;
}

/**
 * @brief This is the entry point of the application.
 * @details The application.fam file sets the entry_point property to this function.
 * @param p Unused parameter.
 * @return int32_t Returns a 0 for success.
 */
int32_t simon_app(void* p) {
    UNUSED(p);

    ActionModelFields fields = ActionModelFieldColorDown | ActionModelFieldFrequency;
    bool single_mode_button = true;

    Flipboard* app = flipboard_alloc(
        FLIPBOARD_APP_NAME,
        FLIPBOARD_PRIMARY_ITEM_NAME,
        ABOUT_TEXT,
        fields,
        single_mode_button,
        false,
        NULL,
        NULL,
        0,
        get_primary_view);

    view_dispatcher_set_event_callback_context(flipboard_get_view_dispatcher(app), app);
    view_dispatcher_set_custom_event_callback(
        flipboard_get_view_dispatcher(app), custom_event_handler);

    view_dispatcher_run(flipboard_get_view_dispatcher(app));

    flipboard_free(app);

    return 0;
}