#include <furi.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <input/input.h>
#include <stdlib.h>
#include <math.h>
#include <notification/notification_messages.h>

typedef enum {
    EventTypeTick,
    EventTypeKey,
} RaycastGameEventType;

typedef struct {
    RaycastGameEventType type;
    InputEvent input;
} RaycastGameEvent;

typedef struct Player {
    float x;
    float y;
    float dirX;
    float dirY;
    float planeX;
    float planeY;
    uint8_t shooting;
    uint8_t points;
} Player;

typedef struct {
    Player* player;
} RaycastGameModel;

typedef struct {
    FuriMessageQueue* queue;
    Gui* gui;
    View* view;
    ViewDispatcher* view_dispatcher;
} RaycastGame;

#define COLOR_BG ColorBlack
#define COLOR_FG ColorWhite

#define CAMERA_HEIGHT 32
#define DISP_WIDTH 128
#define DISP_HEIGHT 64
#define VIEW_RANGE 8

#define PLAYER_ROTATION_SPEED 0.1
#define PLAYER_MOVE_SPEED 0.2

const uint8_t level_map[][35] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0,
     1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
     1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
     1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0,
     1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
     1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
     1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

static void draw_line(Canvas* const canvas, int16_t x, int16_t y0, int16_t y1) {
    canvas_set_color(canvas, COLOR_BG);

    while(y0 >= y1) {
        canvas_draw_dot(canvas, x, y0);
        y0--;
    }
}

static void draw_shaded_line(Canvas* const canvas, int16_t x, int16_t y0, int16_t y1) {
    Color color = COLOR_BG;

    uint8_t i = x % 2;
    for(; y0 >= y1; y0--) {
        i = (i ? 0 : 1);
        color = (i ? COLOR_FG : COLOR_BG);
        canvas_set_color(canvas, color);
        canvas_draw_dot(canvas, x, y0);
    }
}

static void draw_floor_line(Canvas* const canvas, uint8_t x, uint8_t draw_end) {
    draw_line(canvas, x, DISP_HEIGHT, draw_end + 2);
}

static void draw_wall_line(Canvas* const canvas, uint8_t x, uint8_t h, uint8_t side) {
    uint8_t draw_start = CAMERA_HEIGHT + (h >> 1);
    uint8_t draw_end = CAMERA_HEIGHT - (h >> 1);
    if(draw_start < 0) {
        draw_start = 0;
    }
    if(draw_end > DISP_HEIGHT) {
        draw_end = DISP_HEIGHT;
    }
    if(side) {
        draw_line(canvas, x, draw_start, draw_end);
    } else {
        //add shade to y-axis wall to give better contrast
        draw_shaded_line(canvas, x, draw_start, draw_end);
    }
    draw_floor_line(canvas, x, draw_start);
}

static void do_ray_casting(Canvas* const canvas, Player* player) {
    for(uint8_t x = 0; x < DISP_WIDTH; x++) {
        //calculate ray position and direction

        //x-coordinate in camera space from -1 to 1
        float cameraX = 2 * x / (float)(DISP_WIDTH)-1;
        float rayDirX = player->dirX + player->planeX * cameraX;
        float rayDirY = player->dirY + player->planeY * cameraX;

        int mapX = (int)player->x;
        int mapY = (int)player->y;
        float deltaDistX = 0;
        float deltaDistY = 0;

        //length of ray from current position to next x or y-side
        float sideDistX = 0;
        float sideDistY = 0;

        // perpendicular distance to wall
        float perpWallDist = 0;

        //length of ray from one x or y-side to next x or y-side
        if(rayDirX != 0) {
            deltaDistX = sqrt(1 + (rayDirY * rayDirY) / (rayDirX * rayDirX));
        } else {
            deltaDistX = 99999;
        }

        if(rayDirY != 0) {
            deltaDistY = sqrt(1 + (rayDirX * rayDirX) / (rayDirY * rayDirY));
        } else {
            deltaDistY = 99999;
        }

        //what direction to step in x or y-direction (either +1 or -1)
        int stepX = 0;
        int stepY = 0;

        uint8_t hit = 0; //was there a wall hit?
        uint8_t side = 0; //was a NS or a EW wall hit?

        //calculate step and initial sideDist
        if(rayDirX < 0) {
            stepX = -1;
            sideDistX = (player->x - mapX) * deltaDistX;
        } else {
            stepX = 1;
            sideDistX = (mapX + 1.0 - player->x) * deltaDistX;
        }

        if(rayDirY < 0) {
            stepY = -1;
            sideDistY = (player->y - mapY) * deltaDistY;
        } else {
            stepY = 1;
            sideDistY = (mapY + 1 - player->y) * deltaDistY;
        }

        //DDA
        while(!hit) {
            if(sideDistX < sideDistY) {
                sideDistX += deltaDistX;
                mapX += stepX;
                side = 0;
            } else {
                sideDistY += deltaDistY;
                mapY += stepY;
                side = 1;
            }
            uint8_t mapData = level_map[mapY][mapX];

            // for(uint8_t i = 0; i < NBR_OF_ENEMIES; i++) {
            //     if(!enemy[i].destroyed && enemy[i].xPos == mapX && enemy[i].yPos == mapY) {
            //         enemy[i].visible = 1;
            //     }
            // }

            // Check if ray has hit a wall
            hit = (mapData == 1);
        }

        // Calculate distance to point of impact
        if(side == 0) {
            perpWallDist = sqrt(
                ((mapX - player->x + (1 - stepX) / 2) / rayDirX) *
                ((mapX - player->x + (1 - stepX) / 2) / rayDirX));
        } else {
            perpWallDist = sqrt(
                ((mapY - player->y + (1 - stepY) / 2) / rayDirY) *
                ((mapY - player->y + (1 - stepY) / 2) / rayDirY));
        }

        uint8_t lineHeight;
        if(perpWallDist >= 1) {
            //values under 1 makes screen glitch
            lineHeight = abs((int)(DISP_HEIGHT / perpWallDist));
        } else {
            lineHeight = DISP_HEIGHT;
        }

        if(lineHeight >= 1) {
            draw_wall_line(canvas, x, lineHeight, side);
        }
    }
}

