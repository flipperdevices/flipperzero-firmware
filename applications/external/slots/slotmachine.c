#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <stdlib.h>
#include <stdio.h>
#include <input/input.h>
#include <furi_hal.h>
#include "slotmachine_icons.h"

const Icon* slot_frames[] = {&I_x2, &I_x3, &I_x4, &I_x2_2, &I_x5};

const uint8_t slot_coef[] = {2, 3, 4, 2, 5};

typedef struct {
    uint8_t x, y, value, times, speed;
    bool spining;
} SlotColumn;

int COLUMNS_COUNT = 4;
int MAX_COLUMNS_COUNT = 4;

typedef struct {
    Gui* gui; // container gui
    ViewPort* view_port; // current viewport
    FuriMessageQueue* input_queue; // Input Events queue
    FuriMutex** model_mutex; // mutex for safe threads
    uint16_t bet;
    double money, winamount;
    SlotColumn* columns[4];
    bool winview;
} SlotMachineApp;

#define START_MONEY 1500;
#define START_BET 300;
#define SSRAND_MAX 5;
#define DEFAULT_SPEED 16;

uint8_t DEFAULT_SPINNING_TIMES = 10;

void game_results(SlotMachineApp* app) {
    int matches[] = {0, 0, 0, 0, 0};

    double total = 0;

    for(int i = 0; i < COLUMNS_COUNT; i++) {
        matches[app->columns[i]->value]++;
    }

    for(int i = 0; i < 5; i++) {
        if(matches[i] >= 2) {
            total += app->bet * (slot_coef[i] / (double)(MAX_COLUMNS_COUNT + 1 - matches[i]));
        }
    }

    if(total > 0) {
        app->money += total;
        app->winamount = total;
        app->winview = true;
    }
}

void draw_container(Canvas* canvas) {
    canvas_draw_rframe(canvas, 2, 12, 120, 34, 3);
    canvas_draw_rframe(canvas, 2, 13, 120, 34, 3);
    canvas_draw_rframe(canvas, 2, 14, 120, 34, 3);
    canvas_draw_rframe(canvas, 2, 15, 120, 34, 3);
    canvas_draw_rframe(canvas, 2, 16, 120, 34, 3);
    canvas_draw_rframe(canvas, 2, 17, 120, 34, 3);
    canvas_draw_line(canvas, 31, 16, 31, 48);
    canvas_draw_line(canvas, 61, 16, 61, 48);
    canvas_draw_line(canvas, 91, 16, 91, 48);
}

bool checkIsSpinning(SlotMachineApp* slotmachine) {
    for(int i = 0; i < COLUMNS_COUNT; i++) {
        if(slotmachine->columns[i]->spining) return true;
    }

    return false;
}

void drawButton(Canvas* canvas, uint8_t x, uint8_t y, char* str, bool invert) {
    const uint8_t string_width = canvas_string_width(canvas, str);
    canvas_set_font(canvas, FontSecondary);
    if(invert) {
        canvas_draw_rbox(canvas, x, y, string_width + 15, 11, 3);
        canvas_invert_color(canvas);
    } else {
        canvas_draw_rframe(canvas, x, y, string_width + 15, 11, 3);
    }
    canvas_draw_circle(canvas, x + 5, y + 5, 3);
    canvas_draw_circle(canvas, x + 5, y + 5, 1);
    canvas_draw_str(canvas, x + 13, y + 9, str);
    canvas_invert_color(canvas);
}

// viewport callback
void slotmachine_draw_callback(Canvas* canvas, void* ctx) {
    SlotMachineApp* slotmachine = (SlotMachineApp*)ctx;
    furi_check(furi_mutex_acquire(slotmachine->model_mutex, FuriWaitForever) == FuriStatusOk);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 10, "Slots");
    canvas_draw_icon(canvas, 30, 3, &I_little_coin);

    char moneyStr[15];
    snprintf(moneyStr, sizeof(moneyStr), "$%.0f", slotmachine->money);

    char betStr[7];
    snprintf(betStr, sizeof(betStr), "$%d", slotmachine->bet);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 45, 10, moneyStr);
    canvas_draw_str(canvas, 2, canvas_height(canvas) - 3, "Bet:");
    canvas_draw_str(canvas, 20, canvas_height(canvas) - 3, betStr);

    if(slotmachine->winview) {
        char winamountStr[30];
        snprintf(winamountStr, sizeof(winamountStr), "You win: $%.2f!", slotmachine->winamount);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 2, 35, winamountStr);
        drawButton(canvas, 95, 52, "Ok", false);

        furi_mutex_release(slotmachine->model_mutex);
        return;
    }

    for(int i = 0; i < COLUMNS_COUNT; i++) {
        if(slotmachine->columns[i]->spining) {
            slotmachine->columns[i]->y += slotmachine->columns[i]->speed;

            if(slotmachine->columns[i]->y > 31) {
                slotmachine->columns[i]->y = 13;
                slotmachine->columns[i]->times--;
                slotmachine->columns[i]->speed--;
                slotmachine->columns[i]->value = rand() % SSRAND_MAX;

                if(slotmachine->columns[i]->times == 0) {
                    slotmachine->columns[i]->y = 23;
                    slotmachine->columns[i]->spining = false;

                    if(i == COLUMNS_COUNT - 1) {
                        game_results(slotmachine);
                    }
                }

                if(i < COLUMNS_COUNT - 1 &&
                   slotmachine->columns[i]->times ==
                       (DEFAULT_SPINNING_TIMES - (int)(DEFAULT_SPINNING_TIMES / 3))) {
                    slotmachine->columns[i + 1]->spining = true;
                }
            }
        }
        canvas_draw_icon(
            canvas,
            slotmachine->columns[i]->x,
            slotmachine->columns[i]->y,
            slot_frames[slotmachine->columns[i]->value]);
    }
    draw_container(canvas);
    drawButton(canvas, 90, 52, "Spin", checkIsSpinning(slotmachine));

    furi_mutex_release(slotmachine->model_mutex);
}

