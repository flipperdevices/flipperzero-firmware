#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <counter_icons.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define MAX_COUNT 99999
#define NUM_WIDTH 12
#define BOXTIME 2
#define MIN_BOXWIDTH 30
#define BOXHEIGHT 30
#define MIDDLE_Y 32 - BOXHEIGHT / 2
#define OFFSET_X 8
#define OFFSET_Y 9
#define VIBRO_TIME_MS 20

typedef struct {
    FuriMessageQueue* input_queue;
    ViewPort* view_port;
    Gui* gui;
    FuriMutex** mutex;
    NotificationApp* notifications;

    int count;
    bool pressed;
    int boxtimer;
    bool vibro;
} Counter;

void state_free(Counter* c) {
    gui_remove_view_port(c->gui, c->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(c->view_port);
    furi_message_queue_free(c->input_queue);
    furi_mutex_free(c->mutex);
    free(c);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    Counter* c = ctx;
    if(input_event->type == InputTypeShort || input_event->type == InputTypeLong) {
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

    char scount[8];
    snprintf(scount, sizeof(scount), "%d", c->count);
    size_t cntr_len = strlen(scount);
    size_t boxwidth = cntr_len * NUM_WIDTH + OFFSET_X;
    if(boxwidth < MIN_BOXWIDTH) {
        boxwidth = MIN_BOXWIDTH;
    }
    size_t middle_x = 64 - boxwidth / 2;
    if(c->pressed == true || c->boxtimer > 0) {
        canvas_draw_rframe(canvas, middle_x, MIDDLE_Y + OFFSET_Y, boxwidth, BOXHEIGHT, 5);
        canvas_draw_rframe(
            canvas, middle_x - 1, MIDDLE_Y + OFFSET_Y - 1, boxwidth + 2, BOXHEIGHT + 2, 5);
        canvas_draw_rframe(
            canvas, middle_x - 2, MIDDLE_Y + OFFSET_Y - 2, boxwidth + 4, BOXHEIGHT + 4, 5);
        c->pressed = false;
        c->boxtimer--;
    } else {
        canvas_draw_rframe(canvas, middle_x, MIDDLE_Y + OFFSET_Y, boxwidth, BOXHEIGHT, 5);
    }
    canvas_draw_str_aligned(canvas, 64, 32 + OFFSET_Y, AlignCenter, AlignCenter, scount);
    furi_mutex_release(c->mutex);
}

Counter* state_init() {
    Counter* c = malloc(sizeof(Counter));
    c->input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    c->view_port = view_port_alloc();
    c->gui = furi_record_open(RECORD_GUI);
    c->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    c->notifications = furi_record_open(RECORD_NOTIFICATION);
    c->count = 0;
    c->boxtimer = 0;
    c->vibro = false;
    view_port_input_callback_set(c->view_port, input_callback, c);
    view_port_draw_callback_set(c->view_port, render_callback, c);
    gui_add_view_port(c->gui, c->view_port, GuiLayerFullscreen);
    return c;
}

int32_t counterapp(void) {
    Counter* c = state_init();

    InputEvent input;
    for(bool processing = true; processing;) {
        while(furi_message_queue_get(c->input_queue, &input, FuriWaitForever) == FuriStatusOk) {
            furi_check(furi_mutex_acquire(c->mutex, FuriWaitForever) == FuriStatusOk);

            if(input.type == InputTypeShort) {
                switch(input.key) {
                case InputKeyBack:
                    processing = false;
                    break;
                case InputKeyUp:
                case InputKeyOk:
                    if(c->count < MAX_COUNT) {
                        c->pressed = true;
                        c->boxtimer = BOXTIME;
                        c->count++;
                        if(c->vibro) {
                            notification_message(c->notifications, &sequence_set_vibro_on);
                            furi_delay_ms(VIBRO_TIME_MS);
                            notification_message(c->notifications, &sequence_reset_vibro);
                        }
                    }
                    break;
                case InputKeyDown:
                    if(c->count > 0) {
                        c->pressed = true;
                        c->boxtimer = BOXTIME;
                        c->count--;
                    }
                    break;
                default:
                    break;
                }
            } else if(input.type == InputTypeLong) {
                switch(input.key) {
                case InputKeyBack:
                    c->count = 0;
                    break;
                case InputKeyOk:
                    c->vibro = !c->vibro;
                    break;
                default:
                    break;
                }
            }
            furi_mutex_release(c->mutex);
            if(!processing) {
                break;
            }
            view_port_update(c->view_port);
        }
    }
    state_free(c);
    return 0;
}