static void game_render_callback(Canvas* const canvas, void* context) {
    furi_assert(canvas);
    furi_assert(context);

    RaycastGameModel* model = context;
    canvas_clear(canvas);

    do_ray_casting(canvas, model->player);
}

static bool game_input_callback(InputEvent* input_event, void* context) {
    furi_assert(context);
    RaycastGame* game = context;

    RaycastGameEvent event = {.type = EventTypeKey, .input = *input_event};
    furi_message_queue_put(game->queue, &event, 0);
    return true;
}

static Player* player_alloc() {
    Player* player = malloc(sizeof(Player));

    player->x = 2;
    player->y = 3;
    player->dirX = 1;
    player->dirY = 0;
    player->planeX = 0;
    player->planeY = 0.66f;
    player->shooting = 0;
    player->points = 0;

    return player;
}

static void player_free(Player* player) {
    free(player);
}

static RaycastGame* raycast_game_aloc() {
    RaycastGame* game = malloc(sizeof(RaycastGame));

    game->queue = furi_message_queue_alloc(8, sizeof(RaycastGameEvent));

    game->view_dispatcher = view_dispatcher_alloc();

    game->view = view_alloc();
    view_set_context(game->view, game);
    view_allocate_model(game->view, ViewModelTypeLockFree, sizeof(RaycastGameModel));
    view_set_draw_callback(game->view, game_render_callback);
    view_set_input_callback(game->view, game_input_callback);

    with_view_model(
        game->view, (RaycastGameModel * model) {
            model->player = player_alloc();
            return false;
        });

    game->gui = furi_record_open(RECORD_GUI);
    view_dispatcher_attach_to_gui(game->view_dispatcher, game->gui, ViewDispatcherTypeFullscreen);
    view_dispatcher_add_view(game->view_dispatcher, 0, game->view);
    view_dispatcher_switch_to_view(game->view_dispatcher, 0);

    // NotificationApp* notifications = furi_record_open("notification");
    // notification_message(notifications, &sequence_display_lock);
    // furi_record_close("notification");

    return game;
}

static void raycast_game_free(RaycastGame* game) {
    view_dispatcher_remove_view(game->view_dispatcher, 0);
    view_dispatcher_free(game->view_dispatcher);

    with_view_model(
        game->view, (RaycastGameModel * model) {
            player_free(model->player);
            return true;
        });

    view_free(game->view);

    furi_message_queue_free(game->queue);

    // NotificationApp* notifications = furi_record_open("notification");
    // notification_message(notifications, &sequence_display_unlock);
    // furi_record_close("notification");
    furi_record_close(RECORD_GUI);

    free(game);
}

void rotate_player(Player* player, bool right) {
    float rotation = right ? PLAYER_ROTATION_SPEED : -PLAYER_ROTATION_SPEED;

    float oldXDir = player->dirX;
    player->dirX = player->dirX * cosf(rotation) - player->dirY * sinf(rotation);
    player->dirY = oldXDir * sinf(rotation) + player->dirY * cosf(rotation);

    float oldplaneX = player->planeX;
    player->planeX = player->planeX * cosf(rotation) - player->planeY * sinf(rotation);
    player->planeY = oldplaneX * sinf(rotation) + player->planeY * cosf(rotation);
}

void move_player(Player* player, bool forward) {
    float nextStepX = player->dirX * PLAYER_MOVE_SPEED;
    float nextStepY = player->dirY * PLAYER_MOVE_SPEED;

    if(forward) {
        if(level_map[(int)(player->y)][(int)(player->x + nextStepX)] != 1) {
            player->x += nextStepX;
        }

        if(level_map[(int)(player->y + nextStepY)][(int)(player->x)] != 1) {
            player->y += nextStepY;
        }
    } else {
        if(level_map[(int)(player->y)][(int)(player->x - nextStepX)] != 1) {
            player->x -= nextStepX;
        }

        if(level_map[(int)(player->y - nextStepY)][(int)(player->x)] != 1) {
            player->y -= nextStepY;
        }
    }
}

int32_t raycast_game_app(void* p) {
    RaycastGame* game = raycast_game_aloc();

    RaycastGameEvent event;
    size_t button_state = 0;

    for(bool processing = true; processing;) {
        FuriStatus event_status = furi_message_queue_get(game->queue, &event, 100);

        if(event_status == FuriStatusOk) {
            // press events
            if(event.type == EventTypeKey) {
                if(event.input.type == InputTypePress) {
                    button_state |= 1 << event.input.key;
                }

                if(event.input.type == InputTypeRelease) {
                    button_state &= ~(1 << event.input.key);
                }
            }
        }

        with_view_model(
            game->view, (RaycastGameModel * model) {
                Player* player = model->player;
                if(button_state & (1 << InputKeyLeft) || button_state & (1 << InputKeyRight)) {
                    rotate_player(player, button_state & (1 << InputKeyRight));
                }

                if(button_state & (1 << InputKeyUp) || button_state & (1 << InputKeyDown)) {
                    move_player(player, button_state & (1 << InputKeyUp));
                }

                if(button_state & (1 << InputKeyBack)) {
                    processing = false;
                }

                return true;
            });
    }

    raycast_game_free(game);

    return 0;
}