// callback for viewport input events
static void slotmachine_input_callback(InputEvent* input_event, void* ctx) {
    SlotMachineApp* slotmachine = ctx;
    furi_message_queue_put(slotmachine->input_queue, input_event, FuriWaitForever);
}

// allocation memory and initialization
SlotMachineApp* slotmachine_app_alloc() {
    SlotMachineApp* app = malloc(sizeof(SlotMachineApp));
    app->model_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    app->input_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    app->view_port = view_port_alloc();
    view_port_draw_callback_set(
        app->view_port, slotmachine_draw_callback, app); // viewport callback register
    view_port_input_callback_set(app->view_port, slotmachine_input_callback, app);

    app->money = START_MONEY;
    app->bet = START_BET;
    app->winview = false;
    app->winamount = 0;

    int x = 7;

    for(int i = 0; i < COLUMNS_COUNT; i++) {
        app->columns[i] = malloc(sizeof(SlotColumn));
        app->columns[i]->x = x;
        app->columns[i]->y = 25;
        app->columns[i]->value = 0;
        app->columns[i]->spining = false;
        x += 30;
    }

    app->gui = furi_record_open("gui"); // start gui and adding viewport
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    return app;
}

void slotmachine_app_free(SlotMachineApp* app) {
    gui_remove_view_port(app->gui, app->view_port);
    view_port_free(app->view_port);
    furi_record_close("gui"); // free memory
    furi_mutex_free(app->model_mutex);
    for(int i = 0; i < COLUMNS_COUNT; i++) {
        free(app->columns[i]);
    }
    free(app);
}

// entry point
int32_t slotmachine_app(void* p) {
    UNUSED(p);

    SlotMachineApp* slotmachine = slotmachine_app_alloc();
    InputEvent input;

    // endless input cycle
    while(furi_message_queue_get(slotmachine->input_queue, &input, FuriWaitForever) ==
          FuriStatusOk) {
        // if thread idle - take it
        furi_check(furi_mutex_acquire(slotmachine->model_mutex, FuriWaitForever) == FuriStatusOk);

        if(!checkIsSpinning(slotmachine)) {
            if(input.key == InputKeyBack) {
                // exit on back button
                furi_mutex_release(slotmachine->model_mutex);
                break;
            } else if(input.key == InputKeyOk && input.type == InputTypeShort && slotmachine->winview) {
                slotmachine->winview = false;
            } else if(
                input.key == InputKeyOk && input.type == InputTypeShort &&
                slotmachine->bet <= slotmachine->money) {
                COLUMNS_COUNT = rand() % 3 + 2;
                slotmachine->money -= slotmachine->bet;
                slotmachine->columns[0]->spining = true;

                for(int i = 0; i < COLUMNS_COUNT; i++) {
                    slotmachine->columns[i]->times = DEFAULT_SPINNING_TIMES;
                    slotmachine->columns[i]->speed = DEFAULT_SPEED;
                }
            } else if(input.key == InputKeyUp) {
                if(slotmachine->bet + 10 < slotmachine->money) {
                    slotmachine->bet += 10;
                }
            } else if(input.key == InputKeyDown) {
                if(slotmachine->bet - 10 > 0) {
                    slotmachine->bet -= 10;
                }
            }
        }

        // release thread
        furi_mutex_release(slotmachine->model_mutex);
        // redraw viewport
        view_port_update(slotmachine->view_port);
    }

    slotmachine_app_free(slotmachine);

    return 0;
}
