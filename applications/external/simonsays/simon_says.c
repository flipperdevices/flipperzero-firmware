#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include <gui/icon.h>
#include <input/input.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <stdbool.h> // Header-file for boolean data-type.
#include <stdio.h>
#include <string.h>

#include "simon_says_icons.h"

#define TAG "Simon" // Used for logging
#define DEBUG_MSG 1
#define SCREEN_XRES 128
#define SCREEN_YRES 64
#define BOARD_X 72 // Used for board placement
#define BOARD_Y 8
#define GAME_START_LIVES 3
#define SAVING_DIRECTORY EXT_PATH("apps_data/simonsays")
#define SAVING_FILENAME SAVING_DIRECTORY "/game_simon_says.save"

// Define Notes
// Shamelessly stolen from Ocarina application
// https://github.com/invalidna-me/flipperzero-ocarina
#define NOTE_UP 587.33f
#define NOTE_LEFT 493.88f
#define NOTE_RIGHT 440.00f
#define NOTE_DOWN 349.23
#define NOTE_OK 293.66f

/* ============================ Data structures ============================= */

typedef enum game_state { preloading, mainMenu, inGame, gameOver, gameVictory } game_state;

typedef enum difficulty_mode { normal, hard } difficulty_mode;

typedef enum shape_names { up, down, left, right, number_of_shapes } Direction;

typedef enum currently_playing { simon, player } currently_playing;

typedef struct {
    /* Game state. */
    enum game_state gameState; // This is the current game state
    bool gameover; /* if true then switch to the game over state */
    bool is_wrong_direction; /* Is the last direction wrong? */
    enum currently_playing activePlayer; // This is used to track who is playing at the moment
    uint32_t lives; /* Number of lives in the current game. */

    enum difficulty_mode difficultyMode; // This is the difficulty mode for the current game
    bool sound_enabled; // This is the sound enabled flag for the current game
    float volume; // This is the volume for the current game

    /* Handle Score */
    int currentScore; // This is the score for the current
    int highScore; /* Highscore. Shown on Game Over Screen */
    bool is_new_highscore; /* Is the last score a new highscore? */

    /* Handle Shape Display */
    uint32_t numberOfMillisecondsBeforeShapeDisappears; // This defines the speed of the game
    enum shape_names simonMoves[1000]; // Store the sequence of shapes that Simon plays
    enum shape_names selectedShape; // This is used to track the shape that the player has selected
    bool set_board_neutral; // This is used to track if the board should be neutral or not
    int moveIndex; // This is used to track the current move in the sequence

    uint32_t last_button_press_tick;
    NotificationApp* notification;
    FuriMutex* mutex;
} SimonData;

/* ============================== Sequences ============================== */

const NotificationSequence sequence_wrong_move = {
    &message_red_255,

    &message_vibro_on,
    // &message_note_g5, // Play sound but currently disabled
    &message_delay_25,
    // &message_note_e5,
    &message_vibro_off,
    &message_sound_off,
    NULL,
};

const NotificationSequence sequence_player_submit_move = {
    &message_vibro_on,
    // &message_note_g5, // Play sound but currently disabled. Need On/Off menu setting
    &message_delay_10,
    &message_delay_1,
    &message_delay_1,
    &message_delay_1,
    &message_delay_1,
    &message_delay_1,

    // &message_note_e5,
    &message_vibro_off,
    &message_sound_off,
    NULL,
};

const NotificationSequence sequence_up = {
    // &message_vibro_on,
    &message_note_g4,
    &message_delay_100,
    // &message_vibro_off,
    &message_sound_off,
    NULL,
};

const NotificationSequence sequence_down = {
    // &message_vibro_on,
    &message_note_c3,
    &message_delay_100,
    // &message_vibro_off,
    &message_sound_off,
    NULL,
};

const NotificationSequence sequence_left = {
    // &message_vibro_on,
    &message_note_e3,
    &message_delay_100,
    // &message_vibro_off,
    &message_sound_off,
    NULL,
};

