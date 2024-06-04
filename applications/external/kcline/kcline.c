/*********************************************
 *  KC Line
 *
 *
 *              by Andrew Diamond (HappyAmos)
 *              2024 * MIT License
 *              https://gihub.com/HappyAmos
 *********************************************/
#include <gui/scene_manager.h>
#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <input/input.h>
#include <gui/modules/widget.h>

#include <notification/notification.h>
#include <notification/notification_messages.h>

// Include the icons directory in our project
#include "kcline_icons.h"

#define TAG "kcline" // For logging

const uint8_t TICKS = 40; // Speed of game ticks
const uint8_t EVENT_TYPE_TICK = 0;
const uint8_t TICK_ADVANCE = 3; // How many ticks to reduce for each level.

const uint8_t KC_MOVE_WIDTH = 1; // in pixels
const uint8_t GHOST_MOVE_WIDTH = 1;
const uint8_t MAX_ROWS = 64;
const uint8_t MAX_COLS = 128;

// KC Sprite is 10 x 10
// Ghost sprite is 10 x 10
// Bit sprites are 5 x 5

#define NUM_BITS 10
const uint8_t BIT_SPACING = 13;
const uint8_t BIT_WIDTH = 5; // Required for drawing and checking collisions.
const uint8_t BIT_HEIGHT = 5;
const uint16_t BIT_TICK_COOLDOWN = 80;
const uint8_t BIT_RANDOM = 15; // 1 in BIT_RANDOM chance of regenerating bit
const uint8_t GHOST_WIDTH = 5; // Required for checking collisions
const uint8_t KC_WIDTH = 10;
const uint8_t KC_HEIGHT = 10;
const uint8_t GHOST_COLLISION_ADJUSTMENT = 6;
const uint16_t MAX_ANIMATION_TICKS =
    10; // animation occurs when this is >= MAX_ANIMATIONS_TICKS / 2

const uint8_t KC_START_COL = 3; // Starting positions!
const uint8_t KC_START_ROW = 24;
const uint8_t GHOST_START_COL = 62;
const uint8_t GHOST_START_ROW = 24;
const uint8_t KC_LIVES = 3;

const char GAME_OVER_MESSAGE_LINE_1[] = "Game Over";
const char GAME_OVER_MESSAGE_LINE_2[] = "Score: %d";

enum AppScene {
    SCENE_MENU,
    SCENE_GAME,
    SCENE_QUIT,
    SCENE_LOSE,
    SCENE_INIT,
    SCENE_DIED,
    SCENE_ABOUT,
};

// For animating the characters
enum Icons {
    KC_ICON_1,
    KC_ICON_2,
    GHOST_ICON_1,
    GHOST_ICON_2,
};

typedef struct // TODO not used ATM, copied because I liked the ASCII art.
{
    //    +-----x
    //    |
    //    |
    //    y
    uint8_t x;
    uint8_t y;
} Point;

/* The direction an entity is moving */
typedef enum {
    STOP,
    RIGHT,
    LEFT,
} Directions;

typedef struct {
    uint8_t row;
    uint8_t col;
    uint8_t icon; // Icons
    Directions direction; // As defined above, left, right, or stop
} Entity;

typedef struct {
    bool enabled; // true = it's there, false means it snot
    uint8_t col;
    uint16_t tick_cooldown;
} Bit;

typedef struct {
    uint16_t score;
    uint8_t lives;
    Bit bits[NUM_BITS];
    Entity ghost;
    Entity kc;
    uint16_t ticks; // game level tracking of ticks
    uint8_t level;
} Game;

typedef struct {
    FuriMutex* mutex; // Does some magic I have't wrapped my head around yet.
    uint8_t ticks; // Furi ticks
    uint8_t scene;
    Game game;
    uint8_t menuIndex;
    uint8_t scrollMax;
    uint8_t scrollIndex;
} App;

enum MenuEntryId {
    MN_NEWGAME,
    MN_ABOUT,
    MN_EXIT,
};

typedef struct {
    uint8_t id;
    char* text;
} MenuEntry;

const MenuEntry MAIN_MENU[] = {
    {MN_NEWGAME, "New Game"},
    {MN_ABOUT, "About"},
    {MN_EXIT, "Exit"},
    {0, NULL},
};

