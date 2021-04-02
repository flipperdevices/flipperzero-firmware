#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <u8g2/u8g2.h>

typedef enum {
    EventTypeTick,
    EventTypeKey,
} EventType;

typedef struct {
    union {
        InputEvent input;
    } value;
    EventType type;
    osMessageQueueId_t mqueue;
} AppEvent;

typedef struct {
    int32_t x;
    int32_t y;
} Vec2;

typedef struct {
    Gui* gui;
    ViewPort* view_port;
    ValueMutex* vm;
    AppEvent* event;
    bool enabled;

    Vec2 player;
    Vec2 player_global;
    Vec2 player_v;

    Vec2 screen;

    bool in_boundaries;
    uint8_t player_anim;

    size_t label_id;
    uint8_t glitch_level;
    uint32_t glitch_t;

    uint32_t action_timeout;
    uint8_t action;
    uint8_t next_action;
    uint8_t previous_action;
    uint8_t poi;

} SceneState;

typedef struct {
    uint8_t layer;
    uint16_t x;
    uint16_t y;
    IconName icon;
    char action_name[16];
    void (*draw)(Canvas* canvas, void* model);
    void (*callback)(Canvas* canvas, void* model);
} Item;

// global
#define SCALE 1024

// screen
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define BONDARIES_X_LEFT 40
#define BONDARIES_X_RIGHT 88

// player
#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 32
#define SPEED_X 40

// world
#define WORLD_WIDTH 2048
#define WORLD_HEIGHT 64

#define LABEL_X 30 * SCALE
#define LABEL_Y 12 * SCALE
#define LABEL_HEIGHT 8 * SCALE

#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define ITEMS_NUM 4
#define LAYERS 8
#define DOLPHIN_LAYER 6
#define PARALLAX_MOD 7
#define PARALLAX(layer) layer / PARALLAX_MOD - layer

void scene_show(SceneState* state);
void render_scene(SceneState* state, Canvas* canvas, uint32_t t);

void render_dolphin(SceneState* state, Canvas* canvas);
void handle_user_input(SceneState* state, InputEvent* input);
void update_dolphin_coordinates(SceneState* state, uint32_t dt);

void render_dolphin_state(SceneState* state, Canvas* canvas);
void update_dolphin_state(SceneState* state, uint32_t t, uint32_t dt);

void scene_redraw(Canvas* canvas, void* ctx);
void tick_handler(SceneState* state, uint32_t t, uint32_t dt);
void handle_input(SceneState* state, InputEvent* input);
