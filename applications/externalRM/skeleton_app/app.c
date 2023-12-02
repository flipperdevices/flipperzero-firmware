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
#include "skeleton_app_icons.h"

#define TAG "Skeleton"

// Change this to BACKLIGHT_AUTO if you don't want the backlight to be continuously on.
#define BACKLIGHT_ON 1

// Our application menu has 3 items.  You can add more items if you want.
typedef enum {
    SkeletonSubmenuIndexConfigure,
    SkeletonSubmenuIndexGame,
    SkeletonSubmenuIndexAbout,
} SkeletonSubmenuIndex;

// Each view is a screen we show the user.
typedef enum {
    SkeletonViewSubmenu, // The menu when the app starts
    SkeletonViewTextInput, // Input for configuring text settings
    SkeletonViewConfigure, // The configuration screen
    SkeletonViewGame, // The main screen
    SkeletonViewAbout, // The about screen with directions, link to social channel, etc.
} SkeletonView;

typedef enum {
    SkeletonEventIdRedrawScreen = 0, // Custom event to redraw the screen
    SkeletonEventIdOkPressed = 42, // Custom event to process OK button getting pressed down
} SkeletonEventId;

typedef struct {
    ViewDispatcher* view_dispatcher; // Switches between our views
    NotificationApp* notifications; // Used for controlling the backlight
    Submenu* submenu; // The application menu
    TextInput* text_input; // The text input screen
    VariableItemList* variable_item_list_config; // The configuration screen
    View* view_game; // The main screen
    Widget* widget_about; // The about screen

    VariableItem* setting_2_item; // The name setting item (so we can update the text)
    char* temp_buffer; // Temporary buffer for text input
    uint32_t temp_buffer_size; // Size of temporary buffer

    FuriTimer* timer; // Timer for redrawing the screen
} SkeletonApp;

typedef struct {
    uint32_t setting_1_index; // The team color setting index
    FuriString* setting_2_name; // The name setting
    uint8_t x; // The x coordinate
} SkeletonGameModel;

/**
 * @brief      Callback for exiting the application.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to exit the application.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t skeleton_navigation_exit_callback(void* _context) {
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
static uint32_t skeleton_navigation_submenu_callback(void* _context) {
    UNUSED(_context);
    return SkeletonViewSubmenu;
}

/**
 * @brief      Callback for returning to configure screen.
 * @details    This function is called when user press back button.  We return VIEW_NONE to
 *            indicate that we want to navigate to the configure screen.
 * @param      _context  The context - unused
 * @return     next view id
*/
static uint32_t skeleton_navigation_configure_callback(void* _context) {
    UNUSED(_context);
    return SkeletonViewConfigure;
}

/**
 * @brief      Handle submenu item selection.
 * @details    This function is called when user selects an item from the submenu.
 * @param      context  The context - SkeletonApp object.
 * @param      index     The SkeletonSubmenuIndex item that was clicked.
*/
static void skeleton_submenu_callback(void* context, uint32_t index) {
    SkeletonApp* app = (SkeletonApp*)context;
    switch(index) {
    case SkeletonSubmenuIndexConfigure:
        view_dispatcher_switch_to_view(app->view_dispatcher, SkeletonViewConfigure);
        break;
    case SkeletonSubmenuIndexGame:
        view_dispatcher_switch_to_view(app->view_dispatcher, SkeletonViewGame);
        break;
    case SkeletonSubmenuIndexAbout:
        view_dispatcher_switch_to_view(app->view_dispatcher, SkeletonViewAbout);
        break;
    default:
        break;
    }
}

/**
 * Our 1st sample setting is a team color.  We have 3 options: red, green, and blue.
*/
static const char* setting_1_config_label = "Team color";
static uint8_t setting_1_values[] = {1, 2, 4};
static char* setting_1_names[] = {"Red", "Green", "Blue"};
static void skeleton_setting_1_change(VariableItem* item) {
    SkeletonApp* app = variable_item_get_context(item);
    uint8_t index = variable_item_get_current_value_index(item);
    variable_item_set_current_value_text(item, setting_1_names[index]);
    SkeletonGameModel* model = view_get_model(app->view_game);
    model->setting_1_index = index;
}

