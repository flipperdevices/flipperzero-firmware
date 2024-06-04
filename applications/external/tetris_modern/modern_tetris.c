/*
    This is a fork from https://github.com/xMasterX/all-the-plugins/blob/dev/base_pack/tetris_game/tetris_game.c
    It was modified to follow the official Tetris Guidelines
    (described in https://tetris.wiki/Tetris_Guideline)
    The following features have been added :
        - counter clockwise rotation [done]
        - 5 pieces preview [done]
        - hold feature [done]
        - lock delay with move delay limit [done, needs adjustements]
        - advanced tspin detection
        - perfect clear detection (also called all clear)
        - b2b chains
    The following changes were made :
        - clockwise rotation moved to the 'up' button
        - hold center button to pause the game
        - fixed the rotation system
        - according to the official guidelines, "Tetrominoes appear on the 21st and 
        22nd rows of the playfield, centered and rounded to the left when needed."
        Previously, the pieces were centered to the RIGHT and spawned on the 20th row.
    The following bugs were fixed :
        - the 7 bag randomizer is now reset after a game over.
        - pressing any button no longer reset the game when its paused

    If someone has enough skill, a local connection 1v1 (or multiplayer) version can be created,
    following the traditionnal garbage sending system (the official fighting system).
*/

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <furi_hal_resources.h>
#include <furi_hal_gpio.h>
#include <dolphin/dolphin.h>
#include "tetris_modern_icons.h"

#define BORDER_OFFSET 1
#define MARGIN_OFFSET 3

#define BLOCK_HEIGHT 6
#define BLOCK_WIDTH 6

#define SMALL_BLOCK_HEIGHT 4
#define SMALL_BLOCK_WIDTH 4

#define FIELD_WIDTH 10
#define FIELD_HEIGHT 22

#define FIELD_X_OFFSET 3
#define FIELD_Y_OFFSET 20

#define MAX_FALL_SPEED 500
#define MIN_FALL_SPEED 100

typedef struct Point {
    // Also used for offset data, which is sometimes negative
    int8_t x, y;
} Point;

typedef enum { OffsetTypeCommon, OffsetTypeI, OffsetTypeO } OffsetType;

typedef enum { Clockwise, Counter_clockwise } RotationType;