const NotificationSequence sequence_right = {
    // &message_vibro_on,
    &message_note_g3,
    &message_delay_100,
    // &message_vibro_off,
    &message_sound_off,
    NULL,
};

// Indicate that it's Simon's turn
const NotificationSequence sequence_simon_is_playing = {
    &message_red_255,
    &message_do_not_reset,
    NULL,
};

// Indicate that it's the Player's turn
const NotificationSequence sequence_player_is_playing = {
    &message_red_0,
    &message_do_not_reset,
    NULL,
};

const NotificationSequence sequence_cleanup = {
    &message_red_0,
    &message_green_0,
    &message_blue_0,
    &message_sound_off,
    &message_vibro_off,
    NULL,
};

/* ============================ 2D drawing ================================== */

/* Display remaining lives in the center of the board */
void draw_remaining_lives(Canvas* canvas, const SimonData* simon_state) {
    // Convert score to string
    // int length = snprintf(NULL, 0, "%lu", simon_state->lives);
    // char* str_lives_remaining = malloc(length + 1);
    // snprintf(str_lives_remaining, length + 1, "%lu", simon_state->lives);

    // TODO: Make it a Simon Says icon on top right
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    int x = SCREEN_XRES - 6;
    int lives = simon_state->lives;
    while(lives--) {
        canvas_draw_str(canvas, x, 8, "*");
        x -= 7;
    }
}

void draw_current_score(Canvas* canvas, const SimonData* simon_data) {
    /* Draw Game Score. */
    canvas_set_color(canvas, ColorXOR);
    canvas_set_font(canvas, FontSecondary);
    char str_score[32];
    snprintf(str_score, sizeof(str_score), "%i", simon_data->currentScore);
    canvas_draw_str_aligned(canvas, SCREEN_XRES / 2 + 4, 2, AlignCenter, AlignTop, str_score);
}

void play_sound_up(const SimonData* app) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_UP, app->volume);
    }
}

void play_sound_down(const SimonData* app) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_DOWN, app->volume);
    }
}

void play_sound_left(const SimonData* app) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_LEFT, app->volume);
    }
}

void play_sound_right(const SimonData* app) {
    if(furi_hal_speaker_is_mine() || furi_hal_speaker_acquire(30)) {
        furi_hal_speaker_start(NOTE_RIGHT, app->volume);
    }
}

void stop_sound() {
    if(furi_hal_speaker_is_mine()) {
        furi_hal_speaker_stop();
        furi_hal_speaker_release();
    }
}

