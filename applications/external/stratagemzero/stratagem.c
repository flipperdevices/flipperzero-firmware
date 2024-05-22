#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <gui/elements.h>
#include <gui/canvas.h>
#include <stdlib.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stratagem_icons.h>
#include "tinyfont.h"

#define TIME_PENALTY_MULTIPLIER 0.10
#define TIME_REWARD_MULTIPLIER 0.10
#define TIME_DECREASE_MULTIPLER 0.125

typedef struct {
    char name[24];
    char arrows[12];
} stratagem;

typedef struct {
    char name[24];
    int groupID;
    stratagem stratagems[16];
    int stratagemCount;
} group;

typedef enum {
    begin,
    active,
    end,
} state;

group groups[6] = {};

NotificationApp* notif = 0;

int32_t totaltime = 2000;
int32_t remainingtime = 2000;
int8_t groupIndex = 0;
int8_t strataIndex = 0;
int8_t arrowIndex = 0;
int8_t score = 0;
int8_t level = 0;
int8_t levelLen = 0;
int8_t levelCompleted = 0;
state gameState = begin;

char levelText[16];
char scoreText[16];

void nextStrata() {
    int nextIndex = furi_hal_random_get() % groups[groupIndex].stratagemCount;
    while(strataIndex == nextIndex) {
        nextIndex = furi_hal_random_get() % groups[groupIndex].stratagemCount;
    }
    strataIndex = nextIndex;
}

void nextLevel() {
    level++;
    levelLen = (furi_hal_random_get() % 4) + 4;
    levelCompleted = 0;
    totaltime -= ((float)totaltime * TIME_DECREASE_MULTIPLER);
    remainingtime = totaltime;

    int nextIndex = furi_hal_random_get() % 6;
    while(groupIndex == nextIndex) {
        nextIndex = furi_hal_random_get() % 6;
    }
    groupIndex = nextIndex;

    nextStrata();
}

void newGame() {
    totaltime = 2000;
    remainingtime = 2000;
    groupIndex = 0;
    strataIndex = 0;
    arrowIndex = 0;
    level = 0;
    levelLen = 0;
    levelCompleted = 0;
    nextLevel();
}

void initGroup(int groupID, const char* name) {
    memcpy(groups[groupID].name, name, strlen(name) + 1);
}

void initStratagem(int groupID, const char* name, const char* pattern) {
    group* gr = &groups[groupID];
    memcpy(gr->stratagems[gr->stratagemCount].name, name, strlen(name) + 1);
    memcpy(gr->stratagems[gr->stratagemCount].arrows, pattern, strlen(pattern) + 1);
    gr->stratagemCount++;
}

