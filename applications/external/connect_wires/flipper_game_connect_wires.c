/*
 * Copyright 2023 Alexander Taran
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT
 *
 * Thanks to:
 *  - Eugene Kirzhanov: https://github.com/eugene-kirzhanov/flipper-zero-2048-game for code example
 *  - Andrew Diamond https://github.com/HappyAmos for contributions
 */

#include <furi.h>
#include <gui/gui.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#define MAX_FIELD_WIDTH 16
#define MAX_FIELD_HEIGHT 8

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define LOG_TAG "connect_wires" // For logging

enum AppStatus {
    ST_PLAYING,
    ST_MAIN_MENU,
    ST_SELECTION_MENU,
    ST_ABOUT,
    ST_WINNING,
};

enum MenuEntryId {

    MN_NEWGAME,
    MN_ABOUT,
    MN_EXIT,

    MN_EASY,
    MN_MEDIUM,
    MN_HARD,
};

typedef struct {
    uint8_t id;
    char* text;
} MenuEntry;

const MenuEntry MainMenu[] = {
    {MN_NEWGAME, "New Game"},
    {MN_ABOUT, "About"},
    {MN_EXIT, "Exit"},
    {0, NULL},
};

const MenuEntry SelectionMenu[] = {
    {MN_EASY, "Easy (5x5)"},
    {MN_MEDIUM, "Medium (5x10)"},
    {MN_HARD, "Hard (6x12)"},
    {0, NULL},
};

const char* AboutStrings[] = {
    "Connect Wires game",
    "Turn field cells and connect",
    "all lamps to the power source.",
    "",
    "Alexander Taran, 2023",
    "Press any key",
    0};

uint8_t menuSize(const MenuEntry* menu) {
    uint8_t res = 0;
    while(menu[res].text != 0) res++;
    return res;
}

enum EdgeDir { DIR_LEFT, DIR_TOP, DIR_RIGHT, DIR_BOTTOM };

static const int8_t DX[4] = {-1, 0, 1, 0};
static const int8_t DY[4] = {0, -1, 0, 1};
static const int8_t OPP[4] = {DIR_RIGHT, DIR_BOTTOM, DIR_LEFT, DIR_TOP};

static const char* WINNING_MESSAGE = "Congratulations!";
static const char* SCORE_MESSAGE =
    "Moves: %u"; // Will show the user how many "rotations" they made

const NotificationSequence sequence_winning = {
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    NULL,
};

// Give a notification when the user changes direction
// show green led for clockwise.
const NotificationSequence sequence_clockwise = {
    &message_green_255,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_green_0,
    NULL,
};

// Give a notification when the user changes direction
// show red led for counter-clockwise.
const NotificationSequence sequence_counter_clockwise = {
    &message_red_255,
    &message_vibro_on,
    &message_delay_50,
    &message_vibro_off,
    &message_red_0,
    NULL,
};

typedef struct {
    bool edges[4];
} GridElement;

GridElement createGridElement() {
    GridElement g;
    for(uint8_t i = 0; i < 4; ++i) {
        g.edges[i] = false;
    }
    return g;
}

void rotate_grid_element(GridElement* elem, bool clockwise) {
    if(!clockwise) { // Counter-clockwise logic
        bool tmp = elem->edges[0];
        for(int8_t i = 0; i < 3; ++i) {
            elem->edges[i] = elem->edges[i + 1];
        }
        elem->edges[3] = tmp;
    } else { // Clockwise logic
        bool tmp = elem->edges[3];
        for(int8_t i = 3; i > 0; --i) {
            elem->edges[i] = elem->edges[i - 1];
        }
        elem->edges[0] = tmp;
    }
}

uint8_t count_edges(GridElement* elem) {
    uint8_t res = 0;
    for(uint8_t i = 0; i < 4; ++i) {
        if(elem->edges[i]) {
            res++;
        }
    }
    return res;
}

typedef struct {
    int8_t x, y;
} Coord;

Coord createCoord(int8_t x, int8_t y) {
    Coord c;
    c.x = x;
    c.y = y;
    return c;
}

bool eqCoord(Coord* c1, Coord* c2) {
    return c1->x == c2->x && c1->y == c2->y;
}