/**
 * Our 2nd sample setting is a text field.  When the user clicks OK on the configuration 
 * setting we use a text input screen to allow the user to enter a name.  This function is
 * called when the user clicks OK on the text input screen.
*/
static const char* setting_2_config_label = "Name";
static const char* setting_2_entry_text = "Enter name";
static const char* setting_2_default_value = "Bob";
static void skeleton_setting_2_text_updated(void* context) {
    SkeletonApp* app = (SkeletonApp*)context;
    bool redraw = true;
    with_view_model(
        app->view_game,
        SkeletonGameModel * model,
        {
            furi_string_set(model->setting_2_name, app->temp_buffer);
            variable_item_set_current_value_text(
                app->setting_2_item, furi_string_get_cstr(model->setting_2_name));
        },
        redraw);
    view_dispatcher_switch_to_view(app->view_dispatcher, SkeletonViewConfigure);
}

/**
 * @brief      Callback when item in configuration screen is clicked.
 * @details    This function is called when user clicks OK on an item in the configuration screen.
 *            If the item clicked is our text field then we switch to the text input screen.
 * @param      context  The context - SkeletonApp object.
 * @param      index - The index of the item that was clicked.
*/
static void skeleton_setting_item_clicked(void* context, uint32_t index) {
    SkeletonApp* app = (SkeletonApp*)context;
    index++; // The index starts at zero, but we want to start at 1.

    // Our configuration UI has the 2nd item as a text field.
    if(index == 2) {
        // Header to display on the text input screen.
        text_input_set_header_text(app->text_input, setting_2_entry_text);

        // Copy the current name into the temporary buffer.
        bool redraw = false;
        with_view_model(
            app->view_game,
            SkeletonGameModel * model,
            {
                strncpy(
                    app->temp_buffer,
                    furi_string_get_cstr(model->setting_2_name),
                    app->temp_buffer_size);
            },
            redraw);

        // Configure the text input.  When user enters text and clicks OK, skeleton_setting_text_updated be called.
        bool clear_previous_text = false;
        text_input_set_result_callback(
            app->text_input,
            skeleton_setting_2_text_updated,
            app,
            app->temp_buffer,
            app->temp_buffer_size,
            clear_previous_text);

        // Pressing the BACK button will reload the configure screen.
        view_set_previous_callback(
            text_input_get_view(app->text_input), skeleton_navigation_configure_callback);

        // Show text input dialog.
        view_dispatcher_switch_to_view(app->view_dispatcher, SkeletonViewTextInput);
    }
}