uint8_t menuSize(const MenuEntry* menu) {
    uint8_t res = 0;
    while(menu[res].text != 0) res++;
    return res;
}

// Initialize all the yummy bits
// .enabled: true = not eaten yet.
// .col: pretty obvious, this.
// .tick_cooldown: countdown timer that causes bit to become re-enabled
static void reset_bits(App* app) {
    uint8_t col = 2; //, row = 27;
    for(uint8_t i = 0; i < NUM_BITS; i++) {
        // Make sure we don't give them the one that shows up under KC at the beginning. No freebies!
        if(i == 0) {
            app->game.bits[i].enabled = false;
        } else {
            app->game.bits[i].enabled = true;
        }
        app->game.bits[i].col = col;
        app->game.bits[i].tick_cooldown = 0;
        col = col + BIT_SPACING;
    }
}

/* input = source number
 * output = number to search for multiples of */
static uint8_t count_multiples(uint16_t input, uint16_t output) {
    uint8_t count = 0;

    for(uint16_t i = 1; i <= input; i++) {
        if(i % output == 0) {
            count++;
        }
    }
    //FURI_LOG_D(TAG, "Score: %d, multiples of %d: %d", input, output, count);
    return count;
}

/* For looping through menus or whatever 
 * idx = current selection
 * nitems = pretty obvious
 * delta = increment, which also doubles as direction. So:
 *   -1 = up in a menu
 *    1 = down in a menu
 * */
uint8_t add_delta_looped(uint8_t idx, uint8_t nitems, int8_t delta) {
    int8_t v = (int8_t)idx;
    v += delta;
    while(v < 0) v += nitems;
    v %= nitems;
    return (uint8_t)v;
}

/* #defaults - Set game defaults */
static void initialize_game(App* app) {
    FURI_LOG_I(TAG, "initializing_game");
    app->scene = SCENE_MENU;
    reset_bits(app);
    app->game.kc.direction = STOP;
    app->game.kc.row = KC_START_ROW;
    app->game.kc.col = KC_START_COL;
    app->game.ghost.direction = STOP;
    app->game.ghost.row = GHOST_START_ROW;
    app->game.ghost.col = GHOST_START_COL;
    app->game.score = 0;
    app->game.lives = KC_LIVES;
    app->game.level = 0;
    app->ticks = TICKS;
    app->menuIndex = MN_NEWGAME;
    app->scrollIndex = 0;
}

static void draw_scene_scene_about(Canvas* canvas, App* app) {
    app->scrollMax = 0; // initialize our scrollbar maximum value
    char* string[] = {
        "About",
        "-------------------",
        "This is a simple one line",
        "game where the player tries",
        "to eat dots, and not get",
        "killed by the enemy.",
        "",
        "It's pretty obvious where",
        "the inspiration for this",
        "game came from, but if",
        "you have questions hit",
        "me up on my Github page.",
        "",
        "Directions are pretty",
        "simple, use OK button to",
        "pause, left and right to",
        "scroll left or right, ",
        "back to exit.",
        "",
        "Special thanks to to",
        "those that share their",
        "code with others. I",
        "relied heavily on",
        "referencing others",
        "work for this project.",
        "Specifically, Alexander Taran",
        "for the Connect Wires game,",
        "plus the F0 team for letting",
        "us dig around the firmware",
        "code.",
        "",
        "Game written by",
        "Andrew Diamond, 2024",
        "under the MIT license",
        "",
        "github.com/HappyAmos",
        NULL,
    }; // Make sure last entry is NULL, that's our EOL flag for below

    uint8_t nitems = 0; // Holds the number of lines in About text
    uint8_t item_h = canvas_current_font_height(canvas); // font height
    uint8_t nrows =
        MAX_ROWS / item_h; // Calculate the number of rows this font can display on screen

    while(string[nitems] != NULL) ++nitems; // count the number of items in about string thing

    app->scrollMax = nitems - 1; // sets the maximum for our app->scrollIndex incrementor

    uint8_t
        row = 10,
        row_i =
            0; // row = starting row to print on canvas, row_i is the current "text row" we are on in the loop

    canvas_set_font(canvas, FontSecondary);

    for(uint8_t i = app->scrollIndex; i < app->scrollIndex + nrows; i++) {
        if(app->scrollIndex + row_i < nitems) {
            canvas_draw_str(canvas, 1, row, string[app->scrollIndex + row_i]);
        }
        row = row + item_h;
        row_i++;
    }

    elements_scrollbar(canvas, app->scrollIndex, nitems);
}

