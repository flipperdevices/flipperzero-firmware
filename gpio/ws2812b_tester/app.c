#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>

#include "led_driver.h"

#define TAG "WS2812B-Tester"

// Our application menu has 2 items.
typedef enum {
    LedTesterSubmenuIndexLeds,
    LedTesterSubmenuIndexAbout,
} LedTesterSubmenuIndex;

// Custom events that describe why the view dispatcher is calling our custom event callback.
typedef enum {
    LedTesterEventInitialized,
    LedTesterEventDeinit,
    LedTesterEventTimer,
    LedTesterEventConfigChange,
    LedTesterEventClicked,
} LedTesterEventId;

// Each view is a screen we show the user.
typedef enum {
    LedTesterViewSubmenu, // The menu when the app starts
    LedTesterViewLeds, // The LED screen
    LedTesterViewAbout, // The about screen with directions, link to social channel, etc.
} LedTesterView;

// Our model is the data we use to control the LEDs.
typedef struct {
    uint8_t led_pin_index; // The index of the pin we are using to control the LEDs
    uint32_t led_count; // The number of LEDs
    uint8_t led_pattern_index; // The pattern index
    int8_t led_divisor; // The speed divisor (1-4, -1 = stopped)
    uint8_t led_max_brightness; // The maximum brightness (0-100)
    uint8_t timer_counter; // The current timer counter (used for auto-scrolling)
    bool enable_5v; // Enable 5V output
} LedTesterModel;

// The application object.
typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    Submenu* submenu; // The application menu
    VariableItemList* variable_item_list; // The WS2812B settings
    Widget* widget_about; // The about screen
    FuriTimer* timer; // Timer for automatic updating the LEDs
    LedTesterModel* model; // The model
    LedDriver* led_driver; // The LED driver
} LedTesterApp;

// Hack so that we can access the application object from a variable_item_list on_enter/exit callback.
static LedTesterApp* global_app = NULL;

/**
 * @brief      Callback for exiting the application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t led_tester_navigation_exit_callback(void* _context) {
    UNUSED(_context);
    return VIEW_NONE;
}

/**
 * @brief      Callback for the application's menu.
 * @details    This function is called when user press back button.  We return LedTesterViewSubmenu to
 *            indicate that we want to return to the application menu.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t led_tester_navigation_submenu_callback(void* _context) {
    UNUSED(_context);
    return LedTesterViewSubmenu;
}

/**
 * @brief      Handle submenu item selection.
 * @details    This function is called when user selects an item from the submenu.
 * @param      context  The context - LedTesterApp object.
 * @param      index    The LedTesterSubmenuIndex item that was clicked.
*/
static void led_tester_submenu_callback(void* context, uint32_t index) {
    LedTesterApp* app = (LedTesterApp*)context;
    switch(index) {
    case LedTesterSubmenuIndexLeds:
        view_dispatcher_switch_to_view(app->view_dispatcher, LedTesterViewLeds);
        break;
    case LedTesterSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, LedTesterViewAbout);
        break;
    default:
        break;
    }
}

/**
 * @brief      Callback for timer elapsed.
 * @details    This function is called when the timer is elapsed.
 * @param      context  The context - LedTesterApp object.
*/
static void led_tester_timer_callback(void* context) {
    LedTesterApp* app = (LedTesterApp*)context;
    view_dispatcher_send_custom_event(app->view_dispatcher, LedTesterEventTimer);
}

/**
 * @brief      Callback for when the LED configuration settings are updated.
 * @details    This function is called when the LED configuration settings are changed by the user.
 * @param      context  The context - LedTesterApp object.
*/
static void led_tester_settings_updated(LedTesterApp* app) {
    view_dispatcher_send_custom_event(app->view_dispatcher, LedTesterEventConfigChange);
}

/**
 * @brief      Callback for when the LED configuration settings are clicked.
 * @details    This function is called when the user presses OK button while in the LED configuration settings.
 * @param      context  The context - LedTesterApp object.
*/
static void led_tester_item_clicked(void* context, uint32_t index) {
    UNUSED(index);
    LedTesterApp* app = (LedTesterApp*)context;
    view_dispatcher_send_custom_event(app->view_dispatcher, LedTesterEventClicked);
}