typedef struct {
    Coord fieldSize;
    Coord startingPoint;
    GridElement elements[MAX_FIELD_WIDTH][MAX_FIELD_HEIGHT];
    Coord currentSelection;
    uint16_t gameMoves; // 65535 theres no check for this, but if they go over....
    bool clockwise; // CLOCKWISE(true) || COUNTERCLOCKWISE(false)

    // calculated properties
    bool reachable[MAX_FIELD_WIDTH][MAX_FIELD_HEIGHT];
} GameState;

typedef struct {
    FuriMutex* mutex;
    GameState gameState;
    uint8_t status;
    uint8_t currentMenuSelection;
} AppState;

GameState createNewGameState(Coord fieldSize) {
    GameState gs;
    gs.fieldSize = fieldSize;
    Coord start =
        createCoord(1 + random() % (gs.fieldSize.x - 2), 1 + random() % (gs.fieldSize.y - 2));

    gs.gameMoves = 0; // Initialize beginning moves
    gs.clockwise = false; // This was this games original default, we'll keep it the same.

    gs.startingPoint = start;

    gs.currentSelection = start;
    for(uint8_t i = 0; i < MAX_FIELD_WIDTH; ++i) {
        for(uint8_t j = 0; j < MAX_FIELD_HEIGHT; ++j) {
            gs.elements[i][j] = createGridElement();
        }
    }

    // Field generation
    bool visited[MAX_FIELD_WIDTH][MAX_FIELD_HEIGHT] = {false};
    Coord* candidates = malloc(gs.fieldSize.x * gs.fieldSize.y * sizeof(Coord));
    uint8_t num_candidates = 0;
    candidates[num_candidates] = createCoord(start.x + 1, start.y);
    num_candidates++;
    candidates[num_candidates] = createCoord(start.x - 1, start.y);
    num_candidates++;
    candidates[num_candidates] = createCoord(start.x, start.y + 1);
    num_candidates++;
    candidates[num_candidates] = createCoord(start.x, start.y - 1);
    num_candidates++;
    visited[start.x][start.y] = true;

    for(uint8_t iteration = 1; iteration < gs.fieldSize.x * gs.fieldSize.y; ++iteration) {
        if(num_candidates == 0) {
            break;
        }
        // Picking a candidate until it has good dirs
        uint8_t cand_id;
        Coord cand;
        // Calculating possible dirs for an edge
        uint8_t dirs[4];
        uint8_t num_dirs = 0;
        while(true) {
            cand_id = random() % num_candidates;
            cand = candidates[cand_id];
            for(uint8_t i = 0; i < 4; ++i) {
                Coord neib = createCoord(cand.x + DX[i], cand.y + DY[i]);
                if(neib.x < 0 || neib.y < 0 || neib.x >= gs.fieldSize.x ||
                   neib.y >= gs.fieldSize.y) {
                    continue;
                }
                if(visited[neib.x][neib.y] && count_edges(&gs.elements[neib.x][neib.y]) < 3) {
                    dirs[num_dirs] = i;
                    num_dirs++;
                }
            }
            if(num_dirs != 0) break;
            candidates[cand_id] = candidates[num_candidates - 1];
            num_candidates--;
        }
        uint8_t dir = dirs[random() % num_dirs];

        // Generating an edge
        Coord neib = createCoord(cand.x + DX[dir], cand.y + DY[dir]);
        gs.elements[cand.x][cand.y].edges[dir] = true;
        gs.elements[neib.x][neib.y].edges[OPP[dir]] = true;

        visited[cand.x][cand.y] = true;

        // Remove cand from candidates array
        candidates[cand_id] = candidates[num_candidates - 1];
        num_candidates--;
        // Adding potential candidates
        for(uint8_t i = 0; i < 4; ++i) {
            Coord newCand = createCoord(cand.x + DX[i], cand.y + DY[i]);
            if(newCand.x < 0 || newCand.y < 0 || newCand.x >= gs.fieldSize.x ||
               newCand.y >= gs.fieldSize.y) {
                continue;
            }
            if(visited[newCand.x][newCand.y]) {
                continue;
            }
            bool found = false;
            for(uint16_t cid = 0; cid < num_candidates; ++cid) {
                if(eqCoord(&candidates[cid], &newCand)) {
                    found = true;
                    break;
                }
            }
            if(found) continue;
            candidates[num_candidates] = newCand;
            num_candidates++;
        }
    }
    free(candidates);

    return gs;
}