static const Point SRS_kick_table[4][3][5] = {
    // SRS kick table. Changed the order since [desired][current] was so confusing
    // [currentRotation][desiredRotation][kick]
    {
        // Spawn state
        {{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}, // to right
        {{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}, // to left
    },
    {
        // Right (from spawn)
        {{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}, // to spawn
        {{0, 0}, {+1, 0}, {+1, -1}, {0, +2}, {+1, +2}}, // to 180
    },
    {
        // 180 (from spawn)
        {{0, 0}, {-1, 0}, {-1, +1}, {0, -2}, {-1, -2}}, // to right
        {{0, 0}, {+1, 0}, {+1, +1}, {0, -2}, {+1, -2}}, // to left
    },
    {
        // Left (from spawn)
        {{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}, // to 180
        {{0, 0}, {-1, 0}, {-1, -1}, {0, +2}, {-1, +2}}, // to spawn
    }};

static const Point SRS_I_kick_table[4][3][5] = {
    {
        // Spawn state
        {{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}}, // to right
        {{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}}, // to left
    },
    {
        // Right (from spawn)
        {{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}}, // to spawn
        {{0, 0}, {-1, 0}, {+2, 0}, {-1, +2}, {+2, -1}}, // to 180
    },
    {
        // 180 (from spawn)
        {{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}}, // to right
        {{0, 0}, {+2, 0}, {-1, 0}, {+2, +1}, {-1, -2}}, // to left
    },
    {
        // Left (from spawn)
        {{0, 0}, {-2, 0}, {+1, 0}, {-2, -1}, {+1, +2}}, // to 180
        {{0, 0}, {+1, 0}, {-2, 0}, {+1, -2}, {-2, +1}}, // to spawn
    }};
// O-spin is underrated. (joke)

typedef enum {
    NoRotation,
    NormalRotation,
    T_Mec_Rotation,
    T_TST_Rotation,
    T_Fin_Rotation,
    T_Neo_Rotation,
    T_Iso_Rotation,
    Iso_Neo_kick,
    Upgrade_Rotation,
} LastRotation;

typedef enum {
    Nothing,
    Single,
    Double,
    Triple,
    Tetris,
    TS0,
    TSS,
    TSD,
    TST,
    M_TS0,
    M_TSS,
    I_TSD,
    F_TSD,
    N_M_TSD,
} LastAction;

typedef enum {
    NoTspin,
    MiniTspin,
    FullTspin,
} T_Spin_Type;

typedef struct {
    Point p[4];
    uint8_t rotIdx;
    OffsetType offsetType;
} Piece;

typedef enum {
    Z_piece,
    S_piece,
    L_piece,
    J_piece,
    T_piece,
    I_piece,
    O_piece,

    // when there's no held piece, especially when the game starts.
    hold_empty_value
} Piece_name;

typedef enum { GameStatePlaying, GameStateGameOver, GameStatePaused } GameState;

static Piece shapes[] = {
    // Shapes @ spawn locations, rotation point first
    {.p = {{4, 1}, {3, 0}, {4, 0}, {5, 1}}, .rotIdx = 0, .offsetType = OffsetTypeCommon}, // Z
    {.p = {{4, 1}, {3, 1}, {4, 0}, {5, 0}}, .rotIdx = 0, .offsetType = OffsetTypeCommon}, // S
    {.p = {{4, 1}, {3, 1}, {5, 1}, {5, 0}}, .rotIdx = 0, .offsetType = OffsetTypeCommon}, // L
    {.p = {{4, 1}, {3, 0}, {3, 1}, {5, 1}}, .rotIdx = 0, .offsetType = OffsetTypeCommon}, // J
    {.p = {{4, 1}, {3, 1}, {4, 0}, {5, 1}}, .rotIdx = 0, .offsetType = OffsetTypeCommon}, // T
    {.p = {{4, 1}, {3, 1}, {5, 1}, {6, 1}}, .rotIdx = 0, .offsetType = OffsetTypeI}, // I
    {.p = {{4, 1}, {4, 0}, {5, 0}, {5, 1}}, .rotIdx = 0, .offsetType = OffsetTypeO} // O
};

typedef struct {
    // representation of the current board
    bool playField[FIELD_HEIGHT][FIELD_WIDTH];
    // the 7-bag current state (remaining pieces, previews NOT included)
    bool bag[7];
    // First of the 5 previews, drawed as a normal piece (only the letter appears of the others)
    Piece_name next_id;
    // 5 previews like on PPT2 (official Tetris game), Tetr.io, JStris and four-tris (for a better blitz/ultra/pc experience)
    uint8_t next_previews[4];
    // The current falling piece
    Piece currPiece;
    // The current falling piece's id
    Piece_name currPiece_id;
    // The held piece
    Piece_name holdPiece_id;
    // the hold feature can be used only once per piece
    bool used_hold;
    // is the piece at the bottom waiting to be locked; if false -> true, if true -> locks the piece
    // so the piece has an extra frame before being locked; according to the guidelines :
    // A piece has 0.5 seconds after landing on the stack before it locks down.
    bool lock_delay_active;
    // Number of times where the lock delay had been reset after a move, the limit being 15, after what the piece will be hard locked.
    uint8_t move_reset_count;
    // Last rotation performed, for tspin analysis.
    LastRotation last_rotation;
    // Last action performed, what the last piece did (how many lines cleared, tspin ?)
    LastAction last_action;
    // Is the last line clear performed a perfect clear ? If yes, display "Perfect Clear !"
    bool is_perfect_clear;
    // Tetrises and T-spins made back to back. Any other kind of line clear will reset the b2b.
    // Negative (-1) when the last line clear wasn't a tetris/t-spin. 0 when only one was performed.
    int16_t b2b_count;
    // I'll probably remove this stat (or keep it for an after-game result)
    uint16_t numLines;
    uint16_t fallSpeed;
    GameState gameState;
    FuriTimer* timer;
    FuriMutex* mutex;
} TetrisState;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} TetrisEvent;

// -------------------------------------------- Rendering functions --------------------------------------------

static void tetris_game_draw_border(Canvas* const canvas) {
    canvas_draw_line(
        canvas,
        FIELD_X_OFFSET,
        FIELD_Y_OFFSET,
        FIELD_X_OFFSET,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 7);
    canvas_draw_line(
        canvas,
        FIELD_X_OFFSET,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 7,
        FIELD_X_OFFSET + FIELD_WIDTH * 5 + 8,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 7);
    canvas_draw_line(
        canvas,
        FIELD_X_OFFSET + FIELD_WIDTH * 5 + 8,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 7,
        FIELD_X_OFFSET + FIELD_WIDTH * 5 + 8,
        FIELD_Y_OFFSET);

    canvas_draw_line(
        canvas,
        FIELD_X_OFFSET + 2,
        FIELD_Y_OFFSET + 0,
        FIELD_X_OFFSET + 2,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 5);
    canvas_draw_line(
        canvas,
        FIELD_X_OFFSET + 2,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 5,
        FIELD_X_OFFSET + FIELD_WIDTH * 5 + 6,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 5);
    canvas_draw_line(
        canvas,
        FIELD_X_OFFSET + FIELD_WIDTH * 5 + 6,
        FIELD_Y_OFFSET + (FIELD_HEIGHT - 2) * 5 + 5,
        FIELD_X_OFFSET + FIELD_WIDTH * 5 + 6,
        FIELD_Y_OFFSET);
}

static void tetris_game_draw_hold_separator(Canvas* const canvas) {
    // Draws the box where the held piece is shown. Empty when starting a new game.
    canvas_draw_line(
        canvas, 0, SMALL_BLOCK_HEIGHT * 2, SMALL_BLOCK_WIDTH * 4 - 3, SMALL_BLOCK_HEIGHT * 2);
    canvas_draw_line(
        canvas, SMALL_BLOCK_WIDTH * 4 - 2, 0, SMALL_BLOCK_WIDTH * 4 - 2, SMALL_BLOCK_HEIGHT * 2);
}

static void tetris_game_draw_block(Canvas* const canvas, uint16_t xOffset, uint16_t yOffset) {
    canvas_draw_rframe(
        canvas,
        BORDER_OFFSET + MARGIN_OFFSET + xOffset,
        BORDER_OFFSET + MARGIN_OFFSET + yOffset - 1,
        BLOCK_WIDTH,
        BLOCK_HEIGHT,
        1);
    canvas_draw_dot(
        canvas,
        BORDER_OFFSET + MARGIN_OFFSET + xOffset + 2,
        BORDER_OFFSET + MARGIN_OFFSET + yOffset + 1);
    canvas_draw_dot(
        canvas,
        BORDER_OFFSET + MARGIN_OFFSET + xOffset + 3,
        BORDER_OFFSET + MARGIN_OFFSET + yOffset + 1);
    canvas_draw_dot(
        canvas,
        BORDER_OFFSET + MARGIN_OFFSET + xOffset + 2,
        BORDER_OFFSET + MARGIN_OFFSET + yOffset + 2);
}

static void
    tetris_game_draw_small_block(Canvas* const canvas, uint16_t xOffset, uint16_t yOffset) {
    // saving auto-indents weirdly here idk why
    canvas_draw_rframe(
        canvas,
        BORDER_OFFSET + MARGIN_OFFSET + xOffset,
        BORDER_OFFSET + MARGIN_OFFSET + yOffset - 1,
        SMALL_BLOCK_WIDTH,
        SMALL_BLOCK_HEIGHT,
        0);
}

static void tetris_game_draw_playfield(Canvas* const canvas, const TetrisState* tetris_state) {
    for(int y = 0; y < FIELD_HEIGHT; y++) {
        for(int x = 0; x < FIELD_WIDTH; x++) {
            if(tetris_state->playField[y][x]) {
                tetris_game_draw_block(
                    canvas,
                    FIELD_X_OFFSET + x * (BLOCK_WIDTH - 1),
                    FIELD_Y_OFFSET + y * (BLOCK_HEIGHT - 1) - 2 * (BLOCK_HEIGHT - 1));
            }
        }
    }
}

static void tetris_game_draw_hold_piece(Canvas* const canvas, const TetrisState* tetris_state) {
    if(tetris_state->holdPiece_id == hold_empty_value)
        return; // The game starts with no held piece.
    Piece* next_piece = &shapes[tetris_state->holdPiece_id];
    for(int i = 0; i < 4; i++) {
        uint8_t x = next_piece->p[i].x;
        uint8_t y = next_piece->p[i].y;

        uint8_t centering_offset = 2;
        if(next_piece->offsetType == OffsetTypeI)
            centering_offset = 0;
        else if(next_piece->offsetType == OffsetTypeO)
            centering_offset = 1;

        tetris_game_draw_small_block(
            canvas,
            x * (SMALL_BLOCK_WIDTH - 1) - (SMALL_BLOCK_WIDTH * 4) + centering_offset + 3,
            y * (SMALL_BLOCK_HEIGHT - 1) - 3);
    }
}

static void tetris_game_draw_next_piece(Canvas* const canvas, const TetrisState* tetris_state) {
    Piece* next_piece = &shapes[tetris_state->next_id];
    for(int i = 0; i < 4; i++) {
        uint8_t x = next_piece->p[i].x;
        uint8_t y = next_piece->p[i].y;
        uint8_t centering_offset = 0;

        if(tetris_state->next_id != I_piece) centering_offset = BLOCK_WIDTH;

        tetris_game_draw_block(
            canvas,
            x * (BLOCK_WIDTH - 1) - (BLOCK_WIDTH * 4) + SMALL_BLOCK_WIDTH * 4 + 5 +
                centering_offset,
            y * (BLOCK_HEIGHT - 1) - 3);
    }
}

static void tetris_game_draw_previews(Canvas* const canvas, const TetrisState* tetris_state) {
    for(int j = 0; j < 4; j++) {
        //Piece* next_piece = &shapes[tetris_state->next_previews[j]];
        const char* pieces = "ZSLJTIO";
        char next_piece[] = {pieces[tetris_state->next_previews[j]], '\0'};

        if(next_piece[0] == 'I')
            canvas_draw_str(
                canvas, SMALL_BLOCK_WIDTH * 4 + BLOCK_WIDTH * 4 + j * 6 + 1, 8, next_piece);
        else
            canvas_draw_str(
                canvas, SMALL_BLOCK_WIDTH * 4 + BLOCK_WIDTH * 4 + j * 6 - 1, 8, next_piece);
    }
}

static void
    tetris_game_draw_line_clear_result(Canvas* const canvas, const TetrisState* tetris_state) {
    //FURI_LOG_D(
    //    "Tetris_game",
    //    "last_act %d, last_rot %d",
    //    tetris_state->last_action,
    //    tetris_state->last_rotation);
    switch(tetris_state->last_action) {
    case Nothing:
        return;
        break;
    case Single:
        return canvas_draw_str(canvas, 55, 17, "x1");
        break;
    case Double:
        return canvas_draw_str(canvas, 53, 17, "x2");
        break;
    case Triple:
        return canvas_draw_str(canvas, 53, 17, "x3");
        break;
    case Tetris:
        return canvas_draw_str(canvas, 40, 17, "Tetris");
        break;
    case TS0:
        return canvas_draw_str(canvas, 46, 17, "TS0");
        break;
    case TSS:
        return canvas_draw_str(canvas, 46, 17, "TSS");
        break;
    case TSD:
        return canvas_draw_str(canvas, 46, 17, "TSD");
        break;
    case TST:
        return canvas_draw_str(canvas, 46, 17, "TST");
        break;
    case M_TS0:
        return canvas_draw_str(canvas, 39, 17, "mTS0");
        break;
    case M_TSS:
        return canvas_draw_str(canvas, 39, 17, "mTSS");
        break;
    case I_TSD:
        return canvas_draw_str(canvas, 39, 17, "iTSD");
        break;
    case F_TSD:
        return canvas_draw_str(canvas, 39, 17, "fTSD");
        break;
    case N_M_TSD:
        // all my respect if u manage to freestyle this one (Iso is quite tricky too)
        return canvas_draw_str(canvas, 37, 17, "nmTSD");
        break;
    default:
        return canvas_draw_str(canvas, 40, 17, "???");
        break;
    }
    //canvas_draw_str(canvas, 40, 17, "Tetris");
    canvas_draw_str(canvas, 40, 17, "TS");
}

static void tetris_game_draw_b2b_count(Canvas* const canvas, const TetrisState* tetris_state) {
    if(tetris_state->b2b_count <= 0) return;
    size_t b2b_size = snprintf(NULL, 0, "B2B%d", tetris_state->b2b_count) + 2; // +1 for the '\0'
    char* str = malloc(b2b_size);
    snprintf(str, b2b_size, "B2B%d", tetris_state->b2b_count);
    return canvas_draw_str(canvas, 0, 19, str);
    free(str);
}

static void tetris_game_draw_perfect_clear(Canvas* const canvas, const TetrisState* tetris_state) {
    if(!tetris_state->is_perfect_clear) return;
    canvas_draw_str(canvas, 18, 40, "Perfect");
    canvas_draw_str(canvas, 21, 50, "Clear!");
}

static void tetris_game_render_curr_piece(TetrisState* tetris_state) {
    //FURI_LOG_D(
    //    "Tetris_game",
    //    "piece %d render %d ",
    //    tetris_state->currPiece_id,
    //    tetris_state->currPiece.p[0].y);
    for(int i = 0; i < 4; i++) {
        uint8_t x = tetris_state->currPiece.p[i].x;
        uint8_t y = tetris_state->currPiece.p[i].y;

        tetris_state->playField[y][x] = true;
    }
}

static void tetris_game_remove_curr_piece(TetrisState* tetris_state) {
    for(int i = 0; i < 4; i++) {
        uint8_t x = tetris_state->currPiece.p[i].x;
        uint8_t y = tetris_state->currPiece.p[i].y;

        tetris_state->playField[y][x] = false;
    }
}

static void tetris_game_render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    const TetrisState* tetris_state = ctx;
    furi_mutex_acquire(tetris_state->mutex, FuriWaitForever);

    tetris_game_draw_border(canvas);
    tetris_game_draw_playfield(canvas, tetris_state);
    tetris_game_draw_next_piece(canvas, tetris_state);
    tetris_game_draw_hold_piece(canvas, tetris_state);
    tetris_game_draw_previews(canvas, tetris_state);
    tetris_game_draw_line_clear_result(canvas, tetris_state);
    tetris_game_draw_hold_separator(canvas);
    tetris_game_draw_perfect_clear(canvas, tetris_state);
    tetris_game_draw_b2b_count(canvas, tetris_state);

    // Show score on the game field
    //if(tetris_state->gameState == GameStatePlaying || tetris_state->gameState == GameStatePaused) {
    //    char buffer2[6];
    //    snprintf(buffer2, sizeof(buffer2), "%u", tetris_state->numLines);
    //    canvas_draw_str_aligned(canvas, 62, 10, AlignRight, AlignBottom, buffer2);
    //}

    if(tetris_state->gameState == GameStateGameOver) {
        // 128 x 64
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 1, 52, 62, 24);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, 1, 52, 62, 24);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 63, "Game Over");

        char buffer[13];
        snprintf(buffer, sizeof(buffer), "Lines: %u", tetris_state->numLines);
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(canvas, 32, 73, AlignCenter, AlignBottom, buffer);
    }

    if(tetris_state->gameState == GameStatePaused) {
        // 128 x 64
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, 1, 52, 62, 24);

        canvas_set_color(canvas, ColorBlack);
        canvas_draw_frame(canvas, 1, 52, 62, 24);

        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str(canvas, 4, 63, "Paused");

        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str(canvas, 4, 73, "hold      to quit");
        canvas_draw_icon(canvas, 22, 66, &I_Pin_back_arrow_10x8);
    }
    furi_mutex_release(tetris_state->mutex);
}

