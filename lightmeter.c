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

typedef struct {
    i2cSender* sender;
    ViewPort* view_port;
} LightMeter;

static void draw_callback(Canvas* canvas, void* ctx) {
    // UNUSED(ctx);
    LightMeter* lightmeter = acquire_mutex((ValueMutex*)ctx, 25);

    canvas_clear(canvas);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 0, 10, "Received value!");

    if(lightmeter->sender->must_send) {
        i2c_send(lightmeter->sender);
    }

    char str[12];
    // snprintf(str, sizeof(str), "%#04x %#04x", lightmeter->sender->recv[0], lightmeter->sender->recv[1]);
    // canvas_draw_str(canvas, 30, 30, str);

    if(lightmeter->sender->sended) {
        for(uint8_t i = 0; i < sizeof(lightmeter->sender->recv); i++) {
            snprintf(str, sizeof(str), "0x%02x", (int)lightmeter->sender->recv[i]);
            canvas_draw_str_aligned(canvas, 90, 25 + (i * 10), AlignLeft, AlignTop, str);
        }
        
        int num1, num2, num3;
        num1 = (int)lightmeter->sender->recv[0];
        num2 = (int)lightmeter->sender->recv[1];
        num3 = (num1 << 8) | (num2);
        snprintf(str, sizeof(str), "Int: %d", num3);
        canvas_draw_str_aligned(canvas, 5, 25 + (0 * 10), AlignLeft, AlignTop, str);
    }

    elements_button_center(canvas, "Send");
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

    lightmeter->sender = i2c_sender_alloc();

    view_port_draw_callback_set(lightmeter->view_port, draw_callback, &lightmeter_mutex);
    view_port_input_callback_set(lightmeter->view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, lightmeter->view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);
    // furi_timer_start(timer, 500);

    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);

    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.type == EventTypeInput) {
            if(event.input.key == InputKeyBack) {
                break;
            } else if(event.input.key == InputKeyOk) {
                lightmeter->sender->value = 0x20;
                lightmeter->sender->must_send = true;
                view_port_update(lightmeter->view_port);
            }

        } else if(event.type == EventTypeTick) {
            notification_message(notifications, &sequence_blink_blue_100);
        }
    }

    furi_message_queue_free(event_queue);

    gui_remove_view_port(gui, lightmeter->view_port);
    view_port_free(lightmeter->view_port);
    furi_record_close(RECORD_GUI);
    furi_timer_free(timer);
    furi_record_close(RECORD_NOTIFICATION);
    i2c_sender_free(lightmeter->sender);

    return 0;
}
