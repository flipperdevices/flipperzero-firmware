#pragma once
#include "dolphin_i.h"

#define ARRSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#define ITEMS_NUM 4
#define LAYERS 8
#define DOLPHIN_LAYER 6
#define PARALLAX_MOD 7
#define PARALLAX(layer) layer / PARALLAX_MOD - layer

typedef struct {
    uint8_t layer;
    uint16_t x;
    uint16_t y;
    IconName icon;
    char action_name[16];
    void (*draw)(Canvas* canvas, void* model);
    void (*callback)(Canvas* canvas, void* model);
} Item;

// item callbacks and draw helpers
void smash_tv(Canvas* canvas, void* model);

void draw_tv(Canvas* canvas, void* model);

void sofa_sit(Canvas* canvas, void* model);

void inspect_painting(Canvas* canvas, void* model);

void pc_callback(Canvas* canvas, void* model);

// ext
void dolphin_draw_emote_bubble(Canvas* canvas, void* model, char* custom);

void dolphin_handle_keys(InputEvent* event, DolphinViewMainModel* model);

void dolphin_update_scene(Canvas* canvas, void* model);
