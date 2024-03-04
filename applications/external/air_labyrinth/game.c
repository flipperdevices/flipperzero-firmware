// Air Labyrinth -- by @CodeAllNight (https://youtube.com/@MrDerekJamison/about)
//
// v0.1 - Initial release (d-pad and tilt controls for player movement)
//
/*
This game was made based on the [air_arkanoid]
(https://github.com/flipperdevices/flipperzero-good-faps/tree/dev/air_arkanoid) and 
[flipperzero-game-engine-example](https://github.com/flipperdevices/flipperzero-game-engine-example) 
projects. Thanks to the authors of these projects for the inspiration and the code.
*/

#include "game.h"
#include "walls.h"

#define MOTION_X 1
#define MOTION_Y 1

/****** Entities: Player ******/

typedef struct {
    Vector trajectory; // Direction player would like to move.
    float radius; // collision radius
    int8_t dx; // x direction
    int8_t dy; // y direction
    Sprite* sprite; // player sprite
} PlayerContext;

// Forward declaration of player_desc, because it's used in player_spawn function.
static const EntityDescription player_desc;

static void player_spawn(Level* level, GameManager* manager) {
    Entity* player = level_add_entity(level, &player_desc);

    // Set player position.
    // Depends on your game logic, it can be done in start entity function, but also can be done here.
    entity_pos_set(player, (Vector){64, 28});

    entity_collider_add_rect(player, 3, 2);

    // Get player context
    PlayerContext* player_context = entity_context_get(player);

    // Load player sprite
    player_context->sprite = game_manager_sprite_load(manager, "player.fxbm");
    player_context->dx = 0;
    player_context->dy = 0;
    player_context->radius = 5;
    player_context->trajectory = (Vector){0, 0};
}

static float player_x_from_pitch(float pitch) {
    if(pitch > 9.0) {
        return 1.0f;
    } else if(pitch < -9.0) {
        return -1.0f;
    }
    return 0;
}

static float player_y_from_roll(float roll) {
    if(roll > 5.0) {
        return 0.7f;
    } else if(roll < -20.0) {
        return -0.7f;
    }
    return 0;
}

static void player_update(Entity* self, GameManager* manager, void* context) {
    // Get player context
    PlayerContext* player = context;
    player->dx = 0;
    player->dy = 0;

    GameContext* game_context = game_manager_game_context_get(manager);
    if(game_context->imu_present) {
        FURI_LOG_D(
            "Player",
            "pitch:%f   roll:%f  yaw:%f",
            (double)imu_pitch_get(game_context->imu),
            (double)imu_roll_get(game_context->imu),
            (double)imu_yaw_get(game_context->imu));

        player->trajectory = vector_add(
            player->trajectory,
            ((Vector){
                player_x_from_pitch(-imu_pitch_get(game_context->imu)),
                player_y_from_roll(-imu_roll_get(game_context->imu))}));
    }

    // Get game input
    InputState input = game_manager_input_get(manager);

    // Control player movement
    if(input.held & GameKeyUp) {
        player->trajectory = vector_add(player->trajectory, ((Vector){0, -0.8}));
    }

    if(input.held & GameKeyDown) {
        player->trajectory = vector_add(player->trajectory, ((Vector){0, +0.8}));
    }

    if(input.held & GameKeyLeft) {
        player->trajectory = vector_add(player->trajectory, ((Vector){-0.8, 0}));
    }

    if(input.held & GameKeyRight) {
        player->trajectory = vector_add(player->trajectory, ((Vector){0.8, 0}));
    }

    // Get player position
    Vector pos = entity_pos_get(self);

    if(player->trajectory.x >= 1.0) {
        player->dx = MOTION_X;
        player->trajectory.x = 0;
        pos.x += player->dx;
    } else if(player->trajectory.x <= -1.0) {
        player->dx = -MOTION_X;
        player->trajectory.x = 0;
        pos.x += player->dx;
    }

    if(player->trajectory.y >= 1.0) {
        player->dy = MOTION_Y;
        player->trajectory.y = 0;
        pos.y += player->dy;
    } else if(player->trajectory.y <= -1.0) {
        player->dy = -MOTION_Y;
        player->trajectory.y = 0;
        pos.y += player->dy;
    }

    // Clamp player position to screen bounds, considering player sprite size (10x10)
    pos.x = CLAMP(pos.x, 126, 3);
    pos.y = CLAMP(pos.y, 62, 2);

    // Set new player position
    entity_pos_set(self, pos);

    // Control game exit
    if(input.pressed & GameKeyBack) {
        game_manager_game_stop(manager);
    }
}

