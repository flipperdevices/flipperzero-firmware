#pragma once

#include <gui/view.h>
#include "../animation_manager.h"

typedef struct BubbleAnimationView BubbleAnimationView;
typedef void (*BubbleAnimationInteractCallback)(void*);

BubbleAnimationView* bubble_animation_view_alloc(void);
void bubble_animation_view_free(BubbleAnimationView* bubble_animation_view);

void bubble_animation_view_set_interact_callback(BubbleAnimationView* bubble_animation_view, BubbleAnimationInteractCallback callback, void* context);
void bubble_animation_view_set_animation(BubbleAnimationView* bubble_animation_view, const BubbleAnimation* new_bubble_animation);
View* bubble_animation_get_view(BubbleAnimationView* view);

/*
 * Usage:
 *  animation_view_alloc()
 *  set_animation()
 *  ...
 *  freeze_animation()
 *   // release animation
 *  ...
 *   // allocate animation
 *  set_animation()
 *  unfreeze()
 */
void bubble_animation_freeze(BubbleAnimationView* view);
void bubble_animation_unfreeze(BubbleAnimationView* view);