/**
 * @brief      Callback for drawing the game screen.
 * @details    This function is called when the screen needs to be redrawn, like when the model gets updated.
 * @param      canvas  The canvas to draw on.
 * @param      model   The model - MyModel object.
*/
static void skeleton_view_game_draw_callback(Canvas* canvas, void* model) {
    SkeletonGameModel* my_model = (SkeletonGameModel*)model;
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
 * @param      context  The context - SkeletonApp object.
*/
static void skeleton_view_game_timer_callback(void* context) {
    SkeletonApp* app = (SkeletonApp*)context;
    view_dispatcher_send_custom_event(app->view_dispatcher, SkeletonEventIdRedrawScreen);
}

/**
 * @brief      Callback when the user starts the game screen.
 * @details    This function is called when the user enters the game screen.  We start a timer to
 *           redraw the screen periodically (so the random number is refreshed).
 * @param      context  The context - SkeletonApp object.
*/
static void skeleton_view_game_enter_callback(void* context) {
    uint32_t period = furi_ms_to_ticks(200);
    SkeletonApp* app = (SkeletonApp*)context;
    furi_assert(app->timer == NULL);
    app->timer =
        furi_timer_alloc(skeleton_view_game_timer_callback, FuriTimerTypePeriodic, context);
    furi_timer_start(app->timer, period);
}

/**
 * @brief      Callback when the user exits the game screen.
 * @details    This function is called when the user exits the game screen.  We stop the timer.
 * @param      context  The context - SkeletonApp object.
*/
static void skeleton_view_game_exit_callback(void* context) {
    SkeletonApp* app = (SkeletonApp*)context;
    furi_timer_stop(app->timer);
    furi_timer_free(app->timer);
    app->timer = NULL;
}

/**
 * @brief      Callback for custom events.
 * @details    This function is called when a custom event is sent to the view dispatcher.
 * @param      event    The event id - SkeletonEventId value.
 * @param      context  The context - SkeletonApp object.
*/
static bool skeleton_view_game_custom_event_callback(uint32_t event, void* context) {
    SkeletonApp* app = (SkeletonApp*)context;
    switch(event) {
    case SkeletonEventIdRedrawScreen:
        // Redraw screen by passing true to last parameter of with_view_model.
        {
            bool redraw = true;
            with_view_model(
                app->view_game, SkeletonGameModel * _model, { UNUSED(_model); }, redraw);
            return true;
        }
    case SkeletonEventIdOkPressed:
        // Process the OK button.  We play a tone based on the x coordinate.
        if(furi_hal_speaker_acquire(500)) {
            float frequency;
            bool redraw = false;
            with_view_model(
                app->view_game,
                SkeletonGameModel * model,
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
 * @brief      Callback for game screen input.
 * @details    This function is called when the user presses a button while on the game screen.
 * @param      event    The event - InputEvent object.
 * @param      context  The context - SkeletonApp object.
 * @return     true if the event was handled, false otherwise.
*/
static bool skeleton_view_game_input_callback(InputEvent* event, void* context) {
    SkeletonApp* app = (SkeletonApp*)context;
    if(event->type == InputTypeShort) {
        if(event->key == InputKeyLeft) {
            // Left button clicked, reduce x coordinate.
            bool redraw = true;
            with_view_model(
                app->view_game,
                SkeletonGameModel * model,
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
                app->view_game,
                SkeletonGameModel * model,
                {
                    // Should we have some maximum value?
                    model->x++;
                },
                redraw);
        }
    } else if(event->type == InputTypePress) {
        if(event->key == InputKeyOk) {
            // We choose to send a custom event when user presses OK button.  skeleton_custom_event_callback will
            // handle our SkeletonEventIdOkPressed event.  We could have just put the code from
            // skeleton_custom_event_callback here, it's a matter of preference.
            view_dispatcher_send_custom_event(app->view_dispatcher, SkeletonEventIdOkPressed);
            return true;
        }
    }

    return false;
}

/**
 * @brief      Allocate the skeleton application.
 * @details    This function allocates the skeleton application resources.
 * @return     SkeletonApp object.
*/
static SkeletonApp* skeleton_app_alloc() {
    SkeletonApp* app = (SkeletonApp*)malloc(sizeof(SkeletonApp));

    Gui* gui = furi_record_open(RECORD_GUI);

    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);
    view_dispatcher_attach_to_gui(app->view_dispatcher, gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);

    app->submenu = submenu_alloc();
    submenu_add_item(
        app->submenu, "Config", SkeletonSubmenuIndexConfigure, skeleton_submenu_callback, app);
    submenu_add_item(
        app->submenu, "Play", SkeletonSubmenuIndexGame, skeleton_submenu_callback, app);
    submenu_add_item(
        app->submenu, "About", SkeletonSubmenuIndexAbout, skeleton_submenu_callback, app);
    view_set_previous_callback(submenu_get_view(app->submenu), skeleton_navigation_exit_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, SkeletonViewSubmenu, submenu_get_view(app->submenu));
    view_dispatcher_switch_to_view(app->view_dispatcher, SkeletonViewSubmenu);

    app->text_input = text_input_alloc();
    view_dispatcher_add_view(
        app->view_dispatcher, SkeletonViewTextInput, text_input_get_view(app->text_input));
    app->temp_buffer_size = 32;
    app->temp_buffer = (char*)malloc(app->temp_buffer_size);

    app->variable_item_list_config = variable_item_list_alloc();
    variable_item_list_reset(app->variable_item_list_config);
    VariableItem* item = variable_item_list_add(
        app->variable_item_list_config,
        setting_1_config_label,
        COUNT_OF(setting_1_values),
        skeleton_setting_1_change,
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
        app->variable_item_list_config, skeleton_setting_item_clicked, app);

    view_set_previous_callback(
        variable_item_list_get_view(app->variable_item_list_config),
        skeleton_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher,
        SkeletonViewConfigure,
        variable_item_list_get_view(app->variable_item_list_config));

    app->view_game = view_alloc();
    view_set_draw_callback(app->view_game, skeleton_view_game_draw_callback);
    view_set_input_callback(app->view_game, skeleton_view_game_input_callback);
    view_set_previous_callback(app->view_game, skeleton_navigation_submenu_callback);
    view_set_enter_callback(app->view_game, skeleton_view_game_enter_callback);
    view_set_exit_callback(app->view_game, skeleton_view_game_exit_callback);
    view_set_context(app->view_game, app);
    view_set_custom_callback(app->view_game, skeleton_view_game_custom_event_callback);
    view_allocate_model(app->view_game, ViewModelTypeLockFree, sizeof(SkeletonGameModel));
    SkeletonGameModel* model = view_get_model(app->view_game);
    model->setting_1_index = setting_1_index;
    model->setting_2_name = setting_2_name;
    model->x = 0;
    view_dispatcher_add_view(app->view_dispatcher, SkeletonViewGame, app->view_game);

    app->widget_about = widget_alloc();
    widget_add_text_scroll_element(
        app->widget_about,
        0,
        0,
        128,
        64,
        "This is a sample application.\n---\nReplace code and message\nwith your content!\n\nauthor: @codeallnight\nhttps://discord.com/invite/NsjCvqwPAd\nhttps://youtube.com/@MrDerekJamison");
    view_set_previous_callback(
        widget_get_view(app->widget_about), skeleton_navigation_submenu_callback);
    view_dispatcher_add_view(
        app->view_dispatcher, SkeletonViewAbout, widget_get_view(app->widget_about));

    app->notifications = furi_record_open(RECORD_NOTIFICATION);

#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_on);
#endif

    return app;
}

/**
 * @brief      Free the skeleton application.
 * @details    This function frees the skeleton application resources.
 * @param      app  The skeleton application object.
*/
static void skeleton_app_free(SkeletonApp* app) {
#ifdef BACKLIGHT_ON
    notification_message(app->notifications, &sequence_display_backlight_enforce_auto);
#endif
    furi_record_close(RECORD_NOTIFICATION);

    view_dispatcher_remove_view(app->view_dispatcher, SkeletonViewTextInput);
    text_input_free(app->text_input);
    free(app->temp_buffer);
    view_dispatcher_remove_view(app->view_dispatcher, SkeletonViewAbout);
    widget_free(app->widget_about);
    view_dispatcher_remove_view(app->view_dispatcher, SkeletonViewGame);
    view_free(app->view_game);
    view_dispatcher_remove_view(app->view_dispatcher, SkeletonViewConfigure);
    variable_item_list_free(app->variable_item_list_config);
    view_dispatcher_remove_view(app->view_dispatcher, SkeletonViewSubmenu);
    submenu_free(app->submenu);
    view_dispatcher_free(app->view_dispatcher);
    furi_record_close(RECORD_GUI);

    free(app);
}

/**
 * @brief      Main function for skeleton application.
 * @details    This function is the entry point for the skeleton application.  It should be defined in
 *           application.fam as the entry_point setting.
 * @param      _p  Input parameter - unused
 * @return     0 - Success
*/
int32_t main_skeleton_app(void* _p) {
    UNUSED(_p);

    SkeletonApp* app = skeleton_app_alloc();
    view_dispatcher_run(app->view_dispatcher);

    skeleton_app_free(app);
    return 0;
}