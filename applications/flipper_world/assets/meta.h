#pragma once
#include "../flipper_world_i.h"

const DolphinFrame up = {
    .frames = {59, 60},
    .total = 2,
};

const DolphinFrame up_down = {
    .frames = {61, 62},
    .total = 2,
};

const DolphinFrame up_right = {
    .frames = {63, 64},
    .total = 2,
};

const DolphinFrame up_left = {
    .frames = {63, 64},
    .total = 2,
    .flip_h = true,
};

const DolphinFrame right = {
    .frames = {50, 51, 52},
    .total = 3,
};

const DolphinFrame right_up = {
    .frames = {57, 58},
    .total = 2,
};

const DolphinFrame right_down = {
    .frames = {53, 54},
    .total = 2,
    .flip_v = true,
};

const DolphinFrame right_left = {
    .frames = {55, 56},
    .total = 2,
};

const DolphinFrame down = {
    .frames = {59, 60},
    .total = 2,
    .flip_v = true,
};

const DolphinFrame down_up = {
    .frames = {61, 62},
    .total = 2,
    .flip_v = true,
};

const DolphinFrame down_left = {
    .frames = {48, 49},
    .total = 2,
    .flip_h = true,
};

const DolphinFrame down_right = {
    .frames = {48, 49},
    .total = 2,
};

const DolphinFrame left = {
    .frames = {50, 51, 52},
    .total = 3,
    .flip_h = true,
};

const DolphinFrame left_up = {
    .frames = {57, 58},
    .total = 2,
    .flip_h = true,
};

const DolphinFrame left_down = {
    .frames = {53, 54},
    .total = 2,
    .flip_h = true,
};

const DolphinFrame left_right = {
    .frames = {55, 56},
    .total = 2,
    .flip_h = true,
};

const DolphinFrame* dolphin_frames[4][4] = {
    [DirUp] = {[DirUp] = &up, [DirRight] = &up_right, [DirDown] = &up_down, [DirLeft] = &up_left},
    [DirRight] =
        {[DirUp] = &right_up, [DirRight] = &right, [DirDown] = &right_down, [DirLeft] = &right_left},
    [DirDown] =
        {[DirUp] = &down_up, [DirRight] = &down_right, [DirDown] = &down, [DirLeft] = &down_left},
    [DirLeft] =
        {[DirUp] = &left_up, [DirRight] = &left_right, [DirDown] = &left_down, [DirLeft] = &left},
};