void initStratagems() {
    initGroup(0, "Mission");
    initStratagem(0, "Reinforce", "UDRLU");
    initStratagem(0, "SOS Beacon", "UDLR");
    initStratagem(0, "Resupply", "DDUR");
    initStratagem(0, "NUX-223 Hellbomb", "DULDURDU");
    initStratagem(0, "SSSD Delivery", "DDDUU");
    initStratagem(0, "Seismic Probe", "UULRDD");
    initStratagem(0, "Upload Data", "LRUUU");
    initStratagem(0, "Illumination Flare", "RRLL");
    initStratagem(0, "SEAF Artillery", "RUUD");
    initStratagem(0, "Super Earth Flag", "DUDU");

    initGroup(1, "Offensive: Eagle");
    initStratagem(1, "Eagle Rearm", "UULUR");
    initStratagem(1, "Eagle Strafing Run", "URR");
    initStratagem(1, "Eagle Airstrike", "URDR");
    initStratagem(1, "Eagle Cluster Bomb", "URDDR");
    initStratagem(1, "Eagle Napalm Strike", "URDU");
    initStratagem(1, "Eagle Smoke Strike", "URUD");
    initStratagem(1, "Eagle 110MM Rockets", "URUL");
    initStratagem(1, "Eagle 500kg Bomb", "URDDD");

    initGroup(2, "Supply: Weapons");
    initStratagem(2, "RL-77 Rocket Launcher", "DUULR");
    initStratagem(2, "AC-8 Autocannon", "DLDUUR");
    initStratagem(2, "EAT-17 Anti-Tank", "DDLUR");
    initStratagem(2, "FLAM-40 Flamethrower", "DLDRL");
    initStratagem(2, "LAS-98 Laser Cannon", "DLDUL");
    initStratagem(2, "M-105 Stalwart", "DLDUUL");
    initStratagem(2, "MG-43 Machine Gun", "DLDUR");
    initStratagem(2, "ARC-3 Arc Thrower", "DRDULL");
    initStratagem(2, "GL-21 Grenade Launcher", "DLULD");
    initStratagem(2, "APW-1 Anti-Mat Rifle", "DLRUD");
    initStratagem(2, "RS-422 Railgun", "DRDULR");
    initStratagem(2, "GR-8 Recoilless Rifle", "DLRRL");
    initStratagem(2, "FAF-14 Spear", "DDUDD");
    initStratagem(2, "LAS-99 Quasar Cannon", "DDULR");
    initStratagem(2, "MG-206 Machine Gun", "DLUDD");

    initGroup(3, "Offensive: Orbital");
    initStratagem(3, "O. Precision Strike", "RRU");
    initStratagem(3, "O. Airbust Strike", "RRR");
    initStratagem(3, "O. 120MM HE Barrage", "RDDLDRDD");
    initStratagem(3, "O. 380MM HE Barrage", "RDUULDD");
    initStratagem(3, "O. Walking Barrage", "RDRDRD");
    initStratagem(3, "Orbital Laser", "RDURD");
    initStratagem(3, "O. Railcannon Strike", "RUDDR");
    initStratagem(3, "O. Gatling Barrage", "RDLUU");
    initStratagem(3, "O. Gas Strike", "RRDR");
    initStratagem(3, "O. EMS Strike", "RRLD");
    initStratagem(3, "O. Smoke Strike", "RRDU");

    initGroup(4, "Supply: Extras");
    initStratagem(4, "AX/LAS-5 Rover", "DULURR");
    initStratagem(4, "AD-334 Guard Dog", "DULURD");
    initStratagem(4, "LIFT-850 Jump Pack", "DUUDU");
    initStratagem(4, "B-1 Supply Pack", "DUDDR");
    initStratagem(4, "SH-20 Generator", "DULRLR");
    initStratagem(4, "SH-20 Ballistic", "DLDDUL");
    initStratagem(4, "EXO-45 Exosuit", "LDRULDD");

    initGroup(5, "Defensive");
    initStratagem(5, "Machine Gun Sentry", "DURRU");
    initStratagem(5, "Mortar Sentry", "DURRD");
    initStratagem(5, "EMS Mortar Sentry", "DURDR");
    initStratagem(5, "Gatling Sentry", "DURL");
    initStratagem(5, "Autocannon Sentry", "DURULU");
    initStratagem(5, "Rocket Sentry", "DURRL");
    initStratagem(5, "Tesla Tower", "DURULR");
    initStratagem(5, "MD-6 Minefield", "DLUR");
    initStratagem(5, "MD-I4 Minefield", "DLLD");
    initStratagem(5, "FX-12 Generator", "DDLRLR");
    initStratagem(5, "E/MG-101 HMG", "DRLRRL");
}

void draw_stratagem(Canvas* canvas, uint8_t y) {
    int arrowSize = 14; //its square
    int x =
        64 - (((int)strlen(groups[groupIndex].stratagems[strataIndex].arrows) * arrowSize) / 2);
    y -= arrowSize / 2;

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(
        canvas, 64, y - (arrowSize * 0.5) - 12, AlignCenter, AlignCenter, groups[groupIndex].name);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(
        canvas,
        64,
        y - (arrowSize * 0.5),
        AlignCenter,
        AlignCenter,
        groups[groupIndex].stratagems[strataIndex].name);

    for(int i = 0; i < (int)strlen(groups[groupIndex].stratagems[strataIndex].arrows); i++) {
        switch(groups[groupIndex].stratagems[strataIndex].arrows[i]) {
        case 'U':
            if(arrowIndex > i) {
                canvas_draw_icon(canvas, x, y, &I_UpArrow_Filled_14x14);
            } else {
                canvas_draw_icon(canvas, x, y, &I_UpArrow_Empty_14x14);
            }
            break;

        case 'D':
            if(arrowIndex > i) {
                canvas_draw_icon(canvas, x, y, &I_DownArrow_Filled_14x14);
            } else {
                canvas_draw_icon(canvas, x, y, &I_DownArrow_Empty_14x14);
            }
            break;

        case 'L':
            if(arrowIndex > i) {
                canvas_draw_icon(canvas, x, y, &I_LeftArrow_Filled_14x14);
            } else {
                canvas_draw_icon(canvas, x, y, &I_LeftArrow_Empty_14x14);
            }
            break;

        case 'R':
            if(arrowIndex > i) {
                canvas_draw_icon(canvas, x, y, &I_RightArrow_Filled_14x14);
            } else {
                canvas_draw_icon(canvas, x, y, &I_RightArrow_Empty_14x14);
            }
            break;

        case 0x00:
            return;
        }
        x += arrowSize;
    }
}