/* Draw handlers */
static void draw_scene_scene_menu(Canvas* canvas, App* app) {
    uint8_t selectedIndex = app->menuIndex;

    const uint8_t DLT_FRAME = 2;
    uint8_t nitems = menuSize(MAIN_MENU);
    canvas_set_font(canvas, FontSecondary);
    int item_h = canvas_current_font_height(canvas);
    int max_width = 0;
    for(uint8_t i = 0; i < nitems; ++i) {
        int w = canvas_string_width(canvas, MAIN_MENU[i].text);
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
                MAX_COLS / 2 - max_width / 2,
                MAX_ROWS / 2 - item_h * nitems / 2 + i * item_h,
                max_width,
                item_h);
        }
        canvas_set_color(canvas, i == selectedIndex ? ColorWhite : ColorBlack);
        canvas_draw_str_aligned(
            canvas,
            MAX_COLS / 2,
            MAX_ROWS / 2 - item_h * nitems / 2 + i * item_h + item_h / 2,
            AlignCenter,
            AlignCenter,
            MAIN_MENU[i].text);
    }
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(
        canvas,
        MAX_COLS / 2 - max_width / 2 - DLT_FRAME,
        MAX_ROWS / 2 - item_h * nitems / 2 - DLT_FRAME,
        max_width + DLT_FRAME * 2,
        item_h * nitems + DLT_FRAME * 2,
        2);
}

/* Show the player how many lives they have left */
static void draw_scene_scene_died(Canvas* canvas, App* app) {
    // Draw "lives" icons
    uint8_t col = (MAX_COLS / 2) - ((3 * (10 + 2)) / 2);
    uint8_t row = MAX_ROWS / 2 - (10 / 2);
    for(uint8_t i = 0; i < KC_LIVES; i++) {
        if(i < app->game.lives) {
            canvas_draw_icon(canvas, col + (i * 10) + 2, row, &I_kc1);
        }
        if(i >= app->game.lives) {
            //FURI_LOG_D(TAG, "Lives: %d, i: %d", app->game.lives, i);
            canvas_draw_line(
                canvas,
                col + (i * 10) + 2,
                row + KC_HEIGHT - 1,
                col + (i * 10) + 2 + KC_WIDTH,
                row);
            canvas_draw_line(
                canvas,
                col + (i * 10) + 2,
                row,
                col + (i * 10) + 2 + KC_WIDTH,
                row + KC_HEIGHT - 1);
        }
    }

    uint8_t upperLeftCol = MAX_COLS / 2 - (((10 + 2) * 3) / 2) - 2;
    uint8_t upperLeftRow = MAX_ROWS / 2 - (10 - 2);
    uint8_t bottomRightCol = ((10 + 2) * 3) + 2;
    uint8_t bottomRightRow = 10 + 2 + 4;
    canvas_draw_rframe(canvas, upperLeftCol, upperLeftRow, bottomRightCol, bottomRightRow, 2);
    canvas_draw_str_aligned(
        canvas, MAX_COLS / 2, MAX_ROWS - 10, AlignCenter, AlignCenter, "< Press OK >");
}

/* Game over, no more lives */
static void draw_scene_scene_lose(Canvas* canvas, App* app) {
    canvas_set_font(canvas, FontPrimary);

    // Use snprintf to combine the score message and the actual score
    size_t sz = snprintf(NULL, 0, "%s:%u", GAME_OVER_MESSAGE_LINE_2, app->game.score);
    char score_message[sz];
    snprintf(score_message, sz, GAME_OVER_MESSAGE_LINE_2, app->game.score);

    const int num_text_rows = 2;
    const int paddingV = 2;
    const int paddingH = 4;
    const int w = canvas_string_width(canvas, GAME_OVER_MESSAGE_LINE_1);
    const int h = (canvas_current_font_height(canvas) * num_text_rows);

    canvas_set_color(canvas, ColorWhite); // clears message area
    canvas_draw_box(
        canvas,
        MAX_COLS / 2 - w / 2 - paddingH,
        MAX_ROWS / 2 - (h / 2) - paddingV * 2 - 1,
        w + paddingH * 2,
        h + paddingV * 2);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_rframe(
        canvas,
        MAX_COLS / 2 - w / 2 - paddingH,
        MAX_ROWS / 2 - (h / 2) - paddingV * 2 - 1,
        w + paddingH * 2,
        h + paddingV * 2,
        2);

    canvas_draw_str_aligned(
        canvas,
        MAX_COLS / 2,
        MAX_ROWS / 2 - h / 2 + paddingV,
        AlignCenter,
        AlignCenter,
        GAME_OVER_MESSAGE_LINE_1);
    canvas_draw_str_aligned(
        canvas, MAX_COLS / 2, (MAX_ROWS / 2) + paddingV, AlignCenter, AlignCenter, score_message);
}

