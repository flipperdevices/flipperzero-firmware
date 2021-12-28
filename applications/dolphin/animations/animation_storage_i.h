#pragma once
#include "animation_storage.h"
#include "assets_icons.h"
#include "dolphin/animations/animation_manager.h"
#include "gui/canvas.h"

struct StorageAnimation {
    const BubbleAnimation* animation;
    bool external;
    StorageAnimationMeta meta;
};

FrameBubble tv_bubble1_2 = {
    .bubble = {.x = 0, .y = 15, .str = "second\nbubble", .horizontal = AlignRight, .vertical = AlignBottom},
    .starts_at_frame = 4,
    .ends_at_frame = 4,
    .next_bubble = NULL,
};

FrameBubble tv_bubble1_1 = {
    .bubble = {.x = 0, .y = 15, .str = "1. first bubble", .horizontal = AlignRight, .vertical = AlignBottom},
    .starts_at_frame = 1,
    .ends_at_frame = 2,
    .next_bubble = &tv_bubble1_2,
};

FrameBubble tv_bubble2_1 = {
    .bubble = {.x = 0, .y = 15, .str = "2. first bubble", .horizontal = AlignRight, .vertical = AlignBottom},
    .starts_at_frame = 2,
    .ends_at_frame = 4,
    .next_bubble = NULL,
};

FrameBubble* tv_bubbles[] = { &tv_bubble1_1, &tv_bubble2_1 };

const Icon* tv_icons[] = {
    &I_tv_frame_01,
    &I_tv_frame_02,
    &I_tv_frame_03,
    &I_tv_frame_04,
    &I_tv_frame_05,
    &I_tv_frame_06,
    &I_tv_frame_07,
    &I_tv_frame_08,
    &I_tv_frame_09,
    &I_tv_frame_10,
    &I_tv_frame_11,
    &I_tv_frame_12,
};

const BubbleAnimation tv_bubble_animation = {
    .icons = tv_icons,
    .frame_bubbles = tv_bubbles,
    .frame_bubbles_count = COUNT_OF(tv_bubbles),
    .passive_frames = 6,
    .active_frames = 6,
    .frame_rate = 2,
    .duration = 10,
    .active_cooldown = 3,
    .active_cycles = 1,
};




const Icon* no_sd_icons[] = {
    &I_tv_frame_01, // for tests
    &I_tv_frame_02,
    &I_tv_frame_03,
    &I_tv_frame_04,
    &I_tv_frame_05,
    &I_tv_frame_06,
};

FrameBubble no_sd_bubble = {
    .bubble = {.x = 0, .y = 35, .str = "Buy me a\nmicroSD\ncard, plz", .horizontal = AlignRight, .vertical = AlignBottom},
    .starts_at_frame = 0,
    .ends_at_frame = 5,
    .next_bubble = NULL,
};

FrameBubble* no_sd_bubbles[] = { &no_sd_bubble };

const BubbleAnimation no_sd_bubble_animation = {
    .icons = no_sd_icons,
    .frame_bubbles = no_sd_bubbles,
    .frame_bubbles_count = COUNT_OF(no_sd_bubbles),
    .passive_frames = 6,
    .active_frames = 0,
    .frame_rate = 2,
    .duration = 10,
    .active_cooldown = 3,
    .active_cycles = 1,
};

static StorageAnimation StorageAnimationInternal[] = {
    {
        .animation = &tv_bubble_animation,
        .external = false,
        .meta = {
//            .name = "tv",
            .min_butthurt = 0,
            .max_butthurt = 3,
            .min_level = 1,
            .max_level = 1,
            .weight = 1,
        }
    },
    {
        .animation = &no_sd_bubble_animation,
        .external = false,
        .meta = {
//            .name = "no_sd",
            .min_butthurt = 0,
            .max_butthurt = 3,
            .min_level = 1,
            .max_level = 1,
            .weight = 5,
        }
    },
};

/* not static inline*/
__attribute__((constructor))
void animation_storage_initialize_internal_animations(void) {
    string_init_set_str(StorageAnimationInternal[0].meta.name, HARDCODED_ANIMATION_NAME);
    string_init_set_str(StorageAnimationInternal[1].meta.name, NO_SD_ANIMATION_NAME);
}

