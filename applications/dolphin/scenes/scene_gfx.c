#include <furi.h>
#include "scene.h"
#include "assets/items.h"
#include <gui/elements.h>

const char* action_str[] = {"Idle", "Emote", "Use", "MC"};

void dolphin_scene_render_dolphin(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    if(state->scene_zoom == SCENE_ZOOM) {
        state->dolphin_gfx = &I_DolphinExcited_64x63;
    } else if(state->action != INTERACT) {
        if(state->player_v.x < 0 && state->player_v.y < 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_leftup1_73x61;
                    state->dolphin_gfx_b = &I_black_leftup1_73x61;
                } else {
                    state->dolphin_gfx = &I_leftup2_73x61;
                    state->dolphin_gfx_b = &I_black_leftup2_73x61;
                }
            }

        } else if(state->player_v.x < 0 && state->player_v.y > 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_leftdown1_73x61;
                    state->dolphin_gfx_b = &I_black_leftdown1_73x61;
                } else {
                    state->dolphin_gfx = &I_leftdown2_73x61;
                    state->dolphin_gfx_b = &I_black_leftdown2_73x61;
                }
            }

        } else if(state->player_v.x > 0 && state->player_v.y > 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_rightdown1_73x61;
                    state->dolphin_gfx_b = &I_black_rightdown1_73x61;
                } else {
                    state->dolphin_gfx = &I_rightdown2_73x61;
                    state->dolphin_gfx_b = &I_black_rightdown2_73x61;
                }
            }
        } else if(state->player_v.x > 0 && state->player_v.y < 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_rightup1_73x61;
                    state->dolphin_gfx_b = &I_black_rightup1_73x61;
                } else {
                    state->dolphin_gfx = &I_rightup2_73x61;
                    state->dolphin_gfx_b = &I_black_rightup2_73x61;
                }
            }

        } else if(state->player_v.y < 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_up1_73x61;
                    state->dolphin_gfx_b = &I_black_up1_73x61;

                } else {
                    state->dolphin_gfx = &I_up1_73x61;
                    state->dolphin_gfx_b = &I_black_up2_73x61;
                }
            }
        } else if(state->player_v.y > 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_down1_73x61;
                    state->dolphin_gfx_b = &I_black_down1_73x61;
                } else {
                    state->dolphin_gfx = &I_down1_73x61;
                    state->dolphin_gfx_b = &I_black_down2_73x61;
                }
            }
        }

        else if(state->player_v.x < 0 || state->player_flipped_x) {
            if(state->transition) {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_rightleft1_73x61;
                    state->dolphin_gfx_b = &I_black_rightleft1_73x61;
                } else {
                    state->dolphin_gfx = &I_rightleft2_73x61;
                    state->dolphin_gfx_b = &I_black_rightleft2_73x61;
                }

            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_left1_73x61;
                    state->dolphin_gfx_b = &I_black_left1_73x61;
                } else if(state->player_anim == 1) {
                    state->dolphin_gfx = &I_left2_73x61;
                    state->dolphin_gfx_b = &I_black_left2_73x61;

                } else {
                    state->dolphin_gfx = &I_left3_73x61;
                    state->dolphin_gfx_b = &I_black_left3_73x61;
                }
            }
        } else if(state->player_v.x > 0 || !state->player_flipped_x) {
            if(state->transition) {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_rightleft2_73x61;
                    state->dolphin_gfx_b = &I_black_rightleft2_73x61;
                } else {
                    state->dolphin_gfx = &I_rightleft1_73x61;
                    state->dolphin_gfx_b = &I_black_rightleft1_73x61;
                }

            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = &I_right1_73x61;
                    state->dolphin_gfx_b = &I_black_right1_73x61;
                } else if(state->player_anim == 1) {
                    state->dolphin_gfx = &I_right2_73x61;
                    state->dolphin_gfx_b = &I_black_right2_73x61;

                } else {
                    state->dolphin_gfx = &I_right3_73x61;
                    state->dolphin_gfx_b = &I_black_right3_73x61;
                }
            }
        }
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