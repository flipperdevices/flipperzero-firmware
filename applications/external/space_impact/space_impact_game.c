#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include "gui/font.h"
#include "gui/assets.h"
#include "data/enemies.h"

static void game_update(GameState* game_state) {
    game_state->level_time++;

    // destroy anims
    if(game_state->level_time % 2 == 0) {
        for(int i = 0; i < ENEMY_PULL; i++) {
            if(game_state->destroy[i].frame > DESTROY_FRAMES) {
                game_state->destroy[i].frame = 0;
            } else if(game_state->destroy[i].frame > 0) {
                game_state->destroy[i].frame++;
            }
        }
    }

    // update position
    if(game_state->up && game_state->player.position.y > HERO_MIN_Y) {
        game_state->player.position.y--;
    } else if(game_state->down && game_state->player.position.y < HERO_MAX_Y) {
        game_state->player.position.y++;
    }

    // half enemy speed
    if(game_state->level_time % 3 == 0) {
        enemy_update(game_state);
    }

    weapon_update(game_state);
    weapon_check_colisions(game_state);
    enemy_try_spawn(game_state);

    if(game_state->fire) {
        weapon_try_fire_bullet(game_state);
    }
}

static void draw_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);

    const GameState* game_state = ctx;
    furi_mutex_acquire(game_state->mutex, FuriWaitForever);

    if(game_state == NULL) {
        return;
    }

    draw_level_bg(canvas, game_state->level.index, game_state->level.isInvertedColor);

    render_clear(game_state->level.isInvertedColor);

    // life
    for(int i = 0; i < game_state->player.life; i++) {
        draw_ui_asset(i * 6 + 1, 1, ui_heart);
    }
    // rockets
    draw_ui_asset(50, 1, ui_rocket);
    draw_number(58, 1, game_state->player.rockets, 2);
    // score
    //draw_number(90, 1, game_state->player.score, 5);
    draw_number(90, 1, game_state->level_time / FRAMES, 5);
    draw_number(90, 10, game_state->enemies.spawn_order, 2);
    // player
    draw_ui_asset(game_state->player.position.x, game_state->player.position.y, ui_hero);
    // bullets
    for(int i = 0; i < BULLET_PULL; i++) {
        if(game_state->player.bullets[i].x >= BULLET_X) {
            draw_ui_asset(
                game_state->player.bullets[i].x, game_state->player.bullets[i].y, ui_bullet);
        }
    }

    // enemies
    for(int i = 0; i < ENEMY_PULL; i++) {
        if(game_state->enemies.spawned[i].life > 0 &&
           game_state->enemies.spawned[i].position.x > 0) {
            draw_ui_asset(
                game_state->enemies.spawned[i].position.x,
                game_state->enemies.spawned[i].position.y,
                enemy_frames[enemies[game_state->enemies.spawned[i].id]
                                 .frames[game_state->enemies.spawned[i].frame]]);
        }
    }

    // destroy anims
    for(int i = 0; i < ENEMY_PULL; i++) {
        if(game_state->destroy[i].frame > 0) {
            draw_destroy(
                canvas,
                game_state->destroy[i].frame,
                game_state->destroy[i].pos.x,
                game_state->destroy[i].pos.y);
        }
    }

    render_draw(canvas);

    furi_mutex_release(game_state->mutex);
}

static void input_callback(InputEvent* input_event, void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);

    GameEvent event = {.type = EventTypeInput, .input = *input_event};
    furi_message_queue_put(event_queue, &event, FuriWaitForever);
}

static void timer_callback(void* ctx) {
    FuriMessageQueue* event_queue = ctx;
    furi_assert(event_queue);

    GameEvent event = {.type = EventTypeTick};
    furi_message_queue_put(event_queue, &event, 0);
}

int32_t impact_game_main() {
    // Create event queue for 8 elements of GameEvent type size
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(GameEvent));

    // Create game state
    GameState* game_state = malloc(sizeof(GameState));
    // Init game with start values
    game_init(game_state);

    game_state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!game_state->mutex) {
        FURI_LOG_E(TAG, "cannot create mutex\r\n");
        free(game_state);
        return 255;
    }

    // Set system callbacks
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, draw_callback, game_state);
    view_port_input_callback_set(view_port, input_callback, event_queue);

    // Set timer
    FuriTimer* timer = furi_timer_alloc(timer_callback, FuriTimerTypePeriodic, event_queue);
    furi_timer_start(timer, furi_kernel_get_tick_frequency() / FRAMES);

    // Open GUI and register view_port
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    FURI_LOG_E(TAG, ">>> started\r\n");
    GameEvent event;
    // Game loop
    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(event_queue, &event, 100);
        furi_mutex_acquire(game_state->mutex, FuriWaitForever);

        if(event_status == FuriStatusOk) {
            // Game update event
            if(event.type == EventTypeTick) {
                game_update(game_state);
            }

            // Key press events
            if(event.type == EventTypeInput) {
                if(event.input.type == InputTypePress) {
                    if(event.input.key == InputKeyOk) {
                        game_state->fire = true;
                    } else if(event.input.key == InputKeyUp) {
                        game_state->up = true;
                    } else if(event.input.key == InputKeyDown) {
                        game_state->down = true;
                    } else if(event.input.key == InputKeyRight) {
                        game_state->fire = true;
                    } else if(event.input.key == InputKeyLeft) {
                        game_state->rocket = true;
                    }
                }
                if(event.input.type == InputTypeRelease) {
                    if(event.input.key == InputKeyOk) {
                        game_state->fire = false;
                    } else if(event.input.key == InputKeyUp) {
                        game_state->up = false;
                    } else if(event.input.key == InputKeyDown) {
                        game_state->down = false;
                    } else if(event.input.key == InputKeyRight) {
                        game_state->fire = false;
                    } else if(event.input.key == InputKeyLeft) {
                        game_state->rocket = false;
                    }
                }
                if(event.input.key == InputKeyBack) {
                    processing = false;
                }
            }
        } else {
            FURI_LOG_D(TAG, "osMessageQueue: event timeout");
        }

        view_port_update(view_port);
        furi_mutex_release(game_state->mutex);
    }

    // Clear resources
    furi_timer_free(timer);
    furi_message_queue_free(event_queue);
    view_port_enabled_set(view_port, false);
    gui_remove_view_port(gui, view_port);
    furi_record_close(RECORD_GUI);
    view_port_free(view_port);
    furi_mutex_free(game_state->mutex);
    free(game_state);

    return 0;
}