#include <furi.h>
#include <gui/gui.h>

#include "../flipper_world_i.h"

#include "../assets/items.h"
#include "../assets/meta.h"
#include "flipper_world_main.h"

void main_view_timer_callback(void* context) {
    FlipperMainView* main_view = context;
    main_view->callback(FlipperMainEventUpdate, main_view->context);
}

void flipper_world_user_input(FlipperMainView* main_view, InputEvent* input) {
    furi_assert(main_view);
    furi_assert(input);
    with_view_model(
        main_view->view, (FlipperMainViewModel * model) {
            model->last_group = model->frame_group;

            if(input->type == InputTypePress && input->key != InputKeyOk) {
                model->action = FlipperActionsMovement;
            }

            if(model->action == FlipperActionsMovement) {
                if(input->type == InputTypePress && input->key != InputKeyOk) {
                    model->frame_pending = input->key;
                }
                if(input->type == InputTypeRelease) {
                    model->action = FlipperActionsIdle;
                }
            }

            if(input->type == InputTypeShort) {
                if(input->key == InputKeyOk) {
                    model->action = FlipperActionsInteract;
                    model->use_pending = true;
                    model->action_timeout = 0;
                }
            }

            return true;
        });
}

void flipper_world_transition_handler(FlipperMainViewModel* model) {
    uint8_t total = 0;
    uint8_t speed_mod = (!!model->player_v.x || !!model->player_v.y || model->transition ||
                         model->transition_pending) ?
                            6 :
                            8;

    model->transition_pending = model->frame_group != model->frame_pending;

    if(*&dolphin_frames[model->frame_group][model->frame_type]->frames[model->frame_idx].f) {
        model->current_frame = dolphin_frames[model->frame_group][model->frame_type];
    }

    total = !model->current_frame->frames[2].f ? 2 : 3;

    if(model->transition_pending && !model->frame_idx) {
        model->transition_pending = false;
        model->transition = true;
    }

    if(model->transition) {
        model->frame_type = model->frame_pending;
        model->frame_group = model->last_group;
        model->transition = !(model->frame_idx == total - 1);
    } else {
        model->last_group = model->frame_group;
        model->frame_group = model->frame_type;
    }

    ++model->player_anim;

    if(!(model->player_anim % speed_mod)) {
        model->frame_idx = (model->frame_idx + 1) % total;
    }
}

void flipper_world_tick_handler(FlipperMainView* main_view) {
    furi_assert(main_view);
    uint8_t action = 0;
    with_view_model(
        main_view->view, (FlipperMainViewModel * model) {
            if(model->action == FlipperActionsIdle) {
                model->player_speed = MAX(model->player_speed - 2, 0);
            } else {
                model->player_speed = MIN(model->player_speed + 5, SPEED_X * 10);
            }

            if(model->frame_pending == DirRight) {
                model->player_v.y = 0;
                model->player_v.x = model->player_speed;
            } else if(model->frame_pending == DirLeft) {
                model->player_v.y = 0;
                model->player_v.x = -model->player_speed;
            } else if(model->frame_pending == DirUp) {
                model->player_v.x = 0;
                model->player_v.y = -model->player_speed;
            } else if(model->frame_pending == DirDown) {
                model->player_v.x = 0;
                model->player_v.y = model->player_speed;
            }

            model->player_global.x =
                CLAMP(model->player_global.x + (model->player_v.x / 10), WORLD_WIDTH, 0);
            model->player_global.y =
                CLAMP(model->player_global.y + (model->player_v.y / 10), WORLD_HEIGHT, 0);

            // nudge camera postition
            if(model->player_global.x > 170) {
                model->player.x = CLAMP(
                    model->player.x - model->player_v.x / 20, DOLPHIN_CENTER, -DOLPHIN_WIDTH / 2);
            } else if(model->player_global.x < 70) {
                model->player.x = CLAMP(
                    model->player.x - model->player_v.x / 20, DOLPHIN_WIDTH * 2, DOLPHIN_CENTER);
            }
            action = model->action;
            model->action = action == FlipperActionsInteract ? FlipperActionsIdle : model->action;
            flipper_world_transition_handler(model);
            return true;
        });

    if(action == FlipperActionsInteract) flipper_world_item_callback(main_view);
}

