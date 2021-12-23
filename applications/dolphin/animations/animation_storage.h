#pragma once
#include "animation_storage.pb.h"
#include <stdint.h>
#include <m-list.h>
#include "views/bubble_animation_view.h"

typedef struct StorageAnimation StorageAnimation;

LIST_DEF(StorageAnimationList, StorageAnimation*, M_PTR_OPLIST)
#define M_OPL_StorageAnimationList_t() LIST_OPLIST(StorageAnimationList)

void animation_storage_fill_animation_list(StorageAnimationList_t*);
const BubbleAnimation* animation_storage_get_bubble_animation(StorageAnimation*);
void animation_storage_free_animation(StorageAnimation* storage_animation);

StorageAnimation* animation_storage_find_animation(const char*);
PB_FA_StorageAnimationMeta* animation_storage_get_meta(StorageAnimation* storage_animation);
const BubbleAnimation* animation_storage_get_bubble_animation(StorageAnimation* storage_animation);
void animation_storage_cache_animation(StorageAnimation* storage_animation);

