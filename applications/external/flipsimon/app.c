#include <furi.h>
#include <gui/view.h>
#include "app_config.h"
#include "app_icons.h"
#include "./common/config_colors.h"
#include "./common/custom_event.h"
#include "./common/flipboard.h"
#include "./common/flipboard_model.h"
#include "./common/flipboard_model_ref.h"
#include "./common/leds.h"

#define MAX_SONG_LENGTH 12
#define SIMON_TEACH_DELAY_MS 1000

uint16_t delays[] = {500, 500, 400, 300, 250, 200, 150, 100, 80};

typedef enum SimonGameState SimonGameState;
enum SimonGameState {
    /// @brief Ready for user to start a new game.
    SimonGameStateGameOver,

    /// @brief Populating a new game
    SimonGameStateNewGame,

    /// @brief Teaching the user the notes.
    SimonGameStateTeaching,

    /// @brief User is trying to play the notes.
    SimonGameStateListening,
};

typedef enum SimonCustomEventId SimonCustomEventId;
enum SimonCustomEventId {
    /// @brief New game was requested.
    SimonCustomEventIdNewGame = 0x4000,
    /// @brief Teach the user the notes.
    SimonCustomEventIdTeachNotes,
    /// @brief Player should repeat the notes.
    SimonCustomEventIdPlayerTurn,
    /// @brief Player pressed the wrong note!
    SimonCustomEventIdWrongNote,
    /// @brief Player played the sequence.
    SimonCustomEventIdPlayedSequence,
};

typedef struct SimonGame SimonGame;
struct SimonGame {
    /// @brief The total number of notes in the song
    uint8_t song_length;

    /// @brief The notes for the song (each note is 1,2,4 or 8).
    uint8_t notes[MAX_SONG_LENGTH];

    /// @brief The current state of the game.
    SimonGameState state;

    /// @brief The highest note number that user has successfully repeated.
    uint8_t successful_note_number;

    /// @brief The note number that the flipper is teaching.
    uint8_t note_number;
};

/**
 * @brief Plays a note and lights the button.
 * @param model Pointer to a FlipboardModel object.
 * @param note The note to play (1, 2, 4 or 8).
 * @param delay_ms The delay in milliseconds.
 */
static void simon_play_note(FlipboardModel* model, int note, int delay_ms) {
    furi_assert((note == 1) || (note == 2) || (note == 4) || (note == 8));

    ActionModel* action_model = flipboard_model_get_action_model(model, note);

    // Simulate pressing the button...
    flipboard_model_play_tone(model, action_model);
    flipboard_model_set_colors(model, action_model, action_model_get_action_id(action_model));
    furi_delay_ms(delay_ms);

    // Simulate releasing the button...
    flipboard_model_play_tone(model, NULL);
    flipboard_model_set_colors(model, NULL, 0);
    furi_delay_ms(delay_ms);
}

/**
 * @brief Teaches the current portion of the song.
 * @param flipboard Pointer to a Flipboard object.
 */
static void simon_teach_notes(Flipboard* flipboard) {
    FlipboardModel* model = flipboard_get_model(flipboard);
    SimonGame* game = flipboard_model_get_custom_data(model);
    game->state = SimonGameStateTeaching;

    uint8_t speed_index = game->successful_note_number;
    if(speed_index >= COUNT_OF(delays)) {
        speed_index = COUNT_OF(delays) - 1;
    }
    simon_play_note(model, game->notes[game->note_number], delays[speed_index]);
    game->note_number++;

    if(game->note_number <= game->successful_note_number) {
        flipboard_send_custom_event(flipboard, SimonCustomEventIdTeachNotes);
    } else {
        flipboard_send_custom_event(flipboard, SimonCustomEventIdPlayerTurn);
    }
}

/**
 * @brief Returns a random button id (1, 2, 4 or 8).
 * @return uint8_t 
 */
static uint8_t random_button_id() {
    uint8_t number = rand() & 0x3;
    return 1 << number;
}

/**
 * @brief Generates a random song.
 * @details Sets game state to new game & populates the 
 *          game song_length and notes.
 * @param model Pointer to a FlipboardModel object.
 */
void generate_song(FlipboardModel* model) {
    SimonGame* game = flipboard_model_get_custom_data(model);
    game->state = SimonGameStateNewGame;
    game->successful_note_number = 0;
    game->note_number = 0;

    // Pick some random notes for the game.
    game->song_length = MAX_SONG_LENGTH;
    for(int i = 0; i < game->song_length; i++) {
        game->notes[i] = random_button_id();
        FURI_LOG_D(TAG, "note %d: %d", i, game->notes[i]);
    }
}

