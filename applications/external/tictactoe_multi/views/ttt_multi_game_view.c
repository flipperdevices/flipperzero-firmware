#include "ttt_multi_game_view.h"

struct TttMultiGameView {
    View* view;
};

typedef struct {
    TttMultiGame* game;
    uint8_t position_x;
    uint8_t position_y;

    TttMultiGamePlayer local_player;
    bool is_local;

    TttMultiGameViewCallback callback;
    void* context;

    TttMultiGameMove* last_move;
    TttMultiGameResult result;
} TttMultiGameViewModel;

void ttt_multi_game_view_draw_callback(Canvas* canvas, void* _model) {
    TttMultiGameViewModel* model = _model;

    canvas_clear(canvas);
    const uint8_t top = 2;
    const uint8_t left = 4;
    const uint8_t cell_size = 20;
    const uint8_t padding = 4;

    canvas_draw_frame(canvas, left - 1, top - 1, 3 * cell_size + 2, 3 * cell_size + 2);

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            canvas_draw_frame(
                canvas, left + i * cell_size, top + j * cell_size, cell_size, cell_size);
        }
    }

    for(uint8_t i = 0; i < 3; i++) {
        for(uint8_t j = 0; j < 3; j++) {
            bool is_selected = model->position_x == i && model->position_y == j &&
                               ttt_multi_game_get_state(model->game) != TttMultiGameStateFinished;

            if(ttt_multi_game_player_at(model->game, i, j) == TttMultiGamePlayerX) {
                canvas_draw_line(
                    canvas,
                    left + i * cell_size + padding,
                    top + j * cell_size + padding,
                    left + i * cell_size + cell_size - padding,
                    top + j * cell_size + cell_size - padding);
                canvas_draw_line(
                    canvas,
                    left + i * cell_size + cell_size - padding,
                    top + j * cell_size + padding,
                    left + i * cell_size + padding,
                    top + j * cell_size + cell_size - padding);
            } else if(ttt_multi_game_player_at(model->game, i, j) == TttMultiGamePlayerO) {
                canvas_draw_circle(
                    canvas,
                    left + i * cell_size + cell_size / 2,
                    top + j * cell_size + cell_size / 2,
                    cell_size / 2 - padding);
            }

            if(is_selected) {
                canvas_draw_frame(
                    canvas,
                    left + i * cell_size + 1,
                    top + j * cell_size + 1,
                    cell_size - 2,
                    cell_size - 2);
            }
        }
    }

    char* status_str = "";
    // TODO: Rewrite this
    if(model->is_local) {
        if(ttt_multi_game_get_state(model->game) == TttMultiGameStateFinished) {
            if(ttt_multi_game_get_result(model->game) == TttMultiGameResultXWin) {
                status_str = "X win";
            } else if(ttt_multi_game_get_result(model->game) == TttMultiGameResultOWin) {
                status_str = "O win";
            } else if(ttt_multi_game_get_result(model->game) == TttMultiGameResultDraw) {
                status_str = "Draw";
            }
        } else {
            if(ttt_multi_game_current_player(model->game) == TttMultiGamePlayerX) {
                status_str = "X turn";
            } else {
                status_str = "O turn";
            }
        }
    } else {
        if(ttt_multi_game_get_state(model->game) == TttMultiGameStateFinished) {
            if(model->local_player == TttMultiGamePlayerX) {
                if(ttt_multi_game_get_result(model->game) == TttMultiGameResultXWin) {
                    status_str = "You win";
                } else if(ttt_multi_game_get_result(model->game) == TttMultiGameResultOWin) {
                    status_str = "You lose";
                } else if(ttt_multi_game_get_result(model->game) == TttMultiGameResultDraw) {
                    status_str = "Draw";
                }
            } else {
                if(ttt_multi_game_get_result(model->game) == TttMultiGameResultXWin) {
                    status_str = "You lose";
                } else if(ttt_multi_game_get_result(model->game) == TttMultiGameResultOWin) {
                    status_str = "You win";
                } else if(ttt_multi_game_get_result(model->game) == TttMultiGameResultDraw) {
                    status_str = "Draw";
                }
            }
        } else {
            if(model->local_player == TttMultiGamePlayerX) {
                if(ttt_multi_game_get_state(model->game) == TttMultiGameStateTurnX) {
                    status_str = "Your turn";
                } else {
                    status_str = "Opponent turn";
                }
            } else {
                if(ttt_multi_game_get_state(model->game) == TttMultiGameStateTurnO) {
                    status_str = "Your turn";
                } else {
                    status_str = "Opponent turn";
                }
            }
        }
    }

    elements_text_box(canvas, 64, 16, 64, 16, AlignCenter, AlignCenter, status_str, false);
    if(ttt_multi_game_get_state(model->game) == TttMultiGameStateFinished) {
        elements_multiline_text_aligned(
            canvas, 100, 32, AlignCenter, AlignTop, "Press OK\nto restart");
    }
}

static void ttt_multi_game_view_process_left(TttMultiGameView* view);
static void ttt_multi_game_view_process_right(TttMultiGameView* view);
static void ttt_multi_game_view_process_up(TttMultiGameView* view);
static void ttt_multi_game_view_process_down(TttMultiGameView* view);
static void ttt_multi_game_view_process_ok(TttMultiGameView* view);

