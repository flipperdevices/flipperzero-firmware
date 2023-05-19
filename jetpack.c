#include <stdlib.h>

#include <jetpack_joyride_icons.h>
#include <furi.h>
#include <gui/gui.h>
#include <gui/icon_animation.h>
#include <input/input.h>

#define TAG "Jetpack Joyride"

#define GRAVITY_BOOST -0.3
#define GRAVITY_TICK 0.15
#define PARTIVLE_VELOCITY 2
#define SCIENTIST_VELOCITY_MIN 2
#define SCIENTIST_VELOCITY_MAX 2

#define SCIENTISTS_MAX 6
#define COINS_MAX 25
#define PARTICLES_MAX 50

typedef struct {
    int x;
    int y;
} POINT;

typedef struct {
    float gravity;
    POINT point;
    bool isBoosting;
} BARRY;

typedef struct {
    float gravity;
    POINT point;
} COIN;

typedef struct {
    POINT point;
} PARTICLE;

typedef enum {
    ScientistStateAlive,
    ScientistStateDead,
} ScientistState;
typedef struct {
    float gravity;
    POINT point;
    int velocity_x;
    ScientistState state;
} SCIENTIST;

typedef enum {
    GameStateLife,
    GameStateGameOver,
} State;

typedef struct {
    IconAnimation* barry;
    IconAnimation* scientist;
    Icon* coin;
    Icon* dead_scientist;
} GameSprites;

typedef struct {
    int points;
    int distance;
    BARRY barry;
    SCIENTIST scientists[SCIENTISTS_MAX];
    COIN coins[COINS_MAX];
    PARTICLE particles[PARTICLES_MAX];
    State state;
    GameSprites sprites;
    FuriMutex* mutex;
} GameState;

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} GameEvent;

static void jetpack_game_random_coins(GameState* const game_state) {
    // Check for an available slot for a new coin
    for(int i = 0; i < COINS_MAX; ++i) {
        if(game_state->coins[i].point.x <= 0 && (rand() % 1000) < 1) {
            game_state->coins[i].point.x = 127;
            game_state->coins[i].point.y = rand() % 64;
            break;
        }
    }
}

static void jetpack_game_random_scientists(GameState* const game_state) {
    // Check for an available slot for a new scientist
    for(int i = 0; i < SCIENTISTS_MAX; ++i) {
        if(game_state->scientists[i].point.x <= 0 &&
           (rand() % 1000) < 10) { // Spawn rate is less frequent than coins
            game_state->scientists[i].state = ScientistStateAlive;
            game_state->scientists[i].point.x = 127;
            game_state->scientists[i].point.y = 49;
            game_state->scientists[i].velocity_x =
                (rand() % (SCIENTIST_VELOCITY_MAX - SCIENTIST_VELOCITY_MIN + 1)) +
                SCIENTIST_VELOCITY_MIN; // random velocity between SCIENTIST_VELOCITY_MIN and SCIENTIST_VELOCITY_MAX
            break;
        }
    }
}

static void jetpack_game_spawn_particles(GameState* const game_state) {
    for(int i = 0; i < PARTICLES_MAX; i++) {
        if(game_state->particles[i].point.y <= 0) {
            game_state->particles[i].point.x = game_state->barry.point.x + (rand() % 7) - 3;

            game_state->particles[i].point.y = game_state->barry.point.y;
            break;
        }
    }
}

static void jetpack_game_state_init(GameState* const game_state) {
    UNUSED(game_state);
    BARRY barry;
    barry.gravity = 0;
    barry.point.x = 64;
    barry.point.y = 32;
    barry.isBoosting = false;

    GameSprites sprites;
    sprites.barry = icon_animation_alloc(&A_barry);
    sprites.scientist = icon_animation_alloc(&A_scientist);

    icon_animation_start(sprites.scientist);
    icon_animation_start(sprites.barry);

    game_state->barry = barry;
    game_state->points = 0;
    game_state->distance = 0;
    game_state->sprites = sprites;
    game_state->state = GameStateLife;

    memset(game_state->scientists, 0, sizeof(game_state->scientists));
    memset(game_state->coins, 0, sizeof(game_state->coins));
    memset(game_state->particles, 0, sizeof(game_state->particles));
}