/**
 * @brief Draw the simon game screen.
 * @details Draw the message "PRESS OK TO PLAY".
 * @param canvas Pointer to Canvas object for drawing.
 * @param model Pointer to the View's model (FlipboardModelRef*)
 */
static void simon_view_draw(Canvas* canvas, void* model) {
    FlipboardModelRef* my_model = (FlipboardModelRef*)model;
    SimonGame* game = flipboard_model_get_custom_data(my_model->model);

    canvas_set_font(canvas, FontPrimary);
    if(game->state == SimonGameStateGameOver) {
        if(game->song_length == 0) {
            canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "PRESS OK TO PLAY");
        } else if(game->song_length == game->note_number) {
            canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "WIN! OK TO PLAY");
        } else {
            canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "LOST. OK TO PLAY");
        }
    } else if(game->state == SimonGameStateNewGame) {
        canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "CREATING NEW GAME");
    } else if(game->state == SimonGameStateTeaching) {
        canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "TEACHING NOTES");
    } else if(game->state == SimonGameStateListening) {
        canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "YOUR TURN");
    }
}

/* @brief Handles the input events.
 * @details This function is invoked whenever the ViewDispatcher is
 *      processing an input event, like a button press on the Flipper Zero.
 * @param event Pointer to the InputEvent object.
 * @param context Pointer to Flipboard object.
 * @return bool Returns true for event handled.
 */
static bool simon_view_input(InputEvent* event, void* context) {
    Flipboard* flipboard = (Flipboard*)context;
    bool handled_event = false;

    if((event->type == InputTypeShort) && (event->key == InputKeyOk)) {
        FlipboardModel* model = flipboard_get_model(flipboard);
        SimonGame* game = flipboard_model_get_custom_data(model);

        if(game->state == SimonGameStateGameOver) {
            flipboard_send_custom_event(flipboard, SimonCustomEventIdNewGame);
            handled_event = true;
        }
    }

    return handled_event;
}

/**
 * @brief This method handles the user's guess.
 * @param flipboard The Flipboard* context.
 * @param played_note The note that the user played.
 */
static void simon_handle_guess(Flipboard* flipboard, uint8_t played_note) {
    FlipboardModel* model = flipboard_get_model(flipboard);
    SimonGame* game = flipboard_model_get_custom_data(model);
    uint8_t expected_note = game->notes[game->note_number];

    if(played_note != expected_note) {
        flipboard_send_custom_event(flipboard, SimonCustomEventIdWrongNote);
    } else {
        game->note_number++;

        if(game->note_number > game->successful_note_number) {
            flipboard_send_custom_event(flipboard, SimonCustomEventIdPlayedSequence);
        }
    }
}

/**
 * @brief This method handles FlipBoard button input.
 * @param context The Flipboard* context.
 * @param old_button The previous button state.
 * @param new_button The new button state.
 */
void flipboard_debounced_switch(void* context, uint8_t old_button, uint8_t new_button) {
    Flipboard* flipboard = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(flipboard);
    uint8_t reduced_new_button = flipboard_model_reduce(model, new_button, false);

    // Only if we are listening for user to press button do we respond.
    SimonGame* game = flipboard_model_get_custom_data(model);
    if(game->state != SimonGameStateListening) {
        FURI_LOG_D(TAG, "Ignoring button press while in game state: %d", game->state);
        return;
    }

    flipboard_model_update_gui(model);

    ActionModel* action_model = flipboard_model_get_action_model(model, reduced_new_button);
    flipboard_model_set_colors(model, action_model, new_button);
    flipboard_model_play_tone(model, action_model);

    // User stopped pressing button...
    if(new_button == 0) {
        furi_assert(old_button);
        uint8_t reduced_old_button = flipboard_model_reduce(model, old_button, false);
        action_model = flipboard_model_get_action_model(model, reduced_old_button);
        furi_assert(action_model);
        FURI_LOG_D(TAG, "Old button was is %d", action_model_get_action_id(action_model));
        simon_handle_guess(flipboard, action_model_get_action_id(action_model));
    }
}

/**
 * @brief This method is invoked when entering the "Play Simon" view.
 * @param context The Flipboard* context.
 */
static void simon_enter_callback(void* context) {
    Flipboard* flipboard = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(flipboard);
    SimonGame* game = flipboard_model_get_custom_data(model);
    game->state = SimonGameStateGameOver;
    game->song_length = 0;

    // Set color up to be a lighter version of color down.
    for(int i = 0; i < 4; i++) {
        ActionModel* action_model = flipboard_model_get_action_model(model, 1 << i);
        uint32_t color = action_model_get_color_down(action_model);
        action_model_set_color_up(action_model, adjust_color_brightness(color, 16));
    }
    flipboard_model_set_colors(model, NULL, 0x0);
    flipboard_model_set_button_monitor(model, flipboard_debounced_switch, flipboard);
}

