#include <gui/elements.h>
#include "applications.h"
#include "items_i.h"

// const Item TV = {
//     .layer = 7,
//     .timeout = 10,
//     .pos =
//         {
//             .x = 160,
//             .y = 34,
//         },
//     .icon = &I_TV_20x24,
//     .anim = false,
//     .action_name = "Use",
//     .draw = draw_tv,
//     .callback = smash_tv};

// const Item Painting = {
//     .layer = 3,
//     .timeout = 20,
//     .pos =
//         {
//             .x = 160,
//             .y = 10,
//         },
//     .icon = &I_Home_painting_17x20,
//     .anim = false,
//     .action_name = "Inspect",
//     .draw = NULL,
//     .callback = inspect_painting};

// const Item Sofa = {
//     .layer = 4,
//     .timeout = 100,
//     .pos =
//         {
//             .x = 250,
//             .y = 34,
//         },
//     .icon = &I_Sofa_40x13,
//     .anim = false,
//     .action_name = "Sit",
//     .draw = NULL,
//     .callback = sofa_sit};

const Item PC = {
    .layer = 4,
    .timeout = 100,
    .pos =
        {
            .x = 357,
            .y = 52,
        },
    .icon = &A_Console_74x67,
    .anim = true,
    .action_name = "Use",
    .draw = NULL,
    .callback = pc_callback};

const Item* Home[ITEMS_NUM] = {
    /* [ItemsTV] = &TV, [ItemsSofa] = &Sofa, [ItemsPainting] = &Painting, */ [ItemsPC] = &PC};
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
    while(item < ITEMS_NUM) {
        int32_t rel_x =
            (DOLPHIN_CENTER + DOLPHIN_WIDTH / 2 -
             (current[item]->pos.x - state->player_global.x) * PARALLAX(current[item]->layer));

        int32_t rel_y = (DOLPHIN_HEIGHT - (current[item]->pos.y - state->player_global.y));

        if(abs(rel_x) <= DOLPHIN_WIDTH / 2 && abs(rel_y) <= DOLPHIN_HEIGHT / 2) {
            found = !found;
            break;
        }
        ++item;
    }
    return found ? current[item] : NULL;
}

// void draw_tv(Canvas* canvas, void* state) {
//     furi_assert(state);
//     SceneState* s = state;
//     canvas_set_color(canvas, ColorWhite);
//     canvas_draw_box(
//         canvas,
//         (TV.pos.x + 3 - s->player_global.x) * PARALLAX(TV.layer),
//         TV.pos.y + 4 - s->player_global.y,
//         16,
//         20);
//     canvas_set_color(canvas, ColorBlack);
//     canvas_set_bitmap_mode(canvas, true);
// }

// void smash_tv(Canvas* canvas, void* state) {
//     furi_assert(state);
//     SceneState* s = state;
//     s->player_flipped = true;
//     canvas_set_bitmap_mode(canvas, true);
//     canvas_draw_icon(
//         canvas,
//         ((TV.pos.x - 5) - s->player_global.x) * PARALLAX(TV.layer),
//         TV.pos.y - 2,
//         &I_FX_Bang_32x6);
//     canvas_set_bitmap_mode(canvas, false);
//     if(s->action_timeout < TV.timeout - 2) {
//         elements_multiline_text_framed(canvas, 80, 24, "Bang!");
//     }
// }

// void sofa_sit(Canvas* canvas, void* state) {
//     furi_assert(state);
//     SceneState* s = state;
//     // temp fix pos
//     s->player_global.x = 154;
//     s->dolphin_gfx = &A_FX_Sitting_40x27;
//     s->dolphin_gfx_b = &I_FX_SittingB_40x27;
// }

// void inspect_painting(Canvas* canvas, void* state) {
//     furi_assert(state);
//     SceneState* s = state;
//     if(s->use_pending) {
//         dolphin_scene_start_app(s, &FLIPPER_SCENE_APPS[0]);
//     }
// }

void pc_callback(Canvas* canvas, void* state) {
    furi_assert(state);
    SceneState* s = state;
    if(s->use_pending) {
        dolphin_scene_start_app(s, &FLIPPER_SCENE_APPS[1]);
    }
}