void shuffle(GameState* gs) {
    uint16_t shuffles = 0;
    do {
        for(uint8_t i = 0; i < gs->fieldSize.x; ++i) {
            for(uint8_t j = 0; j < gs->fieldSize.y; ++j) {
                uint8_t rounds = rand() % 4;
                for(uint8_t r = 0; r < rounds; ++r) {
                    rotate_grid_element(&gs->elements[i][j], false);
                    shuffles++;
                }
            }
        }
    } while(shuffles == 0); // Handling micro chance of no shuffling at all.
}

void moveSelection(GameState* gs, uint8_t dir) {
    Coord* cs = &gs->currentSelection;
    cs->x += DX[dir];
    cs->y += DY[dir];
    if(cs->x < 0) cs->x = gs->fieldSize.x - 1;
    if(cs->y < 0) cs->y = gs->fieldSize.y - 1;
    if(cs->x >= gs->fieldSize.x) cs->x = 0;
    if(cs->y >= gs->fieldSize.y) cs->y = 0;
}

void rotateSelection(GameState* gs) {
    Coord* cs = &gs->currentSelection;
    rotate_grid_element(&gs->elements[cs->x][cs->y], gs->clockwise);

    gs->gameMoves++; // Increment moves counter
}

void recalculateReachables(GameState* gs) {
    for(uint8_t i = 0; i < gs->fieldSize.x; ++i) {
        for(uint8_t j = 0; j < gs->fieldSize.y; ++j) {
            gs->reachable[i][j] = false;
        }
    }
    Coord* stack = malloc(gs->fieldSize.x * gs->fieldSize.y * sizeof(Coord));
    stack[0] = gs->startingPoint;
    uint8_t stackSize = 1;
    while(stackSize > 0) {
        Coord curr = stack[stackSize - 1];
        stackSize--;
        gs->reachable[curr.x][curr.y] = true;
        for(uint8_t i = 0; i < 4; ++i) {
            if(!gs->elements[curr.x][curr.y].edges[i]) {
                continue;
            }
            Coord neib = createCoord(curr.x + DX[i], curr.y + DY[i]);
            if(neib.x < 0 || neib.y < 0 || neib.x >= gs->fieldSize.x || neib.y >= gs->fieldSize.y)
                continue;
            if(gs->reachable[neib.x][neib.y] || !gs->elements[neib.x][neib.y].edges[OPP[i]]) {
                continue;
            }
            stack[stackSize] = neib;
            stackSize++;
        }
    }
    free(stack);
}

bool checkIsWinning(GameState* gs) {
    uint8_t cnt = 0;
    for(uint8_t i = 0; i < gs->fieldSize.x; ++i) {
        for(uint8_t j = 0; j < gs->fieldSize.y; ++j) {
            if(gs->reachable[i][j]) {
                cnt++;
            }
        }
    }
    return cnt == gs->fieldSize.x * gs->fieldSize.y;
}

static void draw_selection(Canvas* canvas, Coord at, uint8_t cellSize, uint8_t cornerSize) {
    canvas_draw_line(canvas, at.x, at.y, at.x + cornerSize - 1, at.y);
    canvas_draw_line(canvas, at.x, at.y, at.x, at.y + cornerSize - 1);

    canvas_draw_line(canvas, at.x + cellSize - 1, at.y, at.x + cellSize - cornerSize, at.y);
    canvas_draw_line(
        canvas, at.x + cellSize - 1, at.y, at.x + cellSize - 1, at.y + cornerSize - 1);

    canvas_draw_line(canvas, at.x, at.y + cellSize - 1, at.x, at.y + cellSize - cornerSize);
    canvas_draw_line(
        canvas, at.x, at.y + cellSize - 1, at.x + cornerSize - 1, at.y + cellSize - 1);

    canvas_draw_line(
        canvas,
        at.x + cellSize - 1,
        at.y + cellSize - 1,
        at.x + cellSize - 1,
        at.y + cellSize - cornerSize);
    canvas_draw_line(
        canvas,
        at.x + cellSize - 1,
        at.y + cellSize - 1,
        at.x + cellSize - cornerSize,
        at.y + cellSize - 1);
}

static void draw_interleaved_dots(Canvas* canvas, Coord at, uint8_t size) {
    canvas_set_color(canvas, ColorWhite);
    for(uint8_t i = 0; i < size; ++i) {
        for(uint8_t j = i % 2; j < size; j += 2) {
            canvas_draw_dot(canvas, at.x + i, at.y + j);
        }
    }
}