/* Main Render Function */
void simon_draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    const SimonData* simon_state = ctx;
    furi_mutex_acquire(simon_state->mutex, FuriWaitForever);

    canvas_clear(canvas);

    // ######################### Main Menu #########################
    // Show Main Menu
    if(simon_state->gameState == mainMenu) {
        // Draw border frame
        canvas_draw_frame(canvas, 1, 1, SCREEN_XRES - 1, SCREEN_YRES - 1); // Border

        // Draw Simon text banner
        canvas_set_font(canvas, FontSecondary);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_str_aligned(
            canvas,
            SCREEN_XRES / 2,
            SCREEN_YRES / 2 - 4,
            AlignCenter,
            AlignCenter,
            "Welcome to Simon Says");

        // Display Press OK to start below title
        canvas_set_color(canvas, ColorXOR);
        canvas_draw_str_aligned(
            canvas,
            SCREEN_XRES / 2,
            SCREEN_YRES / 2 + 10,
            AlignCenter,
            AlignCenter,
            "Press OK to start");
    }

    // ######################### in Game #########################
    //@todo Render Callback
    // We're in an active game
    if(simon_state->gameState == inGame) {
        // Draw Current Score
        draw_current_score(canvas, simon_state);

        // Draw Lives
        draw_remaining_lives(canvas, simon_state);

        // Draw Simon Pose
        if(simon_state->activePlayer == player) {
            // Player's turn
            canvas_draw_icon(canvas, 0, 4, &I_DolphinWait_61x59);
        } else {
            // Simon's turn
            canvas_draw_icon(canvas, 0, 4, &I_DolphinTalking_59x63);
        }

        if(simon_state->set_board_neutral) {
            // Draw Neutral Board
            canvas_draw_icon(canvas, BOARD_X, BOARD_Y, &I_board); // Draw Board

            // Stop Sound TODO: Move this to a better place
            //@todo Sound
            stop_sound();
        } else {
            switch(simon_state->selectedShape) {
            case up:
                canvas_draw_icon(canvas, BOARD_X, BOARD_Y, &I_up); // Draw Up
                play_sound_up(simon_state);
                break;
            case down:
                canvas_draw_icon(canvas, BOARD_X, BOARD_Y, &I_down); // Draw Down
                play_sound_down(simon_state);
                break;
            case left:
                canvas_draw_icon(canvas, BOARD_X, BOARD_Y, &I_left); // Draw Left
                play_sound_left(simon_state);
                break;
            case right:
                canvas_draw_icon(canvas, BOARD_X, BOARD_Y, &I_right); // Draw Right
                play_sound_right(simon_state);
                break;
            default:
                if(DEBUG_MSG)
                    FURI_LOG_E(
                        TAG, "Invalid shape: %d", simon_state->simonMoves[simon_state->moveIndex]);
                break;
            }
        }
    }

    // ######################### Game Over #########################
    if(simon_state->gameState == gameOver) {
        stop_sound(); //TODO: Make a game over sequence
        canvas_set_color(canvas, ColorXOR);
        canvas_set_font(canvas, FontPrimary);

        // TODO: if new highscore, display blinking "New High Score"
        // Display High Score Text
        if(simon_state->is_new_highscore) {
            canvas_draw_str_aligned(
                canvas, SCREEN_XRES / 2, 6, AlignCenter, AlignTop, "New High Score!");
        } else {
            canvas_draw_str_aligned(
                canvas, SCREEN_XRES / 2, 6, AlignCenter, AlignTop, "High Score");
        }

        // Convert highscore to string
        int length = snprintf(NULL, 0, "%i", simon_state->highScore);
        char* str_high_score = malloc(length + 1);
        snprintf(str_high_score, length + 1, "%i", simon_state->highScore);

        // Display High Score
        canvas_draw_str_aligned(
            canvas, SCREEN_XRES / 2, 22, AlignCenter, AlignCenter, str_high_score);
        free(str_high_score);

        // Display Game Over
        canvas_draw_str_aligned(
            canvas, SCREEN_XRES / 2, SCREEN_YRES / 2 + 2, AlignCenter, AlignCenter, "GAME OVER");

        // Display Press OK to restart below title
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_str_aligned(
            canvas,
            SCREEN_XRES / 2,
            SCREEN_YRES / 2 + 15,
            AlignCenter,
            AlignCenter,
            "Press OK to restart");
    }

    // ######################### Victory #########################
    //Player Beat Simon beyond limit! A word record holder here!
    //TODO

    //release the mutex
    furi_mutex_release(simon_state->mutex);
}

/* ======================== Input Handling ============================== */

void simon_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    FuriMessageQueue* event_queue = ctx;
    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

/* ======================== Simon Game Engine ======================== */

bool load_game(SimonData* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);
    storage_common_copy(storage, EXT_PATH("apps/Games/game_simon_says.save"), SAVING_FILENAME);
    storage_common_remove(storage, EXT_PATH("apps/Games/game_simon_says.save"));

    File* file = storage_file_alloc(storage);

    uint16_t bytes_readed = 0;
    if(storage_file_open(file, SAVING_FILENAME, FSAM_READ, FSOM_OPEN_EXISTING)) {
        if(storage_file_size(file) > sizeof(SimonData)) {
            storage_simply_remove(storage, SAVING_FILENAME);
            FURI_LOG_E(
                TAG, "Error: file is larger than the data structure! The file has been deleted.");
        } else {
            bytes_readed = storage_file_read(file, app, sizeof(SimonData));
        }
        storage_file_close(file);
        storage_file_free(file);
    }

    furi_record_close(RECORD_STORAGE);
    return bytes_readed == sizeof(SimonData);
}