static void tetris_game_input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);

    TetrisEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

// -------------------------------------------- Game mechanics & utility --------------------------------------------

static uint8_t tetris_game_7_bag_randomizer(TetrisState* tetris_state) {
    // doesn't change tetris_state anymore, this change needs to be handled; returns the drawed piece.
    bool empty = true;
    // changed misleading name; "is the bag empty and needs to be refilled" variable
    // misleading behaviour (at least from my perspective as an advanced tetris player)
    // 1 means the piece has been drawed and is NOT in the bag anymore, 0 means STILL in the bag.
    for(int i = 0; i < 7; i++) {
        if(!tetris_state->bag[i]) {
            empty = false;
            break;
        }
    }
    if(empty == true) { // fills the bag again
        for(int i = 0; i < 7; i++) {
            tetris_state->bag[i] = false;
        }
    }

    int next_piece = rand() % 7;
    while(tetris_state->bag[next_piece]) {
        next_piece = rand() % 7;
    }
    //tetris_state->bag[next_piece] = true;
    //int result = tetris_state->next_id;
    //tetris_state->next_id = next_piece;
    return next_piece;
}

static uint8_t tetris_game_update_next_pieces(TetrisState* tetris_state) {
    int piece_to_spawn = tetris_state->next_id;
    int next_piece = tetris_game_7_bag_randomizer(tetris_state);
    tetris_state->bag[next_piece] = true; // removes the drawed piece from the bag
    tetris_state->next_id = tetris_state->next_previews[0];
    for(int i = 1; i < 4; i++) {
        tetris_state->next_previews[i - 1] = tetris_state->next_previews[i];
    }
    tetris_state->next_previews[3] = next_piece;
    return piece_to_spawn;
}