static void draw_grid_element_size12(Canvas* canvas, Coord at, Coord element, GameState* gs) {
    GridElement* elem = &gs->elements[element.x][element.y];

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, at.x, at.y, 12, 12);

    uint8_t terminate = count_edges(elem) == 1;
    canvas_set_color(canvas, ColorBlack);
    if(elem->edges[DIR_LEFT]) {
        canvas_draw_box(canvas, at.x, at.y + 4, 4 - terminate * 2, 4);
    }
    if(elem->edges[DIR_RIGHT]) {
        canvas_draw_box(canvas, at.x + 8 + terminate * 2, at.y + 4, 4 - terminate * 2, 4);
    }
    if(elem->edges[DIR_TOP]) {
        canvas_draw_box(canvas, at.x + 4, at.y, 4, 4 - terminate * 2);
    }
    if(elem->edges[DIR_BOTTOM]) {
        canvas_draw_box(canvas, at.x + 4, at.y + 8 + terminate * 2, 4, 4 - terminate * 2);
    }
    if(eqCoord(&element, &gs->startingPoint)) {
        canvas_draw_box(canvas, at.x + 2, at.y + 2, 8, 8);
        draw_interleaved_dots(canvas, createCoord(at.x + 4, at.y + 4), 4);
        canvas_set_color(canvas, ColorBlack);
    } else if(!terminate) {
        canvas_draw_box(canvas, at.x + 4, at.y + 4, 4, 4);
    } else {
        if(gs->reachable[element.x][element.y]) {
            canvas_draw_line(canvas, at.x + 4, at.y + 2, at.x + 7, at.y + 2);
            canvas_draw_line(canvas, at.x + 3, at.y + 3, at.x + 8, at.y + 3);
            canvas_draw_box(canvas, at.x + 2, at.y + 4, 8, 4);
            canvas_draw_line(canvas, at.x + 3, at.y + 8, at.x + 8, at.y + 8);
            canvas_draw_line(canvas, at.x + 4, at.y + 9, at.x + 7, at.y + 9);
        } else {
            canvas_draw_line(canvas, at.x + 4, at.y + 2, at.x + 2, at.y + 4);
            canvas_draw_line(canvas, at.x + 5, at.y + 2, at.x + 2, at.y + 5);
            canvas_draw_line(canvas, at.x + 7, at.y + 2, at.x + 9, at.y + 4);
            canvas_draw_line(canvas, at.x + 6, at.y + 2, at.x + 9, at.y + 5);
            canvas_draw_line(canvas, at.x + 2, at.y + 7, at.x + 4, at.y + 9);
            canvas_draw_line(canvas, at.x + 2, at.y + 6, at.x + 5, at.y + 9);
            canvas_draw_line(canvas, at.x + 9, at.y + 7, at.x + 7, at.y + 9);
            canvas_draw_line(canvas, at.x + 9, at.y + 6, at.x + 6, at.y + 9);
        }
    }
    if(eqCoord(&element, &gs->currentSelection)) {
        draw_selection(canvas, at, 12, 3);
    }
}

static void draw_grid_element_size10(Canvas* canvas, Coord at, Coord element, GameState* gs) {
    GridElement* elem = &gs->elements[element.x][element.y];

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, at.x, at.y, 10, 10);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_box(canvas, at.x + 4, at.y + 4, 2, 2);
    if(elem->edges[DIR_LEFT]) {
        canvas_draw_box(canvas, at.x, at.y + 4, 4, 2);
    }
    if(elem->edges[DIR_RIGHT]) {
        canvas_draw_box(canvas, at.x + 6, at.y + 4, 4, 2);
    }
    if(elem->edges[DIR_TOP]) {
        canvas_draw_box(canvas, at.x + 4, at.y, 2, 4);
    }
    if(elem->edges[DIR_BOTTOM]) {
        canvas_draw_box(canvas, at.x + 4, at.y + 6, 2, 4);
    }
    if(eqCoord(&element, &gs->startingPoint)) {
        canvas_draw_box(canvas, at.x + 2, at.y + 2, 6, 6);
        draw_interleaved_dots(canvas, createCoord(at.x + 3, at.y + 3), 4);
        canvas_set_color(canvas, ColorBlack);
    }
    if(count_edges(elem) == 1) {
        if(gs->reachable[element.x][element.y]) {
            canvas_draw_line(canvas, at.x + 3, at.y + 2, at.x + 6, at.y + 2);
            canvas_draw_line(canvas, at.x + 3, at.y + 7, at.x + 6, at.y + 7);
            canvas_draw_box(canvas, at.x + 2, at.y + 3, 6, 4);
        } else {
            canvas_draw_line(canvas, at.x + 3, at.y + 2, at.x + 6, at.y + 2);
            canvas_draw_line(canvas, at.x + 3, at.y + 7, at.x + 6, at.y + 7);
            canvas_draw_line(canvas, at.x + 2, at.y + 3, at.x + 2, at.y + 6);
            canvas_draw_line(canvas, at.x + 7, at.y + 3, at.x + 7, at.y + 6);
            canvas_set_color(canvas, ColorWhite);
            canvas_draw_box(canvas, at.x + 3, at.y + 3, 4, 4);
            canvas_set_color(canvas, ColorBlack);
            canvas_draw_dot(canvas, at.x + 3, at.y + 3);
            canvas_draw_dot(canvas, at.x + 6, at.y + 6);
            canvas_draw_dot(canvas, at.x + 3, at.y + 6);
            canvas_draw_dot(canvas, at.x + 6, at.y + 3);
        }
    }
    if(eqCoord(&element, &gs->currentSelection)) {
        draw_selection(canvas, at, 10, 3);
    }
}

