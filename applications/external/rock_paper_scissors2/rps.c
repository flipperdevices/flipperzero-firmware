#include <stdlib.h>
#include <stdio.h>

#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>

#include "rps.h"
#include "rps_icons.h"

#define QUEUE_SIZE 8
#define LOG_TAG "RPS"

// ##############
// GAME LOGIC
void set_game_state(RPSApp_t* app, GameState_t game_state) {
    switch(game_state) {
    case GameStateUserSelect:
        app->game_state = GameStateUserSelect;
        app->system_hand_box = HandBoxTypeNormal;
        app->user_hand_box = HandBoxTypeSelected;
        app->click_message = "compete now!";
        break;
    case GameStateSystemRoll:
        app->game_state = GameStateSystemRoll;
        app->system_hand_box = HandBoxTypeSelected;
        app->user_hand_box = HandBoxTypeNormal;
        app->click_message = NULL;
        break;
    case GameStateEnded:
        app->game_state = GameStateEnded;
        app->system_hand_box = HandBoxTypeNormal;
        app->user_hand_box = HandBoxTypeNormal;
        app->click_message = "play again?";
        break;
    default:
        app->game_state = GameStateNone;
        app->system_hand_box = HandBoxTypeNormal;
        app->user_hand_box = HandBoxTypeNormal;
        app->click_message = NULL;
        break;
    }
}

void start_game(RPSApp_t* app) {
    set_game_state(app, GameStateUserSelect);
    app->system_hand = HandTypeEmpty;
    app->middle_message = "<- VS ->";
    view_port_update(app->view_port);
}

GameResult_t calculate_game_result(RPSApp_t* app) {
    if(app->user_hand == HandTypeEmpty || app->system_hand == HandTypeEmpty) {
        return GameResultUnknown;
    }
    if(app->user_hand == app->system_hand) {
        return GameResultDraw;
    }
    if((app->user_hand == HandTypeScissor && app->system_hand == HandTypePaper) ||
       (app->user_hand == HandTypePaper && app->system_hand == HandTypeRock) ||
       (app->user_hand == HandTypeRock && app->system_hand == HandTypeScissor)) {
        app->user_score += 1;
        return GameResultWon;
    }
    app->system_score += 1;
    return GameResultLost;
}

// ##############
// INPUT
void input_callback(InputEvent* input_event, void* ctx) {
    FURI_LOG_D(LOG_TAG, "Input callback fired!");
    RPSApp_t* app = ctx;
    if(input_event->type == InputTypeShort) {
        furi_message_queue_put(app->event_queue, input_event, 0);
    }
}

// ##############
// RENDER
Icon* get_icon_for_hand(HandType_t hand) {
    switch(hand) {
    case HandTypeRock:
        return &I_rock;
    case HandTypePaper:
        return &I_paper;
    case HandTypeScissor:
        return &I_scissor;
    case HandTypeEmpty:
        return &I_empty;
    default:
        return NULL;
    }
}

void rpselements_hand_box(
    Canvas* canvas,
    int x,
    int y,
    HandType_t hand,
    HandBoxType_t hand_box_type) {
    Icon* icon = get_icon_for_hand(hand);

    switch(hand_box_type) {
    case HandBoxTypeFilled:
        canvas_draw_rbox(canvas, x, y, 28, 28, 8);
        canvas_draw_rframe(canvas, x - 1, y - 1, 30, 30, 8);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_icon(canvas, x + 4, y + 4, icon);
        canvas_set_color(canvas, ColorBlack);
        break;

    case HandBoxTypeSelected:
        canvas_draw_rframe(canvas, x, y, 28, 28, 8);
        canvas_draw_rframe(canvas, x - 1, y - 1, 30, 30, 8);
        canvas_draw_icon(canvas, x + 4, y + 4, icon);
        break;

    case HandBoxTypeNormal:
    default:
        canvas_draw_rframe(canvas, x, y, 28, 28, 8);
        canvas_draw_icon(canvas, x + 4, y + 4, icon);
        break;
    }
}

void render_callback(Canvas* canvas, void* ctx) {
    FURI_LOG_D(LOG_TAG, "Render callback fired!");
    RPSApp_t* app = ctx;
    canvas_clear(canvas);

    char score_str[64];
    snprintf(score_str, 64, "[%d  |  %d]", app->system_score, app->user_score);

    canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, score_str);

    canvas_draw_str_aligned(canvas, 24, 12, AlignCenter, AlignCenter, "Bot");
    canvas_draw_str_aligned(canvas, 104, 12, AlignCenter, AlignCenter, "You");
    canvas_draw_str_aligned(canvas, 64, 32, AlignCenter, AlignCenter, app->middle_message);

    rpselements_hand_box(canvas, 10, 18, app->system_hand, app->system_hand_box);
    rpselements_hand_box(canvas, 90, 18, app->user_hand, app->user_hand_box);

    if(app->click_message != NULL) {
        elements_button_center(canvas, app->click_message);
    }
}