static bool tetris_game_is_valid_pos(TetrisState* tetris_state, Point* shape) {
    for(int i = 0; i < 4; i++) {
        //FURI_LOG_E("tetris_game", "y: %d", shape[i].y);
        if(shape[i].x < 0 || shape[i].x > (FIELD_WIDTH - 1) || shape[i].y > FIELD_HEIGHT ||
           tetris_state->playField[shape[i].y][shape[i].x] == true) {
            return false;
        }
    }
    ///FURI_LOG_E("tetris_game", "---");
    return true;
}

static void tetris_game_hold_piece(TetrisState* tetris_state, Piece* newPiece) {
    if(tetris_state->used_hold == true) return;
    if(tetris_state->holdPiece_id == hold_empty_value) {
        // hold the current piece and swap to the next one
        tetris_state->holdPiece_id = tetris_state->currPiece_id;
        tetris_state->currPiece_id = tetris_game_update_next_pieces(tetris_state);
    } else {
        // swap the held piece and the falling piece
        uint8_t piece_to_hold = tetris_state->currPiece_id;
        tetris_state->currPiece_id = tetris_state->holdPiece_id;
        tetris_state->holdPiece_id = piece_to_hold;
    }
    Piece* spawnedPiece = &shapes[tetris_state->currPiece_id];
    for(int i = 0; i < 4; i++) {
        newPiece->p[i].x = spawnedPiece->p[i].x;
        newPiece->p[i].y = spawnedPiece->p[i].y;
    }
    tetris_state->used_hold = true;
}

static void tetris_game_rotate_shape(Point currShape[], Point newShape[]) {
    // Copy shape data
    for(int i = 0; i < 4; i++) {
        newShape[i] = currShape[i];
    }

    for(int i = 1; i < 4; i++) {
        int8_t relX = currShape[i].x - currShape[0].x;
        int8_t relY = currShape[i].y - currShape[0].y;

        // Matrix rotation thing
        int8_t newRelX = (relX * 0) + (relY * -1);
        int8_t newRelY = (relX * 1) + (relY * 0);

        newShape[i].x = currShape[0].x + newRelX;
        newShape[i].y = currShape[0].y + newRelY;
    }
}

static void tetris_game_counter_rotate_shape(Point currShape[], Point newShape[]) {
    // Copy shape data
    for(int i = 0; i < 4; i++) {
        newShape[i] = currShape[i];
    }

    for(int i = 1; i < 4; i++) {
        int8_t relX = currShape[i].x - currShape[0].x;
        int8_t relY = currShape[i].y - currShape[0].y;

        // Matrix rotation thing but ccw ig
        int8_t newRelX = (relX * 0) + (relY * 1);
        int8_t newRelY = (relX * -1) + (relY * 0);

        newShape[i].x = currShape[0].x + newRelX;
        newShape[i].y = currShape[0].y + newRelY;
    }
}

static void tetris_game_apply_kick(Point points[], Point kick) {
    for(int i = 0; i < 4; i++) {
        points[i].x += kick.x;
        points[i].y -= kick.y; // WHY IS THE BOTTOM NOT 0 SERIOUSLY
    }
}

static bool tetris_game_piece_at_bottom(TetrisState* tetris_state, Piece* newPiece) {
    for(int i = 0; i < 4; i++) {
        Point* pos = (Point*)&newPiece->p;
        //FURI_LOG_D("tetris_game", "y %d, %d", pos[i].y, FIELD_HEIGHT);
        if(pos[i].y >= FIELD_HEIGHT || tetris_state->playField[pos[i].y][pos[i].x] == true) {
            return true;
        }
    }
    return false;
}

static bool tetris_game_current_piece_at_bottom(TetrisState* tetris_state) {
    // another misleading game implementation, the game board is oriented upwards instead of downwards,
    // meaning looking to a HIGHER y actually mean looking to a LOWER line
    // lowest_y_of_the_current_piece actually looks for the highest y, but the lower part of the piece.
    Piece* currPiece = &tetris_state->currPiece;
    int lowest_y_of_the_current_piece = currPiece->p[0].y;
    for(int i = 1; i < 4; i++) {
        if(currPiece->p[i].y > lowest_y_of_the_current_piece) {
            lowest_y_of_the_current_piece = currPiece->p[i].y;
        }
    }
    for(int i = 0; i < 4; i++) {
        Point* pos = (Point*)&currPiece->p;
        if(pos[i].y == lowest_y_of_the_current_piece &&
           // check only UNDER the falling piece since the piece itself is rendered
           (tetris_state->playField[pos[i].y + 1][pos[i].x])) {
            // the + 1 actually means looking UNDER (so basically - 1 line)
            return true;
        }
    }
    return false;
}