uint8_t determine_element_size(Coord fieldSize) {
    const uint8_t availableSizes[] = {12, 10};
    uint8_t n = sizeof(availableSizes) / sizeof(availableSizes[0]);
    for(uint8_t i = 0; i < n; ++i) {
        if(SCREEN_WIDTH / availableSizes[i] >= fieldSize.x &&
           SCREEN_HEIGHT / availableSizes[i] >= fieldSize.y) {
            return availableSizes[i];
        }
    }
    return availableSizes[n - 1];
}

void draw_grid(Canvas* canvas, GameState* gs, uint8_t elementSize) {
    Coord startCorner = createCoord(
        SCREEN_WIDTH / 2 - gs->fieldSize.x * elementSize / 2,
        SCREEN_HEIGHT / 2 - gs->fieldSize.y * elementSize / 2);
    for(uint8_t i = 0; i < gs->fieldSize.x; ++i) {
        for(uint8_t j = 0; j < gs->fieldSize.y; ++j) {
            if(elementSize == 10) {
                draw_grid_element_size10(
                    canvas,
                    createCoord(startCorner.x + i * 10, startCorner.y + j * 10),
                    createCoord(i, j),
                    gs);
            } else if(elementSize == 12) {
                draw_grid_element_size12(
                    canvas,
                    createCoord(startCorner.x + i * 12, startCorner.y + j * 12),
                    createCoord(i, j),
                    gs);
            }
        }
    }
}

void draw_menu(Canvas* canvas, const MenuEntry* menu, uint8_t selectedIndex) {
    const uint8_t DLT_FRAME = 2;
    uint8_t nitems = menuSize(menu);
    canvas_set_font(canvas, FontSecondary);
    int item_h = canvas_current_font_height(canvas);
    int max_width = 0;
    for(uint8_t i = 0; i < nitems; ++i) {
        int w = canvas_string_width(canvas, menu[i].text);
        if(w > max_width) {
            max_width = w;
        }
    }
    max_width += 2;
    for(uint8_t i = 0; i < nitems; ++i) {
        canvas_set_color(canvas, ColorBlack);
        if(i == selectedIndex) {
            canvas_draw_box(
                canvas,
                SCREEN_WIDTH / 2 - max_width / 2,
                SCREEN_HEIGHT / 2 - item_h * nitems / 2 + i * item_h,
                max_width,
                item_h);
        }
        canvas_set_color(canvas, i == selectedIndex ? ColorWhite : ColorBlack);
        canvas_draw_str_aligned(
            canvas,
            SCREEN_WIDTH / 2,
            SCREEN_HEIGHT / 2 - item_h * nitems / 2 + i * item_h + item_h / 2,
            AlignCenter,
            AlignCenter,
            menu[i].text);
    }
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(
        canvas,
        SCREEN_WIDTH / 2 - max_width / 2 - DLT_FRAME,
        SCREEN_HEIGHT / 2 - item_h * nitems / 2 - DLT_FRAME,
        max_width + DLT_FRAME * 2,
        item_h * nitems + DLT_FRAME * 2,
        2);
}

