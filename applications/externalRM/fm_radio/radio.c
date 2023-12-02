/**
 *
 * @author Coolshrimp - CoolshrimpModz.com
 *
 * @brief FM Radio using the TEA5767 FM radio chip.
 * @version 1.1
 * @date 2023-09-29
 * 
 * @copyright GPLv3
 */

#include <furi.h>
#include <furi_hal.h>
#include <stdint.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/elements.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stdbool.h> // for true/false
#include <stdint.h> // for uint8_t, size_t
#include <stdio.h> // for snprintf
#include <math.h> // for fabsf
#include "TEA5767/TEA5767.h"
#include <gui/icon_i.h>
#include "fm_radio_icons.h"

// Define macros for easier management
#define BACKLIGHT_ALWAYS_ON
#define TAG "FM_Radio"

// Declare global variables
uint8_t volume_values[] = {0, 1};
char* volume_names[] = {"Un-Muted", "Muted"};
bool current_volume = 1; // Current volume state
int* signal_strength; // Signal strength (unused, consider removing or implementing)
uint8_t tea5767_registers[5];
uint32_t current_station_index = 0; // Default to the first frequency

// Station struct to hold frequency and station name
struct Station {
    float frequency;
    char* name;
};

#define NUM_VOLUME_VALUES (sizeof(volume_values) / sizeof(volume_values[0]))
#define NUM_STATIONS (sizeof(stations) / sizeof(stations[0]))

// Array of Stations
struct Station stations[] = {
    {88.1, "CIND Indie 88"},
    {88.9, "CIRV Portuguese"},
    {89.5, "CIUT FM"},
    {89.7, "89.7 K-Love"},
    {90.3, "Ici Musique"},
    {91.1, "JAZZ.FM91"},
    {92.3, "Zeta 92"},
    {92.5, "KiSS Radio"},
    {93.1, "Easy 93.1"},
    {93.5, "93.5 Today"},
    {93.9, "Urbana FM"},
    {94.1, "CBC Music"},
    {94.5, "Star 94.5"},
    {95.7, "Ritmo 95.7"},
    {95.9, "KX96"},
    {96.3, "Classical 96.3"},
    {96.5, "Power 96"},
    {97.3, "Hits 97.3"},
    {98.1, "CHFI"},
    {98.3, "Mix 98.3"},
    {98.7, "Flow 98.7"},
    {99.1, "99 Jamz"},
    {99.5, "Viva FM"},
    {99.9, "99.9 Kiss Country"},
    {100.3, "Rumba FM"},
    {100.7, "Y100.7"},
    {101.1, "The Rock Station"},
    {101.3, "CMR Diversity"},
    {101.5, "101.5 Lite FM"},
    {102.1, "102.1 Edge"},
    {102.7, "102.7 The Beach"},
    {103.5, "The Beat 103.5"},
    {103.9, "Proud FM"},
    {104.5, "CHUM"},
    {105.1, "HOT 105"},
    {105.5, "VIBE1055 FM"},
    {106.5, "106.5 Elmnt"},
    {106.7, "106.7 El Zol"},
    {107.1, "Q107"},
    {107.5, "Amor 107.5"},
};