static void player_render(Entity* self, GameManager* manager, Canvas* canvas, void* context) {
    // Get player context
    PlayerContext* player = context;

    // Get player position
    Vector pos = entity_pos_get(self);

    // Draw player sprite
    // We subtract 5 from x and y, because collision box is 10x10, and we want to center sprite in it.
    //canvas_draw_sprite(canvas, player->sprite, pos.x - 5, pos.y - 5);

    // Collision box is 2x2 but image is 10x10.
    canvas_draw_sprite(canvas, player->sprite, pos.x - 5, pos.y - 5);

    // Get game context
    GameContext* game_context = game_manager_game_context_get(manager);

    // Draw score
    // canvas_printf(canvas, 80, 57, "Score: %lu", game_context->score);
    UNUSED(game_context);

    canvas_draw_str(canvas, 20, 6, "@codeallnight - ver 0.1");
    canvas_draw_str(canvas, 10, 61, "video game module demo");
}

static const EntityDescription player_desc = {
    .start = NULL, // called when entity is added to the level
    .stop = NULL, // called when entity is removed from the level
    .update = player_update, // called every frame
    .render = player_render, // called every frame, after update
    .collision = NULL, // called when entity collides with another entity
    .event = NULL, // called when entity receives an event
    .context_size =
        sizeof(PlayerContext), // size of entity context, will be automatically allocated and freed
};

/****** Entities: Wall ******/

static void wall_start(Entity* self, GameManager* manager, void* context);

typedef struct {
    float width;
    float height;
} WallContext;

static void wall_render(Entity* self, GameManager* manager, Canvas* canvas, void* context) {
    UNUSED(manager);

    WallContext* wall = context;

    Vector pos = entity_pos_get(self);

    canvas_draw_box(
        canvas, pos.x - wall->width / 2, pos.y - wall->height / 2, wall->width, wall->height);
}

static void wall_collision(Entity* self, Entity* other, GameManager* manager, void* context) {
    WallContext* wall = context;

    // Check if wall collided with player
    if(entity_description_get(other) == &player_desc) {
        // Increase score
        GameContext* game_context = game_manager_game_context_get(manager);
        game_context->score++;

        PlayerContext* player = (PlayerContext*)entity_context_get(other);
        if(player) {
            if(player->dx || player->dy) {
                Vector pos = entity_pos_get(other);

                // TODO: Based on where we collided, we should still slide across/down the wall.
                UNUSED(wall);

                if(player->dx) {
                    FURI_LOG_D(
                        "Player",
                        "Player collided with wall, dx: %d.  center:%f,%f",
                        player->dx,
                        (double)pos.x,
                        (double)pos.y);
                    pos.x -= player->dx;
                    player->dx = 0;
                }
                if(player->dy) {
                    FURI_LOG_D(
                        "Player",
                        "Player collided with wall, dy: %d.  center:%f,%f",
                        player->dy,
                        (double)pos.x,
                        (double)pos.y);
                    pos.y -= player->dy;
                    player->dy = 0;
                }
                entity_pos_set(other, pos);
                FURI_LOG_D("Player", "Set to center:%f,%f", (double)pos.x, (double)pos.y);
            }
        } else {
            FURI_LOG_D("Player", "Player collided with wall, but context null.");
        }
    } else {
        // HACK: Wall touching other items destroys each other (to help find collider issues)
        Level* level = game_manager_current_level_get(manager);
        level_remove_entity(level, self);
        level_remove_entity(level, other);
    }
}