static void jetpack_game_state_free(GameState* const game_state) {
    icon_animation_free(game_state->sprites.barry);
    icon_animation_free(game_state->sprites.scientist);
    free(game_state);
}

static void jetpack_game_tick(GameState* const game_state) {
    // Do jetpack things
    game_state->barry.gravity += GRAVITY_TICK;
    game_state->barry.point.y += game_state->barry.gravity;

    // Increment distance
    game_state->distance++;

    // Move coins towards the player
    for(int i = 0; i < COINS_MAX; i++) {
        if(game_state->coins[i].point.x > 0) {
            if(!(game_state->barry.point.x >
                     game_state->coins[i].point.x + 7 || // Barry is to the right of the coin
                 game_state->barry.point.x + 11 <
                     game_state->coins[i].point.x || // Barry is to the left of the coin
                 game_state->barry.point.y >
                     game_state->coins[i].point.y + 7 || // Barry is below the coin
                 game_state->barry.point.y + 15 <
                     game_state->coins[i].point.y)) { // Barry is above the coin
                game_state->coins[i].point.x = 0; // Remove the coin
                game_state->points++; // Increase the score
            }

            game_state->coins[i].point.x -= 1; // move left by 1 unit
            if(game_state->coins[i].point.x < -16) { // if the coin is out of screen
                game_state->coins[i].point.x =
                    0; // set coin x coordinate to 0 to mark it as "inactive"
            }
        }
    }

    // Move particles
    for(int i = 0; i < PARTICLES_MAX; i++) {
        if(game_state->particles[i].point.y > 0) {
            game_state->particles[i].point.y += PARTIVLE_VELOCITY;

            // Check collision with scientists
            for(int j = 0; j < SCIENTISTS_MAX; j++) {
                if(game_state->scientists[j].state == ScientistStateAlive &&
                   game_state->scientists[j].point.x > 0) {
                    // Added half the width and height of the scientist sprite to the scientist's x and y respectively
                    float scientist_center_x = game_state->scientists[j].point.x + 5.5;
                    float scientist_center_y = game_state->scientists[j].point.y + 7.5;
                    if(!(game_state->particles[i].point.x >
                             scientist_center_x +
                                 5.5 || // particle is to the right of the scientist
                         game_state->particles[i].point.x + 11 <
                             scientist_center_x -
                                 5.5 || // particle is to the left of the scientist
                         game_state->particles[i].point.y >
                             scientist_center_y + 7.5 || // particle is below the scientist
                         game_state->particles[i].point.y + 15 <
                             scientist_center_y - 7.5)) { // particle is above the scientist
                        game_state->scientists[j].state = ScientistStateDead;
                        game_state->points += 2; // Increase the score by 2
                    }
                }
            }

            if(game_state->particles[i].point.x < 0 || game_state->particles[i].point.x > 128 ||
               game_state->particles[i].point.y < 0 || game_state->particles[i].point.y > 64) {
                game_state->particles[i].point.y = 0;
            }
        }
    }

    // Move scientists
    for(int i = 0; i < SCIENTISTS_MAX; i++) {
        if(game_state->scientists[i].point.x > 0) {
            game_state->scientists[i].point.x -=
                game_state->scientists[i].velocity_x; // move based on velocity_x
            if(game_state->scientists[i].point.x < -16) { // if the scientist is out of screen
                game_state->scientists[i].point.x =
                    0; // set scientist x coordinate to 0 to mark it as "inactive"
            }
        }
    }

    // Spawn scientists and coins...
    jetpack_game_random_coins(game_state);
    // Sprite height of Barry
    int sprite_height = 15;

    // Constrain barry's height within sprite_height and 64 - sprite_height
    if(game_state->barry.point.y > (64 - sprite_height)) {
        game_state->barry.point.y = 64 - sprite_height;
        game_state->barry.gravity = 0; // stop upward momentum
    } else if(game_state->barry.point.y < 0) {
        game_state->barry.point.y = 0;
        game_state->barry.gravity = 0; // stop downward momentum
    }

    // spawn scientists and coins...
    jetpack_game_random_coins(game_state);
    jetpack_game_random_scientists(game_state);

    if(game_state->barry.isBoosting) {
        game_state->barry.gravity += GRAVITY_BOOST;
        jetpack_game_spawn_particles(game_state);
    }
}

