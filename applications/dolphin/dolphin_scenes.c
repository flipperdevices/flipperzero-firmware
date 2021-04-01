#include "dolphin_scenes.h"
#include "dolphin_emotions.h"
#include "dolphin_items.h"
#include <gui/elements.h>

static void dolphin_use_item(Canvas* canvas, void* model) {
    furi_assert(model);
    const Item* near = is_nearby(model);
    if(near != NULL) {
        if(near->callback) near->callback(canvas, model);
        DolphinViewMetaModel* m = model;
        m->action_timeout = 20;
    }
}

void dolphin_draw_emote_bubble(Canvas* canvas, void* model, char* custom) {
    furi_assert(model);

    DolphinViewMetaModel* m = model;
    uint8_t font_y = canvas_current_font_height(canvas);

    char buf[32];
    snprintf(buf, 32, custom != NULL ? custom : emotes_list[m->emote_id]);
    uint8_t lines = 1;
    uint16_t str_width = canvas_string_width(canvas, buf);
    // count \n's
    if(buf != NULL) {
        for(int i = 0; buf[i] != '\0'; i++) {
            if(buf[i] == '\n') {
                lines++;
                uint16_t temp_width = canvas_string_width(canvas, buf + (i + 1));
                str_width = temp_width > str_width ? temp_width : str_width;
            }
        }

        uint16_t pos = 70;

        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(canvas, pos, 18 - font_y, str_width + 8, font_y * lines + 6);
        canvas_set_color(canvas, ColorBlack);
        elements_multiline_text(canvas, pos + 4, 20, buf);
        elements_frame(canvas, pos, 18 - font_y, str_width + 8, font_y * lines + 6);
        if(!custom) {
            m->previous_action = m->action;
            if(m->action_timeout == 0) {
                m->action = IDLE;
            }
        }
    }
}

static void draw_dolphin(Canvas* canvas, void* model) {
    furi_assert(model);
    DolphinViewMetaModel* m = model;

    if(m->animation && m->back) {
        canvas_set_bitmap_mode(canvas, true);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_icon(
            canvas, (m->position - m->scene_offset) * PARALLAX(DOLPHIN_LAYER), 18, m->back);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(
            canvas, (m->position - m->scene_offset) * PARALLAX(DOLPHIN_LAYER), 18, m->animation);
        canvas_set_bitmap_mode(canvas, false);
    }
}

static void draw_hint(Canvas* canvas, void* model) {
    furi_assert(model);
    DolphinViewMetaModel* m = model;
    const Item* near = is_nearby(model);
    if(near != NULL) {
        uint16_t offset = near->y < 20 ? -10 : 10;

        canvas_draw_str(
            canvas, (near->x - m->scene_offset) + offset, near->y - offset, near->action_name);
    }
}

static void draw_scene(Canvas* canvas, void* model) {
    furi_assert(model);
    DolphinViewMetaModel* m = model;

    if(m->action == MINDCONTROL && !m->use_item && m->action_timeout == 0) {
        draw_hint(canvas, model);
    }

    for(uint8_t l = 0; l < LAYERS; l++) {
        uint8_t active = m->active_scene;
        for(uint8_t i = 0; i < ITEMS_NUM; i++) {
            if(Scenes[active][i]->draw) {
                Scenes[active][i]->draw(canvas, model);
            }

            if(l == Scenes[active][i]->layer) {
                canvas_draw_icon_name(
                    canvas,
                    (Scenes[active][i]->x - m->scene_offset) * PARALLAX(l),
                    Scenes[active][i]->y,
                    Scenes[active][i]->icon);
                canvas_set_bitmap_mode(canvas, false);
            }
        }
        if(l == DOLPHIN_LAYER) draw_dolphin(canvas, model);

        if(l == 0) canvas_draw_line(canvas, 0, 42, 128, 42);
    }
}

static void dolphin_update_position(void* model) {
    furi_assert(model);
    DolphinViewMetaModel* m = model;
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

            if(m->position < 255) {
                m->position += 5;
            } else if(m->position > 255) {
                m->position = 255;
            }
        } else {
            if(m->action_timeout == 0) m->action_timeout = 15;
            if(m->action_timeout == 15) {
                m->animation = assets_icons_get(A_MDWL_32x32);
                m->back = assets_icons_get(A_MDWLB_32x32);
                icon_start_animation(m->back);
                icon_start_animation(m->animation);
            }
            if(m->position > 0) {
                m->position -= 5;
            } else if(m->position < 0) {
                m->position = 0;
            }
        }
        m->scene_offset = m->position - 50;
        break;
    }

    if(m->position == m->poi) {
        m->action = IDLE;
    }
}

static uint16_t roll_new(uint16_t prev, uint16_t max) {
    uint16_t val = 999;
    while(val != prev) {
        val = random() % max;
        break;
    }
    return val;
}

static void dolphin_actions_update(Canvas* canvas, void* model) {
    furi_assert(model);
    DolphinViewMetaModel* m = model;

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
            m->action_timeout = 50;
            m->emote_id = roll_new(m->previous_emote, ARRSIZE(emotes_list));
        }

        else if(m->previous_action != m->action) {
            dolphin_draw_emote_bubble(canvas, model, NULL);
        } else {
            m->action = IDLE;
        }

        break;
    case MINDCONTROL:
        break;

    default:
        m->animation = assets_icons_get(A_MDI_32x32);
        m->back = assets_icons_get(A_MDIB_32x32);
        icon_start_animation(m->back);
        icon_start_animation(m->animation);

        m->action_timeout = 0;
        m->previous_action = m->action;
        m->action = m->next_action;
        break;
    }
}

void dolphin_handle_keys(InputEvent* event, DolphinViewMetaModel* model) {
    furi_assert(model);
    furi_assert(event);

    if(event->type == InputTypePress) {
        if(event->key == InputKeyRight) {
            model->animation = assets_icons_get(A_MDWR_32x32);
            model->back = assets_icons_get(A_MDWRB_32x32);
            if(!icon_is_animating(model->animation)) {
                icon_start_animation(model->back);
                icon_start_animation(model->animation);
            }
            if(model->position < 255) {
                model->position += 5;
            } else if(model->position > 255) {
                model->position = 255;
            }
        } else if(event->key == InputKeyLeft) {
            model->animation = assets_icons_get(A_MDWL_32x32);
            model->back = assets_icons_get(A_MDWLB_32x32);
            if(!icon_is_animating(model->animation)) {
                icon_start_animation(model->back);
                icon_start_animation(model->animation);
            }
            if(model->position > 0) {
                model->position -= 5;
            } else if(model->position < 0) {
                model->position = 0;
            }
        }
        model->scene_offset = model->position - 50;

    } else if(event->type == InputTypeShort) {
        if(event->key == InputKeyOk) {
            if(!model->use_item) {
                model->use_item = true;
            }

        } else if(event->key == InputKeyBack) {
            model->action = IDLE;
        }
    }
}

void dolphin_update_scene(Canvas* canvas, void* model) {
    furi_assert(model);
    draw_scene(canvas, model);
    dolphin_actions_update(canvas, model);
}
