

/**
 *
 * @author Coolshrimp - CoolshrimpModz.com
 *
 * @brief FM Radio using the TEA5767 FM radio chip.
 * @version 0.8
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
#include <string.h>
#include <stdlib.h>

#include "TEA5767/TEA5767.h"
#include "fmradio_controller_icons.h"

// Define a macro for enabling the backlight always on.
#define BACKLIGHT_ALWAYS_ON

#define TAG "FMRadio"

// Define volume options and names
uint8_t volume_values[] = {0, 1};
char* volume_names[] = {"Un-Muted", "Muted"};
bool current_volume = 0;  // Muted or not
char* current_vol = "Un-Muted";  // Current volume status as text. Changed type to char*
int* signal_strength;
int loopcount = 0;

uint8_t tea5767_registers[5];

// Define values for frequency selection
float frequency_values[] = {
    88.1, 88.9, 89.1, 90.3, 91.5, 91.7, 92.0, 92.5, 94.1, 95.9, 96.3, 96.9,
    97.3, 98.1, 98.7, 99.1, 99.9, 100.7, 101.3, 103.9, 104.5, 105.1, 105.5, 106.5,
    107.1, 102.7, 105.3
};

uint32_t current_frequency_index = 0;  // Default to the first frequency

//lib can only do bottom left/right
void elements_button_top_left(Canvas* canvas, const char* str) {
    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 3;
    const uint8_t horizontal_offset = 3;

    // You may need to declare or pass 'button_width' here.
    const uint8_t string_width = canvas_string_width(canvas, str);    

    // 'button_width' should be declared or passed here.
    const uint8_t button_width = string_width + horizontal_offset * 2 + 3;

    const uint8_t x = 0;
    const uint8_t y = 0 + button_height;

    canvas_draw_box(canvas, x, y - button_height, button_width, button_height);
    canvas_draw_line(canvas, x + button_width + 0, y - button_height, x + button_width + 0, y - 1);
    canvas_draw_line(canvas, x + button_width + 1, y - button_height, x + button_width + 1, y - 2);
    canvas_draw_line(canvas, x + button_width + 2, y - button_height, x + button_width + 2, y - 3);

    canvas_invert_color(canvas);
    canvas_draw_str(
        canvas, x + horizontal_offset + 3, y - vertical_offset, str);
    canvas_invert_color(canvas);
}

void elements_button_top_right(Canvas* canvas, const char* str) {
    const uint8_t button_height = 12;
    const uint8_t vertical_offset = 3;
    const uint8_t horizontal_offset = 3;
    // You may need to declare or pass 'button_width' here.
    const uint8_t string_width = canvas_string_width(canvas, str);

    // 'button_width' should be declared or passed here.
    const uint8_t button_width = string_width + horizontal_offset * 2 + 3;

    const uint8_t x = canvas_width(canvas);
    const uint8_t y = 0 + button_height;

    canvas_draw_box(canvas, x - button_width, y - button_height, button_width, button_height);
    canvas_draw_line(canvas, x - button_width - 1, y - button_height, x - button_width - 1, y - 1);
    canvas_draw_line(canvas, x - button_width - 2, y - button_height, x - button_width - 2, y - 2);
    canvas_draw_line(canvas, x - button_width - 3, y - button_height, x - button_width - 3, y - 3);

    canvas_invert_color(canvas);
    canvas_draw_str(canvas, x - button_width + horizontal_offset, y - vertical_offset, str);
    canvas_invert_color(canvas);
}

// Enumerations for submenu and view indices
typedef enum {
    FMRadioSubmenuIndexConfigure,
    FMRadioSubmenuIndexFlipTheWorld,
    FMRadioSubmenuIndexAbout,
} FMRadioSubmenuIndex;

typedef enum {
    FMRadioViewSubmenu,
    FMRadioViewConfigure,
    FMRadioViewFlipTheWorld,
    FMRadioViewAbout,
} FMRadioView;

// Define a struct to hold the application's components
typedef struct {
    ViewDispatcher* view_dispatcher;
    NotificationApp* notifications;
    Submenu* submenu;
    VariableItemList* variable_item_list_config;
    View* view_flip_the_world;
    Widget* widget_about;
} FMRadio;

// Define a model struct for your application
typedef struct {
    uint32_t frequency_index;
    uint8_t volume_index;
} MyModel;

// Callback for navigation events

uint32_t fmradio_controller_navigation_exit_callback(void* context) {
    UNUSED(context);
    uint8_t buffer[5];  // Create a buffer to hold the TEA5767 register values
        tea5767_sleep(buffer);  // Call the tea5767_sleep function, passing the buffer as an argument
    return VIEW_NONE;
}

// Callback for navigating to the submenu
uint32_t fmradio_controller_navigation_submenu_callback(void* context) {
    UNUSED(context);
    return FMRadioViewSubmenu;
}

// Callback for handling submenu selections
void fmradio_controller_submenu_callback(void* context, uint32_t index) {
    FMRadio* app = (FMRadio*)context;
    switch(index) {
    case FMRadioSubmenuIndexConfigure:
        view_dispatcher_switch_to_view(app->view_dispatcher, FMRadioViewConfigure);
        break;
    case FMRadioSubmenuIndexFlipTheWorld:
        view_dispatcher_switch_to_view(app->view_dispatcher, FMRadioViewFlipTheWorld);
        break;
    case FMRadioSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, FMRadioViewAbout);
        break;
    default:
        break;
    }
}

bool fmradio_controller_view_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if (event->type == InputTypeShort && event->key == InputKeyLeft) {
        tea5767_seekDown();
        current_volume = 0;
        current_vol = "Un-Muted";
        return true;  // Event was handled
    } else if (event->type == InputTypeShort && event->key == InputKeyRight) {
        tea5767_seekUp();
        current_volume = 0;
        current_vol = "Un-Muted";
        return true;  // Event was handled
    } else if (event->type == InputTypeShort && event->key == InputKeyOk) {       
        if (current_volume == 0) {  // Fixed: == instead of =
            tea5767_MuteOn();
            current_volume = 1;
            current_vol = "Muted";
        } else {
            tea5767_MuteOff();
            current_volume = 0;
            current_vol = "Un-Muted";
        }
        return true;  // Event was handled
    } else if (event->type == InputTypeShort && event->key == InputKeyUp) {
        // Increment the current frequency index and loop back if at the end
        current_frequency_index = (current_frequency_index + 1) % (sizeof(frequency_values) / sizeof(frequency_values[0]));
        // Set the new frequency
        tea5767_SetFreqMHz(frequency_values[current_frequency_index]);
        current_volume = 0;
        current_vol = "Un-Muted";
        return true;  // Event was handled
    } else if (event->type == InputTypeShort && event->key == InputKeyDown) {
        // Decrement the current frequency index and loop back if at the beginning
        if (current_frequency_index == 0) {
            current_frequency_index = (sizeof(frequency_values) / sizeof(frequency_values[0])) - 1;
        } else {
            current_frequency_index--;
        }
        // Set the new frequency
        tea5767_SetFreqMHz(frequency_values[current_frequency_index]);
        current_volume = 0;
        current_vol = "Un-Muted";
        return true;  // Event was handled
    }
    
    return false;  // Event was not handled
}

// Callback for handling frequency changes
void fmradio_controller_frequency_change(VariableItem* item) {
    FMRadio* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->frequency_index = index;

    // Display the selected frequency value as text
    char frequency_display[16];  // Adjust the buffer size as needed
    snprintf(frequency_display, sizeof(frequency_display), "%.1f MHz", (double)frequency_values[index]);
    variable_item_set_current_value_text(item, frequency_display);
}

// Callback for handling volume changes
void fmradio_controller_volume_change(VariableItem* item) {
    FMRadio* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, volume_names[index]);  // Display the selected volume as text
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->volume_index = index;
}

// Callback for drawing the view

void fmradio_controller_view_draw_callback(Canvas* canvas, void* model) {
    (void)model;  // Mark model as unused
    
    char frequency_display[64];    
    char signal_display[64];
    char volume_display[32]; 
    
    // tea5767_get_radio_info() populates the info
    struct RADIO_INFO info;
    uint8_t buffer[5];

    // Draw strings on the canvas
    canvas_draw_str(canvas, 45, 10, "FM Radio");    

    // Draw button prompts
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Scan-");
    elements_button_right(canvas, "Scan+");
    elements_button_center(canvas, "Mute");
    elements_button_top_left(canvas, " Pre");
    elements_button_top_right(canvas, "Pre ");
    
    
    if (tea5767_get_radio_info(buffer, &info)) { 
        snprintf(frequency_display, sizeof(frequency_display), "Frequency: %.1f MHz", (double)info.frequency);
        canvas_draw_str(canvas, 10, 25, frequency_display);

        snprintf(signal_display, sizeof(signal_display), "RSSI: %d (%s) %d", info.signalLevel, info.signalQuality, loopcount);
        canvas_draw_str(canvas, 10, 45, signal_display); 

        snprintf(volume_display, sizeof(volume_display), "Status: %s %s", info.muted ? "Playing" : "Muted", info.stereo ? "(Stereo)" : "(Mono)");
        canvas_draw_str(canvas, 10, 35, volume_display);              
    } else {
        snprintf(frequency_display, sizeof(frequency_display), "TEA5767 Not Detected");
        canvas_draw_str(canvas, 10, 25, frequency_display); 

        snprintf(signal_display, sizeof(signal_display), "Pin 15 = SDA | Pin 16 = SLC");
        canvas_draw_str(canvas, 10, 45, signal_display); 
    }   

}

// Allocate memory for the application
FMRadio* fmradio_controller_alloc() {
    FMRadio* app = (FMRadio*)malloc(sizeof(FMRadio));

    Gui* gui = furi_record_open(RECORD_GUI);

    // Initialize the view dispatcher
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);

    // Initialize the submenu
    app->submenu = submenu_alloc();
    submenu_add_item(app->submenu,"Listen Now",FMRadioSubmenuIndexFlipTheWorld,fmradio_controller_submenu_callback,app);
    //submenu_add_item(app->submenu, "Config", FMRadioSubmenuIndexConfigure, fmradio_controller_submenu_callback, app);
    submenu_add_item(app->submenu, "About", FMRadioSubmenuIndexAbout, fmradio_controller_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), fmradio_controller_navigation_exit_callback);
    view_dispatcher_add_view(app->view_dispatcher, FMRadioViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, FMRadioViewSubmenu);

    // Initialize the variable item list for configuration
    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);

    // Add frequency configuration
    VariableItem* frequency_item = variable_item_list_add(app->variable_item_list_config,"Freq (MHz)", COUNT_OF(frequency_values),fmradio_controller_frequency_change,app); 
    uint32_t frequency_index = 0;
    variable_item_set_current_value_index(frequency_item, frequency_index);

    // Add volume configuration
    VariableItem* volume_item = variable_item_list_add(app->variable_item_list_config,"Volume", COUNT_OF(volume_values),fmradio_controller_volume_change,app);
    uint8_t volume_index = 0;
    variable_item_set_current_value_index(volume_item, volume_index);
    view_set_previous_callback(variable_item_list_get_view(app->variable_item_list_config),fmradio_controller_navigation_submenu_callback);
    view_dispatcher_add_view(app->view_dispatcher,FMRadioViewConfigure,variable_item_list_get_view(app->variable_item_list_config));

    // Initialize the view for flipping the world
    app->view_flip_the_world = view_alloc();
    view_set_draw_callback(app->view_flip_the_world, fmradio_controller_view_draw_callback);
    view_set_input_callback(app->view_flip_the_world, fmradio_controller_view_input_callback);
    view_set_previous_callback(app->view_flip_the_world, fmradio_controller_navigation_submenu_callback);
    view_allocate_model(app->view_flip_the_world, ViewModelTypeLockFree, sizeof(MyModel));
    MyModel* model = view_get_model(app->view_flip_the_world);
    model->frequency_index = frequency_index;
    model->volume_index = volume_index;

    view_dispatcher_add_view(app->view_dispatcher, FMRadioViewFlipTheWorld, app->view_flip_the_world);

    // Initialize the widget for displaying information about the app
    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(app->widget_about,0,0,128,64,"FM Radio. (v0.8)\n---\n Created By Coolshrimp\n\nUp = Preset Up\nDown = Preset Down\nLeft = Seek Down \nRight = Seek Up \n OK = Toggle Mute");
    view_set_previous_callback(widget_get_view(app->widget_about), fmradio_controller_navigation_submenu_callback);
    view_dispatcher_add_view(app->view_dispatcher, FMRadioViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif

    return app;
}

// Free memory used by the application
void fmradio_controller_free(FMRadio* app) {
#ifdef BACKLIGHT_ALWAYS_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, FMRadioViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, FMRadioViewFlipTheWorld);
    view_free(app->view_flip_the_world);
    view_dispatcher_remove_view(app->view_dispatcher, FMRadioViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, FMRadioViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

// Main function to start the application
int32_t fmradio_controller_app(void* p) {
    UNUSED(p);

    FMRadio* app = fmradio_controller_alloc();
    view_dispatcher_run(app->view_dispatcher);

    fmradio_controller_free(app);
    return 0;
}