// Function prototypes for forward declarations
void elements_button_top_left(Canvas* canvas, const char* str);
void elements_button_top_right(Canvas* canvas, const char* str);
//lib can only do bottom left/right
void elements_button_top_left(Canvas* canvas, const char* str) {
    const uint8_t button_height = 10; // Define the height of the button
    const uint8_t vertical_offset = 2; // Define the vertical offset of the text
    const uint8_t horizontal_offset = 2; // Define the horizontal offset of the text
    const uint8_t string_width = canvas_string_width(canvas, str);
    const Icon* icon = &I_ButtonUp;
    const uint8_t icon_h_offset = 2;
    const uint8_t icon_width_with_offset = icon->width + icon_h_offset;
    const uint8_t icon_v_offset = icon->height + vertical_offset;
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset;

    const uint8_t x = 0;
    const uint8_t y = 0 + button_height;

    canvas_draw_box(canvas, x, y - button_height, button_width, button_height); // Draw the button
    canvas_draw_line(
        canvas,
        x + button_width + 0,
        y - button_height,
        x + button_width + 0,
        y - 1); // Draw the button border
    canvas_draw_line(
        canvas,
        x + button_width + 1,
        y - button_height,
        x + button_width + 1,
        y - 2); // Draw the button border
    canvas_draw_line(
        canvas,
        x + button_width + 2,
        y - button_height,
        x + button_width + 2,
        y - 3); // Draw the button border

    canvas_invert_color(canvas); // Invert the color of the text and icon
    canvas_draw_str(
        canvas,
        x + horizontal_offset + icon_width_with_offset,
        y - vertical_offset,
        str); // Draw the text
    canvas_draw_icon(canvas, x - horizontal_offset - icon->width, y - icon_v_offset, &I_ButtonUp);
    canvas_invert_color(canvas); // Invert the color of the text and icon
}

void elements_button_top_right(Canvas* canvas, const char* str) {
    const uint8_t button_height = 10;
    const uint8_t vertical_offset = 2;
    const uint8_t horizontal_offset = 2;
    const uint8_t string_width = canvas_string_width(canvas, str);
    const Icon* icon = &I_ButtonUp;
    const uint8_t icon_h_offset = 2;
    const uint8_t icon_width_with_offset = icon->width + icon_h_offset;
    const uint8_t icon_v_offset = icon->height + vertical_offset;
    const uint8_t button_width = string_width + horizontal_offset * 2 + icon_width_with_offset;

    const uint8_t x = canvas_width(canvas);
    const uint8_t y = 0 + button_height;

    canvas_draw_box(canvas, x - button_width, y - button_height, button_width, button_height);
    canvas_draw_line(canvas, x - button_width - 1, y - button_height, x - button_width - 1, y - 1);
    canvas_draw_line(canvas, x - button_width - 2, y - button_height, x - button_width - 2, y - 2);
    canvas_draw_line(canvas, x - button_width - 3, y - button_height, x - button_width - 3, y - 3);

    canvas_invert_color(canvas);
    canvas_draw_str(canvas, x - button_width + horizontal_offset, y - vertical_offset, str);
    canvas_draw_icon_ex(
        canvas, x + horizontal_offset, y - icon_v_offset, &I_ButtonUp, IconRotation180);
    canvas_invert_color(canvas);
}

// Enumerations for submenu and view indices
typedef enum {
    MyAppSubmenuIndexConfigure,
    MyAppSubmenuIndexFlipTheWorld,
    MyAppSubmenuIndexAbout,
} MyAppSubmenuIndex;

typedef enum {
    MyAppViewSubmenu,
    MyAppViewConfigure,
    MyAppViewFlipTheWorld,
    MyAppViewAbout,
} MyAppView;

// Define a struct to hold the application's components
typedef struct {
    ViewDispatcher* view_dispatcher;
    NotificationApp* notifications;
    Submenu* submenu;
    VariableItemList* variable_item_list_config;
    View* view_flip_the_world;
    Widget* widget_about;
} MyApp;

// Define a model struct for your application
typedef struct {
    uint32_t current_station_index;
    uint8_t volume_index;
} MyModel;

// Callback for navigation events

uint32_t my_app_navigation_exit_callback(void* context) {
    UNUSED(context);
    uint8_t buffer[5]; // Create a buffer to hold the TEA5767 register values
    tea5767_sleep(buffer); // Call the tea5767_sleep function, passing the buffer as an argument
    return VIEW_NONE;
}

// Callback for navigating to the submenu
uint32_t my_app_navigation_submenu_callback(void* context) {
    UNUSED(context);
    return MyAppViewSubmenu;
}

