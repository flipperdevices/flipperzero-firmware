#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/modules/submenu.h>
#include <gui/modules/text_input.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "subghz_remote.h"

#define TAG "SubGhz Remote"

// Change this to BACKLIGHT_AUTO if you don't want the backlight to be continuously on.
#define BACKLIGHT_Auto

// Our application menu has 3 items.  You can add more items if you want.
typedef enum {
    SubGhzRemoteSubmenuIndexPlay,
    SubGhzRemoteSubmenuIndexConfigure,
    SubGhzRemoteSubmenuIndexAbout,
} SubGhzRemoteSubmenuIndex;

// Each view is a screen we show the user.
typedef enum {
    SubGhzRemoteViewSubmenu, // The menu when the app starts
    SubGhzRemoteViewTextInput, // Input for configuring text settings
    SubGhzRemoteViewConfigure, // The configuration screen
    SubGhzRemoteViewPlay, // The main screen
    SubGhzRemoteViewAbout, // The about screen with directions, link to social channel, etc.
} SubGhzRemoteView;

/**Not use for the moment.*/
typedef enum {
    SubGhzRemoteEventIdRedrawScreen = 0, // Custom event to redraw the screen
    SubGhzRemoteEventIdOkPressed = 42, // Custom event to process OK button getting pressed down
} SubGhzRemoteEventId;

typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    NotificationApp* notifications; // Used for controlling the backlight
    Submenu* submenu; // The application menu
    TextInput* text_input; // The text input screen
    VariableItemList* variable_item_list_config; // The configuration screen
    View* view_play; // The main screen
    Widget* widget_about; // The about screen

    VariableItem* setting_2_item; // The name setting item (so we can update the text)
    char* temp_buffer; // Temporary buffer for text input
    uint32_t temp_buffer_size; // Size of temporary buffer

    FuriTimer* timer; // Timer for redrawing the screen
} SubGhzRemoteApp;

typedef struct {
    uint32_t setting_1_index; // The Remote name setting index
    FuriString* setting_2_name; // The name setting
    uint8_t x; // The x coordinate
} SubGhzRemotePlayModel;

/**
 * @brief      Callback for exiting the application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t subghzremote_navigation_exit_callback(void* _context) {
    UNUSED(_context);
    return VIEW_NONE;
}

/**
 * @brief      Callback for returning to submenu.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to navigate to the submenu.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t subghzremote_navigation_submenu_callback(void* _context) {
    UNUSED(_context);
    return SubGhzRemoteViewSubmenu;
}

/**
 * @brief      Callback for returning to configure screen.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to navigate to the configure screen.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t subghzremote_navigation_configure_callback(void* _context) {
    UNUSED(_context);
    return SubGhzRemoteViewConfigure;
}

/**
 * @brief      Handle submenu item selection.
 * @details    This function is called when user selects an item from the submenu.
 * @param      context  The context - SubGhzRemoteApp object.
 * @param      index     The SubGhzRemoteSubmenuIndex item that was clicked.
*/
static void subghzremote_submenu_callback(void* context, uint32_t index) {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    switch(index) {
    case SubGhzRemoteSubmenuIndexPlay:
        view_dispatcher_switch_to_view(app->view_dispatcher, SubGhzRemoteViewPlay);
        break;
    case SubGhzRemoteSubmenuIndexConfigure:
        view_dispatcher_switch_to_view(app->view_dispatcher, SubGhzRemoteViewConfigure);
        break;
    case SubGhzRemoteSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, SubGhzRemoteViewAbout);
        break;
    default:
        break;
    }
}

/**
 * Setting the Remote Number.  We have 4 options: A, B, C, D.
*/
static const char* setting_1_config_label = "Remote Number";
static uint8_t setting_1_values[] = {1, 2, 3, 4};
static char* setting_1_names[] = {"A", "B", "C", "D"};
static void subghzremote_setting_1_change(VariableItem* item) {
    SubGhzRemoteApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_1_names[index]);
    SubGhzRemotePlayModel* model = view_get_model(app->view_play);
    model->setting_1_index = index;
}

