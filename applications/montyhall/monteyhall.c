#include <furi.h>
#include <gui/gui.h>
#include <gui/elements.h>

#include <input/input.h>
#include <stdlib.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// CONCEPT: one of three doors will have a happy flipper while the other two have only a goat
// randomize a winning door each round, let the player choose a first selection
// reveal a goat door and allow the player to keep or switch their selection
// based on the Monty Hall problem from Let's Make a Deal

typedef struct {
    // store game values
    uint16_t pickedCar;
    uint16_t pickedGoat;
} GameState;

static void montyhall_game_init_state(GameState* game_state) {
    game_state->pickedCar = 0;
    game_state->pickedGoat = 0;
}

static void montyhall_render_callback(Canvas* const canvas, void* ctx) {
    const GameState* game_state = acquire_mutex((ValueMutex*)ctx, 25);
    if(game_state == NULL) {
        return;
    }
    canvas_clear(canvas);

    canvas_set_color(canvas, ColorBlack);

    char buffer[16];
    char buffer2[16];
    snprintf(buffer, sizeof(buffer), "%u", game_state->pickedCar);
    snprintf(buffer2, sizeof(buffer2), "%u", game_state->pickedGoat);
    canvas_draw_str_aligned(canvas, 2, 8, AlignLeft, AlignBottom, buffer);
    canvas_draw_str_aligned(canvas, SCREEN_WIDTH - 2, 8, AlignRight, AlignBottom, buffer2);

    canvas_draw_frame(canvas, 12, 2, 28, SCREEN_HEIGHT - 16); // x, y, width, height
    canvas_draw_frame(canvas, 50, 2, 28, SCREEN_HEIGHT - 16);
    canvas_draw_frame(canvas, 88, 2, 28, SCREEN_HEIGHT - 16);

    elements_button_right(canvas, ""); // move selection right
    elements_button_center(canvas, "Select");
    elements_button_left(canvas, ""); // move selection left

    release_mutex((ValueMutex*)ctx, game_state);
}

static void
    montyhall_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t montyhall_game_app(void* p) {
    UNUSED(p);
    int32_t return_code = 0;

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    GameState* game_state = malloc(sizeof(GameState));

    ValueMutex state_mutex;
    if(!init_mutex(&state_mutex, game_state, sizeof(GameState))) {
        return_code = 255;
        goto free_and_exit;
    }

    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, montyhall_render_callback, &state_mutex);
    view_port_input_callback_set(view_port, montyhall_input_callback, event_queue);

    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    /******

        Start the game

    ******/
    montyhall_game_init_state(game_state);
    game_state->pickedCar = 5;
    game_state->pickedGoat = 5;


    InputEvent event;
    for(bool loop = true; loop;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        GameState* game_state = (GameState*)acquire_mutex_block(&state_mutex);

        if(event_status == FuriStatusOk) {
            if(event.type == InputTypePress) {
                switch(event.key) {
                case InputKeyUp:
                    game_state->pickedCar++;
                    game_state->pickedGoat++;
                    break;
                case InputKeyDown:
                    game_state->pickedCar--;
                    game_state->pickedGoat--;
                    break;
                case InputKeyRight:
                    game_state->pickedGoat++;
                    break;
                case InputKeyLeft:
                    game_state->pickedCar++;
                    break;
                case InputKeyOk:
                    break;
                case InputKeyBack:
                    loop = false;
                    break;
                }
            }
        }

        view_port_update(view_port);
        release_mutex(&state_mutex, game_state);
    }

    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    delete_mutex(&state_mutex);

free_and_exit:
    free(game_state);
    furi_message_queue_free(event_queue);

    return return_code;
}