/**
 * @brief This method is invoked when exiting the "Play Simon" view.
 * @param context The Flipboard* context.
 */
static void simon_exit_callback(void* context) {
    Flipboard* flipboard = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(flipboard);
    flipboard_model_set_button_monitor(model, NULL, NULL);
}

/**
 * @brief Returns a View* object.
 * @details Returns a View* object, configured with draw settings 
 *          and the model.
 * @param context Unused parameter.
 * @return View* The view* object.
 */
static View* get_primary_view(void* context) {
    Flipboard* flipboard = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(flipboard);
    View* view = view_alloc();
    view_set_context(view, context);
    view_set_input_callback(view, simon_view_input);
    view_set_draw_callback(view, simon_view_draw);
    view_set_previous_callback(view, flipboard_navigation_show_app_menu);
    view_set_enter_callback(view, simon_enter_callback);
    view_set_exit_callback(view, simon_exit_callback);
    view_allocate_model(view, ViewModelTypeLockFree, sizeof(FlipboardModelRef));
    FlipboardModelRef* ref = (FlipboardModelRef*)view_get_model(view);
    ref->model = model;
    return view;
}

/**
 * @brief Invoked whenever the main application menu is loaded.
 * @details This function is invoked whenever the main application 
 *          menu is loaded.  The first time (inital_load) we will
 *          show an LED startup sequence, then turn the LEDs off.
 *          If not the first time, we just turn the LEDs off.
 * @param model Pointer to FlipboardModel object.
 */