/**
 * Our 2nd sample setting is a text field.  When the user clicks OK on the configuration 
 * setting we use a text input screen to allow the user to enter a name.  This function is
 * called when the user clicks OK on the text input screen.
*/
static const char* setting_2_config_label = "Category";
static const char* setting_2_entry_text = "Enter name of Category";
static const char* setting_2_default_value = "Remote ";
static void subghzremote_setting_2_text_updated(void* context) {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    bool redraw = true;
    with_view_model(
        app->view_play,
        SubGhzRemotePlayModel * model,
        {
            furi_string_set(model->setting_2_name, app->temp_buffer);
            variable_item_set_current_value_text(
                app->setting_2_item, furi_string_get_cstr(model->setting_2_name));
        },
        redraw);
    view_dispatcher_switch_to_view(app->view_dispatcher, SubGhzRemoteViewConfigure);
}

/**
 * @brief      Callback when item in configuration screen is clicked.
 * @details    This function is called when user clicks OK on an item in the configuration screen.
 *            If the item clicked is our text field then we switch to the text input screen.
 * @param      context  The context - SubGhzRemoteApp object.
 * @param      index - The index of the item that was clicked.
*/
static void subghzremote_setting_item_clicked(void* context, uint32_t index) {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    index++; // The index starts at zero, but we want to start at 1.

    // Our configuration UI has the 2nd item as a text field.
    if(index == 2) {
        // Header to display on the text input screen.
        text_input_set_header_text(app->text_input, setting_2_entry_text);

        // Copy the current name into the temporary buffer.
        bool redraw = false;
        with_view_model(
            app->view_play,
            SubGhzRemotePlayModel * model,
            {
                strncpy(
                    app->temp_buffer,
                    furi_string_get_cstr(model->setting_2_name),
                    app->temp_buffer_size);
            },
            redraw);

        // Configure the text input.  When user enters text and clicks OK, subghzremote_setting_text_updated be called.
        bool clear_previous_text = false;
        text_input_set_result_callback(
            app->text_input,
            subghzremote_setting_2_text_updated,
            app,
            app->temp_buffer,
            app->temp_buffer_size,
            clear_previous_text);

        // Pressing the BACK button will reload the configure screen.
        view_set_previous_callback(
            text_input_get_view(app->text_input), subghzremote_navigation_configure_callback);

        // Show text input dialog.
        view_dispatcher_switch_to_view(app->view_dispatcher, SubGhzRemoteViewTextInput);
    }
}

/**
 * @brief      Callback for drawing the play screen.
 * @details    This function is called when the screen needs to be redrawn, like when the model gets updated.
 * @param      canvas  The canvas to draw on.
 * @param      model   The model - MyModel object.
*/
static void subghzremote_view_play_draw_callback(Canvas* canvas, void* model) {
    SubGhzRemotePlayModel* my_model = (SubGhzRemotePlayModel*)model;
    canvas_draw_icon(canvas, my_model->x, 20, &I_glyph_1_14x40);
    canvas_draw_str(canvas, 1, 10, "LEFT/RIGHT to change x");
    FuriString* xstr = furi_string_alloc();
    furi_string_printf(xstr, "x: %u  OK=play tone", my_model->x);
    canvas_draw_str(canvas, 44, 24, furi_string_get_cstr(xstr));
    furi_string_printf(xstr, "random: %u", (uint8_t)(furi_hal_random_get() % 256));
    canvas_draw_str(canvas, 44, 36, furi_string_get_cstr(xstr));
    furi_string_printf(
        xstr,
        "team: %s (%u)",
        setting_1_names[my_model->setting_1_index],
        setting_1_values[my_model->setting_1_index]);
    canvas_draw_str(canvas, 44, 48, furi_string_get_cstr(xstr));
    furi_string_printf(xstr, "name: %s", furi_string_get_cstr(my_model->setting_2_name));
    canvas_draw_str(canvas, 44, 60, furi_string_get_cstr(xstr));
    furi_string_free(xstr);
}

