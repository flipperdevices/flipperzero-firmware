#include "assets_icons.h"
#include <assets_animation.h>
#include <desktop/animations/animation_storage_i.h>
#include <desktop/animations/animation_manager.h>
#include <gui/icon_i.h>

// Sd Bad
extern const uint8_t *_A_SdBad_128x51[];
const BubbleAnimation BA_SdBad_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=2,.frame_rate=2,.frames=_A_SdBad_128x51}, .frame_order={0, 1}, .passive_frames = 2, .frame_rate = 2};

// Sd Ok
extern const uint8_t *_A_SdOk_128x51[];
const BubbleAnimation BA_SdOk_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=4,.frame_rate=2,.frames=_A_SdOk_128x51}, .frame_order={0, 1, 2, 3}, .passive_frames = 4, .frame_rate = 2};

// No Db
extern const uint8_t *_A_NoDb_128x51[];
const BubbleAnimation BA_NoDb_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=4,.frame_rate=2,.frames=_A_NoDb_128x51}, .frame_order={0, 1, 2, 3}, .passive_frames = 4, .frame_rate = 2};

// Url
extern const uint8_t *_A_Url_128x51[];
const BubbleAnimation BA_Url_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=4,.frame_rate=2,.frames=_A_Url_128x51}, .frame_order={0, 1, 2, 3}, .passive_frames = 4, .frame_rate = 2};

// Tv
const FrameBubble Tv_bubble_0_0 = {.bubble = {.x = 1, .y = 23, .str = "Take the red pill", .horizontal = AlignRight, .vertical = AlignBottom}, .starts_at_frame = 7, .ends_at_frame = 9};
const FrameBubble Tv_bubble_1_0 = {.bubble = {.x = 1, .y = 23, .str = "I can joke better", .horizontal = AlignRight, .vertical = AlignBottom}, .starts_at_frame = 7, .ends_at_frame = 9};
const FrameBubble* const Tv_bubbles[] = {&Tv_bubble_0_0, &Tv_bubble_1_0};
extern const uint8_t *_A_Tv_128x47[];
const BubbleAnimation BA_Tv_128x47 = {.icon_animation = {.width=128,.height=47,.frame_count=8,.frame_rate=2,.frames=_A_Tv_128x47}, .frame_order={0, 1, 2, 3, 4, 5, 6, 7}, .passive_frames = 6, .active_frames = 2, .active_cooldown = 5, .active_cycles = 2, .duration = 3600, .frame_rate = 2, .frame_bubble_sequence = Tv_bubbles, .frame_bubble_sequence_count = COUNT_OF(Tv_bubbles)};

// Bad Battery
const FrameBubble BadBattery_bubble_0_0 = {.bubble = {.x = 4, .y = 21, .str = "I feel so sick!\nI'm dying...", .horizontal = AlignRight, .vertical = AlignCenter}, .starts_at_frame = 0, .ends_at_frame = 1};
const FrameBubble* const BadBattery_bubbles[] = {&BadBattery_bubble_0_0};
extern const uint8_t *_A_BadBattery_128x47[];
const BubbleAnimation BA_BadBattery_128x47 = {.icon_animation = {.width=128,.height=49,.frame_count=2,.frame_rate=2,.frames=_A_BadBattery_128x47}, .frame_order={0, 1}, .passive_frames = 2, .frame_rate = 2, .duration = 3600, .frame_bubble_sequence = BadBattery_bubbles, .frame_bubble_sequence_count = COUNT_OF(BadBattery_bubbles)};

// No Sd
const FrameBubble NoSd_bubble_0_0 = {.bubble = {.x = 40, .y = 18, .str = "Need an\nSD card", .horizontal = AlignRight, .vertical = AlignBottom}, .starts_at_frame = 0, .ends_at_frame = 9};
const FrameBubble* const NoSd_bubbles[] = {&NoSd_bubble_0_0};
extern const uint8_t *_A_NoSd_128x49[];
const BubbleAnimation BA_NoSd_128x49 = {.icon_animation = {.width=128,.height=49,.frame_count=6,.frame_rate=2,.frames=_A_NoSd_128x49}, .frame_order={0, 1, 0, 1, 0, 2, 3, 4, 3, 5}, .passive_frames = 10, .frame_rate = 2, .duration = 3600, .frame_bubble_sequence = NoSd_bubbles, .frame_bubble_sequence_count = COUNT_OF(NoSd_bubbles)};

const StorageAnimation StorageAnimationInternal[] = {
    /* Blocking */
    {.animation = &BA_SdBad_128x51, .meta = {.name = "SdBad_128x51"}},
    {.animation = &BA_SdOk_128x51, .meta = {.name = "SdOk_128x51"}},
    {.animation = &BA_NoDb_128x51, .meta = {.name = "NoDb_128x51"}},
    {.animation = &BA_Url_128x51, .meta = {.name = "Url_128x51"}},

    /* System */
    {.animation = &BA_BadBattery_128x47,
     .meta = {.name = "BadBattery_128x47", .min_butthurt = 0, .max_butthurt = 14, .min_level = 1, .max_level = 3, .weight = 3,}},
    {.animation = &BA_NoSd_128x49,
     .meta = {.name = "NoSd_128x49", .min_butthurt = 0, .max_butthurt = 14, .min_level = 1, .max_level = 3, .weight = 6,}},

    /* Idle */
    {.animation = &BA_Tv_128x47,
     .meta = {.name = "Tv_128x47", .min_butthurt = 0, .max_butthurt = 14, .min_level = 1, .max_level = 3, .weight = 3,}},
};

const size_t StorageAnimationInternalSize = COUNT_OF(StorageAnimationInternal);