static void tetris_game_try_rotation(TetrisState* tetris_state, Piece* newPiece) {
    uint8_t currRotIdx = tetris_state->currPiece.rotIdx;

    Point* rotatedShape = malloc(sizeof(Point) * 4);
    Point* kickedShape = malloc(sizeof(Point) * 4);

    memcpy(rotatedShape, &tetris_state->currPiece.p, sizeof(tetris_state->currPiece.p));

    tetris_game_rotate_shape(tetris_state->currPiece.p, rotatedShape);

    for(int kick = 0; kick < 5; kick++) {
        memcpy(kickedShape, rotatedShape, (sizeof(Point) * 4));
        if(tetris_state->currPiece_id == I_piece) {
            tetris_game_apply_kick(kickedShape, SRS_I_kick_table[currRotIdx][Clockwise][kick]);
        } else if(tetris_state->currPiece_id == O_piece) {
            break;
            // pls don't ever try to rotate an O piece, why would you anyway
            // O-spin should score as much as a TST change my mind.
        } else {
            tetris_game_apply_kick(kickedShape, SRS_kick_table[currRotIdx][Clockwise][kick]);
        }

        if(tetris_game_is_valid_pos(tetris_state, kickedShape)) {
            if(tetris_state->currPiece_id == T_piece) {
                if(currRotIdx == 0 && kick == 4) {
                    tetris_state->last_rotation = T_TST_Rotation;
                } else if(currRotIdx == 2 && kick == 4) {
                    tetris_state->last_rotation = T_Fin_Rotation;
                } else if(currRotIdx == 2 && kick == 3) {
                    // okay so the difference between iso and neo is quite tricky, it's about the 3 corner rule
                    // basically neo is a mini tspin but since its the last kick, like any other mini its upgraded to a full
                    tetris_state->last_rotation = Iso_Neo_kick;
                } else if(kick == 4) {
                    tetris_state->last_rotation = Upgrade_Rotation;
                } else {
                    tetris_state->last_rotation = T_Mec_Rotation;
                }
            } else {
                tetris_state->last_rotation = NormalRotation;
            }
            memcpy(&newPiece->p, kickedShape, sizeof(newPiece->p));
            newPiece->rotIdx = (newPiece->rotIdx + 1) % 4;
            if(tetris_game_current_piece_at_bottom(tetris_state)) {
                if(tetris_state->move_reset_count < 15) {
                    furi_timer_restart(tetris_state->timer, tetris_state->fallSpeed);
                    tetris_state->move_reset_count++;
                }
            }
            break;
        }
    }
    free(rotatedShape);
    free(kickedShape);
}

static void tetris_game_try_counter_rotation(TetrisState* tetris_state, Piece* newPiece) {
    uint8_t currRotIdx = tetris_state->currPiece.rotIdx;
    //uint8_t wantedRotIdx = tetris_state->currPiece.rotIdx == 3 ? 0 : currRotIdx + 1;

    Point* rotatedShape = malloc(sizeof(Point) * 4);
    Point* kickedShape = malloc(sizeof(Point) * 4);

    memcpy(rotatedShape, &tetris_state->currPiece.p, sizeof(tetris_state->currPiece.p));

    tetris_game_counter_rotate_shape(tetris_state->currPiece.p, rotatedShape);

    for(int kick = 0; kick < 5; kick++) {
        memcpy(kickedShape, rotatedShape, (sizeof(Point) * 4));
        if(tetris_state->currPiece_id == I_piece) {
            tetris_game_apply_kick(
                kickedShape, SRS_I_kick_table[currRotIdx][Counter_clockwise][kick]);
        } else if(tetris_state->currPiece_id == O_piece) {
            break;
        } else {
            tetris_game_apply_kick(
                kickedShape, SRS_kick_table[currRotIdx][Counter_clockwise][kick]);
            //FURI_LOG_D(
            //    "tetris_game",
            //    "trying kick %d %d, curr rot : %d",
            //    SRS_kick_table[currRotIdx][Counter_clockwise][kick].x,
            //    SRS_kick_table[currRotIdx][Counter_clockwise][kick].y,
            //    currRotIdx);
        }

        if(tetris_game_is_valid_pos(tetris_state, kickedShape)) {
            if(tetris_state->currPiece_id == T_piece) {
                if(currRotIdx == 0 && kick == 4) {
                    tetris_state->last_rotation = T_TST_Rotation;
                } else if(currRotIdx == 2 && kick == 4) {
                    tetris_state->last_rotation = T_Fin_Rotation;
                } else if(currRotIdx == 2 && kick == 3) {
                    // okay so the difference between iso and neo is quite tricky, it's about the 3 corner rule
                    // basically neo is a mini tspin but since its the last kick, like any other mini its upgraded to a full
                    tetris_state->last_rotation = Iso_Neo_kick;
                } else {
                    tetris_state->last_rotation = T_Mec_Rotation;
                }
            } else {
                tetris_state->last_rotation = NormalRotation;
            }
            memcpy(&newPiece->p, kickedShape, sizeof(newPiece->p));
            newPiece->rotIdx = currRotIdx == 0 ? 3 : currRotIdx - 1;
            if(tetris_game_current_piece_at_bottom(tetris_state)) {
                if(tetris_state->move_reset_count < 15) {
                    furi_timer_restart(tetris_state->timer, tetris_state->fallSpeed);
                    tetris_state->move_reset_count++;
                }
            }
            break;
        }
    }
    free(rotatedShape);
    free(kickedShape);
}

static bool tetris_game_row_is_line(bool row[]) {
    for(int i = 0; i < FIELD_WIDTH; i++) {
        if(row[i] == false) return false;
    }
    return true;
}

static void
    tetris_game_check_for_lines(TetrisState* tetris_state, uint8_t* lines, uint8_t* numLines) {
    for(int i = 0; i < FIELD_HEIGHT; i++) {
        if(tetris_game_row_is_line(tetris_state->playField[i])) {
            *(lines++) = i;
            *numLines += 1;
        }
    }
}

static bool tetris_game_check_for_perfect_clear(TetrisState* tetris_state) {
    for(int y = FIELD_HEIGHT - 1; y >= 0; y--) {
        // Start checking at the bottom for more efficiency
        for(int x = 0; x < FIELD_WIDTH; x++) {
            if(tetris_state->playField[y][x] == true) {
                return false;
            }
        }
    }
    // the board is completely empty
    return true;
}

static T_Spin_Type tetris_game_3_corner_rule_tspin_detection(TetrisState* tetris_state) {
    /* A |T| A       The two A are the top corners, both needs to be filled to be a full spin.
       |T T T|       (The three T represent the T piece)
       B     B       The two A are the bottom corners

       At least 3 of these corners need to be filled to be counted as a tspin or tspin mini.
    */
    int corners_pos[4][2] = {
        // x,y coordinates for spawn state
        {-1, 1},
        {1, 1}, // top corners
        {1, -1},
        {-1, -1} // bottom
    };

    for(int i = 0; i < tetris_state->currPiece.rotIdx; i++) {
        // apply current rotation to the corners so it matches the piece's actual rotation
        int temp[2] = {corners_pos[0][0], corners_pos[0][0]};
        for(int i = 0; i < 3; i++) {
            corners_pos[i][0] = corners_pos[i + 1][0];
            corners_pos[i][1] = corners_pos[i + 1][1];
        }
        corners_pos[3][0] = temp[0];
        corners_pos[3][1] = temp[1];
        FURI_LOG_D(
            "Tetris_game",
            "i %d, %d %d, %d %d, %d %d, %d %d",
            i,
            corners_pos[0][0],
            corners_pos[0][1],
            corners_pos[1][0],
            corners_pos[1][1],
            corners_pos[2][0],
            corners_pos[2][1],
            corners_pos[3][0],
            corners_pos[3][1]);
    }

    int top_count = 0;
    int bottom_count = 0;

    // check the corners
    for(int i = 0; i < 4; i++) {
        if(i < 2) { // top corners
            if(tetris_state->playField[corners_pos[i][1] + tetris_state->currPiece.p[0].y]
                                      [corners_pos[i][0] + tetris_state->currPiece.p[0].x]) {
                // y is first, i'm not the one who designed it this way tho.
                top_count++;
            }
        } else { // bottom
            if(tetris_state->playField[corners_pos[i][1] + tetris_state->currPiece.p[0].y]
                                      [corners_pos[i][0] + tetris_state->currPiece.p[0].x]) {
                bottom_count++;
            }
        }
    }

    FURI_LOG_D(
        "Tetris_game",
        "c : %d%d %d%d %d%d %d%d, top : %d, bot %d",
        corners_pos[0][0] + tetris_state->currPiece.p[0].x,
        corners_pos[0][1] + tetris_state->currPiece.p[0].y,
        corners_pos[1][0] + tetris_state->currPiece.p[0].x,
        corners_pos[1][1] + tetris_state->currPiece.p[0].y,
        corners_pos[2][0] + tetris_state->currPiece.p[0].x,
        corners_pos[2][1] + tetris_state->currPiece.p[0].y,
        corners_pos[3][0] + tetris_state->currPiece.p[0].x,
        corners_pos[3][1] + tetris_state->currPiece.p[0].y,
        top_count,
        bottom_count);
    if(top_count + bottom_count < 3) {
        return NoTspin;
    } else {
        if(top_count == 2) {
            return FullTspin;
        } else {
            return MiniTspin;
        }
    }
}

