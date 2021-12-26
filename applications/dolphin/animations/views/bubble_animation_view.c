
#include "cmsis_os2.h"
#include "dolphin/animations/animation_manager.h"
#include "furi-hal-delay.h"
#include "furi-hal-resources.h"
#include "furi/check.h"
#include "furi/memmgr.h"
#include "gui/canvas.h"
#include "gui/elements.h"
#include "gui/view.h"
#include "input/input.h"
#include <furi.h>
#include "portmacro.h"
#include <gui/icon.h>
#include <stdint.h>
#include <FreeRTOS.h>
#include <timers.h>
#include "bubble_animation_view.h"
#include <gui/icon_i.h>

#define ACTIVE_SHIFT 2
#define TAG "ANM_VIEW"

typedef struct {
    const BubbleAnimation* current;
    const FrameBubble* current_bubble;
    uint8_t current_frame_idx;
    uint8_t active_cycle;
    uint8_t active_bubbles;
    uint8_t passive_bubbles;
    uint8_t active_shift;
    TickType_t active_ended_at;

/* if not NULL - paints freeze_frame instead of 'current' animation */
    Icon* freeze_frame;
} BubbleAnimationViewModel;

struct BubbleAnimationView {
    View* view;
    osTimerId_t timer;
    BubbleAnimationInteractCallback interact_callback;
    void* interact_callback_context;
};

static void bubble_animation_activate_right_now(BubbleAnimationView* view);
static void bubble_animation_activate(BubbleAnimationView* view);

static void bubble_animation_draw_callback(Canvas* canvas, void* model_) {
    furi_assert(model_);
    furi_assert(canvas);

    BubbleAnimationViewModel* model = model_;

    if (model->freeze_frame) {
        canvas_draw_icon(canvas, 0, 0, model->freeze_frame);
        return;
    }

    if (!model->current) {
        return;
    }

    const FrameBubble* bubble = model->current_bubble;

    furi_assert(model->current_frame_idx < 255);
    uint8_t icon_index = 0;
    if (model->current_frame_idx < model->current->passive_frames) {
        icon_index = model->current_frame_idx;
    } else {
        icon_index = (model->current_frame_idx - model->current->passive_frames) % model->current->active_frames + model->current->passive_frames;
    }
    const Icon* icon = model->current->icons[icon_index];
    canvas_draw_icon(canvas, 0, canvas_height(canvas) - icon_get_height(icon), icon);

    if (bubble) {
        if ((model->current_frame_idx >= bubble->starts_at_frame) && (model->current_frame_idx <= bubble->ends_at_frame)) {
            const Bubble* b = &bubble->bubble;
            elements_bubble_str(canvas, b->x, b->y, b->str, b->horizontal, b->vertical);
        }
    }
}

static FrameBubble* bubble_animation_pick_bubble(BubbleAnimationViewModel* model, bool active) {
    FrameBubble* bubble = NULL;
    uint8_t index = random() % (active ? model->active_bubbles : model->passive_bubbles);
    const BubbleAnimation* animation = model->current;

    for (int i = 0; i < animation->frame_bubbles_count; ++i) {
        if ((animation->frame_bubbles[i]->starts_at_frame < animation->passive_frames) ^ active) {
            if (!index) {
                bubble = animation->frame_bubbles[i];
            }
            --index;
        }
    }

    return bubble;
}

static bool bubble_animation_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    furi_assert(event);

    BubbleAnimationView* animation_view = context;
    bool consumed = false;

    if ((event->type == InputTypeShort) && (event->key == InputKeyRight)) {
        if (animation_view->interact_callback) {
            animation_view->interact_callback(animation_view->interact_callback_context);
        }
        consumed = true;
    }
    bubble_animation_activate(animation_view);

    return consumed;
}

static void bubble_animation_activate(BubbleAnimationView* view) {
    furi_assert(view);
    bool activate = true;
    BubbleAnimationViewModel* model = view_get_model(view->view);
    if (!model->current) {
        activate = false;
    } else if (model->current_frame_idx >= model->current->passive_frames) {
        activate = false;
    } else if ((model->active_ended_at + model->current->active_cooldown * 1000) > xTaskGetTickCount()) {
        activate = false;
    } else if (model->active_shift) {
        activate = false;
    }
    view_commit_model(view->view, false);

    if (!activate) {
        return;
    }

    if (ACTIVE_SHIFT > 0) {
        BubbleAnimationViewModel* model = view_get_model(view->view);
        model->active_shift = ACTIVE_SHIFT;
        view_commit_model(view->view, false);
    } else {
        bubble_animation_activate_right_now(view);
    }
}

static void bubble_animation_activate_right_now(BubbleAnimationView* view) {
    furi_assert(view);

    TickType_t frame_rate = 0;

    BubbleAnimationViewModel* model = view_get_model(view->view);
    model->current_frame_idx = model->current->passive_frames;
    model->current_bubble = bubble_animation_pick_bubble(model, true);
    frame_rate = model->current->frame_rate;
    view_commit_model(view->view, true);

    osTimerStart(view->timer, 1000 / frame_rate);
}