void save_game(SimonData* app) {
    Storage* storage = furi_record_open(RECORD_STORAGE);

    if(storage_common_stat(storage, SAVING_DIRECTORY, NULL) == FSE_NOT_EXIST) {
        if(!storage_simply_mkdir(storage, SAVING_DIRECTORY)) {
            return;
        }
    }

    File* file = storage_file_alloc(storage);
    if(storage_file_open(file, SAVING_FILENAME, FSAM_WRITE, FSOM_CREATE_ALWAYS)) {
        storage_file_write(file, app, sizeof(SimonData));
    }
    storage_file_close(file);
    storage_file_free(file);

    furi_record_close(RECORD_STORAGE);
}

int getRandomIntInRange(int lower, int upper) {
    return (rand() % (upper - lower + 1)) + lower;
}

void play_sound_sequence_correct() {
    notification_message(furi_record_open(RECORD_NOTIFICATION), &sequence_success);
}

void play_sound_wrong_move() {
    //TODO: play wrong sound: Try sequence_audiovisual_alert
    notification_message(furi_record_open(RECORD_NOTIFICATION), &sequence_error);
}

/* Restart game and give player a chance to try again on same sequence */
// @todo restartGame
void resetGame(SimonData* app) {
    app->moveIndex = 0;
    app->numberOfMillisecondsBeforeShapeDisappears = 500;
    app->activePlayer = simon;
    app->is_wrong_direction = false;
    app->last_button_press_tick = 0;
    app->set_board_neutral = true;
    app->activePlayer = simon;
}

/* Set gameover state */
void game_over(SimonData* app) {
    if(app->is_new_highscore) save_game(app); // Save highscore but only on change
    app->gameover = true;
    app->lives = GAME_START_LIVES; // Show 3 lives in game over screen to match new game start
    app->gameState = gameOver;
}

/* Called after gameover to restart the game. This function
 * also calls restart_game(). */
void restart_game_after_gameover(SimonData* app) {
    app->volume = 1.0f; //TODO: make this a setting
    app->gameState = inGame;
    app->gameover = false;
    app->currentScore = 0;
    app->is_new_highscore = false;
    app->lives = GAME_START_LIVES;
    app->simonMoves[0] = rand() % number_of_shapes;
    resetGame(app);
}

void addNewSimonMove(int addAtIndex, SimonData* app) {
    app->simonMoves[addAtIndex] = getRandomIntInRange(0, 3);
}

void startNewRound(SimonData* app) {
    addNewSimonMove(app->currentScore, app);
    app->moveIndex = 0;
    app->activePlayer = simon;
}

void onPlayerAnsweredCorrect(SimonData* app) {
    app->moveIndex++;
}

void onPlayerAnsweredWrong(SimonData* app) {
    if(app->lives > 0) {
        app->lives--;

        // Play the wrong sound
        if(app->sound_enabled) {
            play_sound_wrong_move();
        }
        resetGame(app);
    } else {
        // The player has no lives left
        // Game over
        game_over(app);
        //TODO: Play unique game over sound
    }
}

bool isRoundComplete(SimonData* app) {
    return app->moveIndex == app->currentScore;
}

enum shape_names getCurrentSimonMove(SimonData* app) {
    return app->simonMoves[app->moveIndex];
}

void onPlayerSelectedShapeCallback(enum shape_names shape, SimonData* app) {
    if(shape == getCurrentSimonMove(app)) {
        onPlayerAnsweredCorrect(app);
    } else {
        onPlayerAnsweredWrong(app);
    }
}

