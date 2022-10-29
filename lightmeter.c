#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>

#include "i2csender.h"

#define APP_NAME "Light Meter"

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} HelloWorldEvent;

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "Hello World!");

    // char str[3];
    // snprintf(str, sizeof(str), "%d", main_view->counter);
    // canvas_draw_str(canvas, 30, 30, str);

    elements_button_center(canvas, "Send");
}

static void input_callback(InputEvent* input_event, void* ctx) {
    // Проверяем, что контекст не нулевой
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    HelloWorldEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(FuriMessageQueue* event_queue) {
    // Проверяем, что контекст не нулевой
    furi_assert(event_queue);

    HelloWorldEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t lightmeter_app(void* p) {
    UNUSED(p);


	HelloWorldEvent event;

	FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(HelloWorldEvent));

     // Alloc i2ctools
    // LightMeter* lightmeter = malloc(sizeof(LightMeter));
    // ValueMutex lightmeter_mutex;
    // if(!init_mutex(&lightmeter_mutex, lightmeter, sizeof(LightMeter))) {
    //     FURI_LOG_E(APP_NAME, "cannot create mutex\r\n");
    //     free(lightmeter);
    //     return -1;
    // }

    ViewPort* view_port = view_port_alloc();

    i2cSender* sender = i2c_sender_alloc();

    view_port_draw_callback_set(view_port, draw_callback, NULL);
	view_port_input_callback_set(view_port, input_callback, event_queue);

	Gui* gui = furi_record_open(RECORD_GUI);
	gui_add_view_port(gui, view_port, GuiLayerFullscreen);

	FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);
	furi_timer_start(timer, 500);

	NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);

    sender = i2c_sender_alloc();


	while(1) {

    furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);


    if (event.type == EventTypeInput) {

        if (event.input.key == InputKeyBack) {
            break;
        } else if (event.input.key == InputKeyOk) {
            sender->value = 0x20;
            i2c_send(sender);
            view_port_update(view_port);
        }

    } else if (event.type == EventTypeTick) {

        notification_message(notifications, &sequence_blink_blue_100);
    }
}


    furi_message_queue_free(event_queue);

    gui_remove_view_port(gui, view_port);
	view_port_free(view_port);
	furi_record_close(RECORD_GUI);
	furi_timer_free(timer);
	furi_record_close(RECORD_NOTIFICATION);
    i2c_sender_free(sender);

    return 0;
}