static void bubble_animation_next_frame(BubbleAnimationViewModel* model) {
    furi_assert(model);

    if (model->current_frame_idx < model->current->passive_frames) {
        model->current_frame_idx = (model->current_frame_idx + 1) % model->current->passive_frames;
    } else {
        ++model->current_frame_idx;
        model->active_cycle += !((model->current_frame_idx - model->current->passive_frames) % model->current->active_frames);
        if (model->active_cycle >= model->current->active_cycles) {
            // switch to passive
            model->active_cycle = 0;
            model->current_frame_idx = 0;
            model->current_bubble = bubble_animation_pick_bubble(model, false);
            model->active_ended_at = xTaskGetTickCount();
        }

        if (model->current_bubble) {
            if (model->current_frame_idx > model->current_bubble->ends_at_frame) {
                model->current_bubble = model->current_bubble->next_bubble;
            }
        }
    }
}

static void bubble_animation_timer_callback(void* context) {
    furi_assert(context);
    BubbleAnimationView* view = context;
    bool activate = false;

    BubbleAnimationViewModel* model = view_get_model(view->view);

    if (model->active_shift > 0) {
        activate = (--model->active_shift == 0);
    }

    if (!model->freeze_frame && !activate) {
        bubble_animation_next_frame(model);
    }

    view_commit_model(view->view, !activate);

    if (activate) {
        bubble_animation_activate_right_now(view);
    }
}

void bubble_animation_enter(void* context) {
    furi_assert(context);
    FURI_LOG_I(TAG, "enter view");
    bubble_animation_activate_right_now(context);
}

void bubble_animation_exit(void* context) {
    furi_assert(context);
    FURI_LOG_I(TAG, "exit view");
    BubbleAnimationView* view = context;
    osTimerStop(view->timer);
}

BubbleAnimationView* bubble_animation_view_alloc(void) {
    BubbleAnimationView* view = furi_alloc(sizeof(BubbleAnimationView));
    view->view = view_alloc();
    view->interact_callback = NULL;
    view->timer = osTimerNew(bubble_animation_timer_callback, osTimerPeriodic, view, NULL);

    view_allocate_model(view->view, ViewModelTypeLocking, sizeof(BubbleAnimationViewModel));
    view_set_context(view->view, view);
    view_set_draw_callback(view->view, bubble_animation_draw_callback);
    view_set_input_callback(view->view, bubble_animation_input_callback);
    view_set_enter_callback(view->view, bubble_animation_enter);
    view_set_exit_callback(view->view, bubble_animation_exit);

    return view;
}

void bubble_animation_view_free(BubbleAnimationView* view) {
    furi_assert(view);

    view_set_draw_callback(view->view, NULL);
    view_set_input_callback(view->view, NULL);
    view_set_context(view->view, NULL);

    view_free(view->view);
    view->view = NULL;
    free(view);
}

void bubble_animation_view_set_interact_callback(BubbleAnimationView* view, BubbleAnimationInteractCallback callback, void* context) {
    furi_assert(view);

    view->interact_callback_context = context;
    view->interact_callback = callback;
}

void bubble_animation_view_set_animation(BubbleAnimationView* view, const BubbleAnimation* new_animation) {
    furi_assert(view);
    furi_assert(new_animation);

    BubbleAnimationViewModel* model = view_get_model(view->view);
    furi_assert(model);
    model->current = new_animation;

    model->active_ended_at = xTaskGetTickCount() - model->current->active_cooldown;
    model->active_bubbles = 0;
    model->passive_bubbles = 0;
    for (int i = 0; i < new_animation->frame_bubbles_count; ++i) {
        if (new_animation->frame_bubbles[i]->starts_at_frame < new_animation->passive_frames) {
            ++model->passive_bubbles;
        } else {
            ++model->active_bubbles;
        }
    }

    /* select bubble sequence */
    model->current_bubble = bubble_animation_pick_bubble(model, false);
    model->current_frame_idx = 0;
    model->active_cycle = 0;
    view_commit_model(view->view, true);

    osTimerStart(view->timer, 1000 / new_animation->frame_rate);
}

static void copy_icon_first_frame(Icon** icon_dst, const Icon* icon_src) {
    furi_assert(icon_dst);
    furi_assert(!*icon_dst);
    furi_assert(icon_src);

    size_t size_of_freeze_frame = icon_src->width * icon_src->height / 8 + 1;
    *icon_dst = furi_alloc(sizeof(Icon));
    memcpy(*icon_dst, icon_src, sizeof(Icon));
    (*icon_dst)->frames = furi_alloc(size_of_freeze_frame);
    memcpy((void*)(*icon_dst)->frames[0], icon_src->frames[0], size_of_freeze_frame);
}

static void free_icon_first_frame(Icon** icon) {
    furi_assert(icon);
    furi_assert(*icon);

    free((*icon)->frames);
    free(*icon);
    *icon = NULL;
}

void bubble_animation_freeze(BubbleAnimationView* view) {
    furi_assert(view);

    BubbleAnimationViewModel* model = view_get_model(view->view);
    const BubbleAnimation* animation = model->current;
    furi_assert(!model->freeze_frame);
    furi_assert(animation->passive_frames >= 2);
    copy_icon_first_frame(&model->freeze_frame, model->current->icons[1]);
    view_commit_model(view->view, false);
}

void bubble_animation_unfreeze(BubbleAnimationView* view) {

    BubbleAnimationViewModel* model = view_get_model(view->view);
    furi_assert(!model->freeze_frame);
    free_icon_first_frame(&model->freeze_frame);
    view_commit_model(view->view, false);
}

View* bubble_animation_get_view(BubbleAnimationView* view) {
    furi_assert(view);

    return view->view;
}