/**
 * @brief      Enable 5V power.
 * @details    This function enables the 5V power output on pin 1.
*/
static void led_tester_5v_power_on() {
    uint8_t attempts = 5;
    while(--attempts > 0) {
        if(furi_hal_power_enable_otg()) break;
    }
}

/**
 * @brief      Disable 5V power.
 * @details    This function disables the 5V power output on pin 1.
*/
static void led_tester_5v_power_off() {
    if(furi_hal_power_is_otg_enabled()) {
        furi_hal_power_disable_otg();
    }
}

// Settings for configuring which GPIO pin to use for controlling the WS2812B LEDs.
static const char* setting_led_pin_config_label = "LED Pin";
static const GpioPin* setting_led_pin_values[] =
    {&gpio_ext_pa7, &gpio_ext_pa6, &gpio_ext_pa4, &gpio_ext_pb3, &gpio_ext_pb2, &gpio_ext_pc3};
static char* setting_led_pin_names[] = {"A7", "A6", "A4", "B3", "B2", "C3"};
static void led_tester_setting_led_pin_change(VariableItem* item) {
    LedTesterApp* app = variable_item_get_context(item);
    LedTesterModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_led_pin_names[index]);
    model->led_pin_index = index;
}

// Settings for configuring how many LEDs to enable.
static const char* setting_led_count_config_label = "LED Count";
static uint16_t setting_led_count_values[] = {1, 2, 3, 4, 5, 6, 7, 8, 16, 32, 64, 128, 256, 512};
static char* setting_led_count_names[] =
    {"1", "2", "3", "4", "5", "6", "7", "8", "16", "32", "64", "128", "256", "512"};
static uint8_t setting_led_count_default_index = 3; // 4 LEDs
static void led_tester_setting_led_count_change(VariableItem* item) {
    LedTesterApp* app = variable_item_get_context(item);
    LedTesterModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_led_count_names[index]);
    model->led_count = setting_led_count_values[index];
    led_tester_settings_updated(app);
}

// Settings for configuring which LED pattern to use.
static const char* setting_led_pattern_config_label = "LED Pattern";
static char* setting_led_pattern_names[] =
    {"Red", "Green", "Blue", "White", "RGBW", "GBWR", "BWRG", "WRGB"};
static void led_tester_setting_led_pattern_change(VariableItem* item) {
    LedTesterApp* app = variable_item_get_context(item);
    LedTesterModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_led_pattern_names[index]);
    model->led_pattern_index = index;
    led_tester_settings_updated(app);
}

// Settings for configuring which LED speed to use.
static const char* setting_led_speed_config_label = "LED Speed";
static int16_t setting_led_speed_values[] = {-1, 3, 2, 1};
static char* setting_led_speed_names[] = {"Stopped", "Slow", "Medium", "Fast"};
static void led_tester_setting_led_speed_change(VariableItem* item) {
    LedTesterApp* app = variable_item_get_context(item);
    LedTesterModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_led_speed_names[index]);
    model->led_divisor = setting_led_speed_values[index];
    led_tester_settings_updated(app);
}

// Settings for configuring the LED brightness.
static const char* setting_led_brightness_config_label = "LED Brightness";
static uint16_t setting_led_brightness_values[] = {1, 2, 3, 5, 10, 20, 25, 50, 75, 100};
static char* setting_led_brightness_names[] =
    {"1%", "2%", "3%", "5%", "10%", "20%", "25%", "50%", "75%", "100%"};
static void led_tester_setting_led_brightness_change(VariableItem* item) {
    LedTesterApp* app = variable_item_get_context(item);
    LedTesterModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_led_brightness_names[index]);
    model->led_max_brightness = setting_led_brightness_values[index];
    led_tester_settings_updated(app);
}