static void draw_scene_scene_game(Canvas* canvas, App* app) {
    // This instruction no need details :)
    canvas_clear(canvas);

    canvas_set_bitmap_mode(
        canvas, true); // if not true, i think we lose transparency on loaded pngs

    // Draw score
    canvas_set_font(canvas, FontPrimary);
    size_t sz;
    char* score_format = "Score: %d";
    sz = snprintf(NULL, 0, score_format, app->game.score) + 1;
    char score_output[sz];
    sz = snprintf(score_output, sz, score_format, app->game.score);
    //canvas_draw_str(canvas, 5, 10, score_output);
    canvas_draw_str_aligned(canvas, 5, 6, AlignLeft, AlignCenter, score_output);

    // If game is paused, display paused message
    if(app->game.kc.direction == STOP) {
        char* paused = "Paused";
        //uint8_t w = canvas_string_width(canvas, paused);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(canvas, 85, 6, AlignCenter, AlignCenter, paused);
    }

    // TODO Reverse this loop so that they are right aligned
    // Draw "lives" icons
    uint8_t col = 94;
    for(uint8_t i = 0; i < app->game.lives; i++) {
        canvas_draw_icon(canvas, col + (i * 10) + 2, 53, &I_kc1);
    }

    // Draw borders
    canvas_draw_line(canvas, 0, 19, 127, 19);
    canvas_draw_line(canvas, 127, 39, 0, 39);

    // Draw valid dots
    uint8_t row = 27;
    for(uint8_t i = 0; i < NUM_BITS; i++) {
        if(app->game.bits[i].enabled == true) { // true = its there, false = it's eaten.
            canvas_draw_frame(canvas, app->game.bits[i].col, row, BIT_WIDTH, BIT_HEIGHT);
            canvas_draw_frame(canvas, app->game.bits[i].col + 1, row + 1, 3, 3);
        } else {
            if(app->game.kc.direction != STOP) { // Do this next bit if we aren't paused.
                // Eaten bits are restored after a cooldown period.
                if(app->game.bits[i].tick_cooldown > 0) {
                    app->game.bits[i].tick_cooldown--;
                } else {
                    // Make it interesting. Bits only reappear after their cooldown, and then only have a 1 in X (BIT_RANDOM)
                    // chance of appearing each tick cycle.
                    uint8_t rdm = rand() % BIT_RANDOM;
                    if(rdm == 1) app->game.bits[i].enabled = true;
                    //FURI_LOG_D(TAG, "Random: %d", rdm);
                }
            }
        }
    }

    // Draw KC
    if(app->game.ticks < MAX_ANIMATION_TICKS / 2) {
        canvas_draw_icon(canvas, app->game.kc.col, app->game.kc.row, &I_kc1);
    } else {
        canvas_draw_icon(canvas, app->game.kc.col, app->game.kc.row, &I_kc2);
    }

    // Draw Ghost
    if(app->game.ticks < MAX_ANIMATION_TICKS / 2) {
        canvas_draw_icon(canvas, app->game.ghost.col, app->game.ghost.row, &I_ghost1);
    } else {
        canvas_draw_icon(canvas, app->game.ghost.col, app->game.ghost.row, &I_ghost2);
    }
}

