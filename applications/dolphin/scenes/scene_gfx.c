#include <furi.h>
#include "scene.h"
#include "assets/emotes.h"
#include "assets/items.h"
#include <gui/elements.h>

const char* action_str[] = {"Sleep", "Idle", "Walk", "Emote", "Use", "MC"};

static void scene_draw_hint(SceneState* state, Canvas* canvas, bool glitching) {
    furi_assert(state);
    furi_assert(canvas);
    char buf[32];

    const Item* near = is_nearby(state);
    if(near) {
        int32_t hint_pos_x = (near->pos.x - state->player_global.x) * PARALLAX(near->layer) + 25;
        int8_t hint_pos_y = (near->pos.y - state->player_global.y) - 5;

        strcpy(buf, near->action_name);

        if(glitching) {
            for(size_t g = 0; g != state->action_timeout; g++) {
                buf[(g * 23) % strlen(buf)] = ' ' + (random() % g * 17) % ('z' - ' ');
            }
        }

        canvas_draw_str(canvas, hint_pos_x, hint_pos_y, buf);
    }
}

static void scene_draw_current_emote(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);
    elements_multiline_text_framed(canvas, 80, 20, (char*)emotes_list[state->emote_id]);
}

static void scene_draw_sleep_emote(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    char dialog_str[] = "zZzZ..";
    // 2do - sofa x pos getter
    const Vec2 item_pos = item_get_pos(state, ItemsSofa);

    bool on_pos = (abs(state->player_global.x - item_pos.x) <= 1) &&
                  (abs(state->player_global.y - item_pos.y) <= 1);

    if(on_pos && state->action_timeout % 100 < 50) {
        if(state->dialog_progress < strlen(dialog_str)) {
            if(state->action_timeout % 10 == 0) state->dialog_progress++;

            dialog_str[state->dialog_progress + 1] = '\0';
            canvas_draw_str(canvas, 80, 20, dialog_str);
        }

    } else {
        state->dialog_progress = 0;
    }
}

static void scene_draw_dialog(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    char dialog_str[64];
    char buf[64];

    strcpy(dialog_str, (char*)dialogues_list[state->dialogue_id]);

    if(state->dialog_progress <= strlen(dialog_str)) {
        if(state->action_timeout % 2 == 0) state->dialog_progress++;
        dialog_str[state->dialog_progress] = '\0';
        snprintf(buf, state->dialog_progress, dialog_str);
    } else {
        snprintf(buf, 64, dialog_str);
    }

    elements_multiline_text_framed(canvas, 68, 16, buf);
}

static void draw_idle_emote(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    char dialog_str[] = "...";

    if(state->action_timeout % 100 < 50) {
        if(state->dialog_progress < strlen(dialog_str)) {
            if(state->action_timeout % 10 == 0) state->dialog_progress++;

            dialog_str[state->dialog_progress + 1] = '\0';
            canvas_draw_str(canvas, 70, 15, dialog_str);
        }

    } else {
        state->dialog_progress = 0;
    }
}