void draw_about(Canvas* canvas) {
    uint8_t nitems = 0;
    while(AboutStrings[nitems] != NULL) ++nitems;
    canvas_set_font(canvas, FontSecondary);
    int item_h = canvas_current_font_height(canvas);
    int max_width = 0;
    for(uint8_t i = 0; i < nitems; ++i) {
        int w = canvas_string_width(canvas, AboutStrings[i]);
        if(w > max_width) {
            max_width = w;
        }
    }
    canvas_set_color(canvas, ColorBlack);
    for(uint8_t i = 0; i < nitems; ++i) {
        canvas_set_font(canvas, i == 0 ? FontPrimary : FontSecondary);
        canvas_draw_str_aligned(
            canvas,
            SCREEN_WIDTH / 2 - max_width / 2,
            SCREEN_HEIGHT / 2 - item_h * nitems / 2 + i * item_h + item_h / 2,
            AlignLeft,
            AlignCenter,
            AboutStrings[i]);
    }
}

void draw_winning(Canvas* canvas, GameState* gs) {
    canvas_set_font(canvas, FontPrimary);

    size_t s = snprintf(NULL, 0, SCORE_MESSAGE, gs->gameMoves);
    char moves[s + 1];
    // Use snprintf to combine the score message and the actual score
    snprintf(moves, s + 1, SCORE_MESSAGE, gs->gameMoves);

    int w = canvas_string_width(canvas, WINNING_MESSAGE);
    int h = canvas_current_font_height(canvas) * 2;
    const int paddingV = 2;
    const int paddingH = 4;

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(
        canvas,
        SCREEN_WIDTH / 2 - w / 2 - paddingH,
        SCREEN_HEIGHT / 2 - h / 2 - paddingV,
        w + paddingH * 2,
        h + paddingV * 2 + canvas_current_font_height(canvas));
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(
        canvas,
        SCREEN_WIDTH / 2 - w / 2 - paddingH,
        SCREEN_HEIGHT / 2 - h / 2 - paddingV,
        w + paddingH * 2,
        h + paddingV * 2 + canvas_current_font_height(canvas),
        2);
    canvas_draw_str_aligned(
        canvas, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, AlignCenter, AlignCenter, WINNING_MESSAGE);
    canvas_draw_str_aligned(
        canvas,
        SCREEN_WIDTH / 2,
        (SCREEN_HEIGHT / 2) + canvas_current_font_height(canvas),
        AlignCenter,
        AlignCenter,
        moves);
}

static void game_draw_callback(Canvas* canvas, void* ctx) {
    AppState* appState = (AppState*)ctx;
    furi_mutex_acquire(appState->mutex, FuriWaitForever);
    GameState* gs = &appState->gameState;

    canvas_clear(canvas);
    if(appState->status == ST_PLAYING) {
        draw_grid(canvas, gs, determine_element_size(gs->fieldSize));
    } else if(appState->status == ST_WINNING) {
        draw_grid(canvas, gs, determine_element_size(gs->fieldSize));
        draw_winning(canvas, gs);
    } else if(appState->status == ST_MAIN_MENU) {
        draw_menu(canvas, MainMenu, appState->currentMenuSelection);
    } else if(appState->status == ST_SELECTION_MENU) {
        draw_menu(canvas, SelectionMenu, appState->currentMenuSelection);
    } else if(appState->status == ST_ABOUT) {
        draw_about(canvas);
    }

    furi_mutex_release(appState->mutex);
}

static void game_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;

    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

uint8_t add_delta_looped(uint8_t idx, uint8_t nitems, int8_t delta) {
    int8_t v = (int8_t)idx;
    v += delta;
    while(v < 0) v += nitems;
    v %= nitems;
    return (uint8_t)v;
}

Coord get_field_size(uint8_t menuId) {
    if(menuId == MN_EASY) {
        return createCoord(5, 5);
    } else if(menuId == MN_MEDIUM) {
        return createCoord(10, 5);
    } else if(menuId == MN_HARD) {
        return createCoord(12, 6);
    }
    furi_assert(0);
    return createCoord(0, 0);
}