// -------------------------------------------- Processing functions --------------------------------------------

static void tetris_game_init_state(TetrisState* tetris_state) {
    tetris_state->gameState = GameStatePlaying;
    tetris_state->numLines = 0;
    tetris_state->move_reset_count = 0;
    tetris_state->holdPiece_id = hold_empty_value;
    tetris_state->used_hold = false;
    tetris_state->lock_delay_active = false;
    tetris_state->fallSpeed = MAX_FALL_SPEED;
    tetris_state->last_action = Nothing;
    tetris_state->last_rotation = NoRotation;
    tetris_state->is_perfect_clear = false;
    tetris_state->b2b_count = -1;
    memset(tetris_state->playField, 0, sizeof(tetris_state->playField));
    memset(tetris_state->bag, 0, sizeof(tetris_state->bag));

    // init next pieces
    int next_piece_id = tetris_game_7_bag_randomizer(tetris_state);
    // draws the first falling piece
    tetris_state->bag[next_piece_id] = true;
    tetris_state->currPiece_id = next_piece_id;
    memcpy(&tetris_state->currPiece, &shapes[next_piece_id], sizeof(tetris_state->currPiece));
    next_piece_id = tetris_game_7_bag_randomizer(tetris_state); // now draws the first NEXT piece
    tetris_state->next_id = next_piece_id;
    tetris_state->bag[next_piece_id] = true;
    for(int i = 0; i < 4; i++) {
        int piece_id = tetris_game_7_bag_randomizer(tetris_state);
        tetris_state->next_previews[i] = piece_id;
        tetris_state->bag[piece_id] = true;
    }

    furi_timer_start(tetris_state->timer, tetris_state->fallSpeed);
}

