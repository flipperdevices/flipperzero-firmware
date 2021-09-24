#pragma once
#include "../flipper_world_i.h"

const DolphinFrame up = {
    .frames =
        {
            {
                .f = &I_up1_73x61,
                .b = &I_black_up1_73x61,
            },
            {
                .f = &I_up2_73x61,
                .b = &I_black_up2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame up_down = {
    .frames =
        {
            {
                .f = &I_updown1_73x61,
                .b = &I_black_updown1_73x61,
            },
            {
                .f = &I_updown3_73x61,
                .b = &I_black_updown3_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame up_right = {
    .frames =
        {
            {
                .f = &I_upright1_73x61,
                .b = &I_black_upright1_73x61,
            },
            {
                .f = &I_upright2_73x61,
                .b = &I_black_upright2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame up_left = {
    .frames =
        {
            {
                .f = &I_upright1_73x61,
                .b = &I_black_upright1_73x61,
            },
            {
                .f = &I_upright2_73x61,
                .b = &I_black_upright2_73x61,
            },
        },
    .total = 2,
    .flip_h = true,
};

const DolphinFrame right = {
    .frames =
        {
            {
                .f = &I_right1_73x61,
                .b = &I_black_right1_73x61,
            },
            {
                .f = &I_right2_73x61,
                .b = &I_black_right2_73x61,
            },
            {
                .f = &I_right3_73x61,
                .b = &I_black_right3_73x61,
            },
        },
    .total = 3,
};

const DolphinFrame right_up = {
    .frames =
        {
            {
                .f = &I_rightup1_73x61,
                .b = &I_black_rightup1_73x61,
            },
            {
                .f = &I_rightup2_73x61,
                .b = &I_black_rightup2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame right_down = {
    .frames =
        {
            {
                .f = &I_rightup1_73x61,
                .b = &I_black_rightup1_73x61,
            },
            {
                .f = &I_rightup2_73x61,
                .b = &I_black_rightup2_73x61,
            },
        },
    .total = 2,
    .flip_v = true,
};

const DolphinFrame right_left = {
    .frames =
        {
            {
                .f = &I_rightleft1_73x61,
                .b = &I_black_rightleft1_73x61,
            },
            {
                .f = &I_rightleft2_73x61,
                .b = &I_black_rightleft2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame down = {
    .frames =
        {
            {
                .f = &I_up1_73x61,
                .b = &I_black_up1_73x61,
            },
            {
                .f = &I_up2_73x61,
                .b = &I_black_up2_73x61,
            },
        },
    .total = 2,
    .flip_v = true,
};

const DolphinFrame down_up = {
    .frames =
        {
            {
                .f = &I_updown1_73x61,
                .b = &I_black_updown1_73x61,
            },
            {
                .f = &I_updown3_73x61,
                .b = &I_black_updown3_73x61,
            },
        },
    .total = 2,
    .flip_v = true,
};

const DolphinFrame down_left = {
    .frames =
        {
            {
                .f = &I_downright1_73x61,
                .b = &I_black_downright1_73x61,
            },
            {
                .f = &I_downright2_73x61,
                .b = &I_black_downright2_73x61,
            },
        },
    .total = 2,
    .flip_h = true,
};

const DolphinFrame down_right = {
    .frames =
        {
            {
                .f = &I_downright1_73x61,
                .b = &I_black_downright1_73x61,
            },
            {
                .f = &I_downright2_73x61,
                .b = &I_black_downright2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame left = {
    .frames =
        {
            {
                .f = &I_right1_73x61,
                .b = &I_black_right1_73x61,
            },
            {
                .f = &I_right2_73x61,
                .b = &I_black_right2_73x61,
            },
            {
                .f = &I_right3_73x61,
                .b = &I_black_right3_73x61,
            },
        },
    .total = 3,
    .flip_h = true,
};

const DolphinFrame left_up = {
    .frames =
        {
            {
                .f = &I_rightup1_73x61,
                .b = &I_black_rightup1_73x61,
            },
            {
                .f = &I_rightup2_73x61,
                .b = &I_black_rightup2_73x61,
            },
        },
    .total = 2,
    .flip_h = true,
};

const DolphinFrame left_down = {
    .frames =
        {
            {
                .f = &I_rightdown1_73x61,
                .b = &I_black_rightdown1_73x61,
            },
            {
                .f = &I_rightdown2_73x61,
                .b = &I_black_rightdown2_73x61,
            },
        },
    .total = 2,
    .flip_h = true,
};

const DolphinFrame left_right = {
    .frames =
        {
            {
                .f = &I_rightleft1_73x61,
                .b = &I_black_rightleft1_73x61,
            },
            {
                .f = &I_rightleft2_73x61,
                .b = &I_black_rightleft2_73x61,
            },
        },
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
