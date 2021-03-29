#pragma once
#include "dolphin_i.h"
#include <math.h>
#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define ITEMS_NUM 3
#define LAYERS 3
#define DOLPHIN_LAYER 1

typedef struct {
    uint8_t layer;
    uint16_t x;
    uint16_t y;
    IconName icon;
    char action_name[16];
    void (*draw)(Canvas* canvas, void* model);
    void (*callback)(Canvas* canvas, void* model);
} Item;

void draw_tv(Canvas* canvas, void* model);
void draw_sofa(Canvas* canvas, void* model);

typedef struct {
    uint16_t location;
    uint16_t last;
    enum Actions type;
    void (*callback)(Canvas* canvas, DolphinViewMainModel* model);
} Action;

void dolphin_draw_emote_bubble(Canvas* canvas, void* model);

void draw_dolphin(Canvas* canvas, void* m);

void set_dolphin_graphics(void* model);

const Item* is_nearby(void* model);

void dolphin_update_position(void* model);

void dolphin_actions_update(Canvas* canvas, void* model);

void dolphin_update_scene(Canvas* canvas, void* model);