// Callback for handling submenu selections
void my_app_submenu_callback(void* context, uint32_t index) {
    MyApp* app = (MyApp*)context;
    switch(index) {
    case MyAppSubmenuIndexConfigure:
        view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewConfigure);
        break;
    case MyAppSubmenuIndexFlipTheWorld:
        view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewFlipTheWorld);
        break;
    case MyAppSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewAbout);
        break;
    default:
        break;
    }
}

bool my_app_view_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->type == InputTypeShort && event->key == InputKeyLeft) {
        tea5767_seekDown();
        current_volume = 0;
        return true; // Event was handled
    } else if(event->type == InputTypeShort && event->key == InputKeyRight) {
        tea5767_seekUp();
        current_volume = 0;
        return true; // Event was handled
    } else if(event->type == InputTypeShort && event->key == InputKeyOk) {
        if(current_volume == 0) {
            tea5767_MuteOn();
            current_volume = 1;
        } else {
            tea5767_MuteOff();
            current_volume = 0;
        }
        return true; // Event was handled
    } else if(event->type == InputTypeShort && event->key == InputKeyUp) {
        // Increment the current station index and loop back if at the end
        current_station_index = (current_station_index + 1) % NUM_STATIONS;
        // Set the new frequency from the stations array
        tea5767_SetFreqMHz(stations[current_station_index].frequency);
        current_volume = 0;
        return true; // Event was handled
    } else if(event->type == InputTypeShort && event->key == InputKeyDown) {
        // Decrement the current station index and loop back if at the beginning
        if(current_station_index == 0) {
            current_station_index = NUM_STATIONS - 1;
        } else {
            current_station_index--;
        }
        // Set the new frequency from the stations array
        tea5767_SetFreqMHz(stations[current_station_index].frequency);
        current_volume = 0;
        return true; // Event was handled
    }
    return false; // Event was not handled
}

// Callback for handling frequency changes
void my_app_frequency_change(VariableItem* item) {
    MyApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->current_station_index = index;

    // Display the selected frequency value as text
    char frequency_display[16];
    snprintf(
        frequency_display,
        sizeof(frequency_display),
        "%.1f MHz",
        (double)stations[(int)index].frequency);
    variable_item_set_current_value_text(item, frequency_display);
}

// Callback for handling volume changes
void my_app_volume_change(VariableItem* item) {
    MyApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(
        item, volume_names[index]); // Display the selected volume as text
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->volume_index = index;
}

// Declare local buffers for text display
char frequency_display[64];
char station_display[256];
char signal_display[64];
char volume_display[32];

// Callback for drawing the view
void my_app_view_draw_callback(Canvas* canvas, void* model) {
    (void)model;

    // Draw strings on the canvas
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 33, 10, "FM Radio");
    canvas_draw_icon(canvas, 83, 0, &I_RadioSmall);

    // Draw button prompts
    canvas_set_font(canvas, FontSecondary);
    elements_button_top_left(canvas, "Pre");
    elements_button_top_right(canvas, "Pre");
    elements_button_left(canvas, "Scan-");
    elements_button_center(canvas, "Mute");
    elements_button_right(canvas, "Scan+");

    struct RADIO_INFO info; // Create a struct to hold the radio info
    uint8_t buffer[5]; // Create a buffer to hold the TEA5767 register values
    if(tea5767_get_radio_info(buffer, &info)) {
        snprintf(
            frequency_display,
            sizeof(frequency_display),
            "Frequency: %.1f MHz",
            (double)info.frequency);
        float tolerance = 0.05f;
        float diff = stations[current_station_index].frequency - info.frequency;
        if(fabsf(diff) < tolerance) {
            snprintf(
                station_display,
                sizeof(station_display),
                "%s",
                stations[current_station_index].name);
        } else {
            station_display[0] = '\0';
        }
        snprintf(
            volume_display,
            sizeof(volume_display),
            "Status: %s %s",
            info.muted ? "Playing" : "Muted",
            info.stereo ? "(Mono)" : "(Stereo)");
        snprintf(
            signal_display,
            sizeof(signal_display),
            "Signal: %d (%s)",
            info.signalLevel,
            info.signalQuality);
    } else {
        // Display error message if TEA5767 is not detected
        snprintf(frequency_display, sizeof(frequency_display), "TEA5767 Not Detected");
        snprintf(signal_display, sizeof(signal_display), "Pin 15 = SDA | Pin 16 = SLC");
        // Reset frequency_display and volume_display to blank
        station_display[0] = '\0';
        volume_display[0] = '\0';
    }

    // Display the stored values on the 5th loop
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 10, 20, station_display);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 10, 30, frequency_display);
    canvas_draw_str(canvas, 10, 40, volume_display);
    canvas_draw_str(canvas, 10, 49, signal_display);
}

