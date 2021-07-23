#include <furi.h>
#include "scene.h"
#include "assets/items.h"
#include <gui/elements.h>

const char* action_str[] = {"Idle", "Emote", "Use", "MC"};

#define MAX_FRAMES 3
typedef enum {
    GroupUp = 0,
    GroupRight,
    GroupDown,
    GroupLeft,
} FrameGroupsEnum;

typedef enum {
    FrameUp = 0,
    FrameDown,
    FrameLeft,
    FrameRight,
} FrameTypeEnum;

typedef struct {
    const Icon* f;
    const Icon* b;
} DolphinGfxAsset;

typedef struct {
    const DolphinGfxAsset frames[MAX_FRAMES];
    const uint8_t total;
} DolphinFrame;

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
                .f = &I_updown2_73x61,
                .b = &I_black_updown2_73x61,
            },
            {
                .f = &I_updown3_73x61,
                .b = &I_black_updown3_73x61,
            },
        },
    .total = 3,
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
                .f = &I_upleft1_73x61,
                .b = &I_black_upleft1_73x61,
            },
            {
                .f = &I_upleft2_73x61,
                .b = &I_black_upleft2_73x61,
            },
        },
    .total = 2,
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
                .f = &I_rightdown1_73x61,
                .b = &I_black_rightdown1_73x61,
            },
            {
                .f = &I_rightdown2_73x61,
                .b = &I_black_rightdown2_73x61,
            },
        },
    .total = 2,
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
                .f = &I_down1_73x61,
                .b = &I_black_down1_73x61,
            },
            {
                .f = &I_down2_73x61,
                .b = &I_black_down2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame down_up = {
    .frames =
        {
            {
                .f = &I_downup1_73x61,
                .b = &I_black_downup1_73x61,
            },
            {
                .f = &I_downup2_73x61,
                .b = &I_black_downup2_73x61,
            },
            {
                .f = &I_downup3_73x61,
                .b = &I_black_downup3_73x61,
            },
        },
    .total = 3,
};

const DolphinFrame down_left = {
    .frames =
        {
            {
                .f = &I_downleft1_73x61,
                .b = &I_black_downleft1_73x61,
            },
            {
                .f = &I_downleft2_73x61,
                .b = &I_black_downleft2_73x61,
            },
            {
                .f = &I_downleft3_73x61,
                .b = &I_black_downleft3_73x61,
            },
        },
    .total = 3,
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
            {
                .f = &I_downright3_73x61,
                .b = &I_black_downright3_73x61,
            },
        },
    .total = 3,
};

const DolphinFrame left = {
    .frames =
        {
            {
                .f = &I_left1_73x61,
                .b = &I_black_left1_73x61,
            },
            {
                .f = &I_left2_73x61,
                .b = &I_black_left2_73x61,
            },
            {
                .f = &I_left3_73x61,
                .b = &I_black_left3_73x61,
            },
        },
    .total = 3,
};

