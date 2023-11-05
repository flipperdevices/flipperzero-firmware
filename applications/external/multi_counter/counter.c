#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <input/input.h>
#include <stdlib.h>
#include "multi_counter_icons.h"

#define MAX_COUNT 99
#define BOXTIME 2
#define BOXWIDTH 26
#define BOXWIDTH_BIG 42
#define BOXHEIGHT 26
#define MIDDLE_X 64 - BOXWIDTH / 2
#define MIDDLE_X_BIG 64 - BOXWIDTH_BIG / 2
#define MIDDLE_Y 32 - BOXHEIGHT / 2
#define OFFSET_Y 9

static const Icon* UP = &I_UP;
static const Icon* DOWN = &I_DOWN;
static const Icon* S_ON = &I_S_ON;
static const Icon* S_OFF = &I_S_OFF;

typedef struct {
    FuriMessageQueue* input_queue;
    ViewPort* view_port;
    Gui* gui;
    NotificationApp* notification;
    FuriMutex** mutex;

    bool togglelight;
    int count;
    bool pressed;
    int boxtimer;
    int count2;
    bool pressed2;
    int boxtimer2;
    int count3;
    bool pressed3;
    int boxtimer3;
    int count4;
    bool pressed4;
    int boxtimer4;
    int player;

} Counter;

const NotificationSequence sequence_count = {

    // Vibrate
    &message_vibro_on,
    &message_delay_10,
    &message_delay_10,
    &message_vibro_off,

    NULL,
};
const NotificationSequence sequence_count_buzz = {

    // Beep
    &message_note_c7,
    &message_delay_50,
    &message_sound_off,

    NULL,
};

const NotificationSequence sequence_count_ok = {

    // Beep
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_delay_100,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,

    NULL,
};