static void
    tetris_game_process_step(TetrisState* tetris_state, Piece* newPiece, bool wasDownMove) {
    //FURI_LOG_D(
    //    "Tetris_game",
    //    "piece %d x %d y %d",
    //    tetris_state->currPiece_id,
    //    tetris_state->currPiece.p[0].x,
    //    tetris_state->currPiece.p[0].y);

    if(tetris_state->gameState == GameStateGameOver || tetris_state->gameState == GameStatePaused)
        return;

    tetris_game_remove_curr_piece(tetris_state);

    if(wasDownMove) {
        if(tetris_game_piece_at_bottom(tetris_state, newPiece)) {
            if(!tetris_state->lock_delay_active) {
                tetris_state->lock_delay_active = true;
                tetris_game_render_curr_piece(tetris_state);
            } else {
                furi_timer_stop(tetris_state->timer);

                tetris_game_render_curr_piece(tetris_state);
                uint8_t numLines = 0;
                uint8_t lines[] = {0, 0, 0, 0};
                uint16_t nextFallSpeed;

                tetris_game_check_for_lines(tetris_state, lines, &numLines);
                if(numLines > 0) {
                    for(int i = 0; i < numLines; i++) {
                        // zero out row
                        for(int j = 0; j < FIELD_WIDTH; j++) {
                            tetris_state->playField[lines[i]][j] = false;
                        }
                        // move all above rows down
                        for(int k = lines[i]; k >= 0; k--) {
                            for(int m = 0; m < FIELD_WIDTH; m++) {
                                tetris_state->playField[k][m] =
                                    (k == 0) ? false : tetris_state->playField[k - 1][m];
                            }
                        }
                    }

                    uint16_t oldNumLines = tetris_state->numLines;
                    tetris_state->numLines += numLines;
                    if((oldNumLines / 10) % 10 != (tetris_state->numLines / 10) % 10) {
                        nextFallSpeed =
                            tetris_state->fallSpeed - (100 / (tetris_state->numLines / 10));
                        if(nextFallSpeed >= MIN_FALL_SPEED) {
                            tetris_state->fallSpeed = nextFallSpeed;
                        }
                    }
                }

                // Determine action performed
                T_Spin_Type tspin_type = NoTspin;
                if(tetris_state->currPiece_id == T_piece)
                    tspin_type = tetris_game_3_corner_rule_tspin_detection(tetris_state);
                // more explanation about 3 corner rule here :
                // https://harddrop.com/fumen/?m115@sgg0Aeg0QeAtAeAtreF811AyoSTASo78A2no2ACD5r?DlsCSASI/MESGNXEzoo2AJG98AQ51JEBD98AQo7aEJPONEO?BAAAvhGFcutAyoSTASoTABEoo2AUoo2Aw+kkDloo2ApN98A?Q5bkDJmZTASYlNE3CaoDTBAAAFcuxACD5rDFbcRATG88AwU?jXEuICbEFbMLEuoo2AiSg/DFbMLEmoo2AUoo2Aw+kkDFBAA?AFcueAzno2Aj3UNEyoSTASIPrDMj0TAS4wrDxQWXEFnBAAF?curAmXyTAS4wrDnAFeDyoo2AUEzPEJG98AwWyTASo93Du+8?8AQemsCwOxCAFcuzAyno2AyoSTASIPrDMj0TAS4wrDxQWXE?F388AQZjXEFbUVEl7gDEBM98AQemsCwOxCAFcusAV2krDzN?98AQemsCwOpTASYlWEJ5krDFbs9DpViTASIE2DplbTASosa?EFcu0Ayno2AynNbEFbEmDvjpTASo78A2no2Axno2Aj3khEN?G98AQurTASY91Dloo2AzuSrDsggHAegHQeAPAeAPreAAtjA?yYZhEsCyTASYttAzI2JEFbEBEJGVTASI3CElCCbElsKBAMh?H8CeH8AeE8JedruAAMhgWQeAPAeAPNeFrfMhglQeAtAeAtN?edrfvhAFrfMhAPQegWAeAPNeNrfHhC8BeAtQeglAeAtNetl?fMhgWQeAPAeAPNeFrf3gB8IeA8GeA8AeglIeA8GeAtAeAtN?e1gfKhAPAegWQeAPAegWNedrfKhAtAeglQeAPAeglNe1gfK?hAPAegWSegWNedru2AP2EvEFb85AFbUVEF388Aw08CEsoo2?AiA3TASIbeEJzkTAS4wrDnAFeDyoo2AUEzPEJ2BAAvhAdru?lAiYwdD1NVTASICvDFbEwCtMN5Duoo2A0LmQEs488AwAukD?LBAAAKhAtAeglIeAAGeA8AeglNe1guAAvhBdrfFrfKhgWAe?APSeAPNeNrfKhglAeAtQeAAAeAtNe1gfvhBdrfFrfKhgWAe?APSeAPNeNrfhgB8IeA8FeB8AeA8BeE8BeB8AeD8AeBAgHA8?AtFeAAC8FeAAB8AtNeNQfvhC1VfdgfdlfAhgWQeAPAeAPZe?FlfhgBAFeB8AeAAGeAAAeGABeA8glIeA8GeAPA8AtZeVbf9?ggWAeAPQegWAeAPaetkfXgB8IeA8IeG8CeG8glA8APHeA8H?eglAeAPaeNLfvhA1QfqgAPAegWQeAPAegWtedbfXgBAIeAA?FeAAAPAegHFAAeB8GAAeAPAegHFAAeAAAeGAKeAAA8QeFgf?HhgWSegWAeAPQetpu2AP2EvEFb85AFbUVEF388Aw08CEsoo?2AiA3TASIbeEJzkTAS4wrDnAFeDyoo2AUEzPEJ2BAAvhAtp?ulAiYwdD1NVTASICvDFbEwCtMN5Duoo2A0LmQEs488AwAuk?DLBAAAkgB8IeA8FeB8AeD8AeC8CeC8glC8BeD8AeB8AeBAD?eglA8AtDAMeVXuQAmXyTASY91Dloo2As3cyEvhE9hf9rfVw?f1wfNrf3ggWAegWSeAPgelhuyAyYZhEsCyTASYttAzI2JEF?bcRASExrD2ICbEloo2AUEzPEJG98AQuR5DQDVTAylAAAkgB?AIeAACeDAgHAegHGACeGABeAPGABeGADeCAJeAAtgANOJ5D?FbEwCtMN5Duoo2AsOprDFbsiDs4DXEz4CwBkgB8IeA8CeE8?AeH8CeG8BeH8BeH8BeD8Je9huAA3ggWAegWSeAPgelhuYAP?2EvEFb8bDFbcYCJGeTASYttAzI2JE3gglAeglSeAtge9ruA?A3ggWAegWSeAPgelhuzAP2EvEFb8bDFbcYCJGeTASYttAzI?2JEFbcRASEYNEFbEwCyuVDEloo2Areg/DFr4AAvhBlhu4A0?LmQEs488AwAukDr4CwBFbU9AFbUVEvz0TASYBNEXmbfEwow?2BFb85AFbEcEvoo2AzuSrDlhu9Ayno2AynNbEFbEmDvjZ1A?VJ98AQo78AQurTASIT5Dk488Aw3K6BFb0HEvoo2A0LmQEs4?88AwAukDLBAAAkgBAIeAAFeAAgHAegHCAAeCACeCABeBABe?APCABeBAB8FeAAAeB8NeFguAAIhgWSegWAeAPPeNquYAP2E?vEFb8bDFbcYCJGeTASYttAzI2JEzgCAAeF8AAEeD8AeglB8?AeE8BeA8AeAAFeglA8APAAOe1guAAvhBdrfFhfKhAPAegWS?egWNedruYAP2EvEFb8bDFbcYCJGeTASYttAzI2JE3gFAFeG?AAPAeglIeAAGeA8AeglNedrujAtnceEFb0sDy4vhEF22TAS?o93Du+88AQemsCwO5aElsKBAMhAPQegWAegWNeVrfMhAPDA?EeA8GegHAegHNetqfWhAPXeVwflhC8g0A8g0D8VwujAFbew?DyHRKEkoo2AjHRKE0N98AwR0TAS414DMD9nDFr4AAHhC8Le?APDADeB8DeCAgHAAgHDAVrujAtnceEFb0sDy4vhEF22TASo?93Du+88AQemsCwO5aElsKBAJhgWSegWAeAPOetqfHhBAgHG?eCAGeAAAeglA8AtOe1puYABjkNEFbMmEPsyaEFbMLEuoo2A?3iMDEchAPReNpupAFbemEBzkTASI/MEV2GbEFb8bDzoo2Am?uMDEF388Aw08CEsN98AZAAAA/gB8HeA8IeA8GeA8AtAeAAP?eVrueABjkNEFbMmEPsyaEFbMmEJ/bTASIClEF87dDurBAAJ?hgWSegWAeAPOetqf
                if(tetris_state->currPiece_id == T_piece &&
                   tetris_state->last_rotation != NoRotation && tspin_type != NoTspin) {
                    // condition checks if a tspin is performed
                    if(tetris_state->last_rotation == Iso_Neo_kick && tspin_type == MiniTspin) {
                        tetris_state->last_action = N_M_TSD;
                    } else if(tetris_state->last_rotation == Iso_Neo_kick && tspin_type == FullTspin) {
                        tetris_state->last_action = I_TSD;
                    }
                    switch(numLines) {
                    case 0:
                        if(tspin_type == FullTspin ||
                           tetris_state->last_rotation == Upgrade_Rotation) {
                            tetris_state->last_action = TS0;
                        } else {
                            tetris_state->last_action = M_TS0;
                        }
                        break;
                    case 1:
                        if(tspin_type == FullTspin ||
                           tetris_state->last_rotation == Upgrade_Rotation) {
                            tetris_state->last_action = TSS;
                        } else {
                            tetris_state->last_action = M_TSS;
                        }
                        break;
                    case 2:
                        if(tspin_type == FullTspin ||
                           tetris_state->last_rotation == Upgrade_Rotation) {
                            tetris_state->last_action = TSD;
                        }
                        // the only mini tsd i know so far is the neo tsd. lmk if u discover another mini tsd !
                        break;
                    case 3:
                        tetris_state->last_action = TST;
                        // because of the kick performed, tst should always be upgraded to full spins (except if mini tst actually exists ?? /jk)
                        break;
                    default:
                        // Something went wrong if we get here...
                        tetris_state->last_action = Nothing;
                        break;
                    }
                } else {
                    switch(numLines) {
                    case 0:
                        tetris_state->last_action = Nothing;
                        break;
                    case 1:
                        tetris_state->last_action = Single;
                        break;
                    case 2:
                        tetris_state->last_action = Double;
                        break;
                    case 3:
                        tetris_state->last_action = Triple;
                        break;
                    case 4:
                        tetris_state->last_action = Tetris;
                        break;
                    default:
                        // Pls lmk if u manage to clear more at once !!!
                        tetris_state->last_action = Nothing;
                        break;
                    }
                }
                // Check for perfect clear
                if(tetris_game_check_for_perfect_clear(tetris_state)) {
                    tetris_state->is_perfect_clear = true;
                } else {
                    tetris_state->is_perfect_clear = false;
                }

                // Update B2B
                if(numLines > 0 && tetris_state->b2b_count > -1 &&
                   (tetris_state->last_action == Single || tetris_state->last_action == Double ||
                    tetris_state->last_action == Triple)) {
                    // breaks the b2b chain
                    tetris_state->b2b_count = -1;
                } else if(numLines > 0) {
                    // is tetris/t-spin since it's not a regular single/double/triple
                    tetris_state->b2b_count++;
                }

                // Check for game over
                tetris_state->move_reset_count = 0;
                int next_piece_id = tetris_game_update_next_pieces(tetris_state);
                Piece* spawnedPiece = &shapes[next_piece_id];
                if(!tetris_game_is_valid_pos(tetris_state, spawnedPiece->p)) {
                    tetris_state->gameState = GameStateGameOver;
                } else {
                    tetris_state->currPiece_id = next_piece_id;
                    memcpy(
                        &tetris_state->currPiece, spawnedPiece, sizeof(tetris_state->currPiece));
                    furi_timer_start(tetris_state->timer, tetris_state->fallSpeed);
                    tetris_state->lock_delay_active = false;
                    tetris_state->used_hold = false;
                    tetris_state->move_reset_count = 0;
                    tetris_state->currPiece.rotIdx = 0;
                }
            }
        }
    }

    if(tetris_game_is_valid_pos(tetris_state, newPiece->p)) {
        memcpy(&tetris_state->currPiece, newPiece, sizeof(tetris_state->currPiece));
    }

    tetris_game_render_curr_piece(tetris_state);
}

