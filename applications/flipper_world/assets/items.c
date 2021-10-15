#include <gui/elements.h>
#include "applications.h"
#include "items_i.h"
#include <gui/icon_i.h>

const Item Food = {
    .layer = 4,
    .timeout = 100,
    .pos =
        {
            .x = 0,
            .y = 90,
        },
    .width = 60,
    .height = 50,
    .draw = food_redraw,
    .callback = food_callback};

const Item Console = {
    .layer = 4,
    .timeout = 100,
    .pos =
        {
            .x = 357,
            .y = 190,
        },
    .width = 40,
    .height = 20,
    .draw = console_redraw,
    .callback = console_callback};

const Item* Home[] = {&Food, &Console};

const Item** Scenes[] = {Home};

const Item** get_scene(FlipperMainViewModel* state) {
    return Scenes[state->scene_id];
}

void draw_animation_sequence(Canvas* canvas, uint16_t x, uint16_t y, AnimationSequence* data) {
    uint8_t frame = ((HAL_GetTick() / 200) % data->len);
    uint16_t pos_x = x + data->rel_pos.x;
    uint16_t pos_y = y + data->rel_pos.y;

    canvas_draw_icon(canvas, pos_x + data->path[frame].x, pos_y + data->path[frame].y, data->icon);
}

uint16_t roll_new(uint16_t prev, uint16_t max) {
    uint16_t val = 999;
    while(val != prev) {
        val = random() % max;
        break;
    }
    return val;
}

static void dolphin_scene_type_text(
    Canvas* canvas,
    FlipperMainViewModel* state,
    uint8_t x,
    uint8_t y,
    const char* text) {
    char dialog_str[64];
    char buf[64];

    strcpy(dialog_str, (char*)text);

    if(state->dialog_progress <= strlen(dialog_str)) {
        if(HAL_GetTick() / 10 % 2 == 0) state->dialog_progress++;
        dialog_str[state->dialog_progress] = '\0';
        snprintf(buf, state->dialog_progress, dialog_str);
    } else {
        snprintf(buf, 64, dialog_str);
    }

    canvas_draw_str_aligned(canvas, x, y, AlignCenter, AlignCenter, buf);
}

const void flipper_world_item_callback(FlipperMainView* main_view) {
    furi_assert(main_view);

    with_view_model(
        main_view->view, (FlipperMainViewModel * model) {
            const Item* near = is_nearby(model);
            if(near && model->use_pending == true) {
                model->action_timeout = near->timeout;
                near->callback(main_view);
                model->use_pending = false;
            } else if(near) {
                near->callback(main_view);
            }
            return true;
        });
}

const Vec2 item_get_pos(FlipperMainViewModel* state, ItemsEnum item) {
    const Item** current = get_scene(state);
    Vec2 rel_pos = {0, 0};

    rel_pos.x = DOLPHIN_WIDTH / 2 + (current[item]->pos.x * PARALLAX(current[item]->layer));
    rel_pos.y = DOLPHIN_WIDTH / 4 + (current[item]->pos.y * PARALLAX(current[item]->layer));

    return rel_pos;
}

const Item* is_nearby(FlipperMainViewModel* state) {
    furi_assert(state);
    uint8_t item = 0;
    bool found = false;
    const Item** current = get_scene(state);
    while(item < ItemsEnumTotal) {
        int32_t rel_x =
            (DOLPHIN_CENTER + DOLPHIN_WIDTH / 2 -
             (current[item]->pos.x - state->player_global.x) * PARALLAX(current[item]->layer));

        uint8_t item_height = current[item]->height;
        uint8_t item_width = current[item]->width;

        int32_t rel_y = current[item]->pos.y - state->player_global.y;

        if(abs(rel_x) <= item_width && abs(rel_y) <= item_height) {
            found = !found;
            break;
        }
        ++item;
    }
    return found ? current[item] : NULL;
}

