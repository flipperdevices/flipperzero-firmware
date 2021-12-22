#pragma once

#include <gui/view.h>
#include "../animation_manager.h"

typedef struct BubbleAnimationView BubbleAnimationView;
typedef void (*BubbleAnimationInteractCallback)(void*);

BubbleAnimationView* bubble_animation_view_alloc(void);
void bubble_animation_view_free(BubbleAnimationView* bubble_animation_view);

void bubble_animation_view_set_interact_callback(BubbleAnimationView* bubble_animation_view, BubbleAnimationInteractCallback callback, void* context);
void bubble_animation_view_set_animation(BubbleAnimationView* bubble_animation_view, const BubbleAnimation* new_bubble_animation);
void bubble_animation_activate(BubbleAnimationView* bubble_animation);

void bubble_animation_view_construct_model_for_view(BubbleAnimationView* bubble_animation_view, View* view);
void bubble_animation_view_destruct_model(BubbleAnimationView* bubble_animation_view);

