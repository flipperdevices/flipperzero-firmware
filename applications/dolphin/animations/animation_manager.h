#pragma once

#include "dolphin/dolphin.h"
#include <gui/view.h>
#include <stdint.h>

typedef struct AnimationManager AnimationManager;

typedef struct {
    uint8_t x;
    uint8_t y;
    const char* str;
    Align horizontal;
    Align vertical;
} Bubble;

typedef struct FrameBubble {
    Bubble bubble;
    uint8_t starts_at_frame;
    uint8_t ends_at_frame;
    struct FrameBubble* next_bubble;
} FrameBubble;

typedef struct {
    FrameBubble** frame_bubbles;
    uint8_t frame_bubbles_count;
    const Icon** icons;
    uint8_t passive_frames;
    uint8_t active_frames;
    uint8_t active_cycles;
    uint8_t frame_rate;
    uint16_t duration;
    uint16_t active_cooldown;
} BubbleAnimation;

typedef void (*AnimationManagerSetNewIdleAnimationCallback)(void* context);
typedef void (*AnimationManagerCheckBlockingCallback)(void* context);
typedef void (*AnimationManagerInteractCallback)(void*);

AnimationManager* animation_manager_alloc(void);
void animation_manager_free(AnimationManager* animation_manager);

View* animation_manager_get_animation_view(AnimationManager* animation_manager);

void animation_manager_set_context(AnimationManager* animation_manager, void* context);
void animation_manager_set_new_idle_callback(AnimationManager* animation_manager, AnimationManagerSetNewIdleAnimationCallback callback);
void animation_manager_set_check_callback(AnimationManager* animation_manager, AnimationManagerCheckBlockingCallback callback);
void animation_manager_set_interact_callback(AnimationManager* animation_manager, AnimationManagerInteractCallback callback);
void animation_manager_new_idle_process(AnimationManager* animation_manager);
void animation_manager_check_blocking_process(AnimationManager* animation_manager);
void animation_manager_interact_process(AnimationManager* animation_manager);

void animation_manager_load_and_continue_animation(AnimationManager* animation_manager);
void animation_manager_unload_and_stall_animation(AnimationManager* animation_manager);