static const EntityDescription wall_desc = {
    .start = wall_start, // called when entity is added to the level
    .stop = NULL, // called when entity is removed from the level
    .update = NULL, // called every frame
    .render = wall_render, // called every frame, after update
    .collision = wall_collision, // called when entity collides with another entity
    .event = NULL, // called when entity receives an event
    .context_size =
        sizeof(WallContext), // size of entity context, will be automatically allocated and freed
};

static uint8_t wall_index;

static void wall_start(Entity* self, GameManager* manager, void* context) {
    UNUSED(manager);

    WallContext* wall = context;

    // TODO: We can get the current number of items from the level (instead of wall_index).

    if(wall_index < COUNT_OF(walls)) {
        if(walls[wall_index].horizontal) {
            wall->width = walls[wall_index].length * 2;
            wall->height = 1 * 2;
        } else {
            wall->width = 1 * 2;
            wall->height = walls[wall_index].length * 2;
        }

        entity_pos_set(
            self,
            (Vector){
                walls[wall_index].x + wall->width / 2, walls[wall_index].y + wall->height / 2});

        entity_collider_add_rect(self, wall->width, wall->height);

        wall_index++;
    }
}

/****** Level ******/

static void level_alloc(Level* level, GameManager* manager, void* context) {
    UNUSED(manager);
    UNUSED(context);

    // Add player entity to the level
    player_spawn(level, manager);

    // Add wall entities to the level
    wall_index = 0;
    for(size_t i = 0; i < COUNT_OF(walls); i++) {
        level_add_entity(level, &wall_desc);
    }
}

/*
    Alloc/free is called once, when level is added/removed from the game. 
    It useful if you have small amount of levels and entities, that can be allocated at once.

    Start/stop is called when level is changed to/from this level.
    It will save memory, because you can allocate entities only for current level.
*/
static const LevelBehaviour level = {
    .alloc = level_alloc, // called once, when level allocated
    .free = NULL, // called once, when level freed
    .start = NULL, // called when level is changed to this level
    .stop = NULL, // called when level is changed from this level
    .context_size = 0, // size of level context, will be automatically allocated and freed
};

/****** Game ******/

/* 
    Write here the start code for your game, for example: creating a level and so on.
    Game context is allocated (game.context_size) and passed to this function, you can use it to store your game data.
*/
static void game_start(GameManager* game_manager, void* ctx) {
    UNUSED(game_manager);

    // Do some initialization here, for example you can load score from storage.
    // For simplicity, we will just set it to 0.
    GameContext* game_context = ctx;
    game_context->score = 0;

    game_context->imu = imu_alloc();
    game_context->imu_present = imu_present(game_context->imu);

    // Add level to the game
    game_manager_add_level(game_manager, &level);
}

/* 
    Write here the stop code for your game, for example: freeing memory, if it was allocated.
    You don't need to free level, sprites or entities, it will be done automatically.
    Also, you don't need to free game_context, it will be done automatically, after this function.
*/
static void game_stop(void* ctx) {
    GameContext* game_context = ctx;
    imu_free(game_context->imu);
    game_context->imu = NULL;

    // Do some deinitialization here, for example you can save score to storage.
    // For simplicity, we will just print it.
    FURI_LOG_I("Game", "Your score: %lu", game_context->score);
}

/*
    Yor game configuration, do not rename this variable, but you can change it's content here.  
*/
const Game game = {
    .target_fps = 30, // target fps, game will try to keep this value
    .show_fps = false, // show fps counter on the screen
    .always_backlight = true, // keep display backlight always on
    .start = game_start, // will be called once, when game starts
    .stop = game_stop, // will be called once, when game stops
    .context_size = sizeof(GameContext), // size of game context
};