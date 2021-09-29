#pragma once
#include "../flipper_world_i.h"

// icons/Scenes/Swimming/down1_73x61.png enc=48 w=73 h=61
// icons/Scenes/Swimming/down2_73x61.png enc=49 w=73 h=61
// icons/Scenes/Swimming/downleft1_73x61.png enc=50 w=73 h=61
// icons/Scenes/Swimming/downleft2_73x61.png enc=51 w=73 h=61
// icons/Scenes/Swimming/downleft3_73x61.png enc=52 w=73 h=61
// icons/Scenes/Swimming/downright1_73x61.png enc=53 w=73 h=61
// icons/Scenes/Swimming/downright2_73x61.png enc=54 w=73 h=61
// icons/Scenes/Swimming/downright3_73x61.png enc=55 w=73 h=61
// icons/Scenes/Swimming/downup1_73x61.png enc=56 w=73 h=61
// icons/Scenes/Swimming/downup2_73x61.png enc=57 w=73 h=61
// icons/Scenes/Swimming/downup3_73x61.png enc=58 w=73 h=61
// icons/Scenes/Swimming/left1_73x61.png enc=59 w=73 h=61
// icons/Scenes/Swimming/left2_73x61.png enc=60 w=73 h=61
// icons/Scenes/Swimming/left3_73x61.png enc=61 w=73 h=61
// icons/Scenes/Swimming/leftdown1_73x61.png enc=62 w=73 h=61
// icons/Scenes/Swimming/leftdown2_73x61.png enc=63 w=73 h=61
// icons/Scenes/Swimming/leftup1_73x61.png enc=64 w=73 h=61
// icons/Scenes/Swimming/leftup2_73x61.png enc=65 w=73 h=61
// icons/Scenes/Swimming/right1_73x61.png enc=66 w=73 h=61
// icons/Scenes/Swimming/right2_73x61.png enc=67 w=73 h=61
// icons/Scenes/Swimming/right3_73x61.png enc=68 w=73 h=61
// icons/Scenes/Swimming/rightdown1_73x61.png enc=69 w=73 h=61
// icons/Scenes/Swimming/rightdown2_73x61.png enc=70 w=73 h=61
// icons/Scenes/Swimming/rightleft1_73x61.png enc=71 w=73 h=61
// icons/Scenes/Swimming/rightleft2_73x61.png enc=72 w=73 h=61
// icons/Scenes/Swimming/rightup1_73x61.png enc=73 w=73 h=61
// icons/Scenes/Swimming/rightup2_73x61.png enc=74 w=73 h=61
// icons/Scenes/Swimming/up1_73x61.png enc=75 w=73 h=61
// icons/Scenes/Swimming/up2_73x61.png enc=76 w=73 h=61
// icons/Scenes/Swimming/updown1_73x61.png enc=77 w=73 h=61
// icons/Scenes/Swimming/updown2_73x61.png enc=78 w=73 h=61
// icons/Scenes/Swimming/updown3_73x61.png enc=79 w=73 h=61
// icons/Scenes/Swimming/upleft1_73x61.png enc=80 w=73 h=61
// icons/Scenes/Swimming/upleft2_73x61.png enc=81 w=73 h=61
// icons/Scenes/Swimming/upright1_73x61.png enc=82 w=73 h=61
// icons/Scenes/Swimming/upright2_73x61.png enc=83 w=73 h=61

const DolphinFrame up = {
    .frames = {75, 76},
    .total = 2,
};

const DolphinFrame up_down = {
    .frames = {77, 78}, //79
    .total = 2,
};

const DolphinFrame up_right = {
    .frames = {82, 83},
    .total = 2,
};

const DolphinFrame up_left = {
    .frames = {80, 81},
    .total = 2,
    // .flip_h = true,
};

const DolphinFrame right = {
    .frames = {66, 67, 68},
    .total = 3,
};

const DolphinFrame right_up = {
    .frames = {73, 74},
    .total = 2,
};

const DolphinFrame right_down = {
    .frames = {69, 70},
    .total = 2,
    // .flip_v = true,
};

const DolphinFrame right_left = {
    .frames = {71, 72},
    .total = 2,
};

const DolphinFrame down = {
    .frames = {48, 49},
    .total = 2,
    // .flip_v = true,
};

const DolphinFrame down_up = {
    .frames = {56, 58}, // 57
    .total = 2,
    // .flip_v = true,
};

const DolphinFrame down_left = {
    .frames = {50, 52}, // 51
    .total = 2,
    // .flip_h = true,
};

const DolphinFrame down_right = {
    .frames = {53, 55}, // 54
    .total = 2,
};

const DolphinFrame left = {
    .frames = {59, 60, 61},
    .total = 3,
    // .flip_h = true,
};

const DolphinFrame left_up = {
    .frames = {64, 65},
    .total = 2,
    // .flip_h = true,
};

const DolphinFrame left_down = {
    .frames = {62, 63},
    .total = 2,
    // .flip_h = true,
};

const DolphinFrame left_right = {
    .frames = {72, 71},
    .total = 2,
    // .flip_h = true,
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
