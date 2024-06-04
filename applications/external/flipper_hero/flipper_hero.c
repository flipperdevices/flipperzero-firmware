#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>
#include "data/data.h"
#include "view/arrows.h"
#include "helpers/storage.h"
#include "flipper_hero_icons.h"

#define MIN_ARROWS 3
#define MAX_ARROWS 8
#define SCORE_MULTIPLIER 5
#define TIMER_INCREMENT_PER_ARROW 25
#define TIMER_MAX 1000

int generate_numArrows() {
    return MIN_ARROWS + rand() % (MAX_ARROWS - MIN_ARROWS + 1);
}
void generate_arrows(PluginState* plugin_state) {
    // Generate a new set of arrows, 3 to 8
    plugin_state->numArrows = generate_numArrows();

    for(int i = 0; i < plugin_state->numArrows; i++) {
        plugin_state->arrowDirections[i] = "UDLR"[rand() % 4];
        plugin_state->arrowFilled[i] = false;
    }
    plugin_state->nextArrowToFill = 0;
}

void update_score_and_timer(PluginState* plugin_state) {
    plugin_state->timer += TIMER_INCREMENT_PER_ARROW * plugin_state->numArrows;
    plugin_state->score += SCORE_MULTIPLIER * plugin_state->numArrows;
    plugin_state->round++;
}

void end_game(PluginState* plugin_state) {
    bool shouldSave = false;
    if(plugin_state->score > plugin_state->record_score) {
        plugin_state->record_score = plugin_state->score;
        shouldSave = true;
    }
    if(plugin_state->round > plugin_state->record_round) {
        plugin_state->record_round = plugin_state->round;
        shouldSave = true;
    }
    if(shouldSave) save_game_records(plugin_state);
    plugin_state->isGameOver = true;
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);
    PluginEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(void* context) {
    PluginState* plugin_state = (PluginState*)context;
    if(!plugin_state || plugin_state->isGameOver) return;

    // Decrement the timer
    plugin_state->timer--;

    // Check if the game should end
    if(plugin_state->timer <= 0) end_game(plugin_state);
}

void handle_arrow_input(PluginState* plugin_state, InputEvent input_event) {
    if(input_event.type != InputTypePress) return;
    char expectedDirection = plugin_state->arrowDirections[plugin_state->nextArrowToFill];
    bool isCorrect = false;
    switch(input_event.key) {
    case InputKeyUp:
        isCorrect = expectedDirection == 'U';
        break;
    case InputKeyDown:
        isCorrect = expectedDirection == 'D';
        break;
    case InputKeyLeft:
        isCorrect = expectedDirection == 'L';
        break;
    case InputKeyRight:
        isCorrect = expectedDirection == 'R';
        break;
    default:
        break;
    }
    if(isCorrect) {
        plugin_state->arrowFilled[plugin_state->nextArrowToFill++] = true;
    } else if(input_event.key != InputKeyOk) {
        memset(plugin_state->arrowFilled, false, sizeof(plugin_state->arrowFilled));
        plugin_state->nextArrowToFill = 0;
    }
}

void init_game_state(PluginState* plugin_state) {
    plugin_state->isGameStarted = false;
    plugin_state->isGameOver = false;
    plugin_state->score = 0;
    plugin_state->round = 1;
    plugin_state->timer = TIMER_MAX;
}

void start_game(PluginState* plugin_state, FuriTimer* timer) {
    init_game_state(plugin_state);
    plugin_state->isGameStarted = true;
    generate_arrows(plugin_state);
    furi_timer_start(timer, 10);
}

void draw_start_screen(Canvas* const canvas) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_icon(canvas, 0, 0, &I_flipper_hero_128x50);
    elements_button_center(canvas, "Start");
}

