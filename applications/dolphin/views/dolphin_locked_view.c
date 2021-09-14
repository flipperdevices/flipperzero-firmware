#include <furi.h>
#include "../dolphin_i.h"
#include "dolphin_locked_view.h"

static const Icon* idle_scenes[] = {&A_Wink_128x64, &A_WatchingTV_128x64};

void dolphin_locked_set_callback(
    DolphinLockedView* locked_view,
    DolphinLockedViewCallback callback,
    void* context) {
    furi_assert(locked_view);
    furi_assert(callback);
    locked_view->callback = callback;
    locked_view->context = context;
}

// temporary locked screen animation managment
static void
    dolphin_scene_handler_set_scene(DolphinLockedView* locked_view, const Icon* icon_data) {
    with_view_model(
        locked_view->view, (DolphinLockedViewModel * model) {
            if(model->animation) icon_animation_free(model->animation);
            model->animation = icon_animation_alloc(icon_data);
            icon_animation_start(model->animation);
            return true;
        });
}

void dolphin_locked_reset_door_pos(DolphinLockedView* locked_view) {
    with_view_model(
        locked_view->view, (DolphinLockedViewModel * model) {
            model->door_left_x = -57;
            model->door_right_x = 115;
            return true;
        });
}
void dolphin_locked_reset_counter(DolphinLockedView* locked_view) {
    locked_view->lock_count = 0;
    locked_view->lock_lastpress = 0;
    with_view_model(
        locked_view->view, (DolphinLockedViewModel * model) {
            model->hint_timeout = 0;
            return true;
        });
}

void dolphin_locked_view_render(Canvas* canvas, void* model) {
    canvas_clear(canvas);
    DolphinLockedViewModel* m = model;

    canvas_clear(canvas);

    if(m->door_left_x) {
        canvas_set_color(canvas, ColorBlack);
        canvas_draw_icon(canvas, m->door_left_x, 0, &I_DoorLeft_70x55);
        canvas_draw_icon(canvas, m->door_right_x, 0, &I_DoorRight_70x55);
    }

    m->door_left_x = CLAMP(m->door_left_x + 5, 0, -57);
    m->door_right_x = CLAMP(m->door_right_x - 5, 115, 60);

    if(m->door_left_x > -10) {
        canvas_set_font(canvas, FontPrimary);
        elements_multiline_text_framed(canvas, 42, 30, "Locked");
    }

    if(m->animation && !m->door_left_x) {
        canvas_draw_icon_animation(canvas, 0, -3, m->animation);
    }

    if(m->hint_timeout && !m->door_left_x) {
        m->hint_timeout--;
        canvas_set_font(canvas, FontSecondary);
        canvas_draw_icon(canvas, 13, 5, &I_LockPopup_100x49);
        elements_multiline_text(canvas, 65, 20, "To unlock\npress:");
    }
}

View* dolphin_locked_get_view(DolphinLockedView* locked_view) {
    furi_assert(locked_view);
    return locked_view->view;
}

bool dolphin_locked_view_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    DolphinLockedView* locked_view = context;

    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        uint32_t press_time = HAL_GetTick();

        with_view_model(
            locked_view->view, (DolphinLockedViewModel * model) {
                model->hint_timeout = HINT_TIMEOUT;
                return true;
            });
        // check if pressed sequentially
        if(press_time - locked_view->lock_lastpress > UNLOCK_RST_TIMEOUT) {
            locked_view->lock_lastpress = press_time;
            locked_view->lock_count = 0;
        } else if(press_time - locked_view->lock_lastpress < UNLOCK_RST_TIMEOUT) {
            locked_view->lock_lastpress = press_time;
            locked_view->lock_count++;
        }

        if(locked_view->lock_count == UNLOCK_CNT) {
            locked_view->lock_count = 0;
            locked_view->callback(DolphinLockedEventUnlock, locked_view->context);
        }
    }
    // All events consumed
    return true;
}

DolphinLockedView* dolphin_locked_view_alloc() {
    DolphinLockedView* locked_view = furi_alloc(sizeof(DolphinLockedView));
    locked_view->view = view_alloc();
    view_allocate_model(locked_view->view, ViewModelTypeLocking, sizeof(DolphinLockedViewModel));
    view_set_context(locked_view->view, locked_view);
    view_set_draw_callback(locked_view->view, (ViewDrawCallback)dolphin_locked_view_render);
    view_set_input_callback(locked_view->view, dolphin_locked_view_input);

    dolphin_scene_handler_set_scene(locked_view, idle_scenes[random() % COUNT_OF(idle_scenes)]);
    return locked_view;
}

void dolphin_locked_view_free(DolphinLockedView* locked_view) {
    furi_assert(locked_view);

    view_free(locked_view->view);
    free(locked_view);
}
