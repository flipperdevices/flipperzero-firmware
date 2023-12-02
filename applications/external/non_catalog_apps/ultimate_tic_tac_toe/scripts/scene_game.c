#include "scene_game.h"
#include "app.h"
#include "game.h"
#include "app_gameplay.h"
#include "scene_management.h"
#include "game_ai.h"
#include <gui/gui.h>

void draw_single_board(
    Canvas* const canvas,
    GameState* game,
    int boardIndex,
    int x,
    int y,
    bool isNextBoard,
    int selectedCell) {
    canvas_set_color(canvas, ColorBlack);

    // Big X
    if(game_get_board_winner(game, boardIndex) == BoardWinner_X) {
        canvas_draw_line(canvas, x + 1, y + 1, x + 15, y + 15);
        canvas_draw_line(canvas, x + 1, y + 15, x + 15, y + 1);
        return;
    }

    // Big O
    if(game_get_board_winner(game, boardIndex) == BoardWinner_O) {
        canvas_draw_circle(canvas, x + 8, y + 8, 7);
        return;
    }

    // Horizontal lines
    canvas_draw_line(canvas, x, y + 5, x + 16, y + 5);
    canvas_draw_line(canvas, x, y + 11, x + 16, y + 11);
    // Vertical lines
    canvas_draw_line(canvas, x + 5, y, x + 5, y + 16);
    canvas_draw_line(canvas, x + 11, y, x + 11, y + 16);

    for(int cellIndex = 0; cellIndex < 9; cellIndex++) {
        int cellPositionX = (cellIndex % 3) * 6 + x + 1;
        int cellPositionY = (cellIndex / 3) * 6 + y + 1;
        int cellValue = game_get_cell(game, boardIndex, cellIndex);

        if(cellValue == CellState_X) {
            canvas_draw_line(
                canvas, cellPositionX, cellPositionY, cellPositionX + 2, cellPositionY + 2);
            canvas_draw_line(
                canvas, cellPositionX, cellPositionY + 2, cellPositionX + 2, cellPositionY);
        } else if(cellValue == CellState_O) {
            canvas_draw_circle(canvas, cellPositionX + 1, cellPositionY + 1, 1);
        } else if(cellIndex == selectedCell) {
            canvas_draw_box(canvas, cellPositionX, cellPositionY, 3, 3);
        } else if(isNextBoard) {
            canvas_draw_dot(canvas, cellPositionX + 1, cellPositionY + 1);
        }
    }
}

void draw_board(Canvas* const canvas, AppGameplayState* gameplay) {
    GameState* game = gameplay_get_game(gameplay);
    int selectionBoardIndex, selectionCellIndex;
    gameplay_selection_get(gameplay, &selectionBoardIndex, &selectionCellIndex);

    canvas_set_color(canvas, ColorBlack);

    // Big horizontal lines
    canvas_draw_line(canvas, 0, 20, 62, 20);
    canvas_draw_line(canvas, 0, 21, 62, 21);
    canvas_draw_line(canvas, 0, 41, 62, 41);
    canvas_draw_line(canvas, 0, 42, 62, 42);
    // Big vertical lines
    canvas_draw_line(canvas, 20, 0, 20, 62);
    canvas_draw_line(canvas, 21, 0, 21, 62);
    canvas_draw_line(canvas, 41, 0, 41, 62);
    canvas_draw_line(canvas, 42, 0, 42, 62);

    // Boards
    for(int i = 0; i < 9; i++) {
        int x = (i % 3) * 21 + 2;
        int y = (i / 3) * 21 + 2;

        bool isNextBoard = game_get_winner(game) == BoardWinner_TBD &&
                           (game_get_next_board(game) == -1 || game_get_next_board(game) == i);
        int selectedCell = game_get_winner(game) == BoardWinner_TBD && i == selectionBoardIndex ?
                               selectionCellIndex :
                               -1;
        draw_single_board(canvas, game, i, x, y, isNextBoard, selectedCell);
    }
}

void draw_status(Canvas* const canvas, GameState* game) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);

    const int statusX = 96;
    const int statusY = 32;

    if(game_get_winner(game) == BoardWinner_X) {
        for(int i = -3; i <= 3; i++) {
            canvas_draw_line(
                canvas, statusX - 20 + i, statusY - 20, statusX + 20 + i, statusY + 20);
            canvas_draw_line(
                canvas, statusX - 20 + i, statusY + 20, statusX + 20 + i, statusY - 20);
        }
    } else if(game_get_winner(game) == BoardWinner_O) {
        canvas_draw_disc(canvas, statusX, statusY, 23);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_disc(canvas, statusX, statusY, 17);
        canvas_set_color(canvas, ColorBlack);
    } else if(game_get_winner(game) == BoardWinner_Draw) {
        for(int i = -3; i <= 3; i++) {
            canvas_draw_line(canvas, statusX - 20, statusY - 8 + i, statusX + 20, statusY - 8 + i);
            canvas_draw_line(canvas, statusX - 20, statusY + 8 + i, statusX + 20, statusY + 8 + i);
        }
    } else if(game_get_player_turn(game) == PlayerTurn_X) {
        canvas_draw_line(canvas, statusX - 10, statusY - 10, statusX + 10, statusY + 10);
        canvas_draw_line(canvas, statusX - 10, statusY + 10, statusX + 10, statusY - 10);
    } else if(game_get_player_turn(game) == PlayerTurn_O) {
        canvas_draw_circle(canvas, statusX, statusY, 10);
    }
}

void game_transition_callback(int from, int to, void* context) {
    AppContext* app = (AppContext*)context;
    UNUSED(from);

    if(to == SceneType_Game) gameplay_reset(app->gameplay);
}

void game_render_callback(Canvas* const canvas, void* context) {
    AppContext* app = (AppContext*)context;
    AppGameplayState* gameplay = app->gameplay;
    canvas_clear(canvas);
    draw_board(canvas, gameplay);
    draw_status(canvas, gameplay_get_game(gameplay));
}

void game_handle_input(InputKey key, InputType type, void* context) {
    AppContext* app = (AppContext*)context;

    if(key == InputKeyBack && type == InputTypePress) {
        scene_manager_set_scene(app->sceneManager, SceneType_Menu);
        return;
    }

    if(game_get_winner(gameplay_get_game(app->gameplay)) != BoardWinner_TBD) return;

    if(gameplay_get_next_player_type(app->gameplay) != PlayerType_Human) return;

    if(key == InputKeyOk && type == InputTypePress) {
        gameplay_selection_perform_current(app->gameplay);
        return;
    }

    if(type != InputTypePress && type != InputTypeRepeat) return;

    if(key == InputKeyLeft) gameplay_selection_handle_delta(app->gameplay, -1, 0);

    if(key == InputKeyRight) gameplay_selection_handle_delta(app->gameplay, 1, 0);

    if(key == InputKeyUp) gameplay_selection_handle_delta(app->gameplay, 0, -1);

    if(key == InputKeyDown) gameplay_selection_handle_delta(app->gameplay, 0, 1);
}

void game_tick_callback(void* context) {
    AppContext* app = (AppContext*)context;
    game_ai_run(app->gameplay);
}