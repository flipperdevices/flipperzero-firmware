#pragma once

#include <gui/gui_i.h>
#include <gui/view.h>
#include <gui/canvas.h>
#include <gui/elements.h>
#include <furi.h>

#include "../flipper_world_i.h"

#define MAX_FRAMES 3

typedef enum {
    FlipperActionsIdle,
    FlipperActionsEmote,
    FlipperActionsInteract,
    FlipperActionsMovement,
} FlipperActions;

typedef enum {
    FlipperMainEventUpdate,
    FlipperMainEventStartFoodGame,
    FlipperMainEventStartPassport,
    FlipperMainEventExit,
} FlipperMainEvent;

typedef struct {
    int32_t x;
    int32_t y;
} Vec2;

typedef enum {
    DirUp = InputKeyUp,
    DirRight = InputKeyRight,
    DirDown = InputKeyDown,
    DirLeft = InputKeyLeft,
} FrameDirectionEnum;

typedef struct {
    const Icon* f;
    const Icon* b;
} DolphinGfxAsset;

typedef struct {
    const DolphinGfxAsset frames[MAX_FRAMES];
    const uint8_t total;
    bool flip_v;
    bool flip_h;
} DolphinFrame;

typedef struct {
    Vec2 player;
    Vec2 player_global;
    Vec2 player_v;
    Vec2 screen;

    FlipperActions action;
    FrameDirectionEnum frame_group;
    FrameDirectionEnum last_group;
    FrameDirectionEnum frame_pending;
    FrameDirectionEnum frame_type;

    const DolphinFrame* current_frame;

    bool transition;
    bool transition_pending;
    bool use_pending;
    bool debug;

    uint8_t player_speed;
    uint8_t player_anim;
    uint8_t frame_idx;

    uint8_t scene_id;
    uint8_t emote_id;
    uint8_t previous_emote;

    uint8_t prev_action;
    uint8_t action_timeout;
    uint8_t dialog_progress;

} FlipperMainViewModel;

typedef struct FlipperMainView FlipperMainView;

typedef void (*FlipperMainViewCallback)(FlipperMainEvent event, void* context);

struct FlipperMainView {
    View* view;
    FlipperMainViewCallback callback;
    void* context;
    osTimerId_t timer;
};

void flipper_main_set_callback(
    FlipperMainView* main_view,
    FlipperMainViewCallback callback,
    void* context);

View* flipper_main_get_view(FlipperMainView* main_view);

FlipperMainView* flipper_world_main_alloc();

void flipper_world_main_free(FlipperMainView* main_view);

void flipper_world_tick_handler(FlipperMainView* main_view);