const DolphinFrame left_up = {
    .frames =
        {
            {
                .f = &I_leftup1_73x61,
                .b = &I_black_leftup1_73x61,
            },
            {
                .f = &I_leftup2_73x61,
                .b = &I_black_leftup2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame left_down = {
    .frames =
        {
            {
                .f = &I_leftdown1_73x61,
                .b = &I_black_leftdown1_73x61,
            },
            {
                .f = &I_leftdown2_73x61,
                .b = &I_black_leftdown2_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame left_right = {
    .frames =
        {
            {
                .f = &I_rightleft2_73x61,
                .b = &I_black_rightleft2_73x61,
            },
            {
                .f = &I_rightleft1_73x61,
                .b = &I_black_rightleft1_73x61,
            },
        },
    .total = 2,
};

const DolphinFrame* frames[4][4] = {
    [GroupUp] =
        {[FrameUp] = &up, [FrameDown] = &up_down, [FrameLeft] = &up_left, [FrameRight] = &up_right},

    [GroupRight] =
        {[FrameUp] = &right_up,
         [FrameDown] = &right_down,
         [FrameLeft] = &right_left,
         [FrameRight] = &right},
    [GroupDown] =
        {[FrameUp] = &down_up,
         [FrameDown] = &down,
         [FrameLeft] = &down_left,
         [FrameRight] = &down_right},
    [GroupLeft] =
        {[FrameUp] = &left_up,
         [FrameDown] = &left_down,
         [FrameLeft] = &left,
         [FrameRight] = &left_right},
};

void dolphin_scene_render_dolphin(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    const DolphinFrame* current_frame = NULL;
    FrameGroupsEnum group = GroupRight;
    FrameTypeEnum frame = 0;

    state->player_anim = (HAL_GetTick() / 200) % 3;

    if(state->scene_zoom == SCENE_ZOOM) {
        state->dolphin_gfx = &I_DolphinExcited_64x63;
    } else if(state->action != INTERACT) {
        //default
        current_frame = *&frames[GroupRight][FrameRight];

        if(state->player_v.x < 0 && state->player_v.y < 0) {
            if(state->transition) {
            } else {
                current_frame = *&frames[GroupLeft][FrameUp];
            }
        } else if(state->player_v.x < 0 && state->player_v.y > 0) {
            if(state->transition) {
            } else {
                current_frame = *&frames[GroupLeft][FrameDown];
            }
        } else if(state->player_v.x > 0 && state->player_v.y > 0) {
            if(state->transition) {
            } else {
                current_frame = *&frames[GroupRight][FrameDown];
            }
        } else if(state->player_v.x > 0 && state->player_v.y < 0) {
            if(state->transition) {
            } else {
                current_frame = *&frames[GroupRight][FrameUp];
            }
        } else if(state->player_v.y < 0) {
            group = GroupUp;

            if(state->transition) {
            } else {
                if(state->player_v.x > 0) {
                    frame = FrameRight;
                } else if(state->player_v.x < 0) {
                    frame = FrameLeft;
                } else if(state->player_v.x == 0) {
                    frame = FrameUp;
                }

                current_frame = *&frames[group][frame];
            }
        } else if(state->player_v.y > 0) {
            group = GroupDown;

            if(state->transition) {
            } else {
                if(state->player_v.x > 0) {
                    frame = FrameRight;
                } else if(state->player_v.x < 0) {
                    frame = FrameLeft;
                } else if(state->player_v.x == 0) {
                    frame = FrameDown;
                }

                current_frame = *&frames[group][frame];
            }
        } else if(state->player_v.x < 0 || state->player_flipped_x) {
            if(state->transition) {
                current_frame = *&frames[GroupRight][FrameLeft];
            } else {
                current_frame = *&frames[GroupLeft][FrameLeft];
            }
        } else if(state->player_v.x > 0 || !state->player_flipped_x) {
            if(state->transition) {
                current_frame = *&frames[GroupLeft][FrameRight];
            } else {
                current_frame = *&frames[GroupRight][FrameRight];
            }
        }

        state->dolphin_gfx = current_frame->frames[state->player_anim % current_frame->total].f;
        state->dolphin_gfx_b = current_frame->frames[state->player_anim % current_frame->total].b;
    }

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, state->player.x - 1, state->player.y - 1, state->dolphin_gfx_b);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, state->player.x, state->player.y, state->dolphin_gfx);
    canvas_set_bitmap_mode(canvas, false);
}

static bool item_screen_bounds(int32_t pos) {
    return pos > -SCREEN_WIDTH && pos < (SCREEN_WIDTH * 2);
}

void dolphin_scene_render(SceneState* state, Canvas* canvas, uint32_t t) {
    furi_assert(state);
    furi_assert(canvas);

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);
    const Item** current_scene = get_scene(state);

    for(uint8_t l = 0; l < LAYERS; l++) {
        if(state->scene_zoom < SCENE_ZOOM) {
            for(uint8_t i = 0; i < ItemsEnumTotal; i++) {
                int32_t item_pos_X = (current_scene[i]->pos.x - state->player_global.x);
                int32_t item_pos_Y = (current_scene[i]->pos.y - state->player_global.y);

                IconAnimation* animation = NULL;

                if(item_screen_bounds(item_pos_X)) {
                    if(l == current_scene[i]->layer) {
                        if(current_scene[i]->icon) {
                            canvas_draw_icon(
                                canvas,
                                item_pos_X * PARALLAX(l),
                                item_pos_Y,
                                current_scene[i]->icon);
                            canvas_set_bitmap_mode(canvas, false);
                        }

                        if(current_scene[i]->draw) {
                            current_scene[i]->draw(canvas, state);
                        }
                    }

                } else {
                    if(animation != NULL) {
                        icon_animation_free(animation);
                    }
                }
            }
        }

        if(l == DOLPHIN_LAYER) dolphin_scene_render_dolphin(state, canvas);
    }
}

void dolphin_scene_render_state(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    char buf[64];

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);

    // dolphin_scene_debug
    if(state->debug) {
        sprintf(
            buf,
            "x:%ld.%ld>%ld.%ld %ld %s %d",
            state->player_global.x,
            state->player_global.y,
            state->poi.x,
            state->poi.y,
            state->action_timeout,
            action_str[state->action],
            state->transition);
        canvas_draw_str(canvas, 0, 13, buf);
    }

    if(state->action == INTERACT) scene_activate_item_callback(state, canvas);
}