#include <gui/elements.h>
#include "applications.h"
#include "items_i.h"
#include "emotes.h"
#include <gui/icon_i.h>

const Item Food = {
    .layer = 4,
    .timeout = 100,
    .pos =
        {
            .x = 0,
            .y = 30,
        },
    .icon = NULL,
    .anim = true,
    .action_name = "Use",
    .draw = draw_food,
    .callback = food_callback};

const Item PC = {
    .layer = 4,
    .timeout = 100,
    .pos =
        {
            .x = 357,
            .y = 124,
        },
    .icon = NULL,
    .anim = true,
    .action_name = "Use",
    .draw = draw_pc,
    .callback = pc_callback};

const Item* Home[ItemsEnumTotal] = {[ItemsFood] = &Food, [ItemsPC] = &PC};
const Item** Scenes[1] = {*&Home};

const Item** get_scene(SceneState* state) {
    return Scenes[state->scene_id];
}

static void dolphin_scene_start_app(SceneState* state, const FlipperApplication* flipper_app) {
    furi_assert(state);
    furi_assert(flipper_app);

    state->scene_app_thread = furi_thread_alloc();

    furi_assert(flipper_app->app);
    furi_assert(flipper_app->name);

    furi_thread_set_name(state->scene_app_thread, flipper_app->name);
    furi_thread_set_stack_size(state->scene_app_thread, flipper_app->stack_size);
    furi_thread_set_callback(state->scene_app_thread, flipper_app->app);
    furi_thread_start(state->scene_app_thread);
}

uint16_t roll_new(uint16_t prev, uint16_t max) {
    uint16_t val = 999;
    while(val != prev) {
        val = random() % max;
        break;
    }
    return val;
}

const void scene_activate_item_callback(SceneState* state, Canvas* canvas) {
    furi_assert(state);
    furi_assert(canvas);

    const Item* near = is_nearby(state);
    if(near && state->use_pending == true) {
        state->action_timeout = near->timeout;
        near->callback(canvas, state);
        state->use_pending = false;
    } else if(near) {
        near->callback(canvas, state);
    }
}

const Vec2 item_get_pos(SceneState* state, ItemsEnum item) {
    const Item** current = get_scene(state);
    Vec2 rel_pos = {0, 0};

    rel_pos.x = DOLPHIN_WIDTH / 2 + (current[item]->pos.x * PARALLAX(current[item]->layer));
    rel_pos.y = DOLPHIN_WIDTH / 4 + (current[item]->pos.y * PARALLAX(current[item]->layer));

    return rel_pos;
}

const Item* is_nearby(SceneState* state) {
    furi_assert(state);
    uint8_t item = 0;
    bool found = false;
    const Item** current = get_scene(state);
    while(item < ItemsEnumTotal) {
        int32_t rel_x =
            (DOLPHIN_CENTER + DOLPHIN_WIDTH / 2 -
             (current[item]->pos.x - state->player_global.x) * PARALLAX(current[item]->layer));

        int32_t rel_y = ((current[item]->pos.y - state->player_global.y));

        if(abs(rel_x) <= DOLPHIN_WIDTH && abs(rel_y) <= DOLPHIN_HEIGHT / 4) {
            found = !found;
            break;
        }
        ++item;
    }
    return found ? current[item] : NULL;
}

void draw_food(Canvas* canvas, void* state) {
    furi_assert(state);
    SceneState* s = state;

    const Icon* food_frames[] = {
        &I_food1_61x98,
        &I_food2_61x98,
        &I_food3_61x98,
        &I_food4_61x98,
        &I_food5_61x98,
        &I_food6_61x98,
        &I_food7_61x98,
        &I_food8_61x98,
        &I_food9_61x98,
        &I_food10_61x98,
        &I_food11_61x98,
        &I_food12_61x98,
    };

    uint8_t frame = ((HAL_GetTick() / 200) % SIZEOF_ARRAY(food_frames));

    canvas_draw_icon(
        canvas,
        (Food.pos.x - s->player_global.x) * PARALLAX(Food.layer),
        Food.pos.y - s->player_global.y,
        food_frames[frame]);

    canvas_set_bitmap_mode(canvas, true);

    // if(is_nearby(s)) {
    //     char dialog_str[64];
    //     char buf[64];

    //     strcpy(dialog_str, (char*)console_emotes[s->emote_id]);

    //     if(s->dialog_progress <= strlen(dialog_str)) {
    //         if(HAL_GetTick() / 10 % 2 == 0) s->dialog_progress++;
    //         dialog_str[s->dialog_progress] = '\0';
    //         snprintf(buf, s->dialog_progress, dialog_str);
    //     } else {
    //         snprintf(buf, 64, dialog_str);
    //     }

    //     canvas_draw_str_aligned(
    //         canvas,
    //         (PC.pos.x - s->player_global.x) * PARALLAX(PC.layer) - 25,
    //         PC.pos.y - s->player.y - 50,
    //         AlignCenter,
    //         AlignCenter,
    //         buf);

    // } else {
    //     s->dialog_progress = 0;
    //     s->emote_id = roll_new(s->previous_emote, SIZEOF_ARRAY(console_emotes));
    // }
}

void food_callback(Canvas* canvas, void* state) {
    furi_assert(state);
    SceneState* s = state;
    if(s->use_pending) {
        dolphin_scene_start_app(s, &FLIPPER_SCENE_APPS[1]);
    }
}

void draw_pc(Canvas* canvas, void* state) {
    furi_assert(state);
    SceneState* s = state;

    const Icon* console[] = {
        &I_Console_74x67_0,
        &I_Console_74x67_1,
        &I_Console_74x67_2,
        &I_Console_74x67_3,
        &I_Console_74x67_4,
        &I_Console_74x67_5,
        &I_Console_74x67_6,
        &I_Console_74x67_7,
        &I_Console_74x67_8,

    };

    uint8_t frame = ((HAL_GetTick() / 100) % SIZEOF_ARRAY(console));

    canvas_draw_icon(
        canvas,
        (PC.pos.x - s->player_global.x) * PARALLAX(PC.layer),
        PC.pos.y - s->player_global.y,
        console[frame]);

    canvas_set_bitmap_mode(canvas, true);

    // if(is_nearby(s)) {
    if(s->player_global.x > 220 && s->player_global.y > 116) {
        char dialog_str[64];
        char buf[64];

        strcpy(dialog_str, (char*)console_emotes[s->emote_id]);

        if(s->dialog_progress <= strlen(dialog_str)) {
            if(HAL_GetTick() / 10 % 2 == 0) s->dialog_progress++;
            dialog_str[s->dialog_progress] = '\0';
            snprintf(buf, s->dialog_progress, dialog_str);
        } else {
            snprintf(buf, 64, dialog_str);
        }

        canvas_draw_str_aligned(
            canvas,
            (PC.pos.x - s->player_global.x) * PARALLAX(PC.layer) - 25,
            PC.pos.y - s->player.y - 50,
            AlignCenter,
            AlignCenter,
            buf);

    } else {
        s->dialog_progress = 0;
        s->emote_id = roll_new(s->previous_emote, SIZEOF_ARRAY(console_emotes));
    }
}

void pc_callback(Canvas* canvas, void* state) {
    furi_assert(state);
    SceneState* s = state;
    if(s->use_pending) {
        dolphin_scene_start_app(s, &FLIPPER_SCENE_APPS[1]);
    }
}