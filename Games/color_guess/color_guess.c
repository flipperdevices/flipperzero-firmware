#include "color_guess.h"
//#include "digits.h"
//#include <furi.h>
//#include <gui/gui.h>
//#include <input/input.h>
//#include <lib/toolbox/path.h>
//#include <lib/subghz/protocols/protocol_items.h>
//#include "scenes/color_guess_scene.h"
//#include "scenes/color_guess_scene.c"

//#include <stdlib.h>
//#include <notification/notification_messages.h>
//#include "color_guess_icons.h"
//#include "digits.h"

/*static void render_callback(Canvas* const canvas, void* ctx) {
    const PluginState* plugin_state = acquire_mutex((ValueMutex*)ctx, 25);
    const int cursorOffset = 30;
    const int newCursorPos = (plugin_state->cursorpos * 12) + cursorOffset;
    //const Icon* icon = &I_ButtonUp_7x4;
    if (plugin_state == NULL) {
        return;
    }
    //Border around the edge of the screen
    // canvas_draw_frame(canvas, 0, 0, 128, 64);

    // canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_icon(canvas, newCursorPos, 18, &I_ButtonUp_10x5);
    canvas_draw_icon(canvas, newCursorPos, 41, &I_ButtonDown_10x5);
    canvas_draw_icon(canvas, 18, 25, digits[16]);
    canvas_draw_icon(canvas, 30, 25, digits[plugin_state->digit[0]]);
    canvas_draw_icon(canvas, 42, 25, digits[plugin_state->digit[1]]);
    canvas_draw_icon(canvas, 54, 25, digits[plugin_state->digit[2]]);
    canvas_draw_icon(canvas, 66, 25, digits[plugin_state->digit[3]]);
    canvas_draw_icon(canvas, 78, 25, digits[plugin_state->digit[4]]);
    canvas_draw_icon(canvas, 90, 25, digits[plugin_state->digit[5]]);
    release_mutex((ValueMutex*)ctx, plugin_state);
}*/

/*static void input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}*/

/*static void color_guess_state_init(PluginState* const plugin_state) {
    plugin_state->scene = 0;
    plugin_state->cursorpos = 0;
    plugin_state->red = 0;
    for (int i = 0;i < 6; i++) {
        plugin_state->digit[i] = 0;
    }
    plugin_state->green = 0;
    plugin_state->blue = 0;
}*/

bool color_guess_custom_event_callback(void* context, uint32_t event) {
    furi_assert(context);
    ColorGuess* app = context;
    return scene_manager_handle_custom_event(app->scene_manager, event);
}

void color_guess_tick_event_callback(void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    scene_manager_handle_tick_event(app->scene_manager);
}

//leave app if back button pressed
bool color_guess_navigation_event_callback(void* context) {
    furi_assert(context);
    ColorGuess* app = context;
    return scene_manager_handle_back_event(app->scene_manager);
}