static void loaded_app_menu(FlipboardModel* model) {
    static bool initial_load = true;
    FlipboardLeds* leds = flipboard_model_get_leds(model);
    if(initial_load) {
        flipboard_leds_set(leds, LedId1, adjust_color_brightness(LedColorRed, 16));
        flipboard_leds_set(leds, LedId2, adjust_color_brightness(LedColorGreen, 16));
        flipboard_leds_set(leds, LedId3, adjust_color_brightness(LedColorBlue, 16));
        flipboard_leds_set(leds, LedId4, adjust_color_brightness(LedColorCyan, 16));
        flipboard_leds_update(leds);
        furi_delay_ms(200);

        flipboard_leds_set(leds, LedId1, adjust_color_brightness(LedColorRed, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        flipboard_leds_set(leds, LedId1, adjust_color_brightness(LedColorRed, 16));
        flipboard_leds_set(leds, LedId3, adjust_color_brightness(LedColorBlue, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        flipboard_leds_set(leds, LedId3, adjust_color_brightness(LedColorBlue, 16));
        flipboard_leds_set(leds, LedId2, adjust_color_brightness(LedColorGreen, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        flipboard_leds_set(leds, LedId2, adjust_color_brightness(LedColorGreen, 16));
        flipboard_leds_set(leds, LedId4, adjust_color_brightness(LedColorCyan, 255));
        flipboard_leds_update(leds);
        furi_delay_ms(300);

        initial_load = false;
    }

    flipboard_leds_set(leds, LedId1, LedColorBlack);
    flipboard_leds_set(leds, LedId2, LedColorBlack);
    flipboard_leds_set(leds, LedId3, LedColorBlack);
    flipboard_leds_set(leds, LedId4, LedColorBlack);
    flipboard_leds_update(leds);
}

/**
 * @brief This method handles the special ending for a lost game.
 * @param model Pointer to a FlipboardModel object.
 */
static void lost_game(FlipboardModel* model) {
    SimonGame* game = flipboard_model_get_custom_data(model);
    game->state = SimonGameStateGameOver;
    uint8_t correct_note = game->notes[game->note_number];

    ActionModel* action_model = flipboard_model_get_action_model(model, correct_note);

    for(int i = 0; i < 3; i++) {
        // Simulate pressing the button...
        flipboard_model_play_tone(model, action_model);
        flipboard_model_set_colors(model, action_model, action_model_get_action_id(action_model));
        furi_hal_vibro_on(true);
        furi_delay_ms(200);

        // Simulate releasing the button...
        flipboard_model_play_tone(model, NULL);
        flipboard_model_set_colors(model, NULL, 0);
        furi_hal_vibro_on(false);
        furi_delay_ms(100);
    }
}

/**
 * @brief This method handles the special ending for a won game.
 * @param model Pointer to a FlipboardModel object.
 */
static void won_game(FlipboardModel* model) {
    SimonGame* game = flipboard_model_get_custom_data(model);
    game->state = SimonGameStateGameOver;
    FlipboardLeds* leds = flipboard_model_get_leds(model);

    for(int i = 0; i < 3; i++) {
        ActionModel* action_model1 = flipboard_model_get_action_model(model, 1);
        flipboard_leds_set(leds, LedId1, action_model_get_color_down(action_model1));
        ActionModel* action_model2 = flipboard_model_get_action_model(model, 2);
        flipboard_leds_set(leds, LedId2, action_model_get_color_down(action_model2));
        ActionModel* action_model4 = flipboard_model_get_action_model(model, 4);
        flipboard_leds_set(leds, LedId3, action_model_get_color_down(action_model4));
        ActionModel* action_model8 = flipboard_model_get_action_model(model, 8);
        flipboard_leds_set(leds, LedId4, action_model_get_color_down(action_model8));
        flipboard_leds_update(leds);

        Speaker* speaker = flipboard_model_get_speaker(model);

        for(int freq = 0; freq < 16; freq++) {
            speaker_set_frequency(speaker, 400 + (100 * freq));
            furi_delay_ms(50);
        }
        speaker_set_frequency(speaker, 0);

        flipboard_model_set_colors(model, NULL, 0);
        furi_delay_ms(100);
    }
}

/**
 * @brief Handles the custom events.
 * @details This function is invoked whenever the ViewDispatcher is
 *      processing a custom event.
 * @param context Pointer to Flipboard object.
 * @param event The custom event.
 * @return bool Returns true for event handled.
 */
static bool custom_event_handler(void* context, uint32_t event) {
    Flipboard* flipboard = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(flipboard);

    flipboard_model_update_gui(model);

    if(event == CustomEventAppMenuEnter) {
        loaded_app_menu(model);
    } else if(event == SimonCustomEventIdNewGame) {
        generate_song(model);
        furi_delay_ms(SIMON_TEACH_DELAY_MS);
        flipboard_send_custom_event(flipboard, SimonCustomEventIdTeachNotes);
    } else if(event == SimonCustomEventIdTeachNotes) {
        simon_teach_notes(flipboard);
    } else if(event == SimonCustomEventIdPlayerTurn) {
        SimonGame* game = flipboard_model_get_custom_data(model);
        game->state = SimonGameStateListening;
        game->note_number = 0;
    } else if(event == SimonCustomEventIdWrongNote) {
        lost_game(model);
    } else if(event == SimonCustomEventIdPlayedSequence) {
        SimonGame* game = flipboard_model_get_custom_data(model);
        game->successful_note_number++;
        if(game->successful_note_number == game->song_length) {
            won_game(model);
        } else {
            game->state = SimonGameStateTeaching;
            game->note_number = 0;
            furi_delay_ms(SIMON_TEACH_DELAY_MS);
            flipboard_send_custom_event(flipboard, SimonCustomEventIdTeachNotes);
        }
    }

    flipboard_model_update_gui(model);

    return true;
}

static void flipboard_defaults(FlipboardModel* model) {
    ActionModel* action_model;
    action_model = action_model_alloc(1);
    action_model_set_color_down(action_model, 0xFF00);
    action_model_set_frequency(action_model, 164.814);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
    action_model = action_model_alloc(2);
    action_model_set_color_down(action_model, 0xFF0000);
    action_model_set_frequency(action_model, 220);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
    action_model = action_model_alloc(4);
    action_model_set_color_down(action_model, 0xFF7F00);
    action_model_set_frequency(action_model, 277.183);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
    action_model = action_model_alloc(8);
    action_model_set_color_down(action_model, 0xFF);
    action_model_set_frequency(action_model, 329.628);
    flipboard_model_set_action_model(
        model, action_model_get_action_id(action_model), action_model);
}

/**
 * @brief This is the entry point of the application.
 * @details The application.fam file sets the entry_point property to this function.
 * @param p Unused parameter.
 * @return int32_t Returns a 0 for success.
 */
int32_t flipboard_simon_app(void* p) {
    UNUSED(p);

    ActionModelFields fields = ActionModelFieldColorDown | ActionModelFieldFrequency;
    bool single_mode_button = true;

    Flipboard* app = flipboard_alloc(
        FLIPBOARD_APP_NAME,
        &I_qr_github,
        ABOUT_TEXT,
        fields,
        flipboard_defaults,
        single_mode_button,
        NULL,
        NULL,
        0,
        get_primary_view);

    view_dispatcher_set_event_callback_context(flipboard_get_view_dispatcher(app), app);
    view_dispatcher_set_custom_event_callback(
        flipboard_get_view_dispatcher(app), custom_event_handler);

    FlipboardModel* model = flipboard_get_model(app);
    SimonGame* simon_game = malloc(sizeof(SimonGame));
    simon_game->state = SimonGameStateGameOver;
    flipboard_model_set_custom_data(model, simon_game);

    view_dispatcher_run(flipboard_get_view_dispatcher(app));

    flipboard_free(app);

    return 0;
}