void food_redraw(Canvas* canvas, void* s) {
    furi_assert(s);
    FlipperMainViewModel* state = s;

    const char* emotes[] = {
        "Lets play!",
    };

    AnimationSequence burger = {
        .icon = &I_burger_45x37,
        .rel_pos = {10, 40},
        .len = 6,
        .path = {{0, 0}, {0, 1}, {0, 2}, {0, 3}, {0, 2}, {0, 1}},
    };

    AnimationSequence hotdog = {
        .icon = &I_hotdog_36x44,
        .rel_pos = {20, 0},
        .len = 5,
        .path = {{1, 2}, {2, 2}, {1, 1}, {0, 0}, {0, 1}},
    };

    AnimationSequence soda = {
        .icon = &I_soda23x37,
        .rel_pos = {0, 20},
        .len = 6,
        .path = {{0, 2}, {0, 1}, {0, 0}, {0, 0}, {0, 1}, {0, 2}},
    };

    AnimationSequence fish1 = {
        .icon = &I_fish1_13x32,
        .rel_pos = {11, -10},
        .len = 5,
        .path = {{0, -1}, {0, -4}, {1, 1}, {0, -4}, {1, 0}},
    };

    AnimationSequence fish2 = {
        .icon = &I_fish2_10x10,
        .rel_pos = {43, -5},
        .len = 6,
        .path = {{-5, 5}, {-3, 4}, {-2, 3}, {-4, 6}, {-2, 2}, {0, 0}},
    };

    uint16_t pos_x = (Food.pos.x - state->player_global.x) * PARALLAX(Food.layer);
    uint16_t pos_y = Food.pos.y - state->player_global.y;

    canvas_set_bitmap_mode(canvas, true);

    draw_animation_sequence(canvas, pos_x, pos_y, &hotdog);
    draw_animation_sequence(canvas, pos_x, pos_y, &soda);
    draw_animation_sequence(canvas, pos_x, pos_y, &fish1);
    draw_animation_sequence(canvas, pos_x, pos_y, &fish2);
    draw_animation_sequence(canvas, pos_x, pos_y, &burger);

    if(is_nearby(state) && (state->player_global.y > Food.pos.y)) {
        dolphin_scene_type_text(canvas, state, pos_x + 90, pos_y + 8, emotes[state->emote_id]);

    } else {
        state->dialog_progress = 0;
        state->emote_id = roll_new(state->previous_emote, SIZEOF_ARRAY(emotes));
    }
}

void food_callback(void* context) {
    furi_assert(context);
    FlipperMainView* main_view = context;

    main_view->callback(FlipperMainEventStartFoodGame, main_view->context);
}

void console_redraw(Canvas* canvas, void* s) {
    furi_assert(s);
    FlipperMainViewModel* state = s;

    const char* emotes[] = {
        "Run it, m8",
        "Lets GOOOO",
        "Click it, buddy",
        "I wanna play",
        "Wtf is this?",
        "Just do it",
        "JUST DO IT!",
    };

    const Icon* console_display[] = {
        &I_Display_0_25x18,
        &I_Display_1_25x18,
        &I_Display_2_25x18,
        &I_Display_3_25x18,
        &I_Display_4_25x18,
        &I_Display_5_25x18,
        &I_Display_6_25x18,
        &I_Display_7_25x18,
        &I_Display_8_25x18,
        &I_Display_9_25x18,

    };

    uint8_t frame = ((osKernelGetTickCount() / 100) % SIZEOF_ARRAY(console_display));

    uint16_t pos_x = (Console.pos.x - state->player_global.x) * PARALLAX(Console.layer);
    uint16_t pos_y = Console.pos.y - state->player_global.y;

    canvas_draw_icon(canvas, pos_x, pos_y, &I_Console_74x67);

    canvas_draw_icon(canvas, pos_x + 17, pos_y + 10, console_display[frame]);

    canvas_set_bitmap_mode(canvas, true);

    if(is_nearby(state)) {
        dolphin_scene_type_text(canvas, state, pos_x - 25, pos_y + 14, emotes[state->emote_id]);

    } else {
        state->dialog_progress = 0;
        state->emote_id = roll_new(state->previous_emote, SIZEOF_ARRAY(emotes));
    }
}

void console_callback(void* context) {
    furi_assert(context);
    FlipperMainView* main_view = context;
    main_view->callback(FlipperMainEventStartPassport, main_view->context);
}