// This function is for drawing the screen GUI, everytime
// the Flip.x0 refresh the display
static void draw_callback(Canvas* canvas, void* ctx) {
    App* app = (App*)ctx;
    furi_mutex_acquire(app->mutex, FuriWaitForever);

    switch(app->scene) {
    case SCENE_MENU:
        // TODO popup menu here for new game/quit/resume/pause state
        draw_scene_scene_menu(canvas, app);
        break;
    case SCENE_LOSE:
        draw_scene_scene_lose(canvas, app);
        break;
    case SCENE_GAME:
        draw_scene_scene_game(canvas, app);
        break;
    case SCENE_INIT:
        initialize_game(app);
        break;
    case SCENE_DIED:
        draw_scene_scene_died(canvas, app);
        break;
    case SCENE_ABOUT:
        draw_scene_scene_about(canvas, app);
        break;
        // TODO case SCENE_PAUSED:
        // 			draw_scene_scene_paused (canvas, app);
        // 			break;
    }

    furi_mutex_release(app->mutex);
}

/* #collisions */
static void check_for_collisions(InputEvent* event, App* app) {
    UNUSED(event);

    /*  Check to see if bit was eated. */
    for(uint8_t i = 0; i < NUM_BITS; i++) {
        if(app->game.bits[i].enabled) {
            if(app->game.kc.col >= app->game.bits[i].col &&
               app->game.kc.col <= app->game.bits[i].col + BIT_WIDTH) {
                app->game.bits[i].enabled = false;
                app->game.bits[i].tick_cooldown = BIT_TICK_COOLDOWN;
                app->game.score++;
            }
        }
    }

    /* Check if KC scrolled over to the left or right side of the screen */
    // Went over left side
    if((app->game.kc.col < 3) && app->game.kc.direction == LEFT) {
        app->game.kc.col = MAX_COLS - 3; // Reset KC position to far right
        //FURI_LOG_D(TAG, "Set KC Position to MAX_COL - 3: %d ", app->game.kc.col);
        return;
    }
    // Went over right side
    if((app->game.kc.col > MAX_COLS - 3) && app->game.kc.direction == RIGHT) {
        app->game.kc.col = 3; // Reset KC position to far left
        //FURI_LOG_D(TAG, "Set KC Position to 0 + 3: %d ", app->game.kc.col);
        return;
    }

    /*  Uh oh! KC and the ghost touched each other, this is not appropriate! */
    if(((app->game.kc.col + (KC_WIDTH / 2) >= app->game.ghost.col) &&
        (app->game.kc.col <= app->game.ghost.col + GHOST_WIDTH) &&
        app->game.kc.direction == RIGHT) ||
       ((app->game.kc.col <= app->game.ghost.col + (GHOST_WIDTH / 2)) &&
        (app->game.kc.col >= app->game.ghost.col) && app->game.kc.direction == LEFT)) {
        // STOP all players and reset positions.
        //FURI_LOG_D(TAG, "<Collision>  KC: %d, GHOST: %d", app->game.kc.col, app->game.ghost.col);
        app->game.kc.direction = STOP;
        app->game.ghost.direction = STOP;
        app->game.kc.col = KC_START_COL;
        app->game.ghost.col = GHOST_START_COL;
        // Change the available number of lives.
        if(app->game.lives > 0) {
            app->game.lives--;
            app->scene = SCENE_DIED;
        } else {
            // Game over dude!
            app->scene = SCENE_LOSE;
        }
    }
}

/* #movement */
static void character_movement(InputEvent* event, App* app) {
    /* Move KC */
    switch(app->game.kc.direction) {
    case LEFT:
        app->game.kc.col = app->game.kc.col - KC_MOVE_WIDTH;
        break;
    case RIGHT:
        app->game.kc.col = app->game.kc.col + KC_MOVE_WIDTH;
        break;
    case STOP:
        break;
    }

    /* Move Ghost */
    if(app->game.kc.direction != STOP) {
        switch(app->game.ghost.direction) {
        case LEFT:
            app->game.ghost.col = app->game.ghost.col - GHOST_MOVE_WIDTH;
            break;
        case RIGHT:
            app->game.ghost.col = app->game.ghost.col + GHOST_MOVE_WIDTH;
            break;
        case STOP: // This obviously won't run, but the compiler was complaining, so here it is.
            break;
        }

        if(app->game.kc.col < app->game.ghost.col) {
            app->game.ghost.direction = LEFT;
        }
        if(app->game.kc.col > app->game.ghost.col) {
            app->game.ghost.direction = RIGHT;
        }
        if(app->game.ghost.col < GHOST_WIDTH) {
            app->game.ghost.direction = RIGHT;
        }
        if(app->game.ghost.col > MAX_COLS - GHOST_WIDTH) {
            app->game.ghost.direction = LEFT;
        }
        // walkabout
        if(abs(app->game.ghost.col - app->game.kc.col) > 80) {
            uint8_t rdm = rand() % 3;
            if(rdm > 1) {
                if(app->game.ghost.direction == RIGHT) {
                    app->game.ghost.direction = LEFT;
                } else if(app->game.ghost.direction == LEFT) {
                    app->game.ghost.direction = RIGHT;
                }
            }
        }
    }

    check_for_collisions(event, app);
}

