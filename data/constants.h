#include "levels.h"
#include <furi.h>

#define TAG "SpaceImpact"

#define FRAMES 25 //12

#define HERO_SIZE 8

#define HERO_MIN_Y 7
#define HERO_MAX_Y SCREEN_HEIGHT - HERO_SIZE

#define BULLET_PULL 25
#define ENEMY_PULL 10

#define ENEMY_UI_SIZE 52

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
    int x;
    int y;
} Vector2;

typedef struct {
    uint8_t id;
    uint8_t points;
    uint8_t life;
    uint8_t frame;
    Vector2 position;
} Enemy;

typedef struct {
    uint8_t life;
    uint8_t points;
    uint8_t frames[2];
} EnemyTemplate;

typedef struct {
    int8_t spawn_order;
    Vector2 bullets[BULLET_PULL];
    Enemy spawned[ENEMY_PULL];
} EnemyState;

typedef struct {
    int score;
    uint8_t life;
    uint8_t rockets;
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

    state->player.rockets = 6;
    state->enemies.spawn_order = 0;

    state->player.position.x = 1;
    state->player.position.y = (HERO_MAX_Y) / 2;
}

void game_init(GameState* const state) {
    state->player.life = 3;
    state->player.score = 0;

    state->level_index = 0;
    set_level(state);
}