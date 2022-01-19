#include "assets_icons.h"
#include <assets_dolphin.h>
#include <desktop/animations/animation_storage_i.h>
#include <desktop/animations/animation_manager.h>
#include <gui/icon_i.h>

// Tv
const FrameBubble L1_Tv_bubble_0_0 = {.bubble = {.x = 1, .y = 23, .str = "Take the red pill", .horizontal = AlignRight, .vertical = AlignBottom}, .starts_at_frame = 7, .ends_at_frame = 9};
const FrameBubble L1_Tv_bubble_1_0 = {.bubble = {.x = 1, .y = 23, .str = "I can joke better", .horizontal = AlignRight, .vertical = AlignBottom}, .starts_at_frame = 7, .ends_at_frame = 9};
const FrameBubble* const L1_Tv_bubbles[] = {&L1_Tv_bubble_0_0, &L1_Tv_bubble_1_0};
extern const uint8_t *_A_L1_Tv_128x47[];
const BubbleAnimation BA_L1_Tv_128x47 = {.icon_animation = {.width=128,.height=47,.frame_count=8,.frame_rate=2,.frames=_A_L1_Tv_128x47}, .frame_order={0, 1, 2, 3, 4, 5, 6, 7}, .passive_frames = 6, .active_frames = 2, .active_cooldown = 5, .active_cycles = 2, .duration = 3600, .frame_rate = 2, .frame_bubble_sequence = L1_Tv_bubbles, .frame_bubble_sequence_count = COUNT_OF(L1_Tv_bubbles)};

// Bad Battery
const FrameBubble L1_BadBattery_bubble_0_0 = {.bubble = {.x = 4, .y = 21, .str = "I feel so sick!\nI'm dying...", .horizontal = AlignRight, .vertical = AlignCenter}, .starts_at_frame = 0, .ends_at_frame = 1};
const FrameBubble* const L1_BadBattery_bubbles[] = {&L1_BadBattery_bubble_0_0};
extern const uint8_t *_A_L1_BadBattery_128x47[];
const BubbleAnimation BA_L1_BadBattery_128x47 = {.icon_animation = {.width=128,.height=49,.frame_count=2,.frame_rate=2,.frames=_A_L1_BadBattery_128x47}, .frame_order={0, 1}, .passive_frames = 2, .frame_rate = 2, .duration = 3600, .frame_bubble_sequence = L1_BadBattery_bubbles, .frame_bubble_sequence_count = COUNT_OF(L1_BadBattery_bubbles)};

// No Sd
const FrameBubble L1_NoSd_bubble_0_0 = {.bubble = {.x = 40, .y = 18, .str = "Need an\nSD card", .horizontal = AlignRight, .vertical = AlignBottom}, .starts_at_frame = 0, .ends_at_frame = 9};
const FrameBubble* const L1_NoSd_bubbles[] = {&L1_NoSd_bubble_0_0};
extern const uint8_t *_A_L1_NoSd_128x49[];
const BubbleAnimation BA_L1_NoSd_128x49 = {.icon_animation = {.width=128,.height=49,.frame_count=6,.frame_rate=2,.frames=_A_L1_NoSd_128x49}, .frame_order={0, 1, 0, 1, 0, 2, 3, 4, 3, 5}, .passive_frames = 10, .frame_rate = 2, .duration = 3600, .frame_bubble_sequence = L1_NoSd_bubbles, .frame_bubble_sequence_count = COUNT_OF(L1_NoSd_bubbles)};

const StorageAnimation StorageAnimationInternal[] = {
    /* System */
    {.animation = &BA_L1_BadBattery_128x47,
     .manifest_info = {.name = "L1_BadBattery_128x47", .min_butthurt = 0, .max_butthurt = 14, .min_level = 1, .max_level = 3, .weight = 3,}},
    {.animation = &BA_L1_NoSd_128x49,
     .manifest_info = {.name = "L1_NoSd_128x49", .min_butthurt = 0, .max_butthurt = 14, .min_level = 1, .max_level = 3, .weight = 6,}},

    /* Idle */
    {.animation = &BA_L1_Tv_128x47,
     .manifest_info = {.name = "L1_Tv_128x47", .min_butthurt = 0, .max_butthurt = 14, .min_level = 1, .max_level = 3, .weight = 3,}},
};

const size_t StorageAnimationInternalSize = COUNT_OF(StorageAnimationInternal);

