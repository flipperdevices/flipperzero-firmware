
#include "cmsis_os2.h"
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

typedef struct {
    const BubbleAnimation* current;
    uint8_t current_frame_idx;
    uint8_t active_cycle;
    const FrameBubble* current_bubble;

/* if not NULL - paints freeze_frame instead of 'current' animation */
    Icon* freeze_frame;
} BubbleAnimationViewModel;

struct BubbleAnimationView {
    View* view;
    osTimerId_t timer;
    BubbleAnimationInteractCallback interact_callback;
    void* interact_callback_context;
};

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

    bool is_active = model->current_frame_idx >= model->current->passive_frames;
    const FrameBubble* bubble = model->current_bubble;

    const Icon* icon = model->current->icons[model->current_frame_idx];
    canvas_draw_icon(canvas, 0, canvas_height(canvas) - icon_get_height(icon), icon);

    if (is_active && bubble) {
        uint8_t current_active_frame_idx = model->current_frame_idx - model->current->passive_frames;
        uint8_t active_frame_number = model->active_cycle * model->current->active_cycles + current_active_frame_idx;
        if ((active_frame_number >= bubble->starts_at_frame) && (active_frame_number <= bubble->ends_at_frame)) {
            const Bubble* b = &bubble->bubble;
            elements_bubble_str(canvas, b->x, b->y, b->str, b->horizontal, b->vertical);
        }
    }
}

static void bubble_animation_activate(BubbleAnimationView* bubble_animation_view) {
    furi_assert(bubble_animation_view);

    TickType_t frame_rate = 0;

    BubbleAnimationViewModel* model = view_get_model(bubble_animation_view->view);
    model->current_frame_idx = model->current->active_frames;
    model->current_bubble = model->current->frame_bubbles[random() % model->current->frame_bubbles_count];
    frame_rate = model->current->frame_rate;
    view_commit_model(bubble_animation_view->view, true);

    osTimerStart(bubble_animation_view->timer, 1000 / frame_rate);
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

static void bubble_animation_next_frame(BubbleAnimationViewModel* model) {
    furi_assert(model);

    uint8_t active_frames = model->current->active_frames;
    uint8_t passive_frames = model->current->passive_frames;
    uint8_t current_index = model->current_frame_idx;

    if (current_index >= passive_frames) {
        uint8_t current_active_frame_idx = current_index - passive_frames;
        if (current_active_frame_idx + 1 >= active_frames) {
            ++model->active_cycle;
            if (model->active_cycle >= model->current->active_cycles) {
                // switch to passive
                model->current_frame_idx = 0;
                model->active_cycle = 0;
                model->current_bubble = NULL;
            } else {
                model->current_frame_idx = passive_frames;
            }

            if (model->current_bubble) {
                uint8_t active_frame_number = model->active_cycle * model->current->active_cycles + current_active_frame_idx;
                if (model->current_bubble->ends_at_frame > active_frame_number) {
                    model->current_bubble = model->current_bubble->next_bubble;
                }
            }
        } else {
            ++model->current_frame_idx;
        }
    } else {
        model->current_frame_idx = (current_index + 1) % passive_frames;
    }

    printf("idx: %d, active_cycle: %d/%d\n", model->current_frame_idx, model->active_cycle, model->current->active_cycles);
}

static void bubble_animation_timer_callback(void* context) {
    furi_assert(context);
    BubbleAnimationView* bubble_animation_view = context;

    BubbleAnimationViewModel* model = view_get_model(bubble_animation_view->view);

    if (!model->freeze_frame && bubble_animation_view->view) {
        bubble_animation_next_frame(model);
    }

    printf("bubble_animation_timer: view: %p\n", bubble_animation_view->view);
    view_commit_model(bubble_animation_view->view, true);
}

BubbleAnimationView* bubble_animation_view_alloc(void) {
    BubbleAnimationView* bubble_animation_view = furi_alloc(sizeof(BubbleAnimationView));
    bubble_animation_view->view = view_alloc();
    bubble_animation_view->interact_callback = NULL;
    bubble_animation_view->timer = osTimerNew(bubble_animation_timer_callback, osTimerPeriodic, bubble_animation_view, NULL);

    view_allocate_model(bubble_animation_view->view, ViewModelTypeLocking, sizeof(BubbleAnimationViewModel));
    view_set_context(bubble_animation_view->view, bubble_animation_view);
    view_set_draw_callback(bubble_animation_view->view, bubble_animation_draw_callback);
    view_set_input_callback(bubble_animation_view->view, bubble_animation_input_callback);

    return bubble_animation_view;
}

void bubble_animation_view_free(BubbleAnimationView* bubble_animation_view) {
    furi_assert(bubble_animation_view);

    view_set_draw_callback(bubble_animation_view->view, NULL);
    view_set_input_callback(bubble_animation_view->view, NULL);
    view_set_context(bubble_animation_view->view, NULL);

    view_free(bubble_animation_view->view);
    bubble_animation_view->view = NULL;
    free(bubble_animation_view);
}

void bubble_animation_view_set_interact_callback(BubbleAnimationView* bubble_animation_view, BubbleAnimationInteractCallback callback, void* context) {
    furi_assert(bubble_animation_view);

    bubble_animation_view->interact_callback_context = context;
    bubble_animation_view->interact_callback = callback;
}

void bubble_animation_view_set_animation(BubbleAnimationView* bubble_animation_view, const BubbleAnimation* new_bubble_animation) {
    furi_assert(bubble_animation_view);
    furi_assert(new_bubble_animation);

    BubbleAnimationViewModel* model = view_get_model(bubble_animation_view->view);
    furi_assert(model);
    model->current = new_bubble_animation;
    /* select bubble sequence */
    model->current_bubble = new_bubble_animation->frame_bubbles[random() % new_bubble_animation->frame_bubbles_count];
    model->current_frame_idx = 0;
    model->active_cycle = 0;
    view_commit_model(bubble_animation_view->view, true);

    osTimerStart(bubble_animation_view->timer, 1000 / new_bubble_animation->frame_rate);
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

