#include "bt_trigger.h"

__int32_t bt_trigger_app(void* p) {
    //Fake using p to compile
    UNUSED(p);
    AppStruct* app = appStructAlloc();

    // bt_disconnect(app->bt);

    // Wait 2nd core to update nvm storage
    // furi_delay_ms(200);

    bt_keys_storage_set_storage_path(app->bt, HID_BT_KEYS_STORAGE_PATH);

    if(!bt_set_profile(app->bt, BtProfileHidKeyboard)) {
        FURI_LOG_E(TAG, "Failed to switch to HID profile");
    }

    furi_hal_bt_start_advertising();
    bt_set_status_changed_callback(app->bt, bt_hid_connection_status_changed_callback, app);

    dolphin_deed(DolphinDeedPluginStart);

    //An event
    IosTriggerEvent event;
    //List of 8 events
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(IosTriggerEvent));
    //A timer
    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);

    //Callback for the display
    view_port_draw_callback_set(app->view_port, draw_callback, app);
    //Callback for the inputs passing the list as param
    view_port_input_callback_set(app->view_port, input_callback, event_queue);

    //Linking the drawin on the display
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    //Main loop
    while(app->running) {
        //Geting new event from the envent list in the event variable
        //waiting forever if the list is empty
        //checking status as ok
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        //Dealing with events one by one
        switch(event.type) {
        case(EventTypeInput):
            //On ne considère que les appuies courts
            if(event.input.type == InputTypeShort) {
                switch(event.input.key) {
                case(InputKeyBack):
                    //Breaking main loop if the back key is pressed
                    app->shooting = false;
                    app->running = false;
                    break;
                case(InputKeyOk): //Take a shot and start intervalometer
                    if(app->delay > 0) {
                        app->shooting = !app->shooting;
                        if(app->shooting) {
                            furi_hal_bt_hid_consumer_key_press(HID_CONSUMER_VOLUME_INCREMENT);
                            furi_hal_bt_hid_consumer_key_release(HID_CONSUMER_VOLUME_INCREMENT);
                            notification_message(app->notifications, &sequence_blink_blue_100);
                            app->shots++;
                            //Timer triggered every delay ms
                            furi_timer_start(timer, app->delay * 1000);
                        } else {
                            //Timer triggered every delay ms
                            furi_timer_stop(timer);
                        }
                    }
                    break;
                case(InputKeyUp): //Increase delay
                    if(!app->shooting) {
                        app->delay++;
                    }
                    break;
                case(InputKeyDown): //Decrease delay
                    if(!app->shooting && app->delay > 1) {
                        app->delay--;
                    }
                    break;
                case(InputKeyLeft): //Reset shots counter
                    if(!app->shooting) {
                        app->shots = 0;
                    }
                    break;
                case(InputKeyRight): //Take a shot
                    if(!app->shooting) {
                        furi_hal_bt_hid_consumer_key_press(HID_CONSUMER_VOLUME_INCREMENT);
                        furi_hal_bt_hid_consumer_key_release(HID_CONSUMER_VOLUME_INCREMENT);
                        notification_message(app->notifications, &sequence_blink_blue_100);
                        app->shots++;
                    }
                    break;
                default:
                    break;
                }
            }
            view_port_update(app->view_port);
            break;
        case(EventTypeTick):
            if(app->shooting) {
                //sending command to trigger via BT
                furi_hal_bt_hid_consumer_key_press(HID_CONSUMER_VOLUME_INCREMENT);
                furi_hal_bt_hid_consumer_key_release(HID_CONSUMER_VOLUME_INCREMENT);
                notification_message(app->notifications, &sequence_blink_blue_100);
                app->shots++;
            }
            view_port_update(app->view_port);
            break;
        default:
            break;
        }
    }

    //Going back to serial mode BT
    bt_set_status_changed_callback(app->bt, NULL, NULL);
    bt_disconnect(app->bt);
    // Wait 2nd core to update nvm storage
    furi_delay_ms(200);
    bt_keys_storage_set_default_path(app->bt);
    if(!bt_set_profile(app->bt, BtProfileSerial)) {
        FURI_LOG_E(TAG, "Failed to switch to Serial profile");
    }

    //Freeing memory
    furi_message_queue_free(event_queue);
    //Freeing timer
    furi_timer_free(timer);
    cleanUpBeforeYouLeave(app);

    return 0;
}

