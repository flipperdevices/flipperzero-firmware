#pragma once

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
    uint8_t frame_rate;     // frame changes per second
    uint16_t duration;
    uint16_t active_cooldown;
} BubbleAnimation;

AnimationManager* animation_manager_alloc(void);
void animation_manager_check_blocking(AnimationManager* animation_manager);
void animation_manager_start_new_idle_animation(AnimationManager* animation_manager);
void animation_manager_interact(AnimationManager* animation_manager);

void animation_manager_tie_view(AnimationManager* animation_manager, View* view);
void animation_manager_untie_view(AnimationManager* animation_manager);