void draw_timer_bar_center(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, float progress) {
    furi_assert(canvas);
    furi_assert((progress >= 0.0) && (progress <= 1.0));
    uint8_t height = 9;

    uint8_t progress_length = roundf(progress * (width - 2));

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x + 1, y + 1, width - 2, height - 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x, y, width, height);

    uint8_t midpoint = x + (width / 2);

    canvas_draw_box(canvas, midpoint - (progress_length / 2), y + 1, progress_length, height - 2);
}

void draw_timer_bar(Canvas* canvas, uint8_t x, uint8_t y, uint8_t width, float progress) {
    furi_assert(canvas);
    furi_assert((progress >= 0.0) && (progress <= 1.0));
    uint8_t height = 9;

    uint8_t progress_length = roundf(progress * (width - 2));

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, x + 1, y + 1, width - 2, height - 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_frame(canvas, x, y, width, height);

    canvas_draw_box(canvas, x + 1, y + 1, progress_length, height - 2);
}

static void draw_callback(Canvas* canvas, void* ctx) {
    UNUSED(ctx);

    canvas_clear(canvas);

    if(gameState == active || gameState == begin) {
        draw_stratagem(canvas, 40);
        float remPerc = (float)((float)remainingtime / (float)totaltime);
        draw_timer_bar(canvas, 2, 53, 124, remPerc);
    }
    if(gameState == end) {
        snprintf(levelText, 16, "Level: %i", level);
        snprintf(scoreText, 16, "Score: %i", score);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 64, 20, AlignCenter, AlignCenter, "Game Over");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 64, 34, AlignCenter, AlignCenter, levelText);
        canvas_draw_str_aligned(canvas, 64, 44, AlignCenter, AlignCenter, scoreText);
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, 100);
}

static void timer_callback(void* ctx) {
    furi_assert(ctx);
    UNUSED(ctx);

    if(gameState == begin || gameState == end) {
        return;
    }

    if(remainingtime <= 0) {
        remainingtime = 0;
        gameState = end;
        notification_message(notif, &sequence_error);
        return;
    }
    remainingtime--;
}

static void handleKeyPress(InputKey key) {
    bool fail = false;

    if(gameState == end) {
        gameState = begin;
        newGame();
        return;
    } else if(gameState == begin) {
        gameState = active;
    }

    if(key == InputKeyUp) {
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'U') {
            arrowIndex++;
        } else {
            fail = true;
        }
    }
    if(key == InputKeyDown) {
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'D') {
            arrowIndex++;
        } else {
            fail = true;
        }
    }
    if(key == InputKeyLeft) {
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'L') {
            arrowIndex++;
        } else {
            fail = true;
        }
    }
    if(key == InputKeyRight) {
        if(groups[groupIndex].stratagems[strataIndex].arrows[arrowIndex] == 'R') {
            arrowIndex++;
        } else {
            fail = true;
        }
    }

    if(fail) {
        remainingtime -= ((float)totaltime * TIME_PENALTY_MULTIPLIER);
        ;
        arrowIndex = 0;
        notification_message(notif, &sequence_single_vibro);
    }

    if(arrowIndex == ((int)strlen(groups[groupIndex].stratagems[strataIndex].arrows))) {
        arrowIndex = 0;
        remainingtime += ((float)totaltime * TIME_REWARD_MULTIPLIER);
        if(remainingtime > totaltime) {
            remainingtime = totaltime;
        }
        levelCompleted++;

        if(levelCompleted == levelLen) {
            nextLevel();
            notification_message(notif, &sequence_double_vibro);
        } else {
            nextStrata();
            score++;
        }
    }
}

int32_t stratagem_app(void* p) {
    UNUSED(p);

    furi_hal_random_init();
    initStratagems();

    InputEvent event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, NULL);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, NULL);
    furi_timer_start(timer, (uint32_t)furi_kernel_get_tick_frequency() / 100);

    notif = furi_record_open(RECORD_NOTIFICATION);
    notification_message(notif, &sequence_display_backlight_on);

    bool running = true;

    view_port_update(view_port);

    newGame();

    while(running) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        view_port_update(view_port);
        if(!(event_status == FuriStatusOk)) {
            continue;
        }
        if(event.type == InputTypePress) {
            if(event.key == InputKeyUp || event.key == InputKeyDown || event.key == InputKeyLeft ||
               event.key == InputKeyRight || event.key == InputKeyOk) {
                handleKeyPress(event.key);
            }
            if(event.key == InputKeyBack) {
                running = false;
            }
        }
    }

    furi_timer_stop(timer);
    furi_timer_free(timer);
    furi_message_queue_free(event_queue);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    return 0;
}