//Callback display
static void draw_callback(Canvas* canvas, void* ctx) {
    AppStruct* app = ctx;
    char chaine_photo[36];
    char chaine_delais[36];
    char chaine_shooting[36];

    snprintf(chaine_photo, sizeof(chaine_photo), "%i shots", app->shots);
    snprintf(chaine_delais, sizeof(chaine_delais), "%i", app->delay);
    if(app->shooting) {
        snprintf(chaine_shooting, sizeof(chaine_shooting), "Press to stop");
    } else {
        snprintf(chaine_shooting, sizeof(chaine_shooting), "Press to start");
    }

    canvas_clear(canvas);
    canvas_draw_frame(canvas, 0, 0, 128, 64);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "iOS Intervalometer");
    //Represent
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 92, 62, "Nem0oo");
    if(app->connected) {
        canvas_draw_icon(canvas, 111, 2, &I_Ble_connected_15x15);

        canvas_set_font(canvas, FontSecondary);
        //Delay line
        canvas_draw_icon(canvas, 3, 19, &I_ButtonDown_7x4);
        canvas_draw_icon(canvas, 3, 14, &I_ButtonUp_7x4);
        canvas_draw_str(canvas, 13, 22, "Delay (in sec)");
        canvas_draw_str(canvas, 71, 22, chaine_delais);
        //Start/stop line
        canvas_draw_icon(canvas, 2, 25, &I_Ok_btn_9x9);
        canvas_draw_str(canvas, 13, 33, chaine_shooting);
        //Single shot line
        canvas_draw_icon(canvas, 6, 36, &I_ButtonRight_4x7);
        canvas_draw_str(canvas, 13, 43, "Single shot");
        //Reset shot count line
        canvas_draw_icon(canvas, 3, 45, &I_ButtonLeft_4x7);
        canvas_draw_str(canvas, 13, 52, "Reset shot count");
        //Shots number line
        canvas_draw_icon(canvas, 2, 53, &I_dir_10px);
        canvas_draw_str(canvas, 14, 62, chaine_photo);
    } else {
        canvas_draw_icon(canvas, 111, 2, &I_Ble_disconnected_15x15);
        canvas_draw_icon(canvas, 1, 21, &I_WarningDolphin_45x42);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 48, 37, "Awaiting bluetooth");
    }
}

//Input callbacks
static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    //Getting our event queue
    FuriMessageQueue* event_queue = ctx;

    //Adding the event to our custom Struct
    IosTriggerEvent event = {.type = EventTypeInput, .input = *input_event};

    //Adding our event to the event queue
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

//Timer callback
static void timer_callback(FuriMessageQueue* event_queue) {
    //check eventqueue is not null
    furi_assert(event_queue);
    //creating event and adding it to the event list
    IosTriggerEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

static void bt_hid_connection_status_changed_callback(BtStatus status, void* context) {
    furi_assert(context);
    AppStruct* app = context;
    app->connected = (status == BtStatusConnected);
    view_port_update(app->view_port);
}

AppStruct* appStructAlloc() {
    AppStruct* app = malloc(sizeof(AppStruct));
    //Init bluetooth
    app->bt = furi_record_open(RECORD_BT);
    //Drawing to be displayed
    app->gui = furi_record_open(RECORD_GUI);
    //Display
    app->view_port = view_port_alloc();
    //Init notifications (used for led blink)
    app->notifications = furi_record_open(RECORD_NOTIFICATION);
    app->connected = false;
    app->running = true;
    app->delay = 1;
    return app;
}

void cleanUpBeforeYouLeave(AppStruct* app) {
    furi_assert(app);
    //Freeing notifications
    furi_record_close(RECORD_NOTIFICATION);
    app->notifications = NULL;
    //Remove gui from display
    gui_remove_view_port(app->gui, app->view_port);
    //Freeing display
    view_port_free(app->view_port);
    furi_record_close(RECORD_GUI);
    app->gui = NULL;
    furi_record_close(RECORD_BT);
    app->bt = NULL;
    free(app);
}