int32_t flipper_game_connect_wires(void* p) {
    UNUSED(p);
    furi_assert((uint16_t)MAX_FIELD_WIDTH * MAX_FIELD_HEIGHT < 256);

    InputEvent event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    AppState* appState = malloc(sizeof(AppState));
    appState->gameState.fieldSize = createCoord(0, 0);
    appState->status = ST_MAIN_MENU;

    // Allocate our appState->mutex
    appState->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, game_draw_callback, appState);
    view_port_input_callback_set(view_port, game_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);

    bool isFinishing = false;
    while(!isFinishing) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if((event.type != InputTypeShort) && (event.type != InputTypeLong)) continue;

        furi_mutex_acquire(appState->mutex, FuriWaitForever);

        if(appState->status == ST_PLAYING) {
            if(event.key == InputKeyBack) {
                appState->currentMenuSelection = 0;
                appState->status = ST_MAIN_MENU;
            } else if(event.key == InputKeyLeft) {
                moveSelection(&appState->gameState, DIR_LEFT);
            } else if(event.key == InputKeyRight) {
                moveSelection(&appState->gameState, DIR_RIGHT);
            } else if(event.key == InputKeyUp) {
                moveSelection(&appState->gameState, DIR_TOP);
            } else if(event.key == InputKeyDown) {
                moveSelection(&appState->gameState, DIR_BOTTOM);
            } else if(event.key == InputKeyOk) {
                if(event.type == InputTypeShort) {
                    rotateSelection(&appState->gameState);
                    recalculateReachables(&appState->gameState);
                    if(checkIsWinning(&appState->gameState)) {
                        appState->status = ST_WINNING;
                        notification_message_block(notification, &sequence_winning);
                    }
                } else if(event.type == InputTypeLong) {
                    // Switch rotation direction to opposite
                    appState->gameState.clockwise = !appState->gameState.clockwise;
                    if(appState->gameState.clockwise) {
                        notification_message_block(notification, &sequence_clockwise);
                    } else {
                        notification_message_block(notification, &sequence_counter_clockwise);
                    }
                }
            }
        } else if(appState->status == ST_WINNING) {
            appState->gameState.fieldSize = createCoord(0, 0);
            appState->status = ST_MAIN_MENU;
            appState->currentMenuSelection = 0;
        } else if(appState->status == ST_MAIN_MENU) {
            int sz = menuSize(MainMenu);
            if(event.key == InputKeyBack) {
                if(appState->gameState.fieldSize.x == 0 && appState->gameState.fieldSize.y == 0) {
                    isFinishing = true;
                }
                appState->status = ST_PLAYING;
            } else if(event.key == InputKeyUp) {
                appState->currentMenuSelection =
                    add_delta_looped(appState->currentMenuSelection, sz, -1);
            } else if(event.key == InputKeyDown) {
                appState->currentMenuSelection =
                    add_delta_looped(appState->currentMenuSelection, sz, 1);
            } else if(event.key == InputKeyOk) {
                uint8_t id = MainMenu[appState->currentMenuSelection].id;
                if(id == MN_NEWGAME) {
                    appState->status = ST_SELECTION_MENU;
                    appState->currentMenuSelection = 0;
                } else if(id == MN_ABOUT) {
                    appState->status = ST_ABOUT;
                } else if(id == MN_EXIT) {
                    isFinishing = true;
                }
            }
        } else if(appState->status == ST_SELECTION_MENU) {
            int sz = menuSize(SelectionMenu);
            if(event.key == InputKeyBack) {
                appState->status = ST_MAIN_MENU;
                appState->currentMenuSelection = 0;
            } else if(event.key == InputKeyUp) {
                appState->currentMenuSelection =
                    add_delta_looped(appState->currentMenuSelection, sz, -1);
            } else if(event.key == InputKeyDown) {
                appState->currentMenuSelection =
                    add_delta_looped(appState->currentMenuSelection, sz, 1);
            } else if(event.key == InputKeyOk) {
                uint8_t id = SelectionMenu[appState->currentMenuSelection].id;
                appState->gameState = createNewGameState(get_field_size(id));
                shuffle(&appState->gameState);
                recalculateReachables(&appState->gameState);
                appState->status = ST_PLAYING;
            }
        } else if(appState->status == ST_ABOUT) {
            // Any key goes back.
            appState->status = ST_MAIN_MENU;
            appState->currentMenuSelection = 0;
        }

        view_port_update(view_port);
        furi_mutex_release(appState->mutex);
    }

    furi_message_queue_free(event_queue);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    furi_record_close(RECORD_NOTIFICATION);

    // Should happen after freeing viewport because draw callback could try to acquire mutex.
    furi_mutex_free(appState->mutex);
    free(appState);
    return 0;
}
