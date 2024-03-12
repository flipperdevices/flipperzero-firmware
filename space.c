#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <stdlib.h>
#include <gui/elements.h>
#include <furi_hal.h>
#include <math.h>
#include <yapinvaders_icons.h>

#define PROJECTILES_MAX 10
#define ENEMIES_MAX 10

int enemy_movement_direction = 1;
int move_down_step = 0;
int enemy_move_counter = 0;
int enemy_move_threshold = 20;

typedef struct { int x, y; } Point;

typedef struct { 
    Point position; 
} Player;

typedef struct { 
    Point position; 
    bool active; 
} Projectile;

typedef struct {
    Point position;
    bool active;
} Enemy;

typedef struct {
    Player player;
    Projectile projectiles[PROJECTILES_MAX];
    int projectiles_count;
    bool game_over;
    Enemy enemies[ENEMIES_MAX];
    int current_wave;
    bool waiting_for_restart;
    bool running;
    int score; 
} GameState;

GameState game_state;

void game_state_init(GameState* state) {
    state->player.position = (Point){64, 54};
    state->projectiles_count = 0;
    for(int i = 0; i < PROJECTILES_MAX; ++i) {
        state->projectiles[i].active = false;
    }
    state->running = true; 
    state->game_over = false; 
    state->current_wave = 0;
    state->waiting_for_restart = false;
    state->score = 0;
}

void handle_input(GameState* state, InputEvent* input_event) {
    if(input_event->type == InputTypeShort) {
        switch(input_event->key) {
            case InputKeyLeft:
                state->player.position.x = (state->player.position.x - 1) < 0 ? 0 : state->player.position.x - 1;
                break;
            case InputKeyRight:
                state->player.position.x = (state->player.position.x + 1) > 124 ? 124 : state->player.position.x + 1;
                break;
            case InputKeyOk:
                if(state->projectiles_count < PROJECTILES_MAX) {
                    for(int i = 0; i < PROJECTILES_MAX; ++i) {
                        if(!state->projectiles[i].active) {
                            state->projectiles[i].position = (Point){state->player.position.x, state->player.position.y};
                            state->projectiles[i].active = true;
                            state->projectiles_count++;
                            break;
                        }
                    }
                }
                break;
            case InputKeyBack:
                state->game_over = true;
                break;
            default:
                break;
        }
    }
}

void initialize_enemies(GameState* state) {
    int rows = 2;
    int cols = ENEMIES_MAX / rows;

    int horizontalSpacing = 128 / cols; 
    int verticalSpacing = 10; 

    for(int i = 0; i < ENEMIES_MAX; ++i) {
        state->enemies[i].position.x = (i % cols) * horizontalSpacing + (horizontalSpacing / 2);
        state->enemies[i].position.y = (i / cols) * verticalSpacing + 10;
        state->enemies[i].active = true;
    }

    state->current_wave++;
}

void update_enemy_positions(GameState* state) {
    enemy_move_counter++;
    if(enemy_move_counter >= enemy_move_threshold) {
        bool changeDirection = false;
        int newDirection = enemy_movement_direction;

        for(int i = 0; i < ENEMIES_MAX; ++i) {
            if(state->enemies[i].active) {
                if ((state->enemies[i].position.x <= 0 && enemy_movement_direction < 0) ||
                    (state->enemies[i].position.x >= 124 && enemy_movement_direction > 0)) {
                    changeDirection = true;
                    newDirection *= -1; 
                    break;
                }
            }
        }

        for(int i = 0; i < ENEMIES_MAX; ++i) {
            if(state->enemies[i].active) {
                if (!changeDirection) {
                    state->enemies[i].position.x += enemy_movement_direction;
                }
            }
        }

        if (changeDirection) {
            for(int i = 0; i < ENEMIES_MAX; ++i) {
                if(state->enemies[i].active) {
                    state->enemies[i].position.x += newDirection;
                    state->enemies[i].position.y += 5;
                }
            }
            enemy_movement_direction = newDirection;
        }

        enemy_move_counter = 0;
    }
}

