#include "dolphin_scenes.h"
#include "dolphin_emotions.h"
#include "dolphin_items.h"
#include <gui/elements.h>

void dolphin_use_item(Canvas* canvas, void* model) {
    const Item* near = is_nearby(model);
    if(near != NULL) {
        if(near->callback) near->callback(canvas, model);
        DolphinViewMainModel* m = model;
        m->action_timeout = 20;
    }
}

void dolphin_draw_emote_bubble(Canvas* canvas, void* model, char* custom) {
    DolphinViewMainModel* m = model;
    uint8_t font_y = canvas_current_font_height(canvas);
    uint8_t offset = icon_get_width(m->animation) - 5;

    char buf[16];
    snprintf(buf, 16, custom ? custom : emotes_list[m->emote_id]);

    // count \n's
    if(buf != NULL) {
        uint8_t lines = 1;
        uint8_t len = 1;
        for(int i = 0; buf[i] != '\0'; i++) {
            if(buf[i] == '\n') {
                len = i * 2;
                lines++;
            }
        }

        uint8_t str_width =
            canvas_string_width(canvas, custom ? custom : emotes_list[m->emote_id]) + len;

        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(
            canvas,
            m->position + (m->position < 50 ? offset : -(str_width + 5)),
            18 - font_y,
            str_width + 8,
            font_y * lines + 6);
        canvas_set_color(canvas, ColorBlack);
        elements_multiline_text(
            canvas,
            m->position + (m->position < 50 ? offset + 4 : -(str_width + 5) + 4),
            20,
            custom ? custom : (char*)emotes_list[m->emote_id]);
        elements_frame(
            canvas,
            m->position + (m->position < 50 ? offset : -(str_width + 5)),
            18 - font_y,
            str_width + 8,
            font_y * lines + 6);
        if(!custom) {
            m->previous_action = m->action;

            if(m->action_timeout == 0) {
                m->action = IDLE;
            }
        }
    }
}

void draw_dolphin(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;

    if(m->animation && m->back) {
        canvas_set_bitmap_mode(canvas, true);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_icon(canvas, m->position - m->scene_offset, 18, m->back);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, m->position - m->scene_offset, 18, m->animation);
        canvas_set_bitmap_mode(canvas, false);
    }
}

void set_dolphin_graphics(void* model) {
    DolphinViewMainModel* m = model;
    m->animation = assets_icons_get(action_gfx[m->action].main);
    m->back = assets_icons_get(action_gfx[m->action].back);

    icon_start_animation(m->back);
    icon_start_animation(m->animation);
}

void draw_scene(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;

    if(m->action == MINDCONTROL && !m->use_item) {
        const Item* near = is_nearby(model);
        if(near != NULL) {
            uint16_t y_offset = near->y < 20 ? -10 : 10;
            canvas_draw_str(
                canvas,
                near->x + (m->position < 60 ? -20 : 25) - m->scene_offset,
                near->y - y_offset,
                near->action_name);
        }
    }

    for(uint8_t l = 0; l < LAYERS; l++) {
        uint8_t active = m->active_scene;
        for(uint8_t i = 0; i < ITEMS_NUM; i++) {
            if(l == Scenes[active][i]->layer) {
                canvas_draw_icon_name(
                    canvas,
                    Scenes[active][i]->x - m->scene_offset,
                    Scenes[active][i]->y,
                    Scenes[active][i]->icon);
            }

            if(Scenes[active][i]->draw) {
                Scenes[active][i]->draw(canvas, model);
            }
        }
        if(l == DOLPHIN_LAYER) draw_dolphin(canvas, model);

        if(l == 0) canvas_draw_line(canvas, 0, 42, 128, 42);
    }
}

void dolphin_update_position(void* model) {
    DolphinViewMainModel* m = model;
    while(m->position != m->poi) {
        if(m->position <= m->poi) {
            if(m->action_timeout == 0) m->action_timeout = 15;
            if(m->action_timeout == 15) {
                m->animation = assets_icons_get(A_MDWR_32x32);
                m->back = assets_icons_get(A_MDWRB_32x32);
                icon_start_animation(m->back);
                icon_start_animation(m->animation);
            }
            m->position += 5;
            if(m->position > 60) m->scene_offset += 5;
        } else {
            if(m->action_timeout == 0) m->action_timeout = 15;
            if(m->action_timeout == 15) {
                m->animation = assets_icons_get(A_MDWL_32x32);
                m->back = assets_icons_get(A_MDWLB_32x32);
                icon_start_animation(m->back);
                icon_start_animation(m->animation);
            }
            m->position -= 5;
            if(m->position > 60) m->scene_offset -= 5;
            if(m->position < 60) m->scene_offset = 0;
        }
        break;
    }

    if(m->position == m->poi) {
        m->action = IDLE;
    }
}

static uint8_t roll_new(uint8_t prev, uint8_t max) {
    uint8_t val = -1;
    while(val != prev) {
        val = (uint8_t)random() % max;
        break;
    }
    return val;
}

void dolphin_actions_update(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;

    if(m->action_timeout > 0) {
        m->action_timeout--;
    } else {
        if(m->action != MINDCONTROL && random() % 1000 > 500) {
            m->next_action = roll_new(m->previous_action, ACTION_N);

            if(m->position == m->poi && random() % 100 > 50) {
                dolphin_use_item(canvas, model);
            }

            if(m->action != SLEEP) {
                if(m->position == m->poi || m->action_timeout == 0) {
                    m->poi = Scenes[m->active_scene][random() % ITEMS_NUM]->x;
                }
            }
        }
    }

    if(m->use_item) {
        dolphin_use_item(canvas, model);
        m->use_item = false;
    }

    switch(m->action) {
    case WALK_L:
        dolphin_update_position(model);
        break;

    case WALK_R:
        dolphin_update_position(model);
        break;

    case EMOTE:
        if(m->action_timeout == 0 && m->previous_action != EMOTE) {
            m->action_timeout = 5;
            m->emote_id = roll_new(m->previous_emote, ARRSIZE(emotes_list));
        } else {
            if(m->previous_action != m->action) {
                dolphin_draw_emote_bubble(canvas, model, NULL);
            } else {
                m->action = IDLE;
            }
        }
        break;

    case MINDCONTROL:
        break;

    default:
        m->animation = assets_icons_get(action_gfx[m->next_action].main);
        m->back = assets_icons_get(action_gfx[m->next_action].back);
        icon_start_animation(m->back);
        icon_start_animation(m->animation);

        m->action_timeout = 0;
        m->previous_action = m->action;
        m->action = m->next_action;
        break;
    }
}
void dolphin_update_scene(Canvas* canvas, void* model) {
    draw_scene(canvas, model);
    dolphin_actions_update(canvas, model);
}