// Settings for configuring the 5V output.
static const char* setting_5v_config_label = "Enable +5V pin";
static bool setting_5v_values[] = {true, false};
static char* setting_5v_names[] = {"Yes", "No"};
static void led_tester_setting_5v_change(VariableItem* item) {
    LedTesterApp* app = variable_item_get_context(item);
    LedTesterModel* model = app->model;
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_5v_names[index]);
    model->enable_5v = setting_5v_values[index];
    if(app->model->enable_5v) {
        led_tester_5v_power_on();
    } else {
        led_tester_5v_power_off();
    }
}

/**
 * @brief      Callback for custom events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      context  The context - LedTesterApp object.
 * @param      event    The event id - LedTesterEventId value.
*/
static bool led_tester_custom_event_callback(void* context, uint32_t event) {
    LedTesterApp* app = (LedTesterApp*)context;
    const GpioPin* pin = setting_led_pin_values[app->model->led_pin_index];

    if(!app->led_driver) {
        FURI_LOG_E(TAG, "led_driver is NULL.  Custom event %lu ignored.", event);
        return false;
    }

    if(event == LedTesterEventTimer) {
        app->model->timer_counter++;
    }

    uint8_t offset = (app->model->led_divisor == -1) ?
                         0 :
                         (app->model->timer_counter / app->model->led_divisor) & 0x3;

    uint32_t rgb[4] = {0};
    switch(app->model->led_pattern_index) {
    case 0: // RED
        for(size_t i = 0; i < COUNT_OF(rgb); i++) {
            rgb[i] = 0xFF0000;
        }
        break;
    case 1: // GREEN
        for(size_t i = 0; i < COUNT_OF(rgb); i++) {
            rgb[i] = 0x00FF00;
        }
        break;
    case 2: // BLUE
        for(size_t i = 0; i < COUNT_OF(rgb); i++) {
            rgb[i] = 0x0000FF;
        }
        break;
    case 3: // WHITE
        for(size_t i = 0; i < COUNT_OF(rgb); i++) {
            rgb[i] = 0xFFFFFF;
        }
        break;
    case 4: // RGBW
        rgb[0] = 0xFF0000;
        rgb[1] = 0x00FF00;
        rgb[2] = 0x0000FF;
        rgb[3] = 0xFFFFFF;
        break;
    case 5: // GBWR
        rgb[3] = 0xFF0000;
        rgb[0] = 0x00FF00;
        rgb[1] = 0x0000FF;
        rgb[2] = 0xFFFFFF;
        break;
    case 6: // BWRG
        rgb[2] = 0xFF0000;
        rgb[3] = 0x00FF00;
        rgb[0] = 0x0000FF;
        rgb[1] = 0xFFFFFF;
        break;
    case 7: // WRGB
        rgb[0] = 0xFFFFFF;
        rgb[1] = 0xFF0000;
        rgb[2] = 0x00FF00;
        rgb[3] = 0x0000FF;
        break;
    default:
        break;
    }

    // Rotate the pattern
    for(size_t i = 0; i < offset; i++) {
        uint32_t tmp = rgb[0];
        for(size_t j = 0; j < COUNT_OF(rgb) - 1; j++) {
            rgb[j] = rgb[j + 1];
        }
        rgb[COUNT_OF(rgb) - 1] = tmp;
    }

    // If deinit, turn off the LEDs
    if(event == LedTesterEventDeinit) {
        for(size_t i = 0; i < COUNT_OF(rgb); i++) {
            rgb[i] = 0;
        }
    }

    // Scale the brightness
    for(size_t i = 0; i < COUNT_OF(rgb); i++) {
        rgb[i] = (((rgb[i] >> 16) & 0xFF) * app->model->led_max_brightness / 100) << 16 |
                 (((rgb[i] >> 8) & 0xFF) * app->model->led_max_brightness / 100) << 8 |
                 ((rgb[i] & 0xFF) * app->model->led_max_brightness / 100);
        // rgb[i] = (rgb[i] & 0x0F0F0F); // For Debugging, just use lower 4-bits.
    }

    led_driver_set_pin(app->led_driver, pin);

    // Set the LEDs to the pattern
    for(size_t i = 0; i < app->model->led_count; i++) {
        led_driver_set_led(app->led_driver, i, rgb[i % COUNT_OF(rgb)]);
    }
    // Turn off any remaining LEDs
    for(size_t i = app->model->led_count; i < MAX_LED_COUNT; i++) {
        led_driver_set_led(app->led_driver, i, 0);
    }

    led_driver_transmit(app->led_driver, true);

    if(event == LedTesterEventDeinit) {
        led_driver_free(app->led_driver);
        app->led_driver = NULL;
    }

    return true;
}

