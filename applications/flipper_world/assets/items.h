#pragma once
#include "../flipper_world_i.h"

typedef enum {
    ItemsFood,
    ItemsConsole,
    ItemsEnumTotal,
} ItemsEnum;

typedef struct {
    uint8_t layer;
    uint16_t timeout;
    Vec2 pos;

    uint8_t width;
    uint8_t height;

    void (*draw)(Canvas* canvas, void* model);
    void (*callback)(void* context);
} Item;

uint16_t roll_new(uint16_t prev, uint16_t max);
const Vec2 item_get_pos(FlipperMainViewModel* state, ItemsEnum item);
const Item* is_nearby(FlipperMainViewModel* state);
const Item** get_scene(FlipperMainViewModel* state);
const void flipper_world_item_callback(FlipperMainView* main_view);
