#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <input/input.h>
#include <stdlib.h>
#include "counter_icons.h"

#define MAX_COUNT 999
#define BOXTIME 2
#define BOXWIDTH 30
#define BOXWIDTH_BIG 42
#define BOXHEIGHT 30
#define MIDDLE_X 64 - BOXWIDTH / 2
#define MIDDLE_X_BIG 64 - BOXWIDTH_BIG / 2
#define MIDDLE_Y 32 - BOXHEIGHT / 2
#define OFFSET_Y 9

typedef struct {
    FuriMessageQueue* input_queue;
    ViewPort* view_port;
    Gui* gui;
    NotificationApp* notification;
    FuriMutex** mutex;

    int count;
    bool pressed;
    int boxtimer;
} Counter;

const NotificationSequence sequence_count = {

    // Beep
    //&message_note_c7,
    //&message_delay_50,
    //&message_sound_off,

    // Vibrate
    &message_vibro_on,
    &message_delay_10,
    &message_delay_10,
    &message_vibro_off,

    NULL,
};

void state_free(Counter* c) {
    notification_message_block(c->notification, &sequence_display_backlight_enforce_auto);
    gui_remove_view_port(c->gui, c->view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);
    view_port_free(c->view_port);
    furi_message_queue_free(c->input_queue);
    furi_mutex_free(c->mutex);
    free(c);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    Counter* c = ctx;

    if(input_event->type == InputTypeShort) {
        furi_message_queue_put(c->input_queue, input_event, 0);
    }
}

static void render_callback(Canvas* canvas, void* ctx) {
    Counter* c = ctx;

    furi_check(furi_mutex_acquire(c->mutex, FuriWaitForever) == FuriStatusOk);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignCenter, "Counter :)");
    canvas_set_font(canvas, FontBigNumbers);

    char scount[5];
    uint16_t dynamicBoxWidth = BOXWIDTH;
    uint16_t dynamicMiddleX = MIDDLE_X;

    if(c->count > 99) {
        dynamicBoxWidth = BOXWIDTH_BIG;
        dynamicMiddleX = MIDDLE_X_BIG;
    }

    if(c->pressed == true || c->boxtimer > 0) {
        canvas_draw_rframe(
            canvas, dynamicMiddleX, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX - 1,
            MIDDLE_Y + OFFSET_Y - 1,
            dynamicBoxWidth + 2,
            BOXHEIGHT + 2,
            5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX - 2,
            MIDDLE_Y + OFFSET_Y - 2,
            dynamicBoxWidth + 4,
            BOXHEIGHT + 4,
            5);
        c->pressed = false;
        c->boxtimer--;
    } else {
        canvas_draw_rframe(
            canvas, dynamicMiddleX, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
    }

    snprintf(scount, sizeof(scount), "%d", c->count);
    canvas_draw_str_aligned(canvas, 64, 32 + OFFSET_Y, AlignCenter, AlignCenter, scount);
    furi_mutex_release(c->mutex);
}

Counter* state_init() {
    Counter* c = malloc(sizeof(Counter));

    c->input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    c->view_port = view_port_alloc();
    c->gui = furi_record_open(RECORD_GUI);
    c->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    c->notification = furi_record_open(RECORD_NOTIFICATION);

    c->count = 0;
    c->boxtimer = 0;

    view_port_input_callback_set(c->view_port, input_callback, c);
    view_port_draw_callback_set(c->view_port, render_callback, c);
    gui_add_view_port(c->gui, c->view_port, GuiLayerFullscreen);

    return c;
}

int32_t counterapp(void) {
    Counter* c = state_init();

    InputEvent input;

    while(furi_message_queue_get(c->input_queue, &input, FuriWaitForever) == FuriStatusOk) {
        furi_check(furi_mutex_acquire(c->mutex, FuriWaitForever) == FuriStatusOk);
        notification_message(c->notification, &sequence_count);

        switch(input.key) {
        case InputKeyBack:
            furi_mutex_release(c->mutex);
            state_free(c);
            return 0;
        case InputKeyUp:
            if(c->count < MAX_COUNT) {
                c->pressed = true;
                c->boxtimer = BOXTIME;
                c->count++;
            }
            break;
        case InputKeyDown:
            if(c->count != 0) {
                c->pressed = true;
                c->boxtimer = BOXTIME;
                c->count--;
            }
            break;
        case InputKeyRight:
            if(c->count < MAX_COUNT) {
                c->pressed = true;
                c->boxtimer = BOXTIME;
                c->count++;
            }
            break;
        case InputKeyLeft:
            if(c->count != 0) {
                c->pressed = true;
                c->boxtimer = BOXTIME;
                c->count--;
            }
            break;
        case InputKeyOk:
            if(c->count < MAX_COUNT) {
                c->pressed = true;
                c->boxtimer = BOXTIME;
                c->count++;
            }
            break;
        default:
            break;
        }

        furi_mutex_release(c->mutex);
        view_port_update(c->view_port);
    }

    state_free(c);

    return 0;
}
