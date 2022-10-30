#include <stdio.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include "lightmeter_icons.h"

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

    if(lightmeter->sender->must_send) {
        i2c_send(lightmeter->sender);
    }

    canvas_clear(canvas);

    // top row
    // draw line
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "F");

    canvas_draw_str_aligned(canvas, 20, 0, AlignLeft, AlignTop, "ISO: 400");

    char str[12];

    if(lightmeter->sender->sended) {
        // for(uint8_t i = 0; i < sizeof(lightmeter->sender->recv); i++) {
        //     snprintf(str, sizeof(str), "0x%02x", (int)lightmeter->sender->recv[i]);
        //     canvas_draw_str_aligned(canvas, 90, 25 + (i * 10), AlignLeft, AlignTop, str);
        // }
        
        int lx = ((int)lightmeter->sender->recv[0] << 8) | ((int)lightmeter->sender->recv[1]);
        snprintf(str, sizeof(str), "lx: %d", lx);
        canvas_draw_str_aligned(canvas, 80, 0, AlignLeft, AlignTop, str);
    }

    // add f, T values
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_icon(canvas, 15, 17, &I_f_10x14);
    canvas_draw_str_aligned(canvas, 27, 15, AlignLeft, AlignTop, "/3.2");
    canvas_draw_icon(canvas, 15, 34, &I_T_10x14);
    canvas_draw_str_aligned(canvas, 27, 34, AlignLeft, AlignTop, ":1/50");

    // add asterisk
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_str_aligned(canvas, 3, 17, AlignLeft, AlignTop, "*");
    canvas_draw_str_aligned(canvas, 3, 36, AlignLeft, AlignTop, "*");
    
    // create buttons (for the future)
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Config");
    elements_button_right(canvas, "Config");

    // draw EV number
    canvas_draw_line(canvas, 95, 15, 95, 33);
    canvas_draw_line(canvas, 96, 15, 96, 33);
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 100, 15, AlignLeft, AlignTop, "EV:\n+7");

    // elements_button_center(canvas, "Send");
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
    furi_timer_start(timer, 200);

    NotificationApp* notifications = furi_record_open(RECORD_NOTIFICATION);

    notification_message(notifications, &sequence_display_backlight_enforce_on); // force on backlight

    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.type == EventTypeInput) {
            if(event.input.key == InputKeyBack) {
                break;
            }

        } else if(event.type == EventTypeTick) {
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
    i2c_sender_free(lightmeter->sender);
    notification_message(notifications, &sequence_display_backlight_enforce_auto); // set backlight back to auto

    return 0;
}