//@todo gametick
void game_tick(SimonData* simon_state) {
    if(simon_state->gameState == inGame) {
        if(simon_state->activePlayer == simon) {
            // ############### Simon Turn ###############
            notification_message(simon_state->notification, &sequence_simon_is_playing);

            //@todo Gameplay
            if(simon_state->set_board_neutral) {
                if(simon_state->moveIndex < simon_state->currentScore) {
                    simon_state->selectedShape = getCurrentSimonMove(simon_state);
                    simon_state->set_board_neutral = false;
                    simon_state->moveIndex++;
                } else {
                    simon_state->activePlayer = player;
                    simon_state->set_board_neutral = true;
                    simon_state->moveIndex = 0;
                }
            } else {
                simon_state->set_board_neutral = true;
            }
        } else {
            // ############### Player Turn ###############
            notification_message(simon_state->notification, &sequence_player_is_playing);

            // It's Player's Turn
            if(isRoundComplete(simon_state)) {
                simon_state->activePlayer = simon;
                simon_state->currentScore++;
                // app->numberOfMillisecondsBeforeShapeDisappears -= 50;
                //TODO: Hacky way of handling highscore by subtracting 1 to account for the first move
                if(simon_state->currentScore - 1 > simon_state->highScore) {
                    simon_state->highScore = simon_state->currentScore - 1;
                    simon_state->is_new_highscore = true;
                }
                if(simon_state->sound_enabled) {
                    play_sound_sequence_correct();
                }
                startNewRound(simon_state);
            }
        }
    }
}

/* ======================== Main Entry Point ============================== */

int32_t simon_says_app_entry(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    SimonData* simon_state = malloc(sizeof(SimonData));

    simon_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!simon_state->mutex) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(simon_state);
        return -1;
    }

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, simon_draw_callback, simon_state);
    view_port_input_callback_set(view_port, simon_input_callback, event_queue);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    NotificationApp* notification = furi_record_open(RECORD_NOTIFICATION);
    simon_state->notification = notification;

    InputEvent input;

    // Show Main Menu Screen
    //load_game(simon_state);
    restart_game_after_gameover(simon_state);
    simon_state->gameState = mainMenu;

    while(true) {
        game_tick(simon_state);

        FuriStatus q_status = furi_message_queue_get(
            event_queue, &input, simon_state->numberOfMillisecondsBeforeShapeDisappears);
        furi_mutex_acquire(simon_state->mutex, FuriWaitForever);

        if(q_status == FuriStatusOk) {
            //FURI_LOG_D(TAG, "Got input event: %d", input.key);
            //break out of the loop if the back key is pressed
            if(input.key == InputKeyBack && input.type == InputTypeLong) {
                // Save high score before quitting
                //if(simon_state->is_new_highscore) {
                //  save_game(simon_state);
                //}
                break;
            }

            //@todo Set Game States
            if(input.key == InputKeyOk && simon_state->gameState != inGame) {
                restart_game_after_gameover(simon_state);
                // Set Simon Board state
                startNewRound(simon_state);
                view_port_update(view_port);
            }

            // Keep LED on if it is Simon's turn
            if(simon_state->activePlayer == player) {
                notification_message(notification, &sequence_player_is_playing);

                if(input.type == InputTypePress) {
                    simon_state->set_board_neutral = false;

                    switch(input.key) {
                    case InputKeyUp:
                        simon_state->selectedShape = up;
                        onPlayerSelectedShapeCallback(up, simon_state);
                        break;
                    case InputKeyDown:
                        simon_state->selectedShape = down;
                        onPlayerSelectedShapeCallback(down, simon_state);
                        break;
                    case InputKeyLeft:
                        simon_state->selectedShape = left;
                        onPlayerSelectedShapeCallback(left, simon_state);
                        break;
                    case InputKeyRight:
                        simon_state->selectedShape = right;
                        onPlayerSelectedShapeCallback(right, simon_state);
                        break;
                    default:
                        simon_state->set_board_neutral = true;
                        break;
                    }
                } else {
                    //FURI_LOG_D(TAG, "Input type is not short");
                    simon_state->set_board_neutral = true;
                }
            }
        }

        // @todo Animation Loop for debug
        // if(simon_state->gameState == inGame && simon_state->activePlayer == simon) {
        //     simon_state->currentScore++;
        //     simon_state->set_board_neutral = !simon_state->set_board_neutral;
        // }

        view_port_update(view_port);
        furi_mutex_release(simon_state->mutex);
    }

    stop_sound();
    notification_message(notification, &sequence_cleanup);
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(simon_state->mutex);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    free(simon_state);

    return 0;
}
