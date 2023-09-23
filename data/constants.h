#include "levels.h"
#include <furi.h>

#define TAG "SpaceImpact"

#define HERO_SIZE 8

#define HERO_MIN_Y 7
#define HERO_MAX_Y SCREEN_HEIGHT - HERO_SIZE

#define BULLET_PULL 25
#define ENEMY_PULL 10

#define membersof(x) (sizeof(x) / sizeof(x[0]))

typedef enum {
    EventTypeTick,
    EventTypeInput,
} EventType;

typedef struct {
    EventType type;
    InputEvent input;
} GameEvent;

typedef struct {
    uint8_t x;
    uint8_t y;
} Vector2;

typedef struct {
    uint8_t id;
    int points;
    int life;
    Vector2 position;
} Enemy;

typedef struct {
    int8_t current_spawned;
    Vector2 bullets[BULLET_PULL];
    Enemy spawned[ENEMY_PULL];
} EnemyState;

typedef struct {
    int score;
    int life;
    int rockets;
    Vector2 bullets[BULLET_PULL];
    Vector2 position;
} PlayerState;

typedef struct {
    FuriMutex* mutex;
    PlayerState player;
    EnemyState enemies;
    uint8_t level_index;
    uint16_t level_time;
    Level level;
    bool fire;
    bool rocket;
    bool up;
    bool down;
} GameState;

void set_level(GameState* const state) {
    state->level = levels[state->level_index];
    state->level_time = 0;
}

void game_init(GameState* const state) {
    state->player.life = 3;
    state->player.rockets = 3;
    state->player.score = 0;

    state->player.position.x = 1;
    state->player.position.y = (HERO_MAX_Y) / 2;

    state->enemies.current_spawned = -1;

    state->level_time = 0;
    state->level_index = 0;
    set_level(state);
}