/**
 * @brief      Callback for entering the LED configuration settings.
 * @details    This function is called when the user enters the LED configuration settings.  We 
 *              start the periodic timer to update the LEDs & we signal initialized so the LEDs
 *              turn on to their default state.
 * @param      _context  The context - unused
*/
void led_tester_enter_leds_callback(void* _context) {
    // _context is variable_item_list, but it doesn't expose the item, so we can't get the context.
    UNUSED(_context);
    // Hack, we use a global to access the app object.
    LedTesterApp* app = (LedTesterApp*)global_app;
    app->timer = furi_timer_alloc(led_tester_timer_callback, FuriTimerTypePeriodic, app);
    furi_timer_start(app->timer, 250);
    view_dispatcher_send_custom_event(app->view_dispatcher, LedTesterEventInitialized);
    app->led_driver =
        led_driver_alloc(MAX_LED_COUNT, setting_led_pin_values[app->model->led_pin_index]);
}

/**
 * @brief      Callback for exiting the LED configuration settings.
 * @details    This function is called when the user exits the LED configuration settings.  We 
 *              stop the periodic timer to update the LEDs.
 * @param      _context  The context - unused
*/
void led_tester_exit_leds_callback(void* _context) {
    // _context is variable_item_list, but it doesn't expose the item, so we can't get the context.
    UNUSED(_context);
    // Hack, we use a global to access the app object.
    LedTesterApp* app = (LedTesterApp*)global_app;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
    view_dispatcher_send_custom_event(app->view_dispatcher, LedTesterEventDeinit);
}

