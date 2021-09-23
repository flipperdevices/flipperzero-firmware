#include <furi.h>
#include <gui/gui.h>

#include "../flipper_world_i.h"
#include "flipper_world_foodgame.h"
#include "dolphin/dolphin.h"

static const Icon* letters[DISHES_TOTAL] = {&I_letterA_10x10, &I_letterB_10x10, &I_letterC_10x10};

static void
    draw_dish(Canvas* canvas, FlipperFoodGameViewModel* model, uint8_t x, uint8_t y, uint8_t id) {
    bool active = model->cursor_pos == id;
    bool opened = model->current_event == OpenLootEvent && active;

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorBlack);

    if(active) {
        canvas_draw_icon(canvas, x, y, &I_active_plate_48x18);
    }

    if(opened) {
        model->lid_pos = CLAMP(model->lid_pos + 1, LID_POS_MAX, 0);
    }

    uint8_t lid_pos = (y - 17) - (opened ? model->lid_pos : 0);

    canvas_draw_icon(canvas, x + 3, y - 6, &I_plate_42x19);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, x + 11, y - 10, loot[model->loot_list[id]].b);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, x + 11, y - 10, loot[model->loot_list[id]].f);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(canvas, x + 6, lid_pos, &I_blacklid_36x27);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(canvas, x + 6, lid_pos, &I_lid_36x27);
    canvas_set_bitmap_mode(canvas, false);

    canvas_draw_icon(canvas, x + 19, y + 8, letters[id]);
}

static void draw_dishes_scene(Canvas* canvas, FlipperFoodGameViewModel* model) {
    uint8_t tries_left = MAX_TRIES - model->try;
    for(size_t i = 0; i < MAX_TRIES; i++) {
        if(i < tries_left) {
            canvas_draw_disc(canvas, 5 + i * 8, 5, 2);
        } else {
            canvas_draw_circle(canvas, 5 + i * 8, 5, 2);
        }
    }

    for(size_t i = 0; i < DISHES_TOTAL; i++) {
        draw_dish(canvas, model, i * 40, i % 2 ? 26 : 44, i);
    }
}

static void reset_lid_pos(FlipperFoodGameViewModel* model) {
    model->selected = false;
    model->lid_pos = 0;
}

void foodgame_timer_callback(void* context) {
    FlipperFoodGameView* foodgame = context;
    foodgame->callback(FlipperFoodGameEventUpdate, foodgame->context);
}

void flipper_foodgame_reset_loot(FlipperFoodGameViewModel* model) {
    for(size_t i = 0; i < LootTotalNum; i++) {
        model->loot_list[i] = i;
    }

    for(size_t i = 0; i < LootTotalNum; i++) {
        int temp = model->loot_list[i];
        int r_idx = rand() % LootTotalNum;

        model->loot_list[i] = model->loot_list[r_idx];
        model->loot_list[r_idx] = temp;
    }
}

static bool selected_is_food(FlipperFoodGameViewModel* model) {
    return model->loot_list[model->cursor_pos] == LootFish;
}

static bool tries_exceed(FlipperFoodGameViewModel* model) {
    return model->try == MAX_TRIES;
}

static bool timeout_exceed(FlipperFoodGameViewModel* model) {
    return model->timeout == TRY_TIMEOUT;
}

void flipper_foodgame_update_state(FlipperFoodGameView* food_game) {
    bool end = false;
    with_view_model(
        food_game->view, (FlipperFoodGameViewModel * model) {
            switch(model->current_event) {
            case PlayerChoiceEvent:
                if(model->selected) {
                    model->current_event = OpenLootEvent;
                }
                break;
            case OpenLootEvent:
                model->timeout = CLAMP(model->timeout + 1, TRY_TIMEOUT, 0);
                if(timeout_exceed(model)) {
                    model->timeout = 0;
                    model->current_event = selected_is_food(model) ? WinEvent : LooseEvent;
                    model->deed = selected_is_food(model);
                }
                break;
            case LooseEvent:
                model->timeout = CLAMP(model->timeout + 1, TRY_TIMEOUT, 0);
                if(timeout_exceed(model)) {
                    model->timeout = 0;
                    model->current_event = FinishedEvent;
                }
                break;
            case WinEvent:
                if(model->deed) {
                    Dolphin* state = furi_record_open("dolphin");
                    dolphin_deed(state, DolphinDeedIButtonWrite);
                    furi_record_close("dolphin");
                }
                break;
            case FinishedEvent:
                reset_lid_pos(model);
                flipper_foodgame_reset_loot(model);
                model->try++;
                model->current_event = tries_exceed(model) ? ExitGameEvent : PlayerChoiceEvent;
                break;

            case ExitGameEvent:
                FURI_LOG_E("Tag", "EXIT");
                end = true;
                break;

            default:
                break;
            }
            return true;
        });

    if(end) {
        food_game->callback(FlipperFoodGameEventExit, food_game->context);
    }
}

