#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

#define MAX_TRIES 3
#define DISHES_TOTAL 3
#define LID_POS_MAX 20
#define TRY_TIMEOUT 30

typedef struct {
    const Icon* f;
    const Icon* b;
} LootGfx;

typedef enum {
    LootSkeleton,
    LootFish,
    LootShit,
    LootTotalNum,
} LootIdEnum;

static const LootGfx loot[LootTotalNum] = {
    [LootSkeleton] =
        {
            .f = &I_skeleton_25x17,
            .b = &I_blackskeleton_25x17,
        },
    [LootFish] =
        {
            .f = &I_fish_25x17,
            .b = &I_blackfish_25x17,
        },
    [LootShit] =
        {
            .f = &I_shit_25x17,
            .b = &I_blackshit_25x17,
        },
};

typedef enum {
    FlipperFoodGameEventUpdate,
    FlipperFoodGameEventExit,
} FlipperFoodGameEvent;

typedef struct FlipperFoodGameView FlipperFoodGameView;

typedef void (*FlipperFoodGameViewCallback)(FlipperFoodGameEvent event, void* context);

struct FlipperFoodGameView {
    View* view;
    FlipperFoodGameViewCallback callback;
    void* context;

    osTimerId_t timer;
};

typedef enum {
    PlayerChoiceEvent,
    OpenLootEvent,
    WinEvent,
    LooseEvent,
    FinishedEvent,
    ExitGameEvent,
    GameEventTotal,
} GameEventType;

typedef struct {
    GameEventType current_event;
    osMessageQueueId_t event_queue;
    LootIdEnum loot_list[DISHES_TOTAL];

    uint8_t cursor_pos;
    uint8_t lid_pos;
    uint8_t timeout;
    uint8_t try;

    bool selected;
    bool deed;
} FlipperFoodGameViewModel;

void flipper_foodgame_set_callback(
    FlipperFoodGameView* foodgame,
    FlipperFoodGameViewCallback callback,
    void* context);

View* flipper_foodgame_get_view(FlipperFoodGameView* foodgame);

FlipperFoodGameView* flipper_foodgame_alloc();
void flipper_foodgame_free(FlipperFoodGameView* foodgame);

void flipper_foodgame_reset_loot(FlipperFoodGameViewModel* model);
void flipper_foodgame_update_state(FlipperFoodGameView* foodgame);