ColorGuess* color_guess_app_alloc() {
    ColorGuess* app = malloc(sizeof(ColorGuess));
    //app->event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));
    app->gui = furi_record_open(RECORD_GUI);
    app->notification = furi_record_open(RECORD_NOTIFICATION);
    //app->plugin_state = malloc(sizeof(PluginState));
    //app->view_port = view_port_alloc();
    app->error = false;

    //Scene additions
    app->view_dispatcher = view_dispatcher_alloc();
    view_dispatcher_enable_queue(app->view_dispatcher);

    app->scene_manager = scene_manager_alloc(&color_guess_scene_handlers, app);
    view_dispatcher_set_event_callback_context(app->view_dispatcher, app);
    view_dispatcher_set_navigation_event_callback(app->view_dispatcher, color_guess_navigation_event_callback);
    view_dispatcher_set_tick_event_callback(app->view_dispatcher, color_guess_tick_event_callback, 100);
    view_dispatcher_set_custom_event_callback(app->view_dispatcher, color_guess_custom_event_callback);
    app->submenu = submenu_alloc();
    
    view_dispatcher_add_view(app->view_dispatcher, ColorGuessViewIdMenu, submenu_get_view(app->submenu));
    app->color_guess_color_set = color_guess_color_set_alloc();
    view_dispatcher_add_view(app->view_dispatcher, ColorGuessViewIdColorSet, color_guess_color_set_get_view(app->color_guess_color_set));

    //End Scene Additions

    // Init Values
    //color_guess_state_init(app->plugin_state);

    // Set Mutex Object
    /*if (!init_mutex(&app->state_mutex, app->plugin_state, sizeof(PluginState))) {
        FURI_LOG_E("Color Guess", "cannot create mutex\r\n");
        free(app->plugin_state);
        app->error = true;
        return app;
    }*/

    // Set System Callbacks
    //view_port_input_callback_set(app->view_port, input_callback, app->event_queue);
    //view_port_draw_callback_set(app->view_port, render_callback, &app->state_mutex);
    
    // Open GUI and register view_port
    // gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

void color_guess_app_free(ColorGuess* app) {
    furi_assert(app);
    //furi_message_queue_free(app->event_queue);
    //view_port_enabled_set(app->view_port, false);
    //gui_remove_view_port(app->gui, app->view_port);
    //view_port_free(app->view_port);
    //delete_mutex(&app->state_mutex);

    // Scene manager
    scene_manager_free(app->scene_manager);

    // View Dispatcher
    view_dispatcher_remove_view(app->view_dispatcher, ColorGuessViewIdMenu);
    submenu_free(app->submenu);

    view_dispatcher_free(app->view_dispatcher);

    // GUI
    furi_record_close(RECORD_GUI);

    app->event_queue = NULL;
    app->plugin_state = NULL;
    // app->state_mutex = NULL;
    app->view_port = NULL;
    
    app->gui = NULL;
    //furi_record_close(RECORD_NOTIFICATION);
    app->notification = NULL;

    //Remove whatever is left
    free(app);
}

int32_t color_guess_app(void* p) {
    UNUSED(p);
    ColorGuess* app = color_guess_app_alloc();
    if (app->error) {
        return 255;
    }

    if(!furi_hal_region_is_provisioned()) {
        color_guess_app_free(app);
        return 1;
    }

    // PluginEvent event;
    // bool processing = true;
    // while (processing) {
    //     FuriStatus event_status = furi_message_queue_get(app->event_queue, &event, 100);
    //     app->plugin_state = (PluginState*)acquire_mutex_block(&app->state_mutex);
    //     NotificationMessage notification_led_message_1;
    //     NotificationMessage notification_led_message_2;
    //     NotificationMessage notification_led_message_3;
            
    //     if (event_status == FuriStatusOk) {
    //         //Press events
    //         if (event.type == EventTypeKey) {
    //             bool sendLedMessage = false;
    //             if (event.input.type == InputTypePress) {
    //                 switch (event.input.key) {
    //                     case InputKeyUp:
    //                         sendLedMessage = true;
    //                         if (app->plugin_state->digit[app->plugin_state->cursorpos] == 15) {
    //                              app->plugin_state->digit[app->plugin_state->cursorpos] = 0;
    //                              break;
    //                         }
    //                         app->plugin_state->digit[app->plugin_state->cursorpos]++;
    //                         break;
    //                     case InputKeyDown:
    //                         sendLedMessage = true;
    //                         if (app->plugin_state->digit[app->plugin_state->cursorpos] == 0) {
    //                             app->plugin_state->digit[app->plugin_state->cursorpos] = 15;
    //                             break;
    //                         }
    //                         app->plugin_state->digit[app->plugin_state->cursorpos]--;
    //                         break;
    //                     case InputKeyLeft:
    //                         app->plugin_state->cursorpos--;
    //                         if (app->plugin_state->cursorpos < 0)
    //                         {
    //                             app->plugin_state->cursorpos = 5;
    //                         }
    //                         break;
    //                     case InputKeyRight:
    //                         app->plugin_state->cursorpos++;
    //                         if (app->plugin_state->cursorpos > 5)
    //                         {
    //                             app->plugin_state->cursorpos = 0;
    //                         }
    //                         break;
    //                     case InputKeyOk:
    //                     case InputKeyBack:
    //                         //Exit Plugin
    //                         processing = false;
    //                         notification_message(app->notification, &sequence_reset_red);
    //                         notification_message(app->notification, &sequence_reset_green);
    //                         notification_message(app->notification, &sequence_reset_blue);
    //                         break;
    //                     case InputKeyMAX:
    //                         break;
    //                 }
    //             }
    //             if (sendLedMessage) {
    //                 notification_led_message_1.type = NotificationMessageTypeLedRed;
    //                 notification_led_message_1.data.led.value = (app->plugin_state->digit[0] * 16) + app->plugin_state->digit[1];
    //                 notification_led_message_2.type = NotificationMessageTypeLedGreen;
    //                 notification_led_message_2.data.led.value = (app->plugin_state->digit[2] * 16) + app->plugin_state->digit[3];
    //                 notification_led_message_3.type = NotificationMessageTypeLedBlue;
    //                 notification_led_message_3.data.led.value = (app->plugin_state->digit[4] * 16) + app->plugin_state->digit[5];
    //                 const NotificationSequence notification_sequence = {
    //                     &notification_led_message_1,
    //                     &notification_led_message_2,
    //                     &notification_led_message_3,
    //                     &message_do_not_reset,
    //                     NULL,
    //                 };
    //                 notification_message(app->notification, &notification_sequence);
    //             }
    //         }
    //     } else {
    //         FURI_LOG_D("Colour Guess", "FuriMessageQueue: event timeout");
    //         //Event timeout
    //     }
        
    //     view_port_update(app->view_port);
    //     release_mutex(&app->state_mutex, app->plugin_state);
    // }

    view_dispatcher_attach_to_gui(app->view_dispatcher, app->gui, ViewDispatcherTypeFullscreen);
    //furi_string_set(app->file_path, SUBGHZ_APP_FOLDER);
    scene_manager_next_scene(app->scene_manager, ColorGuessSceneStart);

    furi_hal_power_suppress_charge_enter();

    view_dispatcher_run(app->view_dispatcher);

    furi_hal_power_suppress_charge_exit();

    color_guess_app_free(app);

    return 0;
}