/**
 * @brief      Callback for timer elapsed.
 * @details    This function is called when the timer is elapsed.  We use this to queue a redraw event.
 * @param      context  The context - SubGhzRemoteApp object.
*/
static void subghzremote_view_play_timer_callback(void* context) {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SubGhzRemoteEventIdRedrawScreen);
}

/**
 * @brief      Callback when the user starts the play screen.
 * @details    This function is called when the user enters the play screen.  We start a timer to
 *           redraw the screen periodically (so the random number is refreshed).
 * @param      context  The context - SubGhzRemoteApp object.
*/
static void subghzremote_view_play_enter_callback(void* context) {
    uint32_t period = furi_ms_to_ticks(200);
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(subghzremote_view_play_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
}

/**
 * @brief      Callback when the user exits the play screen.
 * @details    This function is called when the user exits the play screen.  We stop the timer.
 * @param      context  The context - SubGhzRemoteApp object.
*/
static void subghzremote_view_play_exit_callback(void* context) {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Callback for custom events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      event    The event id - SubGhzRemoteEventId value.
 * @param      context  The context - SubGhzRemoteApp object.
*/
static bool subghzremote_view_play_custom_event_callback(uint32_t event, void* context) {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    switch(event) {
    case SubGhzRemoteEventIdRedrawScreen:
        // Redraw screen by passing true to last parameter of with_view_model.
        {
            bool redraw = true;
            with_view_model(
                app->view_play, SubGhzRemotePlayModel * _model, { UNUSED(_model); }, redraw);
            return true;
        }
    case SubGhzRemoteEventIdOkPressed:
        // Process the OK button.  We play a tone based on the x coordinate.
        if(furi_hal_speaker_acquire(500)) {
            float frequency;
            bool redraw = false;
            with_view_model(
                app->view_play,
                SubGhzRemotePlayModel * model,
                { frequency = model->x * 100 + 100; },
                redraw);
            furi_hal_speaker_start(frequency, 1.0);
            furi_delay_ms(100);
            furi_hal_speaker_stop();
            furi_hal_speaker_release();
        }
        return true;
    default:
        return false;
    }
}
/**
 * @brief      Callback for play screen input.
 * @details    This function is called when the user presses a button while on the play screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - SubGhzRemoteApp object.
 * @return     true if the event was handled, false otherwise.
*/
static bool subghzremote_view_play_input_callback(InputEvent* event, void* context) {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)context;
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            // Left button clicked, reduce x coordinate.
            bool redraw = true;
            with_view_model(
                app->view_play,
                SubGhzRemotePlayModel * model,
                {
                    if(model->x > 0) {
                        model->x--;
                    }
                },
                redraw);
        } else if(event->key == InputKeyRight) {
            // Right button clicked, increase x coordinate.
            bool redraw = true;
            with_view_model(
                app->view_play,
                SubGhzRemotePlayModel * model,
                {
                    // Should we have some maximum value?
                    model->x++;
                },
                redraw);
        }
    } else if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {
            // We choose to send a custom event when user presses OK button.  subghzremote_view_play_custom_event_callback will
            // handle our SubGhzRemoteEventIdOkPressed event.  We could have just put the code from
            // subghzremote_view_play_custom_event_callback here, it's a matter of preference.
            view_dispatcher_send_custom_event(app->view_dispatcher, SubGhzRemoteEventIdOkPressed);
            return true;
        }
    }

    return false;
}

