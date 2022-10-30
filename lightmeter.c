#include "lightmeter_i.h"

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} HelloWorldEvent;

static void draw_callback(Canvas* canvas, void* ctx) {
    // UNUSED(ctx);
    LightMeter* lightmeter = acquire_mutex((ValueMutex*)ctx, 25);

    switch(lightmeter->main_view->current_view) {
    case MAIN_VIEW:
        draw_main_view(canvas, lightmeter->main_view, lightmeter->sender);
        break;

    case CONFIG_VIEW:
        // draw_scanner_view(canvas, i2ctools->scanner);
        break;

    default:
        break;
    }

    release_mutex((ValueMutex*)ctx, lightmeter);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    HelloWorldEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    HelloWorldEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t lightmeter_app(void* p) {
    UNUSED(p);

    HelloWorldEvent event;

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(HelloWorldEvent));

    // Alloc lightmeter
    LightMeter* lightmeter = malloc(sizeof(LightMeter));
    ValueMutex lightmeter_mutex;
    if(!init_mutex(&lightmeter_mutex, lightmeter, sizeof(LightMeter))) {
        FURI_LOG_E(APP_NAME, "cannot create mutex\r\n");
        free(lightmeter);
        return -1;
    }

    lightmeter->view_port = view_port_alloc();
    lightmeter->sender = lightmeter_sender_alloc();
    lightmeter->main_view = lightmeter_main_view_alloc();

    view_port_draw_callback_set(lightmeter->view_port, draw_callback, &lightmeter_mutex);
    view_port_input_callback_set(lightmeter->view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, lightmeter->view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, 200);

    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);

    notification_message(
        notifications, &sequence_display_backlight_enforce_on); // force on backlight

    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.type == EventTypeInput) {
            if(event.input.key == InputKeyBack && event.input.type == InputTypeRelease) {
                if(lightmeter->main_view->current_view == MAIN_VIEW) {
                    break;
                } else {
                    if(lightmeter->main_view->current_view == CONFIG_VIEW) {
                        // do something
                    }
                    lightmeter->main_view->current_view = MAIN_VIEW;
                }
            } else if(
                (event.input.key == InputKeyUp || event.input.key == InputKeyDown) &&
                event.input.type == InputTypeRelease) {
                if(lightmeter->main_view->current_view == MAIN_VIEW &&
                    lightmeter->main_view->current_mode == FIXED_TIME) {
                    lightmeter->main_view->current_mode = FIXED_APERTURE;
                } else if(
                    lightmeter->main_view->current_view == MAIN_VIEW &&
                    lightmeter->main_view->current_mode == FIXED_APERTURE) {
                    lightmeter->main_view->current_mode = FIXED_TIME;
                }
            }

        } 
        
        if(event.type == EventTypeTick) {
            notification_message(notifications, &sequence_blink_blue_100);
            lightmeter->sender->value = 0x20;
            lightmeter->sender->must_send = true;
            view_port_update(lightmeter->view_port);
        }
    }

    furi_message_queue_free(event_queue);

    gui_remove_view_port(gui, lightmeter->view_port);
    view_port_free(lightmeter->view_port);
    furi_record_close(RECORD_GUI);
    furi_timer_free(timer);
    furi_record_close(RECORD_NOTIFICATION);
    lightmeter_sender_free(lightmeter->sender);
    lightmeter_main_view_free(lightmeter->main_view);
    notification_message(
        notifications, &sequence_display_backlight_enforce_auto); // set backlight back to auto

    return 0;
}