void draw_game_over_screen(Canvas* const canvas, int score, int round) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 10, AlignCenter, AlignCenter, "Game Over");
    char scoreText[32];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    canvas_draw_str_aligned(canvas, 64, 25, AlignCenter, AlignCenter, scoreText);
    char roundText[32];
    snprintf(roundText, sizeof(roundText), "Round: %d", round);
    canvas_draw_str_aligned(canvas, 64, 35, AlignCenter, AlignCenter, roundText);

    elements_button_center(canvas, "Restart");
    canvas_draw_frame(canvas, 0, 0, 128, 64);
}

void draw_game_ui(Canvas* const canvas, int score, int round) {
    char roundText[32], scoreText[32];
    canvas_set_font(canvas, FontPrimary);
    snprintf(roundText, sizeof(roundText), "Round %d", round);
    snprintf(scoreText, sizeof(scoreText), "%d score", score);
    canvas_draw_str_aligned(canvas, 4, 12, AlignLeft, AlignBottom, roundText);
    canvas_draw_str_aligned(canvas, 124, 12, AlignRight, AlignBottom, scoreText);
    canvas_draw_frame(canvas, 0, 0, 128, 64);
}

void draw_arrows(Canvas* const canvas, PluginState* plugin_state) {
    int spaceX = 15;
    int totalArrowsWidth = (plugin_state->numArrows - 1) * spaceX;
    int startX = (128 - totalArrowsWidth) / 2 + 2; // Assuming canvas width of 128 pixels
    for(int i = 0; i < plugin_state->numArrows; i++) {
        draw_empty_or_filled_arrow(
            canvas,
            startX + i * spaceX,
            32,
            plugin_state->arrowDirections[i],
            plugin_state->arrowFilled[i]);
    }
}

void draw_progress_box(Canvas* const canvas, int timer) {
    canvas_draw_frame(canvas, 0, 52, 128, 12);
    canvas_draw_box(
        canvas, 0, 52, timer * 0.128, 12); // Assuming max timer value scales to full width
}

static void render_callback(Canvas* const canvas, void* ctx) {
    PluginState* plugin_state = (PluginState*)ctx;
    furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);

    if(!plugin_state->isGameStarted) {
        draw_start_screen(canvas);
    } else if(plugin_state->isGameOver) {
        draw_game_over_screen(canvas, plugin_state->score, plugin_state->round);
    } else {
        draw_game_ui(canvas, plugin_state->score, plugin_state->round);
        draw_arrows(canvas, plugin_state);
        draw_progress_box(canvas, plugin_state->timer);
    }

    furi_mutex_release(plugin_state->mutex);
}
int32_t flipper_hero_app() {
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(PluginEvent));

    PluginState* plugin_state = malloc(sizeof(PluginState));

    FuriTimer* timer =
        furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, (void*)plugin_state);
    plugin_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    if(!plugin_state->mutex) {
        FURI_LOG_E("flipper_hero", "cannot create mutex\r\n");
        free(plugin_state);
        return 255;
    }

    init_game_state(plugin_state);
    // load_game_records(plugin_state);

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, plugin_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Open GUI and register view_port
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    PluginEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);

        furi_mutex_acquire(plugin_state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // Pass the event to the new function for processing
            handle_arrow_input(plugin_state, event.input);
        } else {
            FURI_LOG_D("flipper_hero", "FuriMessageQueue: event timeout");
        }

        // Check if all arrows are filled, then regenerate
        if(plugin_state->nextArrowToFill >= plugin_state->numArrows) {
            update_score_and_timer(plugin_state);
            generate_arrows(plugin_state); // Re-initialize arrow states
        }

        view_port_update(view_port);
        furi_mutex_release(plugin_state->mutex);

        if(event.input.key == InputKeyBack) {
            // quit game
            // stop timers
            furi_timer_stop(timer);
            processing = false; // Close the application
        } else if(event.input.key == InputKeyOk) {
            if(!plugin_state->isGameStarted) {
                start_game(plugin_state, timer);
            } else if(plugin_state->isGameOver) {
                start_game(plugin_state, timer);
            }
        }
    }
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    furi_message_queue_free(event_queue);
    furi_mutex_free(plugin_state->mutex);
    return 0;
}