void dolphin_scene_render_dolphin(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    if(state->scene_zoom == SCENE_ZOOM) {
        state->dolphin_gfx = I_DolphinExcited_64x63;
    } else if(state->action != INTERACT) {
        //do not look below this mark
        // ------ (mark)
        // (u've been warned)
        if(state->player_v.x < 0 && state->player_v.y < 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_leftup1_73x61;
                    state->dolphin_gfx_b = I_black_leftup1_73x61;
                } else {
                    state->dolphin_gfx = I_leftup2_73x61;
                    state->dolphin_gfx_b = I_black_leftup2_73x61;
                }
            }

        } else if(state->player_v.x < 0 && state->player_v.y > 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_leftdown1_73x61;
                    state->dolphin_gfx_b = I_black_leftdown1_73x61;
                } else {
                    state->dolphin_gfx = I_leftdown2_73x61;
                    state->dolphin_gfx_b = I_black_leftdown2_73x61;
                }
            }

        } else if(state->player_v.x > 0 && state->player_v.y > 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_rightdown1_73x61;
                    state->dolphin_gfx_b = I_black_rightdown1_73x61;
                } else {
                    state->dolphin_gfx = I_rightdown2_73x61;
                    state->dolphin_gfx_b = I_black_rightdown2_73x61;
                }
            }
        } else if(state->player_v.x > 0 && state->player_v.y < 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_rightup1_73x61;
                    state->dolphin_gfx_b = I_black_rightup1_73x61;
                } else {
                    state->dolphin_gfx = I_rightup2_73x61;
                    state->dolphin_gfx_b = I_black_rightup2_73x61;
                }
            }

        } else if(state->player_v.y < 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_up1_73x61;
                    state->dolphin_gfx_b = I_black_up1_73x61;

                } else {
                    state->dolphin_gfx = I_up1_73x61;
                    state->dolphin_gfx_b = I_black_up2_73x61;
                }
            }
        } else if(state->player_v.y > 0) {
            if(state->transition) {
            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_down1_73x61;
                    state->dolphin_gfx_b = I_black_down1_73x61;
                } else {
                    state->dolphin_gfx = I_down1_73x61;
                    state->dolphin_gfx_b = I_black_down2_73x61;
                }
            }
        }

        else if(state->player_v.x < 0 || state->player_flipped) {
            if(state->transition) {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_rightleft1_73x61;
                    state->dolphin_gfx_b = I_black_rightleft1_73x61;
                } else {
                    state->dolphin_gfx = I_rightleft2_73x61;
                    state->dolphin_gfx_b = I_black_rightleft2_73x61;
                }

            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_left1_73x61;
                    state->dolphin_gfx_b = I_black_left1_73x61;
                } else if(state->player_anim == 1) {
                    state->dolphin_gfx = I_left2_73x61;
                    state->dolphin_gfx_b = I_black_left2_73x61;

                } else {
                    state->dolphin_gfx = I_left3_73x61;
                    state->dolphin_gfx_b = I_black_left3_73x61;
                }
            }
        } else if(state->player_v.x > 0) {
            if(state->transition) {
#if 0
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_upright1_33x57;
                    state->dolphin_gfx_b = I_black_upright1;
                } else {
                    state->dolphin_gfx = I_upright2_57x36;
                    state->dolphin_gfx_b = I_black_upright2;
                }
#endif
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_rightleft1_73x61;
                    state->dolphin_gfx_b = I_black_rightleft1_73x61;
                } else {
                    state->dolphin_gfx = I_rightleft2_73x61;
                    state->dolphin_gfx_b = I_black_rightleft2_73x61;
                }

            } else {
                if(state->player_anim == 0) {
                    state->dolphin_gfx = I_right1_73x61;
                    state->dolphin_gfx_b = I_black_right1_73x61;
                } else if(state->player_anim == 1) {
                    state->dolphin_gfx = I_right2_73x61;
                    state->dolphin_gfx_b = I_black_right2_73x61;

                } else {
                    state->dolphin_gfx = I_right3_73x61;
                    state->dolphin_gfx_b = I_black_right3_73x61;
                }
            }
        }

#if 0
        if(state->action == SLEEP) {
            const Vec2 item_pos = item_get_pos(state, ItemsSofa);

            if((abs(state->player_global.x - item_pos.x) <= 1) &&
               (abs(state->player_global.y - item_pos.y) <= 1)) {
                state->dolphin_gfx = A_FX_Sitting_40x27;
                state->dolphin_gfx_b = I_FX_SittingB_40x27;
            }
        }
#endif
    }

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon_name(canvas, state->player.x - 1, state->player.y - 1, state->dolphin_gfx_b);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon_name(canvas, state->player.x, state->player.y, state->dolphin_gfx);
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
            for(uint8_t i = 0; i < ITEMS_NUM; i++) {
                int32_t item_pos_X = (current_scene[i]->pos.x - state->player_global.x);
                int32_t item_pos_Y = (current_scene[i]->pos.y - state->player_global.y);
                if(item_screen_bounds(item_pos_X)) {
                    if(current_scene[i]->draw) current_scene[i]->draw(canvas, state);

                    if(l == current_scene[i]->layer) {
                        canvas_draw_icon_name(
                            canvas,
                            item_pos_X * PARALLAX(l),
                            item_pos_Y, // * PARALLAX(l),
                            current_scene[i]->icon);
                        canvas_set_bitmap_mode(canvas, false);
                    }
                }
            }

            if(l == 0) {
                canvas_draw_line(
                    canvas, 0, 30 - state->player_global.y, 128, 30 - state->player_global.y);
                canvas_draw_line(
                    canvas, 0, 102 - state->player_global.y, 128, 102 - state->player_global.y);
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

    if(state->scene_zoom == SCENE_ZOOM)
        scene_draw_dialog(state, canvas);
    else if(state->action == EMOTE)
        scene_draw_current_emote(state, canvas);
    else if(state->action == MINDCONTROL)
        scene_draw_hint(state, canvas, state->action_timeout > 45);
    else if(state->action == INTERACT)
        scene_activate_item_callback(state, canvas);
    else if(state->action == SLEEP)
        scene_draw_sleep_emote(state, canvas);
    else if(state->action == IDLE)
        draw_idle_emote(state, canvas);
}