void handle_collisions(GameState* state) {
    for(int p = 0; p < PROJECTILES_MAX; ++p) {
        if(state->projectiles[p].active) {
            for(int e = 0; e < ENEMIES_MAX; ++e) {
                if(state->enemies[e].active) {
                    if(abs(state->projectiles[p].position.x - state->enemies[e].position.x) < 5 &&
                       abs(state->projectiles[p].position.y - state->enemies[e].position.y) < 5) {
                        state->projectiles[p].active = false;
                        state->enemies[e].active = false;
                        state->projectiles_count--;
                        state->score += 10;
                        break;
                    }
                }
            }
        }
    }
}

bool all_enemies_destroyed(GameState* state) {
    for(int i = 0; i < ENEMIES_MAX; ++i) {
        if(state->enemies[i].active) {
            return false;
        }
    }
    return true;
}

void render_game_over_screen(Canvas* canvas, GameState* state) {
    char score_msg[30];
    canvas_clear(canvas);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 8, AlignCenter, AlignCenter, "Game Over!");

    snprintf(score_msg, sizeof(score_msg), "Score: %d", state->score);
    canvas_draw_str_aligned(canvas, 64, 24, AlignCenter, AlignCenter, score_msg);

    canvas_draw_str_aligned(canvas, 64, 40, AlignCenter, AlignCenter, "Press OK to restart");
}

void update_game_state(GameState* state) {
    for(int i = 0; i < PROJECTILES_MAX; ++i) {
        if(state->projectiles[i].active) {
            state->projectiles[i].position.y -= 2;
            if(state->projectiles[i].position.y < 0) {
                state->projectiles[i].active = false;
                state->projectiles_count--;
            }
        }
    }

    for(int i = 0; i < ENEMIES_MAX; ++i) {
        if(state->enemies[i].active && state->enemies[i].position.y >= state->player.position.y) {
            state->game_over = true;
            break;
        }
    }
    
    if(all_enemies_destroyed(state)) {
        initialize_enemies(state);
    }

    update_enemy_positions(state); 
    handle_collisions(state); 
}

void render_callback(Canvas* const canvas, void* ctx) {
    GameState* state = (GameState*)ctx;
    if (!canvas || !state) return; 
    if(state->game_over) {
        render_game_over_screen(canvas, state); 
    } else {
    canvas_clear(canvas);
    char score_text[30];
    snprintf(score_text, sizeof(score_text), "Score: %d", state->score);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 64, 0, AlignCenter, AlignTop, score_text);
    canvas_draw_icon(canvas, state->player.position.x, state->player.position.y, &I_yapinvader);
    for(int i = 0; i < PROJECTILES_MAX; ++i) {
        if(state->projectiles[i].active) {
            canvas_draw_circle(canvas, state->projectiles[i].position.x, state->projectiles[i].position.y, 1);
            }
        }
    }
    for(int i = 0; i < ENEMIES_MAX; ++i) {
        if(state->enemies[i].active) {
            canvas_draw_icon(canvas, state->enemies[i].position.x, state->enemies[i].position.y, &I_yap);
        }
    }
}

static void input_callback(InputEvent* input_event, void* ctx) {
    GameState* state = (GameState*)ctx;
    if (!state) return;

    if (input_event->key == InputKeyBack && input_event->type == InputTypeShort) {
        state->running = false;
        return;
    }
    
    if (state->game_over) {
        if (input_event->key == InputKeyOk && input_event->type == InputTypeShort) {
            game_state_init(state);
            initialize_enemies(state);
            state->game_over = false; 
        }
    } else {
        handle_input(state, input_event);
    }
}

int32_t yapinvaders_app(void) {
    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, render_callback, &game_state);
    view_port_input_callback_set(view_port, input_callback, &game_state);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    game_state_init(&game_state);
    initialize_enemies(&game_state);

    while (game_state.running) {
        if (game_state.game_over) {
            if (game_state.waiting_for_restart) {
                game_state_init(&game_state);
                initialize_enemies(&game_state);
                game_state.waiting_for_restart = false;
            }
        } else {
            update_game_state(&game_state);
            view_port_update(view_port);
        }

        furi_delay_ms(30);
    }

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);

    return 0;
}