void dolphin_scene_render_dolphin(Canvas* canvas, FlipperMainViewModel* model) {
    furi_assert(model);
    furi_assert(canvas);

    canvas_set_bitmap_mode(canvas, true);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_icon(
        canvas, model->player.x, model->player.y, model->current_frame->frames[model->frame_idx].b);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_icon(
        canvas, model->player.x, model->player.y, model->current_frame->frames[model->frame_idx].f);
    canvas_set_bitmap_mode(canvas, false);
}

bool item_screen_bounds_x(int32_t pos) {
    return pos > -SCREEN_WIDTH && pos < (SCREEN_WIDTH * 2);
}
bool item_screen_bounds_y(int32_t pos) {
    return pos > -SCREEN_HEIGHT * 2 && pos < (SCREEN_HEIGHT * 2);
}

void dolphin_scene_render(Canvas* canvas, FlipperMainViewModel* model) {
    furi_assert(model);
    furi_assert(canvas);

    canvas_set_font(canvas, FontSecondary);
    canvas_set_color(canvas, ColorBlack);
    const Item** current_scene = get_scene(model);

    for(uint8_t l = 0; l < LAYERS; l++) {
        for(uint8_t i = 0; i < ItemsEnumTotal; i++) {
            int32_t item_pos_X = (current_scene[i]->pos.x - model->player_global.x);
            int32_t item_pos_Y = (current_scene[i]->pos.y - model->player_global.y);

            if(item_screen_bounds_x(item_pos_X) && item_screen_bounds_y(item_pos_Y)) {
                if(l == current_scene[i]->layer) {
                    if(current_scene[i]->draw) {
                        current_scene[i]->draw(canvas, model);
                    }
                }
            }
        }

        if(l == DOLPHIN_LAYER) dolphin_scene_render_dolphin(canvas, model);
    }

    // dolphin_scene_debug
    if(model->debug) {
        char buf[64];
        sprintf(
            buf,
            "%d:%d %d/%dP%dL%d T%d-%d",
            model->frame_idx,
            model->current_frame->frames[2].f == NULL ? 2 : 3,
            model->frame_group,
            model->frame_type,
            model->frame_pending,
            model->last_group,
            model->transition_pending,
            model->transition);
        canvas_draw_str(canvas, 0, 13, buf);
    }
}

void flipper_main_set_callback(
    FlipperMainView* main_view,
    FlipperMainViewCallback callback,
    void* context) {
    furi_assert(main_view);
    furi_assert(callback);
    main_view->callback = callback;
    main_view->context = context;
}

View* flipper_main_get_view(FlipperMainView* main_view) {
    furi_assert(main_view);
    return main_view->view;
}

bool flipper_main_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    FlipperMainView* main_view = context;

    if(event->key == InputKeyBack) {
        main_view->callback(FlipperMainEventExit, main_view->context);
    } else {
        flipper_world_user_input(main_view, event);
    }

    return true;
}

FlipperMainView* flipper_world_main_alloc() {
    FlipperMainView* main_view = furi_alloc(sizeof(FlipperMainView));
    main_view->view = view_alloc();
    main_view->timer = osTimerNew(main_view_timer_callback, osTimerPeriodic, main_view, NULL);

    view_allocate_model(main_view->view, ViewModelTypeLockFree, sizeof(FlipperMainViewModel));
    view_set_context(main_view->view, main_view);
    view_set_draw_callback(main_view->view, (ViewDrawCallback)dolphin_scene_render);
    view_set_input_callback(main_view->view, flipper_main_input);

    with_view_model(
        main_view->view, (FlipperMainViewModel * model) {
            // defaults
            model->player_global.x = 160;
            model->player_global.y = WORLD_HEIGHT;
            model->player.y = DOLPHIN_DEFAULT_Y;
            model->player.x = DOLPHIN_CENTER;
            model->action = FlipperActionsIdle;

            model->frame_group = DirRight;
            model->frame_type = DirRight;
            model->frame_pending = DirRight;
            model->last_group = DirRight;

            model->screen.x = model->player.x;
            model->screen.y = model->player.y;
            model->current_frame = &right;
            // model->debug = true;
            return true;
        });

    return main_view;
}

void flipper_world_main_free(FlipperMainView* main_view) {
    furi_assert(main_view);

    view_free(main_view->view);
    osTimerDelete(main_view->timer);
    free(main_view);
}