/**
 * @brief      Allocate the led tester  application.
 * @details    This function allocates the led tester application resources.
 * @return     LedTesterApp object.
*/
static LedTesterApp* led_tester_app_alloc() {
    LedTesterApp* app = (LedTesterApp*)malloc(sizeof(LedTesterApp));
    global_app = app;

    app->model = (LedTesterModel*)malloc(sizeof(LedTesterModel));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Test WS2812B", LedTesterSubmenuIndexLeds, led_tester_submenu_callback, app);
    submenu_add_item(
        app->submenu, "About", LedTesterSubmenuIndexAbout, led_tester_submenu_callback, app);
    view_set_previous_callback(
        submenu_get_view(app->submenu), led_tester_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, LedTesterViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_custom_event_callback(
        app->view_dispatcher, led_tester_custom_event_callback);
    view_dispatcher_switch_to_view(app->view_dispatcher, LedTesterViewSubmenu);

    app->variable_item_list = variable_item_list_alloc();
    variable_item_list_set_enter_callback(app->variable_item_list, led_tester_item_clicked, app);
    view_set_enter_callback(
        variable_item_list_get_view(app->variable_item_list), led_tester_enter_leds_callback);
    view_set_exit_callback(
        variable_item_list_get_view(app->variable_item_list), led_tester_exit_leds_callback);
    variable_item_list_reset(app->variable_item_list);

    // Pin
    VariableItem* item = variable_item_list_add(
        app->variable_item_list,
        setting_led_pin_config_label,
        COUNT_OF(setting_led_pin_names),
        led_tester_setting_led_pin_change,
        app);
    uint8_t setting_led_pin_index = 0;
    variable_item_set_current_value_index(item, setting_led_pin_index);
    variable_item_set_current_value_text(item, setting_led_pin_names[setting_led_pin_index]);
    app->model->led_pin_index = setting_led_pin_index;

    // Count
    item = variable_item_list_add(
        app->variable_item_list,
        setting_led_count_config_label,
        COUNT_OF(setting_led_count_names),
        led_tester_setting_led_count_change,
        app);
    uint8_t setting_led_count_index = setting_led_count_default_index;
    variable_item_set_current_value_index(item, setting_led_count_index);
    variable_item_set_current_value_text(item, setting_led_count_names[setting_led_count_index]);
    app->model->led_count = setting_led_count_values[setting_led_count_index];

    // Pattern
    item = variable_item_list_add(
        app->variable_item_list,
        setting_led_pattern_config_label,
        COUNT_OF(setting_led_pattern_names),
        led_tester_setting_led_pattern_change,
        app);
    uint8_t setting_led_pattern_index = 0;
    variable_item_set_current_value_index(item, setting_led_pattern_index);
    variable_item_set_current_value_text(
        item, setting_led_pattern_names[setting_led_pattern_index]);
    app->model->led_pattern_index = setting_led_pattern_index;

    // Speed
    item = variable_item_list_add(
        app->variable_item_list,
        setting_led_speed_config_label,
        COUNT_OF(setting_led_speed_names),
        led_tester_setting_led_speed_change,
        app);
    uint8_t setting_led_speed_index = 0;
    variable_item_set_current_value_index(item, setting_led_speed_index);
    variable_item_set_current_value_text(item, setting_led_speed_names[setting_led_speed_index]);
    app->model->led_divisor = setting_led_speed_values[setting_led_speed_index];

    // Brightness
    item = variable_item_list_add(
        app->variable_item_list,
        setting_led_brightness_config_label,
        COUNT_OF(setting_led_brightness_names),
        led_tester_setting_led_brightness_change,
        app);
    uint8_t setting_led_brightness_index = 3;
    variable_item_set_current_value_index(item, setting_led_brightness_index);
    variable_item_set_current_value_text(
        item, setting_led_brightness_names[setting_led_brightness_index]);
    app->model->led_max_brightness = setting_led_brightness_values[setting_led_brightness_index];

    // 5-volt pin
    item = variable_item_list_add(
        app->variable_item_list,
        setting_5v_config_label,
        COUNT_OF(setting_5v_names),
        led_tester_setting_5v_change,
        app);
    uint8_t setting_5v_index = 0;
    variable_item_set_current_value_index(item, setting_5v_index);
    variable_item_set_current_value_text(item, setting_5v_names[setting_5v_index]);
    app->model->enable_5v = setting_5v_values[setting_5v_index];
    if(app->model->enable_5v) {
        led_tester_5v_power_on();
    }

    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list),
        led_tester_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        LedTesterViewLeds,
        variable_item_list_get_view(app->variable_item_list));

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "This is a WS2812B LED tester\nVersion 1.7\nConnect WS2812B LED data\nwire to GPIO pin on Flipper.\n\nThe 3V3 pin has a 1200mA\nmax current (~4 watts). The\n5V pin has a 1000mA max\ncurrent (5 watts).\n\nauthors: @codeallnight and\nZ3BRO!\n\nhttps://discord.com/invite/NsjCvqwPAd\nhttps://youtube.com/@MrDerekJamison\n\n");
    view_set_previous_callback(
        widget_get_view(app->widget_about), led_tester_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, LedTesterViewAbout, widget_get_view(app->widget_about));

    return app;
}

/**
 * @brief      Free the led tester application.
 * @details    This function frees the led tester application resources.
 * @param      app  The led tester application object.
*/
static void led_tester_app_free(LedTesterApp* app) {
    view_dispatcher_remove_view(app->view_dispatcher, LedTesterViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, LedTesterViewLeds);
    variable_item_list_free(app->variable_item_list);
    view_dispatcher_remove_view(app->view_dispatcher, LedTesterViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    free(app);
}

/**
 * @brief      Main function for led tester application.
 * @details    This function is the entry point for the led tester application.  It should be defined in
 *           application.fam as the entry_point setting.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t ws2812b_led_tester_app(void* _p) {
    UNUSED(_p);
    LedTesterApp* app = led_tester_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    led_tester_app_free(app);
    return 0;
}