static void tetris_game_update_timer_callback(void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);

    TetrisEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

int32_t tetris_game_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(TetrisEvent));

    TetrisState* tetris_state = malloc(sizeof(TetrisState));

    tetris_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!tetris_state->mutex) {
        FURI_LOG_E("TetrisGame", "cannot create mutex\r\n");
        furi_message_queue_free(event_queue);
        free(tetris_state);
        return 255;
    }

    // Not doing this eventually causes issues with TimerSvc due to not sleeping/yielding enough in this task
    furi_timer_set_thread_priority(FuriTimerThreadPriorityElevated);

    ViewPort* view_port = view_port_alloc();
    view_port_set_orientation(view_port, ViewPortOrientationVertical);
    view_port_draw_callback_set(view_port, tetris_game_render_callback, tetris_state);
    view_port_input_callback_set(view_port, tetris_game_input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    tetris_state->timer =
        furi_timer_alloc(tetris_game_update_timer_callback, FuriTimerTypePeriodic, event_queue);
    tetris_game_init_state(tetris_state);

    TetrisEvent event;

    Piece* newPiece = malloc(sizeof(Piece));
    uint8_t downRepeatCounter = 0;

    // Call dolphin deed on game start
    dolphin_deed(DolphinDeedPluginGameStart);

    for(bool processing = true; processing;) {
        // This 10U implicitly sets the game loop speed. downRepeatCounter relies on this value
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 10U);

        furi_mutex_acquire(tetris_state->mutex, FuriWaitForever);

        memcpy(newPiece, &tetris_state->currPiece, sizeof(tetris_state->currPiece));
        bool wasDownMove = false;

        if(!furi_hal_gpio_read(&gpio_button_right)) {
            if(tetris_state->lock_delay_active == false) {
                // do not force the lock when the lock delay is active, or some spins way be annoying to perform.
                if(downRepeatCounter > 3) {
                    for(int i = 0; i < 4; i++) {
                        newPiece->p[i].y += 1;
                    }
                    downRepeatCounter = 0;
                    wasDownMove = true;
                } else {
                    downRepeatCounter++;
                }
            }
        }

        if(event_status == FuriStatusOk) {
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress || event.input.type == InputTypeLong ||
                   event.input.type == InputTypeRepeat) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        if(tetris_state->gameState == GameStatePlaying) {
                            tetris_game_remove_curr_piece(tetris_state);
                            tetris_game_try_rotation(tetris_state, newPiece);
                            tetris_game_render_curr_piece(tetris_state);
                        }
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        for(int i = 0; i < 4; i++) {
                            newPiece->p[i].x += 1;
                        }
                        if(tetris_game_current_piece_at_bottom(tetris_state)) {
                            if(tetris_state->move_reset_count < 15) {
                                furi_timer_restart(tetris_state->timer, tetris_state->fallSpeed);
                                tetris_state->move_reset_count++;
                                tetris_state->lock_delay_active = false;
                            }
                        }
                        break;
                    case InputKeyLeft:
                        for(int i = 0; i < 4; i++) {
                            newPiece->p[i].x -= 1;
                        }
                        if(tetris_game_current_piece_at_bottom(tetris_state)) {
                            if(tetris_state->move_reset_count < 15) {
                                furi_timer_restart(tetris_state->timer, tetris_state->fallSpeed);
                                tetris_state->move_reset_count++;
                                tetris_state->lock_delay_active = false;
                            }
                        }
                        break;
                    case InputKeyOk:
                        if(event.input.type == InputTypeLong) {
                            switch(tetris_state->gameState) {
                            case GameStatePaused:
                                tetris_state->gameState = GameStatePlaying;
                                break;
                            case GameStatePlaying:
                                tetris_state->gameState = GameStatePaused;
                                break;

                            default:
                                break;
                            }
                        } else if(
                            event.input.type == InputTypePress &&
                            tetris_state->gameState == GameStatePlaying) {
                            tetris_game_hold_piece(tetris_state, newPiece);
                        }
                        break;
                    case InputKeyBack:
                        if(event.input.type == InputTypeLong) {
                            processing = false;
                        } else if(event.input.type == InputTypePress) {
                            tetris_game_remove_curr_piece(tetris_state);
                            tetris_game_try_counter_rotation(tetris_state, newPiece);
                            tetris_game_render_curr_piece(tetris_state);
                        }
                        break;
                    default:
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                // TODO: This is inverted.  it returns true when the button is not pressed.
                // see macro in input.c and do that
                if(furi_hal_gpio_read(&gpio_button_right)) {
                    for(int i = 0; i < 4; i++) {
                        newPiece->p[i].y += 1;
                    }
                    wasDownMove = true;
                }
            }
        }

        tetris_game_process_step(tetris_state, newPiece, wasDownMove);

        furi_mutex_release(tetris_state->mutex);
        view_port_update(view_port);
    }

    furi_timer_free(tetris_state->timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(tetris_state->mutex);
    furi_timer_set_thread_priority(FuriTimerThreadPriorityNormal);
    free(newPiece);
    free(tetris_state);

    return 0;
}