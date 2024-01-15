#include <furi.h>
#include <gui/view.h>
#include "app_config.h"
#include "./common/config_colors.h"
#include "./common/custom_event.h"
#include "./common/flipboard.h"
#include "./common/flipboard_model.h"
#include "./common/flipboard_model_ref.h"
#include "./common/leds.h"

#define MAX_SONG_LENGTH 5

typedef enum SimonGameState SimonGameState;
enum SimonGameState {
    /// @brief Ready for user to start a new game.
    SimonGameStateGameOver,

    /// @brief Populating a new game
    SimonGameStateNewGame,
};

typedef enum SimonCustomEventId SimonCustomEventId;
enum SimonCustomEventId {
    /// @brief New game was requested.
    SimonCustomEventIdNewGame = 0x4000,
};

typedef struct SimonGame SimonGame;
struct SimonGame {
    /// @brief The total number of notes in the song
    uint8_t song_length;

    /// @brief The notes for the song (each note is 1,2,4 or 8).
    uint8_t notes[MAX_SONG_LENGTH];

    /// @brief The current state of the game.
    SimonGameState state;
};

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
        canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "PRESS OK TO PLAY");
    } else if(game->state == SimonGameStateNewGame) {
        canvas_draw_str_aligned(canvas, 64, 12, AlignCenter, AlignCenter, "CREATING NEW GAME");
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
 * @brief This method is invoked when entering the "Play Simon" view.
 * @param context The Flipboard* context.
 */
static void simon_enter_callback(void* context) {
    Flipboard* flipboard = (Flipboard*)context;
    FlipboardModel* model = flipboard_get_model(flipboard);
    SimonGame* game = flipboard_model_get_custom_data(model);
    game->state = SimonGameStateGameOver;

    // Set color up to be a lighter version of color down.
    for(int i = 0; i < 4; i++) {
        ActionModel* action_model = flipboard_model_get_action_model(model, 1 << i);
        uint32_t color = action_model_get_color_down(action_model);
        action_model_set_color_up(action_model, adjust_color_brightness(color, 16));
    }
    flipboard_model_set_colors(model, NULL, 0x0);
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
    }

    flipboard_model_update_gui(model);

    return true;
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
        FLIPBOARD_PRIMARY_ITEM_NAME,
        ABOUT_TEXT,
        fields,
        single_mode_button,
        false,
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