const NotificationSequence sequence_count_ok_buzz = {

    // Beep
    &message_note_c7,
    &message_delay_50,
    &message_sound_off,
    &message_delay_100,
    &message_note_c7,
    &message_delay_50,
    &message_sound_off,

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
    canvas_draw_str_aligned(canvas, 16, 18, AlignCenter, AlignCenter, "  P1");
    canvas_draw_str_aligned(canvas, 46, 18, AlignCenter, AlignCenter, "  P2");
    canvas_draw_str_aligned(canvas, 76, 18, AlignCenter, AlignCenter, "  P3");
    canvas_draw_str_aligned(canvas, 106, 18, AlignCenter, AlignCenter, "  P4");
    canvas_set_font(canvas, FontBigNumbers);

    char scount[5];
    uint16_t dynamicBoxWidth = BOXWIDTH;
    uint16_t dynamicMiddleX = MIDDLE_X - 45;
    uint16_t dynamicMiddleX2 = MIDDLE_X - 15;
    uint16_t dynamicMiddleX3 = MIDDLE_X + 15;
    uint16_t dynamicMiddleX4 = MIDDLE_X + 45;
    uint16_t dynamicArrowX = 0;

    if(c->player == 1) {
        dynamicArrowX = dynamicMiddleX + 10;
        if(c->count > 0) {
            canvas_draw_icon(canvas, dynamicArrowX, 55, DOWN);
        }
        if(c->count < 99) {
            canvas_draw_icon(canvas, dynamicArrowX, 23, UP);
        }
    }
    if(c->player == 2) {
        dynamicArrowX = dynamicMiddleX2 + 10;
        if(c->count2 > 0) {
            canvas_draw_icon(canvas, dynamicArrowX, 55, DOWN);
        }
        if(c->count2 < 99) {
            canvas_draw_icon(canvas, dynamicArrowX, 23, UP);
        }
    }
    if(c->player == 3) {
        dynamicArrowX = dynamicMiddleX3 + 10;
        if(c->count3 > 0) {
            canvas_draw_icon(canvas, dynamicArrowX, 55, DOWN);
        }
        if(c->count3 < 99) {
            canvas_draw_icon(canvas, dynamicArrowX, 23, UP);
        }
    }
    if(c->player == 4) {
        dynamicArrowX = dynamicMiddleX4 + 10;
        if(c->count4 > 0) {
            canvas_draw_icon(canvas, dynamicArrowX, 55, DOWN);
        }
        if(c->count4 < 99) {
            canvas_draw_icon(canvas, dynamicArrowX, 23, UP);
        }
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
    } else if(c->pressed2 == true || c->boxtimer2 > 0) {
        canvas_draw_rframe(
            canvas, dynamicMiddleX2, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX2 - 1,
            MIDDLE_Y + OFFSET_Y - 1,
            dynamicBoxWidth + 2,
            BOXHEIGHT + 2,
            5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX2 - 2,
            MIDDLE_Y + OFFSET_Y - 2,
            dynamicBoxWidth + 4,
            BOXHEIGHT + 4,
            5);
        c->pressed2 = false;
        c->boxtimer2--;
    } else if(c->pressed3 == true || c->boxtimer3 > 0) {
        canvas_draw_rframe(
            canvas, dynamicMiddleX3, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX3 - 1,
            MIDDLE_Y + OFFSET_Y - 1,
            dynamicBoxWidth + 2,
            BOXHEIGHT + 2,
            5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX3 - 2,
            MIDDLE_Y + OFFSET_Y - 2,
            dynamicBoxWidth + 4,
            BOXHEIGHT + 4,
            5);
        c->pressed3 = false;
        c->boxtimer3--;
    } else if(c->pressed4 == true || c->boxtimer4 > 0) {
        canvas_draw_rframe(
            canvas, dynamicMiddleX4, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX4 - 1,
            MIDDLE_Y + OFFSET_Y - 1,
            dynamicBoxWidth + 2,
            BOXHEIGHT + 2,
            5);
        canvas_draw_rframe(
            canvas,
            dynamicMiddleX4 - 2,
            MIDDLE_Y + OFFSET_Y - 2,
            dynamicBoxWidth + 4,
            BOXHEIGHT + 4,
            5);
        c->pressed4 = false;
        c->boxtimer4--;
    }
    if(c->togglelight == true) {
        canvas_draw_icon(canvas, 116, 0, S_ON);
    }
    if(c->togglelight == false) {
        canvas_draw_icon(canvas, 116, 0, S_OFF);
    }

    canvas_draw_rframe(canvas, dynamicMiddleX, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
    canvas_draw_rframe(
        canvas, dynamicMiddleX2, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
    canvas_draw_rframe(
        canvas, dynamicMiddleX3, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);
    canvas_draw_rframe(
        canvas, dynamicMiddleX4, MIDDLE_Y + OFFSET_Y, dynamicBoxWidth, BOXHEIGHT, 5);

    snprintf(scount, sizeof(scount), "%d", c->count);
    canvas_draw_str_aligned(canvas, 19, 32 + OFFSET_Y, AlignCenter, AlignCenter, scount);
    furi_mutex_release(c->mutex);

    snprintf(scount, sizeof(scount), "%d", c->count2);
    canvas_draw_str_aligned(canvas, 49, 32 + OFFSET_Y, AlignCenter, AlignCenter, scount);
    furi_mutex_release(c->mutex);

    snprintf(scount, sizeof(scount), "%d", c->count3);
    canvas_draw_str_aligned(canvas, 79, 32 + OFFSET_Y, AlignCenter, AlignCenter, scount);
    furi_mutex_release(c->mutex);

    snprintf(scount, sizeof(scount), "%d", c->count4);
    canvas_draw_str_aligned(canvas, 109, 32 + OFFSET_Y, AlignCenter, AlignCenter, scount);
    furi_mutex_release(c->mutex);
}

Counter* state_init() {
    Counter* c = malloc(sizeof(Counter));

    c->input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));
    c->view_port = view_port_alloc();
    c->gui = furi_record_open(RECORD_GUI);
    c->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    c->notification = furi_record_open(RECORD_NOTIFICATION);

    c->togglelight = false;
    c->count = 0;
    c->count2 = 0;
    c->count3 = 0;
    c->count4 = 0;
    c->player = 1;
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

        switch(input.key) {
        case InputKeyBack:
            furi_mutex_release(c->mutex);
            state_free(c);
            return 0;
        case InputKeyUp:
            if(c->count < MAX_COUNT && c->player == 1) {
                c->pressed = true;
                c->boxtimer = BOXTIME;
                c->count++;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            }
            if(c->count2 < MAX_COUNT && c->player == 2) {
                c->pressed2 = true;
                c->boxtimer2 = BOXTIME;
                c->count2++;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            }
            if(c->count3 < MAX_COUNT && c->player == 3) {
                c->pressed3 = true;
                c->boxtimer3 = BOXTIME;
                c->count3++;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            }
            if(c->count4 < MAX_COUNT && c->player == 4) {
                c->pressed4 = true;
                c->boxtimer4 = BOXTIME;
                c->count4++;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            }
            break;
        case InputKeyDown:
            if(c->count != 0 && c->player == 1) {
                c->pressed = true;
                c->boxtimer = BOXTIME;
                c->count--;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            } else if(c->count2 != 0 && c->player == 2) {
                c->pressed2 = true;
                c->boxtimer2 = BOXTIME;
                c->count2--;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            } else if(c->count3 != 0 && c->player == 3) {
                c->pressed3 = true;
                c->boxtimer3 = BOXTIME;
                c->count3--;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            } else if(c->count4 != 0 && c->player == 4) {
                c->pressed4 = true;
                c->boxtimer4 = BOXTIME;
                c->count4--;
                notification_message(c->notification, &sequence_count);
                if(c->togglelight == true) {
                    notification_message(c->notification, &sequence_count_buzz);
                }
            }
            break;
        case InputKeyRight:
            if(c->player == 4) {
                c->player = 0;
            }
            if(c->player < 4) {
                c->player++;
            }
            break;
        case InputKeyLeft:
            if(c->player == 1) {
                c->player = 4;
            } else {
                c->player--;
            }
            break;
        case InputKeyOk:
            c->togglelight = !c->togglelight;
            if(c->togglelight == true) {
                notification_message(c->notification, &sequence_count_ok_buzz);
            }
            if(c->togglelight == false) {
                notification_message(c->notification, &sequence_count_ok);
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