/* #input Input handlers */
static void input_handler_scene_menu(InputEvent* event, App* app) {
    uint8_t sz = menuSize(MAIN_MENU);
    if(event->key == InputKeyUp) {
        app->menuIndex = add_delta_looped(app->menuIndex, sz, -1);
    } else if(event->key == InputKeyDown) {
        app->menuIndex = add_delta_looped(app->menuIndex, sz, 1);
    } else if(event->key == InputKeyOk) {
        switch(app->menuIndex) {
        case MN_NEWGAME:
            initialize_game(app);
            app->scene = SCENE_GAME;
            break;
        case MN_EXIT:
            app->scene = SCENE_QUIT;
            break;
        case MN_ABOUT:
            app->scene = SCENE_ABOUT;
            break;
        }
    } else if(event->key == InputKeyBack) {
        app->scene = SCENE_QUIT;
    }
}

static void input_handler_scene_lose(InputEvent* event, App* app) {
    if(event->key == InputKeyOk || event->key == InputKeyBack) {
        app->scene = SCENE_MENU;
    }
}

static void input_handler_scene_died(InputEvent* event, App* app) {
    if(event->key == InputKeyOk || event->key == InputKeyBack) {
        app->scene = SCENE_GAME;
    }
}

static void input_handler_scene_about(InputEvent* event, App* app) {
    if(event->key == InputKeyOk || event->key == InputKeyBack) {
        app->scene = SCENE_MENU;
        return;
    }

    if(event->key == InputKeyUp) {
        if(app->scrollIndex > 0) {
            app->scrollIndex--;
        }
    } else if(event->key == InputKeyDown) {
        if(app->scrollIndex < app->scrollMax) {
            app->scrollIndex++;
        }
    }
}

static void input_handler_scene_game(InputEvent* event, App* app) {
    switch(event->key) {
    case InputKeyLeft:
        app->game.kc.direction = LEFT;
        FURI_LOG_I(TAG, "Left navigation button keypress detected.");
        break;
    case InputKeyRight:
        app->game.kc.direction = RIGHT;
        FURI_LOG_I(TAG, "Right navigation button keypress detected.");
        break;
    case InputKeyBack:
        // pause menu
        app->scene = SCENE_MENU;
        FURI_LOG_I(TAG, "Back button keypress detected.");
        break;
    case InputKeyOk:
        app->game.kc.direction = STOP;
        FURI_LOG_I(TAG, "OK button keypress detected.");
        break;
    default:
        break;
    }
}

static void input_event_handler(InputEvent* event, App* app) {
    switch(app->scene) {
    case SCENE_MENU:
        input_handler_scene_menu(event, app);
        break;
    case SCENE_GAME:
        input_handler_scene_game(event, app);
        break;
    case SCENE_LOSE:
        input_handler_scene_lose(event, app);
        break;
    case SCENE_DIED:
        input_handler_scene_died(event, app);
        break;
    case SCENE_ABOUT:
        input_handler_scene_about(event, app);
        break;
    }
}

// This function is an handler for the user input (the buttons on the right
// of the Flip.x0 used for navigate, confirm and back)
static void input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

static void kcline_update_timer_callback(void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);
    FuriMessageQueue* event = event_queue;
    furi_message_queue_put(event_queue, &event, 0);
}

/* Main entry of the application as defined inside the application.fam 
 * #main
 * */