static void jetpack_game_render_callback(Canvas* const canvas, void* ctx) {
    furi_assert(ctx);
    const GameState* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    if(game_state->state == GameStateLife) {
        // Draw scene

        // Draw coins
        for(int i = 0; i < COINS_MAX; ++i) {
            if(game_state->coins[i].point.x > 0) {
                canvas_draw_icon(
                    canvas, game_state->coins[i].point.x, game_state->coins[i].point.y, &I_coin);
            }
        }

        // Draw scientists
        for(int i = 0; i < SCIENTISTS_MAX; ++i) {
            if(game_state->scientists[i].point.x > 0) {
                if(game_state->scientists[i].state == ScientistStateAlive) {
                    canvas_draw_icon_animation(
                        canvas,
                        game_state->scientists[i].point.x,
                        game_state->scientists[i].point.y,
                        game_state->sprites.scientist);
                } else {
                    canvas_draw_icon(
                        canvas,
                        game_state->scientists[i].point.x,
                        game_state->scientists[i].point.y,
                        &I_dead_scientist);
                }
            }
        }

        // Draw particles
        for(int i = 0; i < PARTICLES_MAX; i++) {
            if(game_state->particles[i].point.y > 0) {
                canvas_draw_line(
                    canvas,
                    game_state->particles[i].point.x,
                    game_state->particles[i].point.y,
                    game_state->particles[i].point.x,
                    game_state->particles[i].point.y + 3);
            }
        }

        // Draw barry
        canvas_draw_icon_animation(
            canvas,
            game_state->barry.point.x,
            game_state->barry.point.y,
            game_state->sprites.barry);

        canvas_set_font(canvas, FontSecondary);
        char buffer[12];
        snprintf(buffer, sizeof(buffer), "Dist: %u", game_state->distance);
        canvas_draw_str_aligned(canvas, 123, 12, AlignRight, AlignBottom, buffer);

        snprintf(buffer, sizeof(buffer), "Score: %u", game_state->points);
        canvas_draw_str_aligned(canvas, 5, 12, AlignLeft, AlignBottom, buffer);
    }

    if(game_state->state == GameStateGameOver) {
        // Show highscore
    }

    canvas_draw_frame(canvas, 0, 0, 128, 64);

    furi_mutex_release(game_state->mutex);
}

static void jetpack_game_input_callback(InputEvent* input_event, FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    GameEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void jetpack_game_update_timer_callback(FuriMessageQueue* event_queue) {
    furi_assert(event_queue);

    GameEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t jetpack_game_app(void* p) {
    UNUSED(p);
    int32_t return_code = 0;

    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(GameEvent));

    GameState* game_state = malloc(sizeof(GameState));
    jetpack_game_state_init(game_state);

    game_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!game_state->mutex) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        return_code = 255;
        goto free_and_exit;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, jetpack_game_render_callback, game_state);
    view_port_input_callback_set(view_port, jetpack_game_input_callback, event_queue);

    FuriTimer* timer =
        furi_timer_alloc(jetpack_game_update_timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / 25);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    GameEvent event;
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(game_state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypeRelease && event.input.key == InputKeyOk) {
                    game_state->barry.isBoosting = false;
                }

                if(event.input.type == InputTypePress) {
                    switch(event.input.key) {
                    case InputKeyUp:
                        break;
                    case InputKeyDown:
                        break;
                    case InputKeyRight:
                        break;
                    case InputKeyLeft:
                        break;
                    case InputKeyOk:
                        if(game_state->state == GameStateGameOver) {
                            jetpack_game_state_init(game_state);
                        }

                        if(game_state->state == GameStateLife) {
                            // Do something
                            game_state->barry.isBoosting = true;
                        }

                        break;
                    case InputKeyBack:
                        processing = false;
                        break;
                    default:
                        break;
                    }
                }
            } else if(event.type == EventTypeTick) {
                jetpack_game_tick(game_state);
            }
        }

        view_port_update(view_port);
        furi_mutex_release(game_state->mutex);
    }

    furi_timer_free(timer);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_mutex_free(game_state->mutex);

free_and_exit:
    jetpack_game_state_free(game_state);
    furi_message_queue_free(event_queue);

    return return_code;
}