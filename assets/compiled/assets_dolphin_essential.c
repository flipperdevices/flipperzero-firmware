#include "assets_icons.h"
#include <assets_dolphin.h>
#include <desktop/animations/animation_storage_i.h>
#include <desktop/animations/animation_manager.h>
#include <gui/icon_i.h>

// Sd Bad
extern const uint8_t *_A_L0_SdBad_128x51[];
const BubbleAnimation BA_L0_SdBad_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=2,.frame_rate=2,.frames=_A_L0_SdBad_128x51}, .frame_order={0, 1}, .passive_frames = 2, .frame_rate = 2};

// Sd Ok
extern const uint8_t *_A_L0_SdOk_128x51[];
const BubbleAnimation BA_L0_SdOk_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=4,.frame_rate=2,.frames=_A_L0_SdOk_128x51}, .frame_order={0, 1, 2, 3}, .passive_frames = 4, .frame_rate = 2};

// No Db
extern const uint8_t *_A_L0_NoDb_128x51[];
const BubbleAnimation BA_L0_NoDb_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=4,.frame_rate=2,.frames=_A_L0_NoDb_128x51}, .frame_order={0, 1, 2, 3}, .passive_frames = 4, .frame_rate = 2};

// Url
extern const uint8_t *_A_L0_Url_128x51[];
const BubbleAnimation BA_L0_Url_128x51 = {.icon_animation = {.width=128,.height=51,.frame_count=4,.frame_rate=2,.frames=_A_L0_Url_128x51}, .frame_order={0, 1, 2, 3}, .passive_frames = 4, .frame_rate = 2};

const StorageAnimation StorageAnimationEssential[] = {
    /* Blocking */
    {.animation = &BA_L0_SdBad_128x51, .meta = {.name = "L0_SdBad_128x51"}},
    {.animation = &BA_L0_SdOk_128x51, .meta = {.name = "L0_SdOk_128x51"}},
    {.animation = &BA_L0_NoDb_128x51, .meta = {.name = "L0_NoDb_128x51"}},
    {.animation = &BA_L0_Url_128x51, .meta = {.name = "L0_Url_128x51"}},
};

const size_t StorageAnimationEssentialSize = COUNT_OF(StorageAnimationEssential);