int32_t kcline_main(void* p) {
    UNUSED(p);

    // Current event of type InputEvent
    InputEvent event;

    // Event queue for 8 elements of size InputEvent
    FuriMessageQueue* event_queue = furi_message_queue_alloc(6, sizeof(InputEvent));

    // Allocate our global app state
    App* app = malloc(sizeof(App));

    initialize_game(app); // initialize game variables

    app->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!app->mutex) {
        FURI_LOG_E(TAG, "Cannot create mutex\r\n");
        free(app);
        furi_message_queue_free(event_queue);
        return 255;
    }

    // Start a furitimer to track time in our game
    FuriTimer* tick_timer =
        furi_timer_alloc(kcline_update_timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(tick_timer, app->ticks);
    furi_check(tick_timer);

    // ViewPort is need to draw the GUI
    ViewPort* view_port = view_port_alloc();

    // we give to this [view_port_draw_callback_set] the function defined
    // before (draw_callback) for drawing the GUI on the Flip.x0 display
    view_port_draw_callback_set(view_port, draw_callback, app);

    // The same concept is with [view_port_input_callback_set] associated to the (input_callback)
    // function defined before.
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // we give to this [view_port_draw_callback_set] the function defined
    // before (draw_callback) for drawing the GUI on the Flip.x0 display
    view_port_draw_callback_set(view_port, draw_callback, app);

    // Debugging
    if(!view_port_is_enabled(view_port)) {
        FURI_LOG_E(TAG, "view_port not enabled");
    }

    // You need to create a GUI structure and associate it to the viewport previously defined
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    // Infinite loop...(like arduino or similar) BUT it only runs when an event occurs. We
    // need to trigger an event to make a loop. Time to make a timer.
    while(app->scene != SCENE_QUIT) {
        // We continue (indefinitely) to get out of the queue all the events stacked inside
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, FuriWaitForever);
        furi_mutex_acquire(app->mutex, FuriWaitForever);

        // Handle keypress and tick events
        if(event_status == FuriStatusOk &&
           (event.type == InputTypeShort || event.type == EVENT_TYPE_TICK)) {
            if(event.type == InputTypeShort) {
                furi_timer_stop(tick_timer);
                input_event_handler(&event, app);
                furi_timer_restart(tick_timer, app->ticks);
            }
            if(event.type == EVENT_TYPE_TICK) {
                if(app->game.ticks < MAX_ANIMATION_TICKS) { // Routine to handle animation
                    app->game.ticks++;
                } else {
                    app->game.ticks = 0;
                }
                /* TODO: I don't like scene specific stuff in the main fn, but right now it's the only part of the game
				 * using ticks. If I ever add anything else, create a "tick_handler" function that organizes the code
				 * by app->scene. */
                if(app->scene == SCENE_GAME) {
                    /* if the user started the game, movement is non stop until they press ok. as soon as they press
					 * left or right, it, is, on! */
                    character_movement(&event, app);

                    /* Speed up the game as the user increases their level, make them work for them points! */
                    if((count_multiples(app->game.score, 30) > app->game.level) &&
                       (app->ticks > TICK_ADVANCE)) {
                        FURI_LOG_D(TAG, "Level advance, app->ticks: %d", app->ticks);
                        app->ticks = app->ticks - TICK_ADVANCE;
                        app->game.level++;
                    }
                }
            }
        } // keypress and tick events
        view_port_update(view_port);
        furi_mutex_release(app->mutex);
    }

    // once exit from the loop, we need to free resources:
    // clear all the element inside the queue
    FURI_LOG_I(TAG, "Freeing tick_timer");
    furi_timer_stop(tick_timer);
    furi_timer_free(tick_timer);

    FURI_LOG_I(TAG, "Freeing event_queue");
    furi_message_queue_free(event_queue);

    // Free app memory
    FURI_LOG_I(TAG, "Freeing app");
    furi_mutex_free(app->mutex);
    free(app);

    // We remove the gui from the associated view port
    FURI_LOG_I(TAG, "Removing view_port from gui");
    gui_remove_view_port(gui, view_port);

    // Freeing up memory removing the view_port and close
    // the GUI record
    FURI_LOG_I(TAG, "Freeing view_port");
    view_port_free(view_port);

    FURI_LOG_I(TAG, "Closing furi record");
    furi_record_close(RECORD_GUI);

    return 0;
}