static bool ttt_multi_game_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    TttMultiGameView* game_view = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        switch(event->key) {
        case InputKeyLeft:
            ttt_multi_game_view_process_left(game_view);
            consumed = true;
            break;
        case InputKeyRight:
            ttt_multi_game_view_process_right(game_view);
            consumed = true;
            break;
        case InputKeyUp:
            ttt_multi_game_view_process_up(game_view);
            consumed = true;
            break;
        case InputKeyDown:
            ttt_multi_game_view_process_down(game_view);
            consumed = true;
            break;
        case InputKeyOk:
            ttt_multi_game_view_process_ok(game_view);
            consumed = true;
            break;
        default:
            break;
        }
    }

    return consumed;
}

void ttt_multi_game_view_model_reset(TttMultiGameViewModel* model) {
    furi_assert(model);
    furi_assert(model->game);

    model->position_x = 1;
    model->position_y = 1;
    ttt_multi_game_reset(model->game);
}

TttMultiGameView* ttt_multi_game_view_alloc() {
    TttMultiGameView* game_view = malloc(sizeof(TttMultiGameView));
    game_view->view = view_alloc();
    view_set_context(game_view->view, game_view);
    view_allocate_model(game_view->view, ViewModelTypeLocking, sizeof(TttMultiGameViewModel));
    view_set_draw_callback(game_view->view, (ViewDrawCallback)ttt_multi_game_view_draw_callback);
    view_set_input_callback(game_view->view, ttt_multi_game_view_input_callback);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            model->game = ttt_multi_game_alloc();
            model->last_move = ttt_multi_game_move_alloc();
            ttt_multi_game_view_model_reset(model);
        },
        true);

    return game_view;
}

void ttt_multi_game_view_free(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            ttt_multi_game_free(model->game);
            ttt_multi_game_move_free(model->last_move);
        },
        true);

    view_free(game_view->view);
    free(game_view);
}

View* ttt_multi_game_get_view(TttMultiGameView* game_view) {
    furi_assert(game_view);

    return game_view->view;
}

void ttt_multi_game_view_process_up(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_y > 0) {
                model->position_y--;
            }
        },
        true);
}

void ttt_multi_game_view_process_down(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_y < 2) {
                model->position_y++;
            }
        },
        true);
}

void ttt_multi_game_view_process_left(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_x > 0) {
                model->position_x--;
            }
        },
        true);
}

void ttt_multi_game_view_process_right(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->position_x < 2) {
                model->position_x++;
            }
        },
        true);
}

void ttt_multi_game_view_process_ok(TttMultiGameView* game_view) {
    furi_assert(game_view);
    TttMultiGameViewCallback callback = NULL;
    void* context = NULL;

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(ttt_multi_game_get_state(model->game) == TttMultiGameStateFinished) {
                ttt_multi_game_view_model_reset(model);
            } else {
                model->last_move->player = ttt_multi_game_current_player(model->game);
                model->last_move->x = model->position_x;
                model->last_move->y = model->position_y;

                if(ttt_multi_game_is_move_valid(model->game, model->last_move)) {
                    callback = model->callback;
                    context = model->context;
                }
            }
        },
        true);

    if(callback) {
        callback(context, TttMultiCustomEventGameMove);
    }
}

void ttt_multi_game_view_move(TttMultiGameView* game_view, TttMultiGameMove* move) {
    furi_assert(game_view);
    furi_assert(move);

    TttMultiGameResult result;
    TttMultiGameViewCallback callback = NULL;
    void* context = NULL;

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            ttt_multi_game_make_move(model->game, move);
            result = ttt_multi_game_get_result(model->game);
            if(result != TttMultiGameResultNone) {
                callback = model->callback;
                context = model->context;
            }
        },
        true);

    if(callback) {
        callback(context, TttMultiCustomEventGameFinish);
    }
}

void ttt_multi_game_view_set_callback(
    TttMultiGameView* game_view,
    TttMultiGameViewCallback callback,
    void* context) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            model->callback = callback;
            model->context = context;
        },
        true);
}

void ttt_multi_game_view_set_remote_play(TttMultiGameView* game_view, TttMultiGamePlayer player) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            model->local_player = player;
            model->is_local = false;
        },
        true);
}

void ttt_multi_game_view_set_local_play(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view, TttMultiGameViewModel * model, { model->is_local = true; }, true);
}

void ttt_multi_game_view_get_last_move(TttMultiGameView* game_view, TttMultiGameMove* move) {
    furi_assert(game_view);
    furi_assert(move);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        {
            if(model->last_move->player != TttMultiGamePlayerNone) {
                ttt_multi_game_move_copy(move, model->last_move);
            }
        },
        true);
}

TttMultiGameResult ttt_multi_game_view_get_result(TttMultiGameView* game_view) {
    furi_assert(game_view);

    TttMultiGameResult result = TttMultiGameResultNone;

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        { result = ttt_multi_game_get_result(model->game); },
        true);

    return result;
}

void ttt_multi_game_view_reset(TttMultiGameView* game_view) {
    furi_assert(game_view);

    with_view_model(
        game_view->view,
        TttMultiGameViewModel * model,
        { ttt_multi_game_view_model_reset(model); },
        true);
}