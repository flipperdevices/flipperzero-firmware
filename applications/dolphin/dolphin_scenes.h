#pragma once

#include <math.h>

typedef struct {
    uint16_t location;
    char action_name[8];
    void (*callback)(Canvas* canvas, DolphinViewMainModel* model);
} Item;

/// Items
const Item Classic_TV = {20, "Watch", NULL};
const Item Cosy_Sofa = {70, "Sit", NULL};

const uint16_t Dolphin_POI[] = {Classic_TV.location, Cosy_Sofa.location};

#define ACTION_N 6 // no random mind control
enum Actions { SLEEP = 0, IDLE, WALK_L, WALK_R, TALK, EMOTE, MINDCONTROL };

typedef struct {
    IconName main;
    IconName back;
} DolphinIcon;

const DolphinIcon ActionAssets[ACTION_N + 1] = {
    {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    {.main = A_MDWL_32x32, .back = A_MDWLB_32x32},
    {.main = A_MDWR_32x32, .back = A_MDWRB_32x32},
    {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    {.main = A_MDI_32x32, .back = A_MDIB_32x32},
    {.main = A_MDI_32x32, .back = A_MDIB_32x32},
};

char* dialogs_list[] = {
    "Beep!\n...\nPress [ok]",
    "Wait, have\nyou seen\nP00p3r?",
    "Am I a joke\nto you?\n[ok] - No",
    "Where is remote?\nhaha, jk\n[ok] - Ok"};
char* emotes_list[] = {"(O_o)", "!?!?", "(^_^)", "(*__*)", "(@_@)", "(X_x)"};

static void dolphin_draw_dialog_bubble(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;
    uint8_t font_height = canvas_current_font_height(canvas);
    uint8_t offset = icon_get_width(m->animation);

    if(dialogs_list[m->dialog_id]) {
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(
            canvas,
            m->position + (m->position < 50 ? offset : -(offset + 15)),
            15 - font_height,
            55,
            font_height * 3 + 4);
        canvas_set_color(canvas, ColorBlack);
        elements_multiline_text(
            canvas,
            m->position + (m->position < 50 ? offset + 4 : -(offset + 15) + 4),
            15,
            dialogs_list[m->dialog_id]);
        elements_frame(
            canvas,
            m->position + (m->position < 50 ? offset : -(offset + 15)),
            15 - font_height,
            55,
            font_height * 3 + 4);
        m->previous = m->dialog_id;
    }
}

static void dolphin_draw_emote_bubble(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;
    uint8_t font_height = canvas_current_font_height(canvas);
    uint8_t id = random() % 6;
    uint8_t str_width = canvas_string_width(canvas, emotes_list[id]);
    uint8_t offset = icon_get_width(m->animation);
    if(emotes_list[id]) {
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_box(
            canvas,
            m->position + (m->position < 50 ? offset : -(offset - 15)),
            20 - font_height,
            str_width + 8,
            font_height + 4);
        canvas_set_color(canvas, ColorBlack);
        elements_multiline_text(
            canvas,
            m->position + (m->position < 50 ? offset + 4 : -(offset - 15) + 4),
            20,
            emotes_list[id]);
        elements_frame(
            canvas,
            m->position + (m->position < 50 ? offset : -(offset - 15)),
            20 - font_height,
            str_width + 8,
            font_height + 4);
        m->action = IDLE;
    }
}

void draw_dolphin(Canvas* canvas, void* m) {
    DolphinViewMainModel* model = m;

    if(model->animation && model->back) {
        canvas_set_bitmap_mode(canvas, true);
        canvas_set_color(canvas, ColorWhite);
        canvas_draw_icon(canvas, model->position, 18, model->back);
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, model->position, 18, model->animation);
        canvas_set_bitmap_mode(canvas, false);
    }
}

void set_dolphin_graphics(void* model) {
    DolphinViewMainModel* m = model;
    m->animation = assets_icons_get(ActionAssets[m->action].main);
    m->back = assets_icons_get(ActionAssets[m->action].back);
    icon_start_animation(m->back);
    icon_start_animation(m->animation);
}

void draw_scene(Canvas* canvas, void* model) {
    canvas_draw_line(canvas, 0, 42, 128, 42);
    canvas_draw_icon_name(canvas, 70, 34, I_Sofa_40x13);

    draw_dolphin(canvas, model);

    canvas_draw_icon_name(canvas, 20, 34, I_TV_20x20);
    canvas_draw_line(canvas, 26, 31, 31, 36);
    canvas_draw_line(canvas, 36, 31, 31, 36);
}

static void dolphin_update_position(void* model) {
    DolphinViewMainModel* m = model;
    if(m->position != m->poi) {
        if(m->position <= m->poi) {
            m->position += 5;
        } else {
            m->position -= 5;
        }
    }

    if(m->position == m->poi) m->action = IDLE;
}

static void dolphin_actions_update(Canvas* canvas, void* model) {
    DolphinViewMainModel* m = model;
    if(m->action != MINDCONTROL && m->action != TALK && random() % 1000 > 500) {
        m->next = random() % ACTION_N;
        if((m->action == IDLE || m->action == SLEEP)) {
            m->poi = Dolphin_POI[random() % 2];
            m->dialog_id = random() % 3;
        }
    }

    if(m->action != MINDCONTROL) set_dolphin_graphics(model);

    switch(m->action) {
    case WALK_L:
        dolphin_update_position(model);
        break;
    case WALK_R:
        dolphin_update_position(model);
        break;
    case TALK:
        if(m->locked) {
            dolphin_draw_emote_bubble(canvas, model);
        } else {
            dolphin_draw_dialog_bubble(canvas, model);
        }
        break;
    case EMOTE:
        dolphin_draw_emote_bubble(canvas, model);
        break;
    case MINDCONTROL:
        break;
    default:
        m->action = m->next;
        break;
    }
}
void dolphin_update_scene(Canvas* canvas, void* model) {
    draw_scene(canvas, model);
    dolphin_actions_update(canvas, model);
}