// Allocate memory for the application
MyApp* my_app_alloc() {
    MyApp* app = (MyApp*)malloc(sizeof(MyApp));
    Gui* gui = furi_record_open(RECORD_GUI);
    // Initialize the view dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    // Initialize the submenu
    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Listen Now", MyAppSubmenuIndexFlipTheWorld, my_app_submenu_callback, app);
    //submenu_add_item(app->submenu, "Config", MyAppSubmenuIndexConfigure, my_app_submenu_callback, app);
    submenu_add_item(app->submenu, "About", MyAppSubmenuIndexAbout, my_app_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), my_app_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, MyAppViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, MyAppViewSubmenu);

    // Initialize the variable item list for configuration
    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);

    // Add frequency configuration
    VariableItem* frequency_item = variable_item_list_add(
        app->variable_item_list_config, "Freq (MHz)", NUM_STATIONS, my_app_frequency_change, app);

    uint32_t current_station_index = 0;
    variable_item_set_current_value_index(frequency_item, current_station_index);
    // Add volume configuration
    VariableItem* volume_item = variable_item_list_add(
        app->variable_item_list_config,
        "Volume",
        COUNT_OF(volume_values),
        my_app_volume_change,
        app);
    uint8_t volume_index = 0;
    variable_item_set_current_value_index(volume_item, volume_index);
    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        my_app_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        MyAppViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));

    // Initialize the view for flipping the world
    app->view_flip_the_world = view_alloc();
    view_set_draw_callback(app->view_flip_the_world, my_app_view_draw_callback);
    view_set_input_callback(app->view_flip_the_world, my_app_view_input_callback);
    view_set_previous_callback(app->view_flip_the_world, my_app_navigation_submenu_callback);
    view_allocate_model(app->view_flip_the_world, ViewModelTypeLockFree, sizeof(MyModel));
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->current_station_index = current_station_index;
    model->volume_index = volume_index;
    view_dispatcher_add_view(
        app->view_dispatcher, MyAppViewFlipTheWorld, app->view_flip_the_world);

    // Initialize the widget for displaying information about the app
    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "FM Radio (v1.1)\n"
        "---\n"
        "Created By Coolshrimp\n\n"
        "Up = Preset Up\n"
        "Down = Preset Down\n"
        "Left = Seek Down\n"
        "Right = Seek Up\n"
        "OK = Toggle Mute");

    view_set_previous_callback(
        widget_get_view(app->widget_about), my_app_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, MyAppViewAbout, widget_get_view(app->widget_about));
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif
    return app;
}
// Free memory used by the application
void my_app_free(MyApp* app) {
#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);
    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewFlipTheWorld);
    view_free(app->view_flip_the_world);
    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, MyAppViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);
    free(app);
}
// Main function to start the application
int32_t my_fm_radio(void* p) {
    UNUSED(p);
    MyApp* app = my_app_alloc();
    view_dispatcher_run(app->view_dispatcher);
    my_app_free(app);
    return 0;
}