// ##############
// ROLL
void roll_callback(void* ctx) {
    RPSApp_t* app = ctx;
    app->system_hand = (app->system_hand + 1) % HandTypeEmpty;
    view_port_update(app->view_port);
}

// ##############
// ROLL COMPLETE
void roll_complete_callback(void* ctx) {
    FURI_LOG_D(LOG_TAG, "Roll complete callback fired!");
    RPSApp_t* app = ctx;
    furi_timer_stop(app->roll_timer);

    set_game_state(app, GameStateEnded);

    GameResult_t game_result = calculate_game_result(app);
    switch(game_result) {
    case GameResultDraw:
        app->middle_message = "DRAW...";
        break;

    case GameResultWon:
        app->middle_message = "YOU WON!";
        app->user_hand_box = HandBoxTypeFilled;
        break;

    case GameResultLost:
        app->middle_message = "YOU LOST!";
        app->system_hand_box = HandBoxTypeFilled;
        break;

    default:
        app->middle_message = "Huh?";
        break;
    }

    view_port_update(app->view_port);
}

// ##############
// EVENT HANDLE
void user_hand_up(RPSApp_t* app) {
    app->user_hand = (app->user_hand + 1) % HandTypeEmpty;
    view_port_update(app->view_port);
}

void user_hand_down(RPSApp_t* app) {
    app->user_hand = app->user_hand - 1;
    if(app->user_hand > HandTypeEmpty) {
        app->user_hand = HandTypeEmpty - 1;
    }
    view_port_update(app->view_port);
}

void system_roll_start(RPSApp_t* app) {
    if(app->user_hand == HandTypeEmpty) {
        return;
    }

    int hertz = furi_kernel_get_tick_frequency();
    int roll_ticks = hertz + (rand() % (hertz * 2));

    set_game_state(app, GameStateSystemRoll);

    furi_timer_start(app->roll_timer, 100);
    furi_timer_start(app->roll_complete_timer, roll_ticks);
}

bool main_loop(RPSApp_t* app) {
    InputEvent event;
    FuriStatus status = FuriStatusErrorTimeout;
    while((status = furi_message_queue_get(app->event_queue, &event, 60000)) ==
          FuriStatusErrorTimeout)
        ;

    furi_check(furi_mutex_acquire(app->mutex, FuriWaitForever) == FuriStatusOk);

    if(event.key == InputKeyBack) {
        FURI_LOG_D(LOG_TAG, "exit");
        furi_mutex_release(app->mutex);
        return false;
    }

    else if(event.key == InputKeyUp && app->game_state == GameStateUserSelect) {
        user_hand_up(app);
    }

    else if(event.key == InputKeyDown && app->game_state == GameStateUserSelect) {
        user_hand_down(app);
    }

    else if(event.key == InputKeyOk && app->game_state == GameStateUserSelect) {
        system_roll_start(app);
    }

    else if(event.key == InputKeyOk && app->game_state == GameStateEnded) {
        start_game(app);
    }

    furi_mutex_release(app->mutex);
    return true;
}

// ##############
// STATE
RPSApp_t* state_init() {
    FURI_LOG_D(LOG_TAG, "Initing app...");
    RPSApp_t* app = malloc(sizeof(RPSApp_t));

    app->event_queue = furi_message_queue_alloc(QUEUE_SIZE, sizeof(InputEvent));
    app->view_port = view_port_alloc();
    app->gui = furi_record_open(RECORD_GUI);
    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    app->roll_timer = furi_timer_alloc(roll_callback, FuriTimerTypePeriodic, app);
    app->roll_complete_timer = furi_timer_alloc(roll_complete_callback, FuriTimerTypeOnce, app);

    view_port_input_callback_set(app->view_port, input_callback, app);
    view_port_draw_callback_set(app->view_port, render_callback, app);
    gui_add_view_port(app->gui, app->view_port, GuiLayerFullscreen);

    app->user_hand = HandTypeEmpty;
    app->system_hand = HandTypeEmpty;
    app->game_state = GameStateNone;
    app->system_hand_box = HandBoxTypeNormal;
    app->user_hand_box = HandBoxTypeNormal;
    app->click_message = NULL;
    app->middle_message = NULL;

    start_game(app);

    return app;
}

void state_free(RPSApp_t* app) {
    FURI_LOG_D(LOG_TAG, "Freeing app...");
    furi_message_queue_free(app->event_queue);
    gui_remove_view_port(app->gui, app->view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(app->view_port);
    furi_mutex_free(app->mutex);
    furi_timer_free(app->roll_timer);
    furi_timer_free(app->roll_complete_timer);
    free(app);
}

// ##############
// ENTRY
int32_t rps_app(void) {
    RPSApp_t* app = state_init();
    while(main_loop(app))
        ;
    state_free(app);
    return 0;
}