/**
 * @brief      Allocate the subghzremote application.
 * @details    This function allocates the subghzremote application resources.
 * @return     SubGhzRemoteApp object.
*/
static SubGhzRemoteApp* subghzremote_app_alloc() {
    SubGhzRemoteApp* app = (SubGhzRemoteApp*)malloc(sizeof(SubGhzRemoteApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Play", SubGhzRemoteSubmenuIndexPlay, subghzremote_submenu_callback, app);
    submenu_add_item(
        app->submenu,
        "Config",
        SubGhzRemoteSubmenuIndexConfigure,
        subghzremote_submenu_callback,
        app);
    submenu_add_item(
        app->submenu, "About", SubGhzRemoteSubmenuIndexAbout, subghzremote_submenu_callback, app);
    view_set_previous_callback(
        submenu_get_view(app->submenu), subghzremote_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, SubGhzRemoteViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, SubGhzRemoteViewSubmenu);

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, SubGhzRemoteViewTextInput, text_input_get_view(app->text_input));
    app->temp_buffer_size = 32;
    app->temp_buffer = (char*)malloc(app->temp_buffer_size);

    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);
    VariableItem* item = variable_item_list_add(
        app->variable_item_list_config,
        setting_1_config_label,
        COUNT_OF(setting_1_values),
        subghzremote_setting_1_change,
        app);
    uint8_t setting_1_index = 0;
    variable_item_set_current_value_index(item, setting_1_index);
    variable_item_set_current_value_text(item, setting_1_names[setting_1_index]);

    FuriString* setting_2_name = furi_string_alloc();
    furi_string_set_str(setting_2_name, setting_2_default_value);
    app->setting_2_item = variable_item_list_add(
        app->variable_item_list_config, setting_2_config_label, 1, NULL, NULL);
    variable_item_set_current_value_text(
        app->setting_2_item, furi_string_get_cstr(setting_2_name));
    variable_item_list_set_enter_callback(
        app->variable_item_list_config, subghzremote_setting_item_clicked, app);

    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        subghzremote_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        SubGhzRemoteViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));

    app->view_play = view_alloc();
    view_set_draw_callback(app->view_play, subghzremote_view_play_draw_callback);
    view_set_input_callback(app->view_play, subghzremote_view_play_input_callback);
    view_set_previous_callback(app->view_play, subghzremote_navigation_submenu_callback);
    view_set_enter_callback(app->view_play, subghzremote_view_play_enter_callback);
    view_set_exit_callback(app->view_play, subghzremote_view_play_exit_callback);
    view_set_context(app->view_play, app);
    view_set_custom_callback(app->view_play, subghzremote_view_play_custom_event_callback);
    view_allocate_model(app->view_play, ViewModelTypeLockFree, sizeof(SubGhzRemotePlayModel));
    SubGhzRemotePlayModel* model = view_get_model(app->view_play);
    model->setting_1_index = setting_1_index;
    model->setting_2_name = setting_2_name;
    model->x = 0;
    view_dispatcher_add_view(app->view_dispatcher, SubGhzRemoteViewPlay, app->view_play);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "This is a sample application.\n---\nReplace code and message\nwith your content!\n\nauthor: @codeallnight\nhttps://discord.com/invite/NsjCvqwPAd\nhttps://youtube.com/@MrDerekJamison");
    view_set_previous_callback(
        widget_get_view(app->widget_about), subghzremote_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, SubGhzRemoteViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif

    return app;
}
/**
 * @brief      Free the subghzremote application.
 * @details    This function frees the subghzremote application resources.
 * @param      app  The subghzremote application object.
*/
static void subghzremote_app_free(SubGhzRemoteApp* app) {
#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, SubGhzRemoteViewTextInput);
    text_input_free(app->text_input);
    free(app->temp_buffer);
    view_dispatcher_remove_view(app->view_dispatcher, SubGhzRemoteViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, SubGhzRemoteViewPlay);
    view_free(app->view_play);
    view_dispatcher_remove_view(app->view_dispatcher, SubGhzRemoteViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, SubGhzRemoteViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}
/**
 * @brief      Main function for subghzremote application.
 * @details    This function is the entry point for the subghzremote application.  It should be defined in
 *           application.fam as the entry_point setting.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t main_subghzremote_app(void* _p) {
    UNUSED(_p);

    SubGhzRemoteApp* app = subghzremote_app_alloc();
    view_dispatcher_run(app->view_dispatcher);

    subghzremote_app_free(app);
    return 0;
}