void foodgame_render_callback(Canvas* canvas, FlipperFoodGameViewModel* model) {
    furi_assert(model);
    furi_assert(canvas);

    switch(model->current_event) {
    case WinEvent:
        canvas_draw_str(canvas, 30, 30, "Dolphin_happy.png");
        break;
    case LooseEvent:
        canvas_draw_str_aligned(canvas, 64, 30, AlignCenter, AlignCenter, "Try again!");
        break;
    case ExitGameEvent:
        break;
    case FinishedEvent:
        break;
    default:
        draw_dishes_scene(canvas, model);
        break;
    }
}

static void food_minigame_controls(FlipperFoodGameView* foodgame, InputEvent* event) {
    furi_assert(foodgame);
    furi_assert(event);
    with_view_model(
        foodgame->view, (FlipperFoodGameViewModel * model) {
            if(event->key == InputKeyRight) {
                if(model->current_event == PlayerChoiceEvent) {
                    model->cursor_pos = CLAMP(model->cursor_pos + 1, DISHES_TOTAL - 1, 0);
                }
            } else if(event->key == InputKeyLeft) {
                if(model->current_event == PlayerChoiceEvent) {
                    model->cursor_pos = CLAMP(model->cursor_pos - 1, DISHES_TOTAL - 1, 0);
                }
            } else if(event->key == InputKeyOk) {
                switch(model->current_event) {
                case PlayerChoiceEvent:
                    model->selected = true;
                    break;
                case WinEvent:
                    model->current_event = FinishedEvent;
                    break;
                case LooseEvent:
                    model->current_event = FinishedEvent;
                    break;
                default:
                    break;
                }
            }
            return true;
        });
}

void flipper_foodgame_set_callback(
    FlipperFoodGameView* foodgame,
    FlipperFoodGameViewCallback callback,
    void* context) {
    furi_assert(foodgame);
    furi_assert(callback);
    foodgame->callback = callback;
    foodgame->context = context;
}

View* flipper_foodgame_get_view(FlipperFoodGameView* foodgame) {
    furi_assert(foodgame);
    return foodgame->view;
}

bool flipper_foodgame_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    FlipperFoodGameView* foodgame = context;
    if(event->type != InputTypeShort) {
        return false;
    }

    if(event->key == InputKeyBack) {
        foodgame->callback(FlipperFoodGameEventExit, foodgame->context);
    } else {
        food_minigame_controls(foodgame, event);
    }

    return true;
}

FlipperFoodGameView* flipper_foodgame_alloc() {
    FlipperFoodGameView* foodgame = furi_alloc(sizeof(FlipperFoodGameView));
    foodgame->view = view_alloc();
    foodgame->timer = osTimerNew(foodgame_timer_callback, osTimerPeriodic, foodgame, NULL);

    view_allocate_model(foodgame->view, ViewModelTypeLocking, sizeof(FlipperFoodGameViewModel));
    view_set_context(foodgame->view, foodgame);
    view_set_draw_callback(foodgame->view, (ViewDrawCallback)foodgame_render_callback);
    view_set_input_callback(foodgame->view, flipper_foodgame_input);

    return foodgame;
}

void flipper_foodgame_free(FlipperFoodGameView* foodgame) {
    furi_assert(foodgame);

    view_free(foodgame->view);
    osTimerDelete(foodgame->timer);
    